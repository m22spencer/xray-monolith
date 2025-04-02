#include "stdafx.h"
#include "WeaponStatMgun.h"
#include "../Include/xrRender/Kinematics.h"
#include "../xrphysics/PhysicsShell.h"
#include "weaponAmmo.h"
#include "object_broker.h"
#include "ai_sounds.h"
#include "actor.h"
#include "actorEffector.h"
#include "camerafirsteye.h"
#include "xr_level_controller.h"
#include "game_object_space.h"
#include "level.h"

#ifdef STATIONARYMGUN_NEW
#include "ai_monster_space.h"
#include "stalker_movement_manager_smart_cover.h"
#include "ai/stalker/ai_stalker.h"
#include "../xrEngine/gamemtllib.h"
#include "cameralook.h"
#include "HUDManager.h"
#endif

void CWeaponStatMgun::BoneCallbackX(CBoneInstance* B)
{
	CWeaponStatMgun* P = static_cast<CWeaponStatMgun*>(B->callback_param());
	Fmatrix rX;
	rX.rotateX(P->m_cur_x_rot);
	B->mTransform.mulB_43(rX);
}

void CWeaponStatMgun::BoneCallbackY(CBoneInstance* B)
{
	CWeaponStatMgun* P = static_cast<CWeaponStatMgun*>(B->callback_param());
	Fmatrix rY;
	rY.rotateY(P->m_cur_y_rot);
	B->mTransform.mulB_43(rY);
}

CWeaponStatMgun::CWeaponStatMgun()
{
	m_firing_disabled = false;
	m_Ammo = xr_new<CCartridge>();

#ifdef STATIONARYMGUN_NEW
	camera[eCamFirst] = xr_new<CCameraFirstEye>(this, CCameraBase::flRelativeLink | CCameraBase::flPositionRigid);
	camera[eCamFirst]->tag = eCamFirst;
	camera[eCamFirst]->Load("mounted_weapon_cam");

	camera[eCamChase] = xr_new<CCameraLook>(this);
	camera[eCamChase]->tag = eCamChase;
	camera[eCamChase]->Load("actor_look_cam");

	OnCameraChange(eCamFirst);
#else
	camera = xr_new<CCameraFirstEye>(
		this, CCameraBase::flRelativeLink | CCameraBase::flPositionRigid | CCameraBase::flDirectionRigid);
	camera->Load("mounted_weapon_cam");
#endif

	p_overheat = NULL;

#ifdef STATIONARYMGUN_NEW
	m_min_gun_speed = 0.0F;
	m_max_gun_speed = 0.0F;
	m_drop_bone = BI_NONE;
	m_desire_angle.set(0.0F, 0.0F);
	m_desire_angle_enable = false;

	m_actor_bone = BI_NONE;
	m_exit_position.set(0, 0, 0);

	m_camera_bone_def = BI_NONE;
	m_camera_bone_aim = BI_NONE;
	m_zoom_factor_def = 1.0F;
	m_zoom_factor_aim = 1.0F;
	m_zoom_status = false;

	m_reload_delay = 0.0F;
	m_unload_delay = 0.0F;
	m_bAutoSpawnAmmo = TRUE;
	m_single_shot_wpn = FALSE;
	m_unlimited_ammo = true;
	m_reload_consume_callback = NULL;
	m_ammoType = 0;
	m_ammoTypes.clear();
	iMagazineSize = 1;

	m_on_before_use_callback = NULL;
#endif
}

CWeaponStatMgun::~CWeaponStatMgun()
{
	delete_data(m_Ammo);
#ifdef STATIONARYMGUN_NEW
	xr_delete(camera[eCamFirst]);
	xr_delete(camera[eCamChase]);
#else
	xr_delete(camera);
#endif
}

void CWeaponStatMgun::SetBoneCallbacks()
{
	//m_pPhysicsShell->EnabledCallbacks(FALSE);
#ifdef STATIONARYMGUN_NEW
	/* After attaching, make the gun start at current horizontal rotation but set vertical to 0. */
	Fvector vec;
	Visual()->dcast_PKinematics()->LL_GetTransform(m_rotate_y_bone).getHPB(vec);
	m_cur_x_rot = 0.0F;
	m_cur_y_rot = -vec.x;
#endif

	CBoneInstance& biX = smart_cast<IKinematics*>(Visual())->LL_GetBoneInstance(m_rotate_x_bone);
	biX.set_callback(bctCustom, BoneCallbackX, this);
	CBoneInstance& biY = smart_cast<IKinematics*>(Visual())->LL_GetBoneInstance(m_rotate_y_bone);
	biY.set_callback(bctCustom, BoneCallbackY, this);
}

