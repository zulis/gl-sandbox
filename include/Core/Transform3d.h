#pragma once

#include <Core/Math.h>

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
	const glm::mat4 getMatrix() const;
	void setMatrix(const glm::mat4& matrix);
	void setLookAt(const glm::vec3& point);

	const glm::vec3 getPosition() const;
	const glm::quat getRotation() const;
	const glm::vec3 getScale() const;

private:
	void updateMatrix();
	glm::quat rotateTowards(glm::quat q1, glm::quat q2, float maxAngle);
	glm::quat rotationBetweenVectors(glm::vec3 start, glm::vec3 dest);
	glm::quat lookAt(glm::vec3 direction, glm::vec3 desiredUp = glm::vec3(0, 1, 0));
	glm::vec3 mPosition;
	glm::quat mRotation;
	glm::vec3 mScale;
	glm::mat4 mMatrix;
};

//=========================================================================
Transform3D::Transform3D(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
{
	mPosition = position;
	mRotation = glm::toQuat(glm::orientate3(rotation));
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
void Transform3D::setMoveBy(const  glm::vec3& delta)
{
	mPosition += delta;
	updateMatrix();
}

//=========================================================================
void Transform3D::setRotation(const glm::vec3& rotation)
{
	//mRotation = glm::toQuat(glm::orientate3(rotation));
	auto rot = glm::radians(rotation);
	mRotation = glm::quat(rot);
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
const glm::quat Transform3D::getRotation() const
{
	return mRotation;
}

//=========================================================================
const glm::vec3 Transform3D::getScale() const
{
	//return glm::vec3(mMatrix[0][0], mMatrix[1][1], mMatrix[2][2]);
	return glm::vec3(glm::length(mMatrix[0]), glm::length(mMatrix[1]), glm::length(mMatrix[2]));
}

//=========================================================================
void Transform3D::setLookAt(const glm::vec3& point)
{
	auto position = getPosition();
	glm::vec3 direction = glm::normalize(point - position);

	auto q = lookAt(direction);
	setRotation(q);

	/*
	if (point != position)
	{
	glm::vec3 direction = glm::normalize(point - position);
	auto verticalAngle = asinf(direction.y);
	auto horizontalAngle = atan2f(direction.x, direction.z);
	//setRotationY(glm::degrees(mHorizontalAngle));
	//setRotationX(glm::degrees(mVerticalAngle));
	//setRotationZ(glm::degrees(mVerticalAngle));

	direction = glm::vec3(
	cos(verticalAngle) * sin(horizontalAngle),
	sin(verticalAngle),
	cos(verticalAngle) * cos(horizontalAngle)
	);

	glm::vec3 right = glm::vec3(sin(horizontalAngle - glm::half_pi<float>()), 0, cos(horizontalAngle - glm::half_pi<float>()));
	glm::vec3 up = glm::cross(right, direction);

	setMatrix(glm::lookAt(point, position + direction, up));
	}
	*/
}

glm::quat Transform3D::rotateTowards(glm::quat q1, glm::quat q2, float maxAngle)
{
	if(maxAngle < 0.001f)
	{
		// No rotation allowed. Prevent dividing by 0 later.
		return q1;
	}

	float cosTheta = glm::dot(q1, q2);

	// q1 and q2 are already equal.
	// Force q2 just to be sure
	if(cosTheta > 0.9999f)
	{
		return q2;
	}

	// Avoid taking the long path around the sphere
	if(cosTheta < 0)
	{
		q1 = q1*-1.0f;
		cosTheta *= -1.0f;
	}

	float angle = glm::acos(cosTheta);

	// If there is only a 2° difference, and we are allowed 5°,
	// then we arrived.
	if(angle < maxAngle)
	{
		return q2;
	}

	// This is just like slerp(), but with a custom t
	float t = maxAngle / angle;
	angle = maxAngle;

	glm::quat res = (glm::sin((1.0f - t) * angle) * q1 + glm::sin(t * angle) * q2) / glm::sin(angle);
	res = glm::normalize(res);
	return res;

}

// Returns a quaternion such that q*start = dest
glm::quat Transform3D::rotationBetweenVectors(glm::vec3 start, glm::vec3 dest)
{
	start = glm::normalize(start);
	dest = glm::normalize(dest);

	float cosTheta = glm::dot(start, dest);
	glm::vec3 rotationAxis;

	if(cosTheta < -1 + 0.001f)
	{
		// special case when vectors in opposite directions :
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		// This implementation favors a rotation around the Up axis,
		// since it's often what you want to do.
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);

		if(glm::length2(rotationAxis) < 0.01)  // bad luck, they were parallel, try again!
			rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = glm::normalize(rotationAxis);
		return glm::angleAxis(180.0f, rotationAxis);
	}

	// Implementation from Stan Melax's Game Programming Gems 1 article
	rotationAxis = glm::cross(start, dest);

	float s = glm::sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return glm::quat(
	           s * 0.5f,
	           rotationAxis.x * invs,
	           rotationAxis.y * invs,
	           rotationAxis.z * invs
	       );


}

glm::quat Transform3D::lookAt(glm::vec3 direction, glm::vec3 desiredUp)
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
