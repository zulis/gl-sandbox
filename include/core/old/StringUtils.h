#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "core/Math.h"

using namespace std;

class StringUtils
{
public:
	static const string WHITESPACE;
	static string trim(const string& s);
	static string trimLeft(const string& s);
	static string trimRight(const string& s);
	static bool startsWith(const string& s, const string& value);
	static string extract(const string& s, const char& between);
	static string cutTail(const string& s, const string& cutAt);
	static bool contains(const string& s, const string& value);
	static vector<string> split(const string& s, char delim);
	static string toLower(string& s);
	static int toInt(const string& s);
	static float toFloat(const string& s);
	static double toDouble(const string& s);
	static int toBool(const string& s);
	static const vec2 toVec2(const string& s);
	static const vec3 toVec3(const string& s);

private:
	static vector<string>& split(const string& s, char delim, vector<string>& elems);
};

const string StringUtils::WHITESPACE = " \n\r\t";

//=========================================================================
string StringUtils::trim(const string& s)
{
	return trimRight(trimLeft(s));
}

//=========================================================================
string StringUtils::trimLeft(const string& s)
{
	size_t startpos = s.find_first_not_of(StringUtils::WHITESPACE);
	return (startpos == string::npos) ? "" : s.substr(startpos);
}

//=========================================================================
string StringUtils::trimRight(const string& s)
{
	size_t endpos = s.find_last_not_of(StringUtils::WHITESPACE);
	return (endpos == string::npos) ? "" : s.substr(0, endpos + 1);
}

//=========================================================================
bool StringUtils::startsWith(const string& s, const string& value)
{
	return trim(s).find(value) == 0;
}

//=========================================================================
string StringUtils::extract(const string& s, const char& between)
{
	string result = s;
	result = result.substr(result.find(between) + 1);
	result = result.erase(result.find(between), result.length());
	return result;
}

//=========================================================================
string StringUtils::cutTail(const string& s, const string& cutAt)
{
	string result = s;
	const size_t idx = result.find_last_of(cutAt) + 1;

	if(string::npos != idx)
		result.erase(idx, result.length() - idx);

	return result;
}

//=========================================================================
bool StringUtils::contains(const string& s, const string& value)
{
	return s.find(value, 0) != string::npos;
}

//=========================================================================
vector<string>& StringUtils::split(const string& s, char delim, vector<string>& elems)
{
	stringstream ss(s);
	string item;

	while (getline(ss, item, delim))
	{
		elems.push_back(item);
	}

	return elems;
}

//=========================================================================
vector<string> StringUtils::split(const string& s, char delim)
{
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}

//=========================================================================
string StringUtils::toLower(string& s)
{
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

//=========================================================================
int StringUtils::toInt(const string& s)
{
	return atoi(s.c_str());
}

//=========================================================================
float StringUtils::toFloat(const string& s)
{
	return atof(s.c_str());
}

//=========================================================================
double StringUtils::toDouble(const string& s)
{
	return atof(s.c_str());
}

//=========================================================================
int StringUtils::toBool(const string& s)
{
	auto ss = toLower(trim(s));
	return contains(ss, "1") || contains(ss, "true");
}

//=========================================================================
const vec2 StringUtils::toVec2(const string& s)
{
	vec2 result;
	auto ss = StringUtils::split(s, ' ');
	result.x = StringUtils::toFloat(ss[0]);
	result.y = StringUtils::toFloat(ss[1]);
	return result;
}

//=========================================================================
const vec3 StringUtils::toVec3(const string& s)
{
	vec3 result;
	auto ss = StringUtils::split(s, ' ');
	result.x = StringUtils::toFloat(ss[0]);
	result.y = StringUtils::toFloat(ss[1]);
	result.z = StringUtils::toFloat(ss[2]);
	return result;
}
