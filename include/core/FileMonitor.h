#pragma once

#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <future>

#include <iostream>
#include <sys/stat.h>
#include <time.h>

#include "core/ListenerSet.h"

typedef std::shared_ptr<class FileMonitor> FileMonitorRef;

class FileMonitor : public ListenerSet<FileMonitorListener*>
{
public:
	static FileMonitorRef create();
	FileMonitor() {};
	~FileMonitor() {};

	void add(const std::string& fileName);

private:
	void check(const std::string& fileName);
};

//=========================================================================
FileMonitorRef FileMonitor::create()
{
	return FileMonitorRef(new FileMonitor);
}

//=========================================================================
void FileMonitor::check(const std::string& fileName)
{
	struct tm* time;
	struct stat attrib;
	time_t timestamp, timestampOld, timestampLocal;

	localtime(&timestampOld);
	timestampLocal = timestampOld;

	do
	{
		std::chrono::seconds dura(1);
		std::this_thread::sleep_for(dura);

		stat(fileName.c_str(), &attrib);
		time = gmtime(&(attrib.st_mtime));

		if(time)
		{
			timestamp = mktime(time);

			if(timestampOld == timestampLocal)
				timestampOld = timestamp;
			else if(timestamp != timestampOld)
			{
				timestampOld = timestamp;
				notify(&FileMonitorListener::onFileMonitorFileChange, fileName);
				//break;
			}
		}
	}
	while(true);
}

//=========================================================================
void FileMonitor::add(const std::string& fileName)
{
}
