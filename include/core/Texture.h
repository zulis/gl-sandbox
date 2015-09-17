#pragma once

//#include <memory>
//#include <vector>
//#include <map>
#include "core/GL.h"
#include "core/Color.h"
#include "core/Log.h"
#include "core/Image.h"
#include "core/Resource.h"

class Texture
{
public:
	struct Format;

	Texture(const std::string &fileName = std::string(), const Format &format = Format());
	Texture(const Color &color, const Format &format = Format());
	~Texture();

	void bind(GLuint textureUnit = 0);
	void unbind(GLuint textureUnit = 0);

	int getWidth() const;
	int getHeight() const;
	GLuint getTextureID() const;
	/*int getChannels() const;
	const unsigned char *getPixels() const;
	const char *getFileName() const;
	*/

	struct Format
	{
		Format();
		void setFlipped(bool flipped);
		void setMipmapping(bool mipmapping);
		void setMinFilter(GLenum minFiler);
		void setMagFilter(GLenum magFiler);

	protected:
		GLenum mTarget;
		GLenum mWrapS, mWrapT;
		GLenum mMinFilter, mMagFilter;
		bool mMipmapping;
		bool mFlipped;

		friend class Texture;
	};

private:
	GLuint mTextureID;
	Format mFormat;
	const char *mFileName;

	//unsigned char *mPixels;
	unsigned int mWidth{ 0 };
	unsigned int mHeight{ 0 };
	//unsigned int mChannels{ 0 };

	/*static std::map<std::string, std::shared_ptr<Image>> sImages;
	static std::map<Color, std::shared_ptr<Image>> sImagesColor;*/

private:
	void getTexture(const std::string &fileName, const Format &format);
	void getTexture(const Color &color, const Format &format);
	void loadFromRaw(const int format, const int width, const int height, const unsigned char *pixels);
	/*Image* getImage(const std::string &fileName);
	Image* getImage(const Color &color);*/

};

//std::map<std::string, std::shared_ptr<Image>> Texture::sImages;
//std::map<Color, std::shared_ptr<Image>> Texture::sImagesColor;
//
////=========================================================================
//Image* Texture::getImage(const std::string &fileName)
//{
//	auto it = sImages.find(fileName);
//	if (it != sImages.end())
//		return it->second.get();
//	else
//	{
//		std::shared_ptr<Image> newObject(new Image(fileName));
//		sImages.insert(std::make_pair(fileName, newObject));
//		return newObject.get();
//	}
//}
//
////=========================================================================
//Image* Texture::getImage(const Color &color)
//{
//	auto it = sImagesColor.find(color);
//	if (it != sImagesColor.end())
//		return it->second.get();
//	else
//	{
//		std::shared_ptr<Image> newObject(new Image(color));
//		sImagesColor.insert(std::make_pair(color, newObject));
//		return newObject.get();
//	}
//}

//=========================================================================
Texture::Format::Format()
{
	mTarget = GL_TEXTURE_2D;
	mWrapS = GL_REPEAT;
	mWrapT = GL_REPEAT;
	mMinFilter = GL_LINEAR;
	mMagFilter = GL_LINEAR;
	mMipmapping = true;
	mFlipped = false;
}

//=========================================================================
void Texture::Format::setFlipped(bool flipped)
{
	mFlipped = flipped;
}

//=========================================================================
void Texture::Format::setMipmapping(bool mipmapping)
{
	mMipmapping = mipmapping;
}

//=========================================================================
void Texture::Format::setMinFilter(GLenum minFiler)
{
	mMinFilter = minFiler;
}

//=========================================================================
void Texture::Format::setMagFilter(GLenum magFiler)
{
	mMagFilter = magFiler;
}

//=========================================================================
Texture::Texture(const std::string &fileName, const Format &format)
{
	getTexture(fileName, format);
}

Texture::Texture(const Color &color, const Format &format)
{
	getTexture(color, format);
}

//=========================================================================
Texture::~Texture()
{
	if (mTextureID)
		glDeleteTextures(1, &mTextureID);

	note("Texture released: %s", mFileName);
}

//=========================================================================
void Texture::getTexture(const std::string &fileName, const Format &format)
{
	mFormat = format;

	auto image = Resource::get<Image>(fileName);
	//auto image = getImage(fileName);
	mFileName = image->getFileName();

	if (mFormat.mFlipped)
		image->flipVertical();

	loadFromRaw(GL_BGRA, image->getWidth(), image->getHeight(), image->getPixels());
	note("Texture created: %s", mFileName);
}

//=========================================================================
void Texture::getTexture(const Color &color, const Format &format)
{
	mFormat = format;

	auto image = Resource::get<Image>(color);
	//auto image = getImage(color);
	mFileName = image->getFileName();

	if (mFormat.mFlipped)
		image->flipVertical();

	loadFromRaw(GL_BGRA, image->getWidth(), image->getHeight(), image->getPixels());
	note("Texture created: %s", mFileName);
}

//=========================================================================
void Texture::loadFromRaw(const int format, const int width, const int height, const unsigned char *pixels)
{
	// http://www.opengl.org/wiki/Common_Mistakes#Automatic_mipmap_generation

	mWidth = width;
	mHeight = height;

	if (mFormat.mMipmapping)
	{
		mFormat.mMinFilter = GL_LINEAR_MIPMAP_LINEAR;
		mFormat.mMagFilter = GL_LINEAR;
	}

	int numMipmaps = 1;
	while ((width | height) >> numMipmaps) numMipmaps += 1;

	glGenTextures(1, &mTextureID);
	glBindTexture(mFormat.mTarget, mTextureID);
	glTexStorage2D(mFormat.mTarget, numMipmaps, GL_RGBA8, width, height);
	glTexSubImage2D(mFormat.mTarget, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, pixels);

	if (mFormat.mMipmapping)
	{
		glGenerateMipmap(mFormat.mTarget);
	}

	glTexParameteri(mFormat.mTarget, GL_TEXTURE_WRAP_S, mFormat.mWrapS);
	glTexParameteri(mFormat.mTarget, GL_TEXTURE_WRAP_T, mFormat.mWrapT);
	glTexParameteri(mFormat.mTarget, GL_TEXTURE_MAG_FILTER, mFormat.mMagFilter);
	glTexParameteri(mFormat.mTarget, GL_TEXTURE_MIN_FILTER, mFormat.mMinFilter);

	GLfloat anisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropy);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (GLint)anisotropy);

	glBindTexture(mFormat.mTarget, 0);
}

//=========================================================================
void Texture::bind(GLuint textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(mFormat.mTarget, mTextureID);
	glActiveTexture(GL_TEXTURE0);
}

//=========================================================================
void Texture::unbind(GLuint textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(mFormat.mTarget, 0);
	glActiveTexture(GL_TEXTURE0);
}


//=========================================================================
int Texture::getWidth() const
{
	return mWidth;
}

//=========================================================================
int Texture::getHeight() const
{
	return mHeight;
}

//=========================================================================
GLuint Texture::getTextureID() const
{
	return mTextureID;
}

/*//=========================================================================
int Texture::getChannels() const
{
return mImage->getChannels();
}

//=========================================================================
const unsigned char *Texture::getPixels() const
{
return mImage->getPixels();
}

//=========================================================================
const char *Texture::getFileName() const
{
return mImage->getFileName();
}
*/