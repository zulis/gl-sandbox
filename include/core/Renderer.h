#pragma once

#include <vector>
#include <cassert>
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

	TextureID addTexture(const std::string& fileName = std::string(), const Texture::Format& format = Texture::Format());
	TextureID addTexture(const Color& color, const Texture::Format& format = Texture::Format());
	void setTexture(const TextureID texureID, int textureUnit = 0);

	ShaderID addShader(const std::string& fileName);
	void setShader(const ShaderID shaderID);
	//void setShaderUniform(const std::string &name, const Texture* tex);
	//void setShaderUniform(const std::string &name, const TextureCube* tex);
	void setShaderUniform(const std::string& name, int i);
	void setShaderUniform(const std::string& name, unsigned int i);
	void setShaderUniform(const std::string& name, float f);
	void setShaderUniform(const std::string& name, float f1, float f2);
	void setShaderUniform(const std::string& name, const vec2& vec2);
	void setShaderUniform(const std::string& name, float f1, float f2, float f3);
	void setShaderUniform(const std::string& name, float f1, float f2, float f3, float f4);
	void setShaderUniform(const std::string& name, const vec3& vec3);
	//void setShaderUniform(const std::string &name, const vec4& vec4);
	void setShaderUniform(const std::string& name, const mat3& mat3, bool transpose = false);
	void setShaderUniform(const std::string& name, const mat4& mat4, bool transpose = false);
	void setShaderUniform(const std::string& name, bool b);
	void setShaderUniform(const std::string& name, const vec4* vec4, int count);
	void setShaderUniform(const std::string& name, const vec4& vec4);
	void setShaderUniform(const std::string& name, const Color& color);

	void setCearColor(const Color& color);

	void reset();

	const Shader& getShader(const ShaderID shader) const
	{
		return *mShaders[shader];
	}


private:
	std::vector<Texture*> mTextures;
	std::vector<Shader*> mShaders;
	Color mClearColor;

	ShaderID mSelectedShader;

};

//=========================================================================
Renderer::Renderer()
{
	mClearColor = Color::black();
	mSelectedShader = SHADER_NONE;
}

//=========================================================================
Renderer::~Renderer()
{
	reset();

	// Cleanup textures
	for(const Texture* texture : mTextures)
		delete texture;

	mTextures.clear();

	// Cleanup shaders
	//for (const Shader *shader : mShaders)
	//delete shader;

	mShaders.clear();
}

//=========================================================================
TextureID Renderer::addTexture(const std::string& fileName, const Texture::Format& format)
{
	Texture* texture = new Texture(fileName, format);
	mTextures.push_back(texture);
	return mTextures.size() - 1;
}

//=========================================================================
TextureID Renderer::addTexture(const Color& color, const Texture::Format& format)
{
	Texture* texture = new Texture(color, format);
	mTextures.push_back(texture);
	return mTextures.size() - 1;
}

//=========================================================================
void Renderer::setTexture(const TextureID texureID, int textureUnit)
{
	assert(mSelectedShader != SHADER_NONE);

	if(static_cast<size_t>(texureID) < mTextures.size())
		mTextures[texureID]->bind(textureUnit);
}

//=========================================================================
ShaderID Renderer::addShader(const std::string& fileName)
{
	auto shader = Resource::get<Shader>(fileName);
	mShaders.push_back(shader.get());
	return mShaders.size() - 1;
}

//=========================================================================
void Renderer::setShader(const ShaderID shaderID)
{
	if(static_cast<size_t>(shaderID) < mShaders.size())
	{
		mSelectedShader = shaderID;
		mShaders[mSelectedShader]->bind();
	}
	else
		mSelectedShader = SHADER_NONE;
}

//=========================================================================
//void Renderer::setShaderUniform(const std::string &name, const Texture* tex)
//{
//	glUniform1i(getUniform(name), tex->index);
//}

//=========================================================================
//void Renderer::setShaderUniform(const std::string &name, const TextureCube *tex)
//{
//	glUniform1i(getUniform(name), tex->index);
//}

