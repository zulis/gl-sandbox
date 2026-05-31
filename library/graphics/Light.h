#pragma once

#include "math/Math.h"
#include <string>

namespace library
{
    class Shader;

    class Light
    {
    public:
        enum class Type
        {
            Directional,
            Point,
            Spot
        };

        struct Attenuation
        {
            float constant = 1.0f;
            float linear = 0.0f;
            float quadratic = 0.0f;
        };

        Light();
        explicit Light(Type type);
        Light(const Light&) = default;
        Light& operator=(const Light&) = default;
        Light(Light&&) noexcept = default;
        Light& operator=(Light&&) noexcept = default;
        ~Light();

        static Light directional(const vec3& direction);
        static Light point(const vec3& position);
        static Light spot(const vec3& position, const vec3& direction, float cutoff = 45.0f,
                          float exponent = 0.0f);

        void setType(Type type);
        Type getType() const;

        void setEnabled(bool enabled);
        bool isEnabled() const;

        void setAmbient(const vec3& ambient);
        const vec3& getAmbient() const;

        void setDiffuse(const vec3& diffuse);
        const vec3& getDiffuse() const;

        void setSpecular(const vec3& specular);
        const vec3& getSpecular() const;

        void setPosition(const vec3& position);
        const vec3& getPosition() const;

        void setDirection(const vec3& direction);
        const vec3& getDirection() const;

        void setAttenuation(float constant, float linear, float quadratic);
        const Attenuation& getAttenuation() const;

        void setSpotCutoff(float cutoff);
        float getSpotCutoff() const;

        void setSpotExponent(float exponent);
        float getSpotExponent() const;

        bool usesAttenuation() const;
        bool usesSpotCone() const;

        void applyToShader(const Shader& shader, const std::string& uniformName) const;

    private:
        bool enabled = true;
        Type type = Type::Directional;
        vec3 ambient = vec3(0.0f);
        vec3 diffuse = vec3(1.0f);
        vec3 specular = vec3(1.0f);
        vec3 position = vec3(0.0f, 0.0f, 1.0f);
        vec3 direction = vec3(0.0f, 0.0f, -1.0f);
        Attenuation attenuation;
        float spotCutoff = 45.0f;
        float spotExponent = 0.0f;
    };
}