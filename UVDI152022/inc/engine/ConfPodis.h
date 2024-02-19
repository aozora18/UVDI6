
#pragma once

#include "ConfBase.h"

class CConfPodis : public CConfBase
{
/* 생성자 & 파괴자 */
public:

	CConfPodis(LPG_CIEA config);
	virtual ~CConfPodis();

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

	LPG_CIEA			GetConfig()	{	return m_pstCfg;	}

	BOOL				LoadConfigSetupAlign();
	BOOL				SaveConfigSetupAlign();

	BOOL				LoadConfigSetupCamera();
	BOOL				SaveConfigSetupCamera();

	BOOL				LoadConfigCameraBasler();
	BOOL				SaveConfigCameraBasler();

	BOOL				LoadConfigFileName();
	BOOL				SaveConfigFileName();

	BOOL				LoadConfigRecipe();	// by sysansj
	BOOL				SaveConfigRecipe();	// by sysansj

	BOOL				LoadConfigTemp();
	BOOL				SaveConfigTemp();

	BOOL				LoadConfigPodisCommon();
	BOOL				SaveConfigPodisCommon();

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

	BOOL				LoadConfigStageST();
	BOOL				SaveConfigStageST();

	BOOL				LoadConfigPhStepXY();
	BOOL				SaveConfigPhStepXY();

	BOOL				LoadConfigGlobalTrans();
	BOOL				SaveConfigGlobalTrans();

	BOOL				LoadConfigAutoHotAir();
	BOOL				SaveConfigAutoHotAir();
};