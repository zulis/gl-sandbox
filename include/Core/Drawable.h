#pragma once

#include <vector>
#include <map>
#include "Core/Math.h"
#include "Core/Geometry.h"
#include "Core/AABB.h"
#include "Core/Camera.h"
#include "Core/Material.h"
#include "Core/KeyEvent.h"
#include "Core/MouseEvent.h"

class Drawable
{
	public:
		Drawable();
		virtual ~Drawable();

		AABB getAABB();
		void setMaterial(const MaterialRef& material);
		unsigned int getGeometryCount();
		const Geometry* getGeometry(int index);
		void setFrustumCulling(bool isOn);

protected:
		virtual void setGeometry(std::vector<GeometryRef>* geometries) = 0;
		void initialize();
		virtual void draw(const Camera& camera, const glm::mat4& model);
		virtual void draw(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model);

		virtual void input(const KeyEvent& keyEvent, const MouseEvent& mouseEvent) {};
		virtual void update(float elapsedTime) {};
		
		std::vector<GeometryRef>* mGeometries { nullptr };
		
		MaterialRef mMaterial { nullptr };

	private:
		bool mInitialized { false };
		bool mAABBIsBuilt { false };
		std::map<GeometryRef, AABB> mAABBMap;
		AABB mAABB;
		bool mCullingIsOn { false };

		void setShaderValues(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model);
};

//=========================================================================
Drawable::Drawable()
{
	//mMaterial = MaterialMultiTextured::create();
	mInitialized = false;
}

//=========================================================================
Drawable::~Drawable()
{
	if(mGeometries)
		delete mGeometries;
}

//=========================================================================
void Drawable::initialize()
{
	if(!mInitialized)
	{
		mGeometries = new std::vector<GeometryRef>();
		setGeometry(mGeometries);

		// Collect AABB's
		for(auto& geometry : *mGeometries)
		{
			mAABBMap[geometry] = geometry->getAABB();
		}

		mInitialized = true;
	}
}

//=========================================================================
void Drawable::setShaderValues(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
{
	if (mMaterial->getShader()->hasUniform(ShaderConstants::ProjectionMatrix))
		mMaterial->getShader()->setUniform(ShaderConstants::ProjectionMatrix, projection);

	if (mMaterial->getShader()->hasUniform(ShaderConstants::ViewMatrix))
		mMaterial->getShader()->setUniform(ShaderConstants::ViewMatrix, view);

	if (mMaterial->getShader()->hasUniform(ShaderConstants::ModelMatrix))
		mMaterial->getShader()->setUniform(ShaderConstants::ModelMatrix, model);

	if (mMaterial->getShader()->hasUniform(ShaderConstants::ModelViewMatrix))
		mMaterial->getShader()->setUniform(ShaderConstants::ModelViewMatrix, view * model);

	if (mMaterial->getShader()->hasUniform(ShaderConstants::MVP))
		mMaterial->getShader()->setUniform(ShaderConstants::MVP, projection * view * model);

	if (mMaterial->getShader()->hasUniform(ShaderConstants::NormalMatrix))
	{
		auto mv = view * model;
		mMaterial->getShader()->setUniform(ShaderConstants::NormalMatrix, glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
	}

	/*if (mMaterial->getShader()->hasUniform(Shader::Kd))
		mMaterial->getShader()->setUniform(Shader::Kd, mMaterial->getDiffuse());

		if (mMaterial->getShader()->hasUniform(Shader::Ka))
		mMaterial->getShader()->setUniform(Shader::Ka, mMaterial->getAmbient());

		if (mMaterial->getShader()->hasUniform(Shader::Ks))
		mMaterial->getShader()->setUniform(Shader::Ks, mMaterial->getSpecular());

		if (mMaterial->getShader()->hasUniform(Shader::Shininess))
		mMaterial->getShader()->setUniform(Shader::Shininess, mMaterial->getShininess());*/
}

//=========================================================================
void Drawable::draw(const Camera& camera, const glm::mat4& model)
{
	initialize();

	mMaterial->bind();

	setShaderValues(camera.getProjectionMatrix(), camera.getViewMatrix(), model);

	unsigned int geometryIndex = 0;

	for(auto& geometry : *mGeometries)
	{
		mMaterial->updateUniforms(geometryIndex);

		if (mCullingIsOn)
		{
			auto aabb = mAABBMap[geometry].transformed(model);
			auto mIsCulled = !camera.intersects(aabb);

			if (!mIsCulled)
			{
				geometry->draw(*mMaterial->getShader());
			}
		}
		else
		{
			geometry->draw(*mMaterial->getShader());
		}

		geometryIndex++;
	}

	mMaterial->unbind();
}

//=========================================================================
void Drawable::draw(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
{
	initialize();

	mMaterial->bind();

	setShaderValues(projection, view, model);

	unsigned int geometryIndex = 0;

	for(auto& geometry : *mGeometries)
	{
		mMaterial->updateUniforms(geometryIndex);
		geometry->draw(*mMaterial->getShader());
		geometryIndex++;
	}

	mMaterial->unbind();
}

//=========================================================================
AABB Drawable::getAABB()
{
	initialize();

	if(!mAABBIsBuilt)
	{
		auto minFloat = std::numeric_limits<float>::min();
		auto maxFloat = std::numeric_limits<float>::max();
		glm::vec3 min(maxFloat);
		glm::vec3 max(minFloat);

		for(auto& aabb : mAABBMap)
		{
			auto tmp = aabb.second;

			if(tmp.getMax().x > max.x)
				max.x = tmp.getMax().x;

			if(tmp.getMin().x < min.x)
				min.x = tmp.getMin().x;

			if(tmp.getMax().y > max.y)
				max.y = tmp.getMax().y;

			if(tmp.getMin().y < min.y)
				min.y = tmp.getMin().y;

			if(tmp.getMax().z > max.z)
				max.z = tmp.getMax().z;

			if(tmp.getMin().z < min.z)
				min.z = tmp.getMin().z;
		}

		mAABB = AABB(min, max);
		mAABBIsBuilt = true;
	}

	return mAABB;
}

//=========================================================================
unsigned int Drawable::getGeometryCount()
{
	initialize();
	return mGeometries->size();
}

//=========================================================================
const Geometry* Drawable::getGeometry(int index)
{
	initialize();
	return mGeometries[0][index].get();
}

//=========================================================================
void Drawable::setMaterial(const MaterialRef& material)
{
	mMaterial.reset();
	mMaterial = material;
}

//=========================================================================
void Drawable::setFrustumCulling(bool isOn)
{
	mCullingIsOn = isOn;
}