void CWeaponStatMgun::ResetBoneCallbacks()
{
	CBoneInstance& biX = smart_cast<IKinematics*>(Visual())->LL_GetBoneInstance(m_rotate_x_bone);
#ifdef STATIONARYMGUN_NEW
	biX.set_callback(bctPhysics, PPhysicsShell()->GetBonesCallback(), PPhysicsShell()->get_Element(m_rotate_x_bone));
#else
	biX.reset_callback();
#endif

	CBoneInstance& biY = smart_cast<IKinematics*>(Visual())->LL_GetBoneInstance(m_rotate_y_bone);
#ifdef STATIONARYMGUN_NEW
	biY.set_callback(bctPhysics, PPhysicsShell()->GetBonesCallback(), PPhysicsShell()->get_Element(m_rotate_y_bone));
#else
	biY.reset_callback();
#endif

#ifdef STATIONARYMGUN_NEW
	biX.mTransform.mulB_43(Fmatrix().rotateX(m_cur_x_rot));
	biY.mTransform.mulB_43(Fmatrix().rotateX(m_cur_y_rot));
#endif

	//m_pPhysicsShell->EnabledCallbacks(TRUE);
}

void CWeaponStatMgun::Load(LPCSTR section)
{
	inheritedPH::Load(section);
	inheritedShooting::Load(section);

	m_sounds.LoadSound(section, "snd_shoot", "sndShot", false, SOUND_TYPE_WEAPON_SHOOTING);

	m_Ammo->Load(pSettings->r_string(section, "ammo_class"), 0);
	camMaxAngle = pSettings->r_float(section, "cam_max_angle");
	camMaxAngle = _abs(deg2rad(camMaxAngle));
	camRelaxSpeed = pSettings->r_float(section, "cam_relax_speed");
	camRelaxSpeed = _abs(deg2rad(camRelaxSpeed));

	m_overheat_enabled = pSettings->line_exist(section, "overheat_enabled")
		                     ? !!pSettings->r_bool(section, "overheat_enabled")
		                     : false;
	m_overheat_time_quant = READ_IF_EXISTS(pSettings, r_float, section, "overheat_time_quant", 0.025f);
	m_overheat_decr_quant = READ_IF_EXISTS(pSettings, r_float, section, "overheat_decr_quant", 0.002f);
	m_overheat_threshold = READ_IF_EXISTS(pSettings, r_float, section, "overheat_threshold", 110.f);
	m_overheat_particles = READ_IF_EXISTS(pSettings, r_string, section, "overheat_particles",
	                                      "damage_fx\\burn_creatures00");

	m_bEnterLocked = !!READ_IF_EXISTS(pSettings, r_bool, section, "lock_enter", false);
	m_bExitLocked = !!READ_IF_EXISTS(pSettings, r_bool, section, "lock_exit", false);

	VERIFY(!fis_zero(camMaxAngle));
	VERIFY(!fis_zero(camRelaxSpeed));

#ifdef STATIONARYMGUN_NEW
	m_sounds.LoadSound(section, "snd_shoot", "sndShoot", false, SOUND_TYPE_WEAPON_SHOOTING);
	m_sounds.LoadSound(section, "snd_empty", "sndEmpty", false, SOUND_TYPE_WEAPON_EMPTY_CLICKING);
	m_sounds.LoadSound(section, "snd_reload", "sndReload", true, SOUND_TYPE_WEAPON_RECHARGING);
	m_sounds.LoadSound(section, "snd_unload", "sndUnload", true, SOUND_TYPE_WEAPON_RECHARGING);

	m_reload_delay = READ_IF_EXISTS(pSettings, r_float, section, "reload_delay", 0.0F);
	m_unload_delay = READ_IF_EXISTS(pSettings, r_float, section, "unload_delay", 0.0F);
	m_bAutoSpawnAmmo = !!READ_IF_EXISTS(pSettings, r_bool, section, "auto_spawn_ammo", TRUE);
	m_single_shot_wpn = !!READ_IF_EXISTS(pSettings, r_bool, section, "is_single_shot_wpn", FALSE);
	m_unlimited_ammo = !!READ_IF_EXISTS(pSettings, r_bool, section, "unlimited_ammo", false);
	m_reload_consume_callback = READ_IF_EXISTS(pSettings, r_string, section, "reload_consume", NULL);

	m_ammoTypes.clear();
	LPCSTR ammo_class = pSettings->r_string(section, "ammo_class");
	if (ammo_class && strlen(ammo_class))
	{
		string128 tmp;
		int n = _GetItemCount(ammo_class);
		for (int i = 0; i < n; ++i)
		{
			_GetItem(ammo_class, i, tmp);
			m_ammoTypes.push_back(tmp);
		}
	}
	m_ammoType = 0;
	m_DefaultCartridge.Load(m_ammoTypes[m_ammoType].c_str(), m_ammoType);

	iMagazineSize = READ_IF_EXISTS(pSettings, r_s32, section, "ammo_mag_size", 1);
	SetAmmoElapsed(READ_IF_EXISTS(pSettings, r_s32, section, "ammo_elapsed", iMagazineSize));

	if (pSettings->line_exist(cNameSect_str(), "on_before_use"))
	{
		m_on_before_use_callback = READ_IF_EXISTS(pSettings, r_string, cNameSect_str(), "on_before_use", "");
	}
#endif
}

