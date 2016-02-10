#pragma once

// https://github.com/SuperV1234/Tutorials/blob/master/DiveIntoC%2B%2B11/5_Entities/p9.cpp

#include "core/BaseApp.h"
#include "core/Entities.h"

constexpr int windowWidth{ 800 }, windowHeight{ 600 };
constexpr float ballRadius{ 10.f }, ballVelocity{ 200.f };
constexpr float paddleWidth{ 60.f }, paddleHeight{ 20.f }, paddleVelocity{ 600.f };
constexpr float blockWidth{ 60.f }, blockHeight{ 20.f };
constexpr int countBlocksX{ 11 }, countBlocksY{ 4 };

struct CPosition;
struct CPhysics;
struct CCircle;
struct CRectangle;
struct CPaddle;
struct CoreComponent;

//=========================================================================
class Game : public BaseApp
{
public:
	Game();
	~Game();

	void onInput(const Input &input) override;
	void onUpdate(double deltaTime) override;
	void onDraw() override;
	void onResize(const unsigned int width, const unsigned int height) override;

private:
	Manager manager;

	enum ArkanoidGroup : std::size_t
	{
		GPaddle,
		GBrick,
		GBall
	};

	//=========================================================================
	struct CPosition : Component
	{
		vec2 position;

		CPosition() = default;
		CPosition(const vec2& mPosition) : position(mPosition) {}

		float x() const noexcept { return position.x; }
		float y() const noexcept { return position.y; }
	};

	//=========================================================================
	struct CPhysics : Component
	{
		CPosition* cPosition{ nullptr };
		vec2 velocity, halfSize;

		// We will use a callback to handle the "out of bounds" event.
		std::function<void(const vec2&)> onOutOfBounds;

		CPhysics(const vec2& mHalfSize) : halfSize{ mHalfSize } {}

		void init() override
		{
			// A requirement for `CPhysics` is obviously `CPosition`.
			cPosition = &entity->getComponent<CPosition>();
		}

		void update(float mFT) override
		{
			cPosition->position += velocity * mFT;

			if (onOutOfBounds == nullptr) return;

			if (left() < 0)
				onOutOfBounds(vec2{ 1.f, 0.f });
			else if (right() > windowWidth)
				onOutOfBounds(vec2{ -1.f, 0.f });

			if (top() < 0)
				onOutOfBounds(vec2{ 0.f, 1.f });
			else if (bottom() > windowHeight)
				onOutOfBounds(vec2{ 0.f, -1.f });
		}

		float x() const noexcept { return cPosition->x(); }
		float y() const noexcept { return cPosition->y(); }
		float left() const noexcept { return x() - halfSize.x; }
		float right() const noexcept { return x() + halfSize.x; }
		float top() const noexcept { return y() - halfSize.y; }
		float bottom() const noexcept { return y() + halfSize.y; }
	};

	//=========================================================================
	struct CMesh : Component
	{
		Game* game{ nullptr };

		Mesh mesh;
		ShaderID shader;
		TextureID texColor;
		//TextureID texNormal;
		//TextureID texSpecular;

		Transform transform;

		CMesh(Game* mGame, const std::string& fileName, Color color) : game{ mGame }
		{
			shader = game->renderer.addShader("assets/shaders/basic.shd", Shader::SourceType::File);
			texColor = game->renderer.addTexture(color);
			//texNormal = renderer->addTexture(Color::white());
			//texSpecular = renderer->addTexture(Color::white());

			mesh.loadFromFile(fileName);
			mesh.makeDrawable(game->renderer, shader);
		}

		void init() override
		{
		}

		void update(float mFT) override
		{
		}

		void draw() override
		{
			game->renderer.setShader(shader);
			game->renderer.setShaderUniform(ShaderConstants::ProjectionMatrix, game->camera.getProjectionMatrix());
			game->renderer.setShaderUniform(ShaderConstants::ModelViewMatrix, game->camera.getViewMatrix() * transform.getMatrix());
			game->renderer.setShaderUniform(ShaderConstants::MVP, game->camera.getProjectionMatrix() * game->camera.getViewMatrix() * transform.getMatrix());

			auto mv = game->camera.getViewMatrix() * transform.getMatrix();
			game->renderer.setShaderUniform(ShaderConstants::NormalMatrix, mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

			// Color map
			game->renderer.setTexture(texColor);
			game->renderer.setShaderUniform(ShaderConstants::ColorMap, 0);
			game->renderer.setShaderUniform(ShaderConstants::ColorMapIsUsed, true);

			mesh.draw();
		}
	};

