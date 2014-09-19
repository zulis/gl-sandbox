#pragma once

#include <Artemis.h>
#include "Core/Game.h"
#include "Core/Camera.h"
#include "Core/ImageManager.h"

#include "EntityFactory.h"

#include "Core/Mesh.h"
#include "Core/MaterialPhong.h"
#include "Core/Font.h"
#include "Core/Quad.h"

class Main : public State
{
	public:
		virtual void setup();
		virtual void input(const KeyEvent& keyEvent, const MouseEvent& mouseEvent);
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual void resize(int width, int height);

private:
	float mStrafeSpeed{ 2.0f };
	float mStrafeFastSpeed{ 4.0f };
	bool mMouseLocked{ false };
	float mCenterPositionX;
	float mCenterPositionY;
	float mRotateY{ 0.0f };

	EntityFactoryRef mEntityFactory;

	CameraRef mCamera;
	Image* mIconImage;
	ImageManager mImageMng;
	
	//MeshRef mMesh;
	//MaterialPhongRef mMaterial;
	FontRef mFont;
	QuadRef mQuad;
	QuadRef mGameOverQuad;
	float mRotation{ 0.0f };

	void calculateMouseCenterPosition();
	void createWorld();
};

//=========================================================================
void Main::setup()
{
	Window::setSize(800, 600);

	mCamera = Camera::create();
	mCamera->setPosition(0, 0, 10);
	mCamera->setLookAt(0, 0, 0);
	mCamera->setRotateSpeed(0.002f);
	mCamera->setStrafeSpeed(mStrafeSpeed);

	mIconImage = mImageMng.get()->getResource("assets/textures/misc/hand.png");
	Window::setIcon(mIconImage->getPixels(), mIconImage->getWidth(), mIconImage->getHeight(), mIconImage->getChannels());
	Window::setCursor(mIconImage->getPixels(), mIconImage->getWidth(), mIconImage->getHeight(), mIconImage->getChannels());

	mEntityFactory = EntityFactory::create();
	mEntityFactory->setup(mCamera);

	mFont = Font::create("assets/ui/verdana.ttf", 20);
	mQuad = Quad::create("assets/textures/misc/256x256a.png", 128, 128);
	mGameOverQuad = Quad::create("assets/ui/gameover.png");

	// TESTS ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*mMaterial = MaterialPhong::create();
	mMaterial->setAmbientColor(0.1f, 0.1f, 0.1f);
	mMaterial->setSpecularColor(0.2f, 0.2f, 0.2f);
	mMaterial->setShininess(1.0f);

	mMesh = Mesh::create("assets/models/imrod/imrod.fbx");
	mMesh->setMaterial(mMaterial);
	mMesh->setScale(10);
	mMesh->setRotationX(-90);

	mMaterial->setGeomMaterials(mMesh->getGeomMaterial());

	Light light;
	light.position = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
	light.intensity = glm::vec3(1.0f, 1.0f, 1.0f);

	mMaterial->addLight(light);*/
}

//=========================================================================
void Main::input(const KeyEvent& keyEvent, const MouseEvent& mouseEvent)
{
	mEntityFactory->input(keyEvent, mouseEvent);

	if(keyEvent.isUp(KEY_ESCAPE))
		quit();

	if(keyEvent.isUp(KEY_F11))
		Window::setFullScreenDesktop(!Window::isFullScreen());

	/*if (keyEvent.isDown(KEY_LSHIFT))
		mCamera->setStrafeSpeed(mStrafeFastSpeed);
		else
		mCamera->setStrafeSpeed(mStrafeSpeed);

		if (keyEvent.isDown(KEY_W))
		mCamera->move(Camera::FORWARD);
		else if (keyEvent.isDown(KEY_S))
		mCamera->move(Camera::BACKWARD);

		if (keyEvent.isDown(KEY_A))
		mCamera->move(Camera::LEFT);
		else if (keyEvent.isDown(KEY_D))
		mCamera->move(Camera::RIGHT);

		if (keyEvent.isDown(KEY_Q))
		mCamera->move(Camera::UP);
		else if (keyEvent.isDown(KEY_Z))
		mCamera->move(Camera::DOWN);

		if (mouseEvent.getWheel() != 0)
		mCamera->move(mouseEvent.getWheel() > 0 ? Camera::FORWARD : Camera::BACKWARD);

		if (mouseEvent.isDown(BUTTON_LEFT))
		mRotateY += mouseEvent.getChangeX();

		if (mMouseLocked)
		{
		auto deltaPosX = mouseEvent.getX() - mCenterPositionX;
		auto deltaPosY = mouseEvent.getY() - mCenterPositionY;
		mCamera->rotate(deltaPosX, deltaPosY);
		}

		if (mouseEvent.isDown(BUTTON_RIGHT))
		{
		Window::hideMouse();
		Window::setMousePosition(mCenterPositionX, mCenterPositionY);
		mMouseLocked = true;
		}
		else
		{
		Window::showMouse();
		mMouseLocked = false;
		}*/
}

//=========================================================================
void Main::update(float elapsedTime)
{
	mEntityFactory->update(elapsedTime);
	//mMesh->setRotationY(mRotateY * elapsedTime * 10.f);
}

//=========================================================================
void Main::draw()
{
	//gl::enableCullFace(gl::CullFaceType::Back);
	mEntityFactory->draw();
	//gl::disableCullFace();

	//gl::enableCullFace(gl::CullFaceType::Back);
	//mMesh->draw(mCamera);
	//gl::disableCullFace();

	//gl::enable2D();
	//gl::enableAlphaBlending();
	//mFont->print("This is a test...", 10, 10);

	mRotation++;
	if (mRotation > 360) mRotation = 0.0f;

	//mQuad->setRotation(mRotation);
	//mQuad->draw(Quad::Position::CENTER);
	mQuad->draw(Quad::Position::BOTTOMRIGHT, -10, -10);
	//mGameOverQuad->draw(Quad::CENTER);
	//gl::disableAlphaBlending();
}

//=========================================================================
void Main::resize(int width, int height)
{
	mCamera->setAspectRatio(Window::getAspectRatio());

	calculateMouseCenterPosition();
	Window::setMousePosition(mCenterPositionX, mCenterPositionY);
}

//=========================================================================
void Main::calculateMouseCenterPosition()
{
	mCenterPositionX = (float)Window::getWidth() / 2.0f;
	mCenterPositionY = (float)Window::getHeight() / 2.0f;
}

RUN(Main)