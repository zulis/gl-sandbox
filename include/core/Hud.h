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

	Hud();
	~Hud();

	void setup(unsigned int viewportWidth, unsigned int viewportHeight);
	void loadFromFile(const std::string & fileName);

	void draw();
	void onResize(unsigned int viewportWidth, unsigned int viewportHeight);
	void setPosition(Position position, float offsetX = 0.0f, float offsetY = 0.0f);

private:
	vec2 mTextureSize;
	vec2 mViewportSize;
	vec2 mScale{ vec2(1) };
	vec2 mTranslate{ vec2(0) };
	Position mPosition{ Position::CENTER };
	float mOffsetX{ 0 };
	float mOffsetY{ 0 };
	Shader* mShader;
	Texture* mTexture;
	Geometry* mGeometry;
};

//=========================================================================
Hud::Hud()
{
}

//=========================================================================
Hud::~Hud()
{
	delete mShader;
	delete mTexture;
	delete mGeometry;
}

//=========================================================================
void Hud::setup(unsigned int viewportWidth, unsigned int viewportHeight)
{
	mViewportSize = vec2(viewportWidth, viewportHeight);

	mShader = new Shader(R"(
		[Vertex]
		#version 430
		
		in vec3 VertexPosition;
		in vec2 VertexTexCoord;
		out vec2 TexCoord;
		
		uniform mat4 model;
		uniform vec2 scale;
		
		void main()
		{
			gl_Position = model * vec4(vec2(VertexPosition) * scale, 0.0, 1.0);
			TexCoord = VertexTexCoord;
		}
		
		[Fragment]
		#version 430
		
		in vec2 TexCoord;
		out vec4 FragColor;
		
		layout(binding = 0) uniform sampler2D ColorMap;
		
		void main()
		{
			FragColor = texture2D(ColorMap, TexCoord);
		}
	)", Shader::SourceType::String);

	std::vector<vec2> vertices =
	{
		vec2(-1.0f, -1.0f),
		vec2(1.0f, -1.0f),
		vec2(-1.0f, 1.0f),
		vec2(1.0f, 1.0f)
	};

	std::vector<unsigned int> indices = { 0, 1, 2, 1, 3, 2 };

	std::vector<vec2> texCoords =
	{

		vec2(0.0f, 1.0f),
		vec2(1.0f, 1.0f),
		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f)
	};

	mGeometry = new Geometry();
	mGeometry->setVertices(vertices);
	mGeometry->setIndices(indices);
	mGeometry->setTexCoords(texCoords);
	mGeometry->prepare(*mShader);
}

//=========================================================================
void Hud::loadFromFile(const std::string & fileName)
{
	mTexture = new Texture(fileName);
	mTextureSize = mTexture->getSize();
	mScale = vec2(mTextureSize.x / mViewportSize.x, mTextureSize.y / mViewportSize.y);
}

//=========================================================================
inline void Hud::draw()
{
	mat4 model = glm::translate(mat4(1.0f), vec3(mTranslate, 0.0f));

	mShader->bind();
	mTexture->bind();
	mShader->setUniform(ShaderConstants::ColorMap, 0);
	mShader->setUniform("model", model);
	mShader->setUniform("scale", mScale);
	mGeometry->draw();
	mTexture->unbind();
	mShader->unbind();
}

//=========================================================================
void Hud::onResize(unsigned int viewportWidth, unsigned int viewportHeight)
{
	mViewportSize = vec2(viewportWidth, viewportHeight);
	mScale = vec2(mTextureSize.x / (float)viewportWidth, mTextureSize.y / (float)viewportHeight);
	setPosition(mPosition, mOffsetX, mOffsetY);
}

//=========================================================================
void Hud::setPosition(Position position, float offsetX, float offsetY)
{
	mPosition = position;
	mOffsetX = offsetX;
	mOffsetY = offsetY;
	mTranslate = vec2(0.0f);
	float x = (mViewportSize.x - mTextureSize.x) / mViewportSize.x;
	float y = (mViewportSize.y - mTextureSize.y) / mViewportSize.y;

	switch (position)
	{
	case Hud::TOP:
		mTranslate.y = y;
		break;
	case Hud::BOTTOM:
		mTranslate.y = -y;
		break;
	case Hud::LEFT:
		mTranslate.x = -x;
		break;
	case Hud::RIGHT:
		mTranslate.x = x;
		break;
	case Hud::TOPLEFT:
		mTranslate.x = -x;
		mTranslate.y = y;
		break;
	case Hud::TOPRIGHT:
		mTranslate.x = x;
		mTranslate.y = y;
		break;
	case Hud::BOTTOMLEFT:
		mTranslate.x = -x;
		mTranslate.y = -y;
		break;
	case Hud::BOTTOMRIGHT:
		mTranslate.x = x;
		mTranslate.y = -y;
		break;
	case Hud::CENTER:
		break;
	default:
		break;
	}

	mTranslate.x = mTranslate.x + offsetX / mViewportSize.x;
	mTranslate.y = mTranslate.y - offsetY / mViewportSize.y;
}
