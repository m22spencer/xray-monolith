// DO NOT EDIT, OR INCLUDE IN MODS

#ifndef SCOPE_COMMON_H
#define SCOPE_COMMON_H

#include "common.h"
#include "scope_defines.h"

int scope_phase;

struct Scope
{
    float4 hpos : SV_Position;
    float2 tc0 : TEXCOORD0;	
	float3 w_P : POSITION0;
	float3 w_T : TANGENT0;
	float3 w_B : BINORMAL0;
	float3 w_N : NORMAL0;
	float3 v_P : POSITION1;
	float3 v_T : TANGENT1;
	float3 v_B : BINORMAL1;
	float3 v_N : NORMAL1;
};

Texture2D s_pip_tex;
Texture2D s_3dss_tex;
Texture2D s_reticle;

float4 scope_w_ffp;
float4 scope_w_sfp;

uniform int scope_svp;
float isSVPActive() { return scope_svp; }

uniform float4 s3ds_param_2;
float zoomFactor() {
    return s3ds_param_2.w;
}

uniform float4 shader_scope_params;
float curMag() { return shader_scope_params.x; }
float minMag() { return shader_scope_params.y; }
float maxMag() { return shader_scope_params.z; }
































#endif