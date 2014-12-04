#pragma once

#include "core/Light.h"

typedef std::shared_ptr<class SpotLight> SpotLightRef;

class SpotLight : public Light
{
public:
	static SpotLightRef create();
	virtual ~SpotLight();

private:
	SpotLight();
};

//=========================================================================
SpotLightRef SpotLight::create()
{
	return SpotLightRef(new SpotLight);
}

//=========================================================================
SpotLight::SpotLight() : Light(LightType::Spot)
{
	setExponent(50.0f);
	setCutoff(45.0f);
}

//=========================================================================
SpotLight::~SpotLight()
{
}