BOOL CWeaponStatMgun::net_Spawn(CSE_Abstract* DC)
{
	if (!inheritedPH::net_Spawn(DC)) return FALSE;

	IKinematics* K = smart_cast<IKinematics*>(Visual());
	CInifile* pUserData = K->LL_UserData();

	R_ASSERT2(pUserData, "Empty WeaponStatMgun user data!");

	m_rotate_x_bone = K->LL_BoneID(pUserData->r_string("mounted_weapon_definition", "rotate_x_bone"));
	m_rotate_y_bone = K->LL_BoneID(pUserData->r_string("mounted_weapon_definition", "rotate_y_bone"));
	m_fire_bone = K->LL_BoneID(pUserData->r_string("mounted_weapon_definition", "fire_bone"));
	m_camera_bone = K->LL_BoneID(pUserData->r_string("mounted_weapon_definition", "camera_bone"));

#ifdef STATIONARYMGUN_NEW
	CPHSkeleton::Spawn(DC);
#else
	U16Vec fixed_bones;
	fixed_bones.push_back(K->LL_GetBoneRoot());
	PPhysicsShell() = P_build_Shell(this, false, fixed_bones);
#endif

	CBoneData& bdX = K->LL_GetData(m_rotate_x_bone);
	VERIFY(bdX.IK_data.type==jtJoint);
	m_lim_x_rot.set(bdX.IK_data.limits[0].limit.x, bdX.IK_data.limits[0].limit.y);
	CBoneData& bdY = K->LL_GetData(m_rotate_y_bone);
	VERIFY(bdY.IK_data.type==jtJoint);
	m_lim_y_rot.set(bdY.IK_data.limits[1].limit.x, bdY.IK_data.limits[1].limit.y);


	xr_vector<Fmatrix> matrices;
	K->LL_GetBindTransform(matrices);
	m_i_bind_x_xform.invert(matrices[m_rotate_x_bone]);
	m_i_bind_y_xform.invert(matrices[m_rotate_y_bone]);
	m_bind_x_rot = matrices[m_rotate_x_bone].k.getP();
	m_bind_y_rot = matrices[m_rotate_y_bone].k.getH();
	m_bind_x.set(matrices[m_rotate_x_bone].c);
	m_bind_y.set(matrices[m_rotate_y_bone].c);

#ifdef STATIONARYMGUN_NEW
	/*
	Initial directions are always H = 0 P = 0. Even when the bones were created with angled directions.
	Example: When a bone is created with H = 60 degrees, the bone has a H = 60 degrees angle to the model.
	But it is H = 0 degree to the bone itself.
	*/
	m_cur_x_rot = 0;
	m_cur_y_rot = 0;
#else
	m_cur_x_rot = m_bind_x_rot;
	m_cur_y_rot = m_bind_y_rot;
#endif

	m_destEnemyDir.setHP(m_bind_y_rot, m_bind_x_rot);
	XFORM().transform_dir(m_destEnemyDir);

#ifdef STATIONARYMGUN_NEW
	CInifile *ini = K->LL_UserData();
	LPCSTR mwd = "mounted_weapon_definition";

	{
		LPCSTR str = READ_IF_EXISTS(pSettings, r_string, cNameSect_str(), "traverse_lim_vert", NULL);
		if (str && (_GetItemCount(str) >= 2))
		{
			string16 tmp;
			_GetItem(str, 0, tmp);
			m_lim_x_rot.y = max(deg2rad(-std::stof(tmp)), 0.0F);
			_GetItem(str, 1, tmp);
			m_lim_x_rot.x = min(deg2rad(-std::stof(tmp)), 0.0F);
		}
	}
	{
		LPCSTR str = READ_IF_EXISTS(pSettings, r_string, cNameSect_str(), "traverse_lim_horz", NULL);
		if (str && (_GetItemCount(str) >= 2))
		{
			string16 tmp;
			_GetItem(str, 0, tmp);
			m_lim_y_rot.y = max(deg2rad(-std::stof(tmp)), 0.0F);
			_GetItem(str, 1, tmp);
			m_lim_y_rot.x = min(deg2rad(-std::stof(tmp)), 0.0F);
		}
	}

	m_min_gun_speed = deg2rad(READ_IF_EXISTS(ini, r_float, mwd, "min_gun_speed", 10.0F));
	m_max_gun_speed = deg2rad(READ_IF_EXISTS(ini, r_float, mwd, "max_gun_speed", 10.0F));
	m_drop_bone = ini->line_exist(mwd, "drop_bone") ? K->LL_BoneID(ini->r_string(mwd, "drop_bone")) : BI_NONE;

	m_actor_bone = ini->line_exist(mwd, "actor_bone") ? K->LL_BoneID(ini->r_string(mwd, "actor_bone")) : BI_NONE;

	m_exit_position = READ_IF_EXISTS(ini, r_fvector3, mwd, "exit_position", Fvector().set(0, 0, 0));

	m_camera_bone_def = ini->line_exist(mwd, "camera_bone_def") ? K->LL_BoneID(ini->r_string(mwd, "camera_bone_def")) : BI_NONE;
	m_camera_bone_aim = ini->line_exist(mwd, "camera_bone_aim") ? K->LL_BoneID(ini->r_string(mwd, "camera_bone_aim")) : BI_NONE;
	m_zoom_factor_def = READ_IF_EXISTS(ini, r_float, mwd, "zoom_factor_def", 1.0F);
	m_zoom_factor_aim = READ_IF_EXISTS(ini, r_float, mwd, "zoom_factor_aim", 1.0F);

	if (ini->section_exist("animation"))
	{
		m_animation.m_anim_body = READ_IF_EXISTS(ini, r_string, "animation", "anim_body", m_animation.m_anim_body);
		m_animation.m_anim_legs = READ_IF_EXISTS(ini, r_string, "animation", "anim_legs", m_animation.m_anim_legs);
	}

	LPCSTR custom_cam_first = READ_IF_EXISTS(ini, r_string, "camera", "cam_first", NULL);
	if (custom_cam_first && pSettings->section_exist(custom_cam_first))
	{
		camera[eCamFirst]->Load(custom_cam_first);
	}

	LPCSTR custom_cam_chase = READ_IF_EXISTS(ini, r_string, "camera", "cam_chase", NULL);
	if (custom_cam_chase && pSettings->section_exist(custom_cam_chase))
	{
		camera[eCamChase]->Load(custom_cam_chase);
	}
#endif

	inheritedShooting::Light_Create();

	processing_activate();
	setVisible(TRUE);
	setEnabled(TRUE);

#ifdef STATIONARYMGUN_NEW
	PPhysicsShell()->Enable();
	PPhysicsShell()->add_ObjectContactCallback(IgnoreOwnerCallback);

	if (PPhysicsShell() && m_ignore_collision_flag)
	{
		CPhysicsShellHolder::active_ignore_collision();
	}
#endif

	return TRUE;
}

