#pragma once

#include <memory>
#include "Core/Window.h"
#include "Core/Shader.h"
#include "Core/Texture.h"
#include "COre/Geometry.h"

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

	void draw();
	void draw(Position position, float offsetX = 0.0f, float offsetY = 0.0f);
	float getWidth() const;
	float getHeight() const;
	void setRotation(float degrees);


private:
	Quad(const Quad&);
	Quad& operator = (const Quad&);

	float mWidth;
	float mHeight;
	float mRotation { 0.0f };
	glm::vec2 mPosition;
	glm::vec2 mScale { glm::vec2(1) };

	ShaderRef mShader;
	TextureRef mTexture;
	GeometryRef mGeometry;
	glm::mat4 mMatrix { glm::mat4(1.0f) };

	void setPosition(float x, float y);
	void updateMatrix();

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
void Quad::draw()
{
	glm::mat4 projection = glm::ortho(0.0f, (float)/*Window::*/getWidth(), (float)/*Window::*/getHeight(), 0.0f);

	auto model = mMatrix;
	//model = glm::translate(model, glm::vec3(mWidth / 2.0f, mHeight / 2.0f, 0.0f));
	//model = glm::rotate(model, mRotation, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::translate(model, glm::vec3(-mWidth / 2.0f, -mHeight / 2.0f, 0.0f));

	auto mvp = model * glm::mat4(1.0f) * projection;

	mShader->bind();
	mTexture->bind();
	mShader->setUniform(ShaderConstants::MVP, mvp);

	mGeometry->draw(*mShader);

	mTexture->unbind();
	mShader->unbind();
}

//=========================================================================
void Quad::draw(Position position, float offsetX, float offsetY)
{
	float x = 0.0f;
	float y = 0.0f;

	auto w = /*Window::*/getWidth();
	auto h = /*Window::*/getHeight();

	switch(position)
	{
		case Position::TOP:
			x = (w - mWidth) / w;
			break;

		case Position::BOTTOM:
			x = (w - mWidth) / w;
			y = -(h - mHeight) / h * 2;
			break;

		case Position::LEFT:
			y = -(h - mHeight) / h;
			break;

		case Position::RIGHT:
			x = (w - mWidth) / w * 2;
			y = -(h - mHeight) / h;
			break;

		case Position::TOPLEFT:
			break;

		case Position::TOPRIGHT:
			x = (w - mWidth) / w * 2;
			break;

		case Position::BOTTOMLEFT:
			y = -(h - mHeight) / h * 2;
			break;

		case Position::BOTTOMRIGHT:
			x = (w - mWidth) / w * 2;
			y = -(h - mHeight) / h * 2;
			break;

		case Position::CENTER:
			x = (w - mWidth) / w;
			y = -(h - mHeight) / h;
			break;

		default:
			break;
	}

	setPosition(x + offsetX / w, y - offsetY / h);
	draw();
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
	mRotation = degrees; // glm::radians(degrees);
	updateMatrix();
}

//=========================================================================
void Quad::setPosition(float x, float y)
{
	mPosition = glm::vec2(x, y);
	updateMatrix();
}

//=========================================================================
void Quad::updateMatrix()
{
	mMatrix = glm::mat4(1.0f);
	mMatrix = glm::translate(mMatrix, glm::vec3(mPosition.x, mPosition.y, 0.0f));
	mMatrix = glm::rotate(mMatrix, mRotation, glm::vec3(0.0f, 0.0f, 1.0f));
	mMatrix = glm::scale(mMatrix, glm::vec3(mScale.x, mScale.y, 0.0f));
}
