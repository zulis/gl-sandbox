#include "AABB.h"

#include <limits>

namespace library
{
class AABB::Impl
{
public:
    Impl(const vec3 &min = vec3(), const vec3 &max = vec3());
    vec3 min;
    vec3 max;
};

AABB::Impl::Impl(const vec3 &min, const vec3 &max)
    : min(min), max(max)
{}

//=========================================================================

AABB::AABB()
    : AABB(vec3(std::numeric_limits<float>::max()), vec3(std::numeric_limits<float>::lowest()))
{
}

AABB::AABB(const vec3 &min, const vec3 &max)
    : impl{std::make_unique<Impl>(min, max)}
{
}

AABB::~AABB() = default;

vec3 AABB::getCenter() const
{
    return (impl->min + impl->max) * 0.5f;
}

vec3 AABB::getSize() const
{
    return (impl->max - impl->min);
}

vec3 AABB::getPositive(const vec3 &normal) const
{
    vec3 result = getMin();
    vec3 size = getSize();

    if (normal.x > 0)
        result.x += size.x;

    if (normal.y > 0)
        result.y += size.y;

    if (normal.z > 0)
        result.z += size.z;

    return (result);
}

vec3 AABB::getNegative(const vec3 &normal) const
{
    vec3 result = getMin();
    vec3 size = getSize();

    if (normal.x < 0)
        result.x += size.x;

    if (normal.y < 0)
        result.y += size.y;

    if (normal.z < 0)
        result.z += size.z;

    return (result);
}

/*AABB AABB::transformed(const mat4 &matrix) const
{
    AABB res;
    vec4 point(1, 1, 1, 1);
    float aabbIn[6] = {impl->min.x, impl->min.y, impl->min.z, impl->max.x, impl->max.y, impl->max.z};

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                point.x = aabbIn[i * 3];
                point.y = aabbIn[j * 3 + 1];
                point.z = aabbIn[k * 3 + 2];
                res.extend(vec3(matrix * point));
            }
        }
    }

    return res;
}*/

vec3 AABB::getMin() const
{
    return impl->min;
}

vec3 AABB::getMax() const
{
    return impl->max;
}

float AABB::getLongestEdge() const
{
    return compMax(getSize());
}

float AABB::getShortestEdge() const
{
    return compMin(getSize());
}

void AABB::extend(float value)
{
    impl->min -= vec3(value);
    impl->max += vec3(value);
}

void AABB::extend(const vec3 &point)
{
    impl->min = min(impl->min, point);
    impl->max = max(impl->max, point);
}

AABB::IntersectionType AABB::intersect(const AABB &other) const
{
    if ((impl->max.x < other.impl->min.x) || (impl->min.x > other.impl->max.x) ||
        (impl->max.y < other.impl->min.y) || (impl->min.y > other.impl->max.y) ||
        (impl->max.z < other.impl->min.z) || (impl->min.z > other.impl->max.z))
        return IntersectionType::Outside;

    if ((impl->min.x <= other.impl->min.x) && (impl->max.x >= other.impl->max.x) &&
        (impl->min.y <= other.impl->min.y) && (impl->max.y >= other.impl->max.y) &&
        (impl->min.z <= other.impl->min.z) && (impl->max.z >= other.impl->max.z))
        return IntersectionType::Inside;

    return IntersectionType::Intersect;
}

bool AABB::overlaps(const AABB &other) const
{
    if (other.impl->min.x > impl->max.x || other.impl->max.x < impl->min.x)
        return false;
    else if (other.impl->min.y > impl->max.y || other.impl->max.y < impl->min.y)
        return false;
    else if (other.impl->min.z > impl->max.z || other.impl->max.z < impl->min.z)
        return false;

    return true;
}

void AABB::setMin(const vec3 &min)
{
    impl->min = min;
}

void AABB::setMax(const vec3 &max)
{
    impl->max = max;
}

}
