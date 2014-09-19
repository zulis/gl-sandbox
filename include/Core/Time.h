#pragma once

#include <chrono>

class Time
{
	public:
		static double getTime();

	private:
		static std::chrono::system_clock::time_point mEpoch;
};

std::chrono::system_clock::time_point Time::mEpoch = std::chrono::high_resolution_clock::now();

//=========================================================================
double Time::getTime()
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - mEpoch).count() / 1000000000.0;
}