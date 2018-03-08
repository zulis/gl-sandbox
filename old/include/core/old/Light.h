#pragma once

#include <memory>
#include "core/Color.h"
#include "core/Math.h"
#include "core/Shader.h"

enum class LightType
{
    Point,
    Directional,
    Spot
};

class Light
{
public:
	Light(const LightType& lightType);
	virtual ~Light();

	void setPosition(const vec3& position);
	void setPositionX(float x);
	void setPositionY(float y);
	void setPositionZ(float z);
	const vec4& getPosition() const;

	void setLookAt(const vec3& position);
	void setLookAtX(float x);
	void setLookAtY(float y);
	void setLookAtZ(float z);
	const vec4& getLookAt() const;

	void setAmbient(const vec4& ambient);
	void setAmbient(const Color& color);
	const vec4& getAmbient() const;

	void setDiffuse(const vec4& diffuse);
	void setDiffuse(const Color& color);
	const vec4& getDiffuse() const;

	void setDiffuseAndAmbient(const vec4& color);
	void setDiffuseAndAmbient(const Color& color);

	void setSpecular(const vec4& specular);
	void setSpecular(const Color& color);
	const vec4& getSpecular() const;

	void setAttenuation(const vec2& attenuation);
	const vec2& getAttenuation() const;

	void setCutoff(float cutoff);
	float getCutoff();

	void setExponent(float exponent);
	float getExponent();

	void updateUniforms(const ShaderRef& shader, unsigned int lightIndex);

private:
	//unsigned int mLightIndex;
	LightType mLightType;
	vec4 mPosition { vec4(0.0f, 0.0f, 0.0f, 0.0f) };
	vec4 mLookAt{ vec4(0.0f, 0.0f, 0.0f, 0.0f) };
	vec4 mAmbient { vec4(1.0f, 244.0f / 255.0f, 214.0f / 255.0f, 1.0f) };
	vec4 mDiffuse { vec4(1.0f, 244.0f / 255.0f, 214.0f / 255.0f, 1.0f) };
	vec4 mSpecular { vec4(1.0f, 1.0f, 1.0f, 1.0f) };
	vec2 mAttenuation { vec2(0.0, 100.0) }; // Only nonnegative values are accepted.
	float mCutoff{ 0.0f }; // Only values in the range [0, 90] and the special value 180 are accepted.
	float mExponent { 0.0f }; // Only values in the range [0, 128] are accepted.

	const std::string createUniformName(const std::string& uniformName, unsigned int lightIndex) const;
};

//=========================================================================
Light::Light(const LightType& lightType)
{
	mLightType = lightType;

	switch(lightType)
	{
		case LightType::Directional:
			mPosition.w = 0.0f;
			break;
		default:
			mPosition.w = 1.0f;
			break;
	}
}

//=========================================================================
Light::~Light()
{
}

//=========================================================================
void Light::setPosition(const vec3& position)
{
	mPosition.x = position.x;
	mPosition.y = position.y;
	mPosition.z = position.z;
}

//=========================================================================
void Light::setPositionX(float x)
{
	mPosition.x = x;
}

//=========================================================================
void Light::setPositionY(float y)
{
	mPosition.y = y;
}

//=========================================================================
void Light::setPositionZ(float z)
{
	mPosition.z = z;
}

//=========================================================================
const vec4& Light::getPosition() const
{
	return mPosition;
}

//=========================================================================
void Light::setLookAt(const vec3& position)
{
	mLookAt.x = position.x;
	mLookAt.y = position.y;
	mLookAt.z = position.z;
}

//=========================================================================
void Light::setLookAtX(float x)
{
	mLookAt.x = x;
}

//=========================================================================
void Light::setLookAtY(float y)
{
	mLookAt.y = y;
}

//=========================================================================
void Light::setLookAtZ(float z)
{
	mLookAt.z = z;
}

//=========================================================================
const vec4& Light::getLookAt() const
{
	return mLookAt;
}

//=========================================================================
void Light::setAmbient(const vec4& ambient)
{
	mAmbient = ambient;
}

//=========================================================================
void Light::setAmbient(const Color& color)
{
	mAmbient = vec4(color.r, color.g, color.b, color.a);
}

//=========================================================================
const vec4& Light::getAmbient() const
{
	return mAmbient;
}

//=========================================================================
void Light::setDiffuse(const vec4& diffuse)
{
	mDiffuse = diffuse;
}

//=========================================================================
void Light::setDiffuse(const Color& color)
{
	mDiffuse = vec4(color.r, color.g, color.b, color.a);
}

//=========================================================================
const vec4& Light::getDiffuse() const
{
	return mDiffuse;
}

//=========================================================================
void Light::setDiffuseAndAmbient(const vec4& color)
{
	setDiffuse(color);
	setAmbient(color);
}

//=========================================================================
void Light::setDiffuseAndAmbient(const Color& color)
{
	setDiffuse(color);
	setAmbient(color);
}

//=========================================================================
void Light::setSpecular(const vec4& specular)
{
	mSpecular = specular;
}

//=========================================================================
void Light::setSpecular(const Color& color)
{
	mSpecular = vec4(color.r, color.g, color.b, color.a);
}

//=========================================================================
const vec4& Light::getSpecular() const
{
	return mSpecular;
}

//=========================================================================
void Light::setAttenuation(const vec2& attenuation)
{
	mAttenuation = attenuation;
}

//=========================================================================
const vec2& Light::getAttenuation() const
{
	return mAttenuation;
}

//=========================================================================
void Light::setCutoff(float cutoff)
{
	//mCutoff = cos(cutoffAngle);
	mCutoff = cutoff;
}

//=========================================================================
float Light::getCutoff()
{
	return mCutoff;
}

//=========================================================================
void Light::setExponent(float exponent)
{
	mExponent = exponent;
}

//=========================================================================
float Light::getExponent()
{
	return mExponent;
}

//=========================================================================
void Light::updateUniforms(const ShaderRef& shader, unsigned int lightIndex)
{
	std::string uniformName;

	uniformName = createUniformName(ShaderConstants::LightPosition, lightIndex);
	shader->setUniform(uniformName, mPosition);

	uniformName = createUniformName(ShaderConstants::LightLookAt, lightIndex);
	shader->setUniform(uniformName, mLookAt);

	uniformName = createUniformName(ShaderConstants::LightAmbient, lightIndex);
	shader->setUniform(uniformName, mAmbient);

	uniformName = createUniformName(ShaderConstants::LightDiffuse, lightIndex);
	shader->setUniform(uniformName, mDiffuse);

	uniformName = createUniformName(ShaderConstants::LightSpecular, lightIndex);
	shader->setUniform(uniformName, mSpecular);

	uniformName = createUniformName(ShaderConstants::LightAttenuation, lightIndex);
	shader->setUniform(uniformName, mAttenuation);

	uniformName = createUniformName(ShaderConstants::LightCutoff, lightIndex);
	shader->setUniform(uniformName, mCutoff);

	uniformName = createUniformName(ShaderConstants::LightExponent, lightIndex);
	shader->setUniform(uniformName, mExponent);
}

//=========================================================================
const std::string Light::createUniformName(const std::string& uniformName, unsigned int lightIndex) const
{
	std::string str = uniformName;
	return str.replace(str.find('#'), 1, std::to_string(lightIndex));
}


