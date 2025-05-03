// HUDCrosshair.cpp:  крестик прицела, отображающий текущую дисперсию
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "HUDCrosshair.h"
#include "../xrEngine/CustomHUD.h"
#include "../xrEngine/igame_persistent.h"
#include "ui_base.h"

string32 crosshair_shader = "hud\\cursor";
string32 crosshair_texture = "ui\\cursor";
float crosshair_near_size = 1.f;
float crosshair_far_size = 1.f;
float crosshair_depth_begin = 0.f;
float crosshair_depth_end = 100.f;

CHUDCrosshair::CHUDCrosshair()
{
	strcpy(lastCrosshairShader, "");
	strcpy(lastCrosshairTexture, "");
	shaderWire->create("hud\\crosshair");
	transform = Fmatrix().identity();
	minRadius = 0.001f;
	maxRadius = 0.004f;
	crossColor = 0;
	dispersionRadius = 0.f;
}

CHUDCrosshair::~CHUDCrosshair()
{
}

void CHUDCrosshair::Load()
{
	minRadius = pSettings->r_float(HUD_CURSOR_SECTION, "min_radius");
	maxRadius = pSettings->r_float(HUD_CURSOR_SECTION, "max_radius");
	crossColor = pSettings->r_fcolor(HUD_CURSOR_SECTION, "cross_color").get();
}

void CHUDCrosshair::SetTransform(const Fmatrix& m)
{
	transform.set(m);
}

void CHUDCrosshair::SetColor(u32 c)
{
	crossColor = c;
}

void CHUDCrosshair::SetDispersion(float d)
{
	dispersionRadius = d;
}

extern ENGINE_API BOOL g_bRendering;

static float lerp(float a, float b, float t)
{
	clamp(t, 0.f, 1.f);
	return a * (1 - t) + b * t;
}

static float remap(float value, float from1, float to1, float from2, float to2) {
	return (value - from1) / (to1 - from1) * (to2 - from2) + from2;
}

void CHUDCrosshair::DeinitShaderCrosshair()
{
	if (shaderCrosshair->inited())
	{
		shaderCrosshair->destroy();
		strcpy(lastCrosshairShader, "");
		strcpy(lastCrosshairTexture, "");
	}
}

bool CHUDCrosshair::InitShaderCrosshair()
{
	if (strcmp(lastCrosshairShader, crosshair_shader) || strcmp(lastCrosshairTexture, crosshair_texture))
	{
		DeinitShaderCrosshair();

		shaderCrosshair->create(crosshair_shader, crosshair_texture);
		strcpy(lastCrosshairShader, crosshair_shader);
		strcpy(lastCrosshairTexture, crosshair_texture);
	}

	return shaderCrosshair->inited();
}

void CHUDCrosshair::PushVerts(Fvector* verts, Fvector* uvs, int count, Fmatrix mat, Fvector4 pos)
{
	Fvector2 scr_size = {
		float(::Render->getTarget()->get_width()),
		float(::Render->getTarget()->get_height())
	};

	// Calculate size from linear depth
	float zNear = Device.ViewportNear;
	float zFar = g_pGamePersistent->Environment().CurrentEnv->far_plane;
	float t = remap(pos.w / zFar, crosshair_depth_begin / zFar, crosshair_depth_end / zFar, 0.f, 1.f);
	float size = pos.w * lerp(crosshair_near_size, crosshair_far_size, t) * (Device.fFOV / 90.0);

	for (int i = 0; i < count; i++)
	{
		Fvector vert = verts[i];
		Fvector uv;
		if (uvs)
			uv = uvs[i];

		vert.mul(size);
		mat.transform(vert);
		vert.x *= scr_size.x / scr_size.y;
		vert.y *= -1;
		vert.x += pos.x;
		vert.y += pos.y;
		UIRender->PushPoint(vert.x, vert.y, 0, crossColor, uv.x, uv.y);
	}
}

void CHUDCrosshair::RenderShaderCrosshair()
{
	// Fetch the render target size
	Fvector2 scr_size = {
		float(::Render->getTarget()->get_width()),
		float(::Render->getTarget()->get_height())
	};

	float min = minRadius;
	float max = maxRadius;

	Fvector verts[4] = {
		{-max, -max},
		{-max, max},
		{max, -max},
		{max, max},
	};

	Fvector uvs[4] = {
		{0, 1},
		{0, 0},
		{1, 1},
		{1, 0},
	};

	Fmatrix mat = Fmatrix().mul(Device.mFullTransform, transform);
	Fvector4 pos = Fvector4().set(mat._41, mat._42, mat._43, mat._44);

	// Apply perspective divide to aim point and transform into screen space
	pos.x = ((pos.x / pos.w) + 1.f) * 0.5f * scr_size.x;
	pos.y = (-(pos.y / pos.w) + 1.f) * 0.5f * scr_size.y;

	UIRender->StartPrimitive(4, IUIRender::ptTriStrip, UI().m_currentPointType);
	PushVerts(verts, uvs, 4, mat, pos);

	// Draw
	UIRender->SetShader(*shaderCrosshair);
	UIRender->FlushPrimitive();
}

void CHUDCrosshair::RenderWireCrosshair()
{
	// Fetch the render target size
	Fvector2 scr_size = {
		float(::Render->getTarget()->get_width()),
		float(::Render->getTarget()->get_height())
	};

	float min = minRadius;
	float max = maxRadius;

	// Create vertices from our size metrics
	Fvector verts[8] = {
		{ min, 0 },
		{ max, 0 },
		{ -min, 0 },
		{ -max, 0 },
		{ 0, min },
		{ 0, max },
		{ 0, -min },
		{ 0, -max },
	};

	Fvector uvs[8];

	Fmatrix mat = Fmatrix().mul(Device.mFullTransform, transform);
	Fvector4 pos = Fvector4().set(mat._41, mat._42, mat._43, mat._44);

	// Apply perspective divide to aim point and transform into screen space
	pos.x = ((pos.x / pos.w) + 1.f) * 0.5f * scr_size.x;
	pos.y = (-(pos.y / pos.w) + 1.f) * 0.5f * scr_size.y;

	// Project vertices for accurate scaling
	UIRender->StartPrimitive(8, IUIRender::ptLineList, UI().m_currentPointType);
	PushVerts(verts, NULL, 8, mat, pos);

	// Render a 1px wide line  for the center dot
	UIRender->PushPoint(pos.x - 0.5f, pos.y, 0, crossColor, 0, 0);
	UIRender->PushPoint(pos.x + 0.5f, pos.y, 0, crossColor, 0, 0);

	UIRender->SetShader(*shaderWire);
	UIRender->FlushPrimitive();
}

void CHUDCrosshair::OnRender()
{
	VERIFY(g_bRendering);

	if (psHUD_Flags.is(HUD_SHADER_CROSSHAIR))
	{
		if (InitShaderCrosshair())
			RenderShaderCrosshair();
	}
	else
	{
		DeinitShaderCrosshair();
		RenderWireCrosshair();
	}
}