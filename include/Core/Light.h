#pragma once

#include "Core/Math.h"
#include "COre/Shader.h"

enum class LightType
{
	Point,
	Directional
};

class Light
{
public:
	Light(const LightType& lightType);
	virtual ~Light();

	void setPosition(const glm::vec3& position);
	const glm::vec4& getPosition() const;

	void setAmbient(const glm::vec4& ambient);
	const glm::vec4& getAmbient() const;

	void setDiffuse(const glm::vec4& diffuse);
	const glm::vec4& getDiffuse() const;

	void setSpecular(const glm::vec4& specular);
	const glm::vec4& getSpecular() const;

	void updateUniforms(const ShaderRef& shader, unsigned int lightIndex) const;

private:
	LightType mLightType;
	glm::vec4 mPosition{ glm::vec4(0.0f, 0.0f, 0.0f, 0.0f) };
	glm::vec4 mAmbient{ glm::vec4(1.0f, 244.0f / 255.0f, 214.0f / 255.0f, 1.0f) };
	glm::vec4 mDiffuse{ glm::vec4(1.0f, 244.0f / 255.0f, 214.0f / 255.0f, 1.0f) };
	glm::vec4 mSpecular{ glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) };

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
const glm::vec4& Light::getSpecular() const
{
	return mSpecular;
}

//=========================================================================
void Light::updateUniforms(const ShaderRef& shader, unsigned int lightIndex) const
{
	std::string uniformName;

	shader->bind();

	if (shader->hasUniform(ShaderConstants::MaxLights))
		shader->setUniform(ShaderConstants::MaxLights, lightIndex);

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

	//shader->unbind();
}

//=========================================================================
const std::string Light::createUniformName(const std::string& uniformName, unsigned int lightIndex) const
{
	std::string str = uniformName;
	return str.replace(str.find('#'), 1, std::to_string(lightIndex));
}
