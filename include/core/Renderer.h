#pragma once

#include <vector>
#include "core/Texture.h"
#include "core/Shader.h"

typedef int TextureID;
typedef int ShaderID;

#define TEXTURE_NONE (-1)
#define SHADER_NONE  (-1)

class Renderer
{
public:
	Renderer();
	~Renderer();

	TextureID addTexture(const char *fileName = NULL, const Texture::Format& format = Texture::Format());
	TextureID addTexture(const Color &color, const Texture::Format &format = Texture::Format());
	void setTexture(const TextureID texureID, int textureUnit = 0);

	ShaderID addShader(const char *fileName);
	void setShader(const ShaderID shaderID);

private:
	std::vector<Texture*> mTextures;
	std::vector<Shader*> mShaders;

};

//=========================================================================
Renderer::Renderer()
{
}

//=========================================================================
Renderer::~Renderer()
{
	// Cleanup textures
	for (const Texture *texture : mTextures)
		delete texture;

	mTextures.clear();

	// Cleanup shaders
	for (const Shader *shader : mShaders)
		delete shader;

	mShaders.clear();
}

//=========================================================================
TextureID Renderer::addTexture(const char *fileName, const Texture::Format& format)
{
	Texture *texture = new Texture(fileName, format);
	mTextures.push_back(texture);
	return mTextures.size() - 1;
}

//=========================================================================
TextureID Renderer::addTexture(const Color &color, const Texture::Format &format)
{
	Texture *texture = new Texture(color, format);
	mTextures.push_back(texture);
	return mTextures.size() - 1;
}

//=========================================================================
void Renderer::setTexture(const TextureID texureID, int textureUnit)
{
	if (texureID < mTextures.size())
		mTextures[texureID]->bind(textureUnit);
}

//=========================================================================
ShaderID Renderer::addShader(const char *fileName)
{
	Shader *shader = new Shader(fileName);
	mShaders.push_back(shader);
	return mShaders.size() - 1;
}

//=========================================================================
void Renderer::setShader(const ShaderID shaderID)
{
	if (shaderID < mShaders.size())
		mShaders[shaderID]->bind();
}
