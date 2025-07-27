// DO NOT EDIT, OR INCLUDE IN MODS

#ifndef SCOPE_COMMON_H
#define SCOPE_COMMON_H

#include "common.h"
#include "scope_defines.h"

#define ASPECT_CORRECT_TC(tc) (tc - 0.5) * float2(screen_res.x/screen_res.y, 1.0) + 0.5;

int scope_phase;

struct Scope
{
	float2 ffp;
	float2 sfp;
	float4 hpos;
    float2 tc0;	

	float4 dbg;
};


struct v_out {
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
float4 scope_w_eyepiece;
int scope_debug;

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

float2 ndc2(float4 p) {
	return p.xy / p.w;
}

float dbg_wp(v_out v, float4 p, float d) {
	float2 screen_tc = v.hpos.xy * screen_res.zw;
	float2 ffp_ndc = ndc2(mul(m_VP, p));
	float2 ffp_tc  = ffp_ndc * float2(0.5, -0.5) + 0.5;

	float2 ffp_tc_a = ASPECT_CORRECT_TC(ffp_tc);
	float2 screen_tc_a = ASPECT_CORRECT_TC(screen_tc);

	return distance(ffp_tc_a, screen_tc_a) < d ? 1.0 : 0.0;
}

float2 world_to_corrected_tc(v_out v, float4 w_P) {
	float2 screen_tc = v.hpos.xy * screen_res.zw;
	float2 ffp_ndc = ndc2(mul(m_VP, w_P));
	float2 ffp_tc  = ffp_ndc * float2(0.5, -0.5) + 0.5;

	return ASPECT_CORRECT_TC(ffp_tc);
}

Scope new_Scope(v_out v, float2 tc, float tc_multiplier, bool mag_sfp) {
	float factor = 4.0;

	Scope s;
    
	float r = dbg_wp(v, scope_w_eyepiece, .002);
	float g = dbg_wp(v, scope_w_ffp, .004);
	float b = dbg_wp(v, scope_w_sfp, .008);
	s.dbg = float4(r, g-r, b-(g+r), max(max(r,g),b));

	float2 eye_tc = world_to_corrected_tc(v, scope_w_eyepiece);
	
	{
		// COMPUTE FFP
		float2 ffp_tc = world_to_corrected_tc(v, scope_w_ffp);
		float2 ffp_offset_tc = eye_tc - ffp_tc;


		// FIXME: Aspect correct TC?
		s.ffp = tc + ffp_offset_tc*tc_multiplier;
	}

	{
		// COMPUTE SFP
		float2 sfp_tc = world_to_corrected_tc(v, scope_w_sfp);
		float2 sfp_offset_tc = eye_tc - sfp_tc;
		s.sfp = (mag_sfp ? ((tc - 0.5) / curMag() + 0.5) : tc) + sfp_offset_tc*tc_multiplier;
	}

	s.tc0 = v.tc0;
	s.hpos = v.hpos;

	return s;
}































#endif