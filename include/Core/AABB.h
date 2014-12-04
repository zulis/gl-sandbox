#pragma once

#include "core/Math.h"

enum class AABBIntersectionType
{
    Inside,
    Intersect,
    Outside
};

class AABB
{
public:
	AABB();
	AABB(const glm::vec3& aMin, const glm::vec3& aMax);
	~AABB() {};

	glm::vec3 getCenter() const;
	glm::vec3 getSize() const;
	glm::vec3 getPositive(const glm::vec3& normal) const;
	glm::vec3 getNegative(const glm::vec3& normal) const;
	glm::vec3 getMin() const;
	glm::vec3 getMax() const;
	glm::vec3 getDiagonal() const;
	float getLongestEdge() const;
	float getShortestEdge() const;
	AABB transformed(const glm::mat4& matrix) const;
	void extend(float value);
	void extend(const glm::vec3& point);
	AABBIntersectionType intersect(const AABB& other) const;
	bool overlaps(const AABB& other) const;

private:
	glm::vec3 mMin;
	glm::vec3 mMax;
};

//=========================================================================
AABB::AABB()
{
	auto minFloat = std::numeric_limits<float>::min();
	auto maxFloat = std::numeric_limits<float>::max();
	AABB(glm::vec3(maxFloat), glm::vec3(minFloat));
}

//=========================================================================
AABB::AABB(const glm::vec3& aMin, const glm::vec3& aMax) : mMin(aMin), mMax(aMax)
{
}

//=========================================================================
glm::vec3 AABB::getCenter() const
{
	return (mMin + mMax) * 0.5f;
}

//=========================================================================
glm::vec3 AABB::getSize() const
{
	return mMax - mMin;
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
AABB AABB::transformed(const glm::mat4& matrix)  const
{
	AABB res;
	glm::vec4 point(1, 1, 1, 1);
	float aabbIn[6] = { mMin.x, mMin.y, mMin.z, mMax.x, mMax.y, mMax.z };

	for(int i = 0; i < 2; i++)
	{
		for(int j = 0; j < 2; j++)
		{
			for(int k = 0; k < 2; k++)
			{
				point.x = aabbIn[i * 3];
				point.y = aabbIn[j * 3 + 1];
				point.z = aabbIn[k * 3 + 2];
				res.extend(glm::vec3(matrix * point));
			}
		}
	}

	return res;
}

//=========================================================================
glm::vec3 AABB::getMin() const
{
	return mMin;
}

//=========================================================================
glm::vec3 AABB::getMax() const
{
	return mMax;
}

//=========================================================================
glm::vec3 AABB::getDiagonal() const
{
	return mMax - mMin;
}

//=========================================================================
float AABB::getLongestEdge() const
{
	return glm::compMax(getDiagonal());
}

//=========================================================================
float AABB::getShortestEdge() const
{
	return glm::compMin(getDiagonal());
}

//=========================================================================
void AABB::extend(float value)
{
	mMin -= glm::vec3(value);
	mMax += glm::vec3(value);
}

//=========================================================================
void AABB::extend(const glm::vec3& point)
{
	mMin = glm::min(mMin, point);
	mMax = glm::max(mMax, point);
}

//=========================================================================
AABBIntersectionType AABB::intersect(const AABB& other) const
{
	if((mMax.x < other.mMin.x) || (mMin.x > other.mMax.x) ||
	        (mMax.y < other.mMin.y) || (mMin.y > other.mMax.y) ||
	        (mMax.z < other.mMin.z) || (mMin.z > other.mMax.z))
		return AABBIntersectionType::Outside;

	if((mMin.x <= other.mMin.x) && (mMax.x >= other.mMax.x) &&
	        (mMin.y <= other.mMin.y) && (mMax.y >= other.mMax.y) &&
	        (mMin.z <= other.mMin.z) && (mMax.z >= other.mMax.z))
		return AABBIntersectionType::Inside;

	return AABBIntersectionType::Intersect;
}

//=========================================================================
bool AABB::overlaps(const AABB& other) const
{
	if(other.mMin.x > mMax.x || other.mMax.x < mMin.x)
		return false;
	else if(other.mMin.y > mMax.y || other.mMax.y < mMin.y)
		return false;
	else if(other.mMin.z > mMax.z || other.mMax.z < mMin.z)
		return false;

	return true;
}
