#pragma once

#include <memory>
#include <typeindex>
#include <map>

template<typename T, typename... Args>
std::shared_ptr<T> getResource(Args&& ... args)
{
	static std::map<std::tuple<Args...>, std::shared_ptr<T>> objectCollection;

	// Creating tuple from the arguments
	std::tuple<Args...> currentArgs(std::forward<Args>(args)...);

	//Search for object in map
	auto objectIter = objectCollection.find(currentArgs);

	if (objectIter != objectCollection.end())
	{
		return objectIter->second;
	}

	std::shared_ptr<T> newObject(new T(args...));
	objectCollection.insert(std::pair<std::tuple<Args...>, std::shared_ptr<T>>(currentArgs, newObject));
	return newObject;
}

class Resource
{
public:
	virtual ~Resource() = default;

	template<typename T, typename... Args>
	static std::shared_ptr<T> get(Args&& ... args)
	{
		return getResource<T>(std::forward<Args>(args)...);
	}
};