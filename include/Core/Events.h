#pragma once

#include <map>

struct FileMonitorEvent
{
	std::string fileName;
	FileMonitorEvent(std::string fileName) : fileName(fileName) {}
};

//=========================================================================
typedef std::unique_ptr<class KeyboardEvent> KeyboardEventRef;

class KeyboardEvent
{
public:
	KeyboardEvent(int key = -1, bool isDown = false);
	bool isDown(int key) const;
	bool isUp(int key) const;
private:
	static std::map<int, bool> sKeyTable;
	static std::map<int, bool> sPrevKeyTable;
};

std::map<int, bool> KeyboardEvent::sKeyTable;
std::map<int, bool> KeyboardEvent::sPrevKeyTable;

//=========================================================================
KeyboardEvent::KeyboardEvent(int key, bool isDown)
{
	sPrevKeyTable = sKeyTable;
	sKeyTable[key] = isDown;
}

//=========================================================================
bool KeyboardEvent::isDown(int key) const
{
	auto it = sKeyTable.find(key);

	if(it != sKeyTable.end())
		return it->second;
	else
		return false;
}

//=========================================================================
bool KeyboardEvent::isUp(int key) const
{
	auto itPrev = sPrevKeyTable.find(key);
	auto it = sKeyTable.find(key);

	if(itPrev != sPrevKeyTable.end() && it != sKeyTable.end())
	{
		if(itPrev->second == true && it->second == false)
		{
			sPrevKeyTable[key] = false;
			return true;
		}
	}

	return false;
}