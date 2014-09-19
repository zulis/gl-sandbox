#pragma once

#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#pragma region assimp
#include <cimport.h>
#include <Importer.hpp>
#include <postprocess.h>
#include <scene.h>
#pragma endregion assimp
#include "Core/Geometry.h"
#include "Core/Drawable.h"
#include "Core/Transform3d.h"
#include "Core/Texture.h"

typedef std::shared_ptr<class Mesh> MeshRef;

class Mesh : public Drawable, public Transform3d
{
	public:
		static MeshRef create(const std::string& fileName);

		Mesh(const std::string& fileName);
		~Mesh();

		void draw(const CameraRef& camera);
		
		const std::vector<Material::GeometryMaterial>& getGeomMaterial();
		void setTexturePath(const std::string& texturePath);

	protected:
		virtual void setGeometry(std::vector<GeometryRef>* geometries);

private:
	std::vector<GeometryRef> mGeometries;
	void load(const std::string& fileName);
	std::string mTexturePath { std::string() };
	std::vector<Material::GeometryMaterial> mGeometriesMaterials;
	std::string getFileName(std::string& pathName);
	void parseNode(const aiNode* node);

};

//=========================================================================
MeshRef Mesh::create(const std::string& fileName)
{
	return MeshRef(new Mesh(fileName));
}

//=========================================================================
Mesh::Mesh(const std::string& fileName)
{
	mTexturePath = fileName;

	const size_t idx = mTexturePath.find_last_of("\\/");

	if(std::string::npos != idx)
		mTexturePath.erase(idx, mTexturePath.length() - idx);

	load(fileName);
}

//=========================================================================
Mesh::~Mesh()
{
}

