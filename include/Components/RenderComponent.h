#pragma once

#include <memory>
#include <Artemis.h>
#include "Core/Mesh.h"
#include "Core/Camera.h"

class RenderComponent : public Component
{
public:
	RenderComponent(MeshRef renderable, CameraRef camera);
	MeshRef getRenderable() const;
	CameraRef getCamera() const;
	bool isCulled(const Camera& camera);

private:
	MeshRef mRenderable;
	CameraRef mCamera;
};

//=========================================================================
RenderComponent::RenderComponent(MeshRef renderable, CameraRef camera)
{
	mRenderable = renderable;
	mCamera = camera;
}

//=========================================================================
MeshRef RenderComponent::getRenderable() const
{
	return mRenderable;
}

//=========================================================================
CameraRef RenderComponent::getCamera() const
{
	return mCamera;
}

//=========================================================================
bool RenderComponent::isCulled(const Camera& camera)
{
	//return mRenderable->isCulled(camera);
	return false;
}
