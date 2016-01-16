#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#pragma region assimp
#include <cimport.h>
#include <Importer.hpp>
#include <postprocess.h>
#include <scene.h>
#pragma endregion assimp
#include "core/MeshData.h"
#include "core/Log.h"

class MeshDataLoader
{
public:
	MeshDataLoader(const std::string &fileName, float scaleFactor = 1.0f);
	virtual ~MeshDataLoader() {};

	const MeshData *getMeshData();

	//void setTexturePath(const std::string &texturePath);

private:
	//MeshLoader(const MeshLoader&);
	//MeshLoader& operator = (const MeshLoader&);
	//std::string mTexturePath;
	MeshData mMeshData;
	MeshData loadFromFile(const std::string &fileName, float scaleFactor = 1.0f);

	//std::string getFileName(std::string& pathName) const;
	//std::string setDefaultTexturePath(std::string pathName) const;

	vec3 get(const aiVector3D& v);
	quat get(const aiQuaternion& q);
	mat4 get(const aiMatrix4x4& m);
	Color get(const aiColor4D& c);
	std::string get(const aiString& s);

	std::vector<unsigned int> getIndices(const aiMesh* aimesh);
	std::vector<vec3> getVertices(const aiMesh* aimesh, float scaleFactor);
	std::vector<vec3> getNormals(const aiMesh* aimesh);
	std::vector<vec4> getTangents(const aiMesh* aimesh);
	std::vector<vec3> getBitangents(const aiMesh* aimesh);
	std::vector<vec2> getTexCoords(const aiMesh* aimesh);
};

//=========================================================================
MeshDataLoader::MeshDataLoader(const std::string &fileName, float scaleFactor)
{
	mMeshData = loadFromFile(fileName, scaleFactor);
}

//=========================================================================
const MeshData *MeshDataLoader::getMeshData()
{
	return &mMeshData;
}

