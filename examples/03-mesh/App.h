#pragma once

#include "../common/BaseApp.h"
#include "graphics/Light.h"
#include "graphics/Texture.h"
#include "graphics/Shader.h"
#include "graphics/Geometry.h"
#include "graphics/Mesh.h"
#include <array>

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
    Texture normalMap;
    Texture specularMap;
    Texture aoMap;
    Texture emissiveMap;
    Shader shader;
    Shader lightMarkerShader;
    Geometry lightMarker;
    Mesh mesh;
    mat4 model{mat4(1.0)};
    std::array<Light, 3> lights;
    vec3 lightOrbitCenter{0.0f, 4.0f, 0.0f};
    float lightOrbitTime{0.0f};
};
