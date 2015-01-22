#pragma once

#include <exception>
#include "Core/Window.h"
#include "Core/StateManager.h"

class Game : public Window
{
public:
	Game();
	virtual ~Game();

	template<class T>
	void run();

	virtual void resize(unsigned int width, unsigned int height);
	virtual void input(Input& input);
	virtual void update(double elapsedTime);
	virtual void draw();

};

//=========================================================================
Game::Game() : Window(1280, 720, WindowMode::Windowed)
{
	setTitle("Game!");
}

//=========================================================================
Game::~Game()
{
}

//=========================================================================
template<class T>
void Game::run()
{
	auto state = StateManager::setState<T>();
	Window::run();
}

//=========================================================================
void Game::resize(unsigned int width, unsigned int height)
{
	StateManager::resize(width, height);
}

//=========================================================================
void Game::input(Input& input)
{
	StateManager::input(input);
}

//=========================================================================
void Game::update(double elapsedTime)
{
	if (StateManager::shouldQuit())
		quit();
	else
		StateManager::update(elapsedTime);
}

//=========================================================================
void Game::draw()
{
	gl::clear(Color::gray(0.6f));
	StateManager::draw();
}

//=========================================================================
#define RUN(STATE)                                                        \
int main(int argc, char *argv[])                                          \
{                                                                         \
	try                                                                   \
	{							                                          \
		Game game;				                                          \
		game.run<STATE>();                                                \
		return 0;                                                         \
	}                                                                     \
	catch (std::exception *e)                                             \
	{                                                                     \
		return 1;                                                         \
	}                                                                     \
}                                                                         \

