#pragma once

#include "Core/Material.h"

typedef std::shared_ptr<class MaterialBasic> MaterialBasicRef;

class MaterialBasic : public Material
{
public:
	static MaterialBasicRef create();

	MaterialBasic();
	virtual ~MaterialBasic();

protected:
	virtual void bind();
};

//=========================================================================
MaterialBasicRef MaterialBasic::create()
{
	return MaterialBasicRef(new MaterialBasic);
}

//=========================================================================
MaterialBasic::MaterialBasic() : Material("assets/shaders/basic")
{
}

//=========================================================================
MaterialBasic::~MaterialBasic()
{
}

//=========================================================================
void MaterialBasic::bind()
{
	mShader->bind();
}