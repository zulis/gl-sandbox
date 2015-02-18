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
std::shared_ptr<T> get(Args&& ... args)
{
	ObjectArgs objectArgs;
	unpack(&objectArgs, std::forward<Args>(args)...);
	std::type_index objectType = { typeid(T) };

	auto range = object.equal_range(objectType);

	for(auto it = range.first; it != range.second; ++it)
	{
		if(objectArgs == it->second)
			return std::static_pointer_cast<T>(objectCollection[object]);
	}

	// not found... add
	object.emplace(objectType, objectArgs);
	auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
	objectCollection[object] = ptr;
	return ptr;
}

class Resource
{
public:
	virtual ~Resource() = default;

	template<typename T, typename... Args>
	static std::shared_ptr<T> create(Args&& ... args)
	{
		return get<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	static T* create2(Args&& ... args)
	{
		return get<T>(std::forward<Args>(args)...).get();
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