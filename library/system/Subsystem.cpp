#include "Subsystem.h"

namespace library
{
namespace subsystem
{
namespace details
{
SubsystemContext &context()
{
    static SubsystemContext s_context;
    return s_context;
}
}
}
}