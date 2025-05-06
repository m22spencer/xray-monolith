#include "stdafx.h"
#include "hudtarget.h"

#include "../xrEngine/Environment.h"
#include "../xrEngine/CustomHUD.h"
#include "Entity.h"
#include "Actor.h"
#include "Weapon.h"
#include "WeaponKnife.h"
#include "player_hud.h"
#include "Missile.h"
#include "level.h"
#include "game_cl_base.h"
#include "../xrEngine/igame_persistent.h"
#include "script_render_device.h"
#include "HUDManager.h"

#include "ui_base.h"
#include "InventoryOwner.h"
#include "relation_registry.h"
#include "character_info.h"

#include "string_table.h"
#include "entity_alive.h"

#include "inventory_item.h"
#include "inventory.h"

#include <ai/monsters/poltergeist/poltergeist.h>


#define C_DEFAULT	D3DCOLOR_RGBA(0xff,0xff,0xff,0x80)

static float lerp(float a, float b, float t)
{
	clamp(t, 0.f, 1.f);
	return a * (1 - t) + b * t;
}

CHUDTarget::CHUDTarget()
{
	crosshairPos = Fvector();
	crosshairOpacity = 1.f;

	Load();
	m_bShowCrosshair = false;
	m_bFirstUpdate = true;
}

CHUDTarget::~CHUDTarget()
{
}

float crosshair_occluded_opacity = .6f;
float crosshair_occlusion_fade_rate = 20.f;
float crosshair_distance_lerp_rate = 40.f;

void CHUDTarget::Load()
{
	HUDCrosshair.Load();
}

void CHUDTarget::ShowCrosshair(bool b)
{
	m_bShowCrosshair = b;
}

extern ENGINE_API BOOL g_bRendering;
u32 g_crosshair_color = C_DEFAULT;

float CHUDTarget::GetUIDist() const
{
	const SPickParam& pp = Actor()->GetPick();
	float dist = pp.result.range;
	return dist;
}

float CHUDTarget::GetTargetOpacity() const
{
	const SPickParam& pp = Actor()->GetPick();
	if (pp.barrel_blocked)
	{
		return 1.f;
	}

	// If the barrel is not occluded...
	// Test whether the aim point is occluded
	Fvector dir = Fvector().sub(
		Fvector().add(pp.defs.start, Fvector().mul(pp.defs.dir, GetUIDist())),
		Device.vCameraPosition
	);

	float dist = dir.magnitude();
	dir.normalize();
	SPickParam op = SPickParam();
	op.defs.start = Device.vCameraPosition;
	op.defs.dir = dir;
	op.defs.range = dist * 0.99f;
	if (!HUD().DoPick(op))
	{
		return 1.f;
	}

	// If it is, apply fade
	return crosshair_occluded_opacity;
}

void CHUDTarget::IntegratePosition()
{
	const SPickParam& pp = Actor()->GetPick();

	// Transform ray start and direction into camera space
	Fvector pos = pp.defs.start;
	Fvector dir = pp.defs.dir;
	Device.mView.transform_tiny(pos);
	Device.mView.transform_dir(dir);

	float dist = GetUIDist();
	Fvector target;
	if (dist > 0.f)
		target = Fvector().add(pos, Fvector().mul(dir, dist));
	else
		target = Fvector().add(pos, Fvector().mul(dir, pp.barrel_dist));

	// Interpolate crosshair position toward target
	if (!m_bFirstUpdate && psHUD_Flags.is(HUD_CROSSHAIR_DISTANCE_LERP))
	{
		float zFar = g_pGamePersistent->Environment().CurrentEnv->far_plane;
		float fac = 1 - (target.z / zFar);
		float t = Device.fTimeDelta * (fac + crosshair_distance_lerp_rate);
		clamp(t, 0.f, 1.f);
		crosshairPos.lerp(crosshairPos, target, t);
	}
	else
		crosshairPos = target;

	m_bFirstUpdate = false;
}

void CHUDTarget::IntegrateOpacity()
{
	float opacity_target = GetTargetOpacity();

	// Interpolate opacity offset toward target
	crosshairOpacity = lerp(crosshairOpacity, opacity_target, Device.fTimeDelta * crosshair_occlusion_fade_rate);
}

void CHUDTarget::Render()
{
	IntegratePosition();
	IntegrateOpacity();

	BOOL b_do_rendering = (psHUD_Flags.is(HUD_CROSSHAIR | HUD_CROSSHAIR_RT | HUD_CROSSHAIR_RT2));
	if (!b_do_rendering)
		return;

	VERIFY(g_bRendering);

	CObject* O = Level().CurrentEntity();
	if (0 == O) return;
	CEntity* E = smart_cast<CEntity*>(O);
	if (0 == E) return;

	const SPickParam& pp = Actor()->GetPick();

	// Construct animated aim point matrix from interpolated position and barrel roll
	Fvector hpb_barrel;
	pp.barrel_matrix.getHPB(hpb_barrel);

	Fvector hpb_cam;
	Device.mInvView.getHPB(hpb_cam);

	Fmatrix mat_aim = Fmatrix().identity();
	mat_aim.mulB_43(Device.mInvView);
	mat_aim.mulB_43(Fmatrix().translate(crosshairPos));

	if (HUD().FireposActive())
		mat_aim.mulB_43(Fmatrix().setHPB(0, 0, hpb_barrel.z - hpb_cam.z));

	// Crosshair color
	u32 color_readout = C_DEFAULT;

	HUDRecon.SetTransform(mat_aim);
	HUDRecon.SetColor(color_readout);
	HUDRecon.OnRender(GetUIDist());

	// Use the crosshair color unless the readout color is non-default
	color_readout = HUDRecon.GetColor();
	u32 color_crosshair = color_readout == C_DEFAULT ? g_crosshair_color : color_readout;

	// Modulate color alpha
	DWORD alpha_mask = 0xff000000;
	color_crosshair = (color_crosshair | alpha_mask)
		& ((DWORD)(alpha_mask * crosshairOpacity) | (~alpha_mask));

	if (m_bShowCrosshair)
	{
		// Update the crosshair's transform and color, and draw it
		HUDCrosshair.SetTransform(mat_aim);
		HUDCrosshair.SetColor(color_crosshair);
		HUDCrosshair.OnRender();
	}
}
