#pragma once

#include <string>
#include <vector>
#include <WebCore.h>
#include <BitmapSurface.h>
#include <STLHelpers.h>
#include "core/Geometry.h"
#include "core/Texture.h"

//using namespace Awesomium;

class NUi
{
public:
	NUi();
	~NUi();
	void addUi(unsigned int width, unsigned int height, const std::string& url);
	void draw(Renderer* renderer);

private:
	std::vector<Awesomium::WebView*> mViews;
	GLuint mTextureID;
	ShaderID mShader;
	Geometry* mGeometry;
};

//=========================================================================
NUi::NUi()
{
}

//=========================================================================
NUi::~NUi()
{
	if (mTextureID)
		glDeleteTextures(1, &mTextureID);

	delete mGeometry;

	for (auto& view : mViews)
	{
		view->Destroy();
	}

	Awesomium::WebCore::Shutdown();
}

void NUi::addUi(unsigned int width, unsigned int height, const std::string& url)
{
	using namespace Awesomium;

	if (!WebCore::instance())
	{
		WebCore::Initialize(WebConfig());
		glGenTextures(1, &mTextureID);

		std::vector<vec2> vertices =
		{
			vec2(0.0f, 0.0f),
			vec2(width, 0.0f),
			vec2(width, height),
			vec2(0.0f, height)
		};

		std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };

		std::vector<vec2> texCoords =
		{
			vec2(0.0f, 0.0f),
			vec2(1.0f, 0.0f),
			vec2(1.0f, 1.0f),
			vec2(0.0f, 1.0f)
		};

		mGeometry = new Geometry();
		mGeometry->setVertices(vertices);
		mGeometry->setIndices(indices);
		mGeometry->setTexCoords(texCoords);
	}

	auto webView = WebCore::instance()->CreateWebView(width, height);
	webView->LoadURL(WebURL(WSLit(url.c_str())));
	mViews.push_back(webView);
}

static bool NuiOnce = true;

//=========================================================================
void NUi::draw(Renderer* renderer)
{
	using namespace Awesomium;

	unsigned int windowWidth = 1280;
	unsigned int windowHeight = 720;
	float mRotation{ 0.0f };
	//vec2 mPosition;
	//vec2 mScale{ vec2(1) };

	bool viewsLoaded = true;

	for (auto& view : mViews)
	{
		if (view->IsLoading())
		{
			WebCore::instance()->Update();
			viewsLoaded = false;
		}
	}

	if (NuiOnce)
	{
		NuiOnce = false;
		mShader = renderer->addShader("assets/shaders/quad.shd");

		mGeometry->prepare(renderer->getShader(mShader));
	}
	else if (viewsLoaded)
	{
		//WebCore::instance()->Update();

		for (auto& view : mViews)
		{
			BitmapSurface* surface = (BitmapSurface*)view->surface();

			int w = surface->width();
			int h = surface->height();
			unsigned char *buffer = new unsigned char[w * h * 4];
			surface->CopyTo(buffer, w * 4, 4, false, false);

			glBindTexture(GL_TEXTURE_2D, mTextureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buffer);

			auto mMatrix = mat4(1.0f);
			mMatrix = rotate(mMatrix, mRotation, vec3(0.0f, 0.0f, 1.0f));
			mMatrix = translate(mMatrix, vec3(1.0, -1.0f, 0.0f));
			mat4 projection = ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f);
			auto mvp = mMatrix * mat4(1.0f) * projection;

			renderer->setShader(mShader);
			renderer->setTexture(mTextureID, 0);
			renderer->setShaderUniform(ShaderConstants::ColorMap, 0);
			renderer->setShaderUniform(ShaderConstants::MVP, mvp);

			mGeometry->draw();

			delete[] buffer;
		}
	}
}
