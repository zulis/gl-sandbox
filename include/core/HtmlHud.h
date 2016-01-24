#pragma once

#include <string>
#include <vector>
#include <functional>
#include <WebCore.h>
#include <WebSession.h>
#include <WebPreferences.h>
#include <BitmapSurface.h>
#include <STLHelpers.h>
#include <method_dispatcher.h>
#include <js_delegate.h>
#include "core/Shader.h"
#include "core/Texture.h"
#include "core/Geometry.h"

//using namespace Awesomium;

typedef int HtmlViewID;

#define HTMLVIEW_NONE (-1)

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

	HtmlHud(unsigned int viewportWidth, unsigned int viewportHeight);
	~HtmlHud();

	HtmlViewID addFromWeb(const std::string& source, unsigned int width = 0, unsigned int height = 0, bool transparent = true);
	HtmlViewID addFromFile(const std::string& fileName, unsigned int width = 0, unsigned int height = 0, bool transparent = true);

	void draw();
	void draw(const HtmlViewID htmlViewID);
	void onInput(const Input& input);
	void onResize(const unsigned int viewportWidth, const unsigned int viewportHeight);
	void setPosition(const HtmlViewID htmlViewID, Position position, int offsetX = 0, int offsetY = 0);
	//void setContentSize(const HtmlViewID htmlViewID, unsigned int width, unsigned int height);
	void setVisible(const HtmlViewID htmlViewID, bool visible);
	bool isVisible(const HtmlViewID htmlViewID);

	void addCallback(const HtmlViewID htmlViewID, JSDelegate delegate, const std::string& fuction);

private:
	struct WebViewContainer
	{
		Awesomium::WebView* webView;
		Position position{ Position::CENTER };
		ivec2 contentSize;
		vec2 translate;
		vec2 scale{ vec2(1) };
		bool visible{ true };
	};
	std::vector<WebViewContainer> mWebViewArray;

	Awesomium::WebSession* mWebSession;
	MethodDispatcher methodDispatcher;
	ivec2 mViewportSize;
	Shader* mShader;
	Geometry* mGeometry;
};

//=========================================================================
HtmlHud::HtmlHud(unsigned int viewportWidth, unsigned int viewportHeight)
{
	mViewportSize = ivec2(viewportWidth, viewportHeight);

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

	using namespace Awesomium;

	if (!WebCore::instance())
	{
		WebCore::Initialize(WebConfig());

		WebPreferences preferences;
		preferences.user_stylesheet = WSLit("body { overflow:hidden; }");
		mWebSession = WebCore::instance()->CreateWebSession(WSLit(""), preferences);
	}
}

//=========================================================================
HtmlHud::~HtmlHud()
{
	delete mShader;
	delete mGeometry;

	for (auto & item : mWebViewArray)
	{
		item.webView->Stop();
		item.webView->Destroy();
	}

	mWebViewArray.clear();

	if (Awesomium::WebCore::instance())
	{
		mWebSession->Release();
		//Awesomium::WebCore::instance()->Update();
		Awesomium::WebCore::Shutdown();
	}
}

//=========================================================================
inline HtmlViewID HtmlHud::addFromWeb(const std::string& source, unsigned int width, unsigned int height, bool transparent)
{
	if (width == 0 || height == 0)
	{
		width = mViewportSize.x;
		height = mViewportSize.y;
	}

	using namespace Awesomium;
	WebView* webView = WebCore::instance()->CreateWebView(width, height, mWebSession);
	webView->SetTransparent(transparent);
	webView->LoadURL(WebURL(WSLit(source.c_str())));

	mWebViewArray.push_back(WebViewContainer());
	size_t id = mWebViewArray.size() - 1;
	mWebViewArray[id].webView = webView;
	mWebViewArray[id].contentSize = ivec2(width, height);
	mWebViewArray[id].scale = vec2((float)width / mViewportSize.x, (float)height / mViewportSize.y);

	return id;
}

//=========================================================================
inline HtmlViewID HtmlHud::addFromFile(const std::string& fileName, unsigned int width, unsigned int height, bool transparent)
{
	return addFromWeb("file:///" + fileName, width, height, transparent);
}

//=========================================================================
inline void HtmlHud::draw()
{
	for (unsigned int id = 0; id < mWebViewArray.size(); id++)
		draw(id);
}

