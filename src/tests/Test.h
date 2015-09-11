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

};

//=========================================================================
Test::Test() : BaseApp(800, 600, WindowMode::Windowed)
{
	setTitle("Game!");
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
}

//=========================================================================
void Test::onResize(const unsigned int width, const unsigned int height)
{
}
