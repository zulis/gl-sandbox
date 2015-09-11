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
};

//=========================================================================
Test::Test() : BaseApp(800, 600, WindowMode::Windowed)
{
	setTitle("Game!");
	mTexD = renderer->addTexture("assets/textures/default/default_d.png");
	mTexN = renderer->addTexture("assets/textures/default/default_n.png");
	mTexS = renderer->addTexture("assets/textures/default/default_s.png");

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
}

//=========================================================================
void Test::onResize(const unsigned int width, const unsigned int height)
{
}
