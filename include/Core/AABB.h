#pragma once

#include <memory>
#include <bitset>
#include <vector>
#include "Core/Math.h"

class AABB
{
public:
	AABB();
	AABB(const glm::vec3& aMin, const glm::vec3& aMax);
	~AABB();

	glm::vec3 getCenter() const;
	glm::vec3 getSize() const;
	glm::vec3 getPositive(const glm::vec3& normal) const;
	glm::vec3 getNegative(const glm::vec3& normal) const;
	const glm::vec3& getMin() const;
	const glm::vec3& getMax() const;

	AABB transformed(const glm::mat4& matrix) const;
	void draw();

private:
	glm::vec3 mExtents[2];
	glm::vec3 mVertices[8];

	inline glm::vec3 transformPointAffine(const glm::mat4& m, const glm::vec3& rhs) const;
};

//=========================================================================
AABB::AABB()
{
	auto minFloat = std::numeric_limits<float>::min();
	auto maxFloat = std::numeric_limits<float>::max();
	AABB(glm::vec3(maxFloat, maxFloat, maxFloat), glm::vec3(minFloat, minFloat, minFloat));
}

//=========================================================================
AABB::AABB(const glm::vec3& aMin, const glm::vec3& aMax)
{
	mExtents[0] = glm::vec3(aMin.x, aMin.y, aMin.z);
	mExtents[1] = glm::vec3(aMax.x, aMax.y, aMax.z);

	glm::vec3 extent(aMax.x - aMin.x, aMax.y - aMin.y, aMax.z - aMin.z);
	glm::vec3 mid((aMin.x + aMax.x) / 2.0f, (aMin.y + aMax.y) / 2.0f, (aMin.z + aMax.z) / 2.0f);
	mVertices[0] = glm::vec3(-0.5f, -0.5f, 0.5f) * extent + mid;
	mVertices[1] = glm::vec3(0.5f, -0.5f, 0.5f) * extent + mid;
	mVertices[2] = glm::vec3(-0.5f, 0.5f, 0.5f) * extent + mid;
	mVertices[3] = glm::vec3(0.5f, 0.5f, 0.5f) * extent + mid;
	mVertices[4] = glm::vec3(-0.5f, 0.5f, -0.5f) * extent + mid;
	mVertices[5] = glm::vec3(0.5f, 0.5f, -0.5f) * extent + mid;
	mVertices[6] = glm::vec3(-0.5f, -0.5f, -0.5f) * extent + mid;
	mVertices[7] = glm::vec3(0.5f, -0.5f, -0.5f) * extent + mid;
}

//=========================================================================
AABB::~AABB()
{
}

//=========================================================================
glm::vec3 AABB::getCenter() const
{
	return (mExtents[1] + mExtents[0]) * 0.5f;
}

//=========================================================================
glm::vec3 AABB::getSize() const
{
	return mExtents[1] - mExtents[0];
}

//=========================================================================
AABB AABB::transformed(const glm::mat4& matrix) const
{
	glm::vec3 verts[8];

	for(unsigned int i = 0; i < 8; i++)
		verts[i] = transformPointAffine(matrix, mVertices[i]);

	glm::vec3 min = verts[0];
	glm::vec3 max = verts[0];

	for(unsigned int i = 1; i < 8; i++)
	{
		if(verts[i].x < min.x) min.x = verts[i].x;
		if(verts[i].y < min.y) min.y = verts[i].y;
		if(verts[i].z < min.z) min.z = verts[i].z;

		if(verts[i].x > max.x) max.x = verts[i].x;
		if(verts[i].y > max.y) max.y = verts[i].y;
		if(verts[i].z > max.z) max.z = verts[i].z;
	}

	return AABB(min, max);
}

//=========================================================================
glm::vec3 AABB::transformPointAffine(const glm::mat4& m, const glm::vec3& rhs) const
{
	auto x = m[0][0] * rhs.x + m[0][1] * rhs.y + m[0][2] * rhs.z + m[0][3];
	auto y = m[1][0] * rhs.x + m[1][1] * rhs.y + m[1][2] * rhs.z + m[1][3];
	auto z = m[2][0] * rhs.x + m[2][1] * rhs.y + m[2][2] * rhs.z + m[2][3];

	return glm::vec3(x, y, z);
}

//=========================================================================
glm::vec3 AABB::getPositive(const glm::vec3& normal) const
{
	glm::vec3 result = getMin();
	glm::vec3 size = getSize();

	if(normal.x > 0)
		result.x += size.x;

	if(normal.y > 0)
		result.y += size.y;

	if(normal.z > 0)
		result.z += size.z;

	return(result);
}

//=========================================================================
glm::vec3 AABB::getNegative(const glm::vec3& normal) const
{
	glm::vec3 result = getMin();
	glm::vec3 size = getSize();

	if(normal.x < 0)
		result.x += size.x;

	if(normal.y < 0)
		result.y += size.y;

	if(normal.z < 0)
		result.z += size.z;

	return(result);
}

//=========================================================================
const glm::vec3& AABB::getMin() const
{
	return mExtents[0];
}

//=========================================================================
const glm::vec3& AABB::getMax() const
{
	return mExtents[1];
}