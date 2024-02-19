
#pragma once

#include "../../inc/comn/ThinThread.h"

class CMainThread : public CThinThread
{
/* ������ / �ı��� */
public:
	CMainThread(HWND parent);
	virtual ~CMainThread();

/* ���� �Լ� ������ */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

/* ���� ���� */
protected:

	UINT64				m_u64TickNormal;

/* ���� �Լ� */
protected:

	VOID				SendMesgNormal(UINT32 timeout=100);

/* ���� �Լ� */
public:


};