#pragma once

#include <Artemis.h>
#include "Components/RenderComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/VelocityComponent.h"
#include "Components/TransformComponent.h"

class PlayerMovementSystem : public EntityProcessingSystem
{
public:
	PlayerMovementSystem();

private:
	ComponentMapper<PlayerComponent> mPlayerMapper;
	ComponentMapper<RenderComponent> mRenderMapper;
	ComponentMapper<KeyboardComponent> mKeyboardMapper;
	ComponentMapper<VelocityComponent> mVelocityMapper;
	ComponentMapper<TransformComponent> mTransformMapper;
	float mPlayerMoveSpeed{ 0.05f };
	float mPlayerRotateSpeed{ 3.0f };
	float mPlayerDragFactor{ mPlayerMoveSpeed * 20 };


private:
	virtual void initialize();
	virtual void processEntity(Entity& e);

	float positionX{ 0.0f };
	float positionY{ 0.0f };
	float rotationZ{ 0.0f };

};

//=========================================================================
PlayerMovementSystem::PlayerMovementSystem()
{
	addComponentType<PlayerComponent>();
	addComponentType<RenderComponent>();
	addComponentType<KeyboardComponent>();
	addComponentType<VelocityComponent>();
	addComponentType<TransformComponent>();
}

//=========================================================================
void PlayerMovementSystem::initialize()
{
	mPlayerMapper.init(*world);
	mRenderMapper.init(*world);
	mKeyboardMapper.init(*world);
	mVelocityMapper.init(*world);
	mTransformMapper.init(*world);
}

//=========================================================================
void PlayerMovementSystem::processEntity(Entity& e)
{
	auto player = mPlayerMapper.get(e);
	auto renderable = mRenderMapper.get(e)->getRenderable();
	auto keyboard = mKeyboardMapper.get(e);
	auto velocity = mVelocityMapper.get(e);
	auto transform = mTransformMapper.get(e);

	//auto rotation = transform->getRotation();

	if (keyboard->isPressed(player->forwardKey) || keyboard->isPressed(player->forwardAlternateKey))
	{
		velocity->x -= sin(rotationZ) * mPlayerMoveSpeed * world->getDelta();
		velocity->y += cos(rotationZ) * mPlayerMoveSpeed * world->getDelta();
	}
	else
	{
		// Use Stokes' law to apply drag to the ship
		velocity->x = velocity->x - velocity->x * mPlayerDragFactor * world->getDelta();
		velocity->y = velocity->y - velocity->y * mPlayerDragFactor *world->getDelta();
	}

	if (keyboard->isPressed(player->leftKey) || keyboard->isPressed(player->leftAlternateKey))
	{
		rotationZ += mPlayerRotateSpeed * world->getDelta();
	}
	else if (keyboard->isPressed(player->rightKey) || keyboard->isPressed(player->rightAlternateKey))
	{
		rotationZ -= mPlayerRotateSpeed * world->getDelta();
	}

	transform->setRotation(glm::vec3(0.0f, 0.0f, rotationZ));

	//auto position = transform->getPosition();

	positionX += velocity->x * world->getDelta();
	positionY += velocity->y * world->getDelta();
	//position.z = 0;

	transform->setPosition(glm::vec3(positionX, positionY, 0.0f));

	// 		auto bbox = renderable->getAxisAlignedBoundingBox();
	// 		auto cambbox = GameCamera::getBoundingBox();
	// 		auto w = bbox.maxX - bbox.minX;
	// 		auto h = bbox.maxY - bbox.minY;

	// 		if (renderable->isCulled())
	// 		{
	// 			if (bbox.minX > cambbox.maxX)
	// 			{
	// 				position->x = cambbox.minX - w / 2;
	// 			}
	// 			else if (bbox.maxX < cambbox.minX)
	// 			{
	// 				position->x = cambbox.maxX + w / 2;
	// 			}
	//
	// 			if (bbox.minY > cambbox.maxY)
	// 			{
	// 				position->y = cambbox.minY - h / 2;
	// 			}
	// 			else if (bbox.maxY < cambbox.minY)
	// 			{
	// 				position->y = cambbox.maxY + h / 2;
	// 			}
	// 		}
}
