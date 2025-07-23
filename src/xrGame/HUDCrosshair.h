// HUDCrosshair.h:  крестик прицела, отображающий текущую дисперсию
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#define HUD_CURSOR_SECTION "hud_cursor"

#include "ui_defs.h"

class CHUDCrosshair
{
private:
	Fmatrix transform;
	float minRadius;
	float maxRadius;
	float scale;
	u32 crossColor;

	ui_shader shaderWire;
	ui_shader shaderCrosshair;

	string32* crosshairShader;
	string32* crosshairTexture;
	string32 lastCrosshairShader;
	string32 lastCrosshairTexture;

	float dispersionRadius;

	void PushVerts(Fvector* verts, Fvector* uvs, int count, Fmatrix mat, Fvector4 pos) const;

	void InitShaderWire();
	void DeinitShaderCrosshair();
	bool InitShaderCrosshair();
	void RenderShaderCrosshair();
	void RenderWireCrosshair();

public:
	CHUDCrosshair();
	~CHUDCrosshair();

	Fmatrix GetTransform() const { return transform; };
	u32 GetColor() const { return crossColor; };

	void SetTransform(const Fmatrix& m);
	void SetScale(float s);
	void SetColor(u32 c);
	void SetShader(string32* shader) { crosshairShader = shader; };
	void SetTexture(string32* texture) { crosshairTexture = texture; };
	void SetDispersion(float d);

	void Load();
	void OnRender(bool shader);
};