void CWeaponStatMgun::net_Destroy()
{
	if (p_overheat)
	{
		if (p_overheat->IsPlaying())
			p_overheat->Stop(FALSE);
		CParticlesObject::Destroy(p_overheat);
	}

#ifdef STATIONARYMGUN_NEW
	if (Owner())
	{
#ifdef HOLDERCUSTOM_NEW
		if (Owner()->cast_stalker() && !Owner()->cast_stalker()->g_Alive())
		{
			Owner()->cast_stalker()->detach_Holder();
			return;
		}
#endif
	}

	PPhysicsShell()->remove_ObjectContactCallback(IgnoreOwnerCallback);
	ResetBoneCallbacks();
	CPHUpdateObject::Deactivate();
	CPHSkeleton::RespawnInit();
#endif

	inheritedPH::net_Destroy();
	processing_deactivate();
}

void CWeaponStatMgun::net_Export(NET_Packet& P) // export to server
{
	inheritedPH::net_Export(P);
	P.w_u8(IsWorking() ? 1 : 0);
	save_data(m_destEnemyDir, P);

#ifdef STATIONARYMGUN_NEW
	P.w_u8(m_ammoType);
	P.w_u16(u16(iAmmoElapsed & 0xffff));
#endif
}

void CWeaponStatMgun::net_Import(NET_Packet& P) // import from server
{
	inheritedPH::net_Import(P);
	u8 state = P.r_u8();
	load_data(m_destEnemyDir, P);

	if (TRUE == IsWorking() && !state) FireEnd();
	if (FALSE == IsWorking() && state) FireStart();

#ifdef STATIONARYMGUN_NEW
	SetAmmoType(P.r_u8());
	SetAmmoElapsed(P.r_u16());
#endif
}

void CWeaponStatMgun::UpdateCL()
{
	inheritedPH::UpdateCL();

#ifdef STATIONARYMGUN_NEW
	if (OwnerActor())
	{
		/* Actor will call UpdateEx() to update machine gun instead.
		Doing this to keep weapon rotation and actor camera match each others. */
		return;
	}
	UpdateEx(90.0f);
#else
	UpdateBarrelDir();
	UpdateFire();

	if (OwnerActor() && OwnerActor()->IsMyCamera())
	{
		cam_Update(Device.fTimeDelta, g_fov);
		OwnerActor()->Cameras().UpdateFromCamera(Camera());
		OwnerActor()->Cameras().ApplyDevice(VIEWPORT_NEAR);
	}
#endif
}

