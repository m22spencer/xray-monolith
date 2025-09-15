#include "scope_common.h"

// This file is responsible for generating the image you see underneath the reticle

float3 scope_custom_image(Scope s) {
    float2 tc = SCOPECOORD_TO_TEXCOORD(s.sfp);
    return SampleBackbuffer(tc);
}

float3 scope_custom_image_postprocess(Scope s, float3 color) {
    return color;
}