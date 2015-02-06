#pragma once

#include "core/Math.h"

class Transform3D
{
public:
	Transform3D(const vec3& position = vec3(0.0f), const vec3& rotation = vec3(0.0f), const vec3& scale = vec3(1.0f));

	void setPosition(float x, float y, float z);
	void setMoveBy(float x, float y, float z);
	void setRotation(float x, float y, float z);
	void setScale(float x, float y, float z);
	void setPosition(const vec3& position);
	void setMoveBy(const vec3& delta);
	void setRotation(const vec3& rotation);
	void setRotation(const quat& quat);
	void setRotationX(float x);
	void setRotationY(float Y);
	void setRotationZ(float z);
	void setScale(float scale);
	void setScale(const vec3& scale);
	void setMatrix(const mat4& matrix);
	void setLookAt(const vec3& target);
	void setRotateTowards(const quat& target, float maxAngle);

	vec3 getPosition() const;
	quat getRotation() const;
	vec3 getEulerAngles() const;
	vec3 getScale() const;
	mat4 getMatrix() const;

private:
	void updateMatrix();
	quat rotationBetweenVectors(vec3 start, vec3 target) const;
	quat lookAt(const vec3& direction, vec3 desiredUp = vec3(0, 1, 0)) const;
	quat rotateTowards(quat current, quat target, float maxAngle) const;
	vec3 mPosition;
	quat mRotation;
	vec3 mScale;
	mat4 mMatrix;
};

//=========================================================================
Transform3D::Transform3D(const vec3& position, const vec3& rotation, const vec3& scale)
{
	mPosition = position;
	mRotation = toQuat(orientate3(radians(rotation)));
	mScale = scale;
	updateMatrix();
}

//=========================================================================
void Transform3D::setPosition(float x, float y, float z)
{
	setPosition(vec3(x, y, z));
}

//=========================================================================
void Transform3D::setMoveBy(float x, float y, float z)
{
	setMoveBy(vec3(x, y, z));
}

//=========================================================================
void Transform3D::setRotation(float x, float y, float z)
{
	setRotation(vec3(x, y, z));
}

//=========================================================================
void Transform3D::setRotationX(float x)
{
	auto rotation = getEulerAngles();
	setRotation(vec3(x, rotation.y, rotation.z));
}

//=========================================================================
void Transform3D::setRotationY(float y)
{
	auto rotation = getEulerAngles();
	setRotation(vec3(rotation.x, y, rotation.z));
}

//=========================================================================
void Transform3D::setRotationZ(float z)
{
	auto rotation = getEulerAngles();
	setRotation(vec3(rotation.x, rotation.y, z));
}

//=========================================================================
void Transform3D::setScale(float x, float y, float z)
{
	setScale(vec3(x, y, z));
}

//=========================================================================
void Transform3D::setPosition(const  vec3& position)
{
	mPosition = position;
	updateMatrix();
}

//=========================================================================
void Transform3D::setMoveBy(const  vec3& delta)
{
	mPosition += delta;
	updateMatrix();
}

//=========================================================================
void Transform3D::setRotation(const vec3& rotation)
{
	auto rot = radians(rotation);
	mRotation = quat(rot);
	//mRotation = toQuat(orientate3(radians(rotation)));
	updateMatrix();
}

//=========================================================================
void Transform3D::setRotation(const quat& quat)
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
void Transform3D::setScale(const vec3& scale)
{
	mScale = scale;
	updateMatrix();
}

//=========================================================================
void Transform3D::updateMatrix()
{
	mat4 translationMatrix = translate(mat4(), mPosition);
	mat4 rotationMatrix = toMat4(mRotation);
	mat4 scaleMatrix = scale(mScale);
	mMatrix = translationMatrix * rotationMatrix * scaleMatrix;
}

//=========================================================================
void Transform3D::setMatrix(const mat4& matrix)
{
	mMatrix = matrix;
}

//=========================================================================
vec3 Transform3D::getPosition() const
{
	return vec3(mMatrix[3][0], mMatrix[3][1], mMatrix[3][2]);
}

//=========================================================================
quat Transform3D::getRotation() const
{
	return mRotation;
}

