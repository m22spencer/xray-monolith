#include "scope_common.h"

// This file is responsible for generating the image you see underneath the reticle

float3 scope_custom_image(Scope s) {
    float2 tc = isSVPActive() ? s.tc0 : s.hpos * screen_res.zw;
    return isSVPActive() 
        ? s_pip_tex.Sample(smp_base, tc)
        : s_3dss_tex.Sample(smp_base, tc);
}