	//=========================================================================
	struct CCircle : Component
	{
		Game* game{ nullptr };
		CPosition* cPosition{ nullptr };
		CMesh* mesh;
		float radius;

		CCircle(Game* mGame, float mRadius) : game{ mGame }, radius{ mRadius } {}

		void init() override
		{
			cPosition = &entity->getComponent<CPosition>();
			mesh = &entity->addComponent<CMesh>(game, "assets/models/misc/sphere.fbx", Color::red());
			mesh->transform.setScale(radius);
		}

		void update(float mFT) override
		{
			mesh->transform.setPosition(cPosition->x(), cPosition->y(), 0.f);
		}

		void draw() override
		{
			mesh->draw();
		}
	};

	//=========================================================================
	struct CRectangle : Component
	{
		Game* game{ nullptr };
		CPosition* cPosition{ nullptr };
		CMesh* mesh;
		vec2 size;

		CRectangle(Game* mGame, const vec2& mHalfSize) : game{ mGame }, size{ mHalfSize * 2.f } {}

		void init() override
		{
			cPosition = &entity->getComponent<CPosition>();
			mesh = &entity->addComponent<CMesh>(game, "assets/models/box/box.fbx", Color::blue());
			mesh->transform.setScale(size.x, size.y, 1.f);
		}

		void update(float mFT) override
		{
			mesh->transform.setPosition(cPosition->x(), cPosition->y(), 0.f);
		}
		void draw() override
		{
			mesh->draw();
		}
	};

	//=========================================================================
	struct CPaddleControl : Component
	{
		Game* game{ nullptr };
		CPhysics* cPhysics{ nullptr };

		CPaddleControl(Game* mGame) : game{ mGame } {}

		void init() override { cPhysics = &entity->getComponent<CPhysics>(); }

		void update(float mFT)
		{
			if (game->input.isKeyDown(KEY_LEFT) && cPhysics->left() > 0)
				cPhysics->velocity.x = -paddleVelocity;
			else if (game->input.isKeyDown(KEY_RIGHT) && cPhysics->right() < windowWidth)
				cPhysics->velocity.x = paddleVelocity;
			else
				cPhysics->velocity.x = 0;
		}
	};

	//=========================================================================
	template <class T1, class T2>
	bool isIntersecting(T1& mA, T2& mB) noexcept
	{
		return mA.right() >= mB.left() && mA.left() <= mB.right() &&
			mA.bottom() >= mB.top() && mA.top() <= mB.bottom();
	}

	//=========================================================================
	void testCollisionPB(Entity& mPaddle, Entity& mBall) noexcept
	{
		auto& cpPaddle(mPaddle.getComponent<CPhysics>());
		auto& cpBall(mBall.getComponent<CPhysics>());

		if (!isIntersecting(cpPaddle, cpBall)) return;

		cpBall.velocity.y = -ballVelocity;
		if (cpBall.x() < cpPaddle.x())
			cpBall.velocity.x = -ballVelocity;
		else
			cpBall.velocity.x = ballVelocity;
	}

	//=========================================================================
	void testCollisionBB(Entity& mBrick, Entity& mBall) noexcept
	{
		auto& cpBrick(mBrick.getComponent<CPhysics>());
		auto& cpBall(mBall.getComponent<CPhysics>());

		if (!isIntersecting(cpBrick, cpBall)) return;
		mBrick.destroy();

		float overlapLeft{ cpBall.right() - cpBrick.left() };
		float overlapRight{ cpBrick.right() - cpBall.left() };
		float overlapTop{ cpBall.bottom() - cpBrick.top() };
		float overlapBottom{ cpBrick.bottom() - cpBall.top() };

		bool ballFromLeft(std::abs(overlapLeft) < std::abs(overlapRight));
		bool ballFromTop(std::abs(overlapTop) < std::abs(overlapBottom));

		float minOverlapX{ ballFromLeft ? overlapLeft : overlapRight };
		float minOverlapY{ ballFromTop ? overlapTop : overlapBottom };

		if (std::abs(minOverlapX) < std::abs(minOverlapY))
			cpBall.velocity.x = ballFromLeft ? -ballVelocity : ballVelocity;
		else
			cpBall.velocity.y = ballFromTop ? -ballVelocity : ballVelocity;
	}

