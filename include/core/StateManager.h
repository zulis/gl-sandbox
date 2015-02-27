#pragma once

#include <memory>
#include <cassert>
#include "core/State.h"

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
	assert(mState);
	return mState.get();
}

//=========================================================================
bool StateManager::shouldQuit()
{
	assert(mState);
	return mState->mShouldQuit;
}

//=========================================================================
void StateManager::input(Input& input)
{
	assert(mState);
	mState->input(input);
}

//=========================================================================
void StateManager::update(double elapsedTime)
{
	assert(mState);
	mState->update(elapsedTime);
}

//=========================================================================
void StateManager::draw()
{
	assert(mState);
	mState->draw();
}

//=========================================================================
void StateManager::resize(unsigned int width, unsigned int height)
{
	assert(mState);
	mState->resize(width, height);
}