//=========================================================================
void Renderer::setShaderUniform(const std::string& name, int i)
{
	assert(mSelectedShader != SHADER_NONE);
	mShaders[mSelectedShader]->setUniform(name, i);
}

//=========================================================================
void Renderer::setShaderUniform(const std::string& name, unsigned int i)
{
	assert(mSelectedShader != SHADER_NONE);
	mShaders[mSelectedShader]->setUniform(name, i);
}

//=========================================================================
void Renderer::setShaderUniform(const std::string& name, float f)
{
	assert(mSelectedShader != SHADER_NONE);
	mShaders[mSelectedShader]->setUniform(name, f);
}

//=========================================================================
void Renderer::setShaderUniform(const std::string& name, float f1, float f2)
{
	assert(mSelectedShader != SHADER_NONE);
	mShaders[mSelectedShader]->setUniform(name, f1, f2);
}

//=========================================================================
void Renderer::setShaderUniform(const std::string& name, const vec2& vec2)
{
	assert(mSelectedShader != SHADER_NONE);
	mShaders[mSelectedShader]->setUniform(name, vec2);
}

//=========================================================================
void Renderer::setShaderUniform(const std::string& name, float f1, float f2, float f3)
{
	assert(mSelectedShader != SHADER_NONE);
	mShaders[mSelectedShader]->setUniform(name, f1, f2, f3);
}

//=========================================================================
void Renderer::setShaderUniform(const std::string& name, float f1, float f2, float f3, float f4)
{
	assert(mSelectedShader != SHADER_NONE);
	mShaders[mSelectedShader]->setUniform(name, f1, f2, f3, f4);
}

//=========================================================================
void Renderer::setShaderUniform(const std::string& name, const vec3& vec3)
{
	assert(mSelectedShader != SHADER_NONE);
	mShaders[mSelectedShader]->setUniform(name, vec3);
}

//=========================================================================
//void Renderer::setShaderUniform(const std::string &name, const vec4& vec4)
//{
//	setShaderUniform(name, vec4.x, vec4.y, vec4.z, vec4.w);
//}

//=========================================================================
void Renderer::setShaderUniform(const std::string& name, const vec4* vec4, int count)
{
	assert(mSelectedShader != SHADER_NONE);
	mShaders[mSelectedShader]->setUniform(name, vec4, count);
}

//=========================================================================
void Renderer::setShaderUniform(const std::string& name, const vec4& vec4)
{
	assert(mSelectedShader != SHADER_NONE);
	mShaders[mSelectedShader]->setUniform(name, vec4);
}

//=========================================================================
void Renderer::setShaderUniform(const std::string& name, const mat3& mat3, bool transpose)
{
	assert(mSelectedShader != SHADER_NONE);
	mShaders[mSelectedShader]->setUniform(name, mat3, transpose);
}

//=========================================================================
void Renderer::setShaderUniform(const std::string& name, const mat4& mat4, bool transpose)
{
	assert(mSelectedShader != SHADER_NONE);
	mShaders[mSelectedShader]->setUniform(name, mat4, transpose);
}

//=========================================================================
void Renderer::setShaderUniform(const std::string& name, bool b)
{
	assert(mSelectedShader != SHADER_NONE);
	mShaders[mSelectedShader]->setUniform(name, b);
}

//=========================================================================
void Renderer::setShaderUniform(const std::string& name, const Color& color)
{
	assert(mSelectedShader != SHADER_NONE);
	mShaders[mSelectedShader]->setUniform(name, color);
}

//=========================================================================
void Renderer::setCearColor(const Color& color)
{
	mClearColor = color;
}

//=========================================================================
void Renderer::reset()
{
	if(mSelectedShader != SHADER_NONE)
	{
		mShaders[mSelectedShader]->unbind();
		mSelectedShader = SHADER_NONE;
	}

	for each(Texture* texture in mTextures)
	{
		texture->unbind();
	}
}
