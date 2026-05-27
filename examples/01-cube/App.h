#pragma once

#include "../common/BaseApp.h"
#include "graphics/Texture.h"
#include "graphics/Shader.h"
#include "graphics/Geometry.h"
#include <imgui.h>

using namespace library;

class App : public BaseApp
{
public:
    App();
    ~App() = default;
    void update(float deltaTime) override;
    void draw() override;
    void drawUI() override;
    void onResize(int width, int height) override;

private:
    Texture texture;
    Shader shader;
    Geometry geometry;
    mat4 projection;
    mat4 view;
    mat4 model;
};
