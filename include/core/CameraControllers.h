#pragma once

#include "core/Camera.h"
#include "core/Input.h"

namespace CameraController
{
float strafeSpeed{ 0.2f };
float strafeFastSpeed{ 0.4f };

//=========================================================================
void flyController(CameraPtr& camera, Input& input)
{
	if(input.isKeyDown(KEY_LEFT_SHIFT))
		camera->setStrafeSpeed(strafeFastSpeed);
	else
		camera->setStrafeSpeed(strafeSpeed);

	if(input.isKeyDown(KEY_W))
		camera->move(Camera::FORWARD);
	else if(input.isKeyDown(KEY_S))
		camera->move(Camera::BACKWARD);

	if(input.isKeyDown(KEY_A))
		camera->move(Camera::LEFT);
	else if(input.isKeyDown(KEY_D))
		camera->move(Camera::RIGHT);

	if(input.isKeyDown(KEY_E))
		camera->move(Camera::UP);
	else if(input.isKeyDown(KEY_Q))
		camera->move(Camera::DOWN);

	if(input.getMouseScroolY() != 0)
		camera->move(input.getMouseScroolY() > 0 ? Camera::FORWARD : Camera::BACKWARD);

	if(input.isMouseDown(MouseButton::Right))
	{
		input.hideMouse();
		camera->rotate((float)input.getMouseChangeX(), (float)input.getMouseChangeY());
	}
	else
	{
		input.showMouse();
	}
}
}