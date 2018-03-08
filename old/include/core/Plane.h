#pragma once

#include "core/Math.h"

class Plane
{
public:
	Plane();
	Plane(const vec3& normal, const vec3& p);
	virtual ~Plane();

	static Plane fromPoints(const vec3& v1, const vec3& v2, const vec3& v3);
	float getDistance(const vec3& p) const;

	vec3 n;
	float d;

private:

};

//=========================================================================
Plane::Plane()
{
	n = vec3(0.0f, 1.0f, 0.0f);
	d = 0.0f;
}

//=========================================================================
Plane::Plane(const vec3& normal, const vec3& p)
{
	n = normal;
	d = -dot(n, p);
}

//=========================================================================
Plane::~Plane()
{
}

//=========================================================================
Plane Plane::fromPoints(const vec3& v1, const vec3& v2, const vec3& v3)
{
	Plane temp;
	vec3 e1 = v2 - v1;
	vec3 e2 = v3 - v1;
	temp.n = normalize(cross(e1, e2));
	temp.d = -dot(temp.n, v1);
	return temp;
}

//=========================================================================
float Plane::getDistance(const vec3& p) const
{
	return dot(n, p) + d;
}
