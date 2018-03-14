#ifndef PROJECT_APP_H
#define PROJECT_APP_H

#include "../common/BaseApp.h"
#include "graphics/Texture.h"
#include "graphics/Shader.h"
#include "graphics/Geometry.h"

using namespace library;

class App : public BaseApp
{
public:
    App();
    ~App() = default;
    void update(double deltaTime) override;
    void draw() override;
    void onResize(int width, int height) override;

private:
    Texture texture;
    Shader shader;
    Geometry geometry;
    mat4 mvp{mat4(1.0f)};
};

#endif //PROJECT_APP_H
