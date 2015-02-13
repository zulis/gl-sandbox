#pragma once

#include <memory>
#include <string>
#include "core/Geometry.h"

typedef std::unique_ptr<class Shape> ShapeRef;

class Shape
{
public:
	static GeometryRef createCube(float size = 1.0f, bool stroked = false);
};

//=========================================================================
GeometryRef Shape::createCube(float size, bool stroked)
{
	auto geometry = Geometry::create();
	geometry->setDrawType(stroked ? Geometry::DrawType::LINE : Geometry::DrawType::TRIANGLES);
	std::vector<vec3> vertices;
	std::vector<unsigned int> indices;
	auto side = size / 2.0f;

	if (stroked)
	{
		vertices =
		{
			vec3(-side, -side, -side),
			vec3(-side, -side, side),
			vec3(side, -side, -side),
			vec3(side, -side, side),
			vec3(-side, side, side),
			vec3(-side, side, -side),
			vec3(side, side, -side),
			vec3(side, side, side)
		};

		indices = { 1, 0, 0, 2, 2, 3, 3, 1, 4, 1, 4, 5, 5, 6, 6, 7, 7, 4, 7, 3, 6, 2, 5, 0 };
	}
	else
	{
		vertices =
		{
			// Front
			vec3(-side, -side, side),
			vec3(side, -side, side),
			vec3(side, side, side),
			vec3(-side, side, side),
			// Right
			vec3(side, -side, side),
			vec3(side, -side, -side),
			vec3(side, side, -side),
			vec3(side, side, side),
			// Back
			vec3(-side, -side, -side),
			vec3(-side, side, -side),
			vec3(side, side, -side),
			vec3(side, -side, -side),
			// Left
			vec3(-side, -side, side),
			vec3(-side, side, side),
			vec3(-side, side, -side),
			vec3(-side, -side, -side),
			// Bottom
			vec3(-side, -side, side),
			vec3(-side, -side, -side),
			vec3(side, -side, -side),
			vec3(side, -side, side),
			// Top
			vec3(-side, side, side),
			vec3(side, side, side),
			vec3(side, side, -side),
			vec3(-side, side, -side)
		};

		indices =
		{
			0, 1, 2, 0, 2, 3,
			4, 5, 6, 4, 6, 7,
			8, 9, 10, 8, 10, 11,
			12, 13, 14, 12, 14, 15,
			16, 17, 18, 16, 18, 19,
			20, 21, 22, 20, 22, 23
		};

		std::vector<vec3> normals =
		{
			// Front
			vec3(0.0f, 0.0f, 1.0f),
			vec3(0.0f, 0.0f, 1.0f),
			vec3(0.0f, 0.0f, 1.0f),
			vec3(0.0f, 0.0f, 1.0f),
			// Right
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			// Back
			vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 0.0f, -1.0f),
			// Left
			vec3(-1.0f, 0.0f, 0.0f),
			vec3(-1.0f, 0.0f, 0.0f),
			vec3(-1.0f, 0.0f, 0.0f),
			vec3(-1.0f, 0.0f, 0.0f),
			// Bottom
			vec3(0.0f, -1.0f, 0.0f),
			vec3(0.0f, -1.0f, 0.0f),
			vec3(0.0f, -1.0f, 0.0f),
			vec3(0.0f, -1.0f, 0.0f),
			// Top
			vec3(0.0f, 1.0f, 0.0f),
			vec3(0.0f, 1.0f, 0.0f),
			vec3(0.0f, 1.0f, 0.0f),
			vec3(0.0f, 1.0f, 0.0f)
		};

		std::vector<vec2> texCoords =
		{
			// Front
			vec2(0.0f, 0.0f),
			vec2(1.0f, 0.0f),
			vec2(1.0f, 1.0f),
			vec2(0.0f, 1.0f),
			// Right
			vec2(0.0f, 0.0f),
			vec2(1.0f, 0.0f),
			vec2(1.0f, 1.0f),
			vec2(0.0f, 1.0f),
			// Back
			vec2(0.0f, 0.0f),
			vec2(1.0f, 0.0f),
			vec2(1.0f, 1.0f),
			vec2(0.0f, 1.0f),
			// Left
			vec2(0.0f, 0.0f),
			vec2(1.0f, 0.0f),
			vec2(1.0f, 1.0f),
			vec2(0.0f, 1.0f),
			// Bottom
			vec2(0.0f, 0.0f),
			vec2(1.0f, 0.0f),
			vec2(1.0f, 1.0f),
			vec2(0.0f, 1.0f),
			// Top
			vec2(0.0f, 0.0f),
			vec2(1.0f, 0.0f),
			vec2(1.0f, 1.0f),
			vec2(0.0f, 1.0f)
		};

		geometry->setNormals(normals);
		geometry->setTexCoords(texCoords);
	}

	geometry->setVertices(vertices);
	geometry->setIndices(indices);

	return geometry;
}