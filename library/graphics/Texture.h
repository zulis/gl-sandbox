#pragma once

#include <string>
#include <memory>

namespace library
{
class Texture
{
public:
    Texture();
    virtual ~Texture();

    void fromFile(const std::string &fileName);
    void bind(unsigned int unit = 0);
    int getWidth() const;
    int getHeight() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};
}
