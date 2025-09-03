#pragma once
#include "weaponcustomauto.h"

class CWeaponPistol :
	public CWeaponAutoPistol
{
	typedef CWeaponAutoPistol inherited;
public:
	CWeaponPistol();
	virtual ~CWeaponPistol();

	virtual void Load(LPCSTR section);

protected:
	virtual bool AllowFireWhileWorking()
	{
		return true;
	}

	ESoundTypes m_eSoundClose;
};
