#pragma once

#include "core/Math.h"

class AABB
{
public:
	enum IntersectionType
	{
		Inside,
		Intersect,
		Outside
	};

	AABB();
	AABB(const vec3& aMin, const vec3& aMax);
	~AABB() {};

	vec3 getCenter() const;
	vec3 getSize() const;
	vec3 getPositive(const vec3& normal) const;
	vec3 getNegative(const vec3& normal) const;
	vec3 getMin() const;
	vec3 getMax() const;
	float getLongestEdge() const;
	float getShortestEdge() const;
	AABB transformed(const mat4& matrix) const;
	void extend(float value);
	void extend(const vec3& point);
	AABB::IntersectionType intersect(const AABB& other) const;
	bool overlaps(const AABB& other) const;

private:
	vec3 mMin;
	vec3 mMax;
};

//=========================================================================
AABB::AABB()
{
	auto minFloat = std::numeric_limits<float>::min();
	auto maxFloat = std::numeric_limits<float>::max();
	new (this) AABB(vec3(maxFloat), vec3(minFloat));
}

//=========================================================================
AABB::AABB(const vec3& aMin, const vec3& aMax) : mMin(aMin), mMax(aMax)
{
}

//=========================================================================
vec3 AABB::getCenter() const
{
	return (mMin + mMax) * 0.5f;
}

//=========================================================================
vec3 AABB::getSize() const
{
	return mMax - mMin;
}

//=========================================================================
vec3 AABB::getPositive(const vec3& normal) const
{
	vec3 result = getMin();
	vec3 size = getSize();

	if(normal.x > 0)
		result.x += size.x;

	if(normal.y > 0)
		result.y += size.y;

	if(normal.z > 0)
		result.z += size.z;

	return(result);
}

//=========================================================================
vec3 AABB::getNegative(const vec3& normal) const
{
	vec3 result = getMin();
	vec3 size = getSize();

	if(normal.x < 0)
		result.x += size.x;

	if(normal.y < 0)
		result.y += size.y;

	if(normal.z < 0)
		result.z += size.z;

	return(result);
}

//=========================================================================
AABB AABB::transformed(const mat4& matrix)  const
{
	AABB res;
	vec4 point(1, 1, 1, 1);
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
				res.extend(vec3(matrix * point));
			}
		}
	}

	return res;
}

//=========================================================================
vec3 AABB::getMin() const
{
	return mMin;
}

//=========================================================================
vec3 AABB::getMax() const
{
	return mMax;
}

//=========================================================================
float AABB::getLongestEdge() const
{
	return compMax(getSize());
}

//=========================================================================
float AABB::getShortestEdge() const
{
	return compMin(getSize());
}

//=========================================================================
void AABB::extend(float value)
{
	mMin -= vec3(value);
	mMax += vec3(value);
}

//=========================================================================
void AABB::extend(const vec3& point)
{
	mMin = min(mMin, point);
	mMax = max(mMax, point);
}

//=========================================================================
AABB::IntersectionType AABB::intersect(const AABB& other) const
{
	if((mMax.x < other.mMin.x) || (mMin.x > other.mMax.x) ||
	        (mMax.y < other.mMin.y) || (mMin.y > other.mMax.y) ||
	        (mMax.z < other.mMin.z) || (mMin.z > other.mMax.z))
		return IntersectionType::Outside;

	if((mMin.x <= other.mMin.x) && (mMax.x >= other.mMax.x) &&
	        (mMin.y <= other.mMin.y) && (mMax.y >= other.mMax.y) &&
	        (mMin.z <= other.mMin.z) && (mMax.z >= other.mMax.z))
		return IntersectionType::Inside;

	return IntersectionType::Intersect;
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
