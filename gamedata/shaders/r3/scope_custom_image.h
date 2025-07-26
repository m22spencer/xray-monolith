#include "scope_common.h"

// This file is responsible for generating the image you see underneath the reticle

float3 scope_custom_image(float2 tc) {
    return isSVPActive() 
        ? s_pip_tex.Sample(smp_base, tc)
        : s_3dss_tex.Sample(smp_base, tc);
}