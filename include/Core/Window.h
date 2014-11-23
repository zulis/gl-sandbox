#pragma once

#include <string>
#include <glm/glm.hpp>
#include "Core/GL.h"
#include <glfw/glfw3.h>
#include "Core/Input.h"
#include "Core/Log.h"

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

class Input;

enum class WindowMode
{
    Windowed,
    FullScreen,
    FullScreenNative
};

class Window
{
public:
	Window(int width, int height, WindowMode mode, const std::string& title);
	virtual ~Window();

	virtual void resize(unsigned int width, unsigned int height) = 0;
	virtual void input(Input& input) = 0;
	virtual void update(double elapsedTime) = 0;
	virtual void draw() = 0;

	void run();
	void quit();
	void setTitle(const std::string& title);
	const Input& getInput() const;
	Input& getInput();
	double getTime() const;
	const glm::mat4 getOrtho() const;
	void setMouseVisibility(bool isVisible);
	//GLFWwindow* getGLFWWindow() const;

private:
	GLFWwindow* mWindow { nullptr };
	Input mInput;
	bool mRunning { true };
	double mLastTimeStamp { 0.0 };
	double mTotalTime { 0.0 };

	inline static void errorCallback(int errorCode, const char* description);
	inline static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	inline static void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
	inline static void cursorPosCallback(GLFWwindow* window, double x, double y);
	inline static void scrollCallback(GLFWwindow* window, double offsetX, double offsetY);
	inline static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};

// Callbacks
//=========================================================================
void Window::errorCallback(int errorCode, const char* description)
{
	logError(description);
}

//=========================================================================
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Window* currentWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	bool isDown;

	if(action == GLFW_PRESS)
		isDown = true;
	else if(action == GLFW_RELEASE)
		isDown = false;
	else
		// GLFW_REPEAT must be ignored
		return;

	currentWindow->getInput().setKeyStatus(key, isDown);
}

//=========================================================================
void Window::mouseButtonCallback(GLFWwindow* window, int button, int state, int mod)
{
	MouseButton mouseButton;

	if(button == GLFW_MOUSE_BUTTON_LEFT)
		mouseButton = MouseButton::Left;
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
		mouseButton = MouseButton::Right;
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		mouseButton = MouseButton::Middle;
	else
		return; // Unsupported

	Window* currentWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	currentWindow->getInput().setMouseButtonStatus(mouseButton, state == GLFW_PRESS);
}

//=========================================================================
void Window::cursorPosCallback(GLFWwindow* window, double x, double y)
{
	Window* currentWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	currentWindow->getInput().setMousePositionStatus(x, y);
}

//=========================================================================
void Window::scrollCallback(GLFWwindow* window, double offsetX, double offsetY)
{
	Window* currentWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	currentWindow->getInput().setMouseScrollStatus(offsetX, offsetY);
}

//=========================================================================
void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	gl::setViewport(width, height);
	Window* currentWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	currentWindow->resize(width, height);
}

//=========================================================================
Window::Window(int width, int height, WindowMode mode, const std::string& title)
{
	glfwSetErrorCallback(&errorCallback);

	glfwInit();

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	GLFWmonitor* mon = glfwGetPrimaryMonitor();
	const GLFWvidmode* vidmode = glfwGetVideoMode(mon);

	if(mode == WindowMode::FullScreenNative)
	{
		width = vidmode->width;
		height = vidmode->height;
	}

	mWindow = glfwCreateWindow(width, height, title.c_str(), (mode == WindowMode::FullScreen || mode == WindowMode::FullScreenNative) ? mon : NULL, NULL);

	if(!mWindow)
		exit(1);

	glfwMakeContextCurrent(mWindow);
	glfwSetWindowPos(mWindow, vidmode->width / 2 - width / 2, vidmode->height / 2 - height / 2);
	//glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, GL_TRUE);
	//glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	glfwSetWindowUserPointer(mWindow, this);
	glfwSetKeyCallback(mWindow, keyCallback);
	glfwSetMouseButtonCallback(mWindow, mouseButtonCallback);
	glfwSetCursorPosCallback(mWindow, cursorPosCallback);
	glfwSetScrollCallback(mWindow, scrollCallback);
	glfwSetFramebufferSizeCallback(mWindow, framebufferSizeCallback);

	// Init OpenGL
	logNote("Initialising OpenGL...");

	if(ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		logError("OpenGL initialisation failed.");
		glfwDestroyWindow(mWindow);
		glfwTerminate();
		exit(1);
	}

	// Display some information about the OpenGL version we are running
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	logNote("GL Vendor    = %s", vendor);
	logNote("GL Renderer  = %s", renderer);
	logNote("GL Version (string)  = %s", version);
	logNote("GL Version (integer) = %d.%d", major, minor);
	logNote("GLSL Version = %s", glslVersion);
}

//=========================================================================
Window::~Window()
{
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

//=========================================================================
void Window::run()
{
	glfwSetTime(0.0);
	mLastTimeStamp = 0.0;
	mTotalTime = 0.0;

	// Call resize before we start
	{
		int width, height;
		glfwGetFramebufferSize(mWindow, &width, &height);
		resize(width, height);
	}

	double accumulator = 0.0;
	const double frameTime = 1.0 / 60.0;

	while(!glfwWindowShouldClose(mWindow) && mRunning)
	{
		const double timeStamp = glfwGetTime();
		const double dt = timeStamp - mLastTimeStamp;
		mLastTimeStamp = timeStamp;

		mTotalTime += dt;
		accumulator += dt;

		while(mRunning && accumulator >= frameTime)
		{
			input(mInput);
			update(frameTime);
			accumulator -= frameTime;

			// Reset mouse statuses
			mInput.setMouseScrollStatus(0, 0);
			mInput.setMouseChangeStatus(0, 0);
			// Check mouse visibility
			setMouseVisibility(mInput.isMouseVisible());
		}

		// Enable 3D rendering & alpha
		gl::enable3D();
		gl::enableAlphaBlending();

		draw();

		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}
}

//=========================================================================
void Window::quit()
{
	mRunning = false;
}

//=========================================================================
void Window::setTitle(const std::string& title)
{
	glfwSetWindowTitle(mWindow, title.c_str());
}

//=========================================================================
const Input& Window::getInput() const
{
	return mInput;
}

//=========================================================================
Input& Window::getInput()
{
	return mInput;
}

//=========================================================================
double Window::getTime() const
{
	return mTotalTime;
}

//=========================================================================
const glm::mat4 Window::getOrtho() const
{
	int width;
	int height;
	glfwGetFramebufferSize(mWindow, &width, &height);
	return glm::ortho(0.0f, (float)width, (float)height, (float)0.0f);
}

//=========================================================================
void Window::setMouseVisibility(bool isVisible)
{
	glfwSetInputMode(mWindow, GLFW_CURSOR, isVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

//=========================================================================
// GLFWwindow* Window::getGLFWWindow() const
// {
// 	return mWindow;
// }
