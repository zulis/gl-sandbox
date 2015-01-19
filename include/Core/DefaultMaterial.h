#pragma once

#include "core/Material.h"

typedef std::unique_ptr<class DefaultMaterial> MaterialDefaultRef;

class DefaultMaterial final : public Material
{
public:
	static MaterialDefaultRef create();
	DefaultMaterial();
	~DefaultMaterial() {};

	virtual void updateUniforms(unsigned int geometryIndex);

private:
	TextureRef mDefaultDiffuse;
};

//=========================================================================
MaterialDefaultRef DefaultMaterial::create()
{
	//return MaterialDefaultRef(new DefaultMaterial);
	return std::make_unique<DefaultMaterial>();
}

//=========================================================================
DefaultMaterial::DefaultMaterial() : Material("assets/shaders/defaultMaterial")
{
	mDefaultDiffuse = Texture::create(Color::white());
}

//=========================================================================
void DefaultMaterial::updateUniforms(unsigned int geometryIndex)
{
	if(!bindTexture(TextureType::DiffuseMap, 0, geometryIndex))
	{
		addTexture(mDefaultDiffuse, TextureType::DiffuseMap, geometryIndex);
		bindTexture(TextureType::DiffuseMap, 0, geometryIndex);
	}

	bindTexture(TextureType::NormalMap, 1, geometryIndex);
	bindTexture(TextureType::HeightMap, 2, geometryIndex);
	bindTexture(TextureType::SpecularMap, 3, geometryIndex);
	bindTexture(TextureType::EmissiveMap, 4, geometryIndex);
	bindTexture(TextureType::OpacityMap, 5, geometryIndex);
}
