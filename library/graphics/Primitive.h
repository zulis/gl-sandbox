#pragma once

#include "Geometry.h"

namespace library
{
class Primitive
{
public:
    static Geometry box(float width = 1.0f, float height = 1.0f, float depth = 1.0f);
    static Geometry plane(float width = 1.0f, float depth = 1.0f);
    static Geometry cone(float radius = 0.5f, float height = 1.0f, unsigned int segments = 32);
    static Geometry sphere(float radius = 0.5f, unsigned int segments = 32, unsigned int rings = 16);
    static Geometry cylinder(float radius = 0.5f, float height = 1.0f, unsigned int segments = 32);
    static Geometry capsule(float radius = 0.25f, float height = 1.0f, unsigned int segments = 32);
};
}
