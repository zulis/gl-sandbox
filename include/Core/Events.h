#pragma once

#include <map>

struct FileMonitorEvent
{
	std::string fileName;
	FileMonitorEvent(std::string fileName) : fileName(fileName) {}
};