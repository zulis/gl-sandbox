#include "App.h"

App::App() : BaseApp("Window")
{
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
}

void App::onResize(int width, int height)
{

}
