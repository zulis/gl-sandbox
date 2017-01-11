#pragma once

#include <string>
#include <vector>
#include <functional>
#include <chrono>
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


class HtmlUi
{
public:
	HtmlUi();
	~HtmlUi();

	void setup(unsigned int viewportWidth, unsigned int viewportHeight);
	HtmlViewID addFromWeb(const std::string& source, bool transparent = true);
	HtmlViewID addFromFile(const std::string& fileName, bool transparent = true);
	void draw();
	void onInput(const Input& input);
	void onResize(const unsigned int viewportWidth, const unsigned int viewportHeight);
	void addCallback(const HtmlViewID htmlViewID, JSDelegate delegate, const std::string& function);
	void setActiveView(const HtmlViewID htmlViewID);
	bool isViewActive(const HtmlViewID htmlViewID);

private:
	struct WebViewContainer
	{
		WebView* webView;
		//ivec2 contentSize;
		//vec2 scale{ vec2(1) };
		//bool visible{ true };
	};
	std::vector<WebViewContainer> mWebViewArray;

	WebSession* mWebSession;
	MethodDispatcher methodDispatcher;
	ivec2 mViewportSize;
	Shader* mShader;
	Texture* texture{ nullptr };
	Geometry* mGeometry;
	HtmlViewID mActiveHtmlView{ HTMLVIEW_NONE };
	std::chrono::time_point<std::chrono::system_clock> mLastTime;

	int getWebKeyFromInputKey(int key);
};

//=========================================================================
HtmlUi::HtmlUi()
{
	mLastTime = std::chrono::system_clock::now();
}

//=========================================================================
HtmlUi::~HtmlUi()
{
	delete texture;
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
		WebCore::Shutdown();
	}
}

