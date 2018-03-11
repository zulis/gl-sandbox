#ifndef PROJECT_SHADER_H
#define PROJECT_SHADER_H

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

private:
    class Impl;
    std::unique_ptr<Impl> impl;

};
}

#endif //PROJECT_SHADER_H
