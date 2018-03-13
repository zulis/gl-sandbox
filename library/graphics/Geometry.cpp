#include "Geometry.h"
#include "GL.h"

namespace library
{
class Geometry::Impl
{
public:
    DrawType drawType{DrawType::TRIANGLES};
    std::vector<vec3> vertices;
    std::vector<unsigned int> indices;
    std::vector<vec2> texCoords;
    std::vector<vec3> normals;
    std::vector<vec3> tangents;
    std::vector<vec3> bitangents;
    GLuint vaoHandle;
    GLuint vboHandle[6];
    bool prepared;

    bool hasIndices() const;
    bool hasTexCoords() const;
    bool hasNormals() const;
    bool hasTangents() const;
    bool hasBitangents() const;
    void generateNormals();
    void generateTangents();
    void prepare();

};

bool Geometry::Impl::hasIndices() const
{
    return indices.size() > 0;
}

bool Geometry::Impl::hasTexCoords() const
{
    return texCoords.size() > 0;
}

bool Geometry::Impl::hasNormals() const
{
    return normals.size() > 0;
}

bool Geometry::Impl::hasTangents() const
{
    return tangents.size() > 0;
}

bool Geometry::Impl::hasBitangents() const
{
    return bitangents.size() > 0;
}

void Geometry::Impl::generateNormals()
{
    normals.assign(vertices.size(), vec3(0.0f));

    unsigned int n = indices.size() / 3;

    for (unsigned int i = 0; i < n; ++i) {
        unsigned int index0 = indices[i * 3];
        unsigned int index1 = indices[i * 3 + 1];
        unsigned int index2 = indices[i * 3 + 2];

        vec3 v0 = vertices[index0];
        vec3 v1 = vertices[index1];
        vec3 v2 = vertices[index2];

        vec3 e0 = v1 - v0;
        vec3 e1 = v2 - v0;
        vec3 normal = normalize(cross(e0, e1));

        normals[index0] += normal;
        normals[index1] += normal;
        normals[index2] += normal;
    }

    for (unsigned int i = 0; i < normals.size(); i++) {
        normals[i] = normalize(normals[i]);
    }
}

void Geometry::Impl::generateTangents()
{
    if (hasIndices() && hasNormals() && hasTexCoords()) {
        std::vector<vec3> tan1Accum;
        std::vector<vec3> tan2Accum;

        for (unsigned int i = 0; i < vertices.size(); i++) {
            tan1Accum.push_back(vec3(0.0f));
            tan2Accum.push_back(vec3(0.0f));
            tangents.push_back(vec3(0.0f));
        }

        // Compute the tangent vector
        for (unsigned int i = 0; i < indices.size(); i += 3) {
            const vec3 &p1 = vertices[indices[i]];
            const vec3 &p2 = vertices[indices[i + 1]];
            const vec3 &p3 = vertices[indices[i + 2]];

            const vec2 &tc1 = texCoords[indices[i]];
            const vec2 &tc2 = texCoords[indices[i + 1]];
            const vec2 &tc3 = texCoords[indices[i + 2]];

            vec3 q1 = p2 - p1;
            vec3 q2 = p3 - p1;
            float s1 = tc2.x - tc1.x, s2 = tc3.x - tc1.x;
            float t1 = tc2.y - tc1.y, t2 = tc3.y - tc1.y;
            float r = 1.0f / (s1 * t2 - s2 * t1);
            vec3 tan1((t2 * q1.x - t1 * q2.x) * r,
                      (t2 * q1.y - t1 * q2.y) * r,
                      (t2 * q1.z - t1 * q2.z) * r);
            vec3 tan2((s1 * q2.x - s2 * q1.x) * r,
                      (s1 * q2.y - s2 * q1.y) * r,
                      (s1 * q2.z - s2 * q1.z) * r);
            tan1Accum[indices[i]] += tan1;
            tan1Accum[indices[i + 1]] += tan1;
            tan1Accum[indices[i + 2]] += tan1;
            tan2Accum[indices[i]] += tan2;
            tan2Accum[indices[i + 1]] += tan2;
            tan2Accum[indices[i + 2]] += tan2;
        }

        for (unsigned int i = 0; i < vertices.size(); ++i) {
            const vec3 &n = normals[i];
            vec3 &t1 = tan1Accum[i];
            vec3 &t2 = tan2Accum[i];

            // Gram-Schmidt orthogonalize
            //tangents[i] = vec3(normalize(t1 - (dot(n, t1) * n)));
            tangents[i] = vec4(normalize(t1 - (dot(n, t1) * n)), 0.0f);
            // Store handedness in w
            //tangents[i].w = (dot(cross(n, t1), t2) < 0.0f) ? -1.0f : 1.0f;
        }

        tan1Accum.clear();
        tan2Accum.clear();
    }
}

void Geometry::Impl::prepare()
{
    if (!hasNormals())
        generateNormals();

    if (!hasTangents())
        generateTangents();

    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    glGenBuffers(6, vboHandle);

    // Vertex Positions
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vboHandle[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *) NULL + (0)));

    // Vertex Normals
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vboHandle[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * 3 * sizeof(float), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *) NULL + (0)));

    // Vertex Texture Coords
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, vboHandle[2]);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * 2 * sizeof(float), &texCoords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte *) NULL + (0)));

    // Vertex Tangent
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, vboHandle[3]);
    glBufferData(GL_ARRAY_BUFFER, tangents.size() * 3 * sizeof(float), &tangents[0], GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *) NULL + (0)));

    // Vertex Bitangent
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, vboHandle[4]);
    glBufferData(GL_ARRAY_BUFFER, bitangents.size() * 3 * sizeof(float), &bitangents[0], GL_STATIC_DRAW);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *) NULL + (0)));

    if (indices.size() > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandle[5]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    }

    glBindVertexArray(0);

    prepared = true;
}

