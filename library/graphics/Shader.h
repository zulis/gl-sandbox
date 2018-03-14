#pragma once

#include "math/Math.h"
#include <memory>
#include <string>

namespace library
{
class Shader
{
public:
    enum class SourceType
    {
        File,
        String
    };

    Shader();
    ~Shader();

    void fromFile(const std::string &fileName);
    void fromString(const std::string &source);
    void bind();
	void reload();

    //void setUniform(const std::string &name, const Texture* tex);
    //void setUniform(const std::string &name, const TextureCube* tex);
    void setUniform(const std::string &name, int i) const;
    void setUniform(const std::string &name, unsigned int i) const;
    void setUniform(const std::string &name, float f) const;
    void setUniform(const std::string &name, float f1, float f2) const;
    void setUniform(const std::string &name, const vec2& vec2) const;
    void setUniform(const std::string &name, float f1, float f2, float f3) const;
    void setUniform(const std::string &name, float f1, float f2, float f3, float f4) const;
    void setUniform(const std::string &name, const vec3& vec3) const;
    //void setUniform(const std::string &name, const vec4& vec4) const;
    void setUniform(const std::string &name, const mat3& mat3, bool transpose = false) const;
    void setUniform(const std::string &name, const mat4& mat4, bool transpose = false) const;
    void setUniform(const std::string &name, bool b) const;
    void setUniform(const std::string &name, const vec4* vec4, int count) const;
    void setUniform(const std::string &name, const vec4& vec4) const;
    //void setUniform(const std::string &name, const Color& color) const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;

};
}
