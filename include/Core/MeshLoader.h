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
#include "Core/Log.h"

typedef std::shared_ptr<class MeshLoader> MeshLoaderRef;

class MeshLoader
{
public:
	static MeshLoaderRef create();
	MeshLoader();
	virtual ~MeshLoader();

	void setTexturePath(const std::string& texturePath);
	MeshData loadFromFile(const std::string& fileName, float scaleFactor = 1.0f) const;

private:
	MeshLoader(const MeshLoader&);
	MeshLoader& operator = (const MeshLoader&);
	std::string mTexturePath;


	std::string getFileName(std::string& pathName) const;
	std::string setDefaultTexturePath(std::string pathName) const;
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
		logError("Could not load mesh: %s", fileName.c_str());
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

		logNote("Mesh loaded: %s", fileName.c_str());
		logNote("  Submeshes  : %i", scene->mNumMeshes);
		logNote("  Animations : %i", scene->mNumAnimations);
		logNote("  Materials  : %i", scene->mNumMaterials);
		logNote("  Textures   : %i", scene->mNumTextures);

		const aiVector3D zero3D(0.0f, 0.0f, 0.0f);

		for (unsigned int n = 0; n < scene->mNumMeshes; n++)
		{
			const aiMesh* mesh = scene->mMeshes[n];
			const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			logNote("  mesh[%i] [Geometry.Vertices]    : %i", n, mesh->mNumVertices);
			logNote("  mesh[%i] [Geometry.Faces]       : %i", n, mesh->mNumFaces);

			MeshPart meshPart;

#pragma region geometry
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				const aiVector3D* vertice = &(mesh->mVertices[i]);
				const aiVector3D* normal = &(mesh->mNormals[i]);
				const aiVector3D* tangent = &(mesh->mTangents[i]);
				const aiVector3D* bitangent = &(mesh->mBitangents[i]);
				const aiVector3D* texCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &zero3D;

				/*
				// another example http://www.keithlantz.net/2011/10/tangent-space-normal-mapping-with-glsl/
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
				meshPart.geometry.tangents.push_back(glm::vec4(ti.x, ti.y, ti.z, det));
				*/

				meshPart.geometry.vertices.push_back(glm::vec3(vertice->x, vertice->y, vertice->z));
				meshPart.geometry.normals.push_back(glm::vec3(normal->x, normal->y, normal->z));
				meshPart.geometry.tangents.push_back(glm::vec3(tangent->x, tangent->y, tangent->z));
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
			std::string texturePath = mTexturePath;

			if (mTexturePath == std::string())
				texturePath = setDefaultTexturePath(fileName);

			aiString textureFileName;

			aiColor4D ambientColor;
			if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambientColor))
				meshPart.material.ambient = Color(ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a);

			aiColor4D diffuseColor;
			if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor))
				meshPart.material.diffuse = Color(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);

			aiColor4D specularColor;
			if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specularColor))
				meshPart.material.specular = Color(specularColor.r, specularColor.g, specularColor.b, specularColor.a);

			/*aiColor4D emissiveColor;
			if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &emissiveColor))
			meshPart.material.emissive = Color(emissiveColor.r, emissiveColor.g, emissiveColor.b, emissiveColor.a);

			aiColor4D transparentColor;
			if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_TRANSPARENT, &transparentColor))
			meshPart.material.transparent = Color(transparentColor.r, transparentColor.g, transparentColor.b, transparentColor.a);*/

			float shininess = 0.0;
			unsigned int max;
			aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, &max);
			meshPart.material.shininess = shininess;

			float shininessStrength = 0.0;
			aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS_STRENGTH, &shininessStrength, &max);
			//meshPart.material.shininess = shininess;

			logNote("  mesh[%i] [Material.ambient]     : %.3f %.3f %.3f %.3f", n, meshPart.material.ambient.r, meshPart.material.ambient.g, meshPart.material.ambient.b, meshPart.material.ambient.a);
			logNote("  mesh[%i] [Material.diffuse]     : %.3f %.3f %.3f %.3f", n, meshPart.material.diffuse.r, meshPart.material.diffuse.g, meshPart.material.diffuse.b, meshPart.material.diffuse.a);
			logNote("  mesh[%i] [Material.specular]    : %.3f %.3f %.3f %.3f", n, meshPart.material.specular.r, meshPart.material.specular.g, meshPart.material.specular.b, meshPart.material.specular.a);
			//logNote("  mesh[%i] [Material.Emissive]    : %.3f %.3f %.3f %.3f", n, meshPart.material.emissive.r, meshPart.material.emissive.g, meshPart.material.emissive.b, meshPart.material.emissive.a);
			//logNote("  mesh[%i] [Material.Transparent] : %.3f %.3f %.3f %.3f", n, meshPart.material.transparent.r, meshPart.material.transparent.g, meshPart.material.transparent.b, meshPart.material.transparent.a);
			logNote("  mesh[%i] [Material.shininess]   : %.3f", n, meshPart.material.shininess);

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
						meshTexture.textureType = TextureType::DiffuseMap;
						break;

 					case aiTextureType_SPECULAR:
 						meshTexture.textureType = TextureType::SpecularMap;
 						break;

// 					case aiTextureType_AMBIENT:
// 						meshTexture.type = TextureType::AmbientMap;
// 						break;

// 					case aiTextureType_EMISSIVE:
// 						meshTexture.type = TextureType::SelfIllumination;
// 						break;

					case aiTextureType_HEIGHT:
						meshTexture.textureType = TextureType::NormalMap;
						break;

// 					case aiTextureType_NORMALS:
// 						meshTexture.type = TextureType::Unknown; // ???
// 						break;
// 
// 					case aiTextureType_SHININESS:
// 						meshTexture.type = TextureType::Glossiness;
// 						break;

					case aiTextureType_OPACITY:
						meshTexture.textureType = TextureType::OpacityMap;
						break;

// 					case aiTextureType_DISPLACEMENT:
// 						meshTexture.type = TextureType::Displacement;
// 						break;
// 
// 					case aiTextureType_LIGHTMAP:
// 						meshTexture.type = TextureType::Unknown; // ??? DAE shows as AmbientColor
// 						break;
// 
// 					case aiTextureType_REFLECTION:
// 						meshTexture.type = TextureType::Reflection;
// 						break;

					default:
						//meshTexture.type = TextureType::Unknown;
						++index;
						continue;
						break;
					}

					auto texFileName = getFileName(std::string(textureFileName.C_Str()));
					meshTexture.fileName = texturePath + "/" + texFileName;
					meshPart.material.textures.push_back(meshTexture);

					logNote("  mesh[%i] [Texture.%s] : %s", n, TextureTypeName[meshTexture.textureType].c_str(), texFileName.c_str());

					++index;
				}
			}

#pragma endregion materials

			result.push_back(meshPart);
		}

		return result;
	}

	return result;
}

//=========================================================================
std::string MeshLoader::getFileName(std::string& pathName) const
{
	const size_t idx = pathName.find_last_of("\\/");

	if (std::string::npos != idx)
		pathName.erase(0, idx + 1);

	return pathName;
}

//=========================================================================
std::string MeshLoader::setDefaultTexturePath(std::string pathName) const
{
	const size_t idx = pathName.find_last_of("\\/");

	if (std::string::npos != idx)
		pathName.erase(idx + 1, pathName.length() - idx - 1);

	return pathName;
}

//=========================================================================
void MeshLoader::setTexturePath(const std::string& texturePath)
{
	mTexturePath = texturePath;
}
