#pragma once

#include <memory>
#include <Artemis.h>

#include "core/Camera.h"
#include "core/Mesh.h"
//#include "core/MaterialPhong.h"
#include "core/PointLight.h"
#include "core/DirectionalLight.h"
#include "core/SpotLight.h"

#include "Components/MeshRenderComponent.h"
#include "Components/TransformComponent.h"
#include "Components/VelocityComponent.h"
#include "Components/KeyboardComponent.h"
#include "Components/PlayerComponent.h"

#include "Systems/MeshRenderSystem.h"
#include "Systems/KeyboardSystem.h"
#include "Systems/PlayerMovementSystem.h"

typedef std::shared_ptr<class EntityFactory> EntityFactoryRef;

class EntityFactory
{
public:
	static EntityFactoryRef create();

	EntityFactory();
	~EntityFactory();

	void setup(const CameraPtr& camera);
	//void input(const KeyEvent& keyEvent, const MouseEvent& mouseEvent);
	void update(float elapsedTime);
	void draw();

private:
	CameraPtr mCamera;
	World mWorld;
	SystemManager* mSm;
	EntityManager* mEm;
	EntityProcessingSystem* mMeshRenderSystem;
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
void EntityFactory::setup(const CameraPtr& camera)
{
	mCamera = camera;

	mSm = mWorld.getSystemManager();
	mMeshRenderSystem = mSm->setSystem<MeshRenderSystem>();
	mKeyboardSystem = mSm->setSystem<KeyboardSystem>();
	mPlayerMovementSystem = mSm->setSystem<PlayerMovementSystem>();
	mEm = mWorld.getEntityManager();
	mSm->initializeAll();

// 	Light light;
// 	light.position = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
// 	light.intensity = glm::vec3(1.0f, 1.0f, 1.0f);

// 	auto mat = MaterialPhong::create();
// 	mat->setAmbientColor(0.1f, 0.1f, 0.1f);
// 	mat->setSpecularColor(0.2f, 0.2f, 0.2f);
// 	mat->setShininess(1.0f);
// 	mat->addLight(light);




	auto mesh = Mesh::create();
	mesh->setFrustumCulling(true);
	//mesh->loadFromFile("assets/models/misc/sphere.fbx");
	//mesh->loadFromFile("assets/models/rocks/1/rock_01.fbx");
	//mesh->loadFromFile("assets/models/ship/ship.fbx");
	
	mesh->loadFromFile("assets/models/box/box.fbx");
	//mesh->loadFromFile("assets/models/leprechaun/leprechaun.obj", 0.02f);

	//mesh->setTexturePath("assets/models/sponza/textures");
	//mesh->loadFromFile("assets/models/sponza/sponza.obj", 0.02f);
	
	auto light = PointLight::create();
	light->setPosition(glm::vec3(0, 0, 5));

	auto mat = mesh->getMaterial();
	//mat->addTexture("assets/models/box/box_d.png", TextureType::DiffuseMap, 0);
	//mat->addTexture("assets/models/box/box_n.png", TextureType::NormalMap, 0);
	//mat->addTexture("assets/models/box/box_h.png", TextureType::HeightMap, 0);
	//mat->addTexture("assets/models/box/box_s.png", TextureType::SpecularMap, 0);

	//mat->addTexture("assets/models/sponza/textures/vase_plant.png", TextureType::DiffuseMap, 0);
	//mat->addTexture("assets/textures/misc/opacity.png", TextureType::OpacityMap, 0);

	mat->addLight(*light);
	
	//mat->addTexture("assets/models/leprechaun/leprechaun_d.png");
	//mat->addTexture("assets/models/leprechaun/leprechaun_n.png", TextureType::NormalMap);
	//mat->addTexture("assets/models/leprechaun/leprechaun_s.png", TextureType::SpecularMap);
	

	//mesh->setMaterial(mat);
	//mat->setGeomMaterials(mesh->getGeomMaterial());

	Entity& e = mWorld.createEntity();
	e.setGroup("static");
	//e.addComponent(transform);
	e.addComponent(new TransformComponent);
	e.addComponent(new VelocityComponent);
	e.addComponent(new KeyboardComponent);
	e.addComponent(new MeshRenderComponent(mesh, mCamera));
	//e.addComponent(new PlayerComponent(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT));
	e.refresh();
}

//=========================================================================
// void EntityFactory::input(const KeyEvent& keyEvent, const MouseEvent& mouseEvent)
// {
// 	mKeyboardSystem->setKeyStatus(keyEvent.getLastKey().key, keyEvent.getLastKey().isDown);
// }

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
	mMeshRenderSystem->process();
	//gl::disableCullFace();
}



