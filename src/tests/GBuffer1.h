#pragma once

#include "core/GL.h"
#include "core/Log.h"

#define GBUFFER_POSITION_TEXTURE_UNIT 0
#define GBUFFER_DIFFUSE_TEXTURE_UNIT  1
#define GBUFFER_NORMAL_TEXTURE_UNIT   2
#define GBUFFER_TEXCOORD_TEXTURE_UNIT 3

class GBuffer1
{
public:
	enum GBUFFER_TEXTURE_TYPE {
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_DIFFUSE,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		GBUFFER_TEXTURE_TYPE_TEXCOORD,
		GBUFFER_NUM_TEXTURES
	};

	GBuffer1();
	~GBuffer1();

	bool init(unsigned int windowWidth, unsigned int windowHeight);
	void bindForWriting();
	void bindForReading();
	void setReadBuffer(GBUFFER_TEXTURE_TYPE textureType);
	void resize(unsigned int windowWidth, unsigned int windowHeight);

private:
	GLuint mFbo;
	GLuint mTextures[GBUFFER_NUM_TEXTURES];
	GLuint mDepthTexture;

	void cleanup();
};

//=========================================================================
GBuffer1::GBuffer1()
{
	mFbo = 0;
	mDepthTexture = 0;
	ZERO_MEM(mTextures);
}

//=========================================================================
GBuffer1::~GBuffer1()
{
	cleanup();
}

//=========================================================================
void GBuffer1::cleanup()
{
	if (mFbo != 0) {
		glDeleteFramebuffers(1, &mFbo);
	}

	if (mTextures[0] != 0) {
		glDeleteTextures(ARRAY_SIZE_IN_ELEMENTS(mTextures), mTextures);
	}

	if (mDepthTexture != 0) {
		glDeleteTextures(1, &mDepthTexture);
	}
}

//=========================================================================
bool GBuffer1::init(unsigned int windowWidth, unsigned int windowHeight)
{
	// Create the FBO
	glGenFramebuffers(1, &mFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

	// Create the gbuffer textures
	glGenTextures(ARRAY_SIZE_IN_ELEMENTS(mTextures), mTextures);
	glGenTextures(1, &mDepthTexture);

	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(mTextures); i++)
	{
		glBindTexture(GL_TEXTURE_2D, mTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, mTextures[i], 0);
	}

	// Depth
	glBindTexture(GL_TEXTURE_2D, mDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTexture, 0);

	GLenum drawBuffers[] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3 
	};

	glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(drawBuffers), drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		error("FB error, status: 0x%x\n", status);
		return false;
	}

	// Restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	return true;
}

//=========================================================================
void GBuffer1::bindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFbo);
}

//=========================================================================
void GBuffer1::bindForReading()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mFbo);
}

//=========================================================================
void GBuffer1::setReadBuffer(GBUFFER_TEXTURE_TYPE textureType)
{
	glReadBuffer(GL_COLOR_ATTACHMENT0 + textureType);
}

//=========================================================================
void GBuffer1::resize(unsigned int windowWidth, unsigned int windowHeight)
{
	cleanup();
	init(windowWidth, windowHeight);
}
