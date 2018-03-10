#include "App.h"
#include <imgui.h>
#include <iostream>
#include "filesystem/FileSystem.h"

using fs = FileSystem;

App::App() : BaseApp("Window")
{
    const char *data = fs::readFile("assets/textures/uv.jpg");

    tex1.fromFile("assets/textures/uv.jpg");
    tex2.fromFile("assets/textures/uv.jpg");

    std::cout << std::to_string(tex1.getWidth()) << std::endl;
    std::cout << std::to_string(tex1.getHeight()) << std::endl;

    std::cout << std::to_string(tex2.getWidth()) << std::endl;
    std::cout << std::to_string(tex2.getHeight()) << std::endl;
}

void App::update(double deltaTime)
{
}

void App::draw()
{
    // Set background color as cornflower blue
    glClearColor(0.39f, 0.58f, 0.93f, 1.f);
    // Clear color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    tex1.bind(0);
    tex2.bind(1);

    //ImGui::SetNextWindowSize(ImVec2(400, 300));
    //ImGui::Begin("Hello!");
    //ImGui::End();
}

void App::onResize(int width, int height)
{

}
