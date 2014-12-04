#pragma once

#include <Artemis.h>
#include "core/Math.h"

class TransformComponent : public Component
{
public:
	TransformComponent(glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));
	void setPosition(glm::vec3 position);
	void setRotation(glm::vec3 rotation);
	void setScale(glm::vec3 scale);
	//glm::mat4 getMatrix();

	glm::vec3& getPosition();
	glm::vec3& getRotation();
	glm::vec3& getScale();

private:
	//const float mDiff = 100.0f;
	glm::vec3 mPosition;
	glm::vec3 mRotation;
	glm::vec3 mScale;
	//glm::mat4 mMatrix;
	//bool mNeedUpdate;

private:
	//void updateMatrix();
};

//=========================================================================
TransformComponent::TransformComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	setPosition(position);
	setRotation(rotation);
	setScale(scale);
}

//=========================================================================
void TransformComponent::setPosition(glm::vec3 position)
{
	mPosition = position; // *mDiff;
	//mNeedUpdate = true;
}

//=========================================================================
void TransformComponent::setRotation(glm::vec3 rotation)
{
	mRotation = rotation;
	//mNeedUpdate = true;
}

//=========================================================================
void TransformComponent::setScale(glm::vec3 scale)
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
// 	glm::mat4 translation = glm::translate(glm::mat4(), mPosition);
// 	glm::mat4 rotation = glm::eulerAngleYXZ(mRotation.y, mRotation.x, mRotation.z);
// 	glm::mat4 scale = glm::scale(glm::mat4(), mScale);
// 	mMatrix = translation * rotation * scale;
// 
// 	mNeedUpdate = false;
// }

//=========================================================================
// glm::mat4 TransformComponent::getMatrix()
// {
// 	if(mNeedUpdate)
// 		updateMatrix();
// 
// 	return mMatrix;
// }

//=========================================================================
glm::vec3& TransformComponent::getPosition()
{
	return mPosition;
}

//=========================================================================
glm::vec3& TransformComponent::getRotation()
{
	return mRotation;
}

//=========================================================================
glm::vec3& TransformComponent::getScale()
{
	return mScale;
}