//=========================================================================
inline void HtmlHud::draw(const HtmlViewID htmlViewID)
{
	using namespace Awesomium;

	assert(htmlViewID != HTMLVIEW_NONE);

	if (static_cast<size_t>(htmlViewID) < mWebViewArray.size())
	{
		WebCore::instance()->Update();
		WebViewContainer item = mWebViewArray[htmlViewID];

		if (item.visible && !item.webView->IsLoading())
		{
			BitmapSurface* surface = (BitmapSurface*)item.webView->surface();

			if (surface != nullptr)
			{
				int surfaceWidth = surface->width();
				int surfaceHeight = surface->height();
				unsigned char *buffer = new unsigned char[surfaceWidth * surfaceHeight * 4];
				surface->CopyTo(buffer, surfaceWidth * 4, 4, false, false);
				Texture* texture = new Texture(surfaceWidth, surfaceHeight, buffer);

				mat4 model = glm::translate(mat4(1.0f), vec3(item.translate, 0.0f));

				mShader->bind();
				texture->bind();
				mShader->setUniform(ShaderConstants::ColorMap, 0);
				mShader->setUniform("model", model);
				mShader->setUniform("scale", item.scale);
				mGeometry->draw();
				texture->unbind();
				mShader->unbind();

				delete[] buffer;
				delete texture;
			}
		}
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
	for (auto & item : mWebViewArray)
	{
		if (item.visible)
		{
			item.webView->InjectMouseMove(input.getMouseX(), input.getMouseY());

			if (input.isMouseDown(MouseButton::Left))
				item.webView->InjectMouseDown(Awesomium::MouseButton::kMouseButton_Left);
			else
				item.webView->InjectMouseUp(Awesomium::MouseButton::kMouseButton_Left);

			item.webView->InjectMouseWheel((int)input.getMouseScroolY(), (int)input.getMouseScroolX());
		}
	}
}

//=========================================================================
void HtmlHud::onResize(const unsigned int viewportWidth, const unsigned int viewportHeight)
{
	//mViewportSize = vec2(viewportWidth, viewportHeight);

	//for (auto & item : mWebViewArray)
	//{
	//	item.scale = vec2(item.contentSize.x / mViewportSize.x, item.contentSize.y / mViewportSize.y);
	//	//setPosition(mPosition, mOffsetX, mOffsetY);
	//}
	////mScale = vec2(mTextureSize.x / viewportWidth, mTextureSize.y / viewportHeight);
	////setPosition(mPosition, mOffsetX, mOffsetY);
}

//=========================================================================
void HtmlHud::setPosition(const HtmlViewID htmlViewID, Position position, int offsetX, int offsetY)
{
	assert(htmlViewID != HTMLVIEW_NONE);

	if (static_cast<size_t>(htmlViewID) < mWebViewArray.size())
	{
		ivec2 contentSize = mWebViewArray[htmlViewID].contentSize;

		//mPosition = position;
		float x = (mViewportSize.x - contentSize.x) / (float)mViewportSize.x;
		float y = (mViewportSize.y - contentSize.y) / (float)mViewportSize.y;
		vec2 translate;

		switch (position)
		{
		case HtmlHud::TOP:
			translate.y = y;
			break;
		case HtmlHud::BOTTOM:
			translate.y = -y;
			break;
		case HtmlHud::LEFT:
			translate.x = -x;
			break;
		case HtmlHud::RIGHT:
			translate.x = x;
			break;
		case HtmlHud::TOPLEFT:
			translate.x = -x;
			translate.y = y;
			break;
		case HtmlHud::TOPRIGHT:
			translate.x = x;
			translate.y = y;
			break;
		case HtmlHud::BOTTOMLEFT:
			translate.x = -x;
			translate.y = -y;
			break;
		case HtmlHud::BOTTOMRIGHT:
			translate.x = x;
			translate.y = -y;
			break;
		case HtmlHud::CENTER:
			break;
		default:
			break;
		}

		translate.x = translate.x + (float)offsetX / (float)mViewportSize.x;
		translate.y = translate.y - (float)offsetY / (float)mViewportSize.y;

		mWebViewArray[htmlViewID].translate = translate;
	}
}

//=========================================================================
//void HtmlHud::setContentSize(const HtmlViewID htmlViewID, unsigned int width, unsigned int height)
//{
//	assert(htmlViewID != HTMLVIEW_NONE);
//
//	if (static_cast<size_t>(htmlViewID) < mWebViewArray.size())
//	{
//		mWebViewArray[htmlViewID].webView->Resize(width, height);
//		mWebViewArray[htmlViewID].contentSize = vec2(width, height);
//	}
//
//	onResize(mViewportSize.x, mViewportSize.y);
//}

//=========================================================================
void HtmlHud::setVisible(const HtmlViewID htmlViewID, bool visible)
{
	assert(htmlViewID != HTMLVIEW_NONE);

	if (static_cast<size_t>(htmlViewID) < mWebViewArray.size())
		mWebViewArray[htmlViewID].visible = visible;
}

//=========================================================================
inline bool HtmlHud::isVisible(const HtmlViewID htmlViewID)
{
	assert(htmlViewID != HTMLVIEW_NONE);

	if (static_cast<size_t>(htmlViewID) < mWebViewArray.size())
		return mWebViewArray[htmlViewID].visible;
	else
		return nullptr;
}

//=========================================================================
inline void HtmlHud::addCallback(const HtmlViewID htmlViewID, JSDelegate delegate, const std::string& fuction)
{
	using namespace Awesomium;

	assert(htmlViewID != HTMLVIEW_NONE);

	if (static_cast<size_t>(htmlViewID) < mWebViewArray.size())
	{
		//mWebViewArray[htmlViewID].callbackArray.push_back(callback);

		JSValue result = mWebViewArray[htmlViewID].webView->CreateGlobalJavascriptObject(WSLit("app"));

		if (result.IsObject())
		{
			// Bind our custom method to it.
			JSObject& appObject = result.ToObject();
			methodDispatcher.Bind(appObject, WSLit(fuction.c_str()), delegate);
		}

		// Bind our method dispatcher to the WebView
		mWebViewArray[htmlViewID].webView->set_js_method_handler(&methodDispatcher);
	}
}
