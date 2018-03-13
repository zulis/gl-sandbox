#include "BaseApp.h"
#include "system/Subsystem.h"
#include "window/Window.h"
#include "simulation/Simulation.h"

using namespace library;

class BaseApp::Impl
{
public:
    bool running{false};
    bool isFullScreen{false};
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

    while (impl->running && !window.isKeyDown(Key::Escape)) {
        simulation.runOneFrame();
        auto dt = simulation.getDeltaTime();

        window.handleEvents();

        // On window close
        window.closeEvent = [this]
        { quit(); };

        // On widow resize
        window.resizeEvent = [this](int width, int height)
        {
            onResize(width, height);
        };

        // Toggle full screen
        if (window.isKeyPressed(Key::F11)) {
            impl->isFullScreen ? window.setWindowMode(WindowMode::Windowed) : window.setWindowMode(WindowMode::FullScreenNative);
            impl->isFullScreen = !impl->isFullScreen;
        }

        update(dt.count());
        draw();

        window.swapBuffers();
    }
}

void BaseApp::quit()
{
    impl->running = false;
}