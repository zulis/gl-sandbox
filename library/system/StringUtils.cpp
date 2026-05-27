#include "StringUtils.h"
#include <sstream>
#include <locale>
#include <algorithm>

namespace library
{

//=========================================================================
std::string StringUtils::trim(const std::string &s)
{
    return trimLeft(trimRight(s));
}

//=========================================================================
std::string StringUtils::trimLeft(const std::string &s)
{
    auto temp = s;
    temp.erase(std::begin(temp),
               std::find_if(std::begin(temp), std::end(temp),
                            [](char c)
                            {
                                return !std::isspace(c, std::locale());
                            }));
    return temp;
}

//=========================================================================
std::string StringUtils::trimRight(const std::string &s)
{
    auto temp = s;
    temp.erase(std::find_if(std::rbegin(temp), std::rend(temp),
                            [](char c)
                            { return !std::isspace(c, std::locale()); }).base(),
               std::end(temp));
    return temp;
}

//=========================================================================
bool StringUtils::startsWith(const std::string &s, const std::string &value)
{
    return trim(s).find(value) == 0;
}

//=========================================================================
std::string StringUtils::extract(const std::string &s, const char &between)
{
    std::string result = s;
    result = result.substr(result.find(between) + 1);
    result = result.erase(result.find(between), result.length());
    return result;
}

//=========================================================================
std::string StringUtils::cutTail(const std::string &s, const std::string &cutAt)
{
    std::string result = s;
    const size_t idx = result.find_last_of(cutAt) + 1;

    if (std::string::npos != idx)
        result.erase(idx, result.length() - idx);

    return result;
}

//=========================================================================
bool StringUtils::contains(const std::string &s, const std::string &value)
{
    return s.find(value, 0) != std::string::npos;
}

//=========================================================================
std::vector<std::string> StringUtils::split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }

    return elems;
}

//=========================================================================
std::string StringUtils::toLower(std::string &s)
{
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

//=========================================================================
int StringUtils::toInt(const std::string &s)
{
    return atoi(s.c_str());
}

//=========================================================================
float StringUtils::toFloat(const std::string &s)
{
    return static_cast<float>(atof(s.c_str()));
}

//=========================================================================
double StringUtils::toDouble(const std::string &s)
{
    return atof(s.c_str());
}

//=========================================================================
int StringUtils::toBool(const std::string &s)
{
    auto st = trim(s);
    auto ss = toLower(st);
    return contains(ss, "1") || contains(ss, "true");
}

//=========================================================================
vec2 StringUtils::toVec2(const std::string &s)
{
    vec2 result;
    auto ss = StringUtils::split(s, ' ');
    result.x = StringUtils::toFloat(ss[0]);
    result.y = StringUtils::toFloat(ss[1]);
    return result;
}

//=========================================================================
vec3 StringUtils::toVec3(const std::string &s)
{
    vec3 result;
    auto ss = StringUtils::split(s, ' ');
    result.x = StringUtils::toFloat(ss[0]);
    result.y = StringUtils::toFloat(ss[1]);
    result.z = StringUtils::toFloat(ss[2]);
    return result;
}

}