//=========================================================================
void Mesh::load(const std::string& fileName)
{
	unsigned int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs |
	                     aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_SplitLargeMeshes | aiProcess_SortByPType | aiProcess_TransformUVCoords;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName.c_str(), flags);

	if(!scene)
	{
		printf("Could not load mesh %s\n", fileName.c_str());
	}
	else
	{
		printf("Mesh loaded %s\n", fileName.c_str());
		printf("  %i meshes\n", scene->mNumMeshes);
		printf("  %i animations\n", scene->mNumAnimations);
		printf("  %i materials\n", scene->mNumMaterials);
		printf("  %i textures\n", scene->mNumTextures);
		printf("  %i lights\n", scene->mNumLights);

		std::vector<glm::vec3> vertices;
		std::vector<unsigned int> indices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec4> tangents;
		std::vector<glm::vec3> bitangents;
		std::vector<glm::vec2> texCoords;
		const aiVector3D zero3D(0.0f, 0.0f, 0.0f);

		for(unsigned int n = 0; n < scene->mNumMeshes; n++)
		{
			const aiMesh* mesh = scene->mMeshes[n];
			printf("    %i vertices in mesh[%i]\n", mesh->mNumVertices, n);
			printf("    %i faces in mesh[%i]\n", mesh->mNumFaces, n);

			for(unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				const aiVector3D* vertice = &(mesh->mVertices[i]);
				const aiVector3D* normal = &(mesh->mNormals[i]);
				const aiVector3D* tangent = &(mesh->mTangents[i]);
				const aiVector3D* bitangent = &(mesh->mBitangents[i]);
				const aiVector3D* texCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &zero3D;

				/* put the three vectors into my glm::vec3 struct format for doing maths */
				glm::vec3 t(tangent->x, tangent->y, tangent->z);
				glm::vec3 n(normal->x, normal->y, normal->z);
				glm::vec3 b(bitangent->x, bitangent->y, bitangent->z);

				/* orthogonalize and normalize the tangent so we can use it in something
				approximating a T,N,B inverse matrix */
				glm::vec3 ti = glm::normalize(t - n * glm::dot(n, t));

				/* get determinant of T,B,N 3x3 matrix by dot*cross method */
				float det = (glm::dot(glm::cross(n, t), b));

				if(det < 0.0f)
				{
					det = -1.0f;
				}
				else
				{
					det = 1.0f;
				}

				vertices.push_back(glm::vec3(vertice->x, vertice->y, vertice->z));
				normals.push_back(glm::vec3(normal->x, normal->y, normal->z));
				tangents.push_back(glm::vec4(ti.x, ti.y, ti.z, det));
				bitangents.push_back(glm::vec3(bitangent->x, bitangent->y, bitangent->z));
				texCoords.push_back(glm::vec2(texCoord->x, texCoord->y));
			}

			for(unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				const aiFace& face = mesh->mFaces[i];
				assert(face.mNumIndices == 3);

				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}

			auto geometry = GeometryRef(new Geometry);
			geometry->setVertices(vertices);
			geometry->setIndices(indices);
			//geometry->setDrawType(Geometry::TRIANGLES);
			geometry->setNormals(normals);
			geometry->setTangents(tangents);
			geometry->setBitangents(bitangents);
			geometry->setTexCoords(texCoords);
			mGeometries.push_back(geometry);

			vertices.clear();
			indices.clear();
			normals.clear();
			tangents.clear();
			bitangents.clear();
			texCoords.clear();
		}

		for(unsigned int n = 0; n < scene->mNumMeshes; n++)
		{
			const aiMesh* mesh = scene->mMeshes[n];
			const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			aiString textureFileName;

			Material::GeometryMaterial geomMaterial;

			aiColor4D diffuse;

			if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
				geomMaterial.Kd = glm::vec3(diffuse.r, diffuse.g, diffuse.b);

			aiColor4D ambient;

			if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient))
				geomMaterial.Ka = glm::vec3(ambient.r, ambient.g, ambient.b);

			aiColor4D specular;

			if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular))
				geomMaterial.Ks = glm::vec3(specular.r, specular.g, specular.b);

			float shininess = 0.0;
			unsigned int max;
			aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, &max);
			geomMaterial.Shininess = shininess;

			printf("		mesh[%i] [Material.Ka] : %.3f %.3f %.3f\n", n, geomMaterial.Ka.r, geomMaterial.Ka.g, geomMaterial.Ka.b);
			printf("		mesh[%i] [Material.Kd] : %.3f %.3f %.3f\n", n, geomMaterial.Kd.r, geomMaterial.Kd.g, geomMaterial.Kd.b);
			printf("		mesh[%i] [Material.Ks] : %.3f %.3f %.3f\n", n, geomMaterial.Ks.r, geomMaterial.Ks.g, geomMaterial.Ks.b);
			printf("		mesh[%i] [Material.Shininess] : %.3f\n", n, geomMaterial.Shininess);

			for(unsigned int m = 0; m <= AI_TEXTURE_TYPE_MAX; m++)
			{
				unsigned int index = 0;

				while(true)
				{
					if(AI_SUCCESS != aiGetMaterialTexture(material, (aiTextureType)m, index, &textureFileName, NULL, NULL, NULL, NULL))
					{
						break;
					}

					TextureType texType;

					switch((aiTextureType)m)
					{
						case aiTextureType_DIFFUSE:
							texType = TextureType::DiffuseColor;
							break;

						case aiTextureType_SPECULAR:
							texType = TextureType::SpecularColor;
							break;

						case aiTextureType_AMBIENT:
							texType = TextureType::AmbientColor;
							break;

						case aiTextureType_EMISSIVE:
							texType = TextureType::SelfIllumination;
							break;

						case aiTextureType_HEIGHT:
							texType = TextureType::Bump;
							break;

						case aiTextureType_NORMALS:
							texType = TextureType::Unknown; // ???
							break;

						case aiTextureType_SHININESS:
							texType = TextureType::Glossiness;
							break;

						case aiTextureType_OPACITY:
							texType = TextureType::Opacity;
							break;

						case aiTextureType_DISPLACEMENT:
							texType = TextureType::Displacement;
							break;

						case aiTextureType_LIGHTMAP:
							texType = TextureType::Unknown; // ??? DAE shows as AmbientColor
							break;

						case aiTextureType_REFLECTION:
							texType = TextureType::Reflection;
							break;

						default:
							texType = TextureType::Unknown;
							break;
					}

					printf("		mesh[%i] [%s] : %s\n", n, TextureTypeName[texType].c_str(), textureFileName.C_Str());

					Material::GeometryTexture geomTexture;
					geomTexture.textureType = texType;
					geomTexture.fileName = getFileName(std::string(textureFileName.C_Str()));

					geomMaterial.geometryIndex = n;
					geomMaterial.textures.push_back(geomTexture);

					++index;
				}
			}

			mGeometriesMaterials.push_back(geomMaterial);
		}

		for(unsigned int n = 0; n < scene->mNumLights; n++)
		{
			const aiLight* light = scene->mLights[n];
			int a;
			float radius;

			aiMatrix4x4 trf;
			aiMatrix4x4::Translation(light->mPosition, trf);

			switch(light->mType)
			{
				case aiLightSourceType::aiLightSource_DIRECTIONAL:
					a = 0;
					break;
				case aiLightSourceType::aiLightSource_POINT:
					radius = -light->mAttenuationLinear / light->mAttenuationQuadratic;
					a = 1;
					break;
				case aiLightSourceType::aiLightSource_SPOT:
					a = 3;
					break;
			}
			
			const aiVector3D* position = &(light->mPosition);
			auto direction = light->mDirection;
			const aiColor3D* color = &(light->mColorAmbient);
			int i = 0;
		}

		//parseNode(scene->mRootNode);
	}
}

//=========================================================================
void Mesh::parseNode(const aiNode* node)
{
	/*for (auto n = 0; n < node->mMeshes; n++)
	{

	}*/

	aiVector3D scaling, position;
	aiQuaternion rotation;

	aiMatrix4x4 m = node->mTransformation;
	//m.Transpose();
	m.Decompose(scaling, rotation, position);

	for (unsigned int n = 0; n < node->mNumChildren; n++)
	{
		parseNode(node->mChildren[n]);
	}
}

//=========================================================================
std::string Mesh::getFileName(std::string& pathName)
{
	const size_t idx = pathName.find_last_of("\\/");

	if(std::string::npos != idx)
		pathName.erase(0, idx + 1);

	return pathName;
}

//=========================================================================
void Mesh::setGeometry(std::vector<GeometryRef>* geometries)
{
	for(auto& geometry : mGeometries)
	{
		geometries->push_back(geometry);
	}
}

//=========================================================================
void Mesh::draw(const CameraRef& camera)
{
	Drawable::draw(*camera.get(), getMatrix());
}

//=========================================================================
void Mesh::setTexturePath(const std::string& texturePath)
{
	mTexturePath = texturePath;
}

//=========================================================================
const std::vector<Material::GeometryMaterial>& Mesh::getGeomMaterial()
{
	Texture::Format format;
	format.setFlipped(true);

	for(auto& gm : mGeometriesMaterials)
	{
		for(auto& t : gm.textures)
		{
			t.fileName = mTexturePath + "/" + t.fileName;
			t.texture = Texture::create(t.fileName, format);
		}
	}

	return mGeometriesMaterials;
}