#pragma once

#include <memory>
#include "Core/Shader.h"
#include "Core/Texture.h"
#include "Core/Geometry.h"

typedef std::shared_ptr<class Quad> QuadRef;

class Quad
{
public:
	enum Position
	{
		TOP,
		BOTTOM,
		LEFT,
		RIGHT,
		TOPLEFT,
		TOPRIGHT,
		BOTTOMLEFT,
		BOTTOMRIGHT,
		CENTER
	};

	static QuadRef create(const std::string& fileName = std::string(), float width = 0, float height = 0);
	Quad(const std::string& fileName = std::string(), float width = 0, float height = 0);
	virtual ~Quad();

	void draw(unsigned int windowWidth, unsigned int windowHeight);
	void draw(unsigned int windowWidth, unsigned int windowHeight, Position position, float offsetX = 0.0f, float offsetY = 0.0f);
	float getWidth() const;
	float getHeight() const;
	void setPosition(float x, float y);
	void setRotation(float degrees);


private:
	Quad(const Quad&);
	Quad& operator = (const Quad&);

	float mWidth;
	float mHeight;
	unsigned int mWindowWidth;
	unsigned int mWindowHeight;
	float mRotation { 0.0f };
	glm::vec2 mPosition;
	glm::vec2 mScale { glm::vec2(1) };

	ShaderRef mShader;
	TextureRef mTexture;
	GeometryRef mGeometry;
	glm::mat4 mMatrix { glm::mat4(1.0f) };

};

//=========================================================================
QuadRef Quad::create(const std::string& fileName, float width, float height)
{
	return QuadRef(new Quad(fileName, width, height));
}

//=========================================================================
Quad::Quad(const std::string& fileName, float width, float height)
{
	mShader = Shader::create("assets/shaders/quad");
	mTexture = Texture::create(fileName);

	if(width == 0 || height == 0)
	{
		mWidth = (float)mTexture->getWidth();
		mHeight = (float)mTexture->getHeight();
	}
	else
	{
		mWidth = width;
		mHeight = height;
	}

	std::vector<glm::vec2> vertices =
	{
		glm::vec2(0.0f, 0.0f),
		glm::vec2(mWidth, 0.0f),
		glm::vec2(mWidth, mHeight),
		glm::vec2(0.0f, mHeight)
	};

	std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };

	std::vector<glm::vec2> texCoords =
	{
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f)
	};

	mGeometry = Geometry::create();
	mGeometry->setVertices(vertices);
	mGeometry->setIndices(indices);
	mGeometry->setTexCoords(texCoords);
}

//=========================================================================
Quad::~Quad()
{
}

//=========================================================================
void Quad::draw(unsigned int windowWidth, unsigned int windowHeight)
{
	auto halfImageScale = glm::vec2((float)mWidth / (float)windowWidth, (float)mHeight / (float)windowHeight);

	//mPosition.x = -1 + halfImageScale.x + mPosition.x / windowWidth;
	//mPosition.y = 1 - halfImageScale.y - mPosition.y / windowHeight;

	mMatrix = glm::mat4(1.0f);
	mMatrix = glm::rotate(mMatrix, mRotation, glm::vec3(0.0f, 0.0f, 1.0f));
	mMatrix = glm::translate(mMatrix, glm::vec3(1.0, -1.0f, 0.0f));

	//mMatrix = glm::translate(mMatrix, glm::vec3(mPosition.x, mPosition.y, 0.0f));
	
	//mMatrix = glm::scale(mMatrix, glm::vec3((float)mWidth / (float)windowWidth, (float)mHeight / (float)windowHeight, 0.0f));

	glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f);
	auto mvp = mMatrix * glm::mat4(1.0f) * projection;

	mShader->bind();
	mTexture->bind();
	mShader->setUniform(ShaderConstants::MVP, mvp);

	mGeometry->draw(*mShader);

	mTexture->unbind();
	mShader->unbind();
}

//=========================================================================
void Quad::draw(unsigned int windowWidth, unsigned int windowHeight, Position position, float offsetX, float offsetY)
{
	float x = 0.0f;
	float y = 0.0f;

	auto w = windowWidth;
	auto h = windowHeight;

	auto scaleX = (float)mWidth / (float)windowWidth;
	auto scaleY = (float)mHeight / (float)windowHeight;

	position = LEFT;

	switch(position)
	{
		case Position::TOP:
			mPosition = glm::vec2(windowWidth - mWidth, 0.0f);
			break;

		case Position::BOTTOM:
			mPosition = glm::vec2(windowWidth - mWidth, windowHeight * 2 - mHeight * 2);
			break;

		case Position::LEFT:
			mPosition = glm::vec2(0.0f, windowHeight - mHeight);
			break;

		case Position::RIGHT:
			mPosition = glm::vec2(1, 0);
			break;

		case Position::TOPLEFT:
			mPosition = glm::vec2(-1, 1);
			break;

		case Position::TOPRIGHT:
			mPosition = glm::vec2(1, 1);
			break;

		case Position::BOTTOMLEFT:
			mPosition = glm::vec2(-1, -1);
			break;

		case Position::BOTTOMRIGHT:
			mPosition = glm::vec2(1, -1);
			break;

		case Position::CENTER:
			//x = (w - mWidth) / w;
			//y = -(h - mHeight) / h;
			break;

		default:
			break;
	}

	//mPosition.x += offsetX / windowWidth;
	//mPosition.y -= offsetY / windowHeight;

	//setPosition(x + offsetX / w, y - offsetY / h);
	draw(windowWidth, windowHeight);
}

//=========================================================================
float Quad::getWidth() const
{
	return mWidth;
}

//=========================================================================
float Quad::getHeight() const
{
	return mHeight;
}

//=========================================================================
void Quad::setRotation(float degrees)
{
	mRotation = degrees;
}

//=========================================================================
void Quad::setPosition(float x, float y)
{
	mPosition = glm::vec2(x, y);
}
