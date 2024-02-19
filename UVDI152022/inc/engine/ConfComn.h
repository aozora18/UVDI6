
#pragma once

#include "ConfBase.h"

class CConfComn : public CConfBase
{
/* ������ & �ı��� */
public:

	CConfComn(LPG_CIEA config);
	virtual ~CConfComn();

/* ���� �Լ� */
protected:

public:


/* ���� ���� */
protected:

	LPG_CIEA			m_pstCfg;


/* ���� �Լ� */
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


/* ���� �Լ� */
public:

	BOOL				LoadConfig();
	BOOL				SaveConfig();

	VOID				UpdateMonitorData();

	LPG_CIEA			GetConfig()	{	return m_pstCfg;	}
};