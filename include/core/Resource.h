#pragma once

#include <memory>
#include <typeindex>
#include <map>

/*
class any
{
private:
struct base {
virtual ~base() {}
virtual base* clone() const = 0;
};
template <typename T>
struct data: base {
data(T const& value): value_(value) {}
base* clone() const { return new data<T>(*this); }
T value_;
};
base* ptr_;
public:
template <typename T> any(T const& value): ptr_(new data<T>(value)) {}
any(any const& other): ptr_(other.ptr_->clone()) {}
any& operator= (any const& other) {
any(other).swap(*this);
return *this;
}
~any() { delete this->ptr_; }
void swap(any& other) { std::swap(this->ptr_, other.ptr_); }

template <typename T>
T& get() {
return dynamic_cast<data<T>&>(*this->ptr_).value_;
}
};

int main()
{
any a0(17);
any a1(3.14);
try { a0.get<double>(); } catch (...) {}
a0 = a1;
std::cout << a0.get<double>() << "\n";
}
*/

/*
template <class T>
using StorageType = typename std::decay<T>::type;

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

		std::swap(ptr, a.ptr);

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
*/

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
	unpack(args, std::forward<Rest>(rest)...);
}

template<typename T, typename... Args>
T* getResource(Args&& ... args)
{
	ObjectArgs objectArgs;
	unpack(&objectArgs, std::forward<Args>(args)...);
	std::type_index objectType = { typeid(T) };

	auto range = object.equal_range(objectType);

	for(auto it = range.first; it != range.second; ++it)
	{
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