#ifdef STATIONARYMGUN_NEW
void CWeaponStatMgun::UpdateEx(float fov)
{
	if (PPhysicsShell())
	{
		PPhysicsShell()->InterpolateGlobalTransform(&XFORM());
		Visual()->dcast_PKinematics()->CalculateBones();
	}

	UpdateBarrelDir();
	UpdateFire();

	if (!Owner())
		return;

#ifdef HOLDERCUSTOM_NEW
	/* Dead owner. Kick out. Just in case. We kick them out when on death callback already. */
	if (Owner()->cast_stalker() && !Owner()->cast_stalker()->g_Alive())
	{
		Owner()->cast_stalker()->detach_Holder();
		return;
	}
#endif

	/* Update owner position. */
	if (m_actor_bone != BI_NONE)
	{
		IKinematics *K = Visual()->dcast_PKinematics();
		Fmatrix xform = Fmatrix(K->LL_GetTransform(m_actor_bone));
		Owner()->XFORM().set(Fmatrix().mul_43(XFORM(), xform));
	}
	else
	{
		/* Should not let this happen. It indicates that there is no actor bone or it is invalid. Should fix. */
		Owner()->XFORM().set(XFORM());
	}

	/* Always update camera last. */
	if (OwnerActor() && OwnerActor()->IsMyCamera())
	{
		cam_Update(Device.fTimeDelta, g_fov);
		OwnerActor()->Cameras().UpdateFromCamera(Camera());
		OwnerActor()->Cameras().ApplyDevice(R_VIEWPORT_NEAR);

		OwnerActor()->Orientation().yaw = 0;
		OwnerActor()->Orientation().pitch = 0;

		if (IsCameraZoom())
		{
			SetParam(eWpnDesiredDir, Camera()->Direction());
		}
		else
		{
			Fvector pos = Camera()->Position();
			Fvector dir = Camera()->Direction();
			collide::rq_result &R = HUD().GetCurrentRayQuery();
			Fvector vec = Fvector().mad(pos, dir, (R.range > 3.f) ? R.range : 30.f);
			SetParam(eWpnDesiredDir, Fvector().sub(vec, m_fire_pos).normalize());
		}
	}
}
#endif

//void CWeaponStatMgun::Hit(	float P, Fvector &dir,	CObject* who, 
//							s16 element,Fvector p_in_object_space, 
//							float impulse, ALife::EHitType hit_type)
void CWeaponStatMgun::Hit(SHit* pHDS)
{
#ifdef STATIONARYMGUN_NEW
	inheritedPH::Hit(pHDS);
#else
	if (NULL == Owner())
		//		inheritedPH::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
		inheritedPH::Hit(pHDS);
#endif
}

void CWeaponStatMgun::UpdateBarrelDir()
{
	IKinematics* K = smart_cast<IKinematics*>(Visual());
	m_fire_bone_xform = K->LL_GetTransform(m_fire_bone);

	m_fire_bone_xform.mulA_43(XFORM());
	m_fire_pos.set(0, 0, 0);
	m_fire_bone_xform.transform_tiny(m_fire_pos);
	m_fire_dir.set(0, 0, 1);
	m_fire_bone_xform.transform_dir(m_fire_dir);

	m_allow_fire = true;
	Fmatrix XFi;
	XFi.invert(XFORM());
	Fvector dep;

#ifdef STATIONARYMGUN_NEW
	if (!IsActive())
		return;
	/*
		Take m_i_bind_y_xform as base for both bone x and bone_rotate_y assuming bone_rotate_y always has 0 pitch.
		And reset dep before calculating.
	*/
	{
		// x angle
		if (m_desire_angle_enable)
		{
			dep.setHP(m_desire_angle.x, m_desire_angle.y);
		}
		else
		{
			XFi.transform_dir(dep, m_destEnemyDir);
		}
		m_i_bind_y_xform.transform_dir(dep);
		dep.normalize();
		m_tgt_x_rot = angle_normalize_signed(m_bind_x_rot - dep.getP());
		clamp(m_tgt_x_rot, -m_lim_x_rot.y, -m_lim_x_rot.x);
	}
	{
		// y angle
		if (m_desire_angle_enable)
		{
			dep.setHP(m_desire_angle.x, m_desire_angle.y);
		}
		else
		{
			XFi.transform_dir(dep, m_destEnemyDir);
		}
		m_i_bind_y_xform.transform_dir(dep);
		dep.normalize();
		m_tgt_y_rot = angle_normalize_signed(m_bind_y_rot - dep.getH());

		if (abs(m_lim_y_rot.x) < PI || abs(m_lim_y_rot.y) < PI)
		{
			/* Target is outside of rotating limit. Decide which limit to head toward. */
			if (m_tgt_y_rot < -m_lim_y_rot.y || m_tgt_y_rot > -m_lim_y_rot.x)
			{
				if (angle_difference(m_tgt_y_rot, -m_lim_y_rot.y) < angle_difference(m_tgt_y_rot, -m_lim_y_rot.x))
					m_tgt_y_rot = -m_lim_y_rot.y;
				else
					m_tgt_y_rot = -m_lim_y_rot.x;
			}
			/* If horizontal rotation crosses 180 degrees in the back, make it swings around 0. */
			if (abs(m_tgt_y_rot - m_cur_y_rot) >= PI)
			{
				m_tgt_y_rot = 0.0F;
			}
		}
	}

	m_cur_x_rot = angle_inertion_var(m_cur_x_rot, m_tgt_x_rot, m_min_gun_speed, m_max_gun_speed, PI, Device.fTimeDelta);
	m_cur_y_rot = angle_inertion_var(m_cur_y_rot, m_tgt_y_rot, m_min_gun_speed, m_max_gun_speed, PI, Device.fTimeDelta);
	static float dir_eps = deg2rad(5.0f);
	if (!fsimilar(m_cur_x_rot, m_tgt_x_rot, dir_eps) || !fsimilar(m_cur_y_rot, m_tgt_y_rot, dir_eps))
	{
		m_allow_fire = FALSE;
	}
#else
	XFi.transform_dir(dep, m_destEnemyDir);
	{
		// x angle
		m_i_bind_x_xform.transform_dir(dep);
		dep.normalize();
		m_tgt_x_rot = angle_normalize_signed(m_bind_x_rot - dep.getP());
		float sv_x = m_tgt_x_rot;

		clamp(m_tgt_x_rot, -m_lim_x_rot.y, -m_lim_x_rot.x);
		if (!fsimilar(sv_x, m_tgt_x_rot, EPS_L)) m_allow_fire = FALSE;
	}
	{
		// y angle
		m_i_bind_y_xform.transform_dir(dep);
		dep.normalize();
		m_tgt_y_rot = angle_normalize_signed(m_bind_y_rot - dep.getH());
		float sv_y = m_tgt_y_rot;
		clamp(m_tgt_y_rot, -m_lim_y_rot.y, -m_lim_y_rot.x);
		if (!fsimilar(sv_y, m_tgt_y_rot, EPS_L)) m_allow_fire = FALSE;
	}

	m_cur_x_rot = angle_inertion_var(m_cur_x_rot, m_tgt_x_rot, 0.5f, 3.5f, PI_DIV_6, Device.fTimeDelta);
	m_cur_y_rot = angle_inertion_var(m_cur_y_rot, m_tgt_y_rot, 0.5f, 3.5f, PI_DIV_6, Device.fTimeDelta);
#endif
}

