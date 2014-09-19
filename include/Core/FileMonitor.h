#pragma once

#include <memory>
#include <functional>
#include <string>
#include <chrono>
#include <future>

#include <iostream>
#include <sys/stat.h>
#include <time.h>

namespace common
{

typedef std::shared_ptr<class FileMonitor> FileMonitorRef;
typedef std::function<void()> FileChangedCallback;

class FileMonitor
{
	public:
		static FileMonitorRef create(const std::string& fileName, FileChangedCallback callback);
		~FileMonitor();

	private:
		FileMonitor(const std::string& fileName, FileChangedCallback callback);
		FileMonitor(const FileMonitor&);
		FileMonitor& operator = (const FileMonitor&);

		void check(const std::string& fileName, FileChangedCallback callback)
		{
			struct tm* time, timeOld;
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
						callback();
					}
				}
			}
			while(true);
		}
};

//=========================================================================
FileMonitorRef FileMonitor::create(const std::string& fileName, FileChangedCallback callback)
{
	return FileMonitorRef(new FileMonitor(fileName, callback));
}

//=========================================================================
FileMonitor::FileMonitor(const std::string& fileName, FileChangedCallback callback)
{
	std::async(std::launch::async, &FileMonitor::check, this, fileName, callback);
}

//=========================================================================
FileMonitor::~FileMonitor()
{
}

} // namespace common