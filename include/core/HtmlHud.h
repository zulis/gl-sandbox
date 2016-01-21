#pragma once

#include <string>
#include <WebCore.h>
#include <BitmapSurface.h>
#include <STLHelpers.h>
#include "core/Shader.h"
#include "core/Texture.h"
#include "core/Geometry.h"

class HtmlHud
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

	HtmlHud(const std::string& source, unsigned int viewportWidth, unsigned int viewportHeight);
	~HtmlHud();

	void draw();
	void onInput(const Input& input);
	void setViewportSize(unsigned int viewportWidth, unsigned int viewportHeight);
	void setPosition(Position position, float offsetX = 0.0f, float offsetY = 0.0f);
	void setContentSize(unsigned int width, unsigned int height);

private:
	vec2 mTextureSize;
	vec2 mViewportSize;
	vec2 mScale{ vec2(1) };
	vec2 mTranslate{ vec2(0) };
	Position mPosition{ Position::CENTER };
	float mOffsetX{ 0 };
	float mOffsetY{ 0 };
	Shader* mShader;
	//Texture* mTexture;
	Geometry* mGeometry;
	Awesomium::WebView* mWebView;
};

//=========================================================================
HtmlHud::HtmlHud(const std::string& source, unsigned int viewportWidth, unsigned int viewportHeight)
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

	//mTexture = new Texture(fileName);
	//mTextureSize = mTexture->getSize();
	//mScale = vec2(mTextureSize.x / mViewportSize.x, mTextureSize.y / mViewportSize.y);
	mTextureSize = vec2(viewportWidth, viewportHeight);

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

	using namespace Awesomium;

	if (!WebCore::instance())
		WebCore::Initialize(WebConfig());

	mWebView = WebCore::instance()->CreateWebView(viewportWidth, viewportHeight);
	mWebView->LoadURL(WebURL(WSLit(source.c_str())));
}

//=========================================================================
HtmlHud::~HtmlHud()
{
	delete mShader;
	//delete mTexture;
	delete mGeometry;
	mWebView->Destroy();

	if (Awesomium::WebCore::instance())
		Awesomium:: WebCore::Shutdown();
}

//=========================================================================
inline void HtmlHud::draw()
{
	using namespace Awesomium;

	WebCore::instance()->Update();

	if (!mWebView->IsLoading())
	{
		BitmapSurface* surface = (BitmapSurface*)mWebView->surface();

		int w = surface->width();
		int h = surface->height();
		unsigned char *buffer = new unsigned char[w * h * 4];
		surface->CopyTo(buffer, w * 4, 4, false, false);
		Texture* texture = new Texture(w, h, buffer);

		mat4 model = glm::translate(mat4(1.0f), vec3(mTranslate, 0.0f));

		mShader->bind();
		texture->bind();
		mShader->setUniform(ShaderConstants::ColorMap, 0);
		mShader->setUniform("model", model);
		mShader->setUniform("scale", mScale);
		mGeometry->draw();
		texture->unbind();
		mShader->unbind();

		delete[] buffer;
		delete texture;
	}
}

/*
public void TypeKey(VirtualKey vk, string chr)
{
WebKeyboardEvent keyEvent = new WebKeyboardEvent();

keyEvent.Type = WebKeyboardEventType.KeyDown;
keyEvent.VirtualKeyCode = vk;
webView.InjectKeyboardEvent(keyEvent);

keyEvent.Type = WebKeyboardEventType.Char;
keyEvent.Text = chr;
webView.InjectKeyboardEvent(keyEvent);

keyEvent.Type = WebKeyboardEventType.KeyUp;
keyEvent.VirtualKeyCode = vk;
webView.InjectKeyboardEvent(keyEvent);
}

TypeKey(VirtualKey.A, "A"); // uppercase A
TypeKey(VirtualKey.B, "b"); // lowercase b
TypeKey(VirtualKey.DOWN, ""); // down arrow
TypeKey(VirtualKey.RETURN, "\r\n"); // Enter
*/

//=========================================================================
void HtmlHud::onInput(const Input& input)
{
	mWebView->InjectMouseMove(input.getMouseX(), input.getMouseY());

	if (input.isMouseDown(MouseButton::Left))
		mWebView->InjectMouseDown(Awesomium::MouseButton::kMouseButton_Left);
	else
		mWebView->InjectMouseUp(Awesomium::MouseButton::kMouseButton_Left);

	mWebView->InjectMouseWheel(input.getMouseScroolY(), input.getMouseScroolX());

	//mWebView->InjectKeyboardEvent()
}

//=========================================================================
void HtmlHud::setViewportSize(unsigned int viewportWidth, unsigned int viewportHeight)
{
	mViewportSize = vec2(viewportWidth, viewportHeight);
	mScale = vec2(mTextureSize.x / (float)viewportWidth, mTextureSize.y / (float)viewportHeight);
	setPosition(mPosition, mOffsetX, mOffsetY);
}

//=========================================================================
void HtmlHud::setPosition(Position position, float offsetX, float offsetY)
{
	mPosition = position;
	mOffsetX = offsetX;
	mOffsetY = offsetY;
	mTranslate = vec2(0.0f);
	float x = (mViewportSize.x - mTextureSize.x) / mViewportSize.x;
	float y = (mViewportSize.y - mTextureSize.y) / mViewportSize.y;

	switch (position)
	{
	case HtmlHud::TOP:
		mTranslate.y = y;
		break;
	case HtmlHud::BOTTOM:
		mTranslate.y = -y;
		break;
	case HtmlHud::LEFT:
		mTranslate.x = -x;
		break;
	case HtmlHud::RIGHT:
		mTranslate.x = x;
		break;
	case HtmlHud::TOPLEFT:
		mTranslate.x = -x;
		mTranslate.y = y;
		break;
	case HtmlHud::TOPRIGHT:
		mTranslate.x = x;
		mTranslate.y = y;
		break;
	case HtmlHud::BOTTOMLEFT:
		mTranslate.x = -x;
		mTranslate.y = -y;
		break;
	case HtmlHud::BOTTOMRIGHT:
		mTranslate.x = x;
		mTranslate.y = -y;
		break;
	case HtmlHud::CENTER:
		break;
	default:
		break;
	}

	mTranslate.x = mTranslate.x + offsetX / mViewportSize.x;
	mTranslate.y = mTranslate.y - offsetY / mViewportSize.y;
}

//=========================================================================
void HtmlHud::setContentSize(unsigned int width, unsigned int height)
{
	mWebView->Resize(width, height);
	mTextureSize = vec2(width, height);
	setViewportSize(mViewportSize.x, mViewportSize.y);
}
