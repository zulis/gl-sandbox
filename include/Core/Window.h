#pragma once

#include <memory>
#include <functional>
#include <string>
#include <map>
#include <SDL.h>
#include <SDL_opengl.h>

typedef std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> WindowRef;
typedef std::function<void(int, int)> ResizeCallback;
typedef std::function<void(int, bool)> KeyDownCallback;
typedef std::function<void(int, bool)> MouseDownCallback;
typedef std::function<void(int, int, int, int)> MousePositionCallback;
typedef std::function<void(int)> MouseWheelCallback;

class Window
{
public:
	enum MessageBoxType
	{
		Error,
		Warning,
		Info
	};

	static void create(int width, int height, bool fullScreen, const std::string& title);
	static void close();
	static bool isClosed();
	static void update();
	static void swapBuffers();
	static void setFullScreen(bool fullScreen);
	static void setFullScreenDesktop(bool fullScreen);
	static bool isFullScreen();
	static unsigned int getWidth();
	static unsigned int getHeight();
	static void setResizeCallback(ResizeCallback callback);
	static void setKeyDownCallback(KeyDownCallback callback);
	static void setMouseDownCallback(MouseDownCallback callback);
	static void setMousePositionCallback(MousePositionCallback callback);
	static void setMouseWheelCallback(MouseWheelCallback callback);
	static void setTitle(const std::string& title);
	static void setSize(int width, int height);
	static void showMessageBox(const std::string& message, Window::MessageBoxType type = Window::MessageBoxType::Error);
	static void showMouse();
	static void hideMouse();
	static void setMousePosition(int x, int y);
	static void setCursor(const std::string& fileName, unsigned int hotspotX = 0, unsigned int hotspotY = 0);
	static void setCursor(const unsigned char* pixels, unsigned int width, unsigned int height, unsigned int channels, unsigned int hotspotX = 0, unsigned int hotspotY = 0);
	static void setIcon(const unsigned char* pixels, unsigned int width, unsigned int height, unsigned int channels);
	static float getAspectRatio();
	static void sleep(int milliseconds);

private:
	static WindowRef mWindow;
	static SDL_GLContext mContext;
	static bool mIsClosed;
	static bool mIsFullScreen;
	static unsigned int mWidth;
	static unsigned int mHeight;
	static int eventFilter(void* userdata, SDL_Event* event);
	static ResizeCallback mResizeCallback;
	static KeyDownCallback mKeyDownCallback;
	static MouseDownCallback mMouseDownCallback;
	static MousePositionCallback mMousePositionCallback;
	static MouseWheelCallback mMouseWheelCallback;
	static void resizeEvent(int width, int height);
	static void keyDownEvent(int key, bool isDown);
	static void mouseDownEvent(int button, bool isDown);
	static void mousePositionEvent(int x, int y, int changeX, int changeY);
	static void mouseWheelEvent(int x);

};

WindowRef Window::mWindow = WindowRef(nullptr, SDL_DestroyWindow);
SDL_GLContext Window::mContext;
bool Window::mIsClosed = false;
bool Window::mIsFullScreen = false;
unsigned int Window::mWidth = 0;
unsigned int Window::mHeight = 0;
ResizeCallback Window::mResizeCallback = nullptr;
KeyDownCallback Window::mKeyDownCallback = nullptr;
MouseDownCallback Window::mMouseDownCallback = nullptr;
MousePositionCallback Window::mMousePositionCallback = nullptr;
MouseWheelCallback Window::mMouseWheelCallback = nullptr;

//=========================================================================
void Window::create(int width, int height, bool fullScreen, const std::string& title)
{
	mWidth = width;
	mHeight = height;
	mIsFullScreen = fullScreen;

	if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		showMessageBox("SDL initialization failed.");
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Setup anti aliasing
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN;

	// Create the window
	mWindow.reset(SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags));

	// Make sure it created ok
	if(mWindow == nullptr)
	{
		showMessageBox("Failed to create window.");
		return;
	}

	// Create the context
	mContext = SDL_GL_CreateContext(mWindow.get());

	// Disable vsync (use 1 to enable)
	SDL_GL_SetSwapInterval(0);

	// Update window
	SDL_GL_SwapWindow(mWindow.get());
}

// =========================================================================
void Window::close()
{
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow.get());
	SDL_Quit();
	mIsClosed = true;
}

// =========================================================================
bool Window::isClosed()
{
	return mIsClosed;
}

//=========================================================================
void Window::update()
{
	SDL_Event event;

	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				keyDownEvent(event.key.keysym.sym, event.key.type == SDL_KEYDOWN);
				break;

			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				mouseDownEvent(event.button.button, event.button.type == SDL_MOUSEBUTTONDOWN);
				break;

			case SDL_MOUSEMOTION:
				mousePositionEvent(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
				break;

			case SDL_MOUSEWHEEL:
				mouseWheelEvent(event.wheel.y);
				break;

			case SDL_QUIT:
				close();
				break;

			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
					case SDL_WINDOWEVENT_RESIZED:
						mWidth = event.window.data1;
						mHeight = event.window.data2;
						resizeEvent(mWidth, mHeight);
						break;

					default:
						break;
				}

			default:
				break;
		}
	}
}

//=========================================================================
void Window::swapBuffers()
{
	SDL_GL_SwapWindow(mWindow.get());
}

