#pragma once

#include <vector>
#include "Spatial/Spatial.h"
#include "Core/Mesh.h"
#include "Core/Camera.h"

typedef std::shared_ptr<class MeshSpatial> MeshSpatialRef;

class MeshSpatial : private Mesh, public Spatial
{
public:
	static MeshSpatialRef create(const std::string& fileName);

	virtual void setMatrix(glm::mat4 matrix);
	virtual void update(float deltaTime);
	virtual void draw(const CameraRef& camera);
	virtual bool isCulled(const Camera& camera);

	void setFrustumCulling(bool isOn);
	void setMaterial(const MaterialRef& material);
	const std::vector<Material::GeometryMaterial>& getGeomMaterial();
	void setTexturePath(const std::string& texturePath);

private:
	MeshSpatial(const std::string& fileName);

};

//=========================================================================
MeshSpatialRef MeshSpatial::create(const std::string& fileName)
{
	return MeshSpatialRef(new MeshSpatial(fileName));
}

//=========================================================================
MeshSpatial::MeshSpatial(const std::string& fileName) : Mesh(fileName)
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
const std::vector<Material::GeometryMaterial>& MeshSpatial::getGeomMaterial()
{
	return Mesh::getGeomMaterial();
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
