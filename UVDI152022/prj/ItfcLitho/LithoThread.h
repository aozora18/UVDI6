
#pragma once

#include "../../inc/comn/ThinThread.h"

#ifdef _DEBUG
	#define MAX_PIO_COUNT	16
#else
	#define MAX_PIO_COUNT	64
#endif

class CLithoThread : public CThinThread
{
/* ������ & �ı��� */
public:

	CLithoThread(LPG_PMDV shmem);
	virtual ~ CLithoThread();

/* �����Լ� �缱�� */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();


/* ���� ���� */
protected:

	LPG_PMDV			m_pstShMem;			/* ���� �޸� */

	CMySection			m_syncPIO;
	/* ���� �ֱ��� ������ PIO Data �ӽ� ������ ���� List Buffer */
	CAtlList <STG_STSE>	m_lstPIO;

/* ���� �Լ� */
protected:

	VOID				UpdatePIO();


/* ���� �Լ� */
public:

	BOOL				GetHistPIO(UINT8 count, LPG_STSE state);

	VOID				ResetHistPIO();
};