#pragma once

#include <string>
#include <vector>
#include "math/Math.h"

namespace library
{
class StringUtils
{
public:
    static std::string trim(const std::string &s);
    static std::string trimLeft(const std::string &s);
    static std::string trimRight(const std::string &s);
    static bool startsWith(const std::string &s, const std::string &value);
    static std::string extract(const std::string &s, const char &between);
    static std::string cutTail(const std::string &s, const std::string &cutAt);
    static bool contains(const std::string &s, const std::string &value);
    static std::vector<std::string> split(const std::string &s, char delim);
    static std::string toLower(std::string &s);
    static int toInt(const std::string &s);
    static float toFloat(const std::string &s);
    static double toDouble(const std::string &s);
    static int toBool(const std::string &s);
    static vec2 toVec2(const std::string &s);
    static vec3 toVec3(const std::string &s);
};
}