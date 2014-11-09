#pragma once

#include "Core/Math.h"

class Plane
{
public:
	Plane();
	Plane(const glm::vec3& normal, const glm::vec3& p);
	virtual ~Plane();

	static Plane fromPoints(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
	float getDistance(const glm::vec3& p) const;

	glm::vec3 n;
	float d;

private:

};

//=========================================================================
Plane::Plane()
{
	n = glm::vec3(0.0f, 1.0f, 0.0f);
	d = 0.0f;
}

//=========================================================================
Plane::Plane(const glm::vec3& normal, const glm::vec3& p)
{
	n = normal;
	d = -glm::dot(n, p);
}

//=========================================================================
Plane::~Plane()
{
}

//=========================================================================
Plane Plane::fromPoints(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
	Plane temp;
	glm::vec3 e1 = v2 - v1;
	glm::vec3 e2 = v3 - v1;
	temp.n = glm::normalize(glm::cross(e1, e2));
	temp.d = -glm::dot(temp.n, v1);
	return temp;
}

//=========================================================================
float Plane::getDistance(const glm::vec3& p) const
{
	return glm::dot(n, p) + d;
}
