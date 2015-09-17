#pragma once

#include "core/BaseApp.h"

class Test : public BaseApp
{
public:
	Test();
	~Test();

	virtual void onInput(const Input &input);
	virtual void onUpdate(double deltaTime);
	virtual void onDraw();
	virtual void onResize(const unsigned int width, const unsigned int height);

private:
	TextureID mTexD;
	TextureID mTexN;
	TextureID mTexS;

	ShaderID mShader1;
	ShaderID mShader2;
	ShaderID mShader3;
};

//=========================================================================
Test::Test() : BaseApp(800, 600, WindowMode::Windowed)
{
	setTitle("Game!");
	renderer->setCearColor(Color::gray(0.5f));

	mTexD = renderer->addTexture("assets/textures/default/default_d.png");
	mTexN = renderer->addTexture("assets/textures/default/default_n.png");
	mTexS = renderer->addTexture("assets/textures/default/default_d.png");

	mShader1 = renderer->addShader("assets/shaders/basic.shd");
	//mShader2 = renderer->addShader("assets/shaders/basic.shd");
	//mShader3 = renderer->addShader("assets/shaders/basic.shd");
}

//=========================================================================
Test::~Test()
{
}

//=========================================================================
void Test::onInput(const Input &input)
{
	if (input.isKeyDown(KEY_ESCAPE))
		quit();
}

//=========================================================================
void Test::onUpdate(double deltaTime)
{
}

//=========================================================================
void Test::onDraw()
{
	renderer->setTexture(3, 0);
	renderer->setTexture(mTexD, 0);
	renderer->setTexture(mTexN, 1);
	renderer->setTexture(mTexS, 2);

	renderer->setShader(mShader1);
}

//=========================================================================
void Test::onResize(const unsigned int width, const unsigned int height)
{
}
