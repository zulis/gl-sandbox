#pragma once

#include <memory>
#include <string>
#pragma region assimp
#include <cimport.h>
#include <Importer.hpp>
#include <postprocess.h>
#include <scene.h>
#pragma endregion assimp
#include "Core/MeshData.h"
#include "Core/TextureType.h"

typedef std::shared_ptr<class MeshLoader> MeshLoaderRef;

class MeshLoader
{
public:
	static MeshLoaderRef create();
	MeshLoader();
	virtual ~MeshLoader();

	MeshData loadFromFile(const std::string& fileName, float scaleFactor = 1.0f) const;

private:
	MeshLoader(const MeshLoader&);
	MeshLoader& operator = (const MeshLoader&);

	std::string getFileName(std::string& pathName) const;
};

//=========================================================================
MeshLoaderRef MeshLoader::create()
{
	return MeshLoaderRef(new MeshLoader);
}

//=========================================================================
MeshLoader::MeshLoader()
{
}

//=========================================================================
MeshLoader::~MeshLoader()
{
}

//=========================================================================
MeshData MeshLoader::loadFromFile(const std::string& fileName, float scaleFactor) const
{
	MeshData result;

	unsigned int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs |
		aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_SplitLargeMeshes | aiProcess_SortByPType | aiProcess_TransformUVCoords;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName.c_str(), flags);

	if (!scene)
	{
		printf("Could not load mesh %s\n", fileName.c_str());
	}
	else
	{
		// Doesn't work?
		if (scaleFactor != 1.0f)
		{
			aiMatrix4x4 scaling;
			aiMatrix4x4::Scaling(aiVector3D(scaleFactor), scaling);
			const_cast<aiMatrix4x4&>(scene->mRootNode->mTransformation) = scaling * scene->mRootNode->mTransformation;
		}

		printf("Mesh loaded %s\n", fileName.c_str());
		printf("  %i meshes\n", scene->mNumMeshes);
		printf("  %i animations\n", scene->mNumAnimations);
		printf("  %i materials\n", scene->mNumMaterials);
		printf("  %i textures\n", scene->mNumTextures);
		printf("  %i lights\n", scene->mNumLights);

		const aiVector3D zero3D(0.0f, 0.0f, 0.0f);

		for (unsigned int n = 0; n < scene->mNumMeshes; n++)
		{
			const aiMesh* mesh = scene->mMeshes[n];
			const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			printf("    %i vertices in mesh[%i]\n", mesh->mNumVertices, n);
			printf("    %i faces in mesh[%i]\n", mesh->mNumFaces, n);

			MeshPart meshPart;

#pragma region geometry
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				const aiVector3D* vertice = &(mesh->mVertices[i]);
				const aiVector3D* normal = &(mesh->mNormals[i]);
				const aiVector3D* tangent = &(mesh->mTangents[i]);
				const aiVector3D* bitangent = &(mesh->mBitangents[i]);
				const aiVector3D* texCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &zero3D;

				// put the three vectors into my glm::vec3 struct format for doing maths
				glm::vec3 t(tangent->x, tangent->y, tangent->z);
				glm::vec3 n(normal->x, normal->y, normal->z);
				glm::vec3 b(bitangent->x, bitangent->y, bitangent->z);

				// orthogonalize and normalize the tangent so we can use it in something
				// approximating a T,N,B inverse matrix 
				glm::vec3 ti = glm::normalize(t - n * glm::dot(n, t));

				// get determinant of T,B,N 3x3 matrix by dot*cross method
				float det = (glm::dot(glm::cross(n, t), b));

				if (det < 0.0f)
				{
					det = -1.0f;
				}
				else
				{
					det = 1.0f;
				}


				meshPart.geometry.vertices.push_back(glm::vec3(vertice->x, vertice->y, vertice->z));
				meshPart.geometry.normals.push_back(glm::vec3(normal->x, normal->y, normal->z));
				meshPart.geometry.tangents.push_back(glm::vec4(ti.x, ti.y, ti.z, det));
				meshPart.geometry.bitangents.push_back(glm::vec3(bitangent->x, bitangent->y, bitangent->z));
				meshPart.geometry.texCoords.push_back(glm::vec2(texCoord->x, texCoord->y));
			}

			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				const aiFace& face = mesh->mFaces[i];
				assert(face.mNumIndices == 3);

				meshPart.geometry.indices.push_back(face.mIndices[0]);
				meshPart.geometry.indices.push_back(face.mIndices[1]);
				meshPart.geometry.indices.push_back(face.mIndices[2]);
			}
