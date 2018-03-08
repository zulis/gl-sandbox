#include "Window.h"

#include <glad/glad.h>

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
    SDL_GLContext glContext;
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
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // whether the output is single or double buffered; defaults to double buffering on
    //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // the minimum number of bits in the depth buffer; defaults to 16


    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else {
        Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI; // | SDL_WINDOW_MAXIMIZED;
        //if (!m_window_mode) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

        impl->window = SDL_CreateWindow(
            nullptr,
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            impl->width,
            impl->height,
            flags
        );

        if (impl->window == nullptr) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
        else {
            impl->glContext = SDL_GL_CreateContext(impl->window);

            if (impl->glContext == nullptr) {
                printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
            }
            else {
                if (!gladLoadGL()) {
                    printf("Error occurred on loading glad.\n");
                }
                else {
                    printf("Vendor:          %s\n", glGetString(GL_VENDOR));
                    printf("Renderer:        %s\n", glGetString(GL_RENDERER));
                    printf("Version OpenGL:  %s\n", glGetString(GL_VERSION));
                    printf("Version GLSL:    %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
                }
            }
        }
    }
}

Window::~Window()
{
    SDL_DestroyWindow(impl->window);
    impl->window = nullptr;
    SDL_Quit();
}

void Window::setTitle(const char *title)
{
    SDL_SetWindowTitle(impl->window, title);
}

void Window::setSize(int width, int height)
{
    impl->width = width;
    impl->height = height;
    SDL_SetWindowSize(impl->window, impl->width, impl->height);
}

void Window::centerScreen()
{
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    SDL_SetWindowPosition(impl->window, dm.w / 2 - impl->width / 2, dm.h / 2 - impl->height / 2);
}

void Window::swapBuffers()
{
    SDL_GL_SwapWindow(impl->window);
}

void Window::handleEvents()
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
                if (closeEvent)
                    closeEvent();
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
                        if (resizeEvent)
                            resizeEvent(w, h);
                    }
                        break;
                    case SDL_WINDOWEVENT_MOVED: {
                        int x, y;
                        SDL_GetWindowPosition(impl->window, &x, &y);
                        if (positionChangeEvent)
                            positionChangeEvent(x, y);
                    }
                        break;
                    case SDL_WINDOWEVENT_CLOSE: {
                        if (closeEvent)
                            closeEvent();
                    }
                        break;
                }
            }
                break;
        }
    }
}
bool Window::isKeyDown(Key key)
{
    return impl->keyboard_state.keys_down[(int) key];
}

bool Window::isKeyPressed(Key key)
{
    return impl->keyboard_state.keys_up[(int) key];
}

bool Window::isKeyShiftDown()
{
    return impl->keyboard_state.key_shift;
}

bool Window::isKeyCtrlDown()
{
    return impl->keyboard_state.key_ctrl;
}

bool Window::isKeyAltDown()
{
    return impl->keyboard_state.key_alt;
}

bool Window::isKeySuperDown()
{
    return impl->keyboard_state.key_super;
}

const bool *Window::getKeysDown()
{
    return impl->keyboard_state.keys_down;
}

const char *Window::getTextInput()
{
    return impl->keyboard_state.text_input;
}

ivec2 Window::getMousePosition()
{
    return impl->mouse_state.position;
}

ivec2 Window::getMouseChange()
{
    return impl->mouse_state.change;
}

int Window::getMouseWheelChange()
{
    return impl->mouse_state.wheel;
}

bool Window::isMouseButtonDown(Button button)
{
    return impl->mouse_state.buttons_down[(int) button];
}

void *Window::getWindowHandle()
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

void *Window::getDisplay()
{
    SDL_SysWMinfo window_info = impl->get_window_info();

#if defined(__linux__)
    return window_info.info.x11.display;
#else
    return nullptr;
#endif
}

int Window::getWidth()
{
    return impl->width;
}

int Window::getHeight()
{
    return impl->height;
}

ivec2 Window::getSize()
{
    return ivec2(impl->width, impl->height);
}

}