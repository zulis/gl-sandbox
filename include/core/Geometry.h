#pragma once

#include <vector>
#include <limits>
#include <functional>
#include "core/Math.h"
#include "core/Shader.h"
#include "core/AABB.h"
#include "core/ShaderConstants.h"

class Geometry
{
public:
	enum DrawType
	{
		TRIANGLES = 0x0004,     // GL_TRIANGLES
		TRIANGLESTRIP = 0x0005, // GL_TRIANGLE_STRIP
		TRIANGLEFAN = 0x0006,   // GL_TRIANGLE_FAN
		QUADS = 0x0007,         // GL_QUADS
		POINTS = 0x0000,        // GL_POINTS
		LINE = 0x0001,          // GL_LINE
		LINELOOP = 0x0002,      // GL_LINE_LOOP
		POLYGON = 0x0009        // GL_POLYGON
	};

	Geometry() {};
	Geometry(std::vector<vec3> vertices, std::vector<unsigned int> indices);
	~Geometry();

	void setVertices(const std::vector<vec3>& vertices);
	void setVertices(const std::vector<vec2>& vertices);
	void setIndices(const std::vector<unsigned int>& indices);
	void setTexCoords(const std::vector<vec2>& texCoords);
	void setNormals(const std::vector<vec3>& normals);
	void setTangents(const std::vector<vec4>& tangents);
	void setBitangents(const std::vector<vec3>& bitangents);
	void setDrawType(DrawType drawType);

	const std::vector<vec3> getVertices() const;
	const std::vector<unsigned int> getIndices() const;
	const std::vector<vec2> getTexCoords() const;
	const std::vector<vec3> getNormals() const;
	const std::vector<vec4> getTangents() const;
	const std::vector<vec3> getBitangents() const;
	unsigned int getDrawType() const;
	const AABB getAABB() const;

	bool hasIndices() const;
	bool hasTexCoords() const;
	bool hasNormals() const;
	bool hasTangents() const;
	bool hasBitangents() const;

	void prepare(const Shader& shader);
	void draw() const;

private:
	DrawType mDrawType { DrawType::TRIANGLES };
	std::vector<vec3> mVertices;
	std::vector<unsigned int> mIndices;
	std::vector<vec2> mTexCoords;
	std::vector<vec3> mNormals;
	std::vector<vec4> mTangents;
	std::vector<vec3> mBitangents;
	GLuint mVaoHandle;
	GLuint mVboHandle[6];

	Geometry(const Geometry&) = delete;
	Geometry& operator=(const Geometry&) = delete;
	
	void generateNormals();
	void generateTangents();
};

//=========================================================================
Geometry::Geometry(std::vector<vec3> vertices, std::vector<unsigned int> indices)
{
	setVertices(vertices);
	setIndices(indices);
}

//=========================================================================
Geometry::~Geometry()
{
	mVertices.clear();
	mIndices.clear();
	mTexCoords.clear();
	mNormals.clear();
	mTangents.clear();
	mBitangents.clear();

	glBindVertexArray(0);

	for (auto& hande : mVboHandle)
		glDeleteBuffers(1, &hande);

	glDeleteVertexArrays(1, &mVaoHandle);
}

//=========================================================================
void Geometry::setVertices(const std::vector<vec3>& vertices)
{
	mVertices = vertices;
}

//=========================================================================
void Geometry::setVertices(const std::vector<vec2>& vertices)
{
	std::vector<vec3> result;

	for (auto& v : vertices)
	{
		result.push_back(vec3(v.x, v.y, 0.0f));
	}

	mVertices = result;
}

//=========================================================================
void Geometry::setIndices(const std::vector<unsigned int>& indices)
{
	mIndices = indices;
}

//=========================================================================
void Geometry::setTexCoords(const std::vector<vec2>& texCoords)
{
	mTexCoords = texCoords;
}

//=========================================================================
void Geometry::setNormals(const std::vector<vec3>& normals)
{
	mNormals = normals;
}

//=========================================================================
void Geometry::setTangents(const std::vector<vec4>& tangents)
{
	mTangents = tangents;
}

//=========================================================================
void Geometry::setBitangents(const std::vector<vec3>& bitangents)
{
	mBitangents = bitangents;
}

//=========================================================================
void Geometry::setDrawType(DrawType drawType)
{
	mDrawType = drawType;
}

//=========================================================================
const std::vector<vec3> Geometry::getVertices() const
{
	return mVertices;
}

//=========================================================================
const std::vector<unsigned int> Geometry::getIndices() const
{
	return mIndices;
}

//=========================================================================
const std::vector<vec2> Geometry::getTexCoords() const
{
	return mTexCoords;
}

//=========================================================================
const std::vector<vec3> Geometry::getNormals() const
{
	return mNormals;
}

//=========================================================================
const std::vector<vec4> Geometry::getTangents() const
{
	return mTangents;
}

//=========================================================================
const std::vector<vec3> Geometry::getBitangents() const
{
	return mBitangents;
}

//=========================================================================
unsigned int Geometry::getDrawType() const
{
	return mDrawType;
}

//=========================================================================
bool Geometry::hasIndices() const
{
	return mIndices.size() > 0;
}

//=========================================================================
bool Geometry::hasTexCoords() const
{
	return mTexCoords.size() > 0;
}

