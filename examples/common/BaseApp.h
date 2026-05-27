#pragma once

#include <memory>
#include <imgui.h>
#include "Camera.h"
#include "window/Window.h"
#include "graphics/GL.h"

class BaseApp
{
public:
    BaseApp(const char *title);
    virtual ~BaseApp();

    void run();
    void quit();

    virtual void update(float deltaTime) = 0;
    virtual void draw() = 0;
    virtual void onResize(int width, int height){};
    virtual void drawUI() = 0;

    library::Window *window;
    Camera camera;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};
