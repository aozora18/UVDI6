
#pragma once

#include "EBase.h"

class CICCAL : public CEBase
{
public:
	/* ������ & �ı��� */
	CICCAL(CLogData *logs, CSharedData *share);
	~CICCAL();

/* ����ü */
protected:

/* ���� �Լ� */
protected:
public:


/* ���� ���� */
protected:


/* ���� �Լ� */
protected:

/* ���� �Լ� */
public:

	BOOL				Init(LONG equip_id, LONG conn_id);
	VOID				Close();

	BOOL				GetLogToCIMConnect(BOOL &value);
	BOOL				SetLogToCIMConnect(BOOL value);
};