//=========================================================================
bool Geometry::hasNormals() const
{
	return mNormals.size() > 0;
}

//=========================================================================
bool Geometry::hasTangents() const
{
	return mTangents.size() > 0;
}

//=========================================================================
bool Geometry::hasBitangents() const
{
	return mBitangents.size() > 0;
}

//=========================================================================
void Geometry::prepare(const Shader& shader)
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
void Geometry::draw() const
{
	glBindVertexArray(mVaoHandle);

	if (mIndices.size() > 0)
	{
		glDrawElements(mDrawType, mIndices.size() * sizeof(unsigned int), GL_UNSIGNED_INT, ((GLubyte*)NULL + (0)));
	}
	else
	{
		glDrawArrays(mDrawType, 0, mVertices.size());
	}

	glBindVertexArray(0);
}

//=========================================================================
const AABB Geometry::getAABB() const
{
	auto minFloat = std::numeric_limits<float>::min();
	auto maxFloat = std::numeric_limits<float>::max();
	vec3 min(maxFloat);
	vec3 max(minFloat);

	for (unsigned int i = 0; i < mVertices.size(); i++)
	{
		auto tmp = mVertices[i];

		if (tmp.x > max.x)
			max.x = tmp.x;

		if (tmp.x < min.x)
			min.x = tmp.x;

		if (tmp.y > max.y)
			max.y = tmp.y;

		if (tmp.y < min.y)
			min.y = tmp.y;

		if (tmp.z > max.z)
			max.z = tmp.z;

		if (tmp.z < min.z)
			min.z = tmp.z;
	}

	return AABB(min, max);
}

//=========================================================================
void Geometry::generateNormals()
{
	mNormals.assign(mVertices.size(), vec3(0.0f));

	unsigned int n = mIndices.size() / 3;

	for (unsigned int i = 0; i < n; ++i)
	{
		unsigned int index0 = mIndices[i * 3];
		unsigned int index1 = mIndices[i * 3 + 1];
		unsigned int index2 = mIndices[i * 3 + 2];

		vec3 v0 = mVertices[index0];
		vec3 v1 = mVertices[index1];
		vec3 v2 = mVertices[index2];

		vec3 e0 = v1 - v0;
		vec3 e1 = v2 - v0;
		vec3 normal = normalize(cross(e0, e1));

		mNormals[index0] += normal;
		mNormals[index1] += normal;
		mNormals[index2] += normal;
	}

	for (unsigned int i = 0; i < mNormals.size(); i++)
	{
		mNormals[i] = normalize(mNormals[i]);
	}
}

//=========================================================================
void Geometry::generateTangents()
{
	if (hasIndices() && hasNormals() && hasTexCoords())
	{
		std::vector<vec3> tan1Accum;
		std::vector<vec3> tan2Accum;

		for (unsigned int i = 0; i < mVertices.size(); i++)
		{
			tan1Accum.push_back(vec3(0.0f));
			tan2Accum.push_back(vec3(0.0f));
			mTangents.push_back(vec4(0.0f));
		}

		// Compute the tangent vector
		for (unsigned int i = 0; i < mIndices.size(); i += 3)
		{
			const vec3& p1 = mVertices[mIndices[i]];
			const vec3& p2 = mVertices[mIndices[i + 1]];
			const vec3& p3 = mVertices[mIndices[i + 2]];

			const vec2& tc1 = mTexCoords[mIndices[i]];
			const vec2& tc2 = mTexCoords[mIndices[i + 1]];
			const vec2& tc3 = mTexCoords[mIndices[i + 2]];

			vec3 q1 = p2 - p1;
			vec3 q2 = p3 - p1;
			float s1 = tc2.x - tc1.x, s2 = tc3.x - tc1.x;
			float t1 = tc2.y - tc1.y, t2 = tc3.y - tc1.y;
			float r = 1.0f / (s1 * t2 - s2 * t1);
			vec3 tan1((t2*q1.x - t1*q2.x) * r,
				(t2*q1.y - t1*q2.y) * r,
				(t2*q1.z - t1*q2.z) * r);
			vec3 tan2((s1*q2.x - s2*q1.x) * r,
				(s1*q2.y - s2*q1.y) * r,
				(s1*q2.z - s2*q1.z) * r);
			tan1Accum[mIndices[i]] += tan1;
			tan1Accum[mIndices[i + 1]] += tan1;
			tan1Accum[mIndices[i + 2]] += tan1;
			tan2Accum[mIndices[i]] += tan2;
			tan2Accum[mIndices[i + 1]] += tan2;
			tan2Accum[mIndices[i + 2]] += tan2;
		}

		for (unsigned int i = 0; i < mVertices.size(); ++i)
		{
			const vec3& n = mNormals[i];
			vec3& t1 = tan1Accum[i];
			vec3& t2 = tan2Accum[i];

			// Gram-Schmidt orthogonalize
			//mTangents[i] = vec3(normalize(t1 - (dot(n, t1) * n)));
			mTangents[i] = vec4(normalize(t1 - (dot(n, t1) * n)), 0.0f);
			// Store handedness in w
			mTangents[i].w = (dot(cross(n, t1), t2) < 0.0f) ? -1.0f : 1.0f;
		}

		tan1Accum.clear();
		tan2Accum.clear();
	}
}