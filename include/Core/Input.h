#pragma once

#include <map>
#include <cassert>
#include <glm/glm.hpp>

class Input
{
public:
	Input();
	virtual ~Input() {};

	void setKey(int key, bool isDown);
	bool isKeyDown(int key) const;
	bool isKeyUp(int key) const;

	void setMousePos(int x, int y);
	void setMouseButton(int button, bool down);
	void setScroll(double offsetX, double offsetY);
	glm::ivec2 getMousePosition() const;
	int getMouseX() const;
	int getMouseY() const;
	int getMouseRelX();
	int getMouseRelY();
	bool isMouseDown(int button) const;

private:
	std::map<int, bool> mKeyTable;
	mutable std::map<int, bool> mPrevKeyTable;
	int mMouseX {0};
	int mMouseY { 0 };
	int mMouseRelX { 0 };
	int mMouseRelY { 0 };
	double mMouseOffsetX { 0 };
	double mMouseOffsetY { 0 };
	static const int MOUSE_BUTTON_COUNT = 3;
	bool mMouseButtons[MOUSE_BUTTON_COUNT];
};

//=========================================================================
Input::Input()
{
	for(int i = 0; i < MOUSE_BUTTON_COUNT; ++i)
		mMouseButtons[i] = false;
}

//=========================================================================
void Input::setKey(int key, bool isDown)
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
void Input::setMousePos(int x, int y)
{
	mMouseRelX = x - mMouseX;
	mMouseRelY = y - mMouseY;
	mMouseX = x;
	mMouseY = y;
}

//=========================================================================
void Input::setMouseButton(int button, bool down)
{
	mMouseButtons[button] = down;
}

//=========================================================================
void Input::setScroll(double offsetX, double offsetY)
{
	mMouseOffsetX = offsetX;
	mMouseOffsetY = offsetY;
}

//=========================================================================
glm::ivec2 Input::getMousePosition() const
{
	return glm::ivec2(mMouseX, mMouseY);
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
int Input::getMouseRelX()
{
	int val = mMouseRelX;
	mMouseRelX = 0;
	return val;
}

//=========================================================================
int Input::getMouseRelY()
{
	int val = mMouseRelY;
	mMouseRelY = 0;
	return val;
}

//=========================================================================
bool Input::isMouseDown(int button) const
{
	assert(button < MOUSE_BUTTON_COUNT);
	return mMouseButtons[button];
}