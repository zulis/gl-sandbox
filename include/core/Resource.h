#pragma once

#include <memory>
#include <typeindex>
#include <map>

#include <sstream>


template <class T>
std::string
toString(const T & t) {

	std::ostringstream oss; // create a stream
	oss << t; // insert value to stream
	return oss.str(); // extract value and return
}

typedef std::multimap<std::type_index, void*> ObjectArgs;
typedef std::multimap<std::type_index, ObjectArgs> Object;
typedef std::map<Object, std::shared_ptr<void>> ObjectCollection;

ObjectArgs objectArgs;
Object object;
ObjectCollection objectCollection;

void unpack(ObjectArgs* args) {}

template <typename First, typename... Rest>
void unpack(ObjectArgs* objectArgs, First first, Rest&&... rest)
{
	std::cout << toString<First>(first) << "\t" << endl;

	objectArgs->emplace(typeid(first), (void*)&first);
	unpack(objectArgs, std::forward<Rest>(rest)...);
}

template<typename T, typename... Args>
T* getResource(Args&& ... args)
{
	ObjectArgs objectArgs;
	unpack(&objectArgs, std::forward<Args>(args)...);
	std::type_index objectType = { typeid(T) };


	/*
	for (ObjectArgs::iterator it = objectArgs.begin(); it != objectArgs.end(); ++it) {
		//cout << it->first.name() << "\t" << it->second << endl;
		//cout << it->first.name() << "\t" << *static_cast<std::string*>(it->second) << endl;
	}
	*/


	auto range = object.equal_range(objectType);

	for(auto it = range.first; it != range.second; ++it)
	{
		/*
		ObjectArgs result;
		std::set_intersection(objectArgs.begin(), objectArgs.end(),
			it->second.begin(), it->second.end(),
			std::inserter(result, result.end()));
		*/


		if (objectArgs == it->second)
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
	static T* get(Args&& ... args)
	{
		return getResource<T>(std::forward<Args>(args)...);
	}
};

/*
template<typename T>
class Shared
{
public:
	template<typename... Args >
	static std::shared_ptr<T> create(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
};
*/