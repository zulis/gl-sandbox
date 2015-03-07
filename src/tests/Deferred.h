#pragma once

#include <iostream>
#include <mutex>
#include "core/Game.h"
#include "core/Camera.h"
//#include "core/Shape.h"
//#include "core/Transform3d.h"
#include "core/Resource.h"
//#include "core/Image.h"
//#include "core/Texture.h"
#include "core/Shader.h"
#include "core/MeshLoader.h"
#include "core/Geometry.h"
#include "core/Transform.h"
#include "core/FileMonitor.h"
#include "core/Listeners.h"
#include "core/Ui.h"

class Deferred : public State, public FileMonitorListener
{
public:
	Deferred() {};
	~Deferred() {};

	virtual void setup();
	virtual void cleanup();
	virtual void input(Input& input);
	virtual void update(double elapsedTime);
	virtual void draw();
	virtual void resize(unsigned int width, unsigned int height);

	virtual void onFileMonitorFileChange(const std::string& fileName);

private:
	CameraPtr mCamera;
	float mStrafeSpeed{ 0.2f };
	float mStrafeFastSpeed{ 0.4f };

	//ShaderRef mShader;
	//GeometryRef mShape;
	//Transform3D mTransform;
	//TextureRef mColorTex;
	ShaderPtr mShader;
	std::vector<std::shared_ptr<Geometry>> mGeometryVec;
	FileMonitorRef mFileMonitor;
    std::mutex mMutex;

	void drawUI();
};

//=========================================================================
void Deferred::setup()
{
	//auto a1 = Resource::get<Texture>("assets\\textures\\default\\default_d.png");
	//auto a2 = Resource::get<Texture>("assets\\textures\\default\\default_d.png");
	//auto a3 = Resource::get<Texture>("assets\\textures\\default\\default_d.png");

	//auto b1 = Resource::get<Texture>("assets\\textures\\default\\default_n.png");
	//auto b2 = Resource::get<Texture>("assets\\textures\\default\\default_n.png");
	//auto b3 = Resource::get<Texture>("assets\\textures\\default\\default_n.png");

	//auto a1 = Resource::get<Texture>(Color::red());
	//auto a2 = Resource::get<Texture>(Color::red());
	//auto a3 = Resource::get<Texture>(Color::red());

	//auto b1 = Resource::get<Texture>(Color::blue());
	//auto b2 = Resource::get<Texture>(Color::blue());
	//auto b3 = Resource::get<Texture>(Color::blue());

	//auto sh1 = Shared<Shader>::create();
	//sh1->loadFromFile("assets\\shaders\\basic.vert", ShaderType::Vertex);
	//sh1->loadFromFile("assets\\shaders\\basic.frag", ShaderType::Fragment);
	//sh1->link();

	mFileMonitor = FileMonitor::create();
	mFileMonitor->addFile("assets\\shaders\\basic.vert");
	mFileMonitor->addFile("assets\\shaders\\basic.frag");
	mFileMonitor->addListener(this);

	mShader = Shader::create();
	mShader->loadFromFile("assets\\shaders\\basic.vert", ShaderType::Vertex);
	mShader->loadFromFile("assets\\shaders\\basic.frag", ShaderType::Fragment);
	mShader->link();

	auto ml1 = Resource::get<MeshLoader>("assets\\models\\leprechaun\\leprechaun.fbx");
	auto ml2 = Resource::get<MeshLoader>("assets\\models\\leprechaun\\leprechaun.fbx");
	auto ml3 = Resource::get<MeshLoader>("assets\\models\\leprechaun\\leprechaun.fbx");

	for (const auto& mp : ml1->getMeshData())
	{
		auto geo = mp.geometry;
		auto mat = mp.material;

		auto geometry = Shared<Geometry>::create();
		geometry->setVertices(geo.vertices);
		geometry->setIndices(geo.indices);
		geometry->setTexCoords(geo.texCoords);
		geometry->setTangents(geo.tangents);
		geometry->setBitangents(geo.bitangents);
		geometry->prepare(*mShader);

		mGeometryVec.push_back(geometry);
	}


	mCamera = Camera::create();
	mCamera->setRotateSpeed(0.002f);
	mCamera->setStrafeSpeed(mStrafeSpeed);
	mCamera->setPosition(vec3(0, 5, -10));
	mCamera->setLookAt(vec3(0, 0, 0));

	/*
	mShader = Shader::create("assets/shaders/basic");
	mShape = Shape::createCube();
	mColorTex = Texture::create("assets/textures/default/default_d.png");*/
}

//=========================================================================
void Deferred::cleanup()
{

}

//=========================================================================
void Deferred::input(Input& input)
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
void Deferred::update(double elapsedTime)
{
    mFileMonitor->update();
}

//=========================================================================
void Deferred::draw()
{
	gl::enableCullFace(gl::CullFaceType::Back);

	Transform transform;
	transform.setRotationX(-90);
	transform.setRotationY(180);
	transform.setScale(0.1);

	mShader->bind();

	mShader->setUniform(ShaderConstants::ProjectionMatrix, mCamera->getProjectionMatrix());
	//mShader->setUniform(ShaderConstants::ViewMatrix,  mCamera->getViewMatrix());
	//mShader->setUniform(ShaderConstants::ModelMatrix, transform.getMatrix());
	mShader->setUniform(ShaderConstants::ModelViewMatrix, mCamera->getViewMatrix() * transform.getMatrix());
	mShader->setUniform(ShaderConstants::MVP, mCamera->getProjectionMatrix() *  mCamera->getViewMatrix() * transform.getMatrix());

	auto mv = mCamera->getViewMatrix() * transform.getMatrix();
	mShader->setUniform(ShaderConstants::NormalMatrix, mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

	//mShader->setUniform(ShaderConstants::MVP, mCamera->get ->getViewMatrix() * transform.getMatrix());

	for (const auto& geometry : mGeometryVec)
	{
		geometry->draw();
	}

	mShader->unbind();

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

	drawUI();
}

//=========================================================================
void Deferred::resize(unsigned int width, unsigned int height)
{
	mCamera->setAspectRatio((float)width / height);
}

//=========================================================================
void Deferred::onFileMonitorFileChange(const std::string& fileName)
{
    mShader->reload();
    /* mShader.reset();
     mShader = Shared<Shader>::create();
     mShader->loadFromFile("assets\\shaders\\basic.vert", ShaderType::Vertex);
     mShader->loadFromFile("assets\\shaders\\basic.frag", ShaderType::Fragment);
     mShader->link();*/
}

//=========================================================================
void Deferred::drawUI()
{
	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoTitleBar;
	flags |= ImGuiWindowFlags_NoResize;

	ui::SetNextWindowPos(ImVec2(10, 10), ImGuiSetCond_FirstUseEver); // ImGuiSetCond_Always/*ImGuiSetCond_FirstUseEver*/);
	ui::Begin("Test", (bool*)true, ImVec2(280, 800), 0.0f, flags);

	static bool showHelp = false;
	if (ui::Button(showHelp ? "Hide help" : "Show help"))
		showHelp ^= 1;

	ui::SameLine();
	if (ui::Button("Close"))
		quit();

	if (showHelp)
	{
		ui::Separator();
		ui::Text("Frame time %.3f ms | FPS: %.1f", 1000.0f / ui::GetIO().Framerate, ui::GetIO().Framerate);

		static int item = 1;
		ui::Combo("combo", &item, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");
	}

	ui::End();

	//ui::ShowTestWindow(NULL);
}
