#pragma once

#include <memory>
#include <Artemis.h>
#include "Core/Mesh.h"
#include "Core/Camera.h"

class MeshRenderComponent : public Component
{
public:
	MeshRenderComponent(MeshRef mesh, CameraRef camera);
	MeshRef getMesh() const;
	CameraRef getCamera() const;
	bool isCulled(const Camera& camera);

private:
	MeshRef mMesh;
	CameraRef mCamera;
};

//=========================================================================
MeshRenderComponent::MeshRenderComponent(MeshRef mesh, CameraRef camera)
{
	mMesh = mesh;
	mCamera = camera;
}

//=========================================================================
MeshRef MeshRenderComponent::getMesh() const
{
	return mMesh;
}

//=========================================================================
CameraRef MeshRenderComponent::getCamera() const
{
	return mCamera;
}

//=========================================================================
bool MeshRenderComponent::isCulled(const Camera& camera)
{
	//return mRenderable->isCulled(camera);
	return false;
}
