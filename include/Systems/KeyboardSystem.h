#pragma once

#include <Artemis.h>
#include "Components/KeyboardComponent.h"

class KeyboardSystem : public EntityProcessingSystem
{
public:
	KeyboardSystem();
	void setKeyStatus(int key, bool isDown);

private:
	ComponentMapper<KeyboardComponent> mKeyboardMapper;
	std::unordered_map<int, bool> mKeys;

private:
	virtual void initialize();
	virtual void processEntity(Entity& e);
};

//=========================================================================
KeyboardSystem::KeyboardSystem()
{
	addComponentType<KeyboardComponent>();
}

//=========================================================================
void KeyboardSystem::initialize()
{
	mKeyboardMapper.init(*world);
}

//=========================================================================
void KeyboardSystem::processEntity(Entity& e)
{
	auto keyboard = mKeyboardMapper.get(e);
	keyboard->setKeyStatus(mKeys);
}

//=========================================================================
void KeyboardSystem::setKeyStatus(int key, bool isDown)
{
	mKeys[key] = isDown;
}