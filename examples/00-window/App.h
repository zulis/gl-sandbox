#ifndef PROJECT_APP_H
#define PROJECT_APP_H

#include "../common/BaseApp.h"
#include "graphics/Texture.h"

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
    Texture tex1, tex2;
};

#endif //PROJECT_APP_H
