#pragma once

#include "Core/Material.h"

typedef std::shared_ptr<class MaterialDefault> MaterialDefaultRef;

class MaterialDefault : public Material
{
	public:
		static MaterialDefaultRef create();
		MaterialDefault();
		virtual ~MaterialDefault();

	protected:
		virtual void bind();
		
};

//=========================================================================
MaterialDefaultRef MaterialDefault::create()
{
	return MaterialDefaultRef(new MaterialDefault);
}

//=========================================================================
MaterialDefault::MaterialDefault() : Material("assets/shaders/materialDefault")
{
	
}

//=========================================================================
MaterialDefault::~MaterialDefault()
{
}

//=========================================================================
void MaterialDefault::bind()
{
	mShader->bind();
}
