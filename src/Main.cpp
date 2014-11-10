#pragma once

#include <Artemis.h>
#include "Core/Game.h"
#include "Core/Camera.h"
#include "Core/ImageManager.h"
#include "Core/Math.h"

#include "EntityFactory.h"

#include "Core/Mesh.h"
#include "Core/Font.h"
#include "Core/Quad.h"
#include "Core/PointLight.h"
#include "Core/DirectionalLight.h"
#include "Core/SpotLight.h"
#include "Core/FileMonitor.h"

class Main : public State
{
public:
	virtual void setup();
	virtual void input(const KeyEvent& keyEvent, const MouseEvent& mouseEvent);
	virtual void update(float elapsedTime);
	virtual void draw();
	virtual void resize(int width, int height);

private:
	float mStrafeSpeed { 0.1f };
	float mStrafeFastSpeed { 0.2f };
	bool mMouseLocked { false };
	float mCenterPositionX;
	float mCenterPositionY;
	float mRotateH { 0.0f };
	float mRotateV { 0.0f };

	EntityFactoryRef mEntityFactory;

	CameraRef mCamera;
	Image* mIconImage;
	ImageManager mImageMng;

	MeshRef mMesh;
	//MaterialPhongRef mMaterial;
	FontRef mFont;
	QuadRef mQuad;
	//QuadRef mGameOverQuad;

	void calculateMouseCenterPosition();
	void createWorld();
	glm::vec3 getArcballVector(int x, int y);

	int last_mx, last_my, cur_mx, cur_my;
};

//=========================================================================
void Main::setup()
{
	//Window::setSize(1280, 720);
	Window::setSize(800, 600);

	mCamera = Camera::create();
	mCamera->setPosition(0, 0.75, 2);
	mCamera->setLookAt(0, 0.75, 0);
	mCamera->setRotateSpeed(0.002f);
	mCamera->setStrafeSpeed(mStrafeSpeed);

	mIconImage = mImageMng.get()->getResource("assets/textures/misc/hand.png");
	Window::setIcon(mIconImage->getPixels(), mIconImage->getWidth(), mIconImage->getHeight(), mIconImage->getChannels());
	Window::setCursor(mIconImage->getPixels(), mIconImage->getWidth(), mIconImage->getHeight(), mIconImage->getChannels());

	mEntityFactory = EntityFactory::create();
	mEntityFactory->setup(mCamera);

	//mFont = Font::create("assets/ui/verdana.ttf", 20);
	mQuad = Quad::create("assets/models/leprechaun/copyright.png");
	//mGameOverQuad = Quad::create("assets/ui/gameover.png");

	// TESTS ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	mMesh = Mesh::create();
	mMesh->setFrustumCulling(true);
	
	//mMesh->loadFromFile("assets/models/plane/plane.fbx");
	//mMesh->getMaterial()->setTilingUV(500);

	//mMesh->loadFromFile("assets/models/box/box.fbx");
	mMesh->loadFromFile("assets/models/leprechaun/leprechaun.obj", 0.02f);
	//mMesh->loadFromFile("assets/models/misc/sphere.fbx");
	//mMesh->loadFromFile("assets/models/rocks/1/rock_01.fbx");
	//mMesh->setTexturePath("assets/models/sponza/textures");
	//mMesh->loadFromFile("assets/models/sponza/sponza.obj", 0.02f);
	auto material = mMesh->getMaterial();

	

	auto light = PointLight::create();
	light->setPosition(glm::vec3(0, 0, 5));
	//light->setAmbient(Color::white());
	//light->setDiffuse(Color::white());
	//light->setSpecular(Color::white());
	//light->setAttenuation(glm::vec2(100, 1000));
	material->addLight(*light);



	/*light = PointLight::create();
	light->setPosition(glm::vec3(0, 5, 5));
	light->setAmbient(Color::green());
	light->setDiffuse(Color::green());
	light->setSpecular(Color::black());
	mat->addLight(*light);

	light = PointLight::create();
	light->setPosition(glm::vec3(5, 5, 5));
	light->setAmbient(Color::blue());
	light->setDiffuse(Color::blue());
	light->setSpecular(Color::black());
	mat->addLight(*light);*/

	//mMesh->setScale(10);
	//mMesh->setRotationX(-90);
}

