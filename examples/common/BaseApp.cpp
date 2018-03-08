#include "BaseApp.h"
#include <system/subsystem.h>
#include <window/window.h>

using namespace library;

BaseApp::BaseApp()
{
    core::add_subsystem<Window>();
}

void BaseApp::run()
{
    m_running = true;
    auto& window = core::get_subsystem<Window>();


    while (m_running) {

        window.handle_events();

        if(window.is_key_down(Key::Escape))
            quit();
    }
}

void BaseApp::quit()
{
    m_running = false;
}