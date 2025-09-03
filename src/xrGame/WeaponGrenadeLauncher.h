#ifndef _WEAPON_GRENADE_LAUNCHER_
#define _WEAPON_GRENADE_LAUNCHER_

#include "../xrCDB/xr_collide_defs.h"

class CWeapon;

class CWeaponGrenadeLauncher
{
private:
    static BOOL use_dynamic_range(CWeapon* wpn);
    static collide::rq_target get_rq_target();
public:
    static void LaunchGrenade(CWeapon* wpn);
};

#endif //_WEAPON_GRENADE_LAUNCHER