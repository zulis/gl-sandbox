#pragma once

#include <Core/Math.h>

class Transform3D
{
public:
	Transform3D(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));

	void setPosition(float x, float y, float z);
	void setRotation(float x, float y, float z);
	void setScale(float x, float y, float z);
	void setPosition(const glm::vec3& position);
	void setRotation(const glm::vec3& rotation);
	void setRotationX(float x);
	void setRotationY(float Y);
	void setRotationZ(float z);
	void setScale(float scale);
	void setScale(const glm::vec3& scale);
	const glm::mat4 getMatrix() const;
	void setMatrix(const glm::mat4& matrix);

	const glm::vec3 getPosition() const;
	const glm::vec3 getRotation() const;
	const glm::vec3 getScale() const;

private:
	void updateMatrix();
	glm::vec3 mPosition;
	glm::vec3 mRotation;
	glm::vec3 mScale;
	glm::mat4 mMatrix;
};

//=========================================================================
Transform3D::Transform3D(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
{
	mPosition = position;
	mRotation = rotation;
	mScale = scale;
	updateMatrix();
}

//=========================================================================
void Transform3D::setPosition(float x, float y, float z)
{
	setPosition(glm::vec3(x, y, z));
}

//=========================================================================
void Transform3D::setRotation(float x, float y, float z)
{
	setRotation(glm::vec3(x, y, z));
}

//=========================================================================
void Transform3D::setRotationX(float x)
{
	setRotation(glm::vec3(x, mRotation.y, mRotation.z));
}

//=========================================================================
void Transform3D::setRotationY(float y)
{
	setRotation(glm::vec3(mRotation.x, y, mRotation.z));
}

//=========================================================================
void Transform3D::setRotationZ(float z)
{
	setRotation(glm::vec3(mRotation.x, mRotation.y, z));
}

//=========================================================================
void Transform3D::setScale(float x, float y, float z)
{
	setScale(glm::vec3(x, y, z));
}

//=========================================================================
void Transform3D::setPosition(const  glm::vec3& position)
{
	mPosition = position;
	updateMatrix();
}

//=========================================================================
void Transform3D::setRotation(const glm::vec3& rotation)
{
	mRotation = rotation;
	updateMatrix();
}

//=========================================================================
void Transform3D::setScale(float scale)
{
	setScale(scale, scale, scale);
}

//=========================================================================
void Transform3D::setScale(const glm::vec3& scale)
{
	mScale = scale;
	updateMatrix();
}

//=========================================================================
void Transform3D::updateMatrix()
{
	mMatrix = glm::mat4(1.0f);
	mMatrix = glm::translate(mMatrix, mPosition);
	mMatrix = glm::rotate(mMatrix, mRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	mMatrix = glm::rotate(mMatrix, mRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	mMatrix = glm::rotate(mMatrix, mRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	mMatrix = glm::scale(mMatrix, mScale);
}

//=========================================================================
const glm::mat4 Transform3D::getMatrix() const
{
	return mMatrix;
}

//=========================================================================
 void Transform3D::setMatrix(const glm::mat4& matrix)
 {
 	mMatrix = matrix;
 }

//=========================================================================
const glm::vec3 Transform3D::getPosition() const
{
	return glm::vec3(mMatrix[3][0], mMatrix[3][1], mMatrix[3][2]);
}

//=========================================================================
const glm::vec3 Transform3D::getRotation() const
{
	return mRotation;
}

//=========================================================================
const glm::vec3 Transform3D::getScale() const
{
	//return glm::vec3(mMatrix[0][0], mMatrix[1][1], mMatrix[2][2]);
	return glm::vec3(glm::length(mMatrix[0]), glm::length(mMatrix[1]), glm::length(mMatrix[2]));
}
