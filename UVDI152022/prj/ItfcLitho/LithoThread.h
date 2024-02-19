
#pragma once

#include "../../inc/comn/ThinThread.h"

#ifdef _DEBUG
	#define MAX_PIO_COUNT	16
#else
	#define MAX_PIO_COUNT	64
#endif

class CLithoThread : public CThinThread
{
/* 생성자 & 파괴자 */
public:

	CLithoThread(LPG_PMDV shmem);
	virtual ~ CLithoThread();

/* 가상함수 재선언 */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();


/* 로컬 변수 */
protected:

	LPG_PMDV			m_pstShMem;			/* 공유 메모리 */

	CMySection			m_syncPIO;
	/* 가장 최근의 수집된 PIO Data 임시 저장을 위한 List Buffer */
	CAtlList <STG_STSE>	m_lstPIO;

/* 로컬 함수 */
protected:

	VOID				UpdatePIO();


/* 공용 함수 */
public:

	BOOL				GetHistPIO(UINT8 count, LPG_STSE state);

	VOID				ResetHistPIO();
};