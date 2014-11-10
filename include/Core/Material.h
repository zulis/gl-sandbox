#pragma once

#include <memory>
#include <algorithm>
#include <string>
#include "Core/Shader.h"
#include "Core/Texture.h"
#include "Core/Light.h"
#include "Core/TextureType.h"
#include "Core/Color.h"
#include "Core/FileMonitor.h"

typedef std::shared_ptr<class Material> MaterialRef;

//=========================================================================
class Material
{
public:
	Shader* getShader();
	void addTexture(const std::string& fileName = std::string(), const TextureType& textureType = TextureType::DiffuseMap, unsigned int geometryIndex = 0);
	void addLight(const Light& light);

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

	/*struct ShaderValues
	{
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 model;
	MeshMaterial meshMaterial;
	};*/

	//void updateUniforms(const ShaderValues& shaderValues);

protected:
	Material(const std::string& fileName);
	virtual ~Material();
	bool bindTexture(const TextureType& textureType, unsigned int textureUnit = 0, unsigned int geometryIndex = 0);

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

	//void setGeomMaterials(const std::vector<Material::GeometryMaterial>& geomMaterials);

	//void bindGeomMaterial(unsigned int geometryIndex);
	//bool bindTexture(const TextureType& textureType, unsigned int textureunit = 0, unsigned int geometryIndex = 0);

	void reloadShader();

	std::vector<MaterialTexture> mTextures;



	ShaderRef mShader;

	//std::vector<Material::GeometryMaterial> mGeometriesMaterials;

	Color mMaterialAmbient { Color(0.3, 0.3, 0.3, 1.0) };
	Color mMaterialDiffuse { Color(0.7, 0.7, 0.7, 1.0) };
	Color mMaterialSpecular { Color(0.5, 0.5, 0.5, 1.0) };
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
	FileMonitor::create(fileName + ".frag", std::bind(&Material::reloadShader, this));
	FileMonitor::create(fileName + ".vert", std::bind(&Material::reloadShader, this));
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
/*void Material::setGeomMaterials(const std::vector<Material::GeometryMaterial>& geomMaterials)
{
mGeometriesMaterials = geomMaterials;
}*/

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
/*void Material::bindGeomMaterial(unsigned int geometryIndex)
{
GeometryMaterial geomMaterial;
geomMaterial.geometryIndex = geometryIndex;

auto it = std::find(mGeometriesMaterials.begin(), mGeometriesMaterials.end(), geomMaterial);

if(it != mGeometriesMaterials.end())
{
geomMaterial = (*it);

if(mShader->hasUniform(ShaderConstants::MaterialDiffuse))
mShader->setUniform(ShaderConstants::MaterialDiffuse, mMaterialKd == glm::vec3(-1.0f) ? geomMaterial.Kd : mMaterialKd);

if(mShader->hasUniform(ShaderConstants::MaterialAmbient))
mShader->setUniform(ShaderConstants::MaterialAmbient, mMaterialKa == glm::vec3(-1.0f) ? geomMaterial.Ka : mMaterialKa);

if(mShader->hasUniform(ShaderConstants::MaterialSpecular))
mShader->setUniform(ShaderConstants::MaterialSpecular, mMaterialKs == glm::vec3(-1.0f) ? geomMaterial.Ks : mMaterialKs);

if(mShader->hasUniform(ShaderConstants::MaterialShininess))
mShader->setUniform(ShaderConstants::MaterialShininess, mMaterialShininess == -1.0f ? geomMaterial.Shininess : mMaterialShininess);
}
}*/

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
		case TextureType::SpecularMap:
			if(mShader->hasUniform(ShaderConstants::SpecularMapIsUsed))
				mShader->setUniform(ShaderConstants::SpecularMapIsUsed, bindResult);
			break;
		case TextureType::HeightMap:
			if(mShader->hasUniform(ShaderConstants::HeightMapIsUsed))
				mShader->setUniform(ShaderConstants::HeightMapIsUsed, bindResult);
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
void Material::reloadShader()
{
	logNote("Reloading shader: ", mShaderFileName.c_str());

	//mShader.reset();
	//mShader = Shader::create(mShaderFileName);
}

//=========================================================================
/*void Material::updateUniforms(const ShaderValues& shaderValues)
{


if (mShader->hasUniform(ShaderConstants::MaterialAmbient))
mShader->setUniform(ShaderConstants::MaterialAmbient, shaderValues.meshMaterial.ambient.rgba());

if (mShader->hasUniform(ShaderConstants::MaterialDiffuse))
mShader->setUniform(ShaderConstants::MaterialDiffuse, shaderValues.meshMaterial.diffuse.rgba());

if (mShader->hasUniform(ShaderConstants::MaterialSpecular))
mShader->setUniform(ShaderConstants::MaterialSpecular, shaderValues.meshMaterial.specular.rgba());

if (mShader->hasUniform(ShaderConstants::MaterialShininess))
mShader->setUniform(ShaderConstants::MaterialShininess, shaderValues.meshMaterial.shininess);

if (mShader->hasUniform(ShaderConstants::TexDiffuse))
{
auto texture = shaderValues.meshMaterial.getTexture(TextureType::DiffuseMap);

if (texture != NULL)
{
auto pos = mShader->getUniform(ShaderConstants::TexDiffuse);
auto texID = (unsigned int)texture->getTextureID();

texture->bind(0);
mShader->setUniform(ShaderConstants::TexDiffuse, texID);
}
}

if (mShader->hasUniform(ShaderConstants::TexNormal))
{
auto texture = shaderValues.meshMaterial.getTexture(TextureType::NormalMap);

if (texture != NULL)
{
auto pos = mShader->getUniform(ShaderConstants::TexNormal);
auto texID = (unsigned int)texture->getTextureID();

texture->bind(1);
mShader->setUniform(ShaderConstants::TexNormal, texID);
}
}

if (mShader->hasUniform(ShaderConstants::TexSpecular))
{
auto texture = shaderValues.meshMaterial.getTexture(TextureType::SpecularMap);

if (texture != NULL)
{
auto pos = mShader->getUniform(ShaderConstants::TexSpecular);
auto texID = (unsigned int)texture->getTextureID();

texture->bind(2);
mShader->setUniform(ShaderConstants::TexSpecular, texID);
}
}
}*/
