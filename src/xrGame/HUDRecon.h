// HUDCrosshair.h:  крестик прицела, отображающий текущую дисперсию
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#define HUD_CURSOR_SECTION "hud_cursor"

#include "ui_defs.h"
#include "string_table_defs.h"

struct SPickParam;

class CHUDRecon
{
private:
	bool bDoTransform;
	Fmatrix transform;
	u32 color;
	float dist;
	float power;
	u32 pass;

	LPCSTR line1;
	LPCSTR line2;

	float fuzzyShowInfo;

public:
	CHUDRecon();
	~CHUDRecon();

	u32 GetColor() { return color; };

	void SetOpacity(float a);
	void SetTransform(const Fmatrix& m);
	void SetDoTransform(bool d);

	void Update(const SPickParam& pp);
	void Render() const;
};
