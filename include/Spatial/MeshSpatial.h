#pragma once

#include <vector>
#include "Core/Mesh.h"

typedef std::shared_ptr<class MeshSpatial> MeshSpatialRef;

class MeshSpatial : public Mesh
{
public:
	static MeshSpatialRef create();
	MeshSpatial();
	virtual ~MeshSpatial();

private:
	

};

//=========================================================================
MeshSpatialRef MeshSpatial::create()
{
	return MeshSpatialRef(new MeshSpatial);
}

//=========================================================================
MeshSpatial::MeshSpatial()
{
}

//=========================================================================
MeshSpatial::~MeshSpatial()
{

}