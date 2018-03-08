#pragma once

#include "core/Math.h"
#include "core/AABB.h"
#include "core/Plane.h"

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

	bool intersects(const AABB& aabb) const;
	bool contains(const AABB& aabb) const;
    vec3 pick(const ivec2& mousePos);

private:
	vec3 mPosition;
	vec3 mLookAt;
	vec3 mUp { vec3(0.0f, 1.0f, 0.0f) };
	vec3 mRight;
    vec3 mDirection;
	mat4 mView;
	mat4 mProjection;
	float mRotateSpeed { 0.005f };
	float mStrafeSpeed { 5.0f };
	float mFov;
	float mAspectRatio;
	float mNearClip { 0.1f };
	float mFarClip { 10000.0f };

	float mHorizontalAngle { 3.14f };
	float mVerticalAngle { 0.0f };

	Plane planes[6];

	void update();
};

//=========================================================================
Camera::Camera() : mAspectRatio(4.0f / 3.0f)
{
	setPosition(0, 0, 0);
	setPerspective(60.0f, 1.0f, mNearClip, mFarClip);
	setLookAt(0, 0, 0);
}

//=========================================================================
Camera::~Camera()
{
}

//=========================================================================
void Camera::setPosition(vec3 position)
{
	mPosition = position;
	update();
}

//=========================================================================
void Camera::setPosition(float x, float y, float z)
{
	setPosition(vec3(x, y, z));
}

//=========================================================================
void Camera::setDirection(vec3 direction)
{
    auto tmpPosition = mPosition;
    setPosition(vec3(0));
    setLookAt(direction);
    setPosition(tmpPosition);
}

//=========================================================================
void Camera::setDirection(float x, float y, float z)
{
	setDirection(vec3(x, y, z));
}

//=========================================================================
void Camera::setLookAt(vec3 point)
{
	if(point != mPosition)
	{
		vec3 direction = normalize(point - mPosition);
		mVerticalAngle = asinf(direction.y);
		mHorizontalAngle = atan2f(direction.x, direction.z);
		update();
	}
}

//=========================================================================
void Camera::setLookAt(float x, float y, float z)
{
	setLookAt(vec3(x, y, z));
}

//=========================================================================
vec3 Camera::getPosition() const
{
	return mPosition;
}

//=========================================================================
vec3 Camera::getDirection() const
{
	return mDirection;
}

//=========================================================================
void Camera::setAspectRatio(float ratio)
{
	mAspectRatio = ratio;
	update();
}

//=========================================================================
mat4 Camera::getViewMatrix() const
{
	return mView;
}

//=========================================================================
mat4 Camera::getProjectionMatrix() const
{
	return mProjection;
}

//=========================================================================
void Camera::update()
{
	mDirection = vec3(
	                 cos(mVerticalAngle) * sin(mHorizontalAngle),
	                 sin(mVerticalAngle),
	                 cos(mVerticalAngle) * cos(mHorizontalAngle)
	             );

	mRight = vec3(sin(mHorizontalAngle - half_pi<float>()), 0, cos(mHorizontalAngle - half_pi<float>()));
	mUp = cross(mRight, mDirection);

	mProjection = perspective(radians(mFov), mAspectRatio, mNearClip, mFarClip);
	mView = lookAt(mPosition, mPosition + mDirection, mUp);

	// Calculate frustum planes
	vec3 cN = mPosition + mDirection * mNearClip;
	vec3 cF = mPosition + mDirection * mFarClip;

	float Hnear = 2.0f * tan(radians(mFov / 2.0f)) * mNearClip;
	float Wnear = Hnear * mAspectRatio;
	float Hfar = 2.0f * tan(radians(mFov / 2.0f)) * mFarClip;
	float Wfar = Hfar * mAspectRatio;
	float hHnear = Hnear / 2.0f;
	float hWnear = Wnear / 2.0f;
	float hHfar = Hfar / 2.0f;
	float hWfar = Wfar / 2.0f;

	vec3 farPts[4];
	vec3 nearPts[4];

	farPts[0] = cF + mUp * hHfar - mRight * hWfar;
	farPts[1] = cF - mUp * hHfar - mRight * hWfar;
	farPts[2] = cF - mUp * hHfar + mRight * hWfar;
	farPts[3] = cF + mUp * hHfar + mRight * hWfar;

	nearPts[0] = cN + mUp * hHnear - mRight * hWnear;
	nearPts[1] = cN - mUp * hHnear - mRight * hWnear;
	nearPts[2] = cN - mUp * hHnear + mRight * hWnear;
	nearPts[3] = cN + mUp * hHnear + mRight * hWnear;

	planes[0] = Plane::fromPoints(nearPts[3], nearPts[0], farPts[0]);
	planes[1] = Plane::fromPoints(nearPts[1], nearPts[2], farPts[2]);
	planes[2] = Plane::fromPoints(nearPts[0], nearPts[1], farPts[1]);
	planes[3] = Plane::fromPoints(nearPts[2], nearPts[3], farPts[2]);
	planes[4] = Plane::fromPoints(nearPts[0], nearPts[3], nearPts[2]);
	planes[5] = Plane::fromPoints(farPts[3], farPts[0], farPts[1]);

}