//=========================================================================
vec3 Transform3D::getEulerAngles() const
{
	vec3 eulers;
	float sy = 2.0f * (mRotation.y * mRotation.w - mRotation.x * mRotation.z);

	if(sy < 1.0f - epsilon<float>())
	{
		if(sy > -1.0f + epsilon<float>())
		{
			eulers = vec3(
			             atan2f(mRotation.y * mRotation.z + mRotation.x * mRotation.w, 0.5f - (mRotation.x * mRotation.x + mRotation.y * mRotation.y)),
			             asinf(sy),
			             atan2f(mRotation.x * mRotation.y + mRotation.z * mRotation.w, 0.5f - (mRotation.y * mRotation.y + mRotation.z * mRotation.z)));
		}
		else
		{
			// not a unique solution; x + z = atan2(-m21, m11)
			eulers = vec3(
			             0.0f,
			             -half_pi<float>(),
			             atan2f(mRotation.x * mRotation.w - mRotation.y * mRotation.z, 0.5f - (mRotation.x * mRotation.x + mRotation.z * mRotation.z)));
		}
	}
	else
	{
		// not a unique solution; x - z = atan2(-m21, m11)
		eulers = vec3(
		             0.0f,
		             half_pi<float>(),
		             -atan2f(mRotation.x * mRotation.w - mRotation.y * mRotation.z, 0.5f - (mRotation.x * mRotation.x + mRotation.z * mRotation.z)));
	}

	// adjust so that z, rather than y, is in [-pi/2, pi/2]
	if(eulers.z < -half_pi<float>())
	{
		if(eulers.x < 0.0f)
		{
			eulers.x += pi<float>();
		}
		else
		{
			eulers.x -= pi<float>();
		}
		eulers.y = -eulers.y;
		if(eulers.y < 0.0f)
		{
			eulers.y += pi<float>();
		}
		else
		{
			eulers.y -= pi<float>();
		}
		eulers.z += pi<float>();
	}
	else if(eulers.z > half_pi<float>())
	{
		if(eulers.x < 0.0f)
		{
			eulers.x += pi<float>();
		}
		else
		{
			eulers.x -= pi<float>();
		}
		eulers.y = -eulers.y;
		if(eulers.y < 0.0f)
		{
			eulers.y += pi<float>();
		}
		else
		{
			eulers.y -= pi<float>();
		}
		eulers.z -= pi<float>();
	}

	return degrees(eulers);
}

//=========================================================================
vec3 Transform3D::getScale() const
{
	return vec3(length(mMatrix[0]), length(mMatrix[1]), length(mMatrix[2]));
}

//=========================================================================
mat4 Transform3D::getMatrix() const
{
	return mMatrix;
}

//=========================================================================
void Transform3D::setLookAt(const vec3& target)
{
	auto position = getPosition();

	if (position != target)
	{
		auto direction = normalize(target - position);
		setRotation(lookAt(direction));
	}
}

//=========================================================================
quat Transform3D::rotationBetweenVectors(vec3 start, vec3 target) const
{
	start = normalize(start);
	target = normalize(target);

	float cosTheta = dot(start, target);
	vec3 rotationAxis;

	if(cosTheta < -1 + 0.001f)
	{
		// special case when vectors in opposite directions :
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		// This implementation favors a rotation around the Up axis,
		// since it's often what you want to do.
		rotationAxis = cross(vec3(0.0f, 0.0f, 1.0f), start);

		if(length2(rotationAxis) < 0.01)   // bad luck, they were parallel, try again!
			rotationAxis = cross(vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = normalize(rotationAxis);
		return angleAxis(180.0f, rotationAxis);
	}

	// Implementation from Stan Melax's Game Programming Gems 1 article
	rotationAxis = cross(start, target);

	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return quat(
	           s * 0.5f,
	           rotationAxis.x * invs,
	           rotationAxis.y * invs,
	           rotationAxis.z * invs
	       );
}

//=========================================================================
quat Transform3D::lookAt(const vec3& direction, vec3 desiredUp) const
{
	if(length2(direction) < 0.0001f)
		return quat();

	// Recompute desiredUp so that it's perpendicular to the direction
	// You can skip that part if you really want to force desiredUp
	vec3 right = cross(direction, desiredUp);
	desiredUp = cross(right, direction);

	// Find the rotation between the front of the object (that we assume towards +Z,
	// but this depends on your model) and the desired direction
	quat rot1 = rotationBetweenVectors(vec3(0.0f, 0.0f, 1.0f), direction);
	// Because of the 1rst rotation, the up is probably completely screwed up.
	// Find the rotation between the "up" of the rotated object, and the desired up
	vec3 newUp = rot1 * vec3(0.0f, 1.0f, 0.0f);
	quat rot2 = rotationBetweenVectors(newUp, desiredUp);

	// Apply them
	return rot2 * rot1; // remember, in reverse order.
}

//=========================================================================
void Transform3D::setRotateTowards(const quat& target, float maxAngle)
{
	setRotation(rotateTowards(mRotation, target, maxAngle));
}

//=========================================================================
quat Transform3D::rotateTowards(quat current, quat target, float maxAngle) const
{
	if(maxAngle < 0.001f)
	{
		// No rotation allowed. Prevent dividing by 0 later.
		return current;
	}

	float cosTheta = dot(current, target);

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

	quat res = (sin((1.0f - fT) * angle) * current + sin(fT * angle) * target) / sin(angle);
	res = normalize(res);
	return res;
}
