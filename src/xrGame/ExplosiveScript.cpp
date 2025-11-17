#include "pch_script.h"
#include "Explosive.h"

using namespace luabind;

#pragma optimize("s",on)
void CExplosive::script_register(lua_State* L)
{
	module(L)
	[
		class_<CExplosive>("explosive")
		.def("explode", (&CExplosive::Explode))

#ifdef STATIONARYMGUN_NEW
		.def("Initiator", (&CExplosive::Initiator))
		.def("SetInitiator", (&CExplosive::SetInitiator))
		.def("LoadExplosiveSection", (void (CExplosive::*)(LPCSTR)) &CExplosive::LoadExplosiveSection)
		.def("LoadExplosiveSection", (void (CExplosive::*)(CInifile*, LPCSTR)) &CExplosive::LoadExplosiveSection)
#endif
	];
}
