#pragma once

#include "core/Math.h"
#include "core/Color.h"

//=========================================================================
struct BaseLight
{
	Color color;
	float ambientIntensity;
	float diffuseIntensity;

	BaseLight()
	{
		color = Color(0.0f, 0.0f, 0.0f);
		ambientIntensity = 0.0f;
		diffuseIntensity = 0.0f;
	}
};

//=========================================================================
struct DirectionalLight : public BaseLight
{
	vec3 direction;

	DirectionalLight()
	{
		direction = vec3(0.0f, 0.0f, 0.0f);
	}
};

//=========================================================================
struct PointLight : public BaseLight
{
	vec3 position;

	struct
	{
		float constant;
		float linear;
		float exp;
	} attenuation;

	PointLight()
	{
		position = vec3(0.0f, 0.0f, 0.0f);
		attenuation.constant = 0.0f;
		attenuation.linear = 0.0f;
		attenuation.exp = 0.0f;
	}
};

//=========================================================================
struct SpotLight : public PointLight
{
	vec3 direction;
	float cutoff;

	SpotLight()
	{
		direction = vec3(0.0f, 0.0f, 0.0f);
		cutoff = 0.0f;
	}
};