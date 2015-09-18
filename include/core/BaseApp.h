#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "core/GL.h"
#include "core/Ui.h"
#include <glfw/glfw3.h>
#include <glfw/glfw3native.h>
#include "core/Input.h"
#include "core/Renderer.h"
#include "core/Camera.h"
#include "core/Log.h"


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

protected:
	Renderer *renderer;
	Camera *camera;

private:
	GLFWwindow *mWindow;
	Input *mInput;
	bool mRunning;

private:
	inline static void errorCallback(int errorCode, const char* description);
	inline static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
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
	baseApp->mInput->setKeyStatus(key, action == GLFW_PRESS ? true : false);
}

//=========================================================================
void BaseApp::mouseButtonCallback(GLFWwindow* window, int button, int state, int mod)
{
	BaseApp *baseApp = static_cast<BaseApp*>(glfwGetWindowUserPointer(window));

	MouseButton mouseButton;

	if (button == GLFW_MOUSE_BUTTON_LEFT)
		mouseButton = MouseButton::Left;
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
		mouseButton = MouseButton::Right;
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		mouseButton = MouseButton::Middle;
	else
		return; // Unsupported

	baseApp->mInput->setMouseButtonStatus(mouseButton, state == GLFW_PRESS);
}

//=========================================================================
void BaseApp::cursorPosCallback(GLFWwindow* window, double x, double y)
{
	BaseApp *baseApp = static_cast<BaseApp*>(glfwGetWindowUserPointer(window));
	baseApp->mInput->setMousePositionStatus(static_cast<int>(x), static_cast<int>(y));
}

//=========================================================================
void BaseApp::scrollCallback(GLFWwindow* window, double offsetX, double offsetY)
{
	BaseApp *baseApp = static_cast<BaseApp*>(glfwGetWindowUserPointer(window));
	baseApp->mInput->setMouseScrollStatus(offsetX, offsetY);
}

//=========================================================================
void BaseApp::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	gl::setViewport(width, height);
	BaseApp *baseApp = static_cast<BaseApp*>(glfwGetWindowUserPointer(window));
	baseApp->onResize(width, height);
}

//=========================================================================
BaseApp::BaseApp(int width, int height, WindowMode mode)
{
	glfwSetErrorCallback(&errorCallback);

	if (!glfwInit())
		exit(1);

	//glfwWindowHint(GLFW_SAMPLES, 4);
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

	mWindow = glfwCreateWindow(width, height, "", (mode == WindowMode::FullScreen || mode == WindowMode::FullScreenNative) ? mon : NULL, NULL);

	if (!mWindow)
		exit(1);

	glfwMakeContextCurrent(mWindow);
	glfwSetWindowPos(mWindow, vidmode->width / 2 - width / 2, vidmode->height / 2 - height / 2);
	//glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, GL_TRUE);
	//glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// VSync: 0 - off, 1 - on
	glfwSwapInterval(1);

	glfwSetWindowUserPointer(mWindow, this);
	glfwSetKeyCallback(mWindow, keyCallback);
	glfwSetMouseButtonCallback(mWindow, mouseButtonCallback);
	glfwSetCursorPosCallback(mWindow, cursorPosCallback);
	glfwSetScrollCallback(mWindow, scrollCallback);
	glfwSetFramebufferSizeCallback(mWindow, framebufferSizeCallback);

	// Init OpenGL
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

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

	mInput = new Input();
	renderer = new Renderer();
	camera = new Camera();

	ImGuiWrapper::ImGui_ImplGlfwGL3_Init(mWindow, false);
	//ImGuiWrapper::setStyle();
}

//=========================================================================
BaseApp::~BaseApp()
{
	if (camera)
		delete camera;

	if (renderer)
		delete renderer;

	if (mInput)
		delete mInput;
	
	if (mWindow)
	{
		ImGuiWrapper::ImGui_ImplGlfwGL3_Shutdown();
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
	double lastTimeStamp = 0.0;
	double accumulator = 0.0;
	double frameCounter = 0.0;
	const double frameTime = 1.0 / 60.0;
	int frames = 0;

	while (!glfwWindowShouldClose(mWindow) && mRunning)
	{
		const double timeStamp = glfwGetTime();
		const double dt = timeStamp - lastTimeStamp;
		lastTimeStamp = timeStamp;
		accumulator += dt;
		frameCounter += dt;

		// Reset mouse statuses
		onInput(*mInput);
		mInput->setMouseScrollStatus(0, 0);
		mInput->setMousePositionChangeStatus(0, 0);

		// 		if(frameCounter >= 1.0)
		// 		{
		// 			double totalTime = 1000.0 * frameCounter / (double)frames;
		// 			double fps = 1000.0f / totalTime;
		// 			glfwSetWindowTitle(mWindow, std::string(mTitle + " [" + std::to_string(fps) + "]").c_str());
		// 			frames = 0;
		// 			frameCounter = 0;
		// 		}

		while (mRunning && accumulator >= frameTime)
		{
			onUpdate(frameTime);
			accumulator -= frameTime;
		}

		// Enable 3D rendering & alpha
		gl::enable3D();
		gl::enableAlphaBlending();
		gl::clear(renderer->mClearColor);

		onDraw();

		static float clear_color = 0.0f;
		static bool show_test_window;
		static bool show_another_window;

		ImGuiWrapper::ImGui_ImplGlfwGL3_NewFrame();
		
		{
			static float f = 0.0f;
			ImGui::Text("Hello, world!");
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
			ImGui::ColorEdit3("clear color", (float*)&clear_color);
			if (ImGui::Button("Test Window")) show_test_window ^= 1;
			if (ImGui::Button("Another Window")) show_another_window ^= 1;
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}

		// 2. Show another simple window, this time using an explicit Begin/End pair
		if (show_another_window)
		{
			ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
			ImGui::Begin("Another Window", &show_another_window);
			ImGui::Text("Hello");
			ImGui::End();
		}

		// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
		if (show_test_window)
		{
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
			ImGui::ShowTestWindow(&show_test_window);
		}

		ImGui::Render();

		glfwSwapBuffers(mWindow);
		glfwPollEvents();
		frames++;
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
