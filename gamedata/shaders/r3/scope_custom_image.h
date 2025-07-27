#include "scope_common.h"

// This file is responsible for generating the image you see underneath the reticle

float3 scope_custom_image(Scope s) {
    float2 tc =  isSVPActive() ? s.tc0 : s.hpos.xy * screen_res.zw;

    float4 rtc = mul(m_VP, scope_w_sfp);
    rtc.xyz /= rtc.w;
    rtc.xy *= float2(0.5, -0.5);

    tc += rtc*4;

    if (distance(tc, float2(0.5,0.5)) >= 0.5)
        return float3(0, 0, 0);

    return isSVPActive() 
        ? s_pip_tex.Sample(smp_base, tc)
        : s_3dss_tex.Sample(smp_base, tc);
}