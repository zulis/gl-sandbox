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
	if (!hasNormals() && shader.hasAttribute(ShaderConstants::VertexNormal))
		generateNormals();

	if (!hasTangents() && shader.hasAttribute(ShaderConstants::VertexTangent))
		generateTangents();

	GLuint locVertices = shader.getAttribute(ShaderConstants::VertexPosition);

	glGenVertexArrays(1, &mVaoHandle);
	glBindVertexArray(mVaoHandle);

	glGenBuffers(6, mVboHandle);

	glBindBuffer(GL_ARRAY_BUFFER, mVboHandle[0]);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * 3 * sizeof(float), &mVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(locVertices, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
	glEnableVertexAttribArray(locVertices);

	if (hasNormals() && shader.hasAttribute(ShaderConstants::VertexNormal))
	{
		GLuint locNormals = shader.getAttribute(ShaderConstants::VertexNormal);
		glBindBuffer(GL_ARRAY_BUFFER, mVboHandle[1]);
		glBufferData(GL_ARRAY_BUFFER, mNormals.size() * 3 * sizeof(float), &mNormals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(locNormals, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
		glEnableVertexAttribArray(locNormals);
	}

	if (hasTangents() && shader.hasAttribute(ShaderConstants::VertexTangent))
	{
		GLuint locTangents = shader.getAttribute(ShaderConstants::VertexTangent);
		glBindBuffer(GL_ARRAY_BUFFER, mVboHandle[2]);
		glBufferData(GL_ARRAY_BUFFER, mTangents.size() * 4 * sizeof(float), &mTangents[0], GL_STATIC_DRAW);
		glVertexAttribPointer(locTangents, 4, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
		glEnableVertexAttribArray(locTangents);
	}

	if (hasBitangents() && shader.hasAttribute(ShaderConstants::VertexBitangent))
	{
		GLuint locBitangents = shader.getAttribute(ShaderConstants::VertexBitangent);
		glBindBuffer(GL_ARRAY_BUFFER, mVboHandle[3]);
		glBufferData(GL_ARRAY_BUFFER, mBitangents.size() * 3 * sizeof(float), &mBitangents[0], GL_STATIC_DRAW);
		glVertexAttribPointer(locBitangents, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
		glEnableVertexAttribArray(locBitangents);
	}

	if (hasTexCoords() && shader.hasAttribute(ShaderConstants::VertexTexCoord))
	{
		GLuint locTexCoords = shader.getAttribute(ShaderConstants::VertexTexCoord);
		glBindBuffer(GL_ARRAY_BUFFER, mVboHandle[4]);
		glBufferData(GL_ARRAY_BUFFER, mTexCoords.size() * 2 * sizeof(float), &mTexCoords[0], GL_STATIC_DRAW);
		glVertexAttribPointer(locTexCoords, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
		glEnableVertexAttribArray(locTexCoords);
	}

	if (mIndices.size() > 0)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVboHandle[5]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);
	}

	glBindVertexArray(0);
}

//=========================================================================
void Mesh::draw(Renderer *renderer)
{
}