//=========================================================================
void Main::input(const KeyEvent& keyEvent, const MouseEvent& mouseEvent)
{
	mEntityFactory->input(keyEvent, mouseEvent);

	if(keyEvent.isUp(KEY_ESCAPE))
		quit();

	if(keyEvent.isUp(KEY_F11))
		Window::setFullScreenDesktop(!Window::isFullScreen());

	if(keyEvent.isDown(KEY_LSHIFT))
		mCamera->setStrafeSpeed(mStrafeFastSpeed);
	else
		mCamera->setStrafeSpeed(mStrafeSpeed);

	if(keyEvent.isDown(KEY_W))
		mCamera->move(Camera::FORWARD);
	else if(keyEvent.isDown(KEY_S))
		mCamera->move(Camera::BACKWARD);

	if(keyEvent.isDown(KEY_A))
		mCamera->move(Camera::LEFT);
	else if(keyEvent.isDown(KEY_D))
		mCamera->move(Camera::RIGHT);

	if(keyEvent.isDown(KEY_E))
		mCamera->move(Camera::UP);
	else if(keyEvent.isDown(KEY_Q))
		mCamera->move(Camera::DOWN);

	if(mouseEvent.getWheel() != 0)
		mCamera->move(mouseEvent.getWheel() > 0 ? Camera::FORWARD : Camera::BACKWARD);

	//if (mouseEvent.isDown(BUTTON_LEFT))
//		mRotateH += mouseEvent.getChangeX();

	//if(mouseEvent.isDown(BUTTON_LEFT))
	//mRotateV += mouseEvent.getChangeY();

	if(mouseEvent.isDown(BUTTON_LEFT))
	{
		//cur_mx = mouseEvent.getX();
		//cur_my = mouseEvent.getY();

		cur_mx = mouseEvent.getX() + mouseEvent.getChangeX() * 2;
		cur_my = mouseEvent.getY() + mouseEvent.getChangeY() * 2;

		last_mx = mouseEvent.getX();
		last_my = mouseEvent.getY();
	}
	else
	{
		//last_mx = cur_mx = mouseEvent.getX();
		//last_my = cur_my = mouseEvent.getY();
	}

	// if motion
	//if (mouseEvent.getChangeX() != 0 || mouseEvent.getChangeY() != 0)
	//{
	//	if (arcball_on)     // if left button is pressed
	//	{
	//		cur_mx = mouseEvent.getX();
	//		cur_my = mouseEvent.getY();
	//	}
	//}

	if(mMouseLocked)
	{
		auto deltaPosX = mouseEvent.getX() - mCenterPositionX;
		auto deltaPosY = mouseEvent.getY() - mCenterPositionY;
		mCamera->rotate(deltaPosX, deltaPosY);
	}

	if(mouseEvent.isDown(BUTTON_RIGHT))
	{
		Window::hideMouse();
		Window::setMousePosition((int)mCenterPositionX, (int)mCenterPositionY);
		mMouseLocked = true;
	}
	else
	{
		Window::showMouse();
		mMouseLocked = false;
	}
}

glm::vec3 Main::getArcballVector(int x, int y)
{
	glm::vec3 P = glm::vec3(1.0*x / Window::getWidth() * 2 - 1.0,
	                        1.0*y / Window::getHeight() * 2 - 1.0,
	                        0);
	P.y = -P.y;
	float OP_squared = P.x * P.x + P.y * P.y;
	if(OP_squared <= 1 * 1)
		P.z = sqrt(1 * 1 - OP_squared);  // Pythagore
	else
		P = glm::normalize(P);  // nearest point
	return P;
}

//=========================================================================
void Main::update(float elapsedTime)
{
	//mEntityFactory->update(elapsedTime);
	//mMesh->setRotationY(mRotateH * elapsedTime * 10.f);
	//mMesh->setRotationX(mRotateV * elapsedTime * 10.f);

	if(cur_mx != last_mx || cur_my != last_my)
	{
		glm::vec3 va = getArcballVector(last_mx, last_my);
		glm::vec3 vb = getArcballVector(cur_mx, cur_my);
		float angle = acos(std::min(1.0f, glm::dot(va, vb)));
		glm::vec3 axisInCameraCoord = glm::cross(va, vb);
		glm::mat3 camera2object = glm::inverse(glm::mat3(mCamera->getViewMatrix()) * glm::mat3(mMesh->getMatrix()));
		glm::vec3 axisInObjectCoord = camera2object * axisInCameraCoord;
		mMesh->setMatrix(glm::rotate(mMesh->getMatrix(), glm::degrees(angle), axisInObjectCoord));
		last_mx = cur_mx;
		last_my = cur_my;
	}
}

//=========================================================================
void Main::draw()
{
	/*
	gl::enableCullFace(gl::CullFaceType::Back);
	mEntityFactory->draw();
	gl::disableCullFace();
	*/

	//gl::enableCullFace(gl::CullFaceType::Back);
	mMesh->draw(mCamera);
	//gl::disableCullFace();

	//gl::enable2D();
	//gl::enableAlphaBlending();
	//mFont->print("This is a test...", 10, 10);

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
	Window::setMousePosition((int)mCenterPositionX, (int)mCenterPositionY);
}

//=========================================================================
void Main::calculateMouseCenterPosition()
{
	mCenterPositionX = (float)Window::getWidth() / 2.0f;
	mCenterPositionY = (float)Window::getHeight() / 2.0f;
}

RUN(Main)