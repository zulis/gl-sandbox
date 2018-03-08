#pragma once

#include <Artemis.h>
#include "core/Math.h"

class TransformComponent : public Component
{
public:
	TransformComponent(vec3 position = vec3(0.0f), vec3 rotation = vec3(0.0f), vec3 scale = vec3(1.0f));
	void setPosition(vec3 position);
	void setRotation(vec3 rotation);
	void setScale(vec3 scale);
	//mat4 getMatrix();

	vec3& getPosition();
	vec3& getRotation();
	vec3& getScale();

private:
	//const float mDiff = 100.0f;
	vec3 mPosition;
	vec3 mRotation;
	vec3 mScale;
	//mat4 mMatrix;
	//bool mNeedUpdate;

private:
	//void updateMatrix();
};

//=========================================================================
TransformComponent::TransformComponent(vec3 position, vec3 rotation, vec3 scale)
{
	setPosition(position);
	setRotation(rotation);
	setScale(scale);
}

//=========================================================================
void TransformComponent::setPosition(vec3 position)
{
	mPosition = position; // *mDiff;
	//mNeedUpdate = true;
}

//=========================================================================
void TransformComponent::setRotation(vec3 rotation)
{
	mRotation = rotation;
	//mNeedUpdate = true;
}

//=========================================================================
void TransformComponent::setScale(vec3 scale)
{
	mScale = scale;
	//mNeedUpdate = true;
}

//=========================================================================
// void TransformComponent::updateMatrix()
// {
// 	// In GLM the angle must be in degrees here, so convert it.
// 	//quat q = angleAxis(degrees(m_rotation),  RotationAxis);
// 
// 	mat4 translation = translate(mat4(), mPosition);
// 	mat4 rotation = eulerAngleYXZ(mRotation.y, mRotation.x, mRotation.z);
// 	mat4 scale = scale(mat4(), mScale);
// 	mMatrix = translation * rotation * scale;
// 
// 	mNeedUpdate = false;
// }

//=========================================================================
// mat4 TransformComponent::getMatrix()
// {
// 	if(mNeedUpdate)
// 		updateMatrix();
// 
// 	return mMatrix;
// }

//=========================================================================
vec3& TransformComponent::getPosition()
{
	return mPosition;
}

//=========================================================================
vec3& TransformComponent::getRotation()
{
	return mRotation;
}

//=========================================================================
vec3& TransformComponent::getScale()
{
	return mScale;
}
