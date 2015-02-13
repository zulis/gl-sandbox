#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <boost/any.hpp>
#include <tuple>
#include <string>
#include <iostream>
#include <map>
#include <algorithm>
//#include <boost/any.hpp>
//#include <boost/variant.hpp>

std::vector<std::type_index> vec;

template <typename T>
typename std::vector<T>::const_iterator find(const std::vector<T>& v, const T& value)
{
	return std::find(v.begin(), v.end(), value);
}

typedef std::multimap<std::type_index, std::type_index> MapType;
MapType myMap;

template <typename... Args>
void fillas(std::vector<std::type_index>& vec)
{
	const std::type_info& r = {typeid(Args)...};
	std::type_index a = { typeid(Args)... };

	if (std::find(vec.begin(), vec.end(), a) != vec.end())
	{
		printf("valio!\n");
	}

	//vec.push_back({ typeid(Args)... });
}

template<typename T, typename... Args>
void fillas2(Args&&... args)
{
	std::type_index a = { typeid(T) };
	std::type_index b = { typeid(Args)... };

	auto it = myMap.find(a);

	if (it != myMap.end())
	{
		auto name = it->second.name();

		if (b == it->second)
			printf("valio!\n");
	}

	myMap.insert(MapType::value_type(a, b));


// 	std::type_index a = { typeid(T) };
// 
// 	if (std::find(vec.begin(), vec.end(), a) != vec.end())
// 	{
// 		printf("valio!\n");
// 	}

	//vec.push_back({ typeid(T) });
}

// template<typename T1, typename T2, typename ... Tn>
// void fill(){
// 	fill<T1>();
// 	fill<T2, Tn...>();
// }

class Resource
{
public:
	virtual ~Resource() = default;

	template<typename T, typename... Args>
	static std::shared_ptr<T> create(Args&&... args)
	{
		auto t = std::type_index(typeid(T));
		//fillas<char, int, float>(vec);

		fillas<Args...>(vec);
		fillas2<T>(std::forward<Args>(args)...);

		return std::make_shared<T>(std::forward<Args>(args)...);
		//return unique_ptr<T>(new T(std::forward<Args>(args)...));
	}

private:
	static std::map<std::type_index, std::string> sMap;
};

std::map<std::type_index, std::string> Resource::sMap;

/*
template<typename Creator, typename Destructor, typename... Arguments>
auto make_resource(Creator c, Destructor d, Arguments&&... args)
-> std::unique_ptr < typename std::decay<decltype(*c(std::forward<Arguments>(args)...))>::type, void(*)(typename std::decay<decltype(*c(std::forward<Arguments>(args)...))>::type*) >
{
auto r = c(std::forward<Arguments>(args)...);
if (!r) { throw std::runtime_error{ "Unable to create resource" }; }
typedef typename std::decay<decltype(*r)>::type ResourceType;
return std::unique_ptr<ResourceType, void(*)(ResourceType*)>(r, d);
}
*/


/*
template<typename T>
class NonConvertibleWeakPtr
{
public:
NonConvertibleWeakPtr(const std::shared_ptr<T>& p) : p_(p) {}
... // other constructors / assignment operators
bool expired() const { return p_.expired(); }
T* operator->() const { return get(); }
T& operator*() const { return *get(); }
private:
T* get() const { return p_.lock().get(); }
private:
std::weak_ptr<T> p_;
};
*/