#pragma once

#include <memory>
#include <vector>
#include <limits>
#include <functional>
#include "core/Math.h"
#include "core/Shader.h"
#include "core/AABB.h"

typedef std::unique_ptr<class Geometry> GeometryRef;

class Geometry final
{
public:
	enum DrawType
	{
		TRIANGLES = 0x0004,     // GL_TRIANGLES
		TRIANGLESTRIP = 0x0005, // GL_TRIANGLE_STRIP
		TRIANGLEFAN = 0x0006,   // GL_TRIANGLE_FAN
		QUADS = 0x0007,         // GL_QUADS
		POINTS = 0x0000,        // GL_POINTS
		LINE = 0x0001,          // GL_LINE
		LINELOOP = 0x0002,      // GL_LINE_LOOP
		POLYGON = 0x0009        // GL_POLYGON
	};

	static GeometryRef create();
	static GeometryRef create(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices);
	Geometry() {};
	Geometry(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices);
	~Geometry();

	void setVertices(const std::vector<glm::vec3>& vertices);
	void setVertices(const std::vector<glm::vec2>& vertices);
	void setIndices(const std::vector<unsigned int>& indices);
	void setTexCoords(const std::vector<glm::vec2>& texCoords);
	void setNormals(const std::vector<glm::vec3>& normals);
	void setTangents(const std::vector<glm::vec4>& tangents);
	void setBitangents(const std::vector<glm::vec3>& bitangents);
	void setDrawType(DrawType drawType);

	const std::vector<glm::vec3> getVertices() const;
	const std::vector<unsigned int> getIndices() const;
	const std::vector<glm::vec2> getTexCoords() const;
	const std::vector<glm::vec3> getNormals() const;
	const std::vector<glm::vec4> getTangents() const;
	const std::vector<glm::vec3> getBitangents() const;
	unsigned int getDrawType() const;
	const AABB getAABB() const;

	bool hasIndices() const;
	bool hasTexCoords() const;
	bool hasNormals() const;
	bool hasTangents() const;
	bool hasBitangents() const;

	void draw(const Shader& shader);

private:
	DrawType mDrawType { DrawType::TRIANGLES };
	std::vector<glm::vec3> mVertices;
	std::vector<unsigned int> mIndices;
	std::vector<glm::vec2> mTexCoords;
	std::vector<glm::vec3> mNormals;
	std::vector<glm::vec4> mTangents;
	std::vector<glm::vec3> mBitangents;
	bool mIsReady { false };
	GLuint mVaoHandle;
	GLuint mVboHandle[6];

	Geometry(const Geometry&) = delete;
	Geometry& operator=(const Geometry&) = delete;
	void prepare(const Shader& shader);
	void generateNormals();
	void generateTangents();
};
