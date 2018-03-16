#pragma once

#include "math/Math.h"
//#include "math/AABB.h"
#include <vector>
#include <memory>

namespace library
{
class Geometry
{
public:
    enum DrawType
    {
        Triangles = 0x0004,     // GL_TRIANGLES
        TriangleStrip = 0x0005, // GL_TRIANGLE_STRIP
        TriangleFan = 0x0006,   // GL_TRIANGLE_FAN
        Quads = 0x0007,         // GL_QUADS
        Points = 0x0000,        // GL_POINTS
        Line = 0x0001,          // GL_LINE
        LineLoop = 0x0002,      // GL_LINE_LOOP
        Polygon = 0x0009        // GL_POLYGON
    };

    Geometry();
    Geometry(std::vector<vec3> vertices, std::vector<unsigned int> indices);
    virtual ~Geometry();

    void setVertices(const std::vector<vec3> &vertices);
    void setVertices(const std::vector<vec2> &vertices);
    void setIndices(const std::vector<unsigned int> &indices);
    void setTexCoords(const std::vector<vec2> &texCoords);
    void setNormals(const std::vector<vec3> &normals);
    void setTangents(const std::vector<vec3> &tangents);
    void setBitangents(const std::vector<vec3> &bitangents);

    void addVertices(const std::vector<vec3> &vertices);
    void addVertices(const std::vector<vec2> &vertices);
    void addIndices(const std::vector<unsigned int> &indices);
    void addTexCoords(const std::vector<vec2> &texCoords);
    void addNormals(const std::vector<vec3> &normals);
    void addTangents(const std::vector<vec3> &tangents);
    void addBitangents(const std::vector<vec3> &bitangents);

    void setDrawType(DrawType drawType);

    const std::vector<vec3> getVertices() const;
    const std::vector<unsigned int> getIndices() const;
    const std::vector<vec2> getTexCoords() const;
    const std::vector<vec3> getNormals() const;
    const std::vector<vec3> getTangents() const;
    const std::vector<vec3> getBitangents() const;
    unsigned int getDrawType() const;
    //const AABB getAABB() const;

    bool hasIndices() const;
    bool hasTexCoords() const;
    bool hasNormals() const;
    bool hasTangents() const;
    bool hasBitangents() const;

    void draw();
private:
    class Impl;
    std::unique_ptr<Impl> impl;
};
}
