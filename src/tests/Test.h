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

	const float strafeSpeed{ 0.2f };
	const float strafeFastSpeed{ 0.4f };
};

//=========================================================================
Test::Test() : BaseApp(800, 600, WindowMode::Windowed)
{
	setTitle("Game!");
	renderer->setCearColor(Color::gray(0.5f));

	mTexD = renderer->addTexture("assets/textures/default/default_d.png");
	mTexN = renderer->addTexture("assets/textures/default/default_n.png");
	mTexS = renderer->addTexture("assets/textures/default/default_s.png");

	mShader1 = renderer->addShader("assets/shaders/basic.shd");
	mShader2 = renderer->addShader("assets/shaders/basic.shd");
	mShader3 = renderer->addShader("assets/shaders/basic.shd");
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

	if (input.isKeyDown(KEY_LEFT_SHIFT))
		camera->setStrafeSpeed(strafeFastSpeed);
	else
		camera->setStrafeSpeed(strafeSpeed);

	if (input.isKeyDown(KEY_W))
		camera->move(Camera::FORWARD);
	else if (input.isKeyDown(KEY_S))
		camera->move(Camera::BACKWARD);

	if (input.isKeyDown(KEY_A))
		camera->move(Camera::LEFT);
	else if (input.isKeyDown(KEY_D))
		camera->move(Camera::RIGHT);

	if (input.isKeyDown(KEY_E))
		camera->move(Camera::UP);
	else if (input.isKeyDown(KEY_Q))
		camera->move(Camera::DOWN);

	if (input.getMouseScroolY() != 0)
		camera->move(input.getMouseScroolY() > 0 ? Camera::FORWARD : Camera::BACKWARD);

	if (input.isMouseDown(MouseButton::Right))
	{
		hideMouse();
		camera->rotate(static_cast<float>(input.getMouseChangeX()), static_cast<float>(input.getMouseChangeY()));
	}
	else
		showMouse();
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
