#pragma once

#include <memory>
#include "math/Math.h"
#include "math/AABB.h"
#include "math/Plane.h"

class Camera
{
public:
	Camera();
	~Camera();

	void setPosition(vec3 position);
	void setPosition(float x, float y, float z);
	void setDirection(vec3 direction);
	void setDirection(float x, float y, float z);
	void setLookAt(vec3 lookAt);
	void setLookAt(float x, float y, float z);
	void setAspectRatio(float ratio);
	void setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
	void setRotateSpeed(float speed);
	void setStrafeSpeed(float speed);
	void setNearClip(float nearClip);
	void setFarClip(float farClip);

	vec3 getPosition() const;
	vec3 getDirection() const;
	mat4 getViewMatrix() const;
	mat4 getProjectionMatrix() const;
	vec3 getRight() const;
	vec3 getUp() const;
	float getFarClip() const;
	float getNearClip() const;

	void rotate(float x, float y);

	enum MovementType { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

	void move(MovementType movement);

	bool intersects(const library::AABB& aabb) const;
	bool contains(const library::AABB& aabb) const;
    vec3 pick(const ivec2& mousePos);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};
