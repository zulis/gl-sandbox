#pragma once

#include <map>

class FileMonitorListener
{
public:
	virtual ~FileMonitorListener() {}
	virtual void onFileMonitorFileChange(const std::string& fileName) = 0;
};