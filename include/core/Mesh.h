#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "core/MeshLoader.h"
#include "core/Geometry.h"
#include "core/Transform3d.h"
#include "core/Material.h"
#include "core/Texture.h"
#include "core/AABB.h"
#include "core/Camera.h"

typedef std::unique_ptr<class Mesh> MeshPtr;

class Mesh final : public Transform3D
{
public:
	static MeshPtr create();
	Mesh() {};
	~Mesh() {};

	void loadFromFile(const std::string& fileName, float scaleFactor = 1.0f);
	void draw(const CameraPtr& camera, const Shader& shader);

	void setTexturePath(const std::string& texturePath);
	void setAutoLoadTextures(bool value);

	AABB getAABB() const;
	void setFrustumCulling(bool isOn);

	void updateUniforms(const Camera& camera, const Shader& shader);

	MeshData getMeshData() const;
	std::string getFileName() const;

private:
	MeshLoaderRef mMeshLoader;
	std::vector<GeometryRef> mGeometries;
	std::string mTexturePath;
	AABB mAABB;
	std::unordered_map<int, AABB> mAABBMap;
	bool mCullingIsOn { false };
	//std::string getFileName(std::string& pathName);
	//void parseNode(const aiNode* node);
	MeshData mMeshData;
	bool mAutoloadTextures{ true };
	std::string mFileName;

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	void draw(const Camera& camera, const Shader& shader);
	void draw(const GeometryRef& geometry, const unsigned int geometryIndex, const Camera& camera, const Shader& shader);

};

//=========================================================================
MeshPtr Mesh::create()
{
	//return MeshRef(new Mesh);
	return std::make_unique<Mesh>();
}

//=========================================================================
void Mesh::loadFromFile(const std::string& fileName, float scaleFactor)
{
	mFileName = fileName;
	mMeshLoader = MeshLoader::create();
	mMeshLoader->setTexturePath(mTexturePath);
	mMeshData = mMeshLoader->loadFromFile(fileName, scaleFactor);

	auto minFloat = std::numeric_limits<float>::min();
	auto maxFloat = std::numeric_limits<float>::max();
	glm::vec3 min(maxFloat);
	glm::vec3 max(minFloat);

	unsigned int geometryIndex = 0;

	for(auto& meshPart : mMeshData)
	{
		auto geometry = Geometry::create();
		geometry->setDrawType(Geometry::TRIANGLES);
		geometry->setVertices(meshPart.geometry.vertices);
		geometry->setIndices(meshPart.geometry.indices);
		geometry->setNormals(meshPart.geometry.normals);
		geometry->setTangents(meshPart.geometry.tangents);
		geometry->setBitangents(meshPart.geometry.bitangents);
		geometry->setTexCoords(meshPart.geometry.texCoords);
		AABB aabb = geometry->getAABB();
		mGeometries.push_back(std::move(geometry));

		mAABBMap[geometryIndex] = aabb;

		if(aabb.getMax().x > max.x)
			max.x = aabb.getMax().x;

		if(aabb.getMin().x < min.x)
			min.x = aabb.getMin().x;

		if(aabb.getMax().y > max.y)
			max.y = aabb.getMax().y;

		if(aabb.getMin().y < min.y)
			min.y = aabb.getMin().y;

		if(aabb.getMax().z > max.z)
			max.z = aabb.getMax().z;

		if(aabb.getMin().z < min.z)
			min.z = aabb.getMin().z;

		geometryIndex++;
	}

	mAABB = AABB(min, max);
}

//=========================================================================
void Mesh::draw(const CameraPtr& camera, const Shader& shader)
{
	draw(*camera.get(), shader);
}

//=========================================================================
void Mesh::setTexturePath(const std::string& texturePath)
{
	mTexturePath = texturePath;
}

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
void Mesh::updateUniforms(const Camera& camera, const Shader& shader)
{
	shader.setUniform(ShaderConstants::ProjectionMatrix, camera.getProjectionMatrix());
	shader.setUniform(ShaderConstants::ViewMatrix, camera.getViewMatrix());
	shader.setUniform(ShaderConstants::ModelMatrix, getMatrix());
	shader.setUniform(ShaderConstants::ModelViewMatrix, camera.getViewMatrix() * getMatrix());
	shader.setUniform(ShaderConstants::MVP, camera.getProjectionMatrix() * camera.getViewMatrix() * getMatrix());

	auto mv = camera.getViewMatrix() * getMatrix();
	shader.setUniform(ShaderConstants::NormalMatrix, glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
}

//=========================================================================
void Mesh::draw(const Camera& camera, const Shader& shader)
{
	unsigned int geometryIndex = 0;

	for(auto& geometry : mGeometries)
	{
		if(mCullingIsOn)
		{
			auto aabb = mAABBMap[geometryIndex].transformed(getMatrix());
			auto notCulled = camera.intersects(aabb);

			if(notCulled)
			{
				draw(geometry, geometryIndex, camera, shader);
			}
		}
		else
		{
			draw(geometry, geometryIndex, camera, shader);
		}

		geometryIndex++;
	}
}

//=========================================================================
void Mesh::draw(const GeometryRef& geometry, const unsigned int geometryIndex, const Camera& camera, const Shader& shader)
{
	updateUniforms(camera, shader);
	geometry->draw(shader);
}

//=========================================================================
void Mesh::setAutoLoadTextures(bool value)
{
	mAutoloadTextures = value;
}

//=========================================================================
MeshData Mesh::getMeshData() const
{
	return mMeshData;
}

//=========================================================================
std::string Mesh::getFileName() const
{
	return mFileName;
}
