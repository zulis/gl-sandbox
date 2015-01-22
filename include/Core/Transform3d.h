#pragma once

#include "Core/Math.h"

class Transform3D
{
public:
	Transform3D(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));

	void setPosition(float x, float y, float z);
	void setMoveBy(float x, float y, float z);
	void setRotation(float x, float y, float z);
	void setScale(float x, float y, float z);
	void setPosition(const glm::vec3& position);
	void setMoveBy(const glm::vec3& delta);
	void setRotation(const glm::vec3& rotation);
	void setRotation(const glm::quat& quat);
	void setRotationX(float x);
	void setRotationY(float Y);
	void setRotationZ(float z);
	void setScale(float scale);
	void setScale(const glm::vec3& scale);
	void setMatrix(const glm::mat4& matrix);
	void setLookAt(const glm::vec3& target);
	void setRotateTowards(const glm::quat& target, float maxAngle);

	glm::vec3 getPosition() const;
	glm::quat getRotation() const;
	glm::vec3 getEulerAngles() const;
	glm::vec3 getScale() const;
	glm::mat4 getMatrix() const;

private:
	void updateMatrix();
	glm::quat rotationBetweenVectors(glm::vec3 start, glm::vec3 target) const;
	glm::quat lookAt(const glm::vec3& direction, glm::vec3 desiredUp = glm::vec3(0, 1, 0)) const;
	glm::quat rotateTowards(glm::quat current, glm::quat target, float maxAngle) const;
	glm::vec3 mPosition;
	glm::quat mRotation;
	glm::vec3 mScale;
	glm::mat4 mMatrix;
};

