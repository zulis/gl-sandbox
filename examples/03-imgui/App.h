#pragma once

#include "../common/BaseApp.h"
#include "graphics/Texture.h"
#include "graphics/Shader.h"
#include "graphics/Mesh.h"

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
    Texture colorMap;
    Shader shader;
    Mesh mesh;
    mat4 model{mat4(1.0)};
};