//=========================================================================
void Window::setFullScreen(bool fullScreen)
{
	mIsFullScreen = fullScreen;
	SDL_SetWindowFullscreen(mWindow.get(), mIsFullScreen ? SDL_WINDOW_FULLSCREEN : 0);
}

//=========================================================================
void Window::setFullScreenDesktop(bool fullScreen)
{
	mIsFullScreen = fullScreen;
	SDL_SetWindowFullscreen(mWindow.get(), mIsFullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

//=========================================================================
bool Window::isFullScreen()
{
	return mIsFullScreen;
}

//=========================================================================
unsigned int Window::getWidth()
{
	return mWidth;
}

//=========================================================================
unsigned int Window::getHeight()
{
	return mHeight;
}

//=========================================================================
void Window::setResizeCallback(ResizeCallback callback)
{
	mResizeCallback = callback;
}

//=========================================================================
void Window::resizeEvent(int width, int height)
{
	mWidth = width;
	mHeight = height;

	if(mResizeCallback != nullptr)
		mResizeCallback(width, height);
}

//=========================================================================
void Window::setKeyDownCallback(KeyDownCallback callback)
{
	mKeyDownCallback = callback;
}

//=========================================================================
void Window::keyDownEvent(int key, bool isDown)
{
	if(mKeyDownCallback != nullptr)
		mKeyDownCallback(key, isDown);
}

//=========================================================================
void Window::setMouseDownCallback(MouseDownCallback callback)
{
	mMouseDownCallback = callback;
}

//=========================================================================
void Window::mouseDownEvent(int button, bool isDown)
{
	if(mMouseDownCallback != nullptr)
		mMouseDownCallback(button, isDown);
}

//=========================================================================
void Window::setMousePositionCallback(MousePositionCallback callback)
{
	mMousePositionCallback = callback;
}

//=========================================================================
void Window::mousePositionEvent(int x, int y, int changeX, int changeY)
{
	if(mMousePositionCallback != nullptr)
		mMousePositionCallback(x, y, changeX, changeY);
}

//=========================================================================
void Window::setMouseWheelCallback(MouseWheelCallback callback)
{
	mMouseWheelCallback = callback;
}

//=========================================================================
void Window::mouseWheelEvent(int x)
{
	if(mMouseWheelCallback != nullptr)
		mMouseWheelCallback(x);
}

//=========================================================================
void Window::setTitle(const std::string& title)
{
	SDL_SetWindowTitle(mWindow.get(), title.c_str());
}

//=========================================================================
void Window::setSize(int width, int height)
{
	SDL_SetWindowSize(mWindow.get(), width, height);
	SDL_SetWindowPosition(mWindow.get(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_GL_SwapWindow(mWindow.get());
	resizeEvent(width, height);
}

//=========================================================================
void Window::showMessageBox(const std::string& message, Window::MessageBoxType type)
{
	Uint32 flags;
	const char* title;

	switch(type)
	{
		case Window::Error:
			flags = SDL_MESSAGEBOX_ERROR;
			title = "Error";
			break;

		case Window::Warning:
			flags = SDL_MESSAGEBOX_WARNING;
			title = "Warning";
			break;

		case Window::Info:
			flags = SDL_MESSAGEBOX_INFORMATION;
			title = "Information";
			break;
	}

	SDL_ShowSimpleMessageBox(flags, title, message.c_str(), NULL);
}

//=========================================================================
void Window::showMouse()
{
	SDL_ShowCursor(1);
}

//=========================================================================
void Window::hideMouse()
{
	SDL_ShowCursor(0);
}

//=========================================================================
void Window::setMousePosition(int x, int y)
{
	SDL_WarpMouseInWindow(mWindow.get(), x, y);
}

//=========================================================================
void Window::setCursor(const std::string& fileName, unsigned int hotspotX, unsigned int hotspotY)
{
	SDL_SetCursor(SDL_CreateColorCursor(SDL_LoadBMP(fileName.c_str()), hotspotX, hotspotY));
}

//=========================================================================
void Window::setCursor(const unsigned char* pixels, unsigned int width, unsigned int height, unsigned int channels, unsigned int hotspotX, unsigned int hotspotY)
{
	uint32_t rmask, gmask, bmask, amask;
	SDL_Surface* surface;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	if(channels == 4)
	{
		surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);
	}
	else if(channels == 3)
	{
		surface = SDL_CreateRGBSurface(0, width, height, 24, rmask, gmask, bmask, 0);
	}

	memcpy(surface->pixels, pixels, channels * width * height);

	SDL_SetCursor(SDL_CreateColorCursor(surface, hotspotX, hotspotY));
	SDL_FreeSurface(surface);
}

//=========================================================================
void Window::setIcon(const unsigned char* pixels, unsigned int width, unsigned int height, unsigned int channels)
{
	uint32_t rmask, gmask, bmask, amask;
	SDL_Surface* surface;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	if(channels == 4)
	{
		surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);
	}
	else if(channels == 3)
	{
		surface = SDL_CreateRGBSurface(0, width, height, 24, rmask, gmask, bmask, 0);
	}

	memcpy(surface->pixels, pixels, channels * width * height);

	SDL_SetWindowIcon(mWindow.get(), surface);
	SDL_FreeSurface(surface);
}

//=========================================================================
float Window::getAspectRatio()
{
	return (float)getWidth() / getHeight();
}

//=========================================================================
void Window::sleep(int milliseconds)
{
	SDL_Delay(milliseconds);
}
