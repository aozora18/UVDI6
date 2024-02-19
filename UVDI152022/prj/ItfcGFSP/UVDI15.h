#pragma once

#include "Config.h"

class CPODIS : public CConfig
{
/* ������ & �ı��� */
public:

	CPODIS(PTCHAR work_dir);
	virtual ~CPODIS();

/* �����Լ� ������ */
protected:
public:


/* ���� ���� */
protected:

	LPG_CSID			m_pstConfig;


/* ���� �Լ� */
protected:

	BOOL				LoadInit();
	BOOL				SaveInit();

	BOOL				IsValidConfig();


/* ���� �Լ� */
public:

	BOOL				LoadConfig();
	BOOL				SaveConfig();

	LPG_CSID			GetConfig()			{	return m_pstConfig;					}
};
