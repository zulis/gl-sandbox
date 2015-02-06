#pragma once

#include <unordered_map>
#include "core/Math.h"

enum class MouseButton
{
	Left,
	Right,
	Middle
};

class Input
{
public:
	Input() {};
	virtual ~Input() {};

	void setKeyStatus(int key, bool isDown);
	bool isKeyDown(int key) const;
	bool isKeyUp(int key) const;

	void setMousePositionStatus(int x, int y);
	void setMouseChangeStatus(int x, int y);
	void setMouseButtonStatus(const MouseButton& button, bool down);
	void setMouseScrollStatus(double x, double y);
	const ivec2 getMousePosition() const;
	int getMouseX() const;
	int getMouseY() const;
	int getMouseChangeX() const;
	int getMouseChangeY() const;
	double getMouseScroolX() const;
	double getMouseScroolY() const;
	bool isMouseDown(const MouseButton& button) const;
	void showMouse();
	void hideMouse();
	bool isMouseVisible() const;

private:
	std::unordered_map<int, bool> mKeyTable;
	mutable std::unordered_map<int, bool> mPrevKeyTable;
	std::unordered_map<MouseButton, bool> mMouseButtons;
	int mMouseX {0};
	int mMouseY { 0 };
	int mMouseChangeX { 0 };
	int mMouseChangeY{ 0 };
	double mMouseScrollX { 0 };
	double mMouseScrollY { 0 };
	bool mMouseIsVisible{ true };
};

//=========================================================================
void Input::setKeyStatus(int key, bool isDown)
{
	mPrevKeyTable = mKeyTable;
	mKeyTable[key] = isDown;
}

//=========================================================================
bool Input::isKeyDown(int key) const
{
	auto it = mKeyTable.find(key);

	if(it != mKeyTable.end())
		return it->second;
	else
		return false;
}

//=========================================================================
bool Input::isKeyUp(int key) const
{
	auto itPrev = mPrevKeyTable.find(key);
	auto it = mKeyTable.find(key);

	if(itPrev != mPrevKeyTable.end() && it != mKeyTable.end())
	{
		if(itPrev->second == true && it->second == false)
		{
			mPrevKeyTable[key] = false;
			return true;
		}
	}

	return false;
}

//=========================================================================
void Input::setMousePositionStatus(int x, int y)
{
	mMouseChangeX = x - mMouseX;
	mMouseChangeY = y - mMouseY;
	mMouseX = x;
	mMouseY = y;
}

//=========================================================================
void Input::setMouseChangeStatus(int x, int y)
{
	mMouseChangeX = x;
	mMouseChangeY = y;
}

//=========================================================================
void Input::setMouseButtonStatus(const MouseButton& button, bool down)
{
	mMouseButtons[button] = down;
}

//=========================================================================
void Input::setMouseScrollStatus(double x, double y)
{
	mMouseScrollX = x;
	mMouseScrollY = y;
}

//=========================================================================
const ivec2 Input::getMousePosition() const
{
	return ivec2(mMouseX, mMouseY);
}

//=========================================================================
int Input::getMouseX() const
{
	return mMouseX;
}

//=========================================================================
int Input::getMouseY() const
{
	return mMouseY;
}

//=========================================================================
int Input::getMouseChangeX() const
{
	return mMouseChangeX;
}

//=========================================================================
int Input::getMouseChangeY() const
{
	return mMouseChangeY;
}

//=========================================================================
double Input::getMouseScroolX() const
{
	return mMouseScrollX;
}

//=========================================================================
double Input::getMouseScroolY() const
{
	return mMouseScrollY;
}

//=========================================================================
bool Input::isMouseDown(const MouseButton& button) const
{
	auto it = mMouseButtons.find(button);

	if (it != mMouseButtons.end())
		return mMouseButtons.at(button);
	else
		return false;
}

//=========================================================================
void Input::showMouse()
{
	mMouseIsVisible = true;
}

//=========================================================================
void Input::hideMouse()
{
	mMouseIsVisible = false;
}

//=========================================================================
bool Input::isMouseVisible() const
{
	return mMouseIsVisible;
}
