#pragma once

#include <map>
#include <vector>
#include "core/Material.h"
#include "core/Texture.h"

typedef std::shared_ptr<class MaterialPhong> MaterialPhongRef;

class MaterialPhong : public Material
{
	public:
		static MaterialPhongRef create();

		MaterialPhong();
		virtual ~MaterialPhong();

		virtual void addTexture(const std::string& fileName = std::string(), const TextureType& textureType = TextureType::DiffuseMap, unsigned int geometryIndex = 0);
		virtual void setGeomMaterials(const std::vector<Material::GeometryMaterial>& geomMaterials);
		virtual void setAmbientColor(float r, float g, float b);
		virtual void setDiffuseColor(float r, float g, float b);
		virtual void setSpecularColor(float r, float g, float b);
		virtual void setShininess(float shininess);
		virtual void addLight(const Light& light);

	protected:
		virtual void bind();
		virtual void unbind();
		virtual void updateUniforms(unsigned int geometryIndex);

	private:
		std::map<int, std::vector<TextureRef>> mGeometriesMaterials;
		TextureRef mDefaultDiffuse;
		TextureRef mDefaultNormal;
		TextureRef mDefaultSpecular;
};

//=========================================================================
MaterialPhongRef MaterialPhong::create()
{
	return MaterialPhongRef(new MaterialPhong);
}

//=========================================================================
MaterialPhong::MaterialPhong() : Material("assets/shaders/phong")
{
	mDefaultDiffuse = Texture::create();
	mDefaultNormal = Texture::create("assets/textures/misc/normal.png");
	mDefaultSpecular = Texture::create("assets/textures/misc/white.png");

}

//=========================================================================
MaterialPhong::~MaterialPhong()
{
}

//=========================================================================
void MaterialPhong::bind()
{
	mShader->bind();

	for (auto& light : mLights)
	{
		mShader->setUniform(ShaderConstants::LightPosition, light.position);
		mShader->setUniform(ShaderConstants::LightIntensity, light.intensity);
	}
}

//=========================================================================
void MaterialPhong::unbind()
{
	mShader->unbind();
}

//=========================================================================
void MaterialPhong::updateUniforms(unsigned int geometryIndex)
{
	Material::bindGeomMaterial(geometryIndex);

	if(!Material::bindTexture(TextureType::DiffuseMap, 0, geometryIndex))
		mDefaultDiffuse->bind(0);

	if(!Material::bindTexture(TextureType::NormalMap, 1, geometryIndex))
		mDefaultNormal->bind(1);

	if (!Material::bindTexture(TextureType::SpecularMap, 2, geometryIndex))
		mDefaultSpecular->bind(2);
}

//=========================================================================
void MaterialPhong::addTexture(const std::string& fileName, const TextureType& textureType, unsigned int geometryIndex)
{
	Material::addTexture(fileName, textureType, geometryIndex);
}

//=========================================================================
void MaterialPhong::setGeomMaterials(const std::vector<Material::GeometryMaterial>& geomMaterials)
{
	Material::setGeomMaterials(geomMaterials);
}

//=========================================================================
void MaterialPhong::setAmbientColor(float r, float g, float b)
{
	Material::setAmbientColor(r, g, b);
}

//=========================================================================
void MaterialPhong::setDiffuseColor(float r, float g, float b)
{

	Material::setDiffuseColor(r, g, b);
}

//=========================================================================
void MaterialPhong::setSpecularColor(float r, float g, float b)
{
	Material::setSpecularColor(r, g, b);
}

//=========================================================================
void MaterialPhong::setShininess(float shininess)
{
	Material::setShininess(shininess);
}

//=========================================================================
void MaterialPhong::addLight(const Light& light)
{
	Material::addLight(light);
}