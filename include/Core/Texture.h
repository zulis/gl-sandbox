#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <FreeImage.h>
#include "core/GL.h"
#include "core/ImageManager.h"
#include "core/Color.h"
#include "core/Log.h"

struct TextureCollection
{
	bool operator==(const TextureCollection& m) const
	{
		return m.fileName == fileName;
	}

	unsigned int increaseRef()
	{
		refCount++;
		logNote("Texture ref increased to %i: %s", refCount, fileName.c_str());
		return refCount;
	}

	unsigned int decreaseRef()
	{
		refCount--;
		logNote("Texture ref decreased to %i: %s", refCount, fileName.c_str());
		return refCount;
	}

	std::string fileName;
	GLuint textureID { 0 };
	unsigned int refCount { 1 };
	unsigned int width;
	unsigned int height;
};

typedef std::shared_ptr<class Texture> TextureRef;

class Texture
{
	friend class Font;
public:
	struct Format;

	static TextureRef create(const std::string& fileName = std::string(), const Format& format = Format());
	static TextureRef create(const Color& color, const Format& format = Format());

	Texture(const std::string& fileName = std::string(), const Format& format = Format());
	Texture(const Color& color, const Format& format = Format());
	virtual ~Texture();

	void bind(GLuint textureUnit = 0);
	void unbind(GLuint textureUnit = 0);


