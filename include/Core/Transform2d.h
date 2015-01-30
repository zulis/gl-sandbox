#pragma once

#include "core/Math.h"

class Transform2D
{
public:
	Transform2D(const glm::vec2& position = glm::vec2(0.0f), float rotation = 0.0f, const glm::vec2& scale = glm::vec2(1.0f));

	void setPosition(float x, float y);
	void setPosition(const glm::vec2& position);
	void setRotation(float rotation);
	void setScale(float scale);
	void setScale(float x, float y);
	void setScale(const glm::vec2& scale);
	const glm::mat4 getMatrix() const;
	void setMatrix(const glm::mat4& matrix);

	const glm::vec2 getPosition() const;

private:
	void updateMatrix();
	glm::vec2 mPosition;
	float mRotation;
	glm::vec2 mScale;
	glm::mat4 mMatrix;

};

//=========================================================================
Transform2D::Transform2D(const glm::vec2& position, float rotation, const glm::vec2& scale)
{
	mPosition = position;
	mRotation = rotation;
	mScale = scale;
	updateMatrix();
}

//=========================================================================
void Transform2D::setPosition(float x, float y)
{
	setPosition(glm::vec2(x, y));
}

//=========================================================================
void Transform2D::setPosition(const glm::vec2& position)
{
	mPosition = position;
	updateMatrix();
}

//=========================================================================
void Transform2D::setRotation(float rotation)
{
	mRotation = rotation;
	updateMatrix();
}

//=========================================================================
void Transform2D::setScale(float scale)
{
	setScale(scale, scale);
}

//=========================================================================
void Transform2D::setScale(float x, float y)
{
	setScale(glm::vec2(x, y));
}

//=========================================================================
void Transform2D::setScale(const glm::vec2& scale)
{
	mScale = scale;
	updateMatrix();
}

//=========================================================================
void Transform2D::updateMatrix()
{
	mMatrix = glm::mat4(1.0f);
	mMatrix = glm::translate(mMatrix, glm::vec3(mPosition.x, mPosition.y, 0.0f));
	mMatrix = glm::rotate(mMatrix, mRotation, glm::vec3(0.0f, 0.0f, 1.0f));
	mMatrix = glm::scale(mMatrix, glm::vec3(mScale.x, mScale.y, 0.0f));
}

//=========================================================================
const glm::mat4 Transform2D::getMatrix() const
{
	return mMatrix;
}

//=========================================================================
void Transform2D::setMatrix(const glm::mat4& matrix)
{
	mMatrix = matrix;
}

//=========================================================================
const glm::vec2 Transform2D::getPosition() const
{
	return glm::vec2(mMatrix[3][0], mMatrix[3][1]);
}