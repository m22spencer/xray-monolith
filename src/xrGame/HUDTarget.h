#pragma once

#include "HUDRecon.h"
#include "HUDCrosshair.h"

#define C_TRANSPARENT	D3DCOLOR_RGBA(0xff, 0xff, 0xff, 0x00)
#define C_WHITE			D3DCOLOR_RGBA(0xff, 0xff, 0xff, 0xff)

class CHUDManager;
class CLAItem;

struct SPickParam;

ENGINE_API extern Flags32 psCrosshair_Flags;

constexpr int CROSSHAIR_SHOW_ALWAYS = (1<<0);
constexpr int CROSSHAIR_INDEPENDENT = (1<<1);

constexpr int CROSSHAIR_SHOW = (1<<0);
constexpr int CROSSHAIR_USE_SHADER = (1<<1);
constexpr int CROSSHAIR_DISTANCE_LERP = (1<<2);
constexpr int CROSSHAIR_RECON = (1<<3);
constexpr int CROSSHAIR_LINE = (1<<4);

struct CrosshairSettings
{
	Flags32 flags;

	string32 shader;
	string32 texture;

	float distance_lerp_rate;

	float size;
	float depth;

	u32 color;

	float occluded_opacity;
	float occlusion_fade_rate;
	float recon_max_opacity;

	CrosshairSettings(
		Flags32 _flags,
		string32 _shader,
		string32 _texture,
		float _distance_lerp_rate,
		float _size,
		float _depth,
		u32 _color,
		float _occluded_opacity,
		float _occlusion_fade_rate,
		float _recon_max_opacity
	) :
		flags(_flags),
		distance_lerp_rate(_distance_lerp_rate),
		size(_size),
		depth(_depth),
		color(_color),
		occluded_opacity(_occluded_opacity),
		occlusion_fade_rate(_occlusion_fade_rate),
		recon_max_opacity(_recon_max_opacity)
	{
		xr_strcpy(shader, _shader);
		xr_strcpy(texture, _texture);
	}
};

struct TargetCrosshair {
	CrosshairSettings& settings;
	CHUDCrosshair crosshair;
	CHUDRecon recon;

	Fvector pos;
	float opacity;

	TargetCrosshair(CrosshairSettings& settings) :
		settings(settings),
		pos(Fvector()),
		opacity(1.f)
	{};
	~TargetCrosshair() {};

	bool Is(u32 mask) const { return settings.flags.is(mask); };
	void Load() { crosshair.Load(); };
	void Update(const SPickParam& pp, bool is_far);
	void IntegratePosition(const SPickParam& pp, float dist, bool is_far);
	void IntegrateOpacity(const SPickParam& pp, float opacity_target);
	void Render(const SPickParam& pp);
};

struct CrosshairPair {
	ui_shader shaderWire;
	TargetCrosshair crosshair_near;
	TargetCrosshair crosshair_far;

	CrosshairPair(CrosshairSettings& settings_near, CrosshairSettings& settings_far) :
		crosshair_near(settings_near),
		crosshair_far(settings_far) 
	{
		shaderWire->create("hud\\crosshair");
	};

	void Load();
	void Update(const SPickParam& pp);
	void Render(const SPickParam& pp);

	void RenderAimLine(
		Fvector va,
		Fvector vb
	) const;
};

class CHUDTarget
{
private:
	bool m_bShowCrosshair;

	CrosshairPair m_camera;
	CrosshairPair m_weapon;
	CrosshairPair m_device;

private:
	void Load();

public:
	CHUDTarget();
	~CHUDTarget();
	void Render();
	void SetDispersion(float disp) {};
	void ShowCrosshair(bool b);
};
