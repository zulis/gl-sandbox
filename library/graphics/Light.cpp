#include "Light.h"
#include "Shader.h"

#include <algorithm>

namespace
{
    glm::vec3 normalizeDirection(const glm::vec3& direction)
    {
        if (glm::length(direction) <= 0.0001f)
            return glm::vec3(0.0f, 0.0f, -1.0f);

        return glm::normalize(direction);
    }

    float clampSpotCutoff(float cutoff)
    {
        if (cutoff >= 180.0f)
            return 180.0f;

        return std::clamp(cutoff, 0.0f, 90.0f);
    }

    float clampSpotExponent(float exponent)
    {
        return std::clamp(exponent, 0.0f, 128.0f);
    }
}

namespace library
{
    Light::Light()
        : Light(Type::Directional)
    {
    }

    Light::Light(Type type)
        : type(type)
    {
        direction = normalizeDirection(direction);
    }

    Light::~Light()
    {
    }

    Light Light::directional(const vec3& direction)
    {
        Light light(Type::Directional);
        light.setDirection(direction);
        return light;
    }

    Light Light::point(const vec3& position)
    {
        Light light(Type::Point);
        light.setPosition(position);
        return light;
    }

    Light Light::spot(const vec3& position, const vec3& direction, float cutoff, float exponent)
    {
        Light light(Type::Spot);
        light.setPosition(position);
        light.setDirection(direction);
        light.setSpotCutoff(cutoff);
        light.setSpotExponent(exponent);
        return light;
    }

    void Light::setType(Type type)
    {
        this->type = type;
    }

    Light::Type Light::getType() const
    {
        return type;
    }

    void Light::setEnabled(bool enabled)
    {
        this->enabled = enabled;
    }

    bool Light::isEnabled() const
    {
        return enabled;
    }

    void Light::setAmbient(const vec3& ambient)
    {
        this->ambient = ambient;
    }

    const vec3& Light::getAmbient() const
    {
        return ambient;
    }

    void Light::setDiffuse(const vec3& diffuse)
    {
        this->diffuse = diffuse;
    }

    const vec3& Light::getDiffuse() const
    {
        return diffuse;
    }

    void Light::setSpecular(const vec3& specular)
    {
        this->specular = specular;
    }

    const vec3& Light::getSpecular() const
    {
        return specular;
    }

    void Light::setPosition(const vec3& position)
    {
        this->position = position;
    }

    const vec3& Light::getPosition() const
    {
        return position;
    }

    void Light::setDirection(const vec3& direction)
    {
        this->direction = normalizeDirection(direction);
    }

    const vec3& Light::getDirection() const
    {
        return direction;
    }

    void Light::setAttenuation(float constant, float linear, float quadratic)
    {
        attenuation.constant = std::max(constant, 0.0f);
        attenuation.linear = std::max(linear, 0.0f);
        attenuation.quadratic = std::max(quadratic, 0.0f);
    }

    const Light::Attenuation& Light::getAttenuation() const
    {
        return attenuation;
    }

    void Light::setSpotCutoff(float cutoff)
    {
        spotCutoff = clampSpotCutoff(cutoff);
    }

    float Light::getSpotCutoff() const
    {
        return spotCutoff;
    }

    void Light::setSpotExponent(float exponent)
    {
        spotExponent = clampSpotExponent(exponent);
    }

    float Light::getSpotExponent() const
    {
        return spotExponent;
    }

    bool Light::usesAttenuation() const
    {
        return type != Type::Directional;
    }

    bool Light::usesSpotCone() const
    {
        return type == Type::Spot;
    }

    void Light::applyToShader(const Shader& shader, const std::string& uniformName) const
    {
        shader.setUniform(uniformName + ".enabled", enabled);
        shader.setUniform(uniformName + ".type", static_cast<int>(type));
        shader.setUniform(uniformName + ".ambient", ambient);
        shader.setUniform(uniformName + ".diffuse", diffuse);
        shader.setUniform(uniformName + ".specular", specular);
        shader.setUniform(uniformName + ".position", position);
        shader.setUniform(uniformName + ".direction", direction);
        shader.setUniform(uniformName + ".attenuation",
                          vec3(attenuation.constant, attenuation.linear, attenuation.quadratic));
        shader.setUniform(uniformName + ".spotCutoff", spotCutoff);
        shader.setUniform(uniformName + ".spotExponent", spotExponent);
        shader.setUniform(uniformName + ".usesAttenuation", usesAttenuation());
        shader.setUniform(uniformName + ".usesSpotCone", usesSpotCone());
    }
}