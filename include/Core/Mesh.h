#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "Core/MeshLoader.h"
#include "Core/Geometry.h"
#include "Core/Transform3d.h"
#include "Core/Material.h"
#include "Core/Texture.h"
#include "Core/AABB.h"

typedef std::shared_ptr<class Mesh> MeshRef;

class Mesh : public Transform3D
{
	public:
		static MeshRef create(const std::string& fileName, float scaleFactor = 1.0f);
		Mesh(const std::string& fileName, float scaleFactor = 1.0f);
		~Mesh();

		void draw(const CameraRef& camera);
		
		//const std::vector<Material::GeometryMaterial>& getGeomMaterial();
		void setTexturePath(const std::string& texturePath);

		AABB getAABB() const;
		void setFrustumCulling(bool isOn);
		void setMaterial(const MaterialRef& material);

private:
	MeshLoaderRef mMeshLoader;
	std::vector<GeometryRef> mGeometries;
	void load(const std::string& fileName, float scaleFactor);
	std::string mTexturePath { std::string() };
	//std::vector<Material::GeometryMaterial> mGeometriesMaterials;
	AABB mAABB;
	std::map<GeometryRef, AABB> mAABBMap;
	bool mCullingIsOn{ false };
	MaterialRef mMaterial;
	//std::string getFileName(std::string& pathName);
	//void parseNode(const aiNode* node);

	void draw(const Camera& camera, const glm::mat4& model);
	void draw(const GeometryRef& geometry, const unsigned int geometryIndex, const Camera& camera, const glm::mat4& model);

};

//=========================================================================
MeshRef Mesh::create(const std::string& fileName, float scaleFactor)
{
	return MeshRef(new Mesh(fileName, scaleFactor));
}

//=========================================================================
Mesh::Mesh(const std::string& fileName, float scaleFactor)
{
	mTexturePath = fileName;

	const size_t idx = mTexturePath.find_last_of("\\/");

	if(std::string::npos != idx)
		mTexturePath.erase(idx, mTexturePath.length() - idx);

	load(fileName, scaleFactor);
}

//=========================================================================
Mesh::~Mesh()
{
}

//=========================================================================
void Mesh::load(const std::string& fileName, float scaleFactor)
{
	mMeshLoader = MeshLoader::create();
	MeshData meshData = mMeshLoader->loadFromFile(fileName, scaleFactor);

	auto minFloat = std::numeric_limits<float>::min();
	auto maxFloat = std::numeric_limits<float>::max();
	glm::vec3 min(maxFloat);
	glm::vec3 max(minFloat);

	for each (MeshPart meshPart in meshData)
	{
		auto geometry = GeometryRef(new Geometry);
		geometry->setDrawType(Geometry::TRIANGLES);
		geometry->setVertices(meshPart.geometry.vertices);
		geometry->setIndices(meshPart.geometry.indices);
		geometry->setNormals(meshPart.geometry.normals);
		geometry->setTangents(meshPart.geometry.tangents);
		geometry->setBitangents(meshPart.geometry.bitangents);
		geometry->setTexCoords(meshPart.geometry.texCoords);
		mGeometries.push_back(geometry);

		AABB aabb = geometry->getAABB();
		mAABBMap[geometry] = aabb;
		
		if (aabb.getMax().x > max.x)
			max.x = aabb.getMax().x;

		if (aabb.getMin().x < min.x)
			min.x = aabb.getMin().x;

		if (aabb.getMax().y > max.y)
			max.y = aabb.getMax().y;

		if (aabb.getMin().y < min.y)
			min.y = aabb.getMin().y;

		if (aabb.getMax().z > max.z)
			max.z = aabb.getMax().z;

		if (aabb.getMin().z < min.z)
			min.z = aabb.getMin().z;
	}

	mAABB = AABB(min, max);
}

//=========================================================================
void Mesh::draw(const CameraRef& camera)
{
	draw(*camera.get(), getMatrix());
}

//=========================================================================
void Mesh::setTexturePath(const std::string& texturePath)
{
	mTexturePath = texturePath;
}

//=========================================================================
// const std::vector<Material::GeometryMaterial>& Mesh::getGeomMaterial()
// {
// 	Texture::Format format;
// 	format.setFlipped(true);
// 
// 	for(auto& gm : mGeometriesMaterials)
// 	{
// 		for(auto& t : gm.textures)
// 		{
// 			t.fileName = mTexturePath + "/" + t.fileName;
// 			t.texture = Texture::create(t.fileName, format);
// 		}
// 	}
// 
// 	return mGeometriesMaterials;
// }

//=========================================================================
AABB Mesh::getAABB() const
{
	return mAABB;
}

//=========================================================================
void Mesh::setFrustumCulling(bool isOn)
{
	mCullingIsOn = isOn;
}

//=========================================================================
void Mesh::setMaterial(const MaterialRef& material)
{
	mMaterial.reset();
	mMaterial = material;
}

//=========================================================================
void Mesh::draw(const Camera& camera, const glm::mat4& model)
{
	unsigned int geometryIndex = 0;

	for each (auto geometry in mGeometries)
	{
		if (mCullingIsOn)
		{
			auto aabb = mAABBMap[geometry].transformed(model);
			auto notCulled = camera.intersects(aabb);

			if (notCulled)
			{
				draw(geometry, geometryIndex, camera, model);
			}
		}
		else
		{
			draw(geometry, geometryIndex, camera, model);
		}

		geometryIndex++;
	}
}

//=========================================================================
void Mesh::draw(const GeometryRef& geometry, const unsigned int geometryIndex, const Camera& camera, const glm::mat4& model)
{
	mMaterial->bind();
	mMaterial->setShaderValues(camera.getProjectionMatrix(), camera.getViewMatrix(), model);
	mMaterial->updateUniforms(geometryIndex);
	geometry->draw(*mMaterial->getShader());
	mMaterial->unbind();
}

//=========================================================================
//void Mesh::draw(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
//{
//	mMaterial->bind();
//
//	setShaderValues(projection, view, model);
//
//	unsigned int geometryIndex = 0;
//
//	for each (auto geometry in mGeometries)
//	{
//		mMaterial->updateUniforms(geometryIndex);
//		geometry->draw(*mMaterial->getShader());
//		geometryIndex++;
//	}
//
//	mMaterial->unbind();
//}
