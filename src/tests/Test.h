#pragma once

#include "core/BaseApp.h"

class Test : public BaseApp
{
public:
	Test();
	~Test();

	virtual void onInput(const Input& input);
	virtual void onUpdate(double deltaTime);
	virtual void onDraw();
	virtual void onResize(const unsigned int width, const unsigned int height);

private:
	TextureID mTexD;
	TextureID mTexN;
	TextureID mTexS;

	ShaderID mShader;
	Mesh mMesh;
	Transform mTransform;

	const float strafeSpeed
	{
		0.2f
	};
	const float strafeFastSpeed
	{
		0.4f
	};
};

//=========================================================================
Test::Test() : BaseApp(1280, 720, WindowMode::Windowed)
{
	setTitle("Game!");
	renderer->setCearColor(Color::gray(0.5f));

	mTexD = renderer->addTexture("assets/models/leprechaun/leprechaun_d.png");
	//mTexD = renderer->addTexture(Color::white());
	mTexN = renderer->addTexture("assets/models/leprechaun/leprechaun_n.png");
	mTexS = renderer->addTexture("assets/models/leprechaun/leprechaun_s.png");

	mShader = renderer->addShader("assets/shaders/basic.shd");

	//mMesh.loadFromFile("assets/models/box/box.fbx");
	mMesh.loadFromFile("assets/models/leprechaun/leprechaun.fbx");
	//mMesh.loadFromFile("assets/models/sponza/sponza.obj");
	mMesh.makeDrawable(renderer, mShader);

	camera->setPosition(0, 2, -10);
	camera->setLookAt(0, 4, 0);

	gl::enableCullFace(gl::CullFaceType::Back);
}

//=========================================================================
Test::~Test()
{
}

//=========================================================================
void Test::onInput(const Input& input)
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

	auto k = mMesh.getAABB().transformed(mTransform.getMatrix()).intersect(AABB(camera->getPosition() - 0.5f, camera->getPosition() + 0.5f));

	if (k == AABB::IntersectionType::Inside || k == AABB::IntersectionType::Intersect)
	{
		auto dir = camera->getDirection();
		camera->setDirection(-dir);
		camera->move(Camera::FORWARD);
		camera->setDirection(dir);
	}
}

//=========================================================================
void Test::onUpdate(double deltaTime)
{
}

//=========================================================================
void Test::onDraw()
{
	if (!camera->intersects(mMesh.getAABB().transformed(mTransform.getMatrix())))
		return;

	renderer->setShader(mShader);

	mTransform.setRotationX(-90);
	mTransform.setRotationY(180);
	mTransform.setScale(0.1f);

	renderer->setShaderUniform(ShaderConstants::ProjectionMatrix, camera->getProjectionMatrix());
	renderer->setShaderUniform(ShaderConstants::ModelViewMatrix, camera->getViewMatrix() * mTransform.getMatrix());
	renderer->setShaderUniform(ShaderConstants::MVP, camera->getProjectionMatrix() * camera->getViewMatrix() * mTransform.getMatrix());

	auto mv = camera->getViewMatrix() * mTransform.getMatrix();
	renderer->setShaderUniform(ShaderConstants::NormalMatrix, mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

	// Color map
	renderer->setTexture(mTexD, 0);
	renderer->setShaderUniform(ShaderConstants::ColorMap, 0);
	renderer->setShaderUniform(ShaderConstants::ColorMapIsUsed, true);
	// Normal map
	//renderer->setTexture(mTexN, 1);
	//renderer->setShaderUniform(ShaderConstants::NormalMap, 1);
	//renderer->setShaderUniform(ShaderConstants::NormalMapIsUsed, true);
	// Specular map
	//renderer->setTexture(mTexS, 2);
	//renderer->setShaderUniform(ShaderConstants::SpecularMap, 2);
	//renderer->setShaderUniform(ShaderConstants::SpecularMapIsUsed, true);

	// Draw mesh
	mMesh.draw(renderer);

	renderer->reset();
}

//=========================================================================
void Test::onResize(const unsigned int width, const unsigned int height)
{
	camera->setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
}