//=========================================================================
MeshData MeshDataLoader::loadFromFile(const std::string &fileName, float scaleFactor)
{
	MeshData meshData;

	unsigned int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs |
	                     aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_SplitLargeMeshes | aiProcess_SortByPType | aiProcess_TransformUVCoords;

	/*
	unsigned int flags =
	aiProcess_Triangulate |
	aiProcess_FlipUVs |
	aiProcess_FixInfacingNormals |
	aiProcess_FindInstances |
	aiProcess_ValidateDataStructure |
	aiProcess_CalcTangentSpace |
	aiProcess_GenSmoothNormals |
	aiProcess_JoinIdenticalVertices |
	aiProcess_ImproveCacheLocality |
	aiProcess_LimitBoneWeights |
	aiProcess_RemoveRedundantMaterials |
	aiProcess_GenUVCoords |
	aiProcess_SortByPType |
	aiProcess_FindDegenerates |
	aiProcess_FindInvalidData |
	aiProcess_OptimizeMeshes;
	//aiProcess_OptimizeGraph |
	//aiProcess_SplitLargeMeshes
	//aiProcess_TransformUVCoords;
	*/

	auto importer = std::unique_ptr<Assimp::Importer>(new Assimp::Importer());
	importer->SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
	auto scene = importer->ReadFile(fileName.c_str(), flags);

	if(!scene)
	{
		error("Could not load mesh: %s", fileName.c_str());
	}
	else
	{
		note("Mesh loaded: %s", fileName.c_str());
		note("  Submeshes  : %i", scene->mNumMeshes);
		note("  Materials  : %i", scene->mNumMaterials);
		note("  Animations : %i", scene->mNumAnimations);

		const aiVector3D zero3D(0.0f, 0.0f, 0.0f);

		for(unsigned int m = 0; m < scene->mNumMeshes; ++m)
		{
			const aiMesh* mesh = scene->mMeshes[m];

			//note("  mesh[%i] [Geometry.Vertices]    : %i", m, mesh->mNumVertices);
			//note("  mesh[%i] [Geometry.Faces]       : %i", m, mesh->mNumFaces);

			MeshDataGeometry meshDataGeometry;

			meshDataGeometry.materialIndex = mesh->mMaterialIndex;
			meshDataGeometry.indices = getIndices(mesh);
			meshDataGeometry.vertices = getVertices(mesh, scaleFactor);
			meshDataGeometry.normals = getNormals(mesh);
			meshDataGeometry.tangents = getTangents(mesh);
			meshDataGeometry.bitangents = getBitangents(mesh);
			meshDataGeometry.texCoords = getTexCoords(mesh);

			meshData.geometryVec.push_back(meshDataGeometry);
		}

		for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
		{
			MeshDataMaterial meshDataMaterial;
			const aiMaterial* material = scene->mMaterials[i];
			aiColor4D acolor, dcolor, scolor, ecolor, tcolor;

			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, acolor))
				meshDataMaterial.ambient = get(acolor);

			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, dcolor))
				meshDataMaterial.diffuse = get(dcolor);

			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, scolor))
				meshDataMaterial.specular = get(scolor);

			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, ecolor))
				meshDataMaterial.emissive = get(ecolor);

			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_TRANSPARENT, tcolor))
				meshDataMaterial.transparent = get(tcolor);

			aiString texPath;

			for (unsigned int t = 0; t <= AI_TEXTURE_TYPE_MAX; ++t)
			{
				unsigned int index = 0;

				while (true)
				{
					if (AI_SUCCESS != aiGetMaterialTexture(material, (aiTextureType)t, index, &texPath, NULL, NULL, NULL, NULL))
					{
						break;
					}

					TextureType textureType = TextureType::Unknown;

					switch ((aiTextureType)t)
					{
					case aiTextureType_DIFFUSE:
						 textureType = TextureType::ColorMap;
						break;

					case aiTextureType_SPECULAR:
						textureType = TextureType::SpecularMap;
						break;

					case aiTextureType_AMBIENT:
						// 						textureType = TextureType::AmbientMap;
						break;

					case aiTextureType_EMISSIVE:
						textureType = TextureType::EmissiveMap;
						break;

					case aiTextureType_HEIGHT:
						textureType = TextureType::NormalMap;
						break;

					case aiTextureType_NORMALS:
						// 						textureType = TextureType::Unknown; // ???
						break;

					case aiTextureType_SHININESS:
						// 						textureType = TextureType::Glossiness;
						break;

					case aiTextureType_OPACITY:
						textureType = TextureType::OpacityMap;
						break;

					case aiTextureType_DISPLACEMENT:
						textureType = TextureType::HeightMap;
						break;

					case aiTextureType_LIGHTMAP:
						// 						textureType = TextureType::Unknown; // ??? DAE shows as AmbientColor
						break;

					case aiTextureType_REFLECTION:
						// 						textureType = TextureType::Reflection;
						break;

					default:
						//textureType = TextureType::Unknown;
						++index;
						continue;
						break;
					}

					//auto texFileName = getFileName(std::string(textureFileName.C_Str()));
					//meshTexture.fileName = texturePath + texFileName;
					//meshTexture.fileName = std::string(texPath.C_Str());
					//meshDataMaterial.textures.push_back(meshTexture);

					meshDataMaterial.textureMap[textureType] = std::string(texPath.C_Str());

					//std::string logInfoText = "  mesh[" + std::to_string(m) + "] [Texture." + TextureTypeName[meshTexture.textureType] + "]";
					//std::stringstream ss;
					//ss << std::setiosflags(std::ios_base::left) << std::setw(33) << logInfoText << ": %s";
					//logInfoText = ss.str();
					//note(logInfoText.c_str(), texPath.C_Str());

					++index;
				}
			}

			meshData.materialMap[i] = meshDataMaterial;
		}

		return meshData;
	}

	return meshData;
}

//=========================================================================
glm::vec3 MeshDataLoader::get(const aiVector3D& v)
{
	return vec3(v.x, v.y, v.z);
}

//=========================================================================
glm::quat MeshDataLoader::get(const aiQuaternion& q)
{
	return quat(q.w, q.x, q.y, q.z);
}

//=========================================================================
glm::mat4 MeshDataLoader::get(const aiMatrix4x4& m)
{
	mat4 mat;

	mat[0][0] = m.a1;
	mat[0][1] = m.b1;
	mat[0][2] = m.c1;
	mat[0][3] = m.d1;
	mat[1][0] = m.a2;
	mat[1][1] = m.b2;
	mat[1][2] = m.c2;
	mat[1][3] = m.d2;
	mat[2][0] = m.a3;
	mat[2][1] = m.b3;
	mat[2][2] = m.c3;
	mat[2][3] = m.d3;
	mat[3][0] = m.a4;
	mat[3][1] = m.b4;
	mat[3][2] = m.c4;
	mat[3][3] = m.d4;

	return mat;
}

//=========================================================================
Color MeshDataLoader::get(const aiColor4D& c)
{
	return Color(c.r, c.g, c.b, c.a);
}

//=========================================================================
std::string MeshDataLoader::get(const aiString& s)
{
	return std::string(s.data);
}

