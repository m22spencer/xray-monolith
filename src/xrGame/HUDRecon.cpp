// HUDRecon.cpp: Distance and identification readout
// 
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "HUDRecon.h"

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

u32 C_ON_ENEMY D3DCOLOR_RGBA(0xff, 0, 0, 0x80);
u32 C_ON_NEUTRAL D3DCOLOR_RGBA(0xff, 0xff, 0x80, 0x80);
u32 C_ON_FRIEND D3DCOLOR_RGBA(0, 0xff, 0, 0x80);

#define SHOW_INFO_SPEED		0.5f
#define HIDE_INFO_SPEED		10.f

static float recon_mindist()
{
	return 2.f;
}

static float recon_maxdist()
{
	return 50.f;
}

static float recon_minspeed()
{
	return 0.5f;
}

static float recon_maxspeed()
{
	return 10.f;
}

CHUDRecon::CHUDRecon()
{
	fuzzyShowInfo = 0.f;
}

CHUDRecon::~CHUDRecon()
{
}

void CHUDRecon::SetTransform(const Fmatrix& m)
{
	transform.set(m);
}

void CHUDRecon::SetColor(u32 c)
{
	color = c;
}

void CHUDRecon::OnRender(float result_dist)
{
	Fvector4 pt = Fvector4();
	if (HUD().FireposActive())
	{
		Device.mFullTransform.transform(pt, transform.c);
		pt.y = -pt.y;
	}

	// Readout font
	CGameFont* F = UI().Font().pFontGraffiti19Russian;
	F->SetAligment(CGameFont::alCenter);
	F->OutSetI(pt.x, pt.y + 0.05f);

	if (psHUD_Flags.test(HUD_CROSSHAIR_DIST))
		F->OutSkip();

	if (psHUD_Flags.test(HUD_INFO))
	{
		const SPickParam& pp = Actor()->GetPick();
		bool const is_poltergeist = pp.result.O && !!smart_cast<CPoltergeist*>(pp.result.O);

		if ((pp.result.O && pp.result.O->getVisible()) || is_poltergeist)
		{
			CEntityAlive* EA = smart_cast<CEntityAlive*>(pp.result.O);
			CEntityAlive* pCurEnt = smart_cast<CEntityAlive*>(Level().CurrentEntity());
			PIItem l_pI = smart_cast<PIItem>(pp.result.O);

			if (IsGameTypeSingle())
			{
				CInventoryOwner* our_inv_owner = smart_cast<CInventoryOwner*>(pCurEnt);

				if (EA && EA->g_Alive() && EA->cast_base_monster())
				{
					color = C_ON_ENEMY;
				}
				else if (EA && EA->g_Alive() && !EA->cast_base_monster())
				{
					CInventoryOwner* others_inv_owner = smart_cast<CInventoryOwner*>(EA);

					if (our_inv_owner && others_inv_owner)
					{
						switch (RELATION_REGISTRY().GetRelationType(others_inv_owner, our_inv_owner))
						{
						case ALife::eRelationTypeEnemy:
							color = C_ON_ENEMY;
							break;
						case ALife::eRelationTypeNeutral:
							color = C_ON_NEUTRAL;
							break;
						case ALife::eRelationTypeFriend:
							color = C_ON_FRIEND;
							break;
						}

						if (fuzzyShowInfo > 0.5f)
						{
							CStringTable strtbl;
							F->SetColor(subst_alpha(color, u8(iFloor(255.f * (fuzzyShowInfo - 0.5f) * 2.f))));
							F->OutNext("%s", *strtbl.translate(others_inv_owner->Name()));
							F->OutNext("%s", *strtbl.translate(others_inv_owner->CharacterInfo().Community().id()));
						}
					}

					fuzzyShowInfo += SHOW_INFO_SPEED * Device.fTimeDelta;
				}
				else if (l_pI && our_inv_owner && result_dist < 2.0f * 2.0f)
				{
					if (fuzzyShowInfo > 0.5f && l_pI->NameItem())
					{
						F->SetColor(subst_alpha(color, u8(iFloor(255.f * (fuzzyShowInfo - 0.5f) * 2.f))));
						F->OutNext("%s", l_pI->NameItem());
					}
					fuzzyShowInfo += SHOW_INFO_SPEED * Device.fTimeDelta;
				}
			}
			else
			{
				if (EA && (EA->GetfHealth() > 0))
				{
					if (pCurEnt && GameID() & eGameIDSingle)
					{
						if (GameID() & eGameIDDeathmatch) color = C_ON_ENEMY;
						else
						{
							if (EA->g_Team() != pCurEnt->g_Team()) color = C_ON_ENEMY;
							else color = C_ON_FRIEND;
						};
						if (result_dist >= recon_mindist() && result_dist <= recon_maxdist())
						{
							float ddist = (result_dist - recon_mindist()) / (recon_maxdist() - recon_mindist());
							float dspeed = recon_minspeed() + (recon_maxspeed() - recon_minspeed()) * ddist;
							fuzzyShowInfo += Device.fTimeDelta / dspeed;
						}
						else
						{
							if (result_dist < recon_mindist())
								fuzzyShowInfo += recon_minspeed() * Device.fTimeDelta;
							else
								fuzzyShowInfo = 0;
						};

						if (fuzzyShowInfo > 0.5f)
						{
							clamp(fuzzyShowInfo, 0.f, 1.f);
							int alpha_C = iFloor(255.f * (fuzzyShowInfo - 0.5f) * 2.f);
							u8 alpha_b = u8(alpha_C & 0x00ff);
							F->SetColor(subst_alpha(color, alpha_b));
							F->OutNext("%s", *pp.result.O->cName());
						}
					}
				};
			};
		}
		else
		{
			fuzzyShowInfo -= HIDE_INFO_SPEED * Device.fTimeDelta;
		}
		clamp(fuzzyShowInfo, 0.f, 1.f);
	}

	if (psHUD_Flags.test(HUD_CROSSHAIR_DIST))
	{
		F->OutSetI(pt.x, pt.y + 0.05f);
		F->SetColor(color);
#ifdef DEBUG
		F->OutNext("%4.1f - %4.2f - %d", result_dist, PP.power, PP.pass);
#else
		F->OutNext("%4.1f", result_dist);
#endif
	}
}