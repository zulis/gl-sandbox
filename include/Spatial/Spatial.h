#pragma once

#include "Core/Math.h"
#include "Core/Camera.h"

class Spatial
{
	public:
		virtual ~Spatial() {};
		virtual void setMatrix(glm::mat4 matrix) = 0;
		virtual void update(float deltaTime) = 0;
		virtual void draw(const CameraRef& camera) = 0;
		virtual bool isCulled(const Camera& camera) = 0;

};