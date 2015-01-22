#pragma once

#include <string>
#include <FreeImage.h>
#include "Core/Color.h"
#include "Core/Log.h"

class Image
{
public:
	Image(const std::string& fileName = std::string());
	Image(const Color& color);
	virtual ~Image();

	unsigned int getWidth();
	unsigned int getHeight();
	unsigned int getChannels();
	unsigned int getBpp();
	const unsigned char* getPixels();
	const std::string& getFileName() const;
	void flipVertical();
	void flipHorizontal();

private:
	unsigned int mWidth;
	unsigned int mHeight;
	unsigned int mChannels;
	unsigned int mBpp;
	unsigned char* mPixels;
	std::string mFileName;

private:
	void generateCheckImage();
	void generateImage(const Color& color);
	void loadFromFile(const std::string& fileName);
	static void FreeImageLoadErrorHandler(FREE_IMAGE_FORMAT fif, const char* message);

};

//=========================================================================
Image::Image(const std::string& fileName)
{
	loadFromFile(fileName);
}

//=========================================================================
Image::Image(const Color& color)
{
	generateImage(color);
}

//=========================================================================
Image::~Image()
{
	// Is called ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise();
#endif

	if(mPixels)
		delete[] mPixels;
}

//=========================================================================
void Image::loadFromFile(const std::string& fileName)
{
	// Is called ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_Initialise();
#endif

	mFileName = fileName;
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* bitmap(0);

	// Set error handler
	FreeImage_SetOutputMessage(FreeImageLoadErrorHandler);

	// Check the file signature and deduce its format
	fif = FreeImage_GetFileType(fileName.c_str(), 0);

	// If still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(fileName.c_str());

	// If still unknown, return failure
	if(fif == FIF_UNKNOWN)
		generateCheckImage();
	else
	{
		// Check that the plugin has reading capabilities and load the file
		if(FreeImage_FIFSupportsReading(fif))
			bitmap = FreeImage_Load(fif, fileName.c_str());

		// If the image failed to load, return failure
		if(!bitmap)
			generateCheckImage();
		else
		{
			mBpp = FreeImage_GetBPP(bitmap);

			// Convert, or may crash on Texture->loadFromRaw
			if(mBpp != 32)
			{
				bitmap = FreeImage_ConvertTo32Bits(bitmap);
				mBpp = 32;
			}

			/*switch (mBpp)
			{
			case 8:
			bitmap = FreeImage_ConvertToGreyscale(bitmap);
			break;

			case 24:
			bitmap = FreeImage_ConvertTo24Bits(bitmap);
			break;

			case 32:
			bitmap = FreeImage_ConvertTo32Bits(bitmap);
			break;

			default:
			//Free FreeImage's copy of the data
			FreeImage_Unload(bitmap);
			generateCheckTexture();
			return;
			break;
			}*/

			// Get the image width and height
			mWidth = FreeImage_GetWidth(bitmap);
			mHeight = FreeImage_GetHeight(bitmap);
			mChannels = mBpp / 8;

			// Retrieve the image data
			BYTE* bits = FreeImage_GetBits(bitmap);

			// If this somehow one of these failed (they shouldn't), return failure
			if((bits == 0) || (mWidth == 0) || (mHeight == 0))
			{
				logError("Could not load texture: %s", fileName.c_str());
				generateCheckImage();
			}
			else
			{
				//FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(bitmap);
				//mPixels = (unsigned char*)malloc(mWidth * mHeight * colorType);
				//memcpy(mPixels, bits, mWidth * mHeight * colorType);

				mPixels = new unsigned char[mWidth * mHeight * mChannels];
				FreeImage_ConvertToRawBits(mPixels, bitmap, mWidth * mChannels, mBpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);

				//Free FreeImage's copy of the data
				FreeImage_Unload(bitmap);
			}
		}
	}
}

//=========================================================================
void Image::generateCheckImage()
{
	mFileName = "Check image";
	mWidth = 128;
	mHeight = 128;
	mChannels = 4;
	mBpp = 4 * 8;

	GLubyte checkImage[128][128][4];

	unsigned int i, j, c;

	for(i = 0; i < mWidth; i++)
	{
		for(j = 0; j < mHeight; j++)
		{
			c = (((i & 0x8) == 0) ^ ((j & 0x8)) == 0) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	mPixels = (unsigned char*)malloc(mWidth * mHeight * mChannels);
	memcpy(mPixels, checkImage, mWidth * mHeight * mChannels);
}

//=========================================================================
void Image::generateImage(const Color& color)
{
	mFileName = "Color (r:" + std::to_string(color.r) + " g:" + std::to_string(color.g) + " b:" + std::to_string(color.b) + " a:" + std::to_string(color.a) + ")";
	mWidth = 1;
	mHeight = 1;
	mChannels = 4;
	mBpp = 4 * 8;

	GLubyte image[1][1][4];
	image[0][0][0] = (GLubyte)color.b * 255;
	image[0][0][1] = (GLubyte)color.g * 255;
	image[0][0][2] = (GLubyte)color.r * 255;
	image[0][0][3] = (GLubyte)color.a * 255;

	mPixels = (unsigned char*)malloc(mWidth * mHeight * mChannels);
	memcpy(mPixels, image, mWidth * mHeight * mChannels);
}

//=========================================================================
void Image::FreeImageLoadErrorHandler(FREE_IMAGE_FORMAT fif, const char* message)
{
	logError("FreeImage error: %s", message);
}

//=========================================================================
unsigned int Image::getWidth()
{
	return mWidth;
}

//=========================================================================
unsigned int Image::getHeight()
{
	return mHeight;
}

//=========================================================================
unsigned int Image::getChannels()
{
	return mChannels;
}

//=========================================================================
unsigned int Image::getBpp()
{
	return mBpp;
}

//=========================================================================
const unsigned char* Image::getPixels()
{
	return mPixels;
}

//=========================================================================
const std::string& Image::getFileName() const
{
	return mFileName;
}

//=========================================================================
void Image::flipVertical()
{
	FIBITMAP* bitmap = FreeImage_ConvertFromRawBits(mPixels, mWidth, mHeight, mWidth * mChannels, mBpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
	FreeImage_FlipVertical(bitmap);

	if(mPixels)
		delete[] mPixels;

	mPixels = new unsigned char[mWidth * mHeight * mChannels];
	FreeImage_ConvertToRawBits(mPixels, bitmap, mWidth * mChannels, mBpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
	FreeImage_Unload(bitmap);
}

//=========================================================================
void Image::flipHorizontal()
{
	FIBITMAP* bitmap = FreeImage_ConvertFromRawBits(mPixels, mWidth, mHeight, mWidth * mChannels, mBpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
	FreeImage_FlipHorizontal(bitmap);

	if(mPixels)
		delete[] mPixels;

	mPixels = new unsigned char[mWidth * mHeight * mChannels];
	FreeImage_ConvertToRawBits(mPixels, bitmap, mWidth * mChannels, mBpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
	FreeImage_Unload(bitmap);
}
