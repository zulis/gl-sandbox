#pragma once

#include "Core/Light.h"

class DirectionalLight : public Light
{
	public:
		DirectionalLight();
		virtual ~DirectionalLight();
};

//=========================================================================
DirectionalLight::DirectionalLight() : Light(LightType::Directional)
{
}

//=========================================================================
DirectionalLight::~DirectionalLight()
{
}