//=========================================================================
Geometry::Geometry()
    : impl{std::make_unique<Impl>()}
{
}

Geometry::Geometry(std::vector<vec3> vertices, std::vector<unsigned int> indices)
    : impl{std::make_unique<Impl>()}
{
    setVertices(vertices);
    setIndices(indices);
}

Geometry::~Geometry()
{
    impl->vertices.clear();
    impl->indices.clear();
    impl->texCoords.clear();
    impl->normals.clear();
    impl->tangents.clear();
    impl->bitangents.clear();

    glBindVertexArray(0);

    for (auto &hande : impl->vboHandle)
        glDeleteBuffers(1, &hande);

    glDeleteVertexArrays(1, &impl->vaoHandle);
}

void Geometry::setVertices(const std::vector<vec3> &vertices)
{
    impl->vertices = vertices;
}

void Geometry::setVertices(const std::vector<vec2> &vertices)
{
    std::vector<vec3> result;

    for (auto &v : vertices) {
        result.push_back(vec3(v.x, v.y, 0.0f));
    }

    impl->vertices = result;
}

void Geometry::setIndices(const std::vector<unsigned int> &indices)
{
    impl->indices = indices;
}

void Geometry::setTexCoords(const std::vector<vec2> &texCoords)
{
    impl->texCoords = texCoords;
}

void Geometry::setNormals(const std::vector<vec3> &normals)
{
    impl->normals = normals;
}

void Geometry::setTangents(const std::vector<vec3> &tangents)
{
    impl->tangents = tangents;
}

void Geometry::setBitangents(const std::vector<vec3> &bitangents)
{
    impl->bitangents = bitangents;
}

void Geometry::addVertices(const std::vector<vec3> &vertices)
{
    for (auto &val : vertices)
        impl->vertices.emplace_back(val);
}

void Geometry::addVertices(const std::vector<vec2> &vertices)
{
    for (auto &val : vertices)
        impl->vertices.emplace_back(vec3(val.x, val.y, 0.0f));
}

void Geometry::addIndices(const std::vector<unsigned int> &indices)
{
    for (auto &val : indices)
        impl->indices.emplace_back(val);
}

void Geometry::addTexCoords(const std::vector<vec2> &texCoords)
{
    for (auto &val : texCoords)
        impl->texCoords.emplace_back(val);
}

void Geometry::addNormals(const std::vector<vec3> &normals)
{
    for (auto &val : normals)
        impl->normals.emplace_back(val);
}

void Geometry::addTangents(const std::vector<vec3> &tangents)
{
    for (auto &val : tangents)
        impl->tangents.emplace_back(val);
}

void Geometry::addBitangents(const std::vector<vec3> &bitangents)
{
    for (auto &val : bitangents)
        impl->bitangents.emplace_back(val);
}

void Geometry::setDrawType(DrawType drawType)
{
    impl->drawType = drawType;
}

const std::vector<vec3> Geometry::getVertices() const
{
    return impl->vertices;
}

const std::vector<unsigned int> Geometry::getIndices() const
{
    return impl->indices;
}

const std::vector<vec2> Geometry::getTexCoords() const
{
    return impl->texCoords;
}

const std::vector<vec3> Geometry::getNormals() const
{
    return impl->normals;
}

const std::vector<vec3> Geometry::getTangents() const
{
    return impl->tangents;
}

const std::vector<vec3> Geometry::getBitangents() const
{
    return impl->bitangents;
}

unsigned int Geometry::getDrawType() const
{
    return impl->drawType;
}

bool Geometry::hasIndices() const
{
    return impl->hasIndices();
}

bool Geometry::hasTexCoords() const
{
    return impl->hasTexCoords();
}

bool Geometry::hasNormals() const
{
    return impl->hasNormals();
}

bool Geometry::hasTangents() const
{
    return impl->hasTangents();
}

bool Geometry::hasBitangents() const
{
    return impl->hasBitangents();
}

