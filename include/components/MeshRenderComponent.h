#pragma once

#include <memory>
#include <functional>
#include <Artemis.h>
#include "core/Mesh.h"
#include "core/Camera.h"

class MeshRenderComponent : public Component
{
public:
	MeshRenderComponent(MeshPtr mesh, CameraPtr camera);
	const Mesh& getMesh() const;
	const Camera& getCamera() const;
	bool isCulled(const Camera& camera);

private:
	MeshPtr mMesh;
	CameraPtr mCamera;
};

//=========================================================================
MeshRenderComponent::MeshRenderComponent(MeshPtr mesh, CameraPtr camera)
{
	mMesh = std::move(mesh);
	mCamera = std::move(camera);
}

//=========================================================================
const Mesh& MeshRenderComponent::getMesh() const
{
	return *mMesh;
}

//=========================================================================
const Camera& MeshRenderComponent::getCamera() const
{
	return *mCamera;
}

//=========================================================================
bool MeshRenderComponent::isCulled(const Camera& camera)
{
	//return mRenderable->isCulled(camera);
	return false;
}
