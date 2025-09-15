#include "luabind_memory.h"

namespace luabind {
    // Define the global instance
    allocator_func_t custom_allocator = luabind_allocator;
}
