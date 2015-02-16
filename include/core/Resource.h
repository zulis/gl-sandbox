#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <tuple>
#include <string>
#include <iostream>
#include <map>
#include <algorithm>
//#include <boost/any.hpp>
//#include <boost/variant.hpp>

///////////////////////////////////////////////////////////////////////////////////////

#pragma region ANY
#include <type_traits>
#include <utility>
#include <typeinfo>
#include <string>
#include <cassert>

using namespace std;

//template<class T>
//using StorageType = typename decay<typename remove_reference<T>::type>::type;
template <class T>
using StorageType = typename decay<T>::type;

struct Any
{
    bool is_null() const { return !ptr; }
    bool not_null() const { return ptr; }

    template<typename U> Any(U&& value)
        : ptr(new Derived<StorageType<U>>(forward<U>(value)))
    {

    }

    template<class U> bool is() const
    {
        typedef StorageType<U> T;

        auto derived = dynamic_cast<Derived<T>*> (ptr);

        return derived;
    }

    template<class U>
    StorageType<U>& as()
    {
        typedef StorageType<U> T;

        auto derived = dynamic_cast<Derived<T>*> (ptr);

        if (!derived)
            throw bad_cast();

        return derived->value;
    }

    template<class U>
    operator U()
    {
        return as<StorageType<U>>();
    }

    Any()
        : ptr(nullptr)
    {

    }

    Any(Any& that)
        : ptr(that.clone())
    {

    }

    Any(Any&& that)
        : ptr(that.ptr)
    {
        that.ptr = nullptr;
    }

    Any(const Any& that)
        : ptr(that.clone())
    {

    }

    Any(const Any&& that)
        : ptr(that.clone())
    {

    }

    Any& operator=(const Any& a)
    {
        if (ptr == a.ptr)
            return *this;

        auto old_ptr = ptr;

        ptr = a.clone();

        if (old_ptr)
            delete old_ptr;

        return *this;
    }

    Any& operator=(Any&& a)
    {
        if (ptr == a.ptr)
            return *this;

        swap(ptr, a.ptr);

        return *this;
    }

    ~Any()
    {
        if (ptr)
            delete ptr;
    }

private:
    struct Base
    {
        virtual ~Base() {}

        virtual Base* clone() const = 0;
    };

    template<typename T>
    struct Derived : Base
    {
        template<typename U> Derived(U&& value) : value(forward<U>(value)) { }

        T value;

        Base* clone() const { return new Derived<T>(value); }
    };

    Base* clone() const
    {
        if (ptr)
            return ptr->clone();
        else
            return nullptr;
    }

    Base* ptr;
};
#pragma endregion ANY
///////////////////////////////////////////////////////////////////////////////////////

std::vector<std::type_index> vec;

template <typename T>
typename std::vector<T>::const_iterator find(const std::vector<T>& v, const T& value)
{
    return std::find(v.begin(), v.end(), value);
}

typedef std::multimap<std::type_index, Any> MapType;
MapType myMap;

/*
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
*/

template<typename T, typename... Args>
void fillas2(Args&&... args)
{
    std::type_index a = { typeid(T) };
    //std::type_index b = { typeid(Args)... };
    auto tup = std::make_tuple(args...);

    auto vals = std::tie(tup);

    auto it = myMap.find(a);

    if (it != myMap.end())
    {
        auto asd = std::forward_as_tuple(it->second);

        if (std::get<0>(tup) == std::get<0>(asd))
            printf("valio!\n");
    }

    myMap.insert(MapType::value_type(a, args...));


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

        //fillas<Args...>(vec);
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