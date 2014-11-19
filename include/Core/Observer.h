#pragma once

#include <vector>

template<typename Event>
class Listener
{
public:
	virtual ~Listener() {};
	virtual void onEvent(Event& event) = 0;
};

template<typename Event>
class Dispatcher
{
private:
	std::vector<Listener<Event>*> mListeners;

public:
	void registerListener(Listener<Event>* listener);
	void unregisterListener(Listener<Event>* listener);
	void dispatch(Event& event);
};

//=========================================================================
template<typename Event>
void Dispatcher<Event>::registerListener(Listener<Event>* listener)
{
	if(mListeners.end() == std::find(mListeners.begin(), mListeners.end(), listener))
		mListeners.push_back(listener);
}

//=========================================================================
template<typename Event>
void Dispatcher<Event>::unregisterListener(Listener<Event>* listener)
{
	mListeners.erase(std::remove(mListeners.begin(), mListeners.end(), listener), mListeners.end());
}

//=========================================================================
template<typename Event>
void Dispatcher<Event>::dispatch(Event& event)
{
	std::for_each(mListeners.begin(), mListeners.end(), [&](Listener<Event>* listener)
	{
		listener->onEvent(event);
	});
}