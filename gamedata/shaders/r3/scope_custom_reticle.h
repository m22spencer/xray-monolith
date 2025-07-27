#include "scope_common.h"
 
// This file is responsible for rendering the reticle (and for now, the shadow)

float4 scope_custom_reticle(Scope s) {
    float2 tc = s.tc0;
    
    float4 rtc = mul(m_VP, scope_w_sfp);
    rtc.xyz /= rtc.w;
    rtc.xy *= float2(0.5, -0.5);

    tc = (tc - 0.5) / curMag() + 0.5;
    tc += rtc*4.0;


    float4 o = s_reticle.Sample(smp_base, tc);
    bool opaque = s_reticle.SampleLevel(smp_base, tc, 10).a > .99;
    if (opaque)
        o.a = length(o.xyz);
    return o;
}