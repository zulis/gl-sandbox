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

	std::string mDefaultDiffuseName;
	std::string mDefaultNormalName;
	std::string mDefaultHeightName;
	std::string mDefaultSpecularName;

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
	mDefaultDiffuseName = "assets/textures/default/UV_mapper.jpg";
	mDefaultNormalName = "assets/textures/default/default_n.png";
	mDefaultSpecularName = "assets/textures/default/default_s.png";

	//mDefaultDiffuse = Texture::create("assets/textures/default/default_d.png");
	//mDefaultNormal = Texture::create("assets/textures/default/default_n.png");
	//mDefaultSpecular = Texture::create("assets/textures/default/default_s.png");

	mDefaultDiffuse = Texture::create(Color::white());
	//mDefaultNormal = Texture::create(Color::normal());
	//mDefaultHeight = Texture::create(Color::white());
	mDefaultSpecular = Texture::create(Color::white());

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
		addTexture(mDefaultDiffuseName, TextureType::DiffuseMap, geometryIndex);
		bindTexture(TextureType::DiffuseMap, 0, geometryIndex);
		//mDefaultDiffuse->bind(0);
	}


	if(!bindTexture(TextureType::NormalMap, 1, geometryIndex))
	{
		addTexture(mDefaultNormalName, TextureType::NormalMap, geometryIndex);
		bindTexture(TextureType::NormalMap, 1, geometryIndex);
	}
	//mDefaultNormal->bind(1);

	bindTexture(TextureType::HeightMap, 2, geometryIndex);
	//mDefaultHeight->bind(2);

	if(!bindTexture(TextureType::SpecularMap, 3, geometryIndex))
	{
		//addTexture(mDefaultSpecularName, TextureType::SpecularMap, geometryIndex);
		//bindTexture(TextureType::SpecularMap, 3, geometryIndex);
		mDefaultSpecular->bind(3);
		getShader()->setUniform(ShaderConstants::SpecularMapIsUsed, true);
	}

	bindTexture(TextureType::OpacityMap, 4, geometryIndex);
}
