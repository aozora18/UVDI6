
#pragma once

#include "../../inc/comn/ThinThread.h"

#define MOTION_CALI_CENTERING_MIN	0.0002f	/* ����: mm. 0.2 um */

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


/* ���� �Լ� */
protected:

	VOID				SendMesgNormal(UINT32 timeout=100);


/* ���� �Լ� */
public:

};