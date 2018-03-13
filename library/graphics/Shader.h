#pragma once

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

private:
    class Impl;
    std::unique_ptr<Impl> impl;

};
}
