#include "FileSystem.h"
#include <sstream>
#include <fstream>

namespace library
{

const char *FileSystem::readFile(const char *fileName)
{
    std::ostringstream ss;
    std::ifstream fs(fileName, std::ios::binary);
    ss << fs.rdbuf();
    const std::string str(ss.str());
    return str.c_str();
}
}