#ifndef PROJECT_BASE_APP_H
#define PROJECT_BASE_APP_H

#include <memory>
#include <glad/glad.h>

class BaseApp
{
public:
    BaseApp(const char *title);
    virtual ~BaseApp();

    void run();
    void quit();

    virtual void update(float deltaTime) = 0;
    virtual void draw() = 0;
    virtual void onResize(int width, int height) {};

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};


#endif //PROJECT_BASE_APP_H
