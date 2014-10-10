#pragma once

#include "Core/Light.h"

typedef std::shared_ptr<class DirectionalLight> DirectionalLightRef;

class DirectionalLight : public Light
{
public:
	static DirectionalLightRef create();
	virtual ~DirectionalLight();

private:
	DirectionalLight();
};

//=========================================================================
DirectionalLightRef DirectionalLight::create()
{
	return DirectionalLightRef(new DirectionalLight);
}

//=========================================================================
DirectionalLight::DirectionalLight() : Light(LightType::Directional)
{
}

//=========================================================================
DirectionalLight::~DirectionalLight()
{
}