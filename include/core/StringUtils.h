#pragma once

#include <string>
#include <sstream>
#include <cctype>
#include <locale>
#include <vector>
#include <algorithm>
#include "core/Math.h"

class stringUtils
{
public:
	static std::string trim(const std::string &s);
	static std::string trimLeft(const std::string &s);
	static std::string trimRight(const std::string &s);
	static bool startsWith(const std::string &s, const std::string &value);
	static std::string extract(const std::string &s, const char& between);
	static std::string cutTail(const std::string &s, const std::string &cutAt);
	static bool contains(const std::string &s, const std::string &value);
	static std::vector<std::string> split(const std::string &s, char delim);
	static std::string toLower(std::string &s);
	static int toInt(const std::string &s);
	static float toFloat(const std::string &s);
	static double toDouble(const std::string &s);
	static int toBool(const std::string &s);
	static const vec2 toVec2(const std::string &s);
	static const vec3 toVec3(const std::string &s);

private:
	static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
};

//=========================================================================
std::string stringUtils::trim(const std::string &s)
{
	return trimLeft(trimRight(s));
}

//=========================================================================
std::string stringUtils::trimLeft(const std::string &s)
{
	auto temp = s;
	temp.erase(std::begin(temp),
		std::find_if(std::begin(temp), std::end(temp),
		[](char c){return !std::isspace(c, std::locale());
	}));
	return temp;
}

//=========================================================================
std::string stringUtils::trimRight(const std::string &s)
{
	auto temp = s;
	temp.erase(std::find_if(std::rbegin(temp), std::rend(temp),
		[](char c){return !std::isspace(c, std::locale()); }).base(),
		std::end(temp));
	return temp;
}

//=========================================================================
bool stringUtils::startsWith(const std::string &s, const std::string &value)
{
	return trim(s).find(value) == 0;
}

//=========================================================================
std::string stringUtils::extract(const std::string &s, const char& between)
{
	std::string result = s;
	result = result.substr(result.find(between) + 1);
	result = result.erase(result.find(between), result.length());
	return result;
}

//=========================================================================
std::string stringUtils::cutTail(const std::string &s, const std::string &cutAt)
{
	std::string result = s;
	const size_t idx = result.find_last_of(cutAt) + 1;

	if(std::string::npos != idx)
		result.erase(idx, result.length() - idx);

	return result;
}

//=========================================================================
bool stringUtils::contains(const std::string &s, const std::string &value)
{
	return s.find(value, 0) != std::string::npos;
}

//=========================================================================
std::vector<std::string> &stringUtils::split(const std::string &s, char delim, std::vector<std::string> &elems)
{
	std::stringstream ss(s);
	std::string item;

	while (getline(ss, item, delim))
	{
		elems.push_back(item);
	}

	return elems;
}

//=========================================================================
std::vector<std::string> stringUtils::split(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

//=========================================================================
std::string stringUtils::toLower(std::string &s)
{
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

//=========================================================================
int stringUtils::toInt(const std::string &s)
{
	return atoi(s.c_str());
}

//=========================================================================
float stringUtils::toFloat(const std::string &s)
{
	return static_cast<float>(atof(s.c_str()));
}

//=========================================================================
double stringUtils::toDouble(const std::string &s)
{
	return atof(s.c_str());
}

//=========================================================================
int stringUtils::toBool(const std::string &s)
{
	auto ss = toLower(trim(s));
	return contains(ss, "1") || contains(ss, "true");
}

//=========================================================================
const vec2 stringUtils::toVec2(const std::string &s)
{
	vec2 result;
	auto ss = stringUtils::split(s, ' ');
	result.x = stringUtils::toFloat(ss[0]);
	result.y = stringUtils::toFloat(ss[1]);
	return result;
}

//=========================================================================
const vec3 stringUtils::toVec3(const std::string &s)
{
	vec3 result;
	auto ss = stringUtils::split(s, ' ');
	result.x = stringUtils::toFloat(ss[0]);
	result.y = stringUtils::toFloat(ss[1]);
	result.z = stringUtils::toFloat(ss[2]);
	return result;
}