void CWeaponStatMgun::cam_Update(float dt, float fov)
{
#ifdef STATIONARYMGUN_NEW
	Fvector P;
	Fvector D;
	D.set(0, 0, 0);
	CCameraBase *cam = Camera();

	switch (cam->tag)
	{
	case eCamFirst:
	case eCamChase:
	{
		u16 bone_id = m_camera_bone;
		if (m_camera_bone_def != BI_NONE)
		{
			bone_id = (IsCameraZoom() && (m_camera_bone_aim != BI_NONE)) ? m_camera_bone_aim : m_camera_bone_def;
		}

		IKinematics *K = Visual()->dcast_PKinematics();
		Fmatrix xform = K->LL_GetTransform(bone_id);
		XFORM().transform_tiny(P, xform.c);

		if (OwnerActor())
			OwnerActor()->Orientation().yaw = -cam->yaw;
		if (OwnerActor())
			OwnerActor()->Orientation().pitch = -cam->pitch;
		break;
	}
	}

	float zoom_factor = (IsCameraZoom()) ? m_zoom_factor_aim : m_zoom_factor_def;
	cam->f_fov = fov / zoom_factor;
	cam->Update(P, D);
	Level().Cameras().UpdateFromCamera(cam);
#else
	camera->f_fov = g_fov;

	Fvector P, Da;
	Da.set(0, 0, 0);

	IKinematics* K = smart_cast<IKinematics*>(Visual());
	K->CalculateBones_Invalidate();
	K->CalculateBones(TRUE);
	const Fmatrix& C = K->LL_GetTransform(m_camera_bone);
	XFORM().transform_tiny(P, C.c);

	Fvector d = C.k;
	XFORM().transform_dir(d);
	Fvector2 des_cam_dir;

	d.getHP(des_cam_dir.x, des_cam_dir.y);
	des_cam_dir.mul(-1.0f);


	Camera()->yaw = angle_inertion_var(Camera()->yaw, des_cam_dir.x, 0.5f, 7.5f, PI_DIV_6, Device.fTimeDelta);
	Camera()->pitch = angle_inertion_var(Camera()->pitch, des_cam_dir.y, 0.5f, 7.5f, PI_DIV_6, Device.fTimeDelta);


	if (OwnerActor())
	{
		// rotate head
		OwnerActor()->Orientation().yaw = -Camera()->yaw;
		OwnerActor()->Orientation().pitch = -Camera()->pitch;
	}


	Camera()->Update(P, Da);
	Level().Cameras().UpdateFromCamera(Camera());
#endif
}

void CWeaponStatMgun::renderable_Render()
{
	inheritedPH::renderable_Render();

	RenderLight();
}

void CWeaponStatMgun::SetDesiredDir(float h, float p)
{
	m_destEnemyDir.setHP(h, p);
}

