#pragma once

#include <memory>
#include <typeindex>
#include <map>

typedef std::multimap<std::type_index, void*> ObjectArgs;
typedef std::multimap<std::type_index, ObjectArgs> Object;
typedef std::map<Object, std::shared_ptr<void>> ObjectCollection;

ObjectArgs objectArgs;
Object object;
ObjectCollection objectCollection;

void unpack(ObjectArgs* args) {}

template <typename First, typename... Rest>
void unpack(ObjectArgs* args, First first, Rest... rest)
{
	args->emplace(typeid(first), (void*)first);
	unpack(args, rest...);
}

template<typename T, typename... Args>
T* get(Args&& ... args)
{
	ObjectArgs objectArgs;
	unpack(&objectArgs, std::forward<Args>(args)...);
	std::type_index objectType = { typeid(T) };

	auto range = object.equal_range(objectType);

	for(auto it = range.first; it != range.second; ++it)
	{
		if(objectArgs == it->second)
            // found... return
            return static_cast<T*>(objectCollection[object].get());
	}

	// not found... add and return
	object.emplace(objectType, objectArgs);
    objectCollection[object] = std::make_shared<T>(std::forward<Args>(args)...);
    return static_cast<T*>(objectCollection[object].get());
}

class Resource
{
public:
	virtual ~Resource() = default;

	template<typename T, typename... Args>
	static T* create(Args&& ... args)
	{
		return get<T>(std::forward<Args>(args)...);
	}
};

template<typename T>
class Shared
{
public:
	template<typename... Args >
	static std::shared_ptr<T> create(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
};