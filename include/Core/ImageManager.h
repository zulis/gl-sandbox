#pragma once

#include "Core/ResourceManager.h"
#include "Core/Image.h"

class ImageManager : public ResourceManager<Image>
{
public:
	static ImageManager* get();

private:
	static ImageManager mInstance;

private:
	virtual Image* loadFromFile(const std::string& fileName);

};

ImageManager ImageManager::mInstance;

//=========================================================================
ImageManager* ImageManager::get()
{
	return &mInstance;
}

//=========================================================================
Image* ImageManager::loadFromFile(const std::string& fileName)
{
	Image* image = new Image(fileName);
	return image;
}