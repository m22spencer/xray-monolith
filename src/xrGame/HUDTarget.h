#pragma once

#include "HUDRecon.h"
#include "HUDCrosshair.h"
#include "../xrcdb/xr_collide_defs.h"


class CHUDManager;
class CLAItem;

class CHUDTarget
{
private:
	Fvector crosshairPos;
	float crosshairOpacity;

	bool m_bShowCrosshair;
	bool m_bFirstUpdate;
	CHUDRecon HUDRecon;
	CHUDCrosshair HUDCrosshair;

private:
	float GetUIDist() const;
	float GetTargetOpacity() const;
	void IntegratePosition();
	void IntegrateOpacity();

public:
	CHUDTarget();
	~CHUDTarget();
	void Render();
	void Load();
	CHUDCrosshair& GetHUDCrosshair() { return HUDCrosshair; }
	void ShowCrosshair(bool b);
};
