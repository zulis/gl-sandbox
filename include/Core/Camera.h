#pragma once

#include <memory>
#include "core/Math.h"
#include "core/AABB.h"
#include "core/Plane.h"

typedef std::shared_ptr<class Camera> CameraRef;

class Camera
{
public:
	static CameraRef create();

	Camera();
	virtual ~Camera();

	void setPosition(glm::vec3 position);
	void setPosition(float x, float y, float z);
	void setLookAt(glm::vec3 lookAt);
	void setLookAt(float x, float y, float z);
	void setAspectRatio(float ratio);
	void setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
	void setRotateSpeed(float speed);
	void setStrafeSpeed(float speed);
	void setNearClip(float nearClip);
	void setFarClip(float farClip);

	glm::vec3 getPosition() const;
	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;
	glm::vec3 getRight() const;
	glm::vec3 getUp() const;
	float getFarClip() const;
	float getNearClip() const;

	void rotate(float x, float y);

	enum MovementType { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

	void move(MovementType movement);

	bool intersects(const AABB& aabb) const;
	bool contains(const AABB& aabb) const;

private:
	glm::vec3 mPosition;
	glm::vec3 mLookAt;
	glm::vec3 mUp { glm::vec3(0.0f, 1.0f, 0.0f) };
	glm::vec3 mRight;
	glm::vec3 mDirection;
	glm::mat4 mView;
	glm::mat4 mProjection;
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
CameraRef Camera::create()
{
	return CameraRef(new Camera);
}

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
void Camera::setPosition(glm::vec3 position)
{
	mPosition = position;
	update();
}

//=========================================================================
void Camera::setPosition(float x, float y, float z)
{
	setPosition(glm::vec3(x, y, z));
}

//=========================================================================
void Camera::setLookAt(glm::vec3 point)
{
	if (point != mPosition)
	{
		glm::vec3 direction = glm::normalize(point - mPosition);
		mVerticalAngle = asinf(direction.y);
		mHorizontalAngle = atan2f(direction.x, direction.z);
		update();
	}
}

//=========================================================================
void Camera::setLookAt(float x, float y, float z)
{
	setLookAt(glm::vec3(x, y, z));
}

//=========================================================================
glm::vec3 Camera::getPosition() const
{
	return mPosition;
}

//=========================================================================
void Camera::setAspectRatio(float ratio)
{
	mAspectRatio = ratio;
	update();
}

//=========================================================================
glm::mat4 Camera::getViewMatrix() const
{
	return mView;
}

//=========================================================================
glm::mat4 Camera::getProjectionMatrix() const
{
	return mProjection;
}

//=========================================================================
void Camera::update()
{
	mDirection = glm::vec3(
	                 cos(mVerticalAngle) * sin(mHorizontalAngle),
	                 sin(mVerticalAngle),
	                 cos(mVerticalAngle) * cos(mHorizontalAngle)
	             );

	mRight = glm::vec3(sin(mHorizontalAngle - glm::half_pi<float>()), 0, cos(mHorizontalAngle - glm::half_pi<float>()));
	mUp = glm::cross(mRight, mDirection);

	mProjection = glm::perspective(mFov, mAspectRatio, mNearClip, mFarClip);
	mView = glm::lookAt(mPosition, mPosition + mDirection, mUp);

	// Calculate frustum planes
	glm::vec3 cN = mPosition + mDirection * mNearClip;
	glm::vec3 cF = mPosition + mDirection * mFarClip;

	float Hnear = 2.0f * tan(glm::radians(mFov / 2.0f)) * mNearClip;
	float Wnear = Hnear * mAspectRatio;
	float Hfar = 2.0f * tan(glm::radians(mFov / 2.0f)) * mFarClip;
	float Wfar = Hfar * mAspectRatio;
	float hHnear = Hnear / 2.0f;
	float hWnear = Wnear / 2.0f;
	float hHfar = Hfar / 2.0f;
	float hWfar = Wfar / 2.0f;

	glm::vec3 farPts[4];
	glm::vec3 nearPts[4];

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
glm::vec3 Camera::getRight() const
{
	return mRight;
}

//=========================================================================
glm::vec3 Camera::getUp() const
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