//=========================================================================
void HtmlUi::setup(unsigned int viewportWidth, unsigned int viewportHeight)
{
	mViewportSize = ivec2(viewportWidth, viewportHeight);

	mShader = new Shader(R"(
		[Vertex]
		#version 430
		
		in vec3 VertexPosition;
		in vec2 VertexTexCoord;
		out vec2 TexCoord;
		
		void main()
		{
			gl_Position = vec4(vec2(VertexPosition), 0.0, 1.0);
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
	//mGeometry->prepare(*mShader);

	if (!WebCore::instance())
	{
		WebCore::Initialize(WebConfig());

		WebPreferences preferences;
		preferences.user_stylesheet = WSLit("body { overflow:hidden; }");
		mWebSession = WebCore::instance()->CreateWebSession(WSLit(""), preferences);
	}
}

//=========================================================================
inline HtmlViewID HtmlUi::addFromWeb(const std::string& source, bool transparent)
{
	WebView* webView = WebCore::instance()->CreateWebView(mViewportSize.x, mViewportSize.y, mWebSession);
	webView->SetTransparent(transparent);
	webView->LoadURL(WebURL(WSLit(source.c_str())));
	webView->ReduceMemoryUsage();

	mWebViewArray.push_back(WebViewContainer());
	size_t id = mWebViewArray.size() - 1;
	mWebViewArray[id].webView = webView;

	return id;
}

//=========================================================================
inline HtmlViewID HtmlUi::addFromFile(const std::string& fileName, bool transparent)
{
	return addFromWeb("file:///" + fileName, transparent);
}

//=========================================================================
inline void HtmlUi::draw()
{
	if (mActiveHtmlView != HTMLVIEW_NONE && static_cast<size_t>(mActiveHtmlView) < mWebViewArray.size())
	{
		WebCore::instance()->Update();
		WebViewContainer item = mWebViewArray[mActiveHtmlView];

		if (!item.webView->IsLoading())
		{
			BitmapSurface* surface = (BitmapSurface*)item.webView->surface();

			if (surface && surface->buffer() && surface->is_dirty())
			{
				int surfaceWidth = surface->width();
				int surfaceHeight = surface->height();
				unsigned char *buffer = new unsigned char[surfaceWidth * surfaceHeight * 4];
				surface->CopyTo(buffer, surfaceWidth * 4, 4, false, false);

				if (!texture)
					texture = new Texture(surfaceWidth, surfaceHeight, buffer);
				else
					texture->changePixels(buffer);

				delete[] buffer;
			}
		}

		if (texture)
		{
			mShader->bind();
			texture->bind();
			mShader->setUniform(ShaderConstants::ColorMap, 0);
			mGeometry->draw();
			texture->unbind();
			mShader->unbind();
		}

	}
}

//=========================================================================
void HtmlUi::onInput(const Input& input)
{
	if(mActiveHtmlView != HTMLVIEW_NONE && static_cast<size_t>(mActiveHtmlView) < mWebViewArray.size())
	{
		WebViewContainer item = mWebViewArray[mActiveHtmlView];

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

		std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = currentTime - mLastTime;

		int lastKey = input.getLastKey();

		if (lastKey != KEY_NONE && elapsed_seconds.count() >= 0.2)
		{
			mLastTime = currentTime;

			item.webView->Focus();

			int key = getWebKeyFromInputKey(lastKey);

			Awesomium::WebKeyboardEvent keyEvent;

			char* buf = new char[20];
			keyEvent.virtual_key_code = key;
			Awesomium::GetKeyIdentifierFromVirtualKeyCode(keyEvent.virtual_key_code, &buf);
			strcpy(keyEvent.key_identifier, buf);
			delete[] buf;

			keyEvent.modifiers = 0;
			keyEvent.native_key_code = 0;
			keyEvent.type = (input.isKeyDown(lastKey) ? WebKeyboardEvent::kTypeKeyDown : WebKeyboardEvent::kTypeKeyUp);
			item.webView->InjectKeyboardEvent(keyEvent);


			WebKeyboardEvent e;
			//e.type = (input.isKeyDown(lastKey) ? WebKeyboardEvent::kTypeKeyDown : WebKeyboardEvent::kTypeKeyUp);
			e.type = WebKeyboardEvent::kTypeChar;
			e.virtual_key_code = key;
			e.native_key_code = key;
			e.text[0] = key;
			e.unmodified_text[0] = key;
			e.modifiers = 0;
			// 				if (input.isKeyDown(KEY_LEFT_ALT) || input.isKeyDown(KEY_RIGHT_ALT))
			// 					e.modifiers |= WebKeyboardEvent::kModAltKey;
			// 				if (input.isKeyDown(KEY_LEFT_CONTROL) || input.isKeyDown(KEY_RIGHT_CONTROL))
			// 					e.modifiers |= WebKeyboardEvent::kModControlKey;
							////if (event.key.keysym.mod & KMOD_LMETA || event.key.keysym.mod & KMOD_RMETA)
							//	//keyEvent.modifiers |= WebKeyboardEvent::kModMetaKey;
			// 				if (input.isKeyDown(KEY_LEFT_SHIFT) || input.isKeyDown(KEY_RIGHT_SHIFT))
			// 					e.modifiers |= WebKeyboardEvent::kModShiftKey;
							////if (event.key.keysym.mod & KMOD_NUM)
							//	//keyEvent.modifiers |= WebKeyboardEvent::kModIsKeypad;
			item.webView->InjectKeyboardEvent(e);

			// GLFW doesn't generate onChar events for non-printable characters, 
			// but that means that awesomium never gets some that it needs (to pass into text boxes etc)
			//if (GLFW_KEY_ENTER == key && GLFW_RELEASE == action) onChar(13, GLFW_PRESS);

			/*
			//WebKeyboardEvent keyDown;
			//keyDown.type = WebKeyboardEvent::kTypeKeyDown;
			//keyDown.virtual_key_code = key;
			//keyDown.native_key_code = key;
			//keyDown.text[0] = key;
			//keyDown.unmodified_text[0] = key;
			//keyDown.modifiers = 0;
			//// keyDown.modifiers  ???
			//item.webView->InjectKeyboardEvent(keyDown);

			WebKeyboardEvent typeChar;
			typeChar.type = WebKeyboardEvent::kTypeChar;
			typeChar.virtual_key_code = key;
			typeChar.native_key_code = key;
			typeChar.text[0] = key;
			typeChar.unmodified_text[0] = key;
			typeChar.modifiers = 0;
			if (input.isKeyDown(KEY_LEFT_ALT) || input.isKeyDown(KEY_RIGHT_ALT))
				typeChar.modifiers |= WebKeyboardEvent::kModAltKey;
			if (input.isKeyDown(KEY_LEFT_CONTROL) || input.isKeyDown(KEY_RIGHT_CONTROL))
				typeChar.modifiers |= WebKeyboardEvent::kModControlKey;
			////if (event.key.keysym.mod & KMOD_LMETA || event.key.keysym.mod & KMOD_RMETA)
			//	//keyEvent.modifiers |= WebKeyboardEvent::kModMetaKey;
			if (input.isKeyDown(KEY_LEFT_SHIFT) || input.isKeyDown(KEY_RIGHT_SHIFT))
				typeChar.modifiers |= WebKeyboardEvent::kModShiftKey;
			////if (event.key.keysym.mod & KMOD_NUM)
			//	//keyEvent.modifiers |= WebKeyboardEvent::kModIsKeypad;
			item.webView->InjectKeyboardEvent(typeChar);
			*/
		}
	}
}

//=========================================================================
int HtmlUi::getWebKeyFromInputKey(int key)
{
	if (key >= '0' && key <= 'Z')
		return key;

#define mapKey(a, b) case KEY_##a: return KeyCodes::AK_##b;

	switch (key)
	{
		mapKey(ESCAPE, ESCAPE);
		mapKey(F1, F1);
		mapKey(F2, F2);
		mapKey(F3, F3);
		mapKey(F4, F4);
		mapKey(F5, F5);
		mapKey(F6, F6);
		mapKey(F7, F7);
		mapKey(F8, F8);
		mapKey(F9, F9);
		mapKey(F10, F10);
		mapKey(F11, F11);
		mapKey(F12, F12);
		mapKey(F13, F13);
		mapKey(F14, F14);
		mapKey(F15, F15);
		mapKey(F16, F16);
		mapKey(F17, F17);
		mapKey(F18, F18);
		mapKey(F19, F19);
		mapKey(F20, F20);
		mapKey(F21, F21);
		mapKey(F22, F22);
		mapKey(F23, F23);
		mapKey(F24, F24);
		mapKey(UP, UP);
		mapKey(DOWN, DOWN);
		mapKey(LEFT, LEFT);
		mapKey(RIGHT, RIGHT);
		mapKey(LEFT_SHIFT, LSHIFT);
		mapKey(RIGHT_SHIFT, RSHIFT);
		mapKey(LEFT_CONTROL, LCONTROL);
		mapKey(RIGHT_CONTROL, RCONTROL);
		mapKey(LEFT_ALT, LMENU);    //?
		mapKey(RIGHT_ALT, RMENU);    //?
		mapKey(TAB, TAB);
		mapKey(ENTER, RETURN);
		mapKey(BACKSPACE, BACK);
		mapKey(INSERT, INSERT);
		mapKey(DELETE, DELETE);
		mapKey(PAGE_UP, PRIOR);
		mapKey(PAGE_DOWN, NEXT);
		mapKey(HOME, HOME);
		mapKey(END, END);
		mapKey(KP_0, NUMPAD0);
		mapKey(KP_1, NUMPAD1);
		mapKey(KP_2, NUMPAD2);
		mapKey(KP_3, NUMPAD3);
		mapKey(KP_4, NUMPAD4);
		mapKey(KP_5, NUMPAD5);
		mapKey(KP_6, NUMPAD6);
		mapKey(KP_7, NUMPAD7);
		mapKey(KP_8, NUMPAD8);
		mapKey(KP_9, NUMPAD9);
		mapKey(KP_DIVIDE, DIVIDE);
		mapKey(KP_MULTIPLY, MULTIPLY);
		mapKey(KP_SUBTRACT, SUBTRACT);
		mapKey(KP_ADD, ADD);
		mapKey(KP_DECIMAL, DECIMAL);
		//    mapKey(KP_EQUAL     , );           //?
		mapKey(KP_ENTER, RETURN);
		mapKey(NUM_LOCK, NUMLOCK);
		mapKey(CAPS_LOCK, CAPITAL);
		mapKey(SCROLL_LOCK, SCROLL);
		mapKey(PAUSE, PAUSE);
		mapKey(LEFT_SUPER, LWIN);
		mapKey(RIGHT_SUPER, RWIN);
		mapKey(MENU, APPS); //?
	default:
		return KeyCodes::AK_UNKNOWN;
	}
}

//=========================================================================
void HtmlUi::onResize(const unsigned int viewportWidth, const unsigned int viewportHeight)
{
	if (texture)
	{
		delete texture;
		texture = nullptr;
	}

	for (auto & item : mWebViewArray)
	{
		item.webView->Resize(viewportWidth, viewportHeight);
	}
}

//=========================================================================
inline void HtmlUi::addCallback(const HtmlViewID htmlViewID, JSDelegate delegate, const std::string& function)
{
	assert(htmlViewID != HTMLVIEW_NONE);

	if (static_cast<size_t>(htmlViewID) < mWebViewArray.size())
	{
		JSValue result = mWebViewArray[htmlViewID].webView->CreateGlobalJavascriptObject(WSLit("app"));

		if (result.IsObject())
		{
			JSObject& appObject = result.ToObject();
			methodDispatcher.Bind(appObject, WSLit(function.c_str()), delegate);
		}

		mWebViewArray[htmlViewID].webView->set_js_method_handler(&methodDispatcher);
	}
}

//=========================================================================
void HtmlUi::setActiveView(const HtmlViewID htmlViewID)
{
	if (static_cast<size_t>(htmlViewID) < mWebViewArray.size())
	{
		mActiveHtmlView = htmlViewID;
		mWebViewArray[htmlViewID].webView->Focus();
	}
	else
		mActiveHtmlView = HTMLVIEW_NONE;
}

//=========================================================================
bool HtmlUi::isViewActive(const HtmlViewID htmlViewID)
{
	return mActiveHtmlView == htmlViewID;
}