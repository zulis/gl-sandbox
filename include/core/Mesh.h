#pragma once

#include <string>
#include <vector>
#include <map>
#include "core/MeshDataLoader.h"
#include "core/AABB.h"
#include "core/Geometry.h"
#include "core/Renderer.h"

class Mesh
{
public:
	Mesh();
	~Mesh();

	void loadFromFile(const std::string& fileName);
	void makeDrawable(const Renderer& renderer, const ShaderID shader);
	void draw();
	AABB getAABB();

private:
	std::vector<Geometry*> mGeometyVec;
	std::map<unsigned int, MeshDataMaterial> mMaterialMap;
	AABB mAABB;

private:


};

//=========================================================================
Mesh::Mesh()
{
}

//=========================================================================
Mesh::~Mesh()
{
	for(auto& geometry : mGeometyVec)
	{
		delete geometry;
	}
}

void Mesh::loadFromFile(const std::string& fileName)
{
	MeshDataLoader loader(fileName);

	for(auto& mdg : loader.getMeshData()->geometryVec)
	{
		auto geometry = new Geometry();
		geometry->setVertices(mdg.vertices);
		geometry->setIndices(mdg.indices);
		geometry->setNormals(mdg.normals);
		geometry->setTexCoords(mdg.texCoords);
		geometry->setTangents(mdg.tangents);
		geometry->setBitangents(mdg.bitangents);
		
		AABB geometryAABB = geometry->getAABB();
		mAABB.setMin(min(mAABB.getMin(), geometryAABB.getMin()));
		mAABB.setMax(max(mAABB.getMax(), geometryAABB.getMax()));
		
		mGeometyVec.push_back(geometry);
	}

	mMaterialMap = loader.getMeshData()->materialMap;
}

//=========================================================================
void Mesh::makeDrawable(const Renderer& renderer, const ShaderID shader)
{
	for(auto& geometry : mGeometyVec)
	{
		geometry->prepare(renderer.getShader(shader));
	}
}

//=========================================================================
void Mesh::draw()
{
	for(auto& geometry : mGeometyVec)
	{
		geometry->draw();
	}
}

//=========================================================================
AABB Mesh::getAABB()
{
	return mAABB;
}
