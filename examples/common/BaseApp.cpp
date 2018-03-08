#include "BaseApp.h"
#include <system/Subsystem.h>
#include <window/Window.h>
#include <simulation/Simulation.h>

using namespace library;

class BaseApp::Impl
{
public:
    bool running{false};
};

BaseApp::BaseApp(const char *title)
    : impl{std::make_unique<Impl>()}
{
    auto &window = subsystem::add<Window>();
    subsystem::add<Simulation>();

    window.setTitle(title);
}

BaseApp::~BaseApp()
{
    subsystem::remove<Simulation>();
    subsystem::remove<Window>();
}

void BaseApp::run()
{
    impl->running = true;
    auto &window = subsystem::get<Window>();
    auto &simulation = subsystem::get<Simulation>();

    simulation.setMaxFps(60);
    onResize(window.getWidth(), window.getWidth());

    while (impl->running && !window.isKeyDown(Key::Escape)) {
        simulation.runOneFrame();
        auto dt = simulation.getDeltaTime();

        window.handleEvents();

        window.closeEvent = [this]
        { quit(); };

        window.resizeEvent = [this](int width, int height)
        {
            onResize(width, height);
        };

        update(dt.count());
        draw();

        window.swapBuffers();
    }
}

void BaseApp::quit()
{
    impl->running = false;
}