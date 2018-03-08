#include "subsystem.h"

namespace library
{
namespace core
{
namespace details
{
subsystem_context &context()
{
    static subsystem_context s_context;
    return s_context;
}
}
}
}