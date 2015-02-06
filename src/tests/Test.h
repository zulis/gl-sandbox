#pragma once

#include "core/Game.h"
#include "core/State.h"
//////////////////////////////////////////
#include "core/Camera.h"
#include "core/Mesh.h"
#include "core/PointLight.h"
#include "core/DirectionalLight.h"
#include "core/SpotLight.h"
#include "core/Quad.h"
#include "core/DefaultMaterial.h"

class Test : public State
{
public:
	Test() {};
	~Test() {};

	virtual void setup();
	virtual void cleanup();
	virtual void input(Input& input);
	virtual void update(double elapsedTime);
	virtual void draw();
	virtual void resize(unsigned int width, unsigned int height);

private:
	CameraPtr mCamera;
	MeshRef mMesh1, mMesh2;
	MeshRef mLightMesh;
	QuadRef mQuad;
	MaterialDefaultRef mMaterial;
	float mStrafeSpeed { 0.1f };
	float mStrafeFastSpeed { 0.2f };
	float mRotateH { 0.0f };
	float mRotateV { 0.0f };
	int mWindowWidth;
	int mWindowHeight;
	int mLastMouseX, mLastMouseY, mCurrentMouseX, mCurrentMouseY;
	float mRotation { 0.0f };
	vec3 mLightLookAt{ vec3(0.0f, 0.0f, 1.0f) };

	vec3 getArcballVector(int x, int y);
};

//=========================================================================
void Test::setup()
{
	mCamera = Camera::create();
	mCamera->setPosition(0, 0.75, 2);
	mCamera->setRotateSpeed(0.002f);
	mCamera->setStrafeSpeed(mStrafeSpeed);

	mMaterial = DefaultMaterial::create();

	mMesh1 = Mesh::create();
	mMesh1->setFrustumCulling(false);
	//mMesh->setAutoLoadTextures(false);

	
	//mMesh2 = Mesh::create(); mMesh2->loadFromFile("assets/models/plane/plane.fbx"); mMesh2->getMaterial()->setTilingUV(500); mMesh2->setPosition(vec3(0, -15, 0));

	//mMesh->loadFromFile("assets/models/box/box.fbx");
	mMesh1->loadFromFile("assets/models/leeperrysmith/leeperrysmith.fbx", 6); mMesh1->setRotationX(-90);
	//mMesh->loadFromFile("assets/models/leprechaun/leprechaun.fbx", 0.02f); mMesh->setRotationX(-90);

	//mMesh->loadFromFile("assets/models/misc/sphere.fbx");
	//mMesh->loadFromFile("assets/models/rocks/1/rock_01.fbx");
	//mMesh->setTexturePath("assets/models/sponza/textures");
	//mMesh->loadFromFile("assets/models/sponza/sponza.obj", 0.02f);
	//auto material = mMesh1->getMaterial();

	//material->addTexture("assets/models/leeperrysmith/leeperrysmith_d.jpg", TextureType::ColorMap);
	//material->addTexture("assets/models/leeperrysmith/leeperrysmith_n.jpg", TextureType::NormalMap);
	//material->addTexture("assets/models/leeperrysmith/leeperrysmith_s.jpg", TextureType::SpecularMap);

	unsigned int geometryIndex = 0;

	for(auto& meshPart : mMesh1->getMeshData())
	{
		for(auto& meshTexture : meshPart.material.textures)
		{
			mMaterial->addTexture(meshTexture.fileName, meshTexture.textureType, geometryIndex);
		}

		geometryIndex++;
	}

	auto light = PointLight::create();
	//auto light = DirectionalLight::create();
	//auto light = SpotLight::create();
	light->setPosition(vec3(0.0f, 10.0f, 0.0f));
	light->setLookAt(vec3(mLightLookAt));
	light->setAmbient(Color::white());
	light->setDiffuse(Color::white());
	light->setSpecular(Color::white());
	mMaterial->addLight(*light);

	mMaterial->setAmbientColor(Color(0.3f, 0.3f, 0.3f, 1.0f));
	mMaterial->setDiffuseColor(Color(0.7f, 0.7f, 0.7f, 1.0f));
	mMaterial->setSpecularColor(Color(0.5f, 0.5f, 0.5f, 1.0f));
	mMaterial->setShininess(60.0f);

	mLightMesh = Mesh::create();
	mLightMesh->loadFromFile("assets/models/misc/sphere.fbx");
	mLightMesh->setScale(0.1f);


	/*
	auto light = PointLight::create();
	light->setPosition(vec3(-5, 0, 0));
	light->setAmbient(Color::red());
	light->setDiffuse(Color::red());
	//light->setSpecular(Color::white());
	//light->setAttenuation(vec2(100, 1000));
	material->addLight(*light);

	light->setAmbient(Color::green());
	light->setDiffuse(Color::green());
	light->setPosition(vec3(0, 0, 5));
	material->addLight(*light);

	light->setAmbient(Color::blue());
	light->setDiffuse(Color::blue());
	light->setPosition(vec3(5, 0, 0));
	material->addLight(*light);
	*/

	auto aabb = mMesh1->getAABB().transformed(mMesh1->getMatrix());
	auto camPosition = mCamera->getPosition();
	camPosition.y = aabb.getCenter().y /*+ aabb.getSize().y / 4.0f*/;
	mCamera->setPosition(camPosition);
	mCamera->setLookAt(aabb.getCenter());

	mQuad = Quad::create("assets/models/leprechaun/copyright.png");
}

