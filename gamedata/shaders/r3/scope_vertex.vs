#include "common.h"


struct v_in
{
	float4	P		: POSITION;		// (float,float,float,1)
	float3	N		: NORMAL;		// (nx,ny,nz)
	float2	tc		: TEXCOORD0;	// (u,v)
};


struct vf {
    float4 hpos : SV_Position;
    float2 tc0  : TEXCOORD0;
};

vf main(v_in i) {
    vf o;
    o.hpos = mul(m_WVP, i.P);
    o.tc0  = i.tc;
    return o;
}