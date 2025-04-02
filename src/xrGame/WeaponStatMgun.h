#pragma once

#ifdef STATIONARYMGUN_NEW
#include "Inventory.h"
#include "WeaponAmmo.h"
#include "script_game_object.h"
#include "string_table.h"
#endif

#include "holder_custom.h"
#include "shootingobject.h"
#include "physicsshellholder.h"
#include "hudsound.h"
class CCartridge;
class CCameraBase;

#define DESIRED_DIR 1

#ifdef STATIONARYMGUN_NEW
class CActor;
class CInventoryOwner;
class CInventory;

struct SStmAnimation
{
	LPCSTR m_anim_body;
	LPCSTR m_anim_legs;
	SStmAnimation();
};
#endif

class CWeaponStatMgun : public CPhysicsShellHolder,
                        public CHolderCustom,
#ifdef STATIONARYMGUN_NEW
						public CPHUpdateObject,
						public CPHSkeleton,
#endif
                        public CShootingObject
{
private:
	typedef CPhysicsShellHolder inheritedPH;
	typedef CHolderCustom inheritedHolder;
	typedef CShootingObject inheritedShooting;

private:
#ifdef STATIONARYMGUN_NEW
	enum EStmCamType{
		eCamFirst = 0,
		eCamChase,
		eCamSize,
	};
	CCameraBase *camera[eCamSize];
	CCameraBase *active_camera;
#else
	CCameraBase* camera;
#endif
	// 
	static void _BCL BoneCallbackX(CBoneInstance* B);
	static void _BCL BoneCallbackY(CBoneInstance* B);
	void SetBoneCallbacks();
	void ResetBoneCallbacks();

	HUD_SOUND_COLLECTION_LAYERED m_sounds;

	//casts
public:
	virtual CHolderCustom* cast_holder_custom() { return this; }

	//general
public:
	CWeaponStatMgun();
	virtual ~CWeaponStatMgun();

	virtual void Load(LPCSTR section);

	virtual BOOL net_Spawn(CSE_Abstract* DC);
	virtual void net_Destroy();
	virtual void net_Export(NET_Packet& P); // export to server
	virtual void net_Import(NET_Packet& P); // import from server

	virtual void UpdateCL();

	virtual void Hit(SHit* pHDS);

	//shooting
private:
	u16 m_rotate_x_bone, m_rotate_y_bone, m_fire_bone, m_camera_bone;
	float m_tgt_x_rot, m_tgt_y_rot, m_cur_x_rot, m_cur_y_rot, m_bind_x_rot, m_bind_y_rot;
	Fvector m_bind_x, m_bind_y;
	Fvector m_fire_dir, m_fire_pos;

	Fmatrix m_i_bind_x_xform, m_i_bind_y_xform, m_fire_bone_xform;
	Fvector2 m_lim_x_rot, m_lim_y_rot; //in bone space
	CCartridge* m_Ammo;
	float m_barrel_speed;
	Fvector2 m_dAngle;
	Fvector m_destEnemyDir;
	bool m_allow_fire;
	float camRelaxSpeed;
	float camMaxAngle;

	bool m_firing_disabled;
	bool m_overheat_enabled;
	float m_overheat_value;
	float m_overheat_time_quant;
	float m_overheat_decr_quant;
	float m_overheat_threshold;
	shared_str m_overheat_particles;
	CParticlesObject* p_overheat;
protected:
	void UpdateBarrelDir();
	virtual const Fvector& get_CurrentFirePoint();
	virtual const Fmatrix& get_ParticlesXFORM();

	virtual void FireStart();
	virtual void FireEnd();
	virtual void UpdateFire();
	virtual void OnShot();
	void AddShotEffector();
	void RemoveShotEffector();
	void SetDesiredDir(float h, float p);
	virtual bool IsHudModeNow() { return false; };

	//HolderCustom
public:
#ifdef STATIONARYMGUN_NEW
	virtual bool Use(const Fvector &pos, const Fvector &dir, const Fvector &foot_pos);
#else
	virtual bool Use(const Fvector& pos, const Fvector& dir, const Fvector& foot_pos) { return !Owner(); };
#endif
	virtual void OnMouseMove(int x, int y);
	virtual void OnKeyboardPress(int dik);
	virtual void OnKeyboardRelease(int dik);
	virtual void OnKeyboardHold(int dik);
#ifdef STATIONARYMGUN_NEW
	CInventory *GetInventory();
#else
	virtual CInventory* GetInventory() { return NULL; };
#endif
	virtual void cam_Update(float dt, float fov = 90.0f);

	virtual void renderable_Render();

	virtual bool attach_Actor(CGameObject* actor);
	virtual void detach_Actor();
	virtual bool allowWeapon() const { return false; };
	virtual bool HUDView() const { return true; };
	virtual Fvector ExitPosition();

#ifdef STATIONARYMGUN_NEW
	virtual CCameraBase *Camera() { return active_camera; }
#else
	virtual CCameraBase* Camera() { return camera; };
#endif

	virtual void Action(u16 id, u32 flags);
	virtual void SetParam(int id, Fvector2 val);

#ifdef STATIONARYMGUN_NEW
private:
	bool m_bActive;
	Fvector2 m_desire_angle;
	bool m_desire_angle_enable;

	float m_min_gun_speed;
	float m_max_gun_speed;
	u16 m_drop_bone;
	u16 m_actor_bone;
	Fvector m_exit_position;

	u16 m_camera_bone_def;
	u16 m_camera_bone_aim;
	float m_zoom_factor_def;
	float m_zoom_factor_aim;
	bool m_zoom_status;

	SStmAnimation m_animation;

	void CreateSkeleton(CSE_Abstract *po);
	virtual void PhDataUpdate(float step) {};
	virtual void PhTune(float step) {};
	virtual CPhysicsShellHolder *PPhysicsShellHolder() { return static_cast<CPhysicsShellHolder *>(this); }
	static void IgnoreOwnerCallback(bool &do_colide, bool bo1, dContact &c, SGameMtl *mt1, SGameMtl *mt2);
	void OnCameraChange(u16 type);

	LPCSTR m_on_before_use_callback;
protected:
	virtual void SpawnInitPhysics(CSE_Abstract *D);
	virtual void net_Save(NET_Packet &P);
	virtual BOOL net_SaveRelevant();
	void SaveNetState(NET_Packet &P);

public:
	enum
	{
		eWpnActivate = 0,
		eWpnFire,
		eWpnDesiredPos,
		eWpnDesiredDir,
		eWpnDesiredAng,
		eWpnReload,
		eWpnUnload,
	};
	virtual void SetParam(int id, Fvector val);
	virtual void UpdateEx(float fov);
	virtual void shedule_Update(u32 dt);

	virtual BOOL AlwaysTheCrow();
	virtual bool is_ai_obstacle() const;
	virtual CGameObject *cast_game_object() { return this; }
	virtual CPhysicsShellHolder *cast_physics_shell_holder() { return this; }

	IC bool IsActive() { return m_bActive; }
	CScriptGameObject *GetOwner() { return (Owner()) ? Owner()->lua_game_object() : nullptr; }
	Fvector GetFirePos() { return m_fire_pos; }
	Fvector GetFireDir() { return m_fire_dir; }
	float FireDispersionBase() { return fireDispersionBase; }
	bool IsCameraZoom() { return m_zoom_status; }
	void PlayAnimation();
	bool InFieldOfView(Fvector pos);

	/*----------------------------------------------------------------------------------------------------
		Ammo
	----------------------------------------------------------------------------------------------------*/
private:
	bool m_unlimited_ammo;
	LPCSTR m_reload_consume_callback;
	bool IsUnlimitedAmmo() { return m_unlimited_ammo; }
	bool IsReloadConsume();

	u16 m_state_index;
	float m_state_delay;

public:
	enum eState
	{
		eStateIdle = 0,
		eStateFire,
		eStateReload,
		eStateUnload,
	};

protected:
	IC u16 GetState() const { return m_state_index; }
	virtual void SwitchState(u16 new_tate);
	virtual void switch2_Idle();
	virtual void switch2_Fire();
	virtual void switch2_Reload();
	virtual void switch2_Unload();

	virtual void UpdateReload();
	virtual void UpdateUnload();

public:
	xr_vector<shared_str> m_ammoTypes;
	u8 m_ammoType;

	xr_vector<CCartridge> m_magazine;
	CCartridge m_DefaultCartridge;
	CCartridge m_lastCartridge;
	float m_fCurrentCartirdgeDisp;

	int iAmmoElapsed;
	int iMagazineSize;

	virtual void ReloadMagazine();
	virtual void UnloadMagazine(bool spawn_ammo = true);
	virtual bool GetBriefInfo(II_BriefInfo &info);

protected:
	int m_iShotNum;
	CWeaponAmmo *m_pCurrentAmmo;
	bool m_bLockType;
	float m_reload_delay;
	float m_unload_delay;
	BOOL m_bAutoSpawnAmmo;
	bool m_single_shot_wpn;

	int GetAmmoCount(u8 ammo_type);
	int GetAmmoCount_forType(shared_str const &ammo_type);
	int GetAmmoCount_allType();
	virtual u16 AddCartridge(u16 cnt);
	void SpawnAmmo(u32 boxCurr = 0xffffffff, LPCSTR ammoSect = NULL);

	float GetBaseDispersion(float cartridge_k);
	float GetFireDispersion(bool with_cartridge, bool for_crosshair = false);
	virtual float GetFireDispersion(float cartridge_k, bool for_crosshair = false);

public:
	IC int GetAmmoMagSize() const
	{
		return iMagazineSize;
	}

	IC int GetAmmoElapsed() const
	{
		return iAmmoElapsed;
	}
	void SetAmmoElapsed(int ammo_count);

	IC u8 HasAmmoType(u8 type) const
	{
		return type < m_ammoTypes.size();
	}
	IC u8 GetAmmoType() const
	{
		return m_ammoType;
	}
	void SetAmmoType(u8 type);

	BOOL AutoSpawnAmmo() const
	{
		return m_bAutoSpawnAmmo;
	};

public:
DECLARE_SCRIPT_REGISTER_FUNCTION
#endif

};

#ifdef STATIONARYMGUN_NEW
add_to_type_list(CWeaponStatMgun)
#undef script_type_list
#define script_type_list save_type_list(CWeaponStatMgun)
#endif