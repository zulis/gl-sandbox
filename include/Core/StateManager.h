#pragma once

#include <memory>
#include "Core/State.h"

class StateManager
{
public:
	template<class T>
	static State* setState();
	static State* getState();
	static bool shouldQuit();
	static void input(Input& input);
	static void update(double elapsedTime);
	static void draw();
	static void resize(unsigned int width, unsigned int height);

private:
	static StateRef mState;
};

StateRef StateManager::mState = nullptr;

//=========================================================================
template<class T>
State* StateManager::setState()
{
	mState.reset();
	mState = State::create<T>();
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
void StateManager::input(Input& input)
{
	if (mState)
		mState->input(input);
}

//=========================================================================
void StateManager::update(double elapsedTime)
{
	if(mState)
		mState->update(elapsedTime);
}

//=========================================================================
void StateManager::draw()
{
	if(mState)
		mState->draw();
}

//=========================================================================
void StateManager::resize(unsigned int width, unsigned int height)
{
	if(mState)
		mState->resize(width, height);
}