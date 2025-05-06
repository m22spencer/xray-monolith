// HUDCrosshair.h:  крестик прицела, отображающий текущую дисперсию
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#define HUD_CURSOR_SECTION "hud_cursor"

#include "ui_defs.h"


class CHUDRecon
{
private:
	Fmatrix transform;
	u32 color;

	float fuzzyShowInfo;

public:
	CHUDRecon();
	~CHUDRecon();

	void SetTransform(const Fmatrix& m);
	u32 GetColor() { return color; };
	void SetColor(u32 c);
	void OnRender(float result_dist);
};
