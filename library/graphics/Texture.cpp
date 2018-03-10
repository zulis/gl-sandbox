#include "Texture.h"
#include "GL.h"
#include "../system/Resource.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace library
{

class TextureHandler
{
public:
    TextureHandler(const std::string &fileName);
    ~TextureHandler();

    void bind(unsigned int unit = 0);
    int getWidth() const;
    int getHeight() const;

private:
    unsigned int textureID;
    int width;
    int height;
    int channels;
};

TextureHandler::TextureHandler(const std::string &fileName)
{
    unsigned char *pixels = stbi_load(fileName.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    GLfloat max_aniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
    // set the maximum!
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
}

TextureHandler::~TextureHandler()
{
    glDeleteTextures(1, &textureID);
}

void TextureHandler::bind(unsigned int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

int TextureHandler::getWidth() const
{
    return width;
}

int TextureHandler::getHeight() const
{
    return height;
}

class Texture::Impl
{
public:
    std::shared_ptr<TextureHandler> textureHandler;
};

Texture::Texture()
    : impl{std::make_unique<Impl>()}
{
}

Texture::~Texture()
{
}

void Texture::fromFile(const std::string &fileName)
{
    impl->textureHandler = Resource::get<TextureHandler>(fileName);
}

void Texture::bind(unsigned int unit)
{
    impl->textureHandler->bind(unit);
}

int Texture::getWidth() const
{
    return impl->textureHandler->getWidth();
}

int Texture::getHeight() const
{
    return impl->textureHandler->getHeight();
}

}
