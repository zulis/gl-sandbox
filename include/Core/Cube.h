#pragma once

#include <memory>
#include <string>
#include "Core/Drawable.h"
#include "Core/Transform3d.h"
#include "Core/AABB.h"

typedef std::shared_ptr<class Cube> CubeRef;

class Cube : public Drawable, public Transform3D
{
	public:
		static CubeRef create(float size = 1.0f, bool stroked = false);
		static CubeRef create(AABB aabb);

		Cube(float size = 1.0f, bool stroked = false);
		Cube(AABB aabb);
		~Cube();

		void draw(const CameraRef& camera);

	protected:
		virtual void setGeometry(std::vector<GeometryRef>* geometries);

	private:
		std::vector<glm::vec3> mVertices;
		std::vector<unsigned int> mIndices;
		std::vector<glm::vec2> mTexCoords;
		std::vector<glm::vec3> mNormals;
		Geometry::DrawType mDrawType { Geometry::DrawType::TRIANGLES };

};

//=========================================================================
CubeRef Cube::create(float size, bool stroked)
{
	return CubeRef(new Cube(size, stroked));
}

//=========================================================================
CubeRef Cube::create(AABB aabb)
{
	return CubeRef(new Cube(aabb));
}

//=========================================================================
Cube::Cube(float size, bool stroked)
{
	mDrawType = stroked ? Geometry::DrawType::LINE : Geometry::DrawType::TRIANGLES;
	float side = size / 2.0f;

	if(stroked)
	{
		mVertices =
		{
			glm::vec3(-side, -side, -side),
			glm::vec3(-side, -side, side),
			glm::vec3(side, -side, -side),
			glm::vec3(side, -side, side),
			glm::vec3(-side, side, side),
			glm::vec3(-side, side, -side),
			glm::vec3(side, side, -side),
			glm::vec3(side, side, side)
		};

		mIndices = { 1, 0, 0, 2, 2, 3, 3, 1, 4, 1, 4, 5, 5, 6, 6, 7, 7, 4, 7, 3, 6, 2, 5, 0 };
	}
	else
	{
		mVertices =
		{
			// Front
			glm::vec3(-side, -side, side),
			glm::vec3(side, -side, side),
			glm::vec3(side, side, side),
			glm::vec3(-side, side, side),
			// Right
			glm::vec3(side, -side, side),
			glm::vec3(side, -side, -side),
			glm::vec3(side, side, -side),
			glm::vec3(side, side, side),
			// Back
			glm::vec3(-side, -side, -side),
			glm::vec3(-side, side, -side),
			glm::vec3(side, side, -side),
			glm::vec3(side, -side, -side),
			// Left
			glm::vec3(-side, -side, side),
			glm::vec3(-side, side, side),
			glm::vec3(-side, side, -side),
			glm::vec3(-side, -side, -side),
			// Bottom
			glm::vec3(-side, -side, side),
			glm::vec3(-side, -side, -side),
			glm::vec3(side, -side, -side),
			glm::vec3(side, -side, side),
			// Top
			glm::vec3(-side, side, side),
			glm::vec3(side, side, side),
			glm::vec3(side, side, -side),
			glm::vec3(-side, side, -side)
		};

		mIndices =
		{
			0, 1, 2, 0, 2, 3,
			4, 5, 6, 4, 6, 7,
			8, 9, 10, 8, 10, 11,
			12, 13, 14, 12, 14, 15,
			16, 17, 18, 16, 18, 19,
			20, 21, 22, 20, 22, 23
		};

		mNormals =
		{
			// Front
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			// Right
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			// Back
			glm::vec3(0.0f, 0.0f, -1.0f),
			glm::vec3(0.0f, 0.0f, -1.0f),
			glm::vec3(0.0f, 0.0f, -1.0f),
			glm::vec3(0.0f, 0.0f, -1.0f),
			// Left
			glm::vec3(-1.0f, 0.0f, 0.0f),
			glm::vec3(-1.0f, 0.0f, 0.0f),
			glm::vec3(-1.0f, 0.0f, 0.0f),
			glm::vec3(-1.0f, 0.0f, 0.0f),
			// Bottom
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f),
			// Top
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		};

		mTexCoords =
		{
			// Front
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			// Right
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			// Back
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			// Left
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			// Bottom
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			// Top
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f)
		};
	}
}

//=========================================================================
Cube::Cube(AABB aabb)
{
	mDrawType = Geometry::DrawType::LINE;

	mVertices =
	{
		glm::vec3(aabb.getMin().x, aabb.getMin().y, aabb.getMin().z),
		glm::vec3(aabb.getMin().x, aabb.getMin().y, aabb.getMax().z),
		glm::vec3(aabb.getMax().x, aabb.getMin().y, aabb.getMin().z),
		glm::vec3(aabb.getMax().x, aabb.getMin().y, aabb.getMax().z),
		glm::vec3(aabb.getMin().x, aabb.getMax().y, aabb.getMax().z),
		glm::vec3(aabb.getMin().x, aabb.getMax().y, aabb.getMin().z),
		glm::vec3(aabb.getMax().x, aabb.getMax().y, aabb.getMin().z),
		glm::vec3(aabb.getMax().x, aabb.getMax().y, aabb.getMax().z)
	};

	mIndices =
	{
		1, 0, 0, 2, 2, 3, 3, 1, 4, 1, 4, 5, 5, 6, 6, 7, 7, 4, 7, 3, 6, 2, 5, 0
	};
}

//=========================================================================
Cube::~Cube()
{
}

//=========================================================================
void Cube::setGeometry(std::vector<GeometryRef>* geometries)
{
	auto geometry = GeometryRef(new Geometry);
	geometry->setVertices(mVertices);
	geometry->setIndices(mIndices);

	if(mNormals.size() > 0)
		geometry->setNormals(mNormals);

	if(mTexCoords.size() > 0)
		geometry->setTexCoords(mTexCoords);

	geometry->setDrawType(mDrawType);
	geometries->push_back(geometry);
}

//=========================================================================
void Cube::draw(const CameraRef& camera)
{
	Drawable::draw(camera->getProjectionMatrix(), camera->getViewMatrix(), getMatrix());
}