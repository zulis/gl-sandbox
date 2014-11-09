#pragma once

#include <memory>
#include <map>

typedef std::unique_ptr<class KeyEvent> KeyEventRef;
#define KEY_NONE -1

class KeyEvent
{
public:

	struct LastKey
	{
		int key { KEY_NONE };
		bool isDown {false};
	};

	KeyEvent();
	virtual ~KeyEvent();

	bool isDown(int key) const;
	bool isUp(int key) const;
	void setKeyStatus(int key, bool isDown);
	LastKey getLastKey() const;
	void clearLastKey() const;

private:
	std::map<int, bool> mKeyTable;
	mutable std::map<int, bool> mPrevKeyTable;
	mutable LastKey mLastKey;
};

//=========================================================================
KeyEvent::KeyEvent()
{
}

//=========================================================================
KeyEvent::~KeyEvent()
{
	mKeyTable.clear();
	mPrevKeyTable.clear();
}

//=========================================================================
bool KeyEvent::isDown(int key) const
{
	auto it = mKeyTable.find(key);

	if(it != mKeyTable.end())
		return it->second;
	else
		return false;
}

//=========================================================================
bool KeyEvent::isUp(int key) const
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
void KeyEvent::setKeyStatus(int key,  bool isDown)
{
	mPrevKeyTable = mKeyTable;
	mKeyTable[key] = isDown;

	mLastKey.key = key;
	mLastKey.isDown = isDown;
}

//=========================================================================
KeyEvent::LastKey KeyEvent::getLastKey() const
{
	return mLastKey;
}

void KeyEvent::clearLastKey() const
{
	mLastKey = LastKey();
}
