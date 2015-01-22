#pragma once

#include "Core/Light.h"

typedef std::unique_ptr<class DirectionalLight> DirectionalLightRef;

class DirectionalLight final : public Light
{
public:
	static DirectionalLightRef create();
	DirectionalLight();
	~DirectionalLight() {};
};

//=========================================================================
DirectionalLightRef DirectionalLight::create()
{
	return std::make_unique<DirectionalLight>();
}

//=========================================================================
DirectionalLight::DirectionalLight() : Light(LightType::Directional)
{
}