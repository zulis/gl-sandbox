#include "App.h"
#include <imgui.h>
#include "filesystem/FileSystem.h"

using namespace library;
using fs = FileSystem;

App::App() : BaseApp("Window")
{
    const char *data = fs::readFile("assets/textures/uv.jpg");
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

    //ImGui::SetNextWindowSize(ImVec2(400, 300));
    //ImGui::Begin("Hello!");
    //ImGui::End();
}

void App::onResize(int width, int height)
{

}
