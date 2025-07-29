#include "StdAfx.h"
#include "../xrRender/light.h"
#include "../../xrEngine/cl_intersect.h"

const u32 delay_small_min = 1;
const u32 delay_small_max = 3;
const u32 delay_large_min = 10;
const u32 delay_large_max = 20;
const u32 cullfragments = 4;

void light::vis_prepare()
{
	if (int(indirect_photons) != ps_r2_GI_photons) gi_generate();

	//	. test is sheduled for future	= keep old result
	//	. test time comes :)
	//		. camera inside light volume	= visible,	shedule for 'small' interval
	//		. perform testing				= ???,		pending

	u32 frame = Device.dwFrame;
	if (frame < vis.frame2test)
		return;
	
	if (vis.pending) 
		return; // If a query is pending, do not overwhelm the gpu with more queries.

	vis.distance = Device.vCameraPosition.distance_to(spatial.sphere.P);

	// testing
	vis.pending = true;
	xform_calc();
	RCache.set_xform_world(m_xform);
	vis.query_order = RImplementation.occq_begin(vis.query_id);
	RImplementation.Target->draw_volume(this);
	RImplementation.occq_end(vis.query_id);
}

void light::vis_update()
{
	//	. not pending	->>> return (early out)
	//	. test-result:	visible:
	//		. shedule for 'large' interval
	//	. test-result:	invisible:
	//		. shedule for 'next-frame' interval
	if (!vis.pending) return;

	// Non blocking vis check.
	// Possible minor delays with light visiblity changes
	auto query = RImplementation.occq_try_get(vis.query_id);
	if (!query.complete)
		return;

	u32 frame = Device.dwFrame;

	// Tuning this too aggressively will cull lights when pip is active.
	const float vis_frag_relax_rate = 0.5;
	vis.visible_frags = (vis.visible_frags + query.fragments) * vis_frag_relax_rate;
	vis.visible = vis.visible_frags > cullfragments;
	vis.pending = false;
	if (vis.visible)
	{
		vis.frame2test = frame + ::Random.randI(delay_large_min, delay_large_max);
	}
	else
	{
		vis.frame2test = frame + 1;
	}
}