//=========================================================================
void Camera::setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
{
	mFov = fov;
	mAspectRatio = aspectRatio;
	mNearClip = nearPlane;
	mFarClip = farPlane;
	update();
}

//=========================================================================
float Camera::getFarClip() const
{
	return mFarClip;
}

//=========================================================================
float Camera::getNearClip() const
{
	return mNearClip;
}

//=========================================================================
void Camera::rotate(float x, float y)
{
	mHorizontalAngle += mRotateSpeed * -x;
	mVerticalAngle += mRotateSpeed * -y;
	update();
}

//=========================================================================
void Camera::setRotateSpeed(float speed)
{
	mRotateSpeed = speed;
}

//=========================================================================
void Camera::move(MovementType movement)
{
	switch(movement)
	{
		case FORWARD:
			mPosition += mDirection * mStrafeSpeed;
			break;

		case BACKWARD:
			mPosition -= mDirection * mStrafeSpeed;
			break;

		case LEFT:
			mPosition -= mRight * mStrafeSpeed;
			break;

		case RIGHT:
			mPosition += mRight * mStrafeSpeed;
			break;

		case UP:
			mPosition += mUp * mStrafeSpeed;
			break;

		case DOWN:
			mPosition -= mUp * mStrafeSpeed;
			break;
	}

	update();
}

//=========================================================================
bool Camera::intersects(const AABB& aabb) const
{
	for(unsigned int i = 0; i < 6; i++)
	{
		if(planes[i].getDistance(aabb.getPositive(planes[i].n)) < 0)
			return false;
	}

	return true;
}

//=========================================================================
bool Camera::contains(const AABB& aabb) const
{
	for(size_t i = 0; i < 6; i++)
	{
		if(planes[i].getDistance(aabb.getPositive(planes[i].n)) < 0)
			return false;
		else if(planes[i].getDistance(aabb.getNegative(planes[i].n)) < 0)
			return false;
	}

	return true;
}

//=========================================================================
vec3 Camera::getRight() const
{
	return mRight;
}

//=========================================================================
vec3 Camera::getUp() const
{
	return mUp;
}

//=========================================================================
void Camera::setNearClip(float nearClip)
{
	mNearClip = nearClip;
}

//=========================================================================
void Camera::setFarClip(float farClip)
{
	mFarClip = farClip;
}

//=========================================================================
void Camera::setStrafeSpeed(float speed)
{
	mStrafeSpeed = speed;
}

//=========================================================================
vec3 Camera::pick(const ivec2& mousePos)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    GLfloat zCursor;
	GLint winX = mousePos.x;
	GLint winY = viewport[3] - mousePos.y;
    glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zCursor);

    vec3 pos = glm::unProject(vec3(winX, winY, zCursor), mView, mProjection, ivec4(viewport[0], viewport[1], viewport[2], viewport[3]));

    return pos;
}