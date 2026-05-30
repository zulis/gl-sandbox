#include "primitive.h"

#include <algorithm>
#include <cmath>

namespace library
{
namespace
{
Geometry makeGeometry(std::vector<vec3> vertices,
                      std::vector<unsigned int> indices,
                      std::vector<vec3> normals,
                      std::vector<vec2> texCoords)
{
    Geometry geometry;
    geometry.setVertices(vertices);
    geometry.setIndices(indices);
    geometry.setNormals(normals);
    geometry.setTexCoords(texCoords);
    return geometry;
}

vec3 coneSideNormal(float radius, float height, float x, float z)
{
    return normalize(vec3(height * x, radius, height * z));
}
}

Geometry Primitive::box(float width, float height, float depth)
{
    const float halfWidth = width * 0.5f;
    const float halfHeight = height * 0.5f;
    const float halfDepth = depth * 0.5f;

    std::vector<vec3> vertices = {
        vec3(-halfWidth, -halfHeight, halfDepth), vec3(halfWidth, -halfHeight, halfDepth), vec3(halfWidth, halfHeight, halfDepth), vec3(-halfWidth, halfHeight, halfDepth),
        vec3(halfWidth, -halfHeight, -halfDepth), vec3(-halfWidth, -halfHeight, -halfDepth), vec3(-halfWidth, halfHeight, -halfDepth), vec3(halfWidth, halfHeight, -halfDepth),
        vec3(-halfWidth, -halfHeight, -halfDepth), vec3(-halfWidth, -halfHeight, halfDepth), vec3(-halfWidth, halfHeight, halfDepth), vec3(-halfWidth, halfHeight, -halfDepth),
        vec3(halfWidth, -halfHeight, halfDepth), vec3(halfWidth, -halfHeight, -halfDepth), vec3(halfWidth, halfHeight, -halfDepth), vec3(halfWidth, halfHeight, halfDepth),
        vec3(-halfWidth, halfHeight, halfDepth), vec3(halfWidth, halfHeight, halfDepth), vec3(halfWidth, halfHeight, -halfDepth), vec3(-halfWidth, halfHeight, -halfDepth),
        vec3(-halfWidth, -halfHeight, -halfDepth), vec3(halfWidth, -halfHeight, -halfDepth), vec3(halfWidth, -halfHeight, halfDepth), vec3(-halfWidth, -halfHeight, halfDepth)};

    std::vector<unsigned int> indices;
    indices.reserve(36);
    for (unsigned int face = 0; face < 6; ++face) {
        const unsigned int offset = face * 4;
        indices.push_back(offset + 0);
        indices.push_back(offset + 1);
        indices.push_back(offset + 2);
        indices.push_back(offset + 2);
        indices.push_back(offset + 3);
        indices.push_back(offset + 0);
    }

    std::vector<vec3> normals = {
        vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f),
        vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f),
        vec3(-1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)};

    std::vector<vec2> texCoords;
    texCoords.reserve(24);
    for (int face = 0; face < 6; ++face) {
        texCoords.push_back(vec2(0.0f, 0.0f));
        texCoords.push_back(vec2(1.0f, 0.0f));
        texCoords.push_back(vec2(1.0f, 1.0f));
        texCoords.push_back(vec2(0.0f, 1.0f));
    }

    return makeGeometry(std::move(vertices), std::move(indices), std::move(normals), std::move(texCoords));
}

Geometry Primitive::plane(float width, float depth)
{
    const float halfWidth = width * 0.5f;
    const float halfDepth = depth * 0.5f;

    std::vector<vec3> vertices = {
        vec3(-halfWidth, 0.0f, -halfDepth),
        vec3(halfWidth, 0.0f, -halfDepth),
        vec3(halfWidth, 0.0f, halfDepth),
        vec3(-halfWidth, 0.0f, halfDepth)};

    std::vector<unsigned int> indices = {
        0, 2, 1,
        2, 0, 3};

    std::vector<vec3> normals(4, vec3(0.0f, 1.0f, 0.0f));

    std::vector<vec2> texCoords = {
        vec2(0.0f, 0.0f),
        vec2(1.0f, 0.0f),
        vec2(1.0f, 1.0f),
        vec2(0.0f, 1.0f)};

    return makeGeometry(std::move(vertices), std::move(indices), std::move(normals), std::move(texCoords));
}

