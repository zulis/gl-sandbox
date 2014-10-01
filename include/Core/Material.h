#pragma once

#include <memory>
#include <algorithm>
#include <string>
#include "Core/Shader.h"
#include "Core/Texture.h"
#include "Core/Light.h"
#include "Core/TextureType.h"

typedef std::shared_ptr<class Material> MaterialRef;

//=========================================================================
class Material
{
public:
	/*struct GeometryTexture
	{
	TextureType textureType;
	TextureRef texture;
	std::string fileName;

	bool operator==(const GeometryTexture& t) const
	{
	return t.textureType == textureType;
	}
	};

	struct GeometryMaterial
	{
	unsigned int geometryIndex;
	glm::vec3 Kd { glm::vec3(0.8f, 0.8f, 0.8f) };
	glm::vec3 Ka { glm::vec3(0.8f, 0.8f, 0.8f) };
	glm::vec3 Ks { glm::vec3(0.1f, 0.1f, 0.1f) };
	float Shininess { 0.5f };

	std::vector<GeometryTexture> textures;

	bool operator==(const GeometryMaterial& gm) const
	{
	return gm.geometryIndex == geometryIndex;
	}
	};*/

	Shader* getShader();
	void addTexture(const std::string& fileName = std::string(), const TextureType& textureType = TextureType::DiffuseMap, unsigned int geometryIndex = 0);
	void addLight(const Light& light);

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
	//virtual void setAmbientColor(float r, float g, float b);
	//virtual void setDiffuseColor(float r, float g, float b);
	//virtual void setSpecularColor(float r, float g, float b);
	//virtual void setShininess(float shininess);


	std::vector<MaterialTexture> mTextures;



	ShaderRef mShader;

	//std::vector<Material::GeometryMaterial> mGeometriesMaterials;

	//glm::vec3 mMaterialKa{ glm::vec4(0.0f) };
	//glm::vec3 mMaterialKd { glm::vec4(0.0f) };
	//glm::vec3 mMaterialKs { glm::vec4(0.0f) };
	//float mMaterialShininess { 0.0f };

	std::vector<Light> mLights;
	unsigned int mLightIndex{ 0 };
};

//=========================================================================
Material::Material(const std::string& fileName)
{
	mShader = Shader::create(fileName);
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
	if (mShader)
	{
		mShader->bind();

		unsigned int lightIndex = 0;
		for each (auto light in mLights)
		{
			//light.updateUniforms(mShader, lightIndex++);
		}
	}
}

//=========================================================================
void Material::unbind()
{
	if (mShader)
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
	if (textureType != TextureType::Unknown)
	{
		MaterialTexture matTexture;
		matTexture.geometryIndex = geometryIndex;
		matTexture.textureType = textureType;
		matTexture.texture = Texture::create(fileName);

		auto it = std::find(mTextures.begin(), mTextures.end(), matTexture);

		if (it != mTextures.end())
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
	MaterialTexture mt;
	mt.geometryIndex = geometryIndex;
	mt.textureType = textureType;

	auto it = std::find(mTextures.begin(), mTextures.end(), mt);

	if (it != mTextures.end())
	{
		(*it).texture->bind(textureunit);

		switch (textureType)
		{
		case TextureType::DiffuseMap:
			if (mShader->hasUniform(ShaderConstants::DiffuseMapIsUsed))
				mShader->setUniform(ShaderConstants::DiffuseMapIsUsed, true);
			break;
		case TextureType::NormalMap:
			if (mShader->hasUniform(ShaderConstants::NormalMapIsUsed))
				mShader->setUniform(ShaderConstants::NormalMapIsUsed, true);
			break;
		case TextureType::SpecularMap:
			if (mShader->hasUniform(ShaderConstants::SpecularMapIsUsed))
				mShader->setUniform(ShaderConstants::SpecularMapIsUsed, true);
			break;
		case TextureType::HeightMap:
			if (mShader->hasUniform(ShaderConstants::HeightMapIsUsed))
				mShader->setUniform(ShaderConstants::HeightMapIsUsed, true);
			break;
		case TextureType::OpacityMap:
			if (mShader->hasUniform(ShaderConstants::OpacityMapIsUsed))
				mShader->setUniform(ShaderConstants::OpacityMapIsUsed, true);
			break;
		}

		return true;
	}

	return false;

	/*
	GeometryMaterial geomMaterial;
	geomMaterial.geometryIndex = geometryIndex;

	auto it1 = std::find(mGeometriesMaterials.begin(), mGeometriesMaterials.end(), geomMaterial);

	if (it1 != mGeometriesMaterials.end())
	{
	geomMaterial = (*it1);

	GeometryTexture textureInfo;
	textureInfo.textureType = textureType;

	auto it2 = std::find(geomMaterial.textures.begin(), geomMaterial.textures.end(), textureInfo);

	if (it2 != geomMaterial.textures.end())
	{
	(*it2).texture->bind(textureunit);
	return true;
	}
	}

	return false;
	*/
}

//=========================================================================
/*void Material::setAmbientColor(float r, float g, float b)
{
mMaterialKa = glm::vec3(r, g, b);
}*/

//=========================================================================
/*void Material::setDiffuseColor(float r, float g, float b)
{
mMaterialKd = glm::vec3(r, g, b);
}*/

//=========================================================================
/*void Material::setSpecularColor(float r, float g, float b)
{
mMaterialKs = glm::vec3(r, g, b);
}*/

//=========================================================================
/*void Material::setShininess(float shininess)
{
mMaterialShininess = shininess;
}*/

//=========================================================================
void Material::addLight(const Light& light)
{
	light.updateUniforms(mShader, mLightIndex);
	mLightIndex++;
	//mLights.push_back(light);
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