#pragma endregion geometry

#pragma region materials
			aiString textureFileName;
			aiColor4D diffuse;

			if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
				meshPart.material.Kd = glm::vec3(diffuse.r, diffuse.g, diffuse.b);

			aiColor4D ambient;

			if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient))
				meshPart.material.Ka = glm::vec3(ambient.r, ambient.g, ambient.b);

			aiColor4D specular;

			if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular))
				meshPart.material.Ks = glm::vec3(specular.r, specular.g, specular.b);

			float shininess = 0.0;
			unsigned int max;
			aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, &max);
			meshPart.material.Shininess = shininess;

			printf("		mesh[%i] [Material.Ka] : %.3f %.3f %.3f\n", n, meshPart.material.Ka.r, meshPart.material.Ka.g, meshPart.material.Ka.b);
			printf("		mesh[%i] [Material.Kd] : %.3f %.3f %.3f\n", n, meshPart.material.Kd.r, meshPart.material.Kd.g, meshPart.material.Kd.b);
			printf("		mesh[%i] [Material.Ks] : %.3f %.3f %.3f\n", n, meshPart.material.Ks.r, meshPart.material.Ks.g, meshPart.material.Ks.b);
			printf("		mesh[%i] [Material.Shininess] : %.3f\n", n, meshPart.material.Shininess);

			for (unsigned int m = 0; m <= AI_TEXTURE_TYPE_MAX; m++)
			{
				unsigned int index = 0;
				MeshTexture meshTexture;

				while (true)
				{
					if (AI_SUCCESS != aiGetMaterialTexture(material, (aiTextureType)m, index, &textureFileName, NULL, NULL, NULL, NULL))
					{
						break;
					}

					switch ((aiTextureType)m)
					{
					case aiTextureType_DIFFUSE:
						meshTexture.type = TextureType::DiffuseColor;
						break;

					case aiTextureType_SPECULAR:
						meshTexture.type = TextureType::SpecularColor;
						break;

					case aiTextureType_AMBIENT:
						meshTexture.type = TextureType::AmbientColor;
						break;

					case aiTextureType_EMISSIVE:
						meshTexture.type = TextureType::SelfIllumination;
						break;

					case aiTextureType_HEIGHT:
						meshTexture.type = TextureType::Bump;
						break;

					case aiTextureType_NORMALS:
						meshTexture.type = TextureType::Unknown; // ???
						break;

					case aiTextureType_SHININESS:
						meshTexture.type = TextureType::Glossiness;
						break;

					case aiTextureType_OPACITY:
						meshTexture.type = TextureType::Opacity;
						break;

					case aiTextureType_DISPLACEMENT:
						meshTexture.type = TextureType::Displacement;
						break;

					case aiTextureType_LIGHTMAP:
						meshTexture.type = TextureType::Unknown; // ??? DAE shows as AmbientColor
						break;

					case aiTextureType_REFLECTION:
						meshTexture.type = TextureType::Reflection;
						break;

					default:
						meshTexture.type = TextureType::Unknown;
						break;
					}

					printf("		mesh[%i] [%s] : %s\n", n, TextureTypeName[meshTexture.type].c_str(), textureFileName.C_Str());

					meshTexture.fileName = getFileName(std::string(textureFileName.C_Str()));
					meshPart.material.textures.push_back(meshTexture);

					++index;
				}
			}

#pragma endregion materials

			result.push_back(meshPart);
		}

		return result;
	}
}

//=========================================================================
std::string MeshLoader::getFileName(std::string& pathName) const
{
	const size_t idx = pathName.find_last_of("\\/");

	if (std::string::npos != idx)
		pathName.erase(0, idx + 1);

	return pathName;
}