Geometry Primitive::cone(float radius, float height, unsigned int segments)
{
    const unsigned int clampedSegments = std::max(segments, 3u);
    const float halfHeight = height * 0.5f;

    std::vector<vec3> vertices;
    std::vector<vec3> normals;
    std::vector<vec2> texCoords;
    std::vector<unsigned int> indices;

    vertices.reserve((clampedSegments + 1) * 3 + 2);
    normals.reserve((clampedSegments + 1) * 3 + 2);
    texCoords.reserve((clampedSegments + 1) * 3 + 2);
    indices.reserve(clampedSegments * 6);

    for (unsigned int segment = 0; segment <= clampedSegments; ++segment) {
        const float u = static_cast<float>(segment) / static_cast<float>(clampedSegments);
        const float angle = u * two_pi<float>();
        const float x = std::cos(angle);
        const float z = std::sin(angle);
        const vec3 normal = coneSideNormal(radius, height, x, z);

        vertices.push_back(vec3(0.0f, halfHeight, 0.0f));
        normals.push_back(normal);
        texCoords.push_back(vec2(u, 1.0f));

        vertices.push_back(vec3(radius * x, -halfHeight, radius * z));
        normals.push_back(normal);
        texCoords.push_back(vec2(u, 0.0f));
    }

    for (unsigned int segment = 0; segment < clampedSegments; ++segment) {
        const unsigned int base = segment * 2;
        const unsigned int next = base + 2;
        indices.push_back(base);
        indices.push_back(next + 1);
        indices.push_back(base + 1);
    }

    const unsigned int baseCenterIndex = static_cast<unsigned int>(vertices.size());
    vertices.push_back(vec3(0.0f, -halfHeight, 0.0f));
    normals.push_back(vec3(0.0f, -1.0f, 0.0f));
    texCoords.push_back(vec2(0.5f, 0.5f));

    for (unsigned int segment = 0; segment <= clampedSegments; ++segment) {
        const float u = static_cast<float>(segment) / static_cast<float>(clampedSegments);
        const float angle = u * two_pi<float>();
        const float x = std::cos(angle);
        const float z = std::sin(angle);

        vertices.push_back(vec3(radius * x, -halfHeight, radius * z));
        normals.push_back(vec3(0.0f, -1.0f, 0.0f));
        texCoords.push_back(vec2(x * 0.5f + 0.5f, z * 0.5f + 0.5f));
    }

    for (unsigned int segment = 0; segment < clampedSegments; ++segment) {
        indices.push_back(baseCenterIndex);
        indices.push_back(baseCenterIndex + segment + 1);
        indices.push_back(baseCenterIndex + segment + 2);
    }

    return makeGeometry(std::move(vertices), std::move(indices), std::move(normals), std::move(texCoords));
}

Geometry Primitive::sphere(float radius, unsigned int segments, unsigned int rings)
{
    const unsigned int clampedSegments = std::max(segments, 3u);
    const unsigned int clampedRings = std::max(rings, 2u);

    std::vector<vec3> vertices;
    std::vector<vec3> normals;
    std::vector<vec2> texCoords;
    std::vector<unsigned int> indices;

    vertices.reserve((clampedRings + 1) * (clampedSegments + 1));
    normals.reserve((clampedRings + 1) * (clampedSegments + 1));
    texCoords.reserve((clampedRings + 1) * (clampedSegments + 1));
    indices.reserve(clampedRings * clampedSegments * 6);

    for (unsigned int ring = 0; ring <= clampedRings; ++ring) {
        const float v = static_cast<float>(ring) / static_cast<float>(clampedRings);
        const float phi = v * pi<float>();
        const float y = std::cos(phi);
        const float ringRadius = std::sin(phi);

        for (unsigned int segment = 0; segment <= clampedSegments; ++segment) {
            const float u = static_cast<float>(segment) / static_cast<float>(clampedSegments);
            const float theta = u * two_pi<float>();
            const float x = std::cos(theta) * ringRadius;
            const float z = std::sin(theta) * ringRadius;
            const vec3 normal = vec3(x, y, z);

            vertices.push_back(normal * radius);
            normals.push_back(normalize(normal));
            texCoords.push_back(vec2(u, 1.0f - v));
        }
    }

    const unsigned int stride = clampedSegments + 1;
    for (unsigned int ring = 0; ring < clampedRings; ++ring) {
        for (unsigned int segment = 0; segment < clampedSegments; ++segment) {
            const unsigned int current = ring * stride + segment;
            const unsigned int next = current + stride;

            if (ring != 0) {
                indices.push_back(current);
                indices.push_back(current + 1);
                indices.push_back(next);
            }

            if (ring != clampedRings - 1) {
                indices.push_back(current + 1);
                indices.push_back(next + 1);
                indices.push_back(next);
            }
        }
    }

    return makeGeometry(std::move(vertices), std::move(indices), std::move(normals), std::move(texCoords));
}

