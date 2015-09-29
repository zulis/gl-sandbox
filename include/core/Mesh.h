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

	void loadFromFile(const std::string &fileName);
	void makeDrawable(Renderer *renderer, const ShaderID shader);
	void draw(Renderer *renderer);

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
	for each (Geometry* geometry in mGeometyVec)
	{
		delete geometry;
	}
}

void Mesh::loadFromFile(const std::string &fileName)
{
	MeshDataLoader *loader = new MeshDataLoader(fileName);

	for each (MeshDataGeometry mdg in loader->getMeshData()->geometryVec)
	{
		Geometry *geometry = new Geometry();
		geometry->setIndices(mdg.indices);
		geometry->setVertices(mdg.vertices);
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
void Mesh::makeDrawable(Renderer *renderer, const ShaderID shader)
{
	for each (Geometry* geometry in mGeometyVec)
	{
		geometry->prepare(*renderer->getShader(shader));
	}
}

//=========================================================================
void Mesh::draw(Renderer *renderer)
{
	for each (Geometry* geometry in mGeometyVec)
	{
		geometry->draw();
	}
}