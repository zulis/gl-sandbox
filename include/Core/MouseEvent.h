#pragma once

#include <memory>
#include <map>

typedef std::unique_ptr<class MouseEvent> MouseEventRef;

class MouseEvent
{
public:
	MouseEvent();
	virtual ~MouseEvent();

	int getX() const;
	int getY() const;
	int getChangeX() const;
	int getChangeY() const;
	int getWheel() const;
	bool isDown(int button) const;
	bool isUp(int button) const;
	void setButtonStatus(int button, bool isDown);
	void setPositionStatus(int x, int y, int changeX, int changeY);
	void setWheelStatus(int x);

private:
	std::map<int, bool> mButtonTable;
	int mX;
	int mY;
	int mChangeX;
	int mChangeY;
	int mWheel { 0 };
};

//=========================================================================
MouseEvent::MouseEvent()
{
}

//=========================================================================
MouseEvent::~MouseEvent()
{
	mButtonTable.clear();
}

//=========================================================================
int MouseEvent::getX() const
{
	return mX;
}

//=========================================================================
int MouseEvent::getY() const
{
	return mY;
}

//=========================================================================
int MouseEvent::getChangeX() const
{
	return mChangeX;
}

//=========================================================================
int MouseEvent::getChangeY() const
{
	return mChangeY;
}

//=========================================================================
int MouseEvent::getWheel() const
{
	return mWheel;
}

//=========================================================================
bool MouseEvent::isDown(int button) const
{
	auto it = mButtonTable.find(button);

	if(it != mButtonTable.end())
		return it->second;
	else
		return false;
}

//=========================================================================
bool MouseEvent::isUp(int button) const
{
	return !isDown(button);
}

//=========================================================================
void MouseEvent::setButtonStatus(int button, bool isDown)
{
	mButtonTable[button] = isDown;
}

//=========================================================================
void MouseEvent::setPositionStatus(int x, int y, int changeX, int changeY)
{
	mX = x;
	mY = y;
	mChangeX = changeX;
	mChangeY = changeY;
}

//=========================================================================
void MouseEvent::setWheelStatus(int x)
{
	mWheel = x;
}