//=========================================================================
std::vector<unsigned int> MeshDataLoader::getIndices(const aiMesh* aimesh)
{
	std::vector<unsigned int> indices;

	for(unsigned int i = 0; i < aimesh->mNumFaces; ++i)
	{
		aiFace aiface = aimesh->mFaces[i];
		unsigned numIndices = aiface.mNumIndices;
		assert(numIndices <= 3);

		for(int n = 0; n < 3; ++n)
		{
			if(numIndices == 2 && n == 2)
			{
				indices.push_back(aiface.mIndices[1]);
			}
			else
			{
				indices.push_back(aiface.mIndices[n]);
			}
		}
	}

	return indices;

	/*
	for(unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
	const aiFace& face = mesh->mFaces[i];
	assert(face.mNumIndices == 3);

	meshDataGeometry.indices.push_back(face.mIndices[0]);
	meshDataGeometry.indices.push_back(face.mIndices[1]);
	meshDataGeometry.indices.push_back(face.mIndices[2]);
	}
	*/
}

//=========================================================================
std::vector<vec3> MeshDataLoader::getVertices(const aiMesh* aimesh, float scaleFactor)
{
	std::vector<vec3> vertices;

	if(aimesh->HasPositions())
	{
		for(unsigned int i = 0; i < aimesh->mNumVertices; ++i)
			vertices.push_back(get(aimesh->mVertices[i]) * scaleFactor);
	}

	return vertices;
}

//=========================================================================
std::vector<vec3> MeshDataLoader::getNormals(const aiMesh* aimesh)
{
	std::vector<vec3> normals;

	if(aimesh->HasNormals())
	{
		for(unsigned int i = 0; i < aimesh->mNumVertices; ++i)
			normals.push_back(get(aimesh->mNormals[i]));
	}

	return normals;
}

//=========================================================================
std::vector<vec4> MeshDataLoader::getTangents(const aiMesh* aimesh)
{
	std::vector<vec4> tangents;

	if(aimesh->HasTangentsAndBitangents())
	{
		for(unsigned int i = 0; i < aimesh->mNumVertices; ++i)
		{
			vec3 t = get(aimesh->mTangents[i]);
			vec3 n = get(aimesh->mNormals[i]);
			vec3 b = get(aimesh->mBitangents[i]);

			// Orthogonalize and normalize the tangent so we can use it in something
			// approximating a T, N, B inverse matrix
			vec3 ti = normalize(t - n * dot(n, t));

			// Get determinant of T,B,N 3x3 matrix by dot*cross method
			float det = (dot(cross(n, t), b));

			if(det < 0.0f)
				det = -1.0f;
			else
				det = 1.0f;

			tangents.push_back(vec4(ti.x, ti.y, ti.z, det));
		}
	}

	return tangents;
}

//=========================================================================
std::vector<vec3> MeshDataLoader::getBitangents(const aiMesh* aimesh)
{
	std::vector<vec3> bitangents;

	if(aimesh->HasTangentsAndBitangents())
	{
		for(unsigned int i = 0; i < aimesh->mNumVertices; ++i)
			bitangents.push_back(get(aimesh->mBitangents[i]));
	}

	return bitangents;
}

//=========================================================================
std::vector<vec2> MeshDataLoader::getTexCoords(const aiMesh* aimesh)
{
	std::vector<vec2> texCoords;
	const aiVector3D zero3D(0.0f, 0.0f, 0.0f);

	for(unsigned int i = 0; i < aimesh->mNumVertices; ++i)
	{
		const aiVector3D* texCoord = aimesh->HasTextureCoords(0) ? &(aimesh->mTextureCoords[0][i]) : &zero3D;
		texCoords.push_back(vec2(texCoord->x, texCoord->y));
	}

	assert(texCoords.size() > 0);

	return texCoords;
}

//=========================================================================
//std::string MeshLoader::getFileName(std::string& pathName) const
//{
//	const size_t idx = pathName.find_last_of("\\/");
//
//	if(std::string::npos != idx)
//		pathName.erase(0, idx + 1);
//
//	return pathName;
//}

//=========================================================================
//std::string MeshLoader::setDefaultTexturePath(std::string pathName) const
//{
//	const size_t idx = pathName.find_last_of("\\/");
//
//	if(std::string::npos != idx)
//		pathName.erase(idx + 1, pathName.length() - idx - 1);
//
//	return pathName;
//}

//=========================================================================
//void MeshLoader::setTexturePath(const std::string &texturePath)
//{
//	mTexturePath = texturePath;
//}
