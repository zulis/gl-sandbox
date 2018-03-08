#pragma once

#include <string>
#include <unordered_map>

template<typename T>
class ResourceManager
{
public:
	typedef std::unordered_map<std::string, T*> ResourceMap;

	ResourceManager();
	virtual ~ResourceManager();
	T* getResource(const std::string& fileName);
	void releaseResource(const std::string& fileName);
	void releaseAllResources();

protected:
	virtual T* loadFromFile(const std::string& fileName) = 0;

private:
	ResourceMap mResource;
	T* find(const std::string& fileName);
};

//=========================================================================
template<typename T>
ResourceManager<T>::ResourceManager()
{
}

//=========================================================================
template<typename T>
ResourceManager<T>::~ResourceManager()
{
	releaseAllResources();
}

//=========================================================================
template<typename T>
T* ResourceManager<T>::getResource(const std::string& fileName)
{
	T* resource = find(fileName);

	if(resource == NULL)
	{
		resource = loadFromFile(fileName);

		// If the resource loaded successfully, add it do the resource map
		if(resource != NULL)
			mResource.insert(std::make_pair(fileName, resource));
	}

	return resource;
}

//=========================================================================
template<typename T>
void ResourceManager<T>::releaseResource(const std::string& fileName)
{
	T* resource = find(fileName);

	if(resource != NULL)
	{
		delete resource;
		mResource.erase(mResource.find(fileName));
	}
}

//=========================================================================
template<typename T>
void ResourceManager<T>::releaseAllResources()
{
	while(mResource.begin() != mResource.end())
	{
		delete mResource.begin()->second;
		mResource.erase(mResource.begin());
	}
}

//=========================================================================
template<typename T>
T* ResourceManager<T>::find(const std::string& fileName)
{
	T* resource = NULL;
	typename ResourceMap::iterator it = mResource.find(fileName);

	if(it != mResource.end())
	{
		resource = it->second;
	}

	return resource;
}