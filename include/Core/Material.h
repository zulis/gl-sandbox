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
		struct GeometryTexture
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
		};

		Shader* getShader();
		virtual void bind() = 0;
		virtual void unbind();
		virtual void updateUniforms(unsigned int geometryIndex) {}

		void setShaderValues(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model);

	protected:
		void setGeomMaterials(const std::vector<Material::GeometryMaterial>& geomMaterials);
		virtual void addTexture(const std::string& fileName = std::string(), const TextureType& textureType = TextureType::DiffuseColor, unsigned int geometryIndex = 0);
		void bindGeomMaterial(unsigned int geometryIndex);
		bool bindTexture(const TextureType& textureType, unsigned int textureunit = 0, unsigned int geometryIndex = 0);
		virtual void setAmbientColor(float r, float g, float b);
		virtual void setDiffuseColor(float r, float g, float b);
		virtual void setSpecularColor(float r, float g, float b);
		virtual void setShininess(float shininess);
		virtual void addLight(const Light& light);

		Material(const std::string& fileName);
		virtual ~Material();

		ShaderRef mShader;

		std::vector<Material::GeometryMaterial> mGeometriesMaterials;

		glm::vec3 mMaterialKd { glm::vec4(-1.0f) };
		glm::vec3 mMaterialKa { glm::vec4(-1.0f) };
		glm::vec3 mMaterialKs { glm::vec4(-1.0f) };
		float mMaterialShininess { -1.0f };

		std::vector<Light> mLights;
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
void Material::unbind()
{
	if(mShader)
		mShader->unbind();
}

//=========================================================================
Shader* Material::getShader()
{
	return mShader.get();
}

//=========================================================================
void Material::setGeomMaterials(const std::vector<Material::GeometryMaterial>& geomMaterials)
{
	mGeometriesMaterials = geomMaterials;
}

//=========================================================================
void Material::addTexture(const std::string& fileName, const TextureType& textureType, unsigned int geometryIndex)
{
	GeometryMaterial geomMaterial;
	geomMaterial.geometryIndex = geometryIndex;

	auto it1 = std::find(mGeometriesMaterials.begin(), mGeometriesMaterials.end(), geomMaterial);

	if(it1 != mGeometriesMaterials.end())
	{
		geomMaterial = (*it1);

		GeometryTexture textureInfo;
		textureInfo.textureType = textureType;
		textureInfo.fileName = fileName;

		auto it2 = std::find(geomMaterial.textures.begin(), geomMaterial.textures.end(), textureInfo);

		if(it2 != geomMaterial.textures.end())
		{
			(*it2).texture.reset();
			geomMaterial.textures.erase(it2);
		}

		textureInfo.texture = Texture::create(fileName);
		geomMaterial.textures.push_back(textureInfo);
	}
	else
	{
		GeometryTexture textureInfo;
		textureInfo.textureType = textureType;
		textureInfo.fileName = fileName;
		textureInfo.texture = Texture::create(fileName);
		geomMaterial.textures.push_back(textureInfo);
		mGeometriesMaterials.push_back(geomMaterial);
	}
}

//=========================================================================
void Material::bindGeomMaterial(unsigned int geometryIndex)
{
	GeometryMaterial geomMaterial;
	geomMaterial.geometryIndex = geometryIndex;

	auto it = std::find(mGeometriesMaterials.begin(), mGeometriesMaterials.end(), geomMaterial);

	if(it != mGeometriesMaterials.end())
	{
		geomMaterial = (*it);

		if(mShader->hasUniform(ShaderConstants::MaterialKd))
			mShader->setUniform(ShaderConstants::MaterialKd, mMaterialKd == glm::vec3(-1.0f) ? geomMaterial.Kd : mMaterialKd);

		if(mShader->hasUniform(ShaderConstants::MaterialKa))
			mShader->setUniform(ShaderConstants::MaterialKa, mMaterialKa == glm::vec3(-1.0f) ? geomMaterial.Ka : mMaterialKa);

		if(mShader->hasUniform(ShaderConstants::MaterialKs))
			mShader->setUniform(ShaderConstants::MaterialKs, mMaterialKs == glm::vec3(-1.0f) ? geomMaterial.Ks : mMaterialKs);

		if(mShader->hasUniform(ShaderConstants::MaterialShininess))
			mShader->setUniform(ShaderConstants::MaterialShininess, mMaterialShininess == -1.0f ? geomMaterial.Shininess : mMaterialShininess);
	}
}

//=========================================================================
bool Material::bindTexture(const TextureType& textureType, unsigned int textureunit, unsigned int geometryIndex)
{
	GeometryMaterial geomMaterial;
	geomMaterial.geometryIndex = geometryIndex;

	auto it1 = std::find(mGeometriesMaterials.begin(), mGeometriesMaterials.end(), geomMaterial);

	if(it1 != mGeometriesMaterials.end())
	{
		geomMaterial = (*it1);

		GeometryTexture textureInfo;
		textureInfo.textureType = textureType;

		auto it2 = std::find(geomMaterial.textures.begin(), geomMaterial.textures.end(), textureInfo);

		if(it2 != geomMaterial.textures.end())
		{
			(*it2).texture->bind(textureunit);
			return true;
		}
	}

	return false;
}

//=========================================================================
void Material::setAmbientColor(float r, float g, float b)
{
	mMaterialKa = glm::vec3(r, g, b);
}

//=========================================================================
void Material::setDiffuseColor(float r, float g, float b)
{
	mMaterialKd = glm::vec3(r, g, b);
}

//=========================================================================
void Material::setSpecularColor(float r, float g, float b)
{
	mMaterialKs = glm::vec3(r, g, b);
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
void Material::setShaderValues(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
{
	if (mShader->hasUniform(ShaderConstants::ProjectionMatrix))
		mShader->setUniform(ShaderConstants::ProjectionMatrix, projection);

	if (mShader->hasUniform(ShaderConstants::ViewMatrix))
		mShader->setUniform(ShaderConstants::ViewMatrix, view);

	if (mShader->hasUniform(ShaderConstants::ModelMatrix))
		mShader->setUniform(ShaderConstants::ModelMatrix, model);

	if (mShader->hasUniform(ShaderConstants::ModelViewMatrix))
		mShader->setUniform(ShaderConstants::ModelViewMatrix, view * model);

	if (mShader->hasUniform(ShaderConstants::MVP))
		mShader->setUniform(ShaderConstants::MVP, projection * view * model);

	if (mShader->hasUniform(ShaderConstants::NormalMatrix))
	{
		auto mv = view * model;
		mShader->setUniform(ShaderConstants::NormalMatrix, glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
	}

	/*if (mShader->hasUniform(Shader::Kd))
	mShader->setUniform(Shader::Kd, mMaterial->getDiffuse());

	if (mShader->hasUniform(Shader::Ka))
	mShader->setUniform(Shader::Ka, mMaterial->getAmbient());

	if (mShader->hasUniform(Shader::Ks))
	mShader->setUniform(Shader::Ks, mMaterial->getSpecular());

	if (mShader->hasUniform(Shader::Shininess))
	mShader->setUniform(Shader::Shininess, mMaterial->getShininess());*/
}
