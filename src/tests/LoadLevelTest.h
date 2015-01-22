#pragma once

#include "Core/Game.h"
#include "Core/Level.h"
#include "Core/Camera.h"

class LoadLevelTest : public State
{
public:
	LoadLevelTest() {};
	~LoadLevelTest() {};

	virtual void setup();
	virtual void input(Input& input);
	virtual void update(double elapsedTime);
	virtual void draw();
	virtual void resize(unsigned int width, unsigned int height);

private:
	CameraPtr mCamera;
	float mStrafeSpeed{ 0.2f };
	float mStrafeFastSpeed{ 0.4f };
	LevelRef mLevel;
};

//=========================================================================
void LoadLevelTest::setup()
{
	mLevel = Level::create();
	mLevel->setModelFormat("obj");
	mLevel->loadFromFile("assets/levels/test1.json");

	mCamera = Camera::create();
	mCamera->setRotateSpeed(0.002f);
	mCamera->setStrafeSpeed(mStrafeSpeed);
	mCamera->setPosition(mLevel->getCamPosition());
	mCamera->setLookAt(mLevel->getCamLookAt());
}

//=========================================================================
void LoadLevelTest::input(Input& input)
{
	if (input.isKeyDown(KEY_ESCAPE))
		quit();

	if (input.isKeyDown(KEY_LEFT_SHIFT))
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
	}
}

//=========================================================================
void LoadLevelTest::update(double elapsedTime)
{
	mLevel->update(elapsedTime);
}

//=========================================================================
void LoadLevelTest::draw()
{
	mLevel->draw(mCamera);
}

//=========================================================================
void LoadLevelTest::resize(unsigned int width, unsigned int height)
{
	mCamera->setAspectRatio((float)width / height);
}

//RUN(LoadLevelTest)