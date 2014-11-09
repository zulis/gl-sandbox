#pragma once

#include <memory>
#include "Core/KeyEvent.h"
#include "Core/MouseEvent.h"

typedef std::shared_ptr<class State> StateRef;

class State
{
	friend class Game;
	friend class StateManager;
public:
	virtual void setup() {};
	virtual void cleanup() {};
	virtual void input(const KeyEvent& keyEvent, const MouseEvent& mouseEvent) {};
	virtual void update(float elapsedTime) {};
	virtual void draw() {};
	virtual void resize(int width, int height) {};

	void quit();

private:
	bool mShouldQuit;
	KeyEventRef mKeyEvent;
	MouseEventRef mMouseEvent;

private:
	template<class T>
	static StateRef create();
	void emitInput();
	void setKeyStatus(int key, bool isDown);
	void setMouseButonStatus(int button, bool isDown);
	void setMousePositionStatus(int x, int y, int changeX, int changeY);
	void setMouseWheelStatus(int x);
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
	state->setup();
	state->mShouldQuit = false;
	state->mKeyEvent = KeyEventRef(new KeyEvent);
	state->mMouseEvent = MouseEventRef(new MouseEvent);
	return state;
}

//=========================================================================
void State::quit()
{
	mShouldQuit = true;
}

//=========================================================================
void State::setKeyStatus(int key, bool isDown)
{
	mKeyEvent->setKeyStatus(key, isDown);
}

//=========================================================================
void State::setMouseButonStatus(int button, bool isDown)
{
	mMouseEvent->setButtonStatus(button, isDown);
}

//=========================================================================
void State::setMousePositionStatus(int x, int y, int changeX, int changeY)
{
	mMouseEvent->setPositionStatus(x, y, changeX, changeY);
}

//=========================================================================
void State::setMouseWheelStatus(int x)
{
	mMouseEvent->setWheelStatus(x);
}

//=========================================================================
void State::emitInput()
{
	input(*mKeyEvent, *mMouseEvent);
}