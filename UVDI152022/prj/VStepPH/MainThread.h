
#pragma once

#include "../../inc/comn/ThinThread.h"

#define MOTION_CALI_CENTERING_MIN	0.0002f	/* 단위: mm. 0.2 um */

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


/* 로컬 함수 */
protected:

	VOID				SendMesgNormal(UINT32 timeout=100);


/* 공용 함수 */
public:

};