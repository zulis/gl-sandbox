#pragma once

#include "Core/Material.h"

typedef std::shared_ptr<class DefaultMaterial> MaterialDefaultRef;

class DefaultMaterial : public Material
{
public:
	static MaterialDefaultRef create();
	DefaultMaterial();
	virtual ~DefaultMaterial();

private:
	TextureRef mDefaultDiffuse;
	TextureRef mDefaultNormal;
	TextureRef mDefaultHeight;
	TextureRef mDefaultSpecular;
	TextureRef mDefaultEmissive;
	TextureRef mDefaultOpacity;

	//std::string mDefaultDiffuseName;
	//std::string mDefaultNormalName;
	//std::string mDefaultHeightName;
	//std::string mDefaultSpecularName;

	virtual void updateUniforms(unsigned int geometryIndex);
};

//=========================================================================
MaterialDefaultRef DefaultMaterial::create()
{
	return MaterialDefaultRef(new DefaultMaterial);
}

//=========================================================================
DefaultMaterial::DefaultMaterial() : Material(/*"assets/shaders/textured"*/"assets/shaders/defaultMaterial")
{
	//mDefaultDiffuseName = "assets/textures/default/default_d.png";
	//mDefaultDiffuseName = "assets/textures/default/UV_mapper.jpg";
	//mDefaultNormalName = "assets/textures/default/default_n.png";
	//mDefaultSpecularName = "assets/textures/default/default_s.png";

	//mDefaultDiffuse = Texture::create("assets/textures/default/default_d.png");
	//mDefaultNormal = Texture::create("assets/textures/default/default_n.png");
	//mDefaultSpecular = Texture::create("assets/textures/default/default_s.png");

	mDefaultDiffuse = Texture::create(Color::white());
	mDefaultNormal = Texture::create(Color::normal());
	mDefaultHeight = Texture::create(Color::white());
	mDefaultSpecular = Texture::create(Color::white());
	mDefaultEmissive = Texture::create(Color::white());
	mDefaultOpacity = Texture::create(Color::white());

}

//=========================================================================
DefaultMaterial::~DefaultMaterial()
{
}

//=========================================================================
void DefaultMaterial::updateUniforms(unsigned int geometryIndex)
{
	if(!bindTexture(TextureType::DiffuseMap, 0, geometryIndex))
	{
		addTexture(mDefaultDiffuse, TextureType::DiffuseMap, geometryIndex);
		bindTexture(TextureType::DiffuseMap, 0, geometryIndex);
	}

	if(!bindTexture(TextureType::NormalMap, 1, geometryIndex))
	{
		addTexture(mDefaultNormal, TextureType::NormalMap, geometryIndex);
		bindTexture(TextureType::NormalMap, 1, geometryIndex);
	}

	if (!bindTexture(TextureType::HeightMap, 2, geometryIndex))
	{
		addTexture(mDefaultHeight, TextureType::HeightMap, geometryIndex);
		bindTexture(TextureType::HeightMap, 2, geometryIndex);
	}

	if(!bindTexture(TextureType::SpecularMap, 3, geometryIndex))
	{
		addTexture(mDefaultSpecular, TextureType::SpecularMap, geometryIndex);
		bindTexture(TextureType::SpecularMap, 3, geometryIndex);
	}

	if (!bindTexture(TextureType::EmissiveMap, 4, geometryIndex))
	{
		addTexture(mDefaultEmissive, TextureType::EmissiveMap, geometryIndex);
		bindTexture(TextureType::EmissiveMap, 4, geometryIndex);
	}

	if (!bindTexture(TextureType::OpacityMap, 5, geometryIndex))
	{
		addTexture(mDefaultOpacity, TextureType::OpacityMap, geometryIndex);
		bindTexture(TextureType::OpacityMap, 5, geometryIndex);
	}
}
