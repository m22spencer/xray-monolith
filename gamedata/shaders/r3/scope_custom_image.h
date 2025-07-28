#include "scope_common.h"

// This file is responsible for generating the image you see underneath the reticle

float3 scope_custom_image(Scope s) {
    if (distance(s.sfp, s.center) >= s.radius)
        return float3(0, 0, 0);

    float2 tc = SCOPECOORD_TO_TEXCOORD(s.sfp);
    return SampleBackbuffer(tc);
}