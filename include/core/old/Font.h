#pragma once

#include <memory>
#include <string>
#include <SDL_ttf.h>
#include "core/GL.h"
#include "core/Math.h"
#include "core/Color.h"
#include "core/Shader.h"
#include "core/Geometry.h"
#include "core/Texture.h"

typedef std::shared_ptr<class Font> FontRef;

class Font
{
public:
	static FontRef create(const std::string& fileName, int size);
	Font(const std::string& fileName, int size);
	virtual ~Font();

	void print(const std::string& text, unsigned int x, unsigned int y, Color color = Color::white());

private:
	TTF_Font* mFont;
	ShaderRef mShader;
	GeometryRef mGeometry;
	TextureRef mTexture;

};

//=========================================================================
FontRef Font::create(const std::string& fileName, int size)
{
	return FontRef(new Font(fileName, size));
}

//=========================================================================
Font::Font(const std::string& fileName, int size)
{
	if(TTF_Init())
		printf("Error initializing font: %s", TTF_GetError());

	if(!(mFont = TTF_OpenFont(fileName.c_str(), size)))
		printf("Error loading font: %s", TTF_GetError());

	TTF_SetFontStyle(mFont, TTF_STYLE_NORMAL);

	mShader = Shader::create("assets/shaders/font");
	mGeometry = Geometry::create();

	//float sizeX = float(width) / float(Renderer::getWidth());
	//float sizeY = float(height) / float(Renderer::getHeight());

	float sizeX = 512 / 1024;
	float sizeY = 512 / 800;

	float vertices[] =
	{
		-sizeX, sizeY,
		-sizeX, -sizeY,
		sizeX, -sizeY,
		sizeX, -sizeY,
		sizeX, sizeY,
		-sizeX, sizeY
	};

	float uv[] =
	{
		0.0, 1.0,
		0.0, 0.0,
		1.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0
	};

	std::vector<vec3> v(std::begin(vertices), std::end(vertices));
	std::vector<vec2> t(std::begin(uv), std::end(uv));

	mGeometry->setVertices(v);
	mGeometry->setTexCoords(t);

	Texture::Format format;
	format.setMinFilter(GL_NEAREST);
	format.setMagFilter(GL_NEAREST);
	mTexture = Texture::create(std::string(), format);
}

//=========================================================================
Font::~Font()
{
	if(mFont)
		TTF_CloseFont(mFont);
}

//=========================================================================
void Font::print(const std::string& text, unsigned int x, unsigned int y, Color color)
{
	Texture::Format format;
	format.setMinFilter(GL_NEAREST);
	format.setMagFilter(GL_NEAREST);

	//mTexture.reset();
	//mTexture = Texture::create(std::string(), format);
	//mTexture = Texture::create("assets/textures/misc/hand.png", format);

	//SDL_Color clr = { color.r * 255, color.g * 255, color.b * 255, color.a * 255 };

	//auto surface = TTF_RenderUTF8_Blended(mFont, text.c_str(), clr);
	//GLenum format = (surface->format->BytesPerPixel == 3) ? GL_RGB : GL_RGBA;
	//mTexture->loadFromRaw(GL_RGBA/*format*/, surface->w, surface->h, (unsigned char*)surface->pixels);

	auto projection = ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	mat4 mvp = projection * mat4(1.0f);

	mTexture->bind();

	mShader->bind();
	mShader->setUniform("MVP", mvp);


	//glBindVertexArray(m_vao);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	mGeometry->draw(*mShader.get());
	//glBindVertexArray(0);


	mShader->unbind();

	mTexture->unbind();
}