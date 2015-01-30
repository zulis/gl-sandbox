#pragma once

// https://gist.github.com/siondream/11212605

#include <functional>
#include <set>

#include "core/Listeners.h"

template <class T>
class ListenerSet
{
public:
	ListenerSet() {}
	virtual ~ListenerSet() {}

	inline void addListener(T listener);
	inline void removeListener(T listener);
	template <class Function, class... Arguments>
	inline void notify(Function&& f, Arguments&& ... args);

private:
	std::set<T> mListeners;
};

//=========================================================================
template <class T>
void ListenerSet<T>::addListener(T listener)
{
	mListeners.insert(listener);
}

//=========================================================================
template <class T>
void ListenerSet<T>::removeListener(T listener)
{
	mListeners.erase(listener);
}

//=========================================================================
template <class T>
template <class Function, class... Arguments>
void ListenerSet<T>::notify(Function&& f, Arguments&& ... args)
{
	for(T listener : mListeners)
	{
		auto callback = std::bind(f, listener, args...);
		callback(listener);
	}
}