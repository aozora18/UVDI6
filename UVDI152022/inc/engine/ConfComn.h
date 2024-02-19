
#pragma once

#include "ConfBase.h"

class CConfComn : public CConfBase
{
/* 생성자 & 파괴자 */
public:

	CConfComn(LPG_CIEA config);
	virtual ~CConfComn();

/* 가상 함수 */
protected:

public:


/* 로컬 변수 */
protected:

	LPG_CIEA			m_pstCfg;


/* 로컬 함수 */
protected:

	BOOL				LoadConfigCommon();
	BOOL				SaveConfigCommon();

	BOOL				LoadConfigLuriaSvc();
	BOOL				SaveConfigLuriaSvc();
	BOOL				LoadConfigMC2Svc();
	BOOL				SaveConfigMC2Svc();
	BOOL				LoadConfigMelsecQSvc();
	BOOL				SaveConfigMelsecQSvc();
	BOOL				LoadConfigMonitor();
	BOOL				SaveConfigMonitor();

	BOOL				LoadConfigCommunication();
#if (USE_SIMULATION_DIR)
	BOOL				CreateSimulationOutDir();
#endif


/* 공용 함수 */
public:

	BOOL				LoadConfig();
	BOOL				SaveConfig();

	VOID				UpdateMonitorData();

	LPG_CIEA			GetConfig()	{	return m_pstCfg;	}
};