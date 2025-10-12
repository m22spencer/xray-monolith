#include "StdAfx.h"
#include <utility>

#include "WeaponGrenadeLauncher.h"
#include "../xrPhysics/MathUtils.h"
#include "Actor.h"
#include "Level.h"
#include "Entity.h"
#include "Inventory.h"
#include "InventoryOwner.h"
#include "Weapon.h"
#include "WeaponMagazined.h"
#include "RocketLauncher.h"
#include "ExplosiveRocket.h"
#include "xrDebug.h"

BOOL g_launcher_dynamic_range = FALSE;
BOOL g_launcher_dynamic_range_zoom = TRUE;
BOOL g_launcher_dynamic_range_mode = TRUE;
float g_launcher_dynamic_range_max = 300.0f;

BOOL CWeaponGrenadeLauncher::use_dynamic_range(CWeapon* wpn)
{
    if (wpn->IsZoomed())
    {
        return g_launcher_dynamic_range_zoom;
    }

    return g_launcher_dynamic_range;
}

collide::rq_target CWeaponGrenadeLauncher::get_rq_target()
{
    if(g_launcher_dynamic_range_mode)
        return collide::rqtBoth;
    
    return collide::rqtStatic;
}

void CWeaponGrenadeLauncher::LaunchGrenade(CWeapon* wpn)
{
    CWeaponMagazined* wm = smart_cast<CWeaponMagazined*>(wpn);
    VERIFY(wm);

    CRocketLauncher* rl = smart_cast<CRocketLauncher*>(wpn);
    VERIFY(rl);
    
#ifdef CROCKETLAUNCHER_CHANGE
    LPCSTR ammo_name = wm->m_ammoTypes[wm->m_ammoType].c_str();
    float launch_speed = READ_IF_EXISTS(pSettings, r_float, ammo_name, "ammo_grenade_vel", rl->m_fLaunchSpeed);
#endif
    Fvector p1, d;
    p1.set(wpn->get_LastFP2());
    d.set(wpn->get_LastFD());
    CEntity* E = smart_cast<CEntity*>(wpn->H_Parent());

    if (E)
    {
        CInventoryOwner* io = smart_cast<CInventoryOwner*>(wpn->H_Parent());
        if (NULL == io->inventory().ActiveItem())
        {
            Log("current_state", wpn->GetState());
            Log("next_state", wpn->GetNextState());
            Log("item_sect", wpn->cNameSect().c_str());
            Log("H_Parent", wpn->H_Parent()->cNameSect().c_str());
        }
        E->g_fireParams(wpn, p1, d);
    }
    if (IsGameTypeSingle())
        p1.set(wpn->get_LastFP2());

    Fmatrix launch_matrix;
    launch_matrix.identity();
    launch_matrix.k.set(d);
    Fvector::generate_orthonormal_basis(launch_matrix.k,
                                        launch_matrix.j,
                                        launch_matrix.i);

    launch_matrix.c.set(p1);

    if (IsGameTypeSingle() && use_dynamic_range(wpn) && smart_cast<CActor*>(wpn->H_Parent()))
    {
        wpn->H_Parent()->setEnabled(FALSE);
        wpn->setEnabled(FALSE);

        collide::rq_result RQ;
        BOOL HasPick = Level().ObjectSpace.RayPick(p1, d, g_launcher_dynamic_range_max, get_rq_target(), RQ, wpn);

        wpn->setEnabled(TRUE);
        wpn->H_Parent()->setEnabled(TRUE);

        if (HasPick)
        {
            Fvector Transference;
            Transference.mul(d, RQ.range);
            Fvector res[2];
#ifdef		DEBUG
            //.				DBG_OpenCashedDraw();
            //.				DBG_DrawLine(p1,Fvector().add(p1,d),D3DCOLOR_XRGB(255,0,0));
#endif
#ifdef CROCKETLAUNCHER_CHANGE
            u8 canfire0 = TransferenceAndThrowVelToThrowDir(Transference, launch_speed, wpn->EffectiveGravity(), res);
#else
            u8 canfire0 = TransferenceAndThrowVelToThrowDir(Transference,
                                                            rl->m_fLaunchSpeed,
                                                            rl->EffectiveGravity(),
                                                            res);
#endif
#ifdef DEBUG
            //.				if(canfire0>0)DBG_DrawLine(p1,Fvector().add(p1,res[0]),D3DCOLOR_XRGB(0,255,0));
            //.				if(canfire0>1)DBG_DrawLine(p1,Fvector().add(p1,res[1]),D3DCOLOR_XRGB(0,0,255));
            //.				DBG_ClosedCashedDraw(30000);
#endif

            if (canfire0 != 0)
            {
                d = res[0];
            };
        }
    };

    d.normalize();
#ifdef CROCKETLAUNCHER_CHANGE
    d.mul(launch_speed);
#else
    d.mul(rl->m_fLaunchSpeed);
#endif
    VERIFY2(_valid(launch_matrix), "CWeaponMagazinedWGrenade::SwitchState. Invalid launch_matrix!");
    rl->LaunchRocket(launch_matrix, d, zero_vel);

    CExplosiveRocket* pGrenade = smart_cast<CExplosiveRocket*>(rl->getCurrentRocket());
    VERIFY(pGrenade);
    pGrenade->SetInitiator(wpn->H_Parent()->ID());

    if (wpn->Local() && OnServer())
    {
        NET_Packet P;
        wpn->u_EventGen(P, GE_LAUNCH_ROCKET, wpn->ID());
        P.w_u16(rl->getCurrentRocket()->ID());
        wpn->u_EventSend(P);
    };
}