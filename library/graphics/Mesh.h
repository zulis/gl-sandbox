#pragma once

#include <string>
#include <memory>

namespace library
{
class Mesh
{
public:
    Mesh();
    virtual ~Mesh();

    void fromFile(const std::string &fileName);
    void draw();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};
}
