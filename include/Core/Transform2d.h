#pragma once

#include "Core/Math.h"

class Transform2d
{
	public:
		Transform2d(glm::vec2 position = glm::vec2(0.0f), float rotation = 0.0f, glm::vec2 scale = glm::vec2(1.0f));
		virtual ~Transform2d() {};

		void setPosition(float x, float y);
		void setPosition(glm::vec2 position);
		void setRotation(float rotation);
		void setScale(float scale);
		void setScale(float x, float y);
		void setScale(glm::vec2 scale);
		mat4 getMatrix();
		void setMatrix(mat4 matrix);

		vec2 getPosition() const;

	private:
		void updateMatrix();
		vec2 mPosition;
		float mRotation;
		vec2 mScale;
		mat4 mMatrix;

};

//=========================================================================
Transform2d::Transform2d(glm::vec2 position, float rotation, glm::vec2 scale)
{
	mPosition = position;
	mRotation = rotation;
	mScale = scale;
	updateMatrix();
}

//=========================================================================
void Transform2d::setPosition(float x, float y)
{
	setPosition(glm::vec2(x, y));
}

//=========================================================================
void Transform2d::setPosition(glm::vec2 position)
{
	mPosition = position;
	updateMatrix();
}

//=========================================================================
void Transform2d::setRotation(float rotation)
{
	mRotation = rotation;
	updateMatrix();
}

//=========================================================================
void Transform2d::setScale(float scale)
{
	setScale(scale, scale);
}

//=========================================================================
void Transform2d::setScale(float x, float y)
{
	setScale(glm::vec2(x, y));
}

//=========================================================================
void Transform2d::setScale(glm::vec2 scale)
{
	mScale = scale;
	updateMatrix();
}

//=========================================================================
void Transform2d::updateMatrix()
{
	mMatrix = mat4(1.0f);
	mMatrix = translate(mMatrix, glm::vec3(mPosition.x, mPosition.y, 0.0f));
	mMatrix = rotate(mMatrix, mRotation, glm::vec3(0.0f, 0.0f, 1.0f));
	mMatrix = scale(mMatrix, glm::vec3(mScale.x, mScale.y, 0.0f));
}

//=========================================================================
mat4 Transform2d::getMatrix()
{
	return mMatrix;
}

//=========================================================================
void Transform2d::setMatrix(mat4 matrix)
{
	mMatrix = matrix;
}

//=========================================================================
vec2 Transform2d::getPosition() const
{
	return glm::vec2(mMatrix[3][0], mMatrix[3][1]);
}