#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <chrono>
//#include <future>
//#include <atomic>

#include <iostream>
#include <sys/stat.h>
#include <time.h>

#include "core/ListenerSet.h"

typedef std::shared_ptr<class FileMonitor> FileMonitorRef;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::seconds seconds;

class FileMonitor : public ListenerSet < FileMonitorListener* >
{
public:
    static FileMonitorRef create();
    FileMonitor();
    ~FileMonitor() {};

    void addFile(const std::string& fileName);
    void update();

private:
    //void check(const std::string& fileName);
    std::time_t getFileTime(const std::string& fileName);
    //std::vector<std::future<void>> mTasks;
    std::unordered_map<std::string, std::time_t> mFileNameMap;
    Clock::time_point mTime;
};

//=========================================================================
FileMonitorRef FileMonitor::create()
{
    return FileMonitorRef(new FileMonitor);
}

//=========================================================================
FileMonitor::FileMonitor()
{
    mTime = Clock::now();
}

//=========================================================================
//void FileMonitor::check(const std::string& fileName)
//{
//	auto time = getFileTime(fileName);
//
//	while(true)
//	{
//		std::this_thread::sleep_for(std::chrono::seconds(1));
//		auto newTime = getFileTime(fileName);
//
//		if(time != newTime)
//		{
//			notify(&FileMonitorListener::onFileMonitorFileChange, fileName);
//			break;
//		}
//	}
//}

//=========================================================================
void FileMonitor::addFile(const std::string& fileName)
{
    mFileNameMap[fileName] = getFileTime(fileName);

    //std::async([&](const std::string& fileName)
    //{
    //	auto time = getFileTime(fileName);

    //	while(true)
    //	{
    //		std::this_thread::sleep_for(std::chrono::seconds(1));
    //		auto newTime = getFileTime(fileName);

    //		if(time != newTime)
    //		{
    //            time = newTime;
    //			notify(&FileMonitorListener::onFileMonitorFileChange, fileName);
    //			//break;
    //		}
    //	}
    //}, fileName);

    //auto future = std::async(std::launch::async, &FileMonitor::check, this, fileName);
    //if(task.wait_for(std::chrono::seconds(1)) == std::future_status::
}

//=========================================================================
void FileMonitor::update()
{
    if (std::chrono::duration_cast<seconds>(Clock::now() - mTime) >= seconds(3))
    {
        mTime = Clock::now();

        for (auto it = mFileNameMap.begin(); it != mFileNameMap.end(); it++)
        {
            auto time = getFileTime(it->first);

            if (time != it->second)
            {
                mFileNameMap[it->first] = time;
                notify(&FileMonitorListener::onFileMonitorFileChange, it->first);
            }
        }
    }
}

//=========================================================================
std::time_t FileMonitor::getFileTime(const std::string& fileName)
{
    struct tm* time;
    struct stat attrib;

    stat(fileName.c_str(), &attrib);
    time = gmtime(&(attrib.st_mtime));

    if (time)
        return mktime(time);
    else
        return std::time(nullptr);
}