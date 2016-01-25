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

using namespace Awesomium;

typedef int HtmlViewID;

#define HTMLVIEW_NONE (-1)
#define mapKey(a, b) case KEY_##a: return KeyCodes::AK_##b;

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

	HtmlHud();
	~HtmlHud();

	void setup(unsigned int viewportWidth, unsigned int viewportHeight);

	HtmlViewID addFromWeb(const std::string& source, bool transparent = true);
	HtmlViewID addFromFile(const std::string& fileName, bool transparent = true);

	void draw();
	void draw(const HtmlViewID htmlViewID);
	void onInput(const Input& input);
	void onResize(const unsigned int viewportWidth, const unsigned int viewportHeight);
	void setPosition(const HtmlViewID htmlViewID, Position position, int offsetX = 0, int offsetY = 0);
	//void setContentSize(const HtmlViewID htmlViewID, unsigned int width, unsigned int height);
	void setVisible(const HtmlViewID htmlViewID, bool visible);
	bool isVisible(const HtmlViewID htmlViewID);
	void setFocus(const HtmlViewID htmlViewID, bool focus);

	void addCallback(const HtmlViewID htmlViewID, JSDelegate delegate, const std::string& fuction);

private:
	struct WebViewContainer
	{
		WebView* webView;
		Position position{ Position::CENTER };
		ivec2 contentSize;
		vec2 translate;
		vec2 scale{ vec2(1) };
		bool visible{ true };
	};
	std::vector<WebViewContainer> mWebViewArray;

	WebSession* mWebSession;
	MethodDispatcher methodDispatcher;
	ivec2 mViewportSize;
	Shader* mShader;
	Geometry* mGeometry;

	HtmlViewID mActiveHtmlView{ HTMLVIEW_NONE };

	int getWebKeyFromInputKey(int key);
};

//=========================================================================
HtmlHud::HtmlHud()
{
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

	if (WebCore::instance())
	{
		mWebSession->Release();
		//WebCore::instance()->Update();
		WebCore::Shutdown();
	}
}

//=========================================================================
void HtmlHud::setup(unsigned int viewportWidth, unsigned int viewportHeight)
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

	if (!WebCore::instance())
	{
		WebCore::Initialize(WebConfig());

		WebPreferences preferences;
		preferences.user_stylesheet = WSLit("body { overflow:hidden; }");
		mWebSession = WebCore::instance()->CreateWebSession(WSLit(""), preferences);
	}
}

//=========================================================================
inline HtmlViewID HtmlHud::addFromWeb(const std::string& source, bool transparent)
{
	WebView* webView = WebCore::instance()->CreateWebView(mViewportSize.x, mViewportSize.y, mWebSession);
	webView->SetTransparent(transparent);
	webView->LoadURL(WebURL(WSLit(source.c_str())));

	mWebViewArray.push_back(WebViewContainer());
	size_t id = mWebViewArray.size() - 1;
	mWebViewArray[id].webView = webView;
	mWebViewArray[id].contentSize = ivec2(mViewportSize.x, mViewportSize.y);
	mWebViewArray[id].scale = vec2((float)mViewportSize.x / mViewportSize.x, (float)mViewportSize.y / mViewportSize.y);

	return id;
}

