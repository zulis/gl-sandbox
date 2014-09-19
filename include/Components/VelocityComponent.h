#pragma once

#include <Artemis.h>

class VelocityComponent : public Component
{
public:
	float x { 0.0f };
	float y { 0.0f };
	float z { 0.0f };

	VelocityComponent();
	VelocityComponent(float x, float y, float z);
};

//=========================================================================
VelocityComponent::VelocityComponent()
{
}

//=========================================================================
VelocityComponent::VelocityComponent(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}
