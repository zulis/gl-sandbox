#pragma once

#include "Core/Light.h"

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
}

//=========================================================================
SpotLight::~SpotLight()
{
}