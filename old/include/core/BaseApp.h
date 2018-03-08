#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
//#define GLFW_EXPOSE_NATIVE_WIN32
//#define GLFW_EXPOSE_NATIVE_WGL
#include <windows.h>
#endif

#include <string>
#include "core/GL.h"
#include <glfw/glfw3.h>
//#include <glfw/glfw3native.h>
#include "core/Math.h"
#include "core/Input.h"
#include "core/Renderer.h"
#include "core/Camera.h"
#include "core/Transform.h"
//#include "core/Light.h"
#include "core/MeshData.h"
#include "core/MeshDataLoader.h"
#include "core/Mesh.h"
#include "core/AABB.h"
#include "core/Log.h"
#include "core/Hud.h"

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

enum class WindowMode
{
	Windowed,
	FullScreen,
	FullScreenNative
};

class BaseApp
{
public:
	BaseApp(int width, int height, WindowMode mode);
	virtual ~BaseApp();

	virtual void onInput(const Input &input) = 0;
	virtual void onUpdate(double deltaTime) = 0;
	virtual void onDraw() = 0;
	virtual void onResize(const unsigned int width, const unsigned int height) = 0;

	void run();
	void quit();
	void setTitle(const char *title);
	void showMouse();
	void hideMouse();
	void setClipboardText(const char *text);
	const char *getClipboardText();
	vec2 getViewportSize() const;
	unsigned int getViewportWidth() const;
	unsigned int getViewportHeight() const;
	float getTime() const;

//protected:
	Renderer renderer;
	Camera camera;
	Input input;

private:
	GLFWwindow *mWindow;
	std::string mTitle;
	bool mRunning{ false };
	vec2 mViewportSize;

private:
	inline static void errorCallback(int errorCode, const char* description);
	inline static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	inline static void charCallback(GLFWwindow* window, unsigned int codepoint);
	inline static void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
	inline static void cursorPosCallback(GLFWwindow* window, double x, double y);
	inline static void scrollCallback(GLFWwindow* window, double offsetX, double offsetY);
	inline static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

};

// Callbacks
//=========================================================================
void BaseApp::errorCallback(int errorCode, const char* description)
{
	error(description);
}

//=========================================================================
void BaseApp::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	BaseApp *baseApp = static_cast<BaseApp*>(glfwGetWindowUserPointer(window));

	bool isDown;

	if (action == GLFW_PRESS)
		isDown = true;
	else if (action == GLFW_RELEASE)
		isDown = false;
	else // GLFW_REPEAT must be ignored
		return;

	baseApp->input.setKeyStatus(key, isDown);
}

//=========================================================================
void BaseApp::charCallback(GLFWwindow* window, unsigned int codepoint)
{
	BaseApp *baseApp = static_cast<BaseApp*>(glfwGetWindowUserPointer(window));
}

//=========================================================================
void BaseApp::mouseButtonCallback(GLFWwindow* window, int button, int state, int mod)
{
	BaseApp *baseApp = static_cast<BaseApp*>(glfwGetWindowUserPointer(window));

	Input::MouseButton mouseButton;

	if (button == GLFW_MOUSE_BUTTON_LEFT)
		mouseButton = Input::MouseButton::Left;
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
		mouseButton = Input::MouseButton::Right;
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		mouseButton = Input::MouseButton::Middle;
	else
		return; // Unsupported

	baseApp->input.setMouseButtonStatus(mouseButton, state == GLFW_PRESS);
}

//=========================================================================
void BaseApp::cursorPosCallback(GLFWwindow* window, double x, double y)
{
	BaseApp *baseApp = static_cast<BaseApp*>(glfwGetWindowUserPointer(window));
	baseApp->input.setMousePositionStatus(static_cast<int>(x), static_cast<int>(y));
}

//=========================================================================
void BaseApp::scrollCallback(GLFWwindow* window, double offsetX, double offsetY)
{
	BaseApp *baseApp = static_cast<BaseApp*>(glfwGetWindowUserPointer(window));
	baseApp->input.setMouseScrollStatus(offsetX, offsetY);
}

//=========================================================================
void BaseApp::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	gl::setViewport(width, height);
	BaseApp *baseApp = static_cast<BaseApp*>(glfwGetWindowUserPointer(window));
	baseApp->mViewportSize = vec2(width, height);
	baseApp->onResize(width, height);
}

//=========================================================================
BaseApp::BaseApp(int width, int height, WindowMode mode)
{
	glfwSetErrorCallback(&errorCallback);

	if (!glfwInit())
		exit(1);

	//glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Specifies whether the OpenGL context should be forward-compatible
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	GLFWmonitor* mon = glfwGetPrimaryMonitor();
	const GLFWvidmode* vidmode = glfwGetVideoMode(mon);

	if (mode == WindowMode::FullScreenNative)
	{
		width = vidmode->width;
		height = vidmode->height;
	}

	mViewportSize = vec2(width, height);

	mWindow = glfwCreateWindow(width, height, "", (mode == WindowMode::FullScreen || mode == WindowMode::FullScreenNative) ? mon : NULL, NULL);

	if (!mWindow)
		exit(1);

	glfwMakeContextCurrent(mWindow);
	glfwSetWindowPos(mWindow, vidmode->width / 2 - width / 2, vidmode->height / 2 - height / 2);
	//glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, GL_TRUE);
	//glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// VSync: 0 - off, 1 - on
	glfwSwapInterval(0);

	glfwSetWindowUserPointer(mWindow, this);
	glfwSetKeyCallback(mWindow, keyCallback);
	glfwSetCharCallback(mWindow, charCallback);
	glfwSetMouseButtonCallback(mWindow, mouseButtonCallback);
	glfwSetCursorPosCallback(mWindow, cursorPosCallback);
	glfwSetScrollCallback(mWindow, scrollCallback);
	glfwSetFramebufferSizeCallback(mWindow, framebufferSizeCallback);

	// Init OpenGL
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, width, height);

	if (GLEW_OK != err)
	{
		error("OpenGL initialisation failed: %s", glewGetErrorString(err));
		glfwDestroyWindow(mWindow);
		glfwTerminate();
		exit(1);
	}

	// Display some information about the OpenGL version we are running
	const GLubyte* glRenderer = glGetString(GL_RENDERER);
	const GLubyte* glVendor = glGetString(GL_VENDOR);
	const GLubyte* glVersion = glGetString(GL_VERSION);
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

	GLint glMajor, glMinor;
	glGetIntegerv(GL_MAJOR_VERSION, &glMajor);
	glGetIntegerv(GL_MINOR_VERSION, &glMinor);

	note("GL Vendor    = %s", glVendor);
	note("GL Renderer  = %s", glRenderer);
	note("GL Version (string)  = %s", glVersion);
	note("GL Version (integer) = %d.%d", glMajor, glMinor);
	note("GLSL Version = %s", glslVersion);
}

//=========================================================================
BaseApp::~BaseApp()
{
	if (mWindow)
	{
		glfwDestroyWindow(mWindow);
		glfwTerminate();
	}
}

//=========================================================================
void BaseApp::run()
{
	mRunning = true;

	// Call resize before we start
	{
		int width, height;
		glfwGetFramebufferSize(mWindow, &width, &height);
		onResize(width, height);
	}

	glfwSetTime(0.0);
	const double limitFPS = 1.0 / 60.0;
	double lastTime = glfwGetTime(), timer = lastTime;
	double deltaTime = 0, nowTime = 0;
	int frames = 0, updates = 0;

	while (!glfwWindowShouldClose(mWindow) && mRunning)
	{
		// Measure time
		nowTime = glfwGetTime();
		deltaTime += (nowTime - lastTime) / limitFPS;
		lastTime = nowTime;

		// Reset mouse statuses
		onInput(input);
		input.setMouseScrollStatus(0, 0);
		input.setMousePositionChangeStatus(0, 0);

		// Only update at 60 frames / s
		while (deltaTime >= 1.0) {
			onUpdate(deltaTime);
			updates++;
			deltaTime--;
		}

		// Render at maximum possible frames
		gl::clear(renderer.mClearColor);
		onDraw();
		renderer.reset();
		frames++;

#ifdef _DEBUG
		if (glfwGetTime() - timer > 1.0)
		{
			timer++;
			glfwSetWindowTitle(mWindow, std::string(mTitle + " [FPS: " + std::to_string(static_cast<int>(trunc(frames))) + " Updates: " + std::to_string(updates) + "]").c_str());
			updates = 0, frames = 0;
		}
#endif

		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}
}

//=========================================================================
void BaseApp::quit()
{
	mRunning = false;
}

//=========================================================================
void BaseApp::setTitle(const char *title)
{
	mTitle = title;
	glfwSetWindowTitle(mWindow, title);
}

//=========================================================================
void BaseApp::showMouse()
{
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

//=========================================================================
void BaseApp::hideMouse()
{
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

//=========================================================================
void BaseApp::setClipboardText(const char *text)
{
	glfwSetClipboardString(mWindow, text);
}

//=========================================================================
const char *BaseApp::getClipboardText()
{
	return glfwGetClipboardString(mWindow);
}

//=========================================================================
glm::vec2 BaseApp::getViewportSize() const
{
	return mViewportSize;
}

//=========================================================================
unsigned int BaseApp::getViewportWidth() const
{
	return static_cast<int>(mViewportSize.x);
}

//=========================================================================
unsigned int BaseApp::getViewportHeight() const
{
	return static_cast<int>(mViewportSize.y);
}

//=========================================================================
float BaseApp::getTime() const
{
	return (GLfloat)glfwGetTime();
}
