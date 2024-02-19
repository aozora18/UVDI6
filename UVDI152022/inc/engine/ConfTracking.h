
#pragma once

#include "ConfBase.h"

class CConfTracking : public CConfBase
{
/* ������ & �ı��� */
public:

	CConfTracking(LPG_CWTI config);
	virtual ~CConfTracking();

/* ���� �Լ� */
protected:

public:


/* ���� ���� */
protected:

	LPG_CWTI			m_pstCfg;


/* ���� �Լ� */
protected:

	BOOL				LoadConfigAligner();
	BOOL				SaveConfigAligner();
	BOOL				LoadConfigLoader();
	BOOL				SaveConfigLoader();
	BOOL				LoadConfigHandler();
	BOOL				SaveConfigHandler();
	BOOL				LoadConfigBuffer();
	BOOL				SaveConfigBuffer();


/* ���� �Լ� */
public:

	BOOL				LoadConfig();
	BOOL				SaveConfig();

	LPG_CWTI			GetConfig()	{	return m_pstCfg;	}
};