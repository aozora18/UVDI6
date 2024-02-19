
#pragma once

#include "ConfBase.h"

class CConfGen2i : public CConfBase
{
/* ������ & �ı��� */
public:

	CConfGen2i(LPG_CIEA config);
	virtual ~CConfGen2i();

/* ���� �Լ� */
protected:

public:


/* ���� ���� */
protected:

	LPG_CIEA			m_pstCfg;	/* ȯ�� ���� ���� ���� */


/* ���� �Լ� */
protected:

	BOOL				LoadConfigFileName();
	BOOL				LoadConfigPreAligner();
	BOOL				LoadConfigDiamondRobot();
	BOOL				LoadConfigEFU();


/* ���� �Լ� */
public:

	BOOL				LoadConfig();
	BOOL				SaveConfig();

	LPG_CIEA			GetConfig()	{	return m_pstCfg;	}
};