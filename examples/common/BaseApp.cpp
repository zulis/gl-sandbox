#include "BaseApp.h"
#include "system/Subsystem.h"
#include "simulation/Simulation.h"
#include "ui/UI.h"

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
    window = &subsystem::add<Window>();
    subsystem::add<Simulation>();
    subsystem::add<UI>();

    window->setTitle(title);
}

BaseApp::~BaseApp()
{
    subsystem::remove<UI>();
    subsystem::remove<Simulation>();
    subsystem::remove<Window>();
}

void BaseApp::run()
{
    impl->running = true;
    auto &simulation = subsystem::get<Simulation>();
    auto &ui = subsystem::get<UI>();

    simulation.setMaxFps(60);

    onResize(window->getWidth(), window->getHeight());

    while (impl->running && !window->isKeyDown(Key::Escape))
    {
        simulation.runOneFrame();
        auto deltaTime = simulation.getDeltaTime();

        window->handleEvents();

        // On window close
        window->closeEvent = [this]
        { quit(); };

        // On widow resize
        window->resizeEvent = [this](int width, int height)
        {
            onResize(width, height);
        };

        // Toggle full screen
        if (window->isKeyPressed(Key::F11))
        {
            impl->isFullScreen ? window->setWindowMode(WindowMode::Windowed) : window->setWindowMode(WindowMode::FullScreenNative);
            impl->isFullScreen = !impl->isFullScreen;
        }

        // Register UI events
        UI::Events events;
        events.windowSize = window->getSize();
        events.deltaTime = deltaTime.count();
        events.isKeyShiftDown = window->isKeyShiftDown();
        events.isKeyCtrlDown = window->isKeyCtrlDown();
        events.isKeyAltDown = window->isKeyAltDown();
        events.isKeySuperDown = window->isKeySuperDown();
        events.mousePosition = window->getMousePosition();
        events.mouseWheel = window->getMouseWheelChange();
        events.isMouseButtonLeftDown = window->isMouseButtonDown(Button::Left);
        events.isMouseButtonRightDown = window->isMouseButtonDown(Button::Right);
        events.isMouseButtonMiddleDown = window->isMouseButtonDown(Button::Middle);
        events.keysDown = window->getKeysDown();
        events.textInput = window->getTextInput();
        ui.setEvents(events);

        update(deltaTime.count());
        draw();
        ui.frameStart();
        drawUI();
        ui.frameEnd();

        window->swapBuffers();
    }
}

void BaseApp::quit()
{
    impl->running = false;
}
