#include "App.h"

App::App()
    : BaseApp("ImGui")
{
}

void App::update(float deltaTime)
{
}

void App::draw()
{
    glClearColor(0.39f, 0.58f, 0.93f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void App::drawUI()
{
    static bool show_demo_window = true;
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
    ImGui::ShowDemoWindow(&show_demo_window);

    if (ImGui::Begin("FPS Monitor")) {
        float fps = ImGui::GetIO().Framerate;
        
        // Display FPS
        ImGui::Text("FPS: %.1f", fps);
        
        // Optional: Display frame time in milliseconds
        ImGui::Text("Frame Time: %.3f ms", 1000.0f / (fps > 0 ? fps : 1));
    }
    ImGui::End();
}

void App::onResize(int width, int height)
{
}
