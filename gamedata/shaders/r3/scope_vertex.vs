// DO NOT EDIT, OR INCLUDE IN MODS

#include "scope_common.h"

struct	v_in
{
	float4	P		: POSITION;		// (float,float,float,1)
	float3	N		: NORMAL;		// (nx,ny,nz)
	float3	T		: TANGENT;		// (nx,ny,nz)
	float3	B		: BINORMAL;		// (nx,ny,nz)
	float2	tc		: TEXCOORD0;	// (u,v)
};

Scope     main (v_in v)
{
    Scope o;

    o.hpos = mul(m_WVP, v.P);
    o.tc0 = v.tc.xy;

	o.w_P = mul(m_W, v.P).xyz;
	o.w_T = mul(m_W, v.T).xyz;
	o.w_B = mul(m_W, v.B).xyz;
	o.w_N = mul(m_W, v.N).xyz;

	o.v_P = mul(m_WV, v.P).xyz;
	o.v_T = mul(m_WV, v.T).xyz;
	o.v_B = mul(m_WV, v.B).xyz;
	o.v_N = mul(m_WV, v.N).xyz;
	
    return o;
}