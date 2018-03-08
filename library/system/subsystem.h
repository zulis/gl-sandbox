#pragma once

#include <memory>
#include <cassert>
#include <unordered_map>

namespace library
{
namespace core
{
struct subsystem_context;
namespace details
{
subsystem_context &context();
}

struct subsystem_context
{
    template<typename S, typename... Args>
    S &add_subsystem(Args &&... args);

    template<typename S>
    S &get_subsystem();

    template<typename S>
    bool has_subsystem() const;

    template<typename S>
    void remove_subsystem();

protected:
    std::unordered_map<unsigned int, std::shared_ptr<void>> subsystems;
};

template<typename S, typename... Args>
S &subsystem_context::add_subsystem(Args &&... args)
{
    assert(!has_subsystem<S>() && "duplicated subsystem");
    const auto index = typeid(S).hash_code();
    subsystems.emplace(std::make_pair(index, std::make_unique<S>(std::forward<Args>(args)...)));
    return get_subsystem<S>();
}

template<typename S>
S &subsystem_context::get_subsystem()
{
    assert(has_subsystem<S>() && "failed to find subsystem");
    const auto index = typeid(S).hash_code();
    return *reinterpret_cast<S *>(subsystems[index].get());
}

template<typename S>
bool subsystem_context::has_subsystem() const
{
    const auto index = typeid(S).hash_code();
    return subsystems.find(index) != subsystems.end();
}

template<typename S>
void subsystem_context::subsystem_context::remove_subsystem()
{
    assert(has_subsystem<S>() && "failed to find subsystem");
    const auto index = typeid(S).hash_code();
    subsystems.erase(index);
}

template<typename S, typename... Args>
S &add_subsystem(Args &&... args)
{
    return details::context().add_subsystem<S>(std::forward<Args>(args)...);
}

template<typename S>
S &get_subsystem()
{
    return details::context().get_subsystem<S>();
}

template<typename S>
bool has_subsystem()
{
    return details::context().has_subsystem<S>();
}

template<typename S>
void remove_subsystem()
{
    details::context().remove_subsystem<S>();
}
}
}