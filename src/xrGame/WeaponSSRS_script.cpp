#include "pch_script.h"
#include "WeaponSSRS.h"

using namespace luabind;

#pragma optimize("s",on)
void CWeaponSSRS::script_register(lua_State* L)
{
	module(L)
	[
		class_<CWeaponSSRS, CGameObject>("CWeaponSSRS")
		.def(constructor<>())
	];
}
