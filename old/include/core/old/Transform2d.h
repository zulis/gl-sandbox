#pragma once

#include "core/Math.h"

class Transform2D
{
public:
	Transform2D(const vec2& position = vec2(0.0f), float rotation = 0.0f, const vec2& scale = vec2(1.0f));

	void setPosition(float x, float y);
	void setPosition(const vec2& position);
	void setRotation(float rotation);
	void setScale(float scale);
	void setScale(float x, float y);
	void setScale(const vec2& scale);
	const mat4 getMatrix() const;
	void setMatrix(const mat4& matrix);

	const vec2 getPosition() const;

private:
	void updateMatrix();
	vec2 mPosition;
	float mRotation;
	vec2 mScale;
	mat4 mMatrix;

};

//=========================================================================
Transform2D::Transform2D(const vec2& position, float rotation, const vec2& scale)
{
	mPosition = position;
	mRotation = rotation;
	mScale = scale;
	updateMatrix();
}

//=========================================================================
void Transform2D::setPosition(float x, float y)
{
	setPosition(vec2(x, y));
}

//=========================================================================
void Transform2D::setPosition(const vec2& position)
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
	setScale(vec2(x, y));
}

//=========================================================================
void Transform2D::setScale(const vec2& scale)
{
	mScale = scale;
	updateMatrix();
}

//=========================================================================
void Transform2D::updateMatrix()
{
	mMatrix = mat4(1.0f);
	mMatrix = translate(mMatrix, vec3(mPosition.x, mPosition.y, 0.0f));
	mMatrix = rotate(mMatrix, mRotation, vec3(0.0f, 0.0f, 1.0f));
	mMatrix = scale(mMatrix, vec3(mScale.x, mScale.y, 0.0f));
}

//=========================================================================
const mat4 Transform2D::getMatrix() const
{
	return mMatrix;
}

//=========================================================================
void Transform2D::setMatrix(const mat4& matrix)
{
	mMatrix = matrix;
}

//=========================================================================
const vec2 Transform2D::getPosition() const
{
	return vec2(mMatrix[3][0], mMatrix[3][1]);
}