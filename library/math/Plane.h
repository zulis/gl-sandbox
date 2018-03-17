#pragma once

#include "math/Math.h"

namespace library
{
class Plane
{
public:
    Plane();
    Plane(const vec3 &normal, const vec3 &p);
    virtual ~Plane();

    static Plane fromPoints(const vec3 &v1, const vec3 &v2, const vec3 &v3);
    float getDistance(const vec3 &p) const;

    vec3 n;
    float d;
};
}
