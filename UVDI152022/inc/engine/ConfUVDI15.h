
#pragma once

#include "ConfBase.h"

class CConfUvdi15 : public CConfBase
{
/* 생성자 & 파괴자 */
public:

	CConfUvdi15(LPG_CIEA config);
	virtual ~CConfUvdi15();

/* 가상 함수 */
protected:

public:


/* 로컬 변수 */
protected:

	LPG_CIEA			m_pstCfg;


/* 로컬 함수 */
protected:
	

/* 공용 함수 */
public:

	BOOL				LoadConfig();
	BOOL				SaveConfig();

	BOOL LoadConfigEnvirnomental();
	BOOL SaveConfigEnvirnomental();

	LPG_CIEA			GetConfig()	{	return m_pstCfg;	}

	BOOL				LoadConfigSetupAlign();
	BOOL				SaveConfigSetupAlign();

	BOOL				LoadConfigSetupCamera();
	BOOL				SaveConfigSetupCamera();

	BOOL				LoadConfigCameraBasler();
	BOOL				SaveConfigCameraBasler();

	BOOL				LoadConfigFileName();
	BOOL				SaveConfigFileName();

	BOOL				LoadConfigTemp();
	BOOL				SaveConfigTemp();

	BOOL				LoadConfigUvDI15Common();
	BOOL				SaveConfigUvDI15Common();

	BOOL				LoadConfigTrigger(UINT8 mode);
	BOOL				SaveConfigTrigger(UINT8 mode);

	BOOL				LoadConfigACamSpec();
	BOOL				SaveConfigACamSpec();

	BOOL				LoadConfigACamFocus();
	BOOL				SaveConfigACamFocus();

	BOOL				LoadConfigMarkFind();
	BOOL				SaveConfigMarkFind();

	BOOL				LoadConfigEdgeFind();
	BOOL				SaveConfigEdgeFind();
#if 0
	BOOL				LoadConfigCaliFind();
	BOOL				SaveConfigCaliFind();
#endif
	BOOL				LoadConfigGrabProc();
	BOOL				SaveConfigGrabProc();

	BOOL				LoadConfigLineFind();
	BOOL				SaveConfigLineFind();

	BOOL				LoadConfigLedPower();
	BOOL				SaveConfigLedPower();

	BOOL				LoadConfigPhFocus();
	BOOL				SaveConfigPhFocus();

	BOOL				LoadConfigACamCali();
	BOOL				SaveConfigACamCali();

	BOOL				LoadConfigPhStepXY();
	BOOL				SaveConfigPhStepXY();

	BOOL				LoadConfigFlatParam();
	BOOL				SaveConfigFlatParam();

	BOOL				LoadConfigGlobalTrans();
	BOOL				SaveConfigGlobalTrans();

	BOOL				LoadConfigStrobeLamp();
	BOOL				SaveConfigStrobeLamp();


};