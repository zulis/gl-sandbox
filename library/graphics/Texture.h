#ifndef PROJECT_TEXTURE_H
#define PROJECT_TEXTURE_H

#include <string>
#include <memory>

namespace library
{
class Texture
{
public:
    Texture();
    ~Texture();

    void fromFile(const std::string &fileName);
    void bind(unsigned int unit = 0);
    int getWidth() const;
    int getHeight() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};
}

#endif //PROJECT_TEXTURE_H