//void Geometry::prepare(const Shader& shader)
//{
//	if (!hasNormals() && shader.hasAttribute(ShaderConstants::VertexNormal))
//		generateNormals();
//
//	if (!hasTangents() && shader.hasAttribute(ShaderConstants::VertexTangent))
//		generateTangents();
//
//	GLuint locVertices = shader.getAttribute(ShaderConstants::VertexPosition);
//
//	glGenVertexArrays(1, &impl->vaoHandle);
//	glBindVertexArray(impl->vaoHandle);
//
//	glGenBuffers(6, impl->vboHandle);
//
//	glBindBuffer(GL_ARRAY_BUFFER, impl->vboHandle[0]);
//	glBufferData(GL_ARRAY_BUFFER, impl->vertices.size() * 3 * sizeof(float), &impl->vertices[0], GL_STATIC_DRAW);
//	glVertexAttribPointer(locVertices, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
//	glEnableVertexAttribArray(locVertices);
//
//	if (hasNormals() && shader.hasAttribute(ShaderConstants::VertexNormal))
//	{
//		GLuint locNormals = shader.getAttribute(ShaderConstants::VertexNormal);
//		glBindBuffer(GL_ARRAY_BUFFER, impl->vboHandle[1]);
//		glBufferData(GL_ARRAY_BUFFER, impl->normals.size() * 3 * sizeof(float), &impl->normals[0], GL_STATIC_DRAW);
//		glVertexAttribPointer(locNormals, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
//		glEnableVertexAttribArray(locNormals);
//	}
//
//	if (hasTangents() && shader.hasAttribute(ShaderConstants::VertexTangent))
//	{
//		GLuint locTangents = shader.getAttribute(ShaderConstants::VertexTangent);
//		glBindBuffer(GL_ARRAY_BUFFER, impl->vboHandle[2]);
//		glBufferData(GL_ARRAY_BUFFER, impl->tangents.size() * 4 * sizeof(float), &impl->tangents[0], GL_STATIC_DRAW);
//		glVertexAttribPointer(locTangents, 4, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
//		glEnableVertexAttribArray(locTangents);
//	}
//
//	if (hasBitangents() && shader.hasAttribute(ShaderConstants::VertexBitangent))
//	{
//		GLuint locBitangents = shader.getAttribute(ShaderConstants::VertexBitangent);
//		glBindBuffer(GL_ARRAY_BUFFER, impl->vboHandle[3]);
//		glBufferData(GL_ARRAY_BUFFER, impl->bitangents.size() * 3 * sizeof(float), &impl->bitangents[0], GL_STATIC_DRAW);
//		glVertexAttribPointer(locBitangents, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
//		glEnableVertexAttribArray(locBitangents);
//	}
//
//	if (hasTexCoords() && shader.hasAttribute(ShaderConstants::VertexTexCoord))
//	{
//		GLuint locTexCoords = shader.getAttribute(ShaderConstants::VertexTexCoord);
//		glBindBuffer(GL_ARRAY_BUFFER, impl->vboHandle[4]);
//		glBufferData(GL_ARRAY_BUFFER, impl->texCoords.size() * 2 * sizeof(float), &impl->texCoords[0], GL_STATIC_DRAW);
//		glVertexAttribPointer(locTexCoords, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
//		glEnableVertexAttribArray(locTexCoords);
//	}
//
//	if (impl->indices.size() > 0)
//	{
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, impl->vboHandle[5]);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, impl->indices.size() * sizeof(unsigned int), &impl->indices[0], GL_STATIC_DRAW);
//	}
//
//	glBindVertexArray(0);
//}

void Geometry::draw()
{
    if (!impl->prepared) {
        impl->prepare();
    }

    glBindVertexArray(impl->vaoHandle);

    if (impl->indices.size() > 0) {
        glDrawElements(impl->drawType, impl->indices.size() * sizeof(unsigned int), GL_UNSIGNED_INT, ((GLubyte *) NULL + (0)));
    }
    else {
        glDrawArrays(impl->drawType, 0, impl->vertices.size() * 3 * sizeof(float));
    }

    glBindVertexArray(0);
}

/*const AABB Geometry::getAABB() const
{
    auto minFloat = std::numeric_limits<float>::min();
    auto maxFloat = std::numeric_limits<float>::max();
    vec3 min(maxFloat);
    vec3 max(minFloat);

    for (unsigned int i = 0; i < impl->vertices.size(); i++) {
        auto tmp = impl->vertices[i];

        if (tmp.x > max.x)
            max.x = tmp.x;

        if (tmp.x < min.x)
            min.x = tmp.x;

        if (tmp.y > max.y)
            max.y = tmp.y;

        if (tmp.y < min.y)
            min.y = tmp.y;

        if (tmp.z > max.z)
            max.z = tmp.z;

        if (tmp.z < min.z)
            min.z = tmp.z;
    }

    return AABB(min, max);
}*/

}
