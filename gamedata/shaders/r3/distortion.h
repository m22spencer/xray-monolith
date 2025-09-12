#include "common.h"

#ifndef DISTORTION_H
#define DISTORTION_H

// Taken from combine_2_naa

Texture2D 			s_distort;
#define	EPSDEPTH	0.001

float2 distortion_tc(float2 tc, float distortion_scale = 1.0) {
	float2 sc = tc * screen_res.xy;

	gbuffer_data gbd	= gbuffer_load_data(tc, sc);
	
  	float 	depth 	= gbd.P.z;
	float4 	distort	= s_distort.Sample(smp_nofilter, tc);
	float2	offset	= (distort.xy-(127.0h/255.0h))*def_distort;  // fix newtral offset
	float2	center	= tc + offset * distortion_scale;

	gbuffer_data gbdx	= gbuffer_load_data_offset(tc, center, sc);

	float 	depth_x	= gbdx.P.z;
	if ((depth_x+EPSDEPTH)<depth)	center	= tc;	// discard new sample

	return center;
}

#endif