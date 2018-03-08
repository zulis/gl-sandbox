#include "Subsystem.h"

namespace library
{
namespace subsystem
{
namespace details
{
Context &context()
{
    static Context s_context;
    return s_context;
}
}
}
}