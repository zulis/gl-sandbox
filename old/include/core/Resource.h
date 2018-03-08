#pragma once

// Using ideas from mine post http://stackoverflow.com/questions/32628963/c-templates-stdtuple-to-void-and-back

#include <memory>
#include <map>

template<class T, class...Args>
std::map<std::tuple<Args...>, std::shared_ptr<T>>& getCache()
{
	static std::map<std::tuple<Args...>, std::shared_ptr<T>> cache; // only run once
	return cache;
}

template<typename T, typename... Args>
std::shared_ptr<T> getResource(Args&& ... args)
{
	// std::decay_t should be used
	auto& cache = getCache<T, std::decay_t<Args>...>();

	// Creating tuple from the arguments
	auto arguments = std::forward_as_tuple(std::forward<Args>(args)...);

	// Search for object in the cache
	auto it = cache.find(arguments);

	if (it != cache.end())
	{
		// Found. Return.
		return it->second;
	}

	// Not found. Add to cache.
	auto object = std::make_shared<T>(std::forward<Args>(args)...);
	cache.emplace(std::make_pair(std::move(arguments), object));
	return object;
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

	/*
	template<typename T, typename... Args>
	T& getPtr() const
	{
		return *reinterpret_cast<T*>(Resource::get<T>(std::forward<Args>(args)...));
	}
	*/
};