#pragma once


class CBlender_smaa : public IBlender
{
public:
	virtual LPCSTR getComment() { return "SMAA"; }
	virtual BOOL canBeDetailed() { return FALSE; }
	virtual BOOL canBeLMAPped() { return FALSE; }

	virtual void Compile(CBlender_Compile& C);

	CBlender_smaa();
	virtual ~CBlender_smaa();
};

class CBlender_ssfx_taa : public IBlender
{
public:
	virtual LPCSTR getComment() { return "TAA"; }
	virtual BOOL canBeDetailed() { return FALSE; }
	virtual BOOL canBeLMAPped() { return FALSE; }

	virtual void Compile(CBlender_Compile& C);

	CBlender_ssfx_taa();
	virtual ~CBlender_ssfx_taa();
};
