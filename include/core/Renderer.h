#pragma once

#include <vector>
#include "core/Texture.h"
#include "core/Shader.h"
#include "core/Color.h"
#include "core/Resource.h"

typedef int TextureID;
typedef int ShaderID;

#define TEXTURE_NONE (-1)
#define SHADER_NONE  (-1)

class Renderer
{
	friend class BaseApp;

public:
	Renderer();
	~Renderer();

	TextureID addTexture(const std::string &fileName = std::string(), const Texture::Format& format = Texture::Format());
	TextureID addTexture(const Color &color, const Texture::Format &format = Texture::Format());
	void setTexture(const TextureID texureID, int textureUnit = 0);

	ShaderID addShader(const std::string &fileName);
	void setShader(const ShaderID shaderID);

	void setCearColor(const Color &color);

private:
	std::vector<Texture*> mTextures;
	std::vector<Shader*> mShaders;
	Color mClearColor;

};

//=========================================================================
Renderer::Renderer()
{
	mClearColor = Color::black();
}

//=========================================================================
Renderer::~Renderer()
{
	// Cleanup textures
	for (const Texture *texture : mTextures)
		delete texture;

	mTextures.clear();

	// Cleanup shaders
	//for (const Shader *shader : mShaders)
		//delete shader;

	mShaders.clear();
}

//=========================================================================
TextureID Renderer::addTexture(const std::string &fileName, const Texture::Format& format)
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
ShaderID Renderer::addShader(const std::string &fileName)
{
	//Shader *shader = new Shader(fileName);
	auto shader = Resource::get<Shader>(fileName);
	mShaders.push_back(shader.get());
	return mShaders.size() - 1;
}

//=========================================================================
void Renderer::setShader(const ShaderID shaderID)
{
	if (shaderID < mShaders.size())
		mShaders[shaderID]->bind();
}

//=========================================================================
void Renderer::setCearColor(const Color &color)
{
	mClearColor = color;
}
