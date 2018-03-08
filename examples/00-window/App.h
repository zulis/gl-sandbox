#ifndef PROJECT_APP_H
#define PROJECT_APP_H

#include "../common/BaseApp.h"

class App : public BaseApp
{
public:
    App();
    ~App() = default;
    void update(double deltaTime) override;
    void draw() override;
    void onResize(int width, int height) override;
};

#endif //PROJECT_APP_H
