#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl/GL.h>
#include "core/Color.h"

#if !defined(GL_TEXTURE_MAX_ANISOTROPY_EXT)
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#endif

#if !defined(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT)
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

namespace gl
{

enum CullFaceType
{
	Back,
	Front
};

inline void setViewport(int width, int height)
{
	glViewport(0, 0, width, height);
}

inline void clear(const Color& color = Color::black(), bool clearDepthBuffer = true, bool clearStencilBuffer = true)
{
	glClearColor(color.r, color.g, color.b, color.a);

	auto flags = GL_COLOR_BUFFER_BIT;

	if(clearDepthBuffer)
	{
		glDepthMask(GL_TRUE);
		flags |= GL_DEPTH_BUFFER_BIT;
	}

	if(clearStencilBuffer)
		flags |= GL_STENCIL_BUFFER_BIT;

	glClear(flags);
}

inline void enable(GLenum state)
{
	glEnable(state);
}

inline void disable(GLenum state)
{
	glDisable(state);
}

inline void enableAlphaBlending(bool premultiplied = false)
{
	glEnable(GL_BLEND);

	if(!premultiplied)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	else
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

inline void disableAlphaBlending()
{
	glDisable(GL_BLEND);
}

inline void enableDepthRead()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void disableDepthRead()
{
	glDisable(GL_DEPTH_TEST);
}

inline void enableDepthWrite()
{
	glDepthMask(GL_TRUE);
}

inline void disableDepthWrite()
{
	glDepthMask(GL_FALSE);
}

inline void enableCullFace(CullFaceType type)
{
	glEnable(GL_CULL_FACE);

	if(type == CullFaceType::Front)
		glCullFace(GL_FRONT);
	else
		glCullFace(GL_BACK);
}

inline void disableCullFace()
{
	glDisable(GL_CULL_FACE);
}

inline void enable3D()
{
	enableDepthRead();
	enableDepthWrite();
}

inline void enable2D()
{
	disableDepthWrite();
	disableDepthRead();
}

} // namespace gl