//=========================================================================
void Test::cleanup()
{

}

//=========================================================================
void Test::input(Input& input)
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

	if (input.isKeyDown(KEY_UP))
		mLightLookAt.z -= 0.1f;
	else if (input.isKeyDown(KEY_DOWN))
		mLightLookAt.z += 0.1f;

	if (input.isKeyDown(KEY_LEFT))
		mLightLookAt.x -= 0.1f;
	else if (input.isKeyDown(KEY_RIGHT))
		mLightLookAt.x += 0.1f;

	if (input.isKeyDown(KEY_HOME))
		mLightLookAt.y += 0.1f;
	else if (input.isKeyDown(KEY_END))
		mLightLookAt.y -= 0.1f;
}

//=========================================================================
void Test::update(double elapsedTime)
{
	mRotation += 10.0f * elapsedTime;

	if(mCurrentMouseX != mLastMouseX || mCurrentMouseY != mLastMouseY)
	{
		vec3 va = getArcballVector(mLastMouseX, mLastMouseY);
		vec3 vb = getArcballVector(mCurrentMouseX, mCurrentMouseY);
		float angle = acos(std::min(1.0f, dot(va, vb)));
		vec3 axisInCameraCoord = cross(va, vb);
		mat3 camera2object = inverse(mat3(mCamera->getViewMatrix()) * mat3(mMesh1->getMatrix()));
		vec3 axisInObjectCoord = camera2object * axisInCameraCoord;
		mMesh1->setMatrix(rotate(mMesh1->getMatrix(), angle, axisInObjectCoord));
		mLastMouseX = mCurrentMouseX;
		mLastMouseY = mCurrentMouseY;
	}

	//mMesh->setRotationZ(mRotation);
	mMaterial->getLight(0).setPosition(mLightLookAt);
	mLightMesh->setPosition(mLightLookAt);

	//mMesh1->setLookAt(mCamera->getPosition()); mMesh1->setRotationX(-90);
	//mMesh1->setLookAt(mLightMesh->getPosition()); mMesh1->setRotationX(-90);
	//mMesh1->setRotateTowards(mLightMesh->getRotation(), radians(90.0f) * elapsedTime); //mMesh1->setRotationX(-90);
	
}

//=========================================================================
void Test::draw()
{
	/*
	gl::enableCullFace(gl::CullFaceType::Back);
	mEntityFactory->draw();
	gl::disableCullFace();
	*/

	mMaterial->bind();
	mMaterial->updateUniforms(0);

	gl::enableCullFace(gl::CullFaceType::Back);
	mMesh1->draw(mCamera, mMaterial->getShader());
	mLightMesh->draw(mCamera, mMaterial->getShader());
	gl::disableCullFace();

	if (mMesh2)
		mMesh2->draw(mCamera, mMaterial->getShader());

	mMaterial->unbind();

	gl::enable2D();
	//gl::enableAlphaBlending();
	//mFont->print("This is a test...", 10, 10);

	//mQuad->setRotation(mRotation);
	//mQuad->draw(mWindowWidth, mWindowHeight, Quad::Position::TOPLEFT);
	//mQuad->draw(mWindowWidth, mWindowHeight, Quad::Position::BOTTOMRIGHT, -10, -10);
	//mQuad->setPosition(10, 10);
	//mQuad->draw(mWindowWidth, mWindowHeight);
	//mGameOverQuad->draw(Quad::CENTER);
	//gl::disableAlphaBlending();
}

//=========================================================================
void Test::resize(unsigned int width, unsigned int height)
{
	mWindowWidth = width;
	mWindowHeight = height;

	mCamera->setAspectRatio((float)width/height);
}

//=========================================================================
vec3 Test::getArcballVector(int x, int y)
{
	vec3 P = vec3(1.0 * x / mWindowWidth * 2 - 1.0,
	                        1.0 * y / mWindowHeight * 2 - 1.0,
	                        0);
	P.y = -P.y;
	float OP_squared = P.x * P.x + P.y * P.y;

	if(OP_squared <= 1 * 1)
		P.z = sqrt(1 * 1 - OP_squared);  // Pythagore
	else
		P = normalize(P);  // nearest point

	return P;
}

//RUN(Test)