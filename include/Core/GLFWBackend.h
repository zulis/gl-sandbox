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

class GLFWBackend
{
public:
	GLFWBackend(int width, int height, WindowMode mode, const std::string& title);
	virtual ~GLFWBackend();

	void run();

	void setTitle(const std::string& title);

	virtual void resize(int width, int height) { }

	virtual void input(const Input& input) {}

	virtual void update(double dt) {}

	virtual void draw() = 0;

	double time() const
	{
		return mTotalTime;
	}

	void quit() { mRunning = false; };

	const Input& getInput() const
 	{
 		return mInput;
 	}
 
 	Input& getInput()
 	{
 		return mInput;
 	}

	const glm::mat4 getOrtho() const;

	GLFWwindow* getGLFWWindow() const
	{
		return mWindow;
	}

private:
	GLFWwindow* mWindow { nullptr };
	bool mRunning{ true };
	double mLastTimeStamp { 0.0 };
	double mTotalTime { 0.0 };

	Input mInput;

	inline static void errorCallback(int errorCode, const char* description);
	inline static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	inline static void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
	inline static void cursorPosCallback(GLFWwindow* window, double x, double y);
	inline static void scrollCallback(GLFWwindow* window, double offsetX, double offsetY);
	inline static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};

// Callbacks
//=========================================================================
void GLFWBackend::errorCallback(int errorCode, const char* description)
{
	logError(description);
}

//=========================================================================
void GLFWBackend::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	GLFWBackend* currentWindow = static_cast<GLFWBackend*>(glfwGetWindowUserPointer(window));

	bool isDown;

	if(action == GLFW_PRESS)
		isDown = true;
	else if(action == GLFW_RELEASE)
		isDown = false;
	else
		// GLFW_REPEAT must be ignored
		return;

	currentWindow->getInput().setKey(key, isDown);
}

//=========================================================================
void GLFWBackend::mouseButtonCallback(GLFWwindow* window, int button, int state, int mod)
{
	GLFWBackend* currentWindow = static_cast<GLFWBackend*>(glfwGetWindowUserPointer(window));
	currentWindow->getInput().setMouseButton(button, state == GLFW_PRESS);
}

//=========================================================================
void GLFWBackend::cursorPosCallback(GLFWwindow* window, double x, double y)
{
	GLFWBackend* currentWindow = static_cast<GLFWBackend*>(glfwGetWindowUserPointer(window));
	currentWindow->getInput().setMousePos(x, y);
}

//=========================================================================
void GLFWBackend::scrollCallback(GLFWwindow* window, double offsetX, double offsetY)
{
	GLFWBackend* currentWindow = static_cast<GLFWBackend*>(glfwGetWindowUserPointer(window));
	currentWindow->getInput().setScroll(offsetX, offsetY);
}

//=========================================================================
void GLFWBackend::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	GLFWBackend* currentWindow = static_cast<GLFWBackend*>(glfwGetWindowUserPointer(window));
	currentWindow->resize(width, height);
}

//=========================================================================
GLFWBackend::GLFWBackend(int width, int height, WindowMode mode, const std::string& title)
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

	if (mode == WindowMode::FullScreenNative)
	{
		width = vidmode->width;
		height = vidmode->height;
	}
	
	mWindow = glfwCreateWindow(width, height, title.c_str(), (mode == WindowMode::FullScreen || mode == WindowMode::FullScreenNative) ? mon : NULL, NULL);

	if (!mWindow)
		exit(1);

	glfwMakeContextCurrent(mWindow);
	glfwSetWindowPos(mWindow, vidmode->width / 2 - width / 2, vidmode->height / 2 - height / 2);
	//glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, GL_TRUE);
	//glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetWindowUserPointer(mWindow, this);
	glfwSetKeyCallback(mWindow, keyCallback);
	glfwSetMouseButtonCallback(mWindow, mouseButtonCallback);
	glfwSetCursorPosCallback(mWindow, cursorPosCallback);
	glfwSetScrollCallback(mWindow, scrollCallback);
	glfwSetFramebufferSizeCallback(mWindow, framebufferSizeCallback);

	// Init OpenGL
	logNote("Initializing OpenGL...");

	if(ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		logError("OpenGL initialization failed.");
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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
}

//=========================================================================
GLFWBackend::~GLFWBackend()
{
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

//=========================================================================
void GLFWBackend::run()
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
		}

		draw();

		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}
}

//=========================================================================
void GLFWBackend::setTitle(const std::string& title)
{
	glfwSetWindowTitle(mWindow, title.c_str());
}

//=========================================================================
const glm::mat4 GLFWBackend::getOrtho() const
{
	int width;
	int height;
	glfwGetFramebufferSize(mWindow, &width, &height);

	return glm::ortho(0.0f, (float)width, (float)height, (float)0.0f);
}