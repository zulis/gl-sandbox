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
#include "Core/MaterialDefault.h"

typedef std::shared_ptr<class Mesh> MeshRef;

class Mesh : public Transform3D
{
	public:
		static MeshRef create();
		Mesh();
		virtual ~Mesh();

		void loadFromFile(const std::string& fileName, float scaleFactor = 1.0f);
		void draw(const CameraRef& camera);
		
		//const std::vector<Material::GeometryMaterial>& getGeomMaterial();
		void setTexturePath(const std::string& texturePath);

		AABB getAABB() const;
		void setFrustumCulling(bool isOn);
		void setMaterial(const MaterialRef& material);
		const MaterialRef& getMaterial() const;

private:
	MeshLoaderRef mMeshLoader;
	std::vector<GeometryRef> mGeometries;
	std::string mTexturePath { std::string() };
	//std::vector<Material::GeometryMaterial> mGeometriesMaterials;
	AABB mAABB;
	std::map<GeometryRef, AABB> mAABBMap;
	bool mCullingIsOn{ false };
	MaterialRef mMaterial;
	//std::string getFileName(std::string& pathName);
	//void parseNode(const aiNode* node);
	MeshData mMeshData;

	void updateUniforms(const Camera& camera);
	void draw(const Camera& camera);
	void draw(const GeometryRef& geometry, const unsigned int index, const Camera& camera);
	void setupMaterial();

};

//=========================================================================
MeshRef Mesh::create()
{
	return MeshRef(new Mesh);
}

//=========================================================================
Mesh::Mesh()
{
}

//=========================================================================
Mesh::~Mesh()
{
}

//=========================================================================
void Mesh::loadFromFile(const std::string& fileName, float scaleFactor)
{
/*
// 	if (mTexturePath == std::string())
// 	{
// 		mTexturePath = fileName;
// 
// 		const size_t idx = mTexturePath.find_last_of("\\/");
// 
// 		if (std::string::npos != idx)
// 			mTexturePath.erase(idx, mTexturePath.length() - idx);
// 	}
*/

	mMeshLoader = MeshLoader::create();
	mMeshLoader->setTexturePath(mTexturePath);
	mMeshData = mMeshLoader->loadFromFile(fileName, scaleFactor);

	mMaterial = MaterialDefault::create();
	setupMaterial();

	auto minFloat = std::numeric_limits<float>::min();
	auto maxFloat = std::numeric_limits<float>::max();
	glm::vec3 min(maxFloat);
	glm::vec3 max(minFloat);

	for each (MeshPart meshPart in mMeshData)
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
	draw(*camera.get());
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
	setupMaterial();
}

//=========================================================================
const MaterialRef& Mesh::getMaterial() const
{
	return mMaterial;
}

//=========================================================================
void Mesh::updateUniforms(const Camera& camera)
{
	const Shader* shader = mMaterial->getShader();

	if (shader->hasUniform(ShaderConstants::ProjectionMatrix))
		shader->setUniform(ShaderConstants::ProjectionMatrix, camera.getProjectionMatrix());

	if (shader->hasUniform(ShaderConstants::ViewMatrix))
		shader->setUniform(ShaderConstants::ViewMatrix, camera.getViewMatrix());

	if (shader->hasUniform(ShaderConstants::ModelMatrix))
		shader->setUniform(ShaderConstants::ModelMatrix, getMatrix());

	if (shader->hasUniform(ShaderConstants::ModelViewMatrix))
		shader->setUniform(ShaderConstants::ModelViewMatrix, camera.getViewMatrix() * getMatrix());

	if (shader->hasUniform(ShaderConstants::MVP))
		shader->setUniform(ShaderConstants::MVP, camera.getProjectionMatrix() * camera.getViewMatrix() * getMatrix());

	if (shader->hasUniform(ShaderConstants::NormalMatrix))
	{
		auto mv = camera.getViewMatrix() * getMatrix();
		shader->setUniform(ShaderConstants::NormalMatrix, glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
	}
}

//=========================================================================
void Mesh::draw(const Camera& camera)
{
	unsigned int index = 0;

	for each (auto geometry in mGeometries)
	{
		if (mCullingIsOn)
		{
			auto aabb = mAABBMap[geometry].transformed(getMatrix());
			auto notCulled = camera.intersects(aabb);

			if (notCulled)
			{
				draw(geometry, index, camera);
			}
		}
		else
		{
			draw(geometry, index, camera);
		}

		index++;
	}
}

//=========================================================================
void Mesh::draw(const GeometryRef& geometry, const unsigned int index, const Camera& camera)
{
// 	Material::ShaderValues shaderValues;
// 	shaderValues.projection = camera.getProjectionMatrix();
// 	shaderValues.view = camera.getViewMatrix();
// 	shaderValues.model = model;
// 	shaderValues.meshMaterial = mMeshData[index].material;

	mMaterial->bind();
	updateUniforms(camera);
	mMaterial->updateUniforms(index);
	//mMaterial->updateUniforms(index);
	geometry->draw(*mMaterial->getShader());
	mMaterial->unbind();
}

//=========================================================================
void Mesh::setupMaterial()
{
	if (mMaterial)
	{
		unsigned int geometryIndex = 0;

		for each (MeshPart meshPart in mMeshData)
		{
			for each (auto meshTexture in meshPart.material.textures)
			{
				mMaterial->addTexture(meshTexture.fileName, meshTexture.textureType, geometryIndex);
			}

			geometryIndex++;
		}
	}
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