	//=========================================================================
	Entity& createBall()
	{
		auto& entity(manager.addEntity());

		entity.addComponent<CPosition>(vec2{ windowWidth / 2.f, windowHeight / 2.f });
		entity.addComponent<CPhysics>(vec2{ ballRadius, ballRadius });
		entity.addComponent<CCircle>(this, ballRadius);

		auto& cPhysics(entity.getComponent<CPhysics>());
		cPhysics.velocity = vec2{ -ballVelocity, -ballVelocity };
		cPhysics.onOutOfBounds = [&cPhysics](const vec2& mSide)
		{
			if (mSide.x != 0.f)
				cPhysics.velocity.x = std::abs(cPhysics.velocity.x) * mSide.x;

			if (mSide.y != 0.f)
				cPhysics.velocity.y = std::abs(cPhysics.velocity.y) * mSide.y;
		};

		entity.addGroup(ArkanoidGroup::GBall);

		return entity;
	}

	//=========================================================================
	Entity& createBrick(const vec2& mPosition)
	{
		vec2 halfSize{ blockWidth / 2.f, blockHeight / 2.f };
		auto& entity(manager.addEntity());

		entity.addComponent<CPosition>(mPosition);
		entity.addComponent<CPhysics>(halfSize);
		entity.addComponent<CRectangle>(this, halfSize);

		entity.addGroup(ArkanoidGroup::GBrick);

		return entity;
	}

	//=========================================================================
	Entity& createPaddle(Game* game)
	{
		vec2 halfSize{ paddleWidth / 2.f, paddleHeight / 2.f };
		auto& entity(manager.addEntity());

		entity.addComponent<CPosition>(vec2{ windowWidth / 2.f, windowHeight - 60.f });
		entity.addComponent<CPhysics>(halfSize);
		entity.addComponent<CRectangle>(this, halfSize);
		entity.addComponent<CPaddleControl>(game);

		entity.addGroup(ArkanoidGroup::GPaddle);

		return entity;
	}
};

//=========================================================================
Game::Game() : BaseApp(800, 600, WindowMode::Windowed)
{
	setTitle("Game...");

	createPaddle(this);
	createBall();

	for (int iX{ 0 }; iX < countBlocksX; ++iX)
		for (int iY{ 0 }; iY < countBlocksY; ++iY)
			createBrick(vec2{ (iX + 1) * (blockWidth + 3) + 22,
				(iY + 2) * (blockHeight + 3) });

	camera.setPosition(windowWidth / 2.f, windowHeight / 2.f, 520.f);
	camera.setLookAt(windowWidth / 2.f, windowHeight / 2.f, 0);
	//camera.rotate(360.f, 0.f);

}

//=========================================================================
Game::~Game()
{
}

//=========================================================================
void Game::onInput(const Input &input)
{
	if (input.isKeyDown(KEY_ESCAPE))
		quit();

	/*
	if (input.isKeyDown(KEY_W))
		camera.move(Camera::FORWARD);
	else if (input.isKeyDown(KEY_S))
		camera.move(Camera::BACKWARD);

	if (input.isKeyDown(KEY_A))
		camera.move(Camera::LEFT);
	else if (input.isKeyDown(KEY_D))
		camera.move(Camera::RIGHT);

	if (input.isKeyDown(KEY_E))
		camera.move(Camera::UP);
	else if (input.isKeyDown(KEY_Q))
		camera.move(Camera::DOWN);

	if (input.getMouseScroolY() != 0)
		camera.move(input.getMouseScroolY() > 0 ? Camera::FORWARD : Camera::BACKWARD);

	if (input.isMouseDown(Input::MouseButton::Right))
	{
		hideMouse();
		camera.rotate(static_cast<float>(input.getMouseChangeX()), static_cast<float>(input.getMouseChangeY()));
	}
	else
		showMouse();
		*/
}

//=========================================================================
void Game::onUpdate(double deltaTime)
{
	manager.refresh();
	manager.update(deltaTime);

	// We get our entities by group...
	auto& paddles(manager.getEntitiesByGroup(GPaddle));
	auto& bricks(manager.getEntitiesByGroup(GBrick));
	auto& balls(manager.getEntitiesByGroup(GBall));

	// ...and perform collision tests on them.
	for (auto& b : balls)
	{
		for (auto& p : paddles) testCollisionPB(*p, *b);

		for (auto& br : bricks) testCollisionBB(*br, *b);
	}
}

//=========================================================================
void Game::onDraw()
{
	//gl::clear(Color::gray(0.5f));

	manager.draw();
}

//=========================================================================
void Game::onResize(const unsigned int width, const unsigned int height)
{
	camera.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
}
