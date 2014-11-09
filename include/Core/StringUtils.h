#pragma once

#include <string>

class StringUtils
{
public:
	static const std::string WHITESPACE;
	static std::string trim(const std::string& s);
	static std::string trimLeft(const std::string& s);
	static std::string trimRight(const std::string& s);
	static bool startsWith(const std::string& s, const std::string& value);
	static std::string extract(const std::string& s, const char& between);
	static std::string cutTail(const std::string& s, const std::string& cutAt);
	static bool contains(const std::string& s, const std::string& value);
};

const std::string StringUtils::WHITESPACE = " \n\r\t";

//=========================================================================
std::string StringUtils::trim(const std::string& s)
{
	return trimRight(trimLeft(s));
}

//=========================================================================
std::string StringUtils::trimLeft(const std::string& s)
{
	size_t startpos = s.find_first_not_of(StringUtils::WHITESPACE);
	return (startpos == std::string::npos) ? "" : s.substr(startpos);
}

//=========================================================================
std::string StringUtils::trimRight(const std::string& s)
{
	size_t endpos = s.find_last_not_of(StringUtils::WHITESPACE);
	return (endpos == std::string::npos) ? "" : s.substr(0, endpos + 1);
}

//=========================================================================
bool StringUtils::startsWith(const std::string& s, const std::string& value)
{
	return trim(s).find(value) == 0;
}

//=========================================================================
std::string StringUtils::extract(const std::string& s, const char& between)
{
	std::string result = s;
	result = result.substr(result.find(between) + 1);
	result = result.erase(result.find(between), result.length());
	return result;
}

//=========================================================================
std::string StringUtils::cutTail(const std::string& s, const std::string& cutAt)
{
	std::string result = s;
	const size_t idx = result.find_last_of(cutAt) + 1;

	if(std::string::npos != idx)
		result.erase(idx, result.length() - idx);

	return result;
}

//=========================================================================
bool StringUtils::contains(const std::string& s, const std::string& value)
{
	return s.find(value, 0) != std::string::npos;
}
