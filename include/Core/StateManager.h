#pragma once

#include <memory>
#include "Core/State.h"

class StateManager
{
public:
	template<class T>
	static State* setState(Window* window);
	static State* getState();
	static bool shouldQuit();
	static void update(float elapsedTime);
	static void draw();
	static void resize(unsigned int width, int height);

private:
	static StateRef mState;
};

StateRef StateManager::mState = nullptr;

//=========================================================================
template<class T>
State* StateManager::setState(Window* window)
{
	mState.reset();
	mState = State::create<T>(window);
	return getState();
}

//=========================================================================
State* StateManager::getState()
{
	if(mState)
		return mState.get();
	else
		return nullptr;
}

//=========================================================================
bool StateManager::shouldQuit()
{
	if(mState)
		return mState->mShouldQuit;
	else
		return false;
}

//=========================================================================
void StateManager::update(float elapsedTime)
{
	if(mState)
	{
		mState->emitInput();
		mState->update(elapsedTime);
	}
}

//=========================================================================
void StateManager::draw()
{
	if(mState)
	{
		mState->draw();
	}
}

//=========================================================================
void StateManager::resize(unsigned int width, int height)
{
	if(mState)
	{
		mState->resize(width, height);
	}
}