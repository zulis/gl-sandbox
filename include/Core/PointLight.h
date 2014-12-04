#pragma once

#include "core/Light.h"

typedef std::shared_ptr<class PointLight> PointLightRef;

class PointLight : public Light
{
public:
	static PointLightRef create();
	virtual ~PointLight();

private:
	PointLight();
};

//=========================================================================
PointLightRef PointLight::create()
{
	return PointLightRef(new PointLight);
}

//=========================================================================
PointLight::PointLight() : Light(LightType::Point)
{
}

//=========================================================================
PointLight::~PointLight()
{
}