#pragma once

#include "math/Math.h"
#include <memory>

namespace library
{
class AABB
{
public:
    enum IntersectionType
    {
        Inside,
        Intersect,
        Outside
    };

    AABB();
    AABB(const vec3 &min, const vec3 &max);
    ~AABB();

    vec3 getCenter() const;
    vec3 getSize() const;
    vec3 getPositive(const vec3 &normal) const;
    vec3 getNegative(const vec3 &normal) const;
    vec3 getMin() const;
    vec3 getMax() const;
    float getLongestEdge() const;
    float getShortestEdge() const;
    //AABB transformed(const mat4 &matrix) const;
    void extend(float value);
    void extend(const vec3 &point);
    AABB::IntersectionType intersect(const AABB &other) const;
    bool overlaps(const AABB &other) const;
    void setMin(const vec3 &min);
    void setMax(const vec3 &max);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};
}
