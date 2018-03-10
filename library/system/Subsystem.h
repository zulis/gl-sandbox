#pragma once

#include <memory>
#include <cassert>
#include <unordered_map>

namespace library
{
namespace subsystem
{
struct SubsystemContext;
namespace details
{
SubsystemContext &context();
}

struct SubsystemContext
{
    template<typename S, typename... Args>
    S &add(Args &&... args);

    template<typename S>
    S &get();

    template<typename S>
    bool has() const;

    template<typename S>
    void remove();

protected:
    std::unordered_map<unsigned int, std::shared_ptr<void>> subsystems;
};

template<typename S, typename... Args>
S &SubsystemContext::add(Args &&... args)
{
    assert(!has<S>() && "duplicated subsystem");
    const auto index = typeid(S).hash_code();
    subsystems.emplace(std::make_pair(index, std::make_unique<S>(std::forward<Args>(args)...)));
    return get<S>();
}

template<typename S>
S &SubsystemContext::get()
{
    assert(has<S>() && "failed to find subsystem");
    const auto index = typeid(S).hash_code();
    return *reinterpret_cast<S *>(subsystems[index].get());
}

template<typename S>
bool SubsystemContext::has() const
{
    const auto index = typeid(S).hash_code();
    return subsystems.find(index) != subsystems.end();
}

template<typename S>
void SubsystemContext::remove()
{
    assert(has<S>() && "failed to find subsystem");
    const auto index = typeid(S).hash_code();
    subsystems.erase(index);
}

template<typename S, typename... Args>
S &add(Args &&... args)
{
    return details::context().add<S>(std::forward<Args>(args)...);
}

template<typename S>
S &get()
{
    return details::context().get<S>();
}

template<typename S>
bool has()
{
    return details::context().has<S>();
}

template<typename S>
void remove()
{
    details::context().remove<S>();
}
}
}