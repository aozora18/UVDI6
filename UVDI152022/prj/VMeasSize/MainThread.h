
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

	CMySection			m_syncCali;

/* ���� �Լ� */
protected:

	VOID				SendMesgParent(UINT16 msg_id, UINT32 timeout=100);


/* ���� �Լ� */
public:

};