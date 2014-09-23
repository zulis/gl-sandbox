#pragma once

#include <vector>
#include "Spatial/Spatial.h"
#include "Core/Mesh.h"
#include "Core/Camera.h"

typedef std::shared_ptr<class MeshSpatial> MeshSpatialRef;

class MeshSpatial : private Mesh, public Spatial
{
public:
	static MeshSpatialRef create();

	virtual void setMatrix(glm::mat4 matrix);
	virtual void update(float deltaTime);
	virtual void draw(const CameraRef& camera);
	virtual bool isCulled(const Camera& camera);

	void loadFromFile(const std::string& fileName);
	void setFrustumCulling(bool isOn);
	void setMaterial(const MaterialRef& material);
	void setTexturePath(const std::string& texturePath);

private:
	MeshSpatial();

};

//=========================================================================
MeshSpatialRef MeshSpatial::create()
{
	return MeshSpatialRef(new MeshSpatial);
}

//=========================================================================
MeshSpatial::MeshSpatial() : Mesh()
{
}

//=========================================================================
void MeshSpatial::setMatrix(glm::mat4 matrix)
{
	Mesh::setMatrix(matrix);
}

//=========================================================================
void MeshSpatial::update(float deltaTime)
{
}

//=========================================================================
void MeshSpatial::draw(const CameraRef& camera)
{
	Mesh::draw(camera);
}

//=========================================================================
void MeshSpatial::setFrustumCulling(bool isOn)
{
	Mesh::setFrustumCulling(isOn);
}

//=========================================================================
void MeshSpatial::setMaterial(const MaterialRef& material)
{
	Mesh::setMaterial(material);
}

//=========================================================================
void MeshSpatial::setTexturePath(const std::string& texturePath)
{
	Mesh::setTexturePath(texturePath);
}

//=========================================================================
bool MeshSpatial::isCulled(const Camera& camera)
{
	auto aabb = Mesh::getAABB().transformed(Mesh::getMatrix());
	return !camera.intersects(aabb);
}

//=========================================================================
void MeshSpatial::loadFromFile(const std::string& fileName)
{
	Mesh::loadFromFile(fileName);
}
