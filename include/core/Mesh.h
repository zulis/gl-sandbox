#pragma once

#include <string>
#include <vector>
#include <map>
#include "core/MeshDataLoader.h"
#include "core/Geometry.h"
#include "core/Renderer.h"

class Mesh
{
public:
	Mesh();
	~Mesh();

	void loadFromFile(const std::string& fileName);
	void makeDrawable(Renderer* renderer, const ShaderID shader);
	void draw(Renderer* renderer);

private:
	std::vector<Geometry*> mGeometyVec;
	std::map<unsigned int, MeshDataMaterial> mMaterialMap;

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
	auto loader = new MeshDataLoader(fileName);

	for(auto& mdg : loader->getMeshData()->geometryVec)
	{
		auto geometry = new Geometry();
		geometry->setVertices(mdg.vertices);
		geometry->setIndices(mdg.indices);
		geometry->setNormals(mdg.normals);
		geometry->setTexCoords(mdg.texCoords);
		geometry->setTangents(mdg.tangents);
		geometry->setBitangents(mdg.bitangents);
		mGeometyVec.push_back(geometry);
	}

	mMaterialMap = loader->getMeshData()->materialMap;

	delete loader;
}

//=========================================================================
void Mesh::makeDrawable(Renderer* renderer, const ShaderID shader)
{
	for(auto& geometry : mGeometyVec)
	{
		geometry->prepare(renderer->getShader(shader));
	}
}

//=========================================================================
void Mesh::draw(Renderer* renderer)
{
	for(auto& geometry : mGeometyVec)
	{
		geometry->draw();
	}
}