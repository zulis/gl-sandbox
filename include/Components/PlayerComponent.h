#pragma once

#include <Artemis.h>

class PlayerComponent : public Component
{
public:
	int forwardKey;
	int backKey;
	int leftKey;
	int rightKey;
	int forwardAlternateKey;
	int backAlternateKey;
	int leftAlternateKey;
	int rightAlternateKey;

	PlayerComponent(int forwardKey, int backKey, int leftKey, int rightKey, int forwardAlternateKey = 0, int backAlternateKey = 0, int leftAlternateKey = 0, int rightAlternateKey = 0);
};

//=========================================================================
PlayerComponent::PlayerComponent(int forwardKey, int backKey, int leftKey, int rightKey, int forwardAlternateKey /*= 0*/, int backAlternateKey /*= 0*/, int leftAlternateKey /*= 0*/, int rightAlternateKey /*= 0*/)
{
	this->forwardKey = forwardKey;
	this->backKey = backKey;
	this->leftKey = leftKey;
	this->rightKey = rightKey;
	this->forwardAlternateKey = forwardAlternateKey;
	this->backAlternateKey = backAlternateKey;
	this->leftAlternateKey = leftAlternateKey;
	this->rightAlternateKey = rightAlternateKey;
}