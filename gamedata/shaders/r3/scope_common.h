#ifndef SCOPE_COMMON_H
#define SCOPE_COMMON_H

#include "common.h"
#include "scope_defines.h"

int scope_phase;

struct vf {
    float4 hpos : SV_Position;
    float2 tc0  : TEXCOORD0;
};

Texture2D s_pip_tex;
Texture2D s_3dss_tex;
Texture2D s_reticle;

uniform int scope_svp;
float isSVPActive() { return scope_svp; }

uniform float4 s3ds_param_2;
float zoomFactor() {
    return s3ds_param_2.w;
}

































#endif