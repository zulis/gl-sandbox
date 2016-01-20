#pragma once

#include "core/Shader.h"
#include "core/Texture.h"
#include "core/Geometry.h"

class Hud
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

	Hud(const std::string& fileName, unsigned int viewportWidth, unsigned int viewportHeight);
	~Hud();

	inline void draw();
	inline void setViewportSize(unsigned int viewportWidth, unsigned int viewportHeight);
	inline void setPosition(Position position, float offsetX = 0.0f, float offsetY = 0.0f);

private:
	vec2 mTextureSize;
	vec2 mViewportSize;
	vec2 mScale{ vec2(1) };
	vec2 mTranslate{ vec2(0) };
	Position mPosition{ Position::CENTER };
	Shader* mShader;
	Texture* mTexture;
	Geometry* mGeometry;
};

//=========================================================================
Hud::Hud(const std::string& fileName, unsigned int viewportWidth, unsigned int viewportHeight)
{
	mViewportSize = vec2(viewportWidth, viewportHeight);

	mShader = new Shader(MULTI_LINE(
	[Vertex]
	#version 430 \n

	in vec3 VertexPosition;
	out vec2 TexCoord;

	uniform vec2 translate;
	uniform vec2 scale;

	void main()
	{
		gl_Position = vec4(translate + vec2(VertexPosition) * scale, 0.0, 1.0);
		TexCoord = (vec2(VertexPosition.x, VertexPosition.y) + 1.0) * 0.5;
	}

	[Fragment]
	#version 430 \n

	in vec2 TexCoord;
	out vec4 FragColor;

	layout(binding = 0) uniform sampler2D ColorMap;

	void main()
	{
		FragColor = texture2D(ColorMap, TexCoord);
	}
	), Shader::SourceType::String);

	Texture::Format format;
	format.setFlipped(true);
	//format.setMagFilter(GL_NEAREST);
	//format.setMinFilter(GL_NEAREST);
	mTexture = new Texture(fileName, format);
	mTextureSize = mTexture->getSize();
	mScale = vec2(mTextureSize.x / mViewportSize.x, mTextureSize.y / mViewportSize.y);

	std::vector<vec2> vertices =
	{
		vec2(-1.0f, -1.0f),
		vec2(1.0f, -1.0f),
		vec2(-1.0f, 1.0f),
		vec2(-1.0f, 1.0f),
		vec2(1.0f, -1.0f),
		vec2(1.0f, 1.0f)
	};

	/*
	std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };

	std::vector<vec2> texCoords =
	{
		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
		vec2(0.0f, 1.0f)
	};
	*/

	mGeometry = new Geometry();
	mGeometry->setVertices(vertices);
	//mGeometry->setIndices(indices);
	//mGeometry->setTexCoords(texCoords);
	mGeometry->prepare(*mShader);
}

//=========================================================================
Hud::~Hud()
{
	delete mShader;
	delete mTexture;
	delete mGeometry;
}

//=========================================================================
inline void Hud::draw()
{
	mShader->bind();
	mTexture->bind();
	mShader->setUniform(ShaderConstants::ColorMap, 0);
	mShader->setUniform("translate", mTranslate);
	mShader->setUniform("scale", mScale);
	mGeometry->draw();
	mTexture->unbind();
	mShader->unbind();
}

/*
//=========================================================================
inline void Hud::draw(unsigned int windowWidth, unsigned int windowHeight, Position position, float offsetX, float offsetY)
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
			mPosition = vec2(windowWidth - mWidth, 0.0f);
			break;

		case Position::BOTTOM:
			mPosition = vec2(windowWidth - mWidth, windowHeight * 2 - mHeight * 2);
			break;

		case Position::LEFT:
			mPosition = vec2(0.0f, windowHeight - mHeight);
			break;

		case Position::RIGHT:
			mPosition = vec2(1, 0);
			break;

		case Position::TOPLEFT:
			mPosition = vec2(-1, 1);
			break;

		case Position::TOPRIGHT:
			mPosition = vec2(1, 1);
			break;

		case Position::BOTTOMLEFT:
			mPosition = vec2(-1, -1);
			break;

		case Position::BOTTOMRIGHT:
			mPosition = vec2(1, -1);
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
*/

//=========================================================================
void Hud::setViewportSize(unsigned int viewportWidth, unsigned int viewportHeight)
{
	mViewportSize = vec2(viewportWidth, viewportHeight);
	mScale = vec2(mTextureSize.x / (float)viewportWidth, mTextureSize.y / (float)viewportHeight);
	setPosition(mPosition);
}

//=========================================================================
void Hud::setPosition(Position position, float offsetX, float offsetY)
{
	mPosition = position;


	switch (position)
	{
	case Hud::TOP:
		mTranslate.x = 0;
		//mTranslate.y = (mViewportSize.y - mTextureSize.y / 2.0f) * mScale.y;
		break;
	case Hud::BOTTOM:
		break;
	case Hud::LEFT:
		break;
	case Hud::RIGHT:
		break;
	case Hud::TOPLEFT:
		break;
	case Hud::TOPRIGHT:
		break;
	case Hud::BOTTOMLEFT:
		break;
	case Hud::BOTTOMRIGHT:
		break;
	case Hud::CENTER:
		mTranslate.x = 0;
		mTranslate.y = 0;
		break;
	default:
		break;
	}
}
