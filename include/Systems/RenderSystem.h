#pragma once

#include <functional>
#include <Artemis.h>
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"

class RenderSystem : public EntityProcessingSystem
{
public:
	RenderSystem();

private:
	ComponentMapper<RenderComponent> mRenderMapper;
	ComponentMapper<TransformComponent> mTransformMapper;

private:
	virtual void initialize();
	virtual void processEntity(Entity& e);
};

//=========================================================================
RenderSystem::RenderSystem()
{
	addComponentType<RenderComponent>();
	addComponentType<TransformComponent>();
}

//=========================================================================
void RenderSystem::initialize()
{
	mRenderMapper.init(*world);
	mTransformMapper.init(*world);
}

//=========================================================================
void RenderSystem::processEntity(Entity& e)
{
	auto renderable = mRenderMapper.get(e)->getRenderable();
	auto camera = mRenderMapper.get(e)->getCamera();
	auto transformComp = mTransformMapper.get(e);

// 	if (renderable->isCulled(*camera))
// 	{
// 		printf("culled\n");
// 	}

	renderable->setMatrix(transformComp->getMatrix());
	//renderable->update(world->getDelta());
	renderable->draw(camera);
}