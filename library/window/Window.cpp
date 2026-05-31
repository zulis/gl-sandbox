#include "Window.h"
#include "system/Log.h"
#include "graphics/GL.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_video.h>

#include <cstring>

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

    struct KeyboardState
    {
        bool keysDown[MAX_KEYBOARD_KEYS]{};
        bool keysUp[MAX_KEYBOARD_KEYS]{};
        char textInput[MAX_TEXT_SIZE]{};
        bool keyAlt{};
        bool keyCtrl{};
        bool keyShift{};
        bool keySuper{};
    };

    struct MouseState
    {
        bool buttonsDown[MAX_MOUSE_BUTTONS]{};
        ivec2 position;
        ivec2 change;
        int wheel{};
    };

    KeyboardState keyboardState;
    MouseState mouseState;
};

Window::Window()
    : impl{std::make_unique<Impl>()}
{
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // whether the output is single or double buffered; defaults to double buffering on
    //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // the minimum number of bits in the depth buffer; defaults to 16

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        error("SDL could not initialize! SDL_Error: {}", SDL_GetError());
    }
    else {
        SDL_WindowFlags flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
        //if (!m_window_mode) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

        impl->window = SDL_CreateWindow(nullptr, impl->width, impl->height, flags);

        if (impl->window == nullptr) {
            error("Window could not be created! SDL_Error: {}", SDL_GetError());
        }
        else {
            SDL_SetWindowPosition(impl->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
            impl->glContext = SDL_GL_CreateContext(impl->window);

            SDL_GL_SetSwapInterval(0); // Disable vsync; frame pacing is handled by Simulation

            if (impl->glContext == nullptr) {
                error("OpenGL context could not be created! SDL Error: {}", SDL_GetError());
            }
            else {
                if (!gladLoadGL()) {
                    error("Error occurred on loading glad.");
                }
                else {
                    note("Vendor:         {}", reinterpret_cast<const char *>(glGetString(GL_VENDOR)));
                    note("Renderer:       {}", reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
                    note("Version OpenGL: {}", reinterpret_cast<const char *>(glGetString(GL_VERSION)));
                    note("Version GLSL:   {}", reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
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
    SDL_SetWindowPosition(impl->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void Window::swapBuffers()
{
    SDL_GL_SwapWindow(impl->window);
}

SDL_Window *Window::getSDLWindow()
{
    return impl->window;
}

void *Window::getGLContext()
{
    return impl->glContext;
}

void Window::handleEvents()
{
    memset(impl->keyboardState.textInput, 0, MAX_TEXT_SIZE);
    impl->mouseState.change = ivec2();
    impl->mouseState.wheel = 0;
    for (int i = 0; i < MAX_KEYBOARD_KEYS; i++) {
        impl->keyboardState.keysUp[i] = false;
    }

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (eventCallback) {
            eventCallback(&event);
        }

        switch (event.type) {
            case SDL_EVENT_QUIT: {
                if (closeEvent)
                    closeEvent();
            }
                break;
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP: {
                int scan_code = event.key.scancode;
                impl->keyboardState.keysDown[scan_code] = event.key.down;
                impl->keyboardState.keysUp[scan_code] = !event.key.down;
                const SDL_Keymod keyMods = SDL_GetModState();
                impl->keyboardState.keyShift = ((keyMods & SDL_KMOD_SHIFT) != 0);
                impl->keyboardState.keyCtrl = ((keyMods & SDL_KMOD_CTRL) != 0);
                impl->keyboardState.keyAlt = ((keyMods & SDL_KMOD_ALT) != 0);
                impl->keyboardState.keySuper = ((keyMods & SDL_KMOD_GUI) != 0);
            }
                break;
            case SDL_EVENT_TEXT_INPUT: {
                std::strncpy(impl->keyboardState.textInput, event.text.text, MAX_TEXT_SIZE - 1);
                impl->keyboardState.textInput[MAX_TEXT_SIZE - 1] = '\0';
            }
                break;
            case SDL_EVENT_MOUSE_MOTION: {
                impl->mouseState.position = ivec2((int) event.motion.x, (int) event.motion.y);
                impl->mouseState.change = ivec2((int) event.motion.xrel, (int) event.motion.yrel);
            }
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                switch (event.button.button) {
                    case SDL_BUTTON_LEFT:
                        impl->mouseState.buttonsDown[(int) Button::Left] = event.button.down;
                        break;
                    case SDL_BUTTON_RIGHT:
                        impl->mouseState.buttonsDown[(int) Button::Right] = event.button.down;
                        break;
                    case SDL_BUTTON_MIDDLE:
                        impl->mouseState.buttonsDown[(int) Button::Middle] = event.button.down;
                        break;
                }
            }
                break;
            case SDL_EVENT_MOUSE_WHEEL: {
                impl->mouseState.wheel = event.wheel.x != 0 ? event.wheel.x : event.wheel.y;
            }
                break;
            case SDL_EVENT_WINDOW_RESIZED: {
                impl->width = event.window.data1;
                impl->height = event.window.data2;
                if (resizeEvent)
                    resizeEvent(impl->width, impl->height);
            }
                break;
            case SDL_EVENT_WINDOW_MOVED: {
                if (positionEvent)
                    positionEvent(event.window.data1, event.window.data2);
            }
                break;
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
                if (closeEvent)
                    closeEvent();
            }
                break;
        }
    }
}
bool Window::isKeyDown(Key key)
{
    return impl->keyboardState.keysDown[(int) key];
}

bool Window::isKeyPressed(Key key)
{
    return impl->keyboardState.keysUp[(int) key];
}

bool Window::isKeyShiftDown()
{
    return impl->keyboardState.keyShift;
}

bool Window::isKeyCtrlDown()
{
    return impl->keyboardState.keyCtrl;
}

bool Window::isKeyAltDown()
{
    return impl->keyboardState.keyAlt;
}

bool Window::isKeySuperDown()
{
    return impl->keyboardState.keySuper;
}

const bool *Window::getKeysDown()
{
    return impl->keyboardState.keysDown;
}

const char *Window::getTextInput()
{
    return impl->keyboardState.textInput;
}

ivec2 Window::getMousePosition()
{
    return impl->mouseState.position;
}

ivec2 Window::getMouseChange()
{
    return impl->mouseState.change;
}

int Window::getMouseWheelChange()
{
    return impl->mouseState.wheel;
}

bool Window::isMouseButtonDown(Button button)
{
    return impl->mouseState.buttonsDown[(int) button];
}

void *Window::getWindowHandle()
{
    SDL_PropertiesID props = SDL_GetWindowProperties(impl->window);

#ifdef _WIN32
    return SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
#elif defined(__linux__)
    return (void *) (uintptr_t) SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
#elif defined(__APPLE__)
    return SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
#else
    return nullptr;
#endif
}

void *Window::getDisplay()
{
    SDL_PropertiesID props = SDL_GetWindowProperties(impl->window);

#if defined(__linux__)
    return SDL_GetPointerProperty(props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
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

void Window::setWindowMode(const WindowMode &mode)
{
    bool isFullScreen = SDL_GetWindowFlags(impl->window) & SDL_WINDOW_FULLSCREEN;

    switch (mode) {
        case WindowMode::Windowed:
            SDL_SetWindowFullscreen(impl->window, false);
            break;
        case WindowMode::FullScreen:
            SDL_SetWindowFullscreenMode(impl->window, SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(impl->window)));
            SDL_SetWindowFullscreen(impl->window, true);
            break;
        case WindowMode::FullScreenNative: {
            SDL_SetWindowFullscreenMode(impl->window, nullptr);
            SDL_SetWindowFullscreen(impl->window, true);
        }
            break;
    }
}

void Window::showMouse(bool show)
{
    if (show) {
        SDL_ShowCursor();
        SDL_SetWindowRelativeMouseMode(impl->window, false);
    }
    else {
        SDL_HideCursor();
        SDL_SetWindowRelativeMouseMode(impl->window, true);
    }
}

}