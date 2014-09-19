#pragma once

#include <memory>
#include <Artemis.h>

#include "Core/Camera.h"
#include "Core/KeyEvent.h"
#include "Core/MouseEvent.h"
#include "Core/Mesh.h"
#include "Core/MaterialPhong.h"

#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"
#include "Components/VelocityComponent.h"
#include "Components/KeyboardComponent.h"
#include "Components/PlayerComponent.h"

#include "Systems/RenderSystem.h"
#include "Systems/KeyboardSystem.h"
#include "Systems/PlayerMovementSystem.h"

#include "Spatial/MeshSpatial.h"

typedef std::shared_ptr<class EntityFactory> EntityFactoryRef;

class EntityFactory
{
public:
	static EntityFactoryRef create();

	EntityFactory();
	~EntityFactory();

	void setup(const CameraRef& camera);
	void input(const KeyEvent& keyEvent, const MouseEvent& mouseEvent);
	void update(float elapsedTime);
	void draw();

private:
	CameraRef mCamera;
	World mWorld;
	SystemManager* mSm;
	EntityManager* mEm;
	EntityProcessingSystem* mRenderSystem;
	KeyboardSystem* mKeyboardSystem;
	PlayerMovementSystem* mPlayerMovementSystem;

};

//=========================================================================
EntityFactoryRef EntityFactory::create()
{
	return EntityFactoryRef(new EntityFactory);
}

//=========================================================================
EntityFactory::EntityFactory()
{

}

//=========================================================================
EntityFactory::~EntityFactory()
{

}

//=========================================================================
void EntityFactory::setup(const CameraRef& camera)
{
	mCamera = camera;

	mSm = mWorld.getSystemManager();
	mRenderSystem = mSm->setSystem<RenderSystem>();
	mKeyboardSystem = mSm->setSystem<KeyboardSystem>();
	mPlayerMovementSystem = mSm->setSystem<PlayerMovementSystem>();
	mEm = mWorld.getEntityManager();
	mSm->initializeAll();

	Light light;
	light.position = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
	light.intensity = glm::vec3(1.0f, 1.0f, 1.0f);

	auto mat = MaterialPhong::create();
	mat->setAmbientColor(0.1f, 0.1f, 0.1f);
	mat->setSpecularColor(0.2f, 0.2f, 0.2f);
	mat->setShininess(1.0f);
	mat->addLight(light);

	//auto mesh = MeshSpatial::create("assets/models/teapot/teapot.fbx");
	//auto mesh = MeshSpatial::create("assets/models/rocks/1/rock_01_v2.fbx");
	auto mesh = MeshSpatial::create("assets/models/ship/ship.fbx");
	//auto mesh = MeshSpatial::create("assets/models/imrod/imrod.fbx");
	//auto mesh = MeshSpatial::create("assets/models/sponza/sponza.obj");
	//mesh->setTexturePath("assets/textures/sponza");
	mesh->setFrustumCulling(true);
	mesh->setMaterial(mat);
	mat->setGeomMaterials(mesh->getGeomMaterial());

	auto transform = new TransformComponent();
	transform->setScale(glm::vec3(0.01));

	Entity& e = mWorld.createEntity();
	e.setGroup("static");
	//e.addComponent(transform);
	e.addComponent(new TransformComponent);
	e.addComponent(new VelocityComponent);
	e.addComponent(new KeyboardComponent);
	e.addComponent(new RenderComponent(mesh, mCamera));
	e.addComponent(new PlayerComponent(KEY_W, KEY_S, KEY_A, KEY_D, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT));
	e.refresh();
}

//=========================================================================
void EntityFactory::input(const KeyEvent& keyEvent, const MouseEvent& mouseEvent)
{
	mKeyboardSystem->setKeyStatus(keyEvent.getLastKey().key, keyEvent.getLastKey().isDown);
}

//=========================================================================
void EntityFactory::update(float elapsedTime)
{
	mWorld.setDelta(elapsedTime);
	mWorld.loopStart();

	mKeyboardSystem->process();
	mPlayerMovementSystem->process();
}

//=========================================================================
void EntityFactory::draw()
{
	//gl::enableCullFace(gl::CullFaceType::Back);
	mRenderSystem->process();
	//gl::disableCullFace();
}



