#pragma once

#include <memory>
#include "Core/Defines.h"
#include "Core/Input.h"

typedef std::shared_ptr<class State> StateRef;

class State
{
	//friend class Game;
	friend class StateManager;
public:
	virtual void setup() {};
	virtual void cleanup() {};
	virtual void input(Input& input) {};
	virtual void update(double elapsedTime) {};
	virtual void draw() {};
	virtual void resize(unsigned int width, unsigned int height) {};

	void quit();

private:
	template<class T>
	static StateRef create();
	bool mShouldQuit;
};

//=========================================================================
template<class T>
StateRef State::create()
{
	auto state = StateRef(new T, [](T* ptr)
	{
		ptr->cleanup();
		delete ptr;
	});
	state->mShouldQuit = false;
	state->setup();
	return state;
}

//=========================================================================
void State::quit()
{
	mShouldQuit = true;
}