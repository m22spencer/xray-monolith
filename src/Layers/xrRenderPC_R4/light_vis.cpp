#include "StdAfx.h"
#include "../xrRender/light.h"
#include "../../xrEngine/cl_intersect.h"
#include "../../xrGame/debug_renderer.h"

const u32 delay_small_min = 1;
const u32 delay_small_max = 3;
const u32 delay_large_min = 10;
const u32 delay_large_max = 20;
const u32 cullfragments = 4;

void light::vis_prepare()
{
	PIX_EVENT(LIGHT_PREPARE);
	if (int(indirect_photons) != ps_r2_GI_photons) gi_generate();

	//	. test is sheduled for future	= keep old result
	//	. test time comes :)
	//		. camera inside light volume	= visible,	shedule for 'small' interval
	//		. perform testing				= ???,		pending

	u32 frame = Device.dwFrame;
	if (frame < vis.frame2test && scope_debug < 3)
		return;

	float safe_area = VIEWPORT_NEAR;
	{
		float a0 = deg2rad(Device.fFOV * Device.fASPECT / 2.f);
		float a1 = deg2rad(Device.fFOV / 2.f);
		float x0 = VIEWPORT_NEAR / _cos(a0);
		float x1 = VIEWPORT_NEAR / _cos(a1);
		float c = _sqrt(x0 * x0 + x1 * x1);
		safe_area = _max(_max(VIEWPORT_NEAR, _max(x0, x1)), c);
	}

	auto vLightToPlayer = Fvector(Device.vCameraPosition).sub(position);
	vis.distance = vLightToPlayer.magnitude();
	auto inside_range = vis.distance < (spatial.sphere.R * 1.25f + safe_area);
	auto inside_cone  = direction.dotproduct(vLightToPlayer) > 0;  //FIXME: Use actual cone
	if (inside_range)
	{
		if (scope_debug >=3) CDebugRenderer().draw_aabb(Fvector(position).add(direction), .05, .05, .05, inside_cone ? 0xff00ff00 : 0xff0000ff, false);
		vis.visible = true;
		vis.visible_frags = 65000;  //Extremely high priority light
		vis.frame2test = frame + ::Random.randI(delay_small_min, delay_small_max);
		return;
	}

	xform_calc();
	RCache.set_xform_world(m_xform);
	vis.query_order = RImplementation.occq_begin(vis.query_id);
	vis.r4_query_ids.push_back(vis.query_id);
	RImplementation.Target->draw_volume(this);
	RImplementation.occq_end(vis.query_id);
}

void light::vis_update()
{
	auto frame = Device.dwFrame;
	float safe_area = VIEWPORT_NEAR;
	{
		float a0 = deg2rad(Device.fFOV * Device.fASPECT / 2.f);
		float a1 = deg2rad(Device.fFOV / 2.f);
		float x0 = VIEWPORT_NEAR / _cos(a0);
		float x1 = VIEWPORT_NEAR / _cos(a1);
		float c = _sqrt(x0 * x0 + x1 * x1);
		safe_area = _max(_max(VIEWPORT_NEAR, _max(x0, x1)), c);
	}

	auto vLightToPlayer = Fvector(Device.vCameraPosition).sub(position);
	vis.distance = vLightToPlayer.magnitude();
	auto inside_range = vis.distance < (spatial.sphere.R * 1.25f + safe_area);
	auto inside_cone  = direction.dotproduct(vLightToPlayer) > 0;  //FIXME: Use actual cone
	if (inside_range)
	{
		if (scope_debug >=3) CDebugRenderer().draw_aabb(Fvector(position).add(direction), .05, .05, .05, inside_cone ? 0xff00ff00 : 0xff0000ff, false);
		vis.visible = true;
		vis.visible_frags = 65000;  //Extremely high priority light
		vis.frame2test = frame + ::Random.randI(delay_small_min, delay_small_max);
		return;
	}

	//	. not pending	->>> return (early out)
	//	. test-result:	visible:
	//		. shedule for 'large' interval
	//	. test-result:	invisible:
	//		. shedule for 'next-frame' interval
	if (vis.r4_query_ids.empty()) return;

	// Non blocking vis check.
	// Possible minor delays with light visiblity changes
	xr_vector<u32> o;
	for (auto q : vis.r4_query_ids) {
		auto query = RImplementation.occq_try_get(q);
		if (query.complete) vis.accumulating_frags += query.fragments;
		else o.push_back(q);
	}
	vis.r4_query_ids = o;
	if (!vis.r4_query_ids.empty())
		return;

	vis.visible_frags = vis.accumulating_frags;
	vis.accumulating_frags = 0;
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
