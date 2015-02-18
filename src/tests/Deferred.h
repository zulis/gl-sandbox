#pragma once

#include "core/Game.h"
//#include "core/Camera.h"
//#include "core/Shader.h"
//#include "core/Shape.h"
//#include "core/Transform3d.h"
//#include "core/Texture.h"
#include "core/Resource.h"
#include <iostream>

class Image : public Shared<Image>
{
public:
	Image(const std::string& fileName, int sk) : mFileName(fileName)
	{
		std::cout << "create:" << mFileName << std::endl;
	}

	~Image()
	{
		std::cout << "release:" << mFileName << std::endl;
	}

	std::string mFileName;
};

class Deferred : public State
{
public:
	Deferred() {};
	~Deferred() {};

	virtual void setup();
	virtual void input(Input& input);
	virtual void update(double elapsedTime);
	virtual void draw();
	virtual void resize(unsigned int width, unsigned int height);

private:
	//CameraPtr mCamera;
	//float mStrafeSpeed{ 0.2f };
	//float mStrafeFastSpeed{ 0.4f };

	//ShaderRef mShader;
	//GeometryRef mShape;
	//Transform3D mTransform;
	//TextureRef mColorTex;
};

//=========================================================================
void Deferred::setup()
{
	{
		auto a1 = Resource::create<Image>("aaa", 1);
		auto a2 = Resource::create<Image>("aaa", 1);
		auto a3 = Resource::create<Image>("aaa", 1);
		std::cout << "--------------------" << std::endl;
		//auto b1 = Image::create("bbb", 1);
		//auto b2 = Image::create("bbb", 1);
		//auto b3 = Image::create("bbb", 1);
	}
	
	/*
	std::cout << "--------------------" << std::endl;

	auto b1 = std::make_unique<Abc>("bbb", 5);
	auto b2 = std::make_unique<Abc>("bbb", 5);
	auto b3 = std::make_unique<Abc>("bbb", 5);

	(*b1).mFileName = "pakeistas";
	std::cout << b2.get()->mFileName << std::endl;
	std::cout << b3.get()->mFileName << std::endl;*/

	int i;

	i = 0;
	/*mCamera = Camera::create();
	mCamera->setRotateSpeed(0.002f);
	mCamera->setStrafeSpeed(mStrafeSpeed);
	mCamera->setPosition(vec3(0, 0, -2));
	mCamera->setLookAt(vec3(0, 0, 0));

	mShader = Shader::create("assets/shaders/basic");
	mShape = Shape::createCube();
	mColorTex = Texture::create("assets/textures/default/default_d.png");*/
}

//=========================================================================
void Deferred::input(Input& input)
{
	if (input.isKeyDown(KEY_ESCAPE))
		quit();

	/*if (input.isKeyDown(KEY_LEFT_SHIFT))
		mCamera->setStrafeSpeed(mStrafeFastSpeed);
		else
		mCamera->setStrafeSpeed(mStrafeSpeed);

		if (input.isKeyDown(KEY_W))
		mCamera->move(Camera::FORWARD);
		else if (input.isKeyDown(KEY_S))
		mCamera->move(Camera::BACKWARD);

		if (input.isKeyDown(KEY_A))
		mCamera->move(Camera::LEFT);
		else if (input.isKeyDown(KEY_D))
		mCamera->move(Camera::RIGHT);

		if (input.isKeyDown(KEY_E))
		mCamera->move(Camera::UP);
		else if (input.isKeyDown(KEY_Q))
		mCamera->move(Camera::DOWN);

		if (input.getMouseScroolY() != 0)
		mCamera->move(input.getMouseScroolY() > 0 ? Camera::FORWARD : Camera::BACKWARD);

		if (input.isMouseDown(MouseButton::Right))
		{
		input.hideMouse();
		mCamera->rotate((float)input.getMouseChangeX(), (float)input.getMouseChangeY());
		}
		else
		{
		input.showMouse();
		}*/
}

//=========================================================================
void Deferred::update(double elapsedTime)
{
}

//=========================================================================
void Deferred::draw()
{
	gl::enableCullFace(gl::CullFaceType::Back);

	//mColorTex->bind();
	//mShader->bind();

	////mShader->setUniform(ShaderConstants::ProjectionMatrix, mCamera->getProjectionMatrix());
	////mShader->setUniform(ShaderConstants::ViewMatrix, mCamera->getViewMatrix());
	////mShader->setUniform(ShaderConstants::ModelMatrix, mTransform.getMatrix());
	////mShader->setUniform(ShaderConstants::ModelViewMatrix, mCamera->getViewMatrix() * mTransform.getMatrix());
	//mShader->setUniform(ShaderConstants::MVP, mCamera->getProjectionMatrix() * mCamera->getViewMatrix() * mTransform.getMatrix());
	//mShader->setUniform(ShaderConstants::ColorMapIsUsed, true);
	////mShader->setUniform(ShaderConstants::ColorMap, 0);

	////auto mv = mCamera->getViewMatrix() * mTransform.getMatrix();
	////mShader->setUniform(ShaderConstants::NormalMatrix, mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

	//mShape->draw(*mShader);
	//mShader->unbind();

	//mColorTex->unbind();

	gl::disableCullFace();
}

//=========================================================================
void Deferred::resize(unsigned int width, unsigned int height)
{
	//mCamera->setAspectRatio((float)width / height);
}