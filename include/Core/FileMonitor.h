#pragma once

#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <future>

#include <iostream>
#include <sys/stat.h>
#include <time.h>

#include "Core/ListenerSet.h"

typedef std::shared_ptr<class FileMonitor> FileMonitorRef;

class FileMonitor : public ListenerSet<FileMonitorListener*>
{
public:
	static FileMonitorRef create(const std::string& fileName);
	static FileMonitorRef create(const std::string& fileNameA, const std::string& fileNameB);
	FileMonitor(const std::string& fileName);
	FileMonitor(const std::string& fileNameA, const std::string& fileNameB);
	~FileMonitor();

private:
	void check(const std::string& fileName);
};

//=========================================================================
FileMonitorRef FileMonitor::create(const std::string& fileName)
{
	return FileMonitorRef(new FileMonitor(fileName));
}

//=========================================================================
FileMonitorRef FileMonitor::create(const std::string& fileNameA, const std::string& fileNameB)
{
	return FileMonitorRef(new FileMonitor(fileNameA, fileNameB));
}

//=========================================================================
FileMonitor::FileMonitor(const std::string& fileName)
{
	std::async(std::launch::async, &FileMonitor::check, this, fileName);
}

//=========================================================================
FileMonitor::FileMonitor(const std::string& fileNameA, const std::string& fileNameB)
{
	std::async(std::launch::async, &FileMonitor::check, this, fileNameA);
	std::async(std::launch::async, &FileMonitor::check, this, fileNameB);
}

//=========================================================================
FileMonitor::~FileMonitor()
{
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