//=========================================================================
Transform3D::Transform3D(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
{
	mPosition = position;
	mRotation = glm::toQuat(glm::orientate3(glm::radians(rotation)));
	mScale = scale;
	updateMatrix();
}

//=========================================================================
void Transform3D::setPosition(float x, float y, float z)
{
	setPosition(glm::vec3(x, y, z));
}

//=========================================================================
void Transform3D::setMoveBy(float x, float y, float z)
{
	setMoveBy(glm::vec3(x, y, z));
}

//=========================================================================
void Transform3D::setRotation(float x, float y, float z)
{
	setRotation(glm::vec3(x, y, z));
}

//=========================================================================
void Transform3D::setRotationX(float x)
{
	auto rotation = getEulerAngles();
	setRotation(glm::vec3(x, rotation.y, rotation.z));
}

//=========================================================================
void Transform3D::setRotationY(float y)
{
	auto rotation = getEulerAngles();
	setRotation(glm::vec3(rotation.x, y, rotation.z));
}

//=========================================================================
void Transform3D::setRotationZ(float z)
{
	auto rotation = getEulerAngles();
	setRotation(glm::vec3(rotation.x, rotation.y, z));
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
void Transform3D::setMoveBy(const  glm::vec3& delta)
{
	mPosition += delta;
	updateMatrix();
}

//=========================================================================
void Transform3D::setRotation(const glm::vec3& rotation)
{
	auto rot = glm::radians(rotation);
	mRotation = glm::quat(rot);
	//mRotation = glm::toQuat(glm::orientate3(glm::radians(rotation)));
	updateMatrix();
}

//=========================================================================
void Transform3D::setRotation(const glm::quat& quat)
{
	mRotation = quat;
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
	glm::mat4 translationMatrix = glm::translate(glm::mat4(), mPosition);
	glm::mat4 rotationMatrix = glm::toMat4(mRotation);
	glm::mat4 scaleMatrix = glm::scale(mScale);
	mMatrix = translationMatrix * rotationMatrix * scaleMatrix;
}

//=========================================================================
void Transform3D::setMatrix(const glm::mat4& matrix)
{
	mMatrix = matrix;
}

//=========================================================================
glm::vec3 Transform3D::getPosition() const
{
	return glm::vec3(mMatrix[3][0], mMatrix[3][1], mMatrix[3][2]);
}

//=========================================================================
glm::quat Transform3D::getRotation() const
{
	return mRotation;
}

//=========================================================================
glm::vec3 Transform3D::getEulerAngles() const
{
	glm::vec3 eulers;
	float sy = 2.0f * (mRotation.y * mRotation.w - mRotation.x * mRotation.z);

	if(sy < 1.0f - glm::epsilon<float>())
	{
		if(sy > -1.0f + glm::epsilon<float>())
		{
			eulers = glm::vec3(
			             atan2f(mRotation.y * mRotation.z + mRotation.x * mRotation.w, 0.5f - (mRotation.x * mRotation.x + mRotation.y * mRotation.y)),
			             asinf(sy),
			             atan2f(mRotation.x * mRotation.y + mRotation.z * mRotation.w, 0.5f - (mRotation.y * mRotation.y + mRotation.z * mRotation.z)));
		}
		else
		{
			// not a unique solution; x + z = atan2(-m21, m11)
			eulers = glm::vec3(
			             0.0f,
			             -glm::half_pi<float>(),
			             atan2f(mRotation.x * mRotation.w - mRotation.y * mRotation.z, 0.5f - (mRotation.x * mRotation.x + mRotation.z * mRotation.z)));
		}
	}
	else
	{
		// not a unique solution; x - z = atan2(-m21, m11)
		eulers = glm::vec3(
		             0.0f,
		             glm::half_pi<float>(),
		             -atan2f(mRotation.x * mRotation.w - mRotation.y * mRotation.z, 0.5f - (mRotation.x * mRotation.x + mRotation.z * mRotation.z)));
	}

	// adjust so that z, rather than y, is in [-pi/2, pi/2]
	if(eulers.z < -glm::half_pi<float>())
	{
		if(eulers.x < 0.0f)
		{
			eulers.x += glm::pi<float>();
		}
		else
		{
			eulers.x -= glm::pi<float>();
		}
		eulers.y = -eulers.y;
		if(eulers.y < 0.0f)
		{
			eulers.y += glm::pi<float>();
		}
		else
		{
			eulers.y -= glm::pi<float>();
		}
		eulers.z += glm::pi<float>();
	}
	else if(eulers.z > glm::half_pi<float>())
	{
		if(eulers.x < 0.0f)
		{
			eulers.x += glm::pi<float>();
		}
		else
		{
			eulers.x -= glm::pi<float>();
		}
		eulers.y = -eulers.y;
		if(eulers.y < 0.0f)
		{
			eulers.y += glm::pi<float>();
		}
		else
		{
			eulers.y -= glm::pi<float>();
		}
		eulers.z -= glm::pi<float>();
	}

	return glm::degrees(eulers);
}

//=========================================================================
glm::vec3 Transform3D::getScale() const
{
	return glm::vec3(glm::length(mMatrix[0]), glm::length(mMatrix[1]), glm::length(mMatrix[2]));
}

//=========================================================================
glm::mat4 Transform3D::getMatrix() const
{
	return mMatrix;
}

//=========================================================================
void Transform3D::setLookAt(const glm::vec3& target)
{
	auto position = getPosition();

	if (position != target)
	{
		auto direction = glm::normalize(target - position);
		setRotation(lookAt(direction));
	}
}

//=========================================================================
glm::quat Transform3D::rotationBetweenVectors(glm::vec3 start, glm::vec3 target) const
{
	start = glm::normalize(start);
	target = glm::normalize(target);

	float cosTheta = glm::dot(start, target);
	glm::vec3 rotationAxis;

	if(cosTheta < -1 + 0.001f)
	{
		// special case when vectors in opposite directions :
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		// This implementation favors a rotation around the Up axis,
		// since it's often what you want to do.
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);

		if(glm::length2(rotationAxis) < 0.01)   // bad luck, they were parallel, try again!
			rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = glm::normalize(rotationAxis);
		return glm::angleAxis(180.0f, rotationAxis);
	}

	// Implementation from Stan Melax's Game Programming Gems 1 article
	rotationAxis = glm::cross(start, target);

	float s = glm::sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return glm::quat(
	           s * 0.5f,
	           rotationAxis.x * invs,
	           rotationAxis.y * invs,
	           rotationAxis.z * invs
	       );
}

//=========================================================================
glm::quat Transform3D::lookAt(const glm::vec3& direction, glm::vec3 desiredUp) const
{
	if(glm::length2(direction) < 0.0001f)
		return glm::quat();

	// Recompute desiredUp so that it's perpendicular to the direction
	// You can skip that part if you really want to force desiredUp
	glm::vec3 right = glm::cross(direction, desiredUp);
	desiredUp = glm::cross(right, direction);

	// Find the rotation between the front of the object (that we assume towards +Z,
	// but this depends on your model) and the desired direction
	glm::quat rot1 = rotationBetweenVectors(glm::vec3(0.0f, 0.0f, 1.0f), direction);
	// Because of the 1rst rotation, the up is probably completely screwed up.
	// Find the rotation between the "up" of the rotated object, and the desired up
	glm::vec3 newUp = rot1 * glm::vec3(0.0f, 1.0f, 0.0f);
	glm::quat rot2 = rotationBetweenVectors(newUp, desiredUp);

	// Apply them
	return rot2 * rot1; // remember, in reverse order.
}

//=========================================================================
void Transform3D::setRotateTowards(const glm::quat& target, float maxAngle)
{
	setRotation(rotateTowards(mRotation, target, maxAngle));
}

//=========================================================================
glm::quat Transform3D::rotateTowards(glm::quat current, glm::quat target, float maxAngle) const
{
	if(maxAngle < 0.001f)
	{
		// No rotation allowed. Prevent dividing by 0 later.
		return current;
	}

	float cosTheta = glm::dot(current, target);

	// q1 and q2 are already equal.
	// Force q2 just to be sure
	if(cosTheta > 0.9999f)
	{
		return target;
	}

	// Avoid taking the long path around the sphere
	if(cosTheta < 0)
	{
		current = current*-1.0f;
		cosTheta *= -1.0f;
	}

	float angle = acos(cosTheta);

	// If there is only a 2° difference, and we are allowed 5°,
	// then we arrived.
	if(angle < maxAngle)
	{
		return target;
	}

	float fT = maxAngle / angle;
	angle = maxAngle;

	glm::quat res = (sin((1.0f - fT) * angle) * current + sin(fT * angle) * target) / sin(angle);
	res = glm::normalize(res);
	return res;
}
