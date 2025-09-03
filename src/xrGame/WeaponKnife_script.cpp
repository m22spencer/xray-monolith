#include "pch_script.h"
#include "WeaponKnife.h"

using namespace luabind;

#pragma optimize("s",on)
void CWeaponKnife::script_register(lua_State* L)
{
	module(L)
	[
		class_<CWeaponKnife, CGameObject>("CWeaponKnife")
		.def(constructor<>())
		.def("GetHit1Power", &CWeaponKnife::GetHit1Power)
		.def("GetHit2Power", &CWeaponKnife::GetHit2Power)
		.def("GetHit1PowerCritical", &CWeaponKnife::GetHit1PowerCritical)
		.def("GetHit2PowerCritical", &CWeaponKnife::GetHit2PowerCritical)
		.def("GetHit1Impulse", &CWeaponKnife::GetHit1Impulse)
		.def("GetHit2Impulse", &CWeaponKnife::GetHit2Impulse)
		.def("SetHit1Power", &CWeaponKnife::SetHit1Power)
		.def("SetHit2Power", &CWeaponKnife::SetHit2Power)
		.def("SetHit1PowerCritical", &CWeaponKnife::SetHit1PowerCritical)
		.def("SetHit2PowerCritical", &CWeaponKnife::SetHit2PowerCritical)
		.def("SetHit1Impulse", &CWeaponKnife::SetHit1Impulse)
		.def("SetHit2Impulse", &CWeaponKnife::SetHit2Impulse)
	];
}