Geometry Primitive::cylinder(float radius, float height, unsigned int segments)
{
    const unsigned int clampedSegments = std::max(segments, 3u);
    const float halfHeight = height * 0.5f;

    std::vector<vec3> vertices;
    std::vector<vec3> normals;
    std::vector<vec2> texCoords;
    std::vector<unsigned int> indices;

    vertices.reserve((clampedSegments + 1) * 4 + 2);
    normals.reserve((clampedSegments + 1) * 4 + 2);
    texCoords.reserve((clampedSegments + 1) * 4 + 2);
    indices.reserve(clampedSegments * 12);

    for (unsigned int segment = 0; segment <= clampedSegments; ++segment) {
        const float u = static_cast<float>(segment) / static_cast<float>(clampedSegments);
        const float angle = u * two_pi<float>();
        const float x = std::cos(angle);
        const float z = std::sin(angle);
        const vec3 normal = normalize(vec3(x, 0.0f, z));

        vertices.push_back(vec3(radius * x, -halfHeight, radius * z));
        normals.push_back(normal);
        texCoords.push_back(vec2(u, 0.0f));

        vertices.push_back(vec3(radius * x, halfHeight, radius * z));
        normals.push_back(normal);
        texCoords.push_back(vec2(u, 1.0f));
    }

    for (unsigned int segment = 0; segment < clampedSegments; ++segment) {
        const unsigned int base = segment * 2;
        const unsigned int next = base + 2;
        indices.push_back(base);
        indices.push_back(base + 1);
        indices.push_back(next + 1);
        indices.push_back(base);
        indices.push_back(next + 1);
        indices.push_back(next);
    }

    const unsigned int topCenterIndex = static_cast<unsigned int>(vertices.size());
    vertices.push_back(vec3(0.0f, halfHeight, 0.0f));
    normals.push_back(vec3(0.0f, 1.0f, 0.0f));
    texCoords.push_back(vec2(0.5f, 0.5f));

    for (unsigned int segment = 0; segment <= clampedSegments; ++segment) {
        const float u = static_cast<float>(segment) / static_cast<float>(clampedSegments);
        const float angle = u * two_pi<float>();
        const float x = std::cos(angle);
        const float z = std::sin(angle);

        vertices.push_back(vec3(radius * x, halfHeight, radius * z));
        normals.push_back(vec3(0.0f, 1.0f, 0.0f));
        texCoords.push_back(vec2(x * 0.5f + 0.5f, z * 0.5f + 0.5f));
    }

    for (unsigned int segment = 0; segment < clampedSegments; ++segment) {
        indices.push_back(topCenterIndex);
        indices.push_back(topCenterIndex + segment + 2);
        indices.push_back(topCenterIndex + segment + 1);
    }

    const unsigned int bottomCenterIndex = static_cast<unsigned int>(vertices.size());
    vertices.push_back(vec3(0.0f, -halfHeight, 0.0f));
    normals.push_back(vec3(0.0f, -1.0f, 0.0f));
    texCoords.push_back(vec2(0.5f, 0.5f));

    for (unsigned int segment = 0; segment <= clampedSegments; ++segment) {
        const float u = static_cast<float>(segment) / static_cast<float>(clampedSegments);
        const float angle = u * two_pi<float>();
        const float x = std::cos(angle);
        const float z = std::sin(angle);

        vertices.push_back(vec3(radius * x, -halfHeight, radius * z));
        normals.push_back(vec3(0.0f, -1.0f, 0.0f));
        texCoords.push_back(vec2(x * 0.5f + 0.5f, z * 0.5f + 0.5f));
    }

    for (unsigned int segment = 0; segment < clampedSegments; ++segment) {
        indices.push_back(bottomCenterIndex);
        indices.push_back(bottomCenterIndex + segment + 1);
        indices.push_back(bottomCenterIndex + segment + 2);
    }

    return makeGeometry(std::move(vertices), std::move(indices), std::move(normals), std::move(texCoords));
}
}