void CWeaponStatMgun::Action(u16 id, u32 flags)
{
	inheritedHolder::Action(id, flags);
	switch (id)
	{
#ifdef STATIONARYMGUN_NEW
	case eWpnFire:
		if (flags == 1 && GetState() == eStateIdle)
		{
			SwitchState(eStateFire);
		}
		if (flags != 1 && GetState() == eStateFire)
		{
			SwitchState(eStateIdle);
		}
		break;
	case eWpnActivate:
		if (flags == 1 && m_bActive != true)
		{
			m_bActive = true;
			SwitchState(eStateIdle);
			SetBoneCallbacks();
		}
		if (flags != 1 && m_bActive == true)
		{
			m_bActive = false;
			SwitchState(eStateIdle);
			ResetBoneCallbacks();
		}
		break;
	case eWpnReload:
		if (GetState() == eStateIdle || GetState() == eStateFire)
		{
			SwitchState(eStateReload);
		}
		break;
	case eWpnUnload:
		if (GetState() == eStateIdle || GetState() == eStateFire)
		{
			SwitchState(eStateUnload);
		}
		break;
#else
	case kWPN_FIRE:
		{
			if (flags == CMD_START) FireStart();
			else FireEnd();
		}
		break;
#endif
	}
}

void CWeaponStatMgun::SetParam(int id, Fvector2 val)
{
	inheritedHolder::SetParam(id, val);
	switch (id)
	{
	case DESIRED_DIR:
		SetDesiredDir(val.x, val.y);
		break;
	}
}

#ifdef STATIONARYMGUN_NEW
void CWeaponStatMgun::SetParam(int id, Fvector val)
{
	inheritedHolder::SetParam(id, val);
	switch (id)
	{
	case eWpnDesiredPos:
		Fvector vec = Fmatrix().mul_43(XFORM(), Visual()->dcast_PKinematics()->LL_GetTransform(m_rotate_y_bone)).c;
		m_destEnemyDir.sub(val, vec).normalize_safe();
		m_desire_angle_enable = false;
		break;
	case eWpnDesiredDir:
		m_destEnemyDir.set(val).normalize_safe();
		m_desire_angle_enable = false;
		break;
	case eWpnDesiredAng:
		m_desire_angle.set(val.x, val.y);
		m_desire_angle_enable = true;
		break;
	}
}
#endif

bool CWeaponStatMgun::attach_Actor(CGameObject* actor)
{
#ifdef STATIONARYMGUN_NEW
	if (Owner())
		return false;

	if (!actor)
		return false;

	inheritedHolder::attach_Actor(actor);
	Action(eWpnActivate, 1);

	if (OwnerActor())
	{
		OnCameraChange(eCamFirst);
		Camera()->pitch = 0.0F;
		Camera()->yaw = m_cur_y_rot;
	}
	return true;
#else
	if (Owner())
		return false;
	actor->setVisible(0);
	inheritedHolder::attach_Actor(actor);
	SetBoneCallbacks();
	FireEnd();
	return true;
#endif
}

void CWeaponStatMgun::detach_Actor()
{
#ifdef STATIONARYMGUN_NEW
	if (!Owner())
		return;

	if (OwnerActor())
	{
		OwnerActor()->setVisible(TRUE);
	}
	inheritedHolder::detach_Actor();
	Action(eWpnActivate, 0);
#else
	Owner()->setVisible(1);
	inheritedHolder::detach_Actor();
	ResetBoneCallbacks();
	FireEnd();
#endif
}

Fvector CWeaponStatMgun::ExitPosition()
{
#ifdef STATIONARYMGUN_NEW
	Fvector vec;
	XFORM().transform_tiny(vec, m_exit_position);
	return vec;
#else
	Fvector pos; pos.set(0.f, 0.f, 0.f);
	pos.sub(camera->Direction()).normalize();
	pos.y = 0.f;
	return Fvector(XFORM().c).add(pos);
#endif
}

#ifdef STATIONARYMGUN_NEW
void CWeaponStatMgun::SpawnInitPhysics(CSE_Abstract *D)
{
	CSE_PHSkeleton *so = smart_cast<CSE_PHSkeleton *>(D);
	R_ASSERT(so);
	IKinematics *K = Visual()->dcast_PKinematics();
	CreateSkeleton(D);
	K->CalculateBones_Invalidate();
	K->CalculateBones(TRUE);
	CPHUpdateObject::Activate();
	PPhysicsShell()->applyImpulse(Fvector().set(0.0F, -1.0F, 0.0F), 0.1F);
}

void CWeaponStatMgun::CreateSkeleton(CSE_Abstract *po)
{
	if (!Visual())
		return;
	IKinematics *K = Visual()->dcast_PKinematics();
	phys_shell_verify_object_model(*this);
	U16Vec fixed_bones;
	fixed_bones.push_back(K->LL_GetBoneRoot());
	m_pPhysicsShell = P_build_Shell(this, false, fixed_bones);
	m_pPhysicsShell->SetPrefereExactIntegration();
	ApplySpawnIniToPhysicShell(&po->spawn_ini(), m_pPhysicsShell, fixed_bones.size() > 0);
	ApplySpawnIniToPhysicShell(K->LL_UserData(), m_pPhysicsShell, fixed_bones.size() > 0);
}

