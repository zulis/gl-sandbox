#pragma once

#include <functional>
#include <Artemis.h>
#include "Components/TransformComponent.h"
#include "Components/MeshRenderComponent.h"

class MeshRenderSystem : public EntityProcessingSystem
{
public:
	MeshRenderSystem();

private:
	ComponentMapper<MeshRenderComponent> mRenderMapper;
	ComponentMapper<TransformComponent> mTransformMapper;

private:
	virtual void initialize();
	virtual void processEntity(Entity& e);
};

//=========================================================================
MeshRenderSystem::MeshRenderSystem()
{
	addComponentType<MeshRenderComponent>();
	addComponentType<TransformComponent>();
}

//=========================================================================
void MeshRenderSystem::initialize()
{
	mRenderMapper.init(*world);
	mTransformMapper.init(*world);
}

//=========================================================================
void MeshRenderSystem::processEntity(Entity& e)
{
	auto renderable = mRenderMapper.get(e)->getMesh();
	auto camera = mRenderMapper.get(e)->getCamera();
	auto transformComp = mTransformMapper.get(e);

// 	if (renderable->isCulled(*camera))
// 	{
// 		printf("culled\n");
// 	}

	renderable->setPosition(transformComp->getPosition());
	renderable->RotateBy(transformComp->getRotation());
	renderable->setScale(transformComp->getScale());
	//renderable->update(world->getDelta());
	renderable->draw(camera);
}