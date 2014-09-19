#pragma once

#include <memory>
#include <Artemis.h>
#include "Core/Camera.h"
#include "Spatial/Spatial.h"

class RenderComponent : public Component
{
public:
	RenderComponent(std::shared_ptr<Spatial> renderable, CameraRef camera);
	std::shared_ptr<Spatial> getRenderable() const;
	CameraRef getCamera() const;
	bool isCulled(const Camera& camera);

private:
	std::shared_ptr<Spatial> mRenderable;
	CameraRef mCamera;
};

//=========================================================================
RenderComponent::RenderComponent(std::shared_ptr<Spatial> renderable, CameraRef camera)
{
	mRenderable = renderable;
	mCamera = camera;
}

//=========================================================================
std::shared_ptr<Spatial> RenderComponent::getRenderable() const
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
	return mRenderable->isCulled(camera);
}
