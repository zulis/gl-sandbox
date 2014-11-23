#pragma once

#include <memory>
#include "Core/Window.h"
#include "Core/Events.h"
#include "Core/MouseEvent.h"

typedef std::shared_ptr<class State> StateRef;

class State //: public Listener<KeyboardEvent>
{
	friend class Game;
	friend class StateManager;
public:
	virtual void setup() {};
	virtual void cleanup() {};
	virtual void input(const KeyboardEvent& keyEvent/*, const MouseEvent& mouseEvent*/) {};
	virtual void update(float elapsedTime) {};
	virtual void draw() {};
	virtual void resize(unsigned int width, int height) {};

	//virtual void onEvent(const KeyboardEvent& event) {};

	void quit();
	Window* getWindow() const;

private:
	bool mShouldQuit;
	Window* mWindow;
	MouseEventRef mMouseEvent;
	KeyboardEventRef mKeyboardEvent;

private:
	template<class T>
	static StateRef create(Window* window);
	void emitInput();
	void setMouseButonStatus(int button, bool isDown);
	void setMousePositionStatus(int x, int y, int changeX, int changeY);
	void setMouseWheelStatus(int x);
};

//=========================================================================
template<class T>
StateRef State::create(Window* window)
{
	auto state = StateRef(new T, [](T* ptr)
	{
		ptr->cleanup();
		delete ptr;
	});
	state->mShouldQuit = false;
	state->mWindow = window;
	state->mKeyboardEvent = KeyboardEventRef(new KeyboardEvent);
	state->mMouseEvent = MouseEventRef(new MouseEvent);
	state->setup();
	return state;
}

//=========================================================================
void State::quit()
{
	mShouldQuit = true;
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
	input(*mKeyboardEvent/*, *mMouseEvent*/);
}

//=========================================================================
Window* State::getWindow() const
{
	return mWindow;
}