void CWeaponStatMgun::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	CPHSkeleton::Update(dt);
}

void CWeaponStatMgun::net_Save(NET_Packet &P)
{
	inherited::net_Save(P);
	CPHSkeleton::SaveNetState(P);
}

BOOL CWeaponStatMgun::net_SaveRelevant()
{
	return TRUE;
}

void CWeaponStatMgun::SaveNetState(NET_Packet &P)
{
	CPHSkeleton::SaveNetState(P);
}

BOOL CWeaponStatMgun::AlwaysTheCrow()
{
	return TRUE;
}

bool CWeaponStatMgun::is_ai_obstacle() const
{
	return true;
}

void CWeaponStatMgun::IgnoreOwnerCallback(bool &do_colide, bool bo1, dContact &c, SGameMtl *mt1, SGameMtl *mt2)
{
	if (do_colide == false)
		return;

	dxGeomUserData *gd1 = bo1 ? PHRetrieveGeomUserData(c.geom.g1) : PHRetrieveGeomUserData(c.geom.g2);
	dxGeomUserData *gd2 = bo1 ? PHRetrieveGeomUserData(c.geom.g2) : PHRetrieveGeomUserData(c.geom.g1);
	CGameObject *obj = (gd1) ? smart_cast<CGameObject *>(gd1->ph_ref_object) : nullptr;
	CGameObject *who = (gd2) ? smart_cast<CGameObject *>(gd2->ph_ref_object) : nullptr;
	if (!obj || !who)
	{
		return;
	}

	CWeaponStatMgun *stm = smart_cast<CWeaponStatMgun *>(obj);
	CGameObject *owner = (stm) ? stm->Owner() : nullptr;
	if (owner && (owner->ID() == who->ID()))
	{
		do_colide = false;
	}
}

bool CWeaponStatMgun::Use(const Fvector &pos, const Fvector &dir, const Fvector &foot_pos)
{
	if (Owner())
		return false;

	if (m_on_before_use_callback && strlen(m_on_before_use_callback))
	{
		luabind::functor<bool> lua_function;
		if (ai().script_engine().functor(m_on_before_use_callback, lua_function))
		{
			if (!lua_function(lua_game_object(), pos, dir, foot_pos))
			{
				return false;
			}
		}
	}
	return true;
}

void CWeaponStatMgun::OnCameraChange(u16 type)
{
	if (OwnerActor())
	{
		if (type == eCamFirst)
		{
			Owner()->setVisible(FALSE);
		}
		else
		{
			Owner()->setVisible(TRUE);
		}
	}

	if (active_camera == NULL)
	{
		active_camera = camera[type];
		return;
	}

	if (active_camera->tag != type)
	{
		CCameraBase *cam = camera[type];
		cam->pitch = active_camera->pitch;
		cam->yaw = active_camera->yaw;
		active_camera = camera[type];
	}
}

void CWeaponStatMgun::PlayAnimation()
{
	if (Owner())
	{
		if (OwnerActor())
		{
			OwnerActor()->Visual()->dcast_PKinematicsAnimated()->PlayCycle(m_animation.m_anim_body);
			OwnerActor()->Visual()->dcast_PKinematicsAnimated()->PlayCycle(m_animation.m_anim_legs);
			OwnerActor()->CStepManager::on_animation_start(MotionID(), 0);
		}

		CAI_Stalker *stalker = Owner()->cast_stalker();
		if (stalker)
		{
			stalker->Visual()->dcast_PKinematicsAnimated()->PlayCycle(m_animation.m_anim_body);
			stalker->Visual()->dcast_PKinematicsAnimated()->PlayCycle(m_animation.m_anim_legs);
			stalker->CStepManager::on_animation_start(MotionID(), 0);

			stalker->movement().set_desired_direction(0);
			if (stalker->best_weapon())
				stalker->CObjectHandler::set_goal(eObjectActionStrapped, stalker->best_weapon());
			else
				stalker->CObjectHandler::set_goal(eObjectActionIdle);
		}
	}
}

bool CWeaponStatMgun::InFieldOfView(Fvector pos)
{
	Fvector vec;
	Fmatrix().invert(XFORM()).transform_tiny(vec, pos);
	Fvector dir = Fvector().sub(vec, Visual()->dcast_PKinematics()->LL_GetTransform(m_rotate_y_bone).c).normalize();
	float h = dir.getH();
	float p = dir.getP();
	if (h < -m_lim_y_rot.y || -m_lim_y_rot.x < h)
	{
		return false;
	}
	if (p < -m_lim_x_rot.y || -m_lim_x_rot.x < p)
	{
		return false;
	}
	return true;
}

/*----------------------------------------------------------------------------------------------------
	Structs
----------------------------------------------------------------------------------------------------*/
SStmAnimation::SStmAnimation()
{
	m_anim_body = "norm_torso_m134_aim_0";
	m_anim_legs = "norm_idle_m134";
}
#endif