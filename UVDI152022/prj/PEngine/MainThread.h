
#pragma once

#include "../../inc/comn/ThinThread.h"

class CMainThread : public CThinThread
{
/* 생성자 / 파괴자 */
public:
	CMainThread(HWND parent);
	virtual ~CMainThread();

/* 가상 함수 재정의 */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

/* 로컬 변수 */
protected:

	UINT64				m_u64TickNormal;

/* 로컬 함수 */
protected:

	VOID				SendMesgNormal(UINT32 timeout=100);

/* 공용 함수 */
public:


};