#include "window.h"

#include <SDL_config.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_video.h>

#define MAX_KEYBOARD_KEYS 512
#define MAX_TEXT_SIZE 32
#define MAX_MOUSE_BUTTONS 3

namespace library
{
class Window::Impl
{
public:
    SDL_Window *window;
    int width{1280};
    int height{720};

    SDL_SysWMinfo get_window_info()
    {
        SDL_SysWMinfo window_info;
        SDL_VERSION(&window_info.version);
        SDL_GetWindowWMInfo(window, &window_info);
        return window_info;
    }

    struct KeyboardState
    {
        bool keys_down[MAX_KEYBOARD_KEYS];
        bool keys_up[MAX_KEYBOARD_KEYS];
        char text_input[MAX_TEXT_SIZE];
        bool key_alt;
        bool key_ctrl;
        bool key_shift;
        bool key_super;
    };

    struct MouseState
    {
        bool buttons_down[MAX_MOUSE_BUTTONS];
        ivec2 position;
        ivec2 change;
        int wheel;
    };

    KeyboardState keyboard_state;
    MouseState mouse_state;
};

Window::Window()
    : impl{std::make_unique<Impl>()}
{
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);

    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI; // | SDL_WINDOW_MAXIMIZED;
    //if (!m_window_mode) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    impl->window = SDL_CreateWindow(
        nullptr,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        impl->width,
        impl->height,
        flags
    );
}

Window::~Window()
{
    SDL_DestroyWindow(impl->window);
    SDL_Quit();
}

void Window::set_title(const char *title)
{
    SDL_SetWindowTitle(impl->window, title);
}

void Window::set_size(int width, int height)
{
    impl->width = width;
    impl->height = height;
    SDL_SetWindowSize(impl->window, impl->width, impl->height);
}

void Window::center_screen()
{
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    SDL_SetWindowPosition(impl->window, dm.w / 2 - impl->width / 2, dm.h / 2 - impl->height / 2);
}

void Window::handle_events()
{
    memset(impl->keyboard_state.text_input, 0, MAX_TEXT_SIZE);
    impl->mouse_state.change = ivec2();
    impl->mouse_state.wheel = 0;
    for (int i = 0; i < MAX_KEYBOARD_KEYS; i++) {
        impl->keyboard_state.keys_up[i] = false;
    }

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: {
                if (close_event)
                    close_event();
            }
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                int scan_code = event.key.keysym.scancode;
                impl->keyboard_state.keys_down[scan_code] = (event.type == SDL_KEYDOWN);
                impl->keyboard_state.keys_up[scan_code] = (event.type == SDL_KEYUP);
                impl->keyboard_state.key_shift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
                impl->keyboard_state.key_ctrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
                impl->keyboard_state.key_alt = ((SDL_GetModState() & KMOD_ALT) != 0);
                impl->keyboard_state.key_super = ((SDL_GetModState() & KMOD_GUI) != 0);
            }
                break;
            case SDL_TEXTINPUT: {
                memcpy(impl->keyboard_state.text_input, event.text.text, sizeof(event.text.text));
            }
                break;
            case SDL_MOUSEMOTION: {
                impl->mouse_state.position = ivec2(event.motion.x, event.motion.y);
                impl->mouse_state.change = ivec2(event.motion.xrel, event.motion.yrel);
            }
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                switch (event.button.button) {
                    case SDL_BUTTON_LEFT:
                        impl->mouse_state.buttons_down[(int) Button::Left] = event.button.state;
                        break;
                    case SDL_BUTTON_RIGHT:
                        impl->mouse_state.buttons_down[(int) Button::Right] = event.button.state;
                        break;
                    case SDL_BUTTON_MIDDLE:
                        impl->mouse_state.buttons_down[(int) Button::Middle] = event.button.state;
                        break;
                }
            }
                break;
            case SDL_MOUSEWHEEL: {
                impl->mouse_state.wheel = event.wheel.x != 0 ? event.wheel.x : event.wheel.y;
            }
                break;
            case SDL_WINDOWEVENT: {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED: {
                        int w, h;
                        SDL_GetWindowSize(impl->window, &w, &h);
                        impl->width = w;
                        impl->height = h;
                        if (size_change_event)
                            size_change_event(w, h);
                    }
                        break;
                    case SDL_WINDOWEVENT_MOVED: {
                        int x, y;
                        SDL_GetWindowPosition(impl->window, &x, &y);
                        if (position_change_event)
                            position_change_event(x, y);
                    }
                        break;
                    case SDL_WINDOWEVENT_CLOSE: {
                        if (close_event)
                            close_event();
                    }
                        break;
                }
            }
                break;
        }
    }
}
bool Window::is_key_down(Key key)
{
    return impl->keyboard_state.keys_down[(int) key];
}

bool Window::is_key_pressed(Key key)
{
    return impl->keyboard_state.keys_up[(int) key];
}

bool Window::is_key_shift_down()
{
    return impl->keyboard_state.key_shift;
}

bool Window::is_key_ctrl_down()
{
    return impl->keyboard_state.key_ctrl;
}

bool Window::is_key_alt_down()
{
    return impl->keyboard_state.key_alt;
}

bool Window::is_key_super_down()
{
    return impl->keyboard_state.key_super;
}

const bool *Window::get_keys_down()
{
    return impl->keyboard_state.keys_down;
}

const char *Window::get_text_input()
{
    return impl->keyboard_state.text_input;
}

ivec2 Window::get_mouse_position()
{
    return impl->mouse_state.position;
}

ivec2 Window::get_mouse_change()
{
    return impl->mouse_state.change;
}

int Window::get_mouse_wheel_change()
{
    return impl->mouse_state.wheel;
}

bool Window::is_mouse_button_down(Button button)
{
    return impl->mouse_state.buttons_down[(int) button];
}

void *Window::get_window_handle()
{
    SDL_SysWMinfo window_info = impl->get_window_info();

#ifdef _WIN32
    return window_info.info.win.window;
#elif defined(__linux__)
    return (void *) (uintptr_t) window_info.info.x11.window;
#elif defined(__APPLE__)
    return window_info.info.cocoa.window;
#else
    return nullptr;
#endif
}

void *Window::get_display()
{
    SDL_SysWMinfo window_info = impl->get_window_info();

#if defined(__linux__)
    return window_info.info.x11.display;
#else
    return nullptr;
#endif
}

int Window::get_width()
{
    return impl->width;
}

int Window::get_height()
{
    return impl->height;
}

ivec2 Window::get_size()
{
    return ivec2(impl->width, impl->height);
}

}