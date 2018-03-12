#include "App.h"
#include <imgui.h>
#include <iostream>
#include "system/Subsystem.h"
#include "window/Window.h"
#include "filesystem/FileSystem.h"

using fs = FileSystem;

App::App()
    : BaseApp("Window")
{
    tex1.fromFile("assets/textures/uv.jpg");
    tex2.fromFile("assets/textures/uv.jpg");

    sh1.fromFile("assets/shaders/mesh.shd");
}

void App::update(double deltaTime)
{
	auto &window = subsystem::get<Window>();

	if (window.isKeyPressed(Key::R))
	{
		sh1.reload();
	}
}

void App::draw()
{
    // Set background color as cornflower blue
    glClearColor(0.39f, 0.58f, 0.93f, 1.f);
    // Clear color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sh1.bind();
    tex1.bind(0);
    tex2.bind(1);

    //ImGui::SetNextWindowSize(ImVec2(400, 300));
    //ImGui::Begin("Hello!");
    //ImGui::End();
}

void App::onResize(int width, int height)
{

}
