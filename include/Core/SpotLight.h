#pragma once

#include "Core/Light.h"

class SpotLight : public Light
{
	public:
		SpotLight();
		virtual ~SpotLight();
};

//=========================================================================
SpotLight::SpotLight() : Light(LightType::Point)
{
}

//=========================================================================
SpotLight::~SpotLight()
{
}