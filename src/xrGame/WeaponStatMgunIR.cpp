#include "stdafx.h"
#include "WeaponStatMgun.h"
#include "xr_level_controller.h"

#ifdef STATIONARYMGUN_NEW
#include "Actor.h"
#include "level.h"
#include "camerafirsteye.h"
#endif

void CWeaponStatMgun::OnMouseMove(int dx, int dy)
{
	if (Remote()) return;

#ifdef STATIONARYMGUN_NEW
	CCameraBase *cam = Camera();
	float scale = (cam->f_fov / g_fov) * psMouseSens * psMouseSensScale / 50.f;
	if (dx)
	{
		float d = float(dx) * scale;
		cam->Move((d < 0) ? kLEFT : kRIGHT, _abs(d));
	}
	if (dy)
	{
		float d = ((psMouseInvert.test(1)) ? -1 : 1) * float(dy) * scale * 3.f / 4.f;
		cam->Move((d > 0) ? kUP : kDOWN, _abs(d));
	}
#else
	float scale = psMouseSens * psMouseSensScale / 50.f;
	float h, p;
	m_destEnemyDir.getHP(h, p);
	if (dx)
	{
		float d = float(dx) * scale;
		h -= d;
		SetDesiredDir(h, p);
	}
	if (dy)
	{
		float d = ((psMouseInvert.test(1)) ? -1 : 1) * float(dy) * scale * 3.f / 4.f;
		p -= d;
		SetDesiredDir(h, p);
	}
#endif
}

void CWeaponStatMgun::OnKeyboardPress(int dik)
{
	if (Remote()) return;

	switch (dik)
	{
	case kWPN_FIRE:
#ifdef STATIONARYMGUN_NEW
		Action(eWpnFire, 1);
#else
		FireStart();
#endif
		break;
#ifdef STATIONARYMGUN_NEW
	case kWPN_ZOOM:
		if (!psActorFlags.test(AF_AIM_TOGGLE))
		{
			m_zoom_status = true;
		}
		else
		{
			m_zoom_status = (m_zoom_status) ? false : true;
		}
		break;
	case kWPN_RELOAD:
		Action(eWpnReload, 0);
		break;
	case kDROP:
		Action(eWpnUnload, 0);
		break;
#endif
	};
}

void CWeaponStatMgun::OnKeyboardRelease(int dik)
{
	if (Remote()) return;
	switch (dik)
	{
	case kWPN_FIRE:
#ifdef STATIONARYMGUN_NEW
		Action(eWpnFire, 0);
#else
		FireEnd();
#endif
		break;
#ifdef STATIONARYMGUN_NEW
	case kWPN_ZOOM:
		if (!psActorFlags.test(AF_AIM_TOGGLE))
		{
			m_zoom_status = false;
		}
		break;
	case kCAM_1:
		OnCameraChange(eCamFirst);
		break;
	case kCAM_2:
		OnCameraChange(eCamChase);
		break;
#endif
	};
}

void CWeaponStatMgun::OnKeyboardHold(int dik)
{
}

#ifdef STATIONARYMGUN_NEW
#include "pch_script.h"
#include "alife_space.h"

using namespace luabind;

#pragma optimize("s",on)
void CWeaponStatMgun::script_register(lua_State* L)
{
	module(L)
	[
		class_<CWeaponStatMgun, bases<CGameObject, CHolderCustom>>("CWeaponStatMgun")
		.enum_("stm_wpn")
		[
			value("eWpnActivate", int(CWeaponStatMgun::eWpnActivate)),
			value("eWpnFire", int(CWeaponStatMgun::eWpnFire)),
			value("eWpnDesiredPos", int(CWeaponStatMgun::eWpnDesiredPos)),
			value("eWpnDesiredDir", int(CWeaponStatMgun::eWpnDesiredDir)),
			value("eWpnDesiredAng", int(CWeaponStatMgun::eWpnDesiredAng)),
			value("eWpnReload", int(CWeaponStatMgun::eWpnReload)),
			value("eWpnUnload", int(CWeaponStatMgun::eWpnUnload))
		]
		.def("Action", &CWeaponStatMgun::Action)
		.def("SetParam", (void (CWeaponStatMgun::*)(int, Fvector))&CWeaponStatMgun::SetParam)

		.def("GetOwner", &CWeaponStatMgun::GetOwner)
		.def("GetFirePos", &CWeaponStatMgun::GetFirePos)
		.def("GetFireDir", &CWeaponStatMgun::GetFireDir)
		.def("ExitPosition", &CWeaponStatMgun::ExitPosition)
		.def("InFieldOfView", &CWeaponStatMgun::InFieldOfView)
		.def(constructor<>())
	];
}
#endif