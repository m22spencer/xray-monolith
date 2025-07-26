#include "scope_common.h"

// This file sets the depth of the lens just prior to dof phase
//   - Setting it to a far value will result in a clear image
//   - Setting it to a close value will result in blur (useful for things like NVG)

float scope_custom_depth(float2 tc) {
    return 100.0;
}