//=========================================================================
inline HtmlViewID HtmlHud::addFromFile(const std::string& fileName, bool transparent)
{
	return addFromWeb("file:///" + fileName, transparent);
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
	assert(htmlViewID != HTMLVIEW_NONE);

	if (static_cast<size_t>(htmlViewID) < mWebViewArray.size())
	{
		WebCore::instance()->Update();
		WebViewContainer item = mWebViewArray[htmlViewID];

		if (item.visible && !item.webView->IsLoading())
		{
			BitmapSurface* surface = (BitmapSurface*)item.webView->surface();

			if (surface && surface->buffer() /*&& surface->is_dirty()*/)
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

//=========================================================================
void HtmlHud::onInput(const Input& input)
{
	for (auto & item : mWebViewArray)
	{
		if (item.visible)
		{
			item.webView->InjectMouseMove(input.getMouseX(), input.getMouseY());

			if (input.isMouseDown(Input::MouseButton::Left))
				item.webView->InjectMouseDown(MouseButton::kMouseButton_Left);
			else
				item.webView->InjectMouseUp(MouseButton::kMouseButton_Left);

			// 			if (input.isMouseDown(Input::MouseButton::Right))
			// 				item.webView->InjectMouseDown(MouseButton::kMouseButton_Right);
			// 			else
			// 				item.webView->InjectMouseUp(MouseButton::kMouseButton_Right);
			// 			
			// 			if (input.isMouseDown(Input::MouseButton::Middle))
			// 				item.webView->InjectMouseDown(MouseButton::kMouseButton_Middle);
			// 			else
			// 				item.webView->InjectMouseUp(MouseButton::kMouseButton_Middle);

			item.webView->InjectMouseWheel((int)input.getMouseScroolY(), (int)input.getMouseScroolX());

			int lastKey = input.getLastKey();

			if (lastKey != KEY_NONE)
			{
				item.webView->Focus();

				int key = getWebKeyFromInputKey(lastKey);

				//Awesomium::WebKeyboardEvent keyDown;
				//keyDown.type = Awesomium::WebKeyboardEvent::kTypeKeyDown;
				//keyDown.virtual_key_code = key;
				//keyDown.native_key_code = key;
				//keyDown.text[0] = key;
				//keyDown.unmodified_text[0] = key;
				//keyDown.modifiers = 0;
				//// keyDown.modifiers  ???
				//item.webView->InjectKeyboardEvent(keyDown);

				Awesomium::WebKeyboardEvent typeChar;
				typeChar.type = Awesomium::WebKeyboardEvent::kTypeChar;
				typeChar.virtual_key_code = key;
				typeChar.native_key_code = key;
				typeChar.text[0] = key;
				typeChar.unmodified_text[0] = key;
				typeChar.modifiers = 0;
				if (input.isKeyDown(KEY_LEFT_ALT) || input.isKeyDown(KEY_RIGHT_ALT))
					typeChar.modifiers |= Awesomium::WebKeyboardEvent::kModAltKey;
				if (input.isKeyDown(KEY_LEFT_CONTROL) || input.isKeyDown(KEY_RIGHT_CONTROL))
					typeChar.modifiers |= Awesomium::WebKeyboardEvent::kModControlKey;
				////if (event.key.keysym.mod & KMOD_LMETA || event.key.keysym.mod & KMOD_RMETA)
				//	//keyEvent.modifiers |= Awesomium::WebKeyboardEvent::kModMetaKey;
				if (input.isKeyDown(KEY_LEFT_SHIFT) || input.isKeyDown(KEY_RIGHT_SHIFT))
					typeChar.modifiers |= Awesomium::WebKeyboardEvent::kModShiftKey;
				////if (event.key.keysym.mod & KMOD_NUM)
				//	//keyEvent.modifiers |= Awesomium::WebKeyboardEvent::kModIsKeypad;
				item.webView->InjectKeyboardEvent(typeChar);

				/*else if (!lastKey.isDown)
				{
					Awesomium::WebKeyboardEvent evt;
					evt.type = Awesomium::WebKeyboardEvent::kTypeKeyUp;
					evt.virtual_key_code = (char)lastKey.key;
					evt.native_key_code = (char)lastKey.key;
					evt.text[0] = (char)lastKey.key;
					evt.unmodified_text[0] = (char)lastKey.key;
					evt.modifiers = 0;
					item.webView->InjectKeyboardEvent(evt);
				}*/

				//WebKeyboardEvent keyEvent;

				//if (input.isKeyDown(lastKey))
				//	keyEvent.type = WebKeyboardEvent::kTypeKeyDown;
				//else if (input.isKeyUp(lastKey))
				//	keyEvent.type = WebKeyboardEvent::kTypeKeyUp;

				//char* buf = new char[20];
				//keyEvent.virtual_key_code = getWebKeyFromInputKey(lastKey);
				//Awesomium::GetKeyIdentifierFromVirtualKeyCode(keyEvent.virtual_key_code, &buf);
				//strcpy(keyEvent.key_identifier, buf);
				//delete[] buf;

				//keyEvent.modifiers = 0;

				//if (input.isKeyDown(KEY_LEFT_ALT) || input.isKeyDown(KEY_RIGHT_ALT))
				//	keyEvent.modifiers |= Awesomium::WebKeyboardEvent::kModAltKey;
				//if (input.isKeyDown(KEY_LEFT_CONTROL) || input.isKeyDown(KEY_RIGHT_CONTROL))
				//	keyEvent.modifiers |= Awesomium::WebKeyboardEvent::kModControlKey;
				////if (event.key.keysym.mod & KMOD_LMETA || event.key.keysym.mod & KMOD_RMETA)
				//	//keyEvent.modifiers |= Awesomium::WebKeyboardEvent::kModMetaKey;
				//if (input.isKeyDown(KEY_LEFT_SHIFT) || input.isKeyDown(KEY_RIGHT_SHIFT))
				//	keyEvent.modifiers |= Awesomium::WebKeyboardEvent::kModShiftKey;
				////if (event.key.keysym.mod & KMOD_NUM)
				//	//keyEvent.modifiers |= Awesomium::WebKeyboardEvent::kModIsKeypad;

				////item.webView->InjectKeyboardEvent(keyEvent);


				//keyEvent.type = Awesomium::WebKeyboardEvent::kTypeChar;
				//keyEvent.virtual_key_code = lastKey;
				//keyEvent.native_key_code = lastKey;
				//item.webView->InjectKeyboardEvent(keyEvent);


			}
		}
	}
}

//=========================================================================
int HtmlHud::getWebKeyFromInputKey(int key)
{
	switch (key) {
		mapKey(BACKSPACE, BACK)
			mapKey(TAB, TAB)
			//mapKey(CLEAR, CLEAR)
			mapKey(ENTER, RETURN)
			mapKey(PAUSE, PAUSE)
			mapKey(ESCAPE, ESCAPE)
			mapKey(SPACE, SPACE)
			//mapKey(EXCLAIM, 1)
			//mapKey(QUOTEDBL, 2)
			//mapKey(HASH, 3)
			//mapKey(DOLLAR, 4)
			//mapKey(AMPERSAND, 7)
			//mapKey(QUOTE, OEM_7)
			//mapKey(LEFTPAREN, 9)
			//mapKey(RIGHTPAREN, 0)
			//mapKey(ASTERISK, 8)
			//mapKey(PLUS, OEM_PLUS)
			mapKey(COMMA, OEM_COMMA)
			mapKey(MINUS, OEM_MINUS)
			mapKey(PERIOD, OEM_PERIOD)
			mapKey(SLASH, OEM_2)
			mapKey(0, 0)
			mapKey(1, 1)
			mapKey(2, 2)
			mapKey(3, 3)
			mapKey(4, 4)
			mapKey(5, 5)
			mapKey(6, 6)
			mapKey(7, 7)
			mapKey(8, 8)
			mapKey(9, 9)
			//mapKey(COLON, OEM_1)
			mapKey(SEMICOLON, OEM_1)
			//mapKey(LESS, OEM_COMMA)
			//mapKey(EQUALS, OEM_PLUS)
			//mapKey(GREATER, OEM_PERIOD)
			//mapKey(QUESTION, OEM_2)
			//mapKey(AT, 2)
			//mapKey(LEFTBRACKET, OEM_4)
			mapKey(BACKSLASH, OEM_5)
			//mapKey(RIGHTBRACKET, OEM_6)
			//mapKey(CARET, 6)
			//mapKey(UNDERSCORE, OEM_MINUS)
			//mapKey(BACKQUOTE, OEM_3)
			mapKey(A, A)
			mapKey(B, B)
			mapKey(C, C)
			mapKey(D, D)
			mapKey(E, E)
			mapKey(F, F)
			mapKey(G, G)
			mapKey(H, H)
			mapKey(I, I)
			mapKey(J, J)
			mapKey(K, K)
			mapKey(L, L)
			mapKey(M, M)
			mapKey(N, N)
			mapKey(O, O)
			mapKey(P, P)
			mapKey(Q, Q)
			mapKey(R, R)
			mapKey(S, S)
			mapKey(T, T)
			mapKey(U, U)
			mapKey(V, V)
			mapKey(W, W)
			mapKey(X, X)
			mapKey(Y, Y)
			mapKey(Z, Z)
			mapKey(DELETE, DELETE)
			mapKey(KP_0, NUMPAD0)
			mapKey(KP_1, NUMPAD1)
			mapKey(KP_2, NUMPAD2)
			mapKey(KP_3, NUMPAD3)
			mapKey(KP_4, NUMPAD4)
			mapKey(KP_5, NUMPAD5)
			mapKey(KP_6, NUMPAD6)
			mapKey(KP_7, NUMPAD7)
			mapKey(KP_8, NUMPAD8)
			mapKey(KP_9, NUMPAD9)
			mapKey(KP_DECIMAL, DECIMAL)
			mapKey(KP_DIVIDE, DIVIDE)
			mapKey(KP_MULTIPLY, MULTIPLY)
			mapKey(KP_SUBTRACT, SUBTRACT)
			mapKey(KP_ADD, ADD)
			mapKey(KP_ENTER, RETURN)
			//mapKey(KP_EQUALS, UNKNOWN)
			mapKey(UP, UP)
			mapKey(DOWN, DOWN)
			mapKey(RIGHT, RIGHT)
			mapKey(LEFT, LEFT)
			mapKey(INSERT, INSERT)
			mapKey(HOME, HOME)
			mapKey(END, END)
			//mapKey(PAGE_UP, PRIOR)
			//mapKey(PAGE_DOWN, NEXT)
			mapKey(F1, F1)
			mapKey(F2, F2)
			mapKey(F3, F3)
			mapKey(F4, F4)
			mapKey(F5, F5)
			mapKey(F6, F6)
			mapKey(F7, F7)
			mapKey(F8, F8)
			mapKey(F9, F9)
			mapKey(F10, F10)
			mapKey(F11, F11)
			mapKey(F12, F12)
			mapKey(F13, F13)
			mapKey(F14, F14)
			mapKey(F15, F15)
			//mapKey(NUMLOCK, NUMLOCK)
			//mapKey(CAPSLOCK, CAPITAL)
			//mapKey(SCROLLOCK, SCROLL)
			//mapKey(RSHIFT, RSHIFT)
			//mapKey(LSHIFT, LSHIFT)
			//mapKey(RCTRL, RCONTROL)
			//mapKey(LCTRL, LCONTROL)
			//mapKey(RALT, RMENU)
			//mapKey(LALT, LMENU)
			//mapKey(RMETA, LWIN)
			//mapKey(LMETA, RWIN)
			//mapKey(LSUPER, LWIN)
			//mapKey(RSUPER, RWIN)
			//mapKey(MODE, MODECHANGE)
			//mapKey(COMPOSE, ACCEPT)
			//mapKey(HELP, HELP)
			//mapKey(PRINT, SNAPSHOT)
			//mapKey(SYSREQ, EXECUTE)
	default:
		return KeyCodes::AK_UNKNOWN;
	}
}

//=========================================================================
void HtmlHud::onResize(const unsigned int viewportWidth, const unsigned int viewportHeight)
{
	//mViewportSize = vec2(viewportWidth, viewportHeight);

	for (auto & item : mWebViewArray)
	{
		item.scale = vec2(item.contentSize.x / mViewportSize.x, item.contentSize.y / mViewportSize.y);
	//	//setPosition(mPosition, mOffsetX, mOffsetY);
		item.webView->Resize(viewportWidth, viewportHeight);
	}
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
void HtmlHud::setFocus(const HtmlViewID htmlViewID, bool focus)
{
	assert(htmlViewID != HTMLVIEW_NONE);

	if (static_cast<size_t>(htmlViewID) < mWebViewArray.size())
	{
		if (focus)
			mWebViewArray[htmlViewID].webView->Focus();
		else
			mWebViewArray[htmlViewID].webView->Unfocus();
	}
}

//=========================================================================
inline void HtmlHud::addCallback(const HtmlViewID htmlViewID, JSDelegate delegate, const std::string& fuction)
{
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
