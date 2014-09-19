#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include <map>
#include "ImmutableBag.h"
#include "TypeInfoComparator.h"
#include <iostream>
#include <initializer_list>

namespace artemis {
  
	class EntitySystem;
	class Entity;
	class World;
  
	class SystemManager {
  public:
    SystemManager(World &world);
    ~SystemManager();
    void initializeAll();
    Bag<EntitySystem*> & getSystems();
    EntitySystem* setSystem(EntitySystem * stm);

	/*template<class T, typename... Args>
	T* setSystem(Args&& ... params)
	{
		return (T*)setSystem(new T(std::forward<Args>(params)...));
	}*/

	/*template<class T>
	T* setSystem()
	{
		return (T*)setSystem(new T);
	}

	template<class T, typename arg1>
	T* setSystem(arg1&& a1)
	{
		return (T*)setSystem(new T(a1));
	}

	template<class T, typename arg1, typename arg2>
	T* setSystem(arg1&& a1, arg2&& a2)
	{
		return (T*)setSystem(new T(a1, a2));
	}

	template<class T, typename arg1, typename arg2, typename arg3>
	T* setSystem(arg1&& a1, arg2&& a2, arg3&& a3)
	{
		return (T*)setSystem(new T(a1, a2, a3));
	}

	template<class T, typename arg1, typename arg2, typename arg3, typename arg4>
	T* setSystem(arg1&& a1, arg2&& a2, arg3&& a3, arg4&& a4)
	{
		return (T*)setSystem(new T(a1, a2, a3, a4));
	}*/

	template<class T, typename... Args>
	T* setSystem(Args... args)
	{
		return (T*)setSystem(new T(args...));
	}
    
	template<typename eSystem>
	eSystem* getSystem() {
	return (eSystem*)(systems[&typeid(eSystem)]);
	}
    
  private:
    World * world;
    std::map<const std::type_info*, EntitySystem*, type_info_comparator> systems;
    Bag<EntitySystem*> bagged;
	};
};
#endif // $(Guard token)
