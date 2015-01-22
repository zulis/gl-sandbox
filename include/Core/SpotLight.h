#pragma once

#include "Core/Light.h"

typedef std::unique_ptr<class SpotLight> SpotLightRef;

class SpotLight final : public Light
{
public:
	static SpotLightRef create();
	SpotLight();
	~SpotLight() {};
};

//=========================================================================
SpotLightRef SpotLight::create()
{
	return std::make_unique<SpotLight>();
}

//=========================================================================
SpotLight::SpotLight() : Light(LightType::Spot)
{
	setExponent(50.0f);
	setCutoff(45.0f);
}