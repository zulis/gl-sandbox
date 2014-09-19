#pragma once

#include <map>
#include <Artemis.h>

class KeyboardComponent : public Component
{
public:
	KeyboardComponent();
	virtual ~KeyboardComponent();

	void setKeyStatus(std::map<int, bool> keys);
	bool isPressed(int key);

private:
	std::map<int, bool> mKeys;
};

//=========================================================================
KeyboardComponent::KeyboardComponent()
{
}

//=========================================================================
KeyboardComponent::~KeyboardComponent()
{
	mKeys.clear();
}

//=========================================================================
void KeyboardComponent::setKeyStatus(std::map<int, bool> keys)
{
	mKeys = keys;
}

//=========================================================================
bool KeyboardComponent::isPressed(int key)
{
	return mKeys[key];
}
