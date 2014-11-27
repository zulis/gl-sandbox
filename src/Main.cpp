#pragma once

#include "Core/Game.h"
#include "Core/State.h"
//////////////////////////////////////////
#include "Core/Camera.h"
#include "Core/Mesh.h"
#include "Core/PointLight.h"
#include "Core/DirectionalLight.h"
#include "Core/SpotLight.h"
#include "Core/Quad.h"

class Main : public State
{
public:
	Main() {};
	~Main() {};

	virtual void setup();
	virtual void cleanup();
	virtual void input(Input& input);
	virtual void update(double elapsedTime);
	virtual void draw();
	virtual void resize(unsigned int width, unsigned int height);

private:
	CameraRef mCamera;
	MeshRef mMesh;
	QuadRef mQuad;
	float mStrafeSpeed { 0.1f };
	float mStrafeFastSpeed { 0.2f };
	float mRotateH { 0.0f };
	float mRotateV { 0.0f };
	int mWindowWidth;
	int mWindowHeight;
	int mLastMouseX, mLastMouseY, mCurrentMouseX, mCurrentMouseY;

	glm::vec3 getArcballVector(int x, int y);
};

//=========================================================================
void Main::setup()
{
	mCamera = Camera::create();
	mCamera->setPosition(0, 0.75, 2);
	mCamera->setRotateSpeed(0.002f);
	mCamera->setStrafeSpeed(mStrafeSpeed);

	mMesh = Mesh::create();
	mMesh->setFrustumCulling(false);
	//mMesh->setAutoLoadTextures(false);

	//mMesh->loadFromFile("assets/models/plane/plane.fbx");
	//mMesh->getMaterial()->setTilingUV(500);

	//mMesh->loadFromFile("assets/models/box/box.fbx");
	mMesh->loadFromFile("assets/models/leeperrysmith/leeperrysmith.obj", 6);

	//mMesh->loadFromFile("assets/models/leprechaun/leprechaun.fbx", 0.02f);
	//mMesh->setRotationX(-90);
	//mMesh->loadFromFile("assets/models/misc/sphere.fbx");
	//mMesh->loadFromFile("assets/models/rocks/1/rock_01.fbx");
	//mMesh->setTexturePath("assets/models/sponza/textures");
	//mMesh->loadFromFile("assets/models/sponza/sponza.obj", 0.02f);
	auto material = mMesh->getMaterial();

	material->addTexture("assets/models/leeperrysmith/leeperrysmith_d.jpg", TextureType::DiffuseMap);
	material->addTexture("assets/models/leeperrysmith/leeperrysmith_n.jpg", TextureType::NormalMap);
	material->addTexture("assets/models/leeperrysmith/leeperrysmith_s.jpg", TextureType::SpecularMap);

	auto light = PointLight::create();
	light->setPosition(glm::vec3(-5, 0, 5));
	light->setAmbient(Color::red());
	light->setDiffuse(Color::red());
	//light->setSpecular(Color::white());
	//light->setAttenuation(glm::vec2(100, 1000));
	material->addLight(*light);

	light->setAmbient(Color::green());
	light->setDiffuse(Color::green());
	light->setPosition(glm::vec3(0, 0, 5));
	material->addLight(*light);

	light->setAmbient(Color::blue());
	light->setDiffuse(Color::blue());
	light->setPosition(glm::vec3(5, 0, 5));
	material->addLight(*light);

	auto aabb = mMesh->getAABB().transformed(mMesh->getMatrix());
	auto camPosition = mCamera->getPosition();
	camPosition.y = aabb.getCenter().y + aabb.getSize().y / 4.0f;
	mCamera->setPosition(camPosition);
	mCamera->setLookAt(aabb.getCenter());

	mQuad = Quad::create("assets/models/leprechaun/copyright.png");
}

//=========================================================================
void Main::cleanup()
{

}

//=========================================================================
void Main::input(Input& input)
{
	if(input.isKeyDown(KEY_ESCAPE))
		quit();

	//if (input.isKeyDown(KEY_F11))
	//getWindow()->setFullScreenDesktop(!getWindow()->isFullScreen());

	if(input.isKeyDown(KEY_LEFT_SHIFT))
		mCamera->setStrafeSpeed(mStrafeFastSpeed);
	else
		mCamera->setStrafeSpeed(mStrafeSpeed);

	if(input.isKeyDown(KEY_W))
		mCamera->move(Camera::FORWARD);
	else if(input.isKeyDown(KEY_S))
		mCamera->move(Camera::BACKWARD);

	if(input.isKeyDown(KEY_A))
		mCamera->move(Camera::LEFT);
	else if(input.isKeyDown(KEY_D))
		mCamera->move(Camera::RIGHT);

	if(input.isKeyDown(KEY_E))
		mCamera->move(Camera::UP);
	else if(input.isKeyDown(KEY_Q))
		mCamera->move(Camera::DOWN);

	if(input.getMouseScroolY() != 0)
		mCamera->move(input.getMouseScroolY() > 0 ? Camera::FORWARD : Camera::BACKWARD);

	if(input.isMouseDown(MouseButton::Left))
	{
		mCurrentMouseX = input.getMouseX() + input.getMouseChangeX();
		mCurrentMouseY = input.getMouseY() + input.getMouseChangeY();
		mLastMouseX = input.getMouseX();
		mLastMouseY = input.getMouseY();
	}

	if(input.isMouseDown(MouseButton::Right))
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
void Main::update(double elapsedTime)
{
	if(mCurrentMouseX != mLastMouseX || mCurrentMouseY != mLastMouseY)
	{
		glm::vec3 va = getArcballVector(mLastMouseX, mLastMouseY);
		glm::vec3 vb = getArcballVector(mCurrentMouseX, mCurrentMouseY);
		float angle = acos(std::min(1.0f, glm::dot(va, vb)));
		glm::vec3 axisInCameraCoord = glm::cross(va, vb);
		glm::mat3 camera2object = glm::inverse(glm::mat3(mCamera->getViewMatrix()) * glm::mat3(mMesh->getMatrix()));
		glm::vec3 axisInObjectCoord = camera2object * axisInCameraCoord;
		mMesh->setMatrix(glm::rotate(mMesh->getMatrix(), glm::degrees(angle), axisInObjectCoord));
		mLastMouseX = mCurrentMouseX;
		mLastMouseY = mCurrentMouseY;
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

	gl::enableCullFace(gl::CullFaceType::Back);
	mMesh->draw(mCamera);
	gl::disableCullFace();

	gl::enable2D();
	//gl::enableAlphaBlending();
	//mFont->print("This is a test...", 10, 10);

	//mQuad->setRotation(mRotation);
	//mQuad->draw(Quad::Position::CENTER);
	mQuad->draw(mWindowWidth, mWindowHeight, Quad::Position::BOTTOMRIGHT, -10, -10);
	//mGameOverQuad->draw(Quad::CENTER);
	//gl::disableAlphaBlending();
}

//=========================================================================
void Main::resize(unsigned int width, unsigned int height)
{
	mWindowWidth = width;
	mWindowHeight = height;

	mCamera->setAspectRatio((float)width/height);
}

//=========================================================================
glm::vec3 Main::getArcballVector(int x, int y)
{
	glm::vec3 P = glm::vec3(1.0 * x / mWindowWidth * 2 - 1.0,
	                        1.0 * y / mWindowHeight * 2 - 1.0,
	                        0);
	P.y = -P.y;
	float OP_squared = P.x * P.x + P.y * P.y;
	if(OP_squared <= 1 * 1)
		P.z = sqrt(1 * 1 - OP_squared);  // Pythagore
	else
		P = glm::normalize(P);  // nearest point
	return P;
}

RUN(Main)