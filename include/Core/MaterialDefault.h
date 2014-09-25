#pragma once

#include "Core/Material.h"

typedef std::shared_ptr<class MaterialDefault> MaterialDefaultRef;

class MaterialDefault : public Material
{
	public:
		static MaterialDefaultRef create();
		MaterialDefault();
		virtual ~MaterialDefault();

	private:
		TextureRef mDefaultDiffuse;
		TextureRef mDefaultNormal;
		TextureRef mDefaultSpecular;

		virtual void updateUniforms(unsigned int geometryIndex);
};

//=========================================================================
MaterialDefaultRef MaterialDefault::create()
{
	return MaterialDefaultRef(new MaterialDefault);
}

//=========================================================================
MaterialDefault::MaterialDefault() : Material("assets/shaders/materialDefault")
{
	mDefaultDiffuse = Texture::create("assets/textures/misc/white.png");
	mDefaultNormal = Texture::create("assets/textures/misc/normal.png");
	mDefaultSpecular = Texture::create("assets/textures/misc/white.png");
}

//=========================================================================
MaterialDefault::~MaterialDefault()
{
}

//=========================================================================
void MaterialDefault::updateUniforms(unsigned int geometryIndex)
{
	if (!bindTexture(TextureType::DiffuseMap, 0, geometryIndex))
		mDefaultDiffuse->bind(0);

	if (!bindTexture(TextureType::NormalMap, 1, geometryIndex))
		mDefaultNormal->bind(1);

	if (!bindTexture(TextureType::SpecularMap, 2, geometryIndex))
		mDefaultSpecular->bind(2);
}
