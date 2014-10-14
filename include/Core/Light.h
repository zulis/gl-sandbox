#pragma once

#include <memory>
#include "Core/Color.h"
#include "Core/Math.h"
#include "Core/Shader.h"

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

	void setPosition(const glm::vec3& position);
	const glm::vec4& getPosition() const;

	void setAmbient(const glm::vec4& ambient);
	void setAmbient(const Color& color);
	const glm::vec4& getAmbient() const;

	void setDiffuse(const glm::vec4& diffuse);
	void setDiffuse(const Color& color);
	const glm::vec4& getDiffuse() const;

	void setSpecular(const glm::vec4& specular);
	void setSpecular(const Color& color);
	const glm::vec4& getSpecular() const;

	void setAttenuation(const glm::vec2& attenuation);
	const glm::vec2& getAttenuation() const;

	void setCutoff(float cutoff);
	float getCutoff();

	void setExponent(float exponent);
	float getExponent();

	void updateUniforms(const ShaderRef& shader, unsigned int lightIndex);

private:
	//unsigned int mLightIndex;
	LightType mLightType;
	glm::vec4 mPosition{ glm::vec4(0.0f, 0.0f, 0.0f, 0.0f) };
	glm::vec4 mAmbient{ glm::vec4(1.0f, 244.0f / 255.0f, 214.0f / 255.0f, 1.0f) };
	glm::vec4 mDiffuse{ glm::vec4(1.0f, 244.0f / 255.0f, 214.0f / 255.0f, 1.0f) };
	glm::vec4 mSpecular{ glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) };
	glm::vec2 mAttenuation{ glm::vec2(0.0, 100.0) };
	float mCutoff{ 0.0f };
	float mExponent{ 0.0f };
	
	const std::string createUniformName(const std::string& uniformName, unsigned int lightIndex) const;
};

//=========================================================================
Light::Light(const LightType& lightType)
{
	mLightType = lightType;

	switch (lightType)
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
void Light::setPosition(const glm::vec3& position)
{
	mPosition.x = position.x;
	mPosition.y = position.y;
	mPosition.z = position.z;
}

//=========================================================================
const glm::vec4& Light::getPosition() const
{
	return mPosition;
}

//=========================================================================
void Light::setAmbient(const glm::vec4& ambient)
{
	mAmbient = ambient;
}

//=========================================================================
void Light::setAmbient(const Color& color)
{
	mAmbient = glm::vec4(color.r, color.g, color.b, color.a);
}

//=========================================================================
const glm::vec4& Light::getAmbient() const
{
	return mAmbient;
}

//=========================================================================
void Light::setDiffuse(const glm::vec4& diffuse)
{
	mDiffuse = diffuse;
}

//=========================================================================
void Light::setDiffuse(const Color& color)
{
	mDiffuse = glm::vec4(color.r, color.g, color.b, color.a);
}

//=========================================================================
const glm::vec4& Light::getDiffuse() const
{
	return mDiffuse;
}

//=========================================================================
void Light::setSpecular(const glm::vec4& specular)
{
	mSpecular = specular;
}

//=========================================================================
void Light::setSpecular(const Color& color)
{
	mSpecular = glm::vec4(color.r, color.g, color.b, color.a);
}

//=========================================================================
const glm::vec4& Light::getSpecular() const
{
	return mSpecular;
}

//=========================================================================
void Light::setAttenuation(const glm::vec2& attenuation)
{
	mAttenuation = attenuation;
}

//=========================================================================
const glm::vec2& Light::getAttenuation() const
{
	return mAttenuation;
}

//=========================================================================
void Light::setCutoff(float cutoff)
{
	//mCutoff = glm::cos(cutoffAngle);
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
	if (shader->hasUniform(uniformName))
		shader->setUniform(uniformName, mPosition);

	uniformName = createUniformName(ShaderConstants::LightAmbient, lightIndex);
	if (shader->hasUniform(uniformName))
		shader->setUniform(uniformName, mAmbient);

	uniformName = createUniformName(ShaderConstants::LightDiffuse, lightIndex);
	if (shader->hasUniform(uniformName))
		shader->setUniform(uniformName, mDiffuse);

	uniformName = createUniformName(ShaderConstants::LightSpecular, lightIndex);
	if (shader->hasUniform(uniformName))
		shader->setUniform(uniformName, mSpecular);

	uniformName = createUniformName(ShaderConstants::LightAttenuation, lightIndex);
	if (shader->hasUniform(uniformName))
		shader->setUniform(uniformName, mAttenuation);

	uniformName = createUniformName(ShaderConstants::LightCutoff, lightIndex);
	if (shader->hasUniform(uniformName))
		shader->setUniform(uniformName, mCutoff);

	uniformName = createUniformName(ShaderConstants::LightExponent, lightIndex);
	if (shader->hasUniform(uniformName))
		shader->setUniform(uniformName, mExponent);
}

//=========================================================================
const std::string Light::createUniformName(const std::string& uniformName, unsigned int lightIndex) const
{
	std::string str = uniformName;
	return str.replace(str.find('#'), 1, std::to_string(lightIndex));
}
