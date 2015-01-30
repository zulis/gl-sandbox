#pragma once

#include "core/Light.h"

typedef std::unique_ptr<class PointLight> PointLightRef;

class PointLight final : public Light
{
public:
	static PointLightRef create();
	PointLight();
	~PointLight() {};
};

//=========================================================================
PointLightRef PointLight::create()
{
	return std::make_unique<PointLight>();
}

//=========================================================================
PointLight::PointLight() : Light(LightType::Point)
{
}