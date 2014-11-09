#pragma once

#include <functional>
#include <exception>
#include <cstdio>
#include "Core/Defines.h"
#include "Core/GL.h"
#include "Core/Window.h"
#include "Core/StateManager.h"
#include "Core/Time.h"
#include "Core/Log.h"

using namespace std::placeholders;

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

class Game
{
public:
	Game(double frameRate = 60);
	virtual ~Game();
	template<class T>
	void run();

private:
	bool initialise();
	void resize(int width, int height);
	void keyDown(int key, bool isDown);
	void mouseDown(int button, bool isDown);
	void mousePosition(int x, int y, int changeX, int changeY);
	void mouseWheel(int x);
	int mMouseX, mMouseY;
	double mFrameTime;
};

//=========================================================================
Game::Game(double frameRate) : mFrameTime(1.0 / frameRate)
{
}

//=========================================================================
Game::~Game()
{
}

//=========================================================================
bool Game::initialise()
{
	Window::create(640, 480, false, std::string());
	Window::setResizeCallback(std::bind(&Game::resize, this, _1, _2));
	Window::setKeyDownCallback(std::bind(&Game::keyDown, this, _1, _2));
	Window::setMouseDownCallback(std::bind(&Game::mouseDown, this, _1, _2));
	Window::setMousePositionCallback(std::bind(&Game::mousePosition, this, _1, _2, _3, _4));
	Window::setMouseWheelCallback(std::bind(&Game::mouseWheel, this, _1));

	// Init OPENGL
	if(ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		Window::showMessageBox("OpenGL initialization failed.");
		return false;
	}

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	logNote("Using OpenGL %i.%i", major, minor);

	// Enable 3D rendering & alpha
	gl::enable3D();
	gl::enableAlphaBlending();

	return true;
}

//=========================================================================
template<class T>
void Game::run()
{
	if(initialise())
	{
		StateManager::setState<T>();
		StateManager::resize(Window::getWidth(), Window::getHeight());

		double lastTime = Time::getTime(); // Current time at the start of the last frame
		double frameCounter = 0;           // Total passed time since last frame counter display
		double unprocessedTime = 0;        // Amount of passed time that the engine hasn't accounted for
		int frames = 0;                    // Number of frames rendered since last

		auto totalTime = 0;

		while(!StateManager::shouldQuit() && !Window::isClosed())
		{
			bool render = false;           // Whether or not the game needs to be rerendered.

			double startTime = Time::getTime();       //Current time at the start of the frame.
			double passedTime = startTime - lastTime; //Amount of passed time since last frame.
			lastTime = startTime;

			unprocessedTime += passedTime;
			frameCounter += passedTime;

			// The engine displays profiling statistics after every second because it needs to display them at some point.
			// The choice of once per second is arbitrary, and can be changed as needed.
			if(frameCounter >= 1.0)
			{
				double totalTime = ((1000.0 * frameCounter) / ((double)frames));
				double totalMeasuredTime = 0.0;

				//totalMeasuredTime += m_game->DisplayInputTime((double)frames);
				//totalMeasuredTime += m_game->DisplayUpdateTime((double)frames);
				//totalMeasuredTime += m_renderingEngine->DisplayRenderTime((double)frames);
				//totalMeasuredTime += sleepTimer.DisplayAndReset("Sleep Time: ", (double)frames);
				//totalMeasuredTime += windowUpdateTimer.DisplayAndReset("Window Update Time: ", (double)frames);
				//totalMeasuredTime += swapBufferTimer.DisplayAndReset("Buffer Swap Time: ", (double)frames);
				//totalMeasuredTime += m_renderingEngine->DisplayWindowSyncTime((double)frames);

				//printf("Other Time:                             %f ms\n", (totalTime - totalMeasuredTime));
				//printf("Total Time:                             %f ms\n\n", totalTime);
				auto fps = 1000.0f / totalTime;
				Window::setTitle(std::to_string(fps));
				frames = 0;
				frameCounter = 0;
			}

			//The engine works on a fixed update system, where each update is 1/frameRate seconds of time.
			//Because of this, there can be a situation where there is, for instance, a fixed update of 16ms,
			//but 20ms of actual time has passed. To ensure all time is accounted for, all passed time is
			//stored in unprocessedTime, and then the engine processes as much time as it can. Any
			//unaccounted time can then be processed later, since it will remain stored in unprocessedTime.
			while(unprocessedTime > mFrameTime)
			{
				//windowUpdateTimer.StartInvocation();

				/*if (m_window->IsCloseRequested())
				{
					Stop();
				}
				windowUpdateTimer.StopInvocation();*/

				//Input must be processed here because the window may have found new
				//input events from the OS when it updated. Since inputs can trigger
				//new game actions, the game also needs to be updated immediately
				//afterwards.
				//m_game->ProcessInput(m_window->GetInput(), (float)m_frameTime);
				Window::update();
				StateManager::update((float)mFrameTime);

				// Reset mouse
				mouseWheel(0);
				mousePosition(mMouseX, mMouseY, 0, 0);

				//Since any updates can put onscreen objects in a new place, the flag
				//must be set to rerender the scene.
				render = true;

				unprocessedTime -= mFrameTime;
			}

			if(render)
			{
				//m_game->Render(m_renderingEngine);

				// Clear window
				gl::clear(Color::gray(0.6f));

				// Draw current state
				StateManager::draw();

				//The newly rendered image will be in the window's backbuffer,
				//so the buffers must be swapped to display the new image.
				//swapBufferTimer.StartInvocation();
				Window::swapBuffers();
				//swapBufferTimer.StopInvocation();
				frames++;
			}
			else
			{
				//If no rendering is needed, sleep for some time so the OS
				//can use the processor for other tasks.
				//sleepTimer.StartInvocation();
				Window::sleep(1);
				//sleepTimer.StopInvocation();
			}
		}
	}

	Window::close();
}

//=========================================================================
void Game::resize(int width, int height)
{
	gl::setViewport(width, height);
	StateManager::resize(width, height);
}

//=========================================================================
void Game::keyDown(int key, bool isDown)
{
	StateManager::getState()->setKeyStatus(key, isDown);
}

//=========================================================================
void Game::mouseDown(int button, bool isDown)
{
	StateManager::getState()->setMouseButonStatus(button, isDown);
}

//=========================================================================
void Game::mousePosition(int x, int y, int changeX, int changeY)
{
	mMouseX = x;
	mMouseY = y;
	StateManager::getState()->setMousePositionStatus(x, y, changeX, changeY);
}

//=========================================================================
void Game::mouseWheel(int x)
{
	StateManager::getState()->setMouseWheelStatus(x);
}

#define RUN(STATE)                                             \
int main(int argc, char *argv[])                               \
{                                                              \
	try                                                        \
	{														   \
		Game game;											   \
		game.run<STATE>();                                     \
		return 0;                                              \
	}                                                          \
	catch (std::exception *e)                                  \
	{                                                          \
		Window::showMessageBox(e->what(), Window::Error);      \
		return 1;                                              \
	}                                                          \
}