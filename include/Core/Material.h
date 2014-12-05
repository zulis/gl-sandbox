#pragma once

#include <memory>
#include <algorithm>
#include <string>
#include "core/Shader.h"
#include "core/Texture.h"
#include "core/Light.h"
#include "core/TextureType.h"
#include "core/Color.h"
#include "core/FileMonitor.h"
#include "core/Listeners.h"

typedef std::shared_ptr<class Material> MaterialRef;

//=========================================================================
class Material : public FileMonitorListener
{
public:
	Shader* getShader();
	void addTexture(const TextureRef& texture, const TextureType& textureType = TextureType::DiffuseMap, unsigned int geometryIndex = 0);
	void addTexture(const std::string& fileName = std::string(), const TextureType& textureType = TextureType::DiffuseMap, unsigned int geometryIndex = 0);
	void addLight(const Light& light);
	Light& getLight(unsigned int index);

	void setAmbientColor(const Color& color);
	void setDiffuseColor(const Color& color);
	void setSpecularColor(const Color& color);
	void setShininess(float shininess);

	void setTilingU(float value);
	void setTilingV(float value);
	void setTilingUV(float value);

	virtual void bind();
	virtual void unbind();
	virtual void updateUniforms(unsigned int geometryIndex) {}

protected:
	Material(const std::string& fileName);
	virtual ~Material();
	bool bindTexture(const TextureType& textureType, unsigned int textureUnit = 0, unsigned int geometryIndex = 0);
	virtual void onFileMonitorFileChange(const std::string& fileName);

private:
	struct MaterialTexture
	{
		unsigned int geometryIndex;
		TextureType textureType;
		TextureRef texture;

		bool operator==(const MaterialTexture& mt) const
		{
			return mt.geometryIndex == geometryIndex && mt.textureType == textureType;
		}
	};

	bool mReloadShader { false };

	std::vector<MaterialTexture> mTextures;

	FileMonitorRef mFileMonitor;

	ShaderRef mShader;

	Color mMaterialAmbient { Color(0.3f, 0.3f, 0.3f, 1.0f) };
	Color mMaterialDiffuse { Color(0.7f, 0.7f, 0.7f, 1.0f) };
	Color mMaterialSpecular { Color(0.5f, 0.5f, 0.5f, 1.0f) };
	float mMaterialShininess { 60.0f };

	float mTilingU { 1.0 };
	float mTilingV { 1.0 };

	std::vector<Light> mLights;
	unsigned int mLightIndex { 0 };

	std::string mShaderFileName;
};

//=========================================================================
Material::Material(const std::string& fileName)
{
	mShaderFileName = fileName;
	mShader = Shader::create(fileName);

	mFileMonitor = FileMonitor::create(fileName + ".vert", fileName + ".frag");
	mFileMonitor->addListener(this);
}

//=========================================================================
Material::~Material()
{
}

//=========================================================================
Shader* Material::getShader()
{
	return mShader.get();
}

//=========================================================================
void Material::bind()
{
	if(mReloadShader)
	{
		mShader.reset();
		mShader = Shader::create(mShaderFileName);
		mReloadShader = false;
	}

	if(mShader)
	{
		mShader->bind();

		// Set Material
		if(mShader->hasUniform(ShaderConstants::MaterialAmbient))
			mShader->setUniform(ShaderConstants::MaterialAmbient, mMaterialAmbient);
		if(mShader->hasUniform(ShaderConstants::MaterialDiffuse))
			mShader->setUniform(ShaderConstants::MaterialDiffuse, mMaterialDiffuse);
		if(mShader->hasUniform(ShaderConstants::MaterialSpecular))
			mShader->setUniform(ShaderConstants::MaterialSpecular, mMaterialSpecular);
		if(mShader->hasUniform(ShaderConstants::MaterialShininess))
			mShader->setUniform(ShaderConstants::MaterialShininess, mMaterialShininess);

		if(mShader->hasUniform(ShaderConstants::TilingU))
			mShader->setUniform(ShaderConstants::TilingU, mTilingU);
		if(mShader->hasUniform(ShaderConstants::TilingV))
			mShader->setUniform(ShaderConstants::TilingV, mTilingV);
		if(mShader->hasUniform(ShaderConstants::TilingUV))
			mShader->setUniform(ShaderConstants::TilingUV, glm::vec2(mTilingU, mTilingV));

		// Update lights
		if(mShader->hasUniform(ShaderConstants::TotalLights))
			mShader->setUniform(ShaderConstants::TotalLights, mLights.size());

		unsigned int lightIndex = 0;
		for(auto light : mLights)
		{
			light.updateUniforms(mShader, lightIndex++);
		}
	}
}

//=========================================================================
void Material::unbind()
{
	if(mShader)
		mShader->unbind();
}

//=========================================================================
void Material::addTexture(const TextureRef& texture, const TextureType& textureType, unsigned int geometryIndex)
{
	if(textureType != TextureType::Unknown)
	{
		MaterialTexture matTexture;
		matTexture.geometryIndex = geometryIndex;
		matTexture.textureType = textureType;
		matTexture.texture = texture;

		auto it = std::find(mTextures.begin(), mTextures.end(), matTexture);

		if(it != mTextures.end())
			mTextures.erase(it);

		mTextures.push_back(matTexture);
	}
}

//=========================================================================
void Material::addTexture(const std::string& fileName, const TextureType& textureType, unsigned int geometryIndex)
{
	if(textureType != TextureType::Unknown)
	{
		MaterialTexture matTexture;
		matTexture.geometryIndex = geometryIndex;
		matTexture.textureType = textureType;
		matTexture.texture = Texture::create(fileName);

		auto it = std::find(mTextures.begin(), mTextures.end(), matTexture);

		if(it != mTextures.end())
			mTextures.erase(it);

		mTextures.push_back(matTexture);
	}
}

//=========================================================================
bool Material::bindTexture(const TextureType& textureType, unsigned int textureunit, unsigned int geometryIndex)
{
	auto bindResult = false;

	MaterialTexture mt;
	mt.geometryIndex = geometryIndex;
	mt.textureType = textureType;

	auto it = std::find(mTextures.begin(), mTextures.end(), mt);

	if(it != mTextures.end())
	{
		bindResult = true;
		(*it).texture->bind(textureunit);
	}

	switch(textureType)
	{
		case TextureType::DiffuseMap:
			if(mShader->hasUniform(ShaderConstants::DiffuseMapIsUsed))
				mShader->setUniform(ShaderConstants::DiffuseMapIsUsed, bindResult);
			break;
		case TextureType::NormalMap:
			if(mShader->hasUniform(ShaderConstants::NormalMapIsUsed))
				mShader->setUniform(ShaderConstants::NormalMapIsUsed, bindResult);
			break;
		case TextureType::HeightMap:
			if (mShader->hasUniform(ShaderConstants::HeightMapIsUsed))
				mShader->setUniform(ShaderConstants::HeightMapIsUsed, bindResult);
			break;
		case TextureType::SpecularMap:
			if(mShader->hasUniform(ShaderConstants::SpecularMapIsUsed))
				mShader->setUniform(ShaderConstants::SpecularMapIsUsed, bindResult);
			break;
		case TextureType::EmissiveMap:
			if (mShader->hasUniform(ShaderConstants::EmissiveMapIsUsed))
				mShader->setUniform(ShaderConstants::EmissiveMapIsUsed, bindResult);
			break;
		case TextureType::OpacityMap:
			if(mShader->hasUniform(ShaderConstants::OpacityMapIsUsed))
				mShader->setUniform(ShaderConstants::OpacityMapIsUsed, bindResult);
			break;
	}

	return bindResult;
}

//=========================================================================
void Material::setAmbientColor(const Color& color)
{
	mMaterialAmbient = color;
}

//=========================================================================
void Material::setDiffuseColor(const Color& color)
{
	mMaterialDiffuse = color;
}

//=========================================================================
void Material::setSpecularColor(const Color& color)
{
	mMaterialSpecular = color;
}

//=========================================================================
void Material::setShininess(float shininess)
{
	mMaterialShininess = shininess;
}

//=========================================================================
void Material::addLight(const Light& light)
{
	mLights.push_back(light);
}

//=========================================================================
Light& Material::getLight(unsigned int index)
{
	return mLights.at(index);
}

//=========================================================================
void Material::setTilingU(float value)
{
	mTilingU = value;
}

//=========================================================================
void Material::setTilingV(float value)
{
	mTilingV = value;
}

//=========================================================================
void Material::setTilingUV(float value)
{
	setTilingU(value);
	setTilingV(value);
}

//=========================================================================
void Material::onFileMonitorFileChange(const std::string& fileName)
{
	logNote("File has changed: %s", fileName.c_str());
	logNote("Reloading shader: %s", mShaderFileName.c_str());
	mReloadShader = true;

	auto aaa = Shader::create(mShaderFileName);
}