	int getWidth() const;
	int getHeight() const;
	GLuint getTextureID() const;
	/*int getChannels() const;
	const unsigned char* getPixels() const;
	const std::string& getFileName() const;
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
	ImageManager mImageMng;
	Image* mImage;
	void getTexture(const std::string& fileName, const Format& format);
	void getTexture(const Color& color, const Format& format);
	std::string colorToString(const Color& color);
	void loadFromRaw(const int format, const int width, const int height, const unsigned char* pixels);

	GLuint mTextureID;
	Format mFormat;
	std::string mFileName;
	static std::vector<TextureCollection>* mTextureCollection;

	//unsigned char* mPixels;
	unsigned int mWidth { 0 };
	unsigned int mHeight { 0 };
	//unsigned int mChannels{ 0 };

};

std::vector<TextureCollection>* Texture::mTextureCollection = new std::vector<TextureCollection>();

//=========================================================================
TextureRef Texture::create(const std::string& fileName, const Format& format)
{
	return TextureRef(new Texture(fileName, format));
}

//=========================================================================
TextureRef Texture::create(const Color& color, const Format& format)
{
	return TextureRef(new Texture(color, format));
}

//=========================================================================
// TextureRef Texture::create(const std::string& name, const int internalFormat, const int pixelDataFormat, const int width, const int height, const unsigned char* buffer, unsigned int channels, const Format& format)
// {
// 	return TextureRef(new Texture(name, internalFormat, pixelDataFormat, width, height, buffer, channels, format));
// }

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
Texture::Texture(const std::string& fileName, const Format& format)
{
	getTexture(fileName, format);
}

Texture::Texture(const Color& color, const Format& format)
{
	getTexture(color, format);
}

//=========================================================================
Texture::~Texture()
{
	unsigned int refCount = -1;
	TextureCollection textureCollection;
	textureCollection.fileName = mFileName;

	auto it = std::find(mTextureCollection->begin(), mTextureCollection->end(), textureCollection);

	if(it != mTextureCollection->end())
	{
		refCount = (*it).decreaseRef();
	}

	if(refCount == 0)
	{
		mTextureCollection->erase(it);

		if(mTextureID)
			glDeleteTextures(1, &mTextureID);

		logNote("Texture released: %s", mFileName.c_str());
	}
}

//=========================================================================
void Texture::getTexture(const std::string& fileName, const Format& format)
{
	mFormat = format;
	mFileName = fileName;

	TextureCollection textureCollection;
	textureCollection.fileName = fileName;

	auto it = std::find(mTextureCollection->begin(), mTextureCollection->end(), textureCollection);

	if(it != mTextureCollection->end())
	{
		(*it).increaseRef();
		mTextureID = (*it).textureID;
		mWidth = (*it).width;
		mHeight = (*it).height;
	}
	else
	{
		mImage = mImageMng.get()->getResource(fileName);

		if(mFormat.mFlipped)
			mImage->flipVertical();

		loadFromRaw(GL_BGRA, mImage->getWidth(), mImage->getHeight(), mImage->getPixels());

		textureCollection.textureID = mTextureID;
		textureCollection.width = mImage->getWidth();
		textureCollection.height = mImage->getHeight();

		logNote("Texture created: %s", mFileName.c_str());
		mTextureCollection->push_back(textureCollection);
	}
}

//=========================================================================
void Texture::getTexture(const Color& color, const Format& format)
{
	mFormat = format;
	mFileName = colorToString(color);

	TextureCollection textureCollection;
	textureCollection.fileName = colorToString(color);

	auto it = std::find(mTextureCollection->begin(), mTextureCollection->end(), textureCollection);

	if(it != mTextureCollection->end())
	{
		(*it).increaseRef();
		mTextureID = (*it).textureID;
		mWidth = (*it).width;
		mHeight = (*it).height;
	}
	else
	{
		mImage = new Image(color);

		if(mFormat.mFlipped)
			mImage->flipVertical();

		loadFromRaw(GL_BGRA, mImage->getWidth(), mImage->getHeight(), mImage->getPixels());

		textureCollection.textureID = mTextureID;
		textureCollection.width = mImage->getWidth();
		textureCollection.height = mImage->getHeight();

		logNote("Texture created: %s", mFileName.c_str());
		mTextureCollection->push_back(textureCollection);

		delete mImage;
	}
}

//=========================================================================
std::string Texture::colorToString(const Color& color)
{
	return "Color (r:" + std::to_string(color.r) + " g:" + std::to_string(color.g) + " b:" + std::to_string(color.b) + " a:" + std::to_string(color.a) + ")";
}

//=========================================================================
void Texture::loadFromRaw(const int format, const int width, const int height, const unsigned char* pixels)
{
	// http://www.opengl.org/wiki/Common_Mistakes#Automatic_mipmap_generation

	mWidth = width;
	mHeight = height;

	if(mFormat.mMipmapping)
	{
		mFormat.mMinFilter = GL_LINEAR_MIPMAP_LINEAR;
		mFormat.mMagFilter = GL_LINEAR;
	}

	int numMipmaps = 1;
	while((width | height) >> numMipmaps) numMipmaps += 1;

	glGenTextures(1, &mTextureID);
	glBindTexture(mFormat.mTarget, mTextureID);
	glTexStorage2D(mFormat.mTarget, numMipmaps, GL_RGBA8, width, height);
	glTexSubImage2D(mFormat.mTarget, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(mFormat.mTarget);
	glTexParameteri(mFormat.mTarget, GL_TEXTURE_WRAP_S, mFormat.mWrapS);
	glTexParameteri(mFormat.mTarget, GL_TEXTURE_WRAP_T, mFormat.mWrapT);
	glTexParameteri(mFormat.mTarget, GL_TEXTURE_MAG_FILTER, mFormat.mMagFilter);
	glTexParameteri(mFormat.mTarget, GL_TEXTURE_MIN_FILTER, mFormat.mMinFilter);

	GLfloat anisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropy);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (GLint)anisotropy);

	glBindTexture(mFormat.mTarget, 0);

	/*
	if (mFormat.mMipmapping)
	{
		mFormat.mMinFilter = GL_LINEAR_MIPMAP_LINEAR;
		mFormat.mMagFilter = GL_LINEAR;
	}

	glGenTextures(1, &mTextureID);
	glBindTexture(mFormat.mTarget, mTextureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glTexImage2D(mFormat.mTarget, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(mFormat.mTarget, GL_TEXTURE_WRAP_S, mFormat.mWrapS);
	glTexParameteri(mFormat.mTarget, GL_TEXTURE_WRAP_T, mFormat.mWrapT);
	glTexParameteri(mFormat.mTarget, GL_TEXTURE_MIN_FILTER, mFormat.mMinFilter);
	glTexParameteri(mFormat.mTarget, GL_TEXTURE_MAG_FILTER, mFormat.mMagFilter);

	GLfloat anisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropy);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);

	if (mFormat.mMipmapping)
		glGenerateMipmap(mFormat.mTarget);

	glBindTexture(mFormat.mTarget, 0);
	*/
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
const unsigned char* Texture::getPixels() const
{
	return mImage->getPixels();
}

//=========================================================================
const std::string& Texture::getFileName() const
{
	return mImage->getFileName();
}
*/