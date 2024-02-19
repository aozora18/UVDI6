
#pragma once

#include "../WorkStep.h"

class CWorkAbort : public CWorkStep
{
/* 생성자 & 파괴자 */
public:

	CWorkAbort(UINT8 forced=0x00);
	virtual ~CWorkAbort();

/* 가상함수 재정의 */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();


/* 로컬 변수 */
protected:

	UINT8				m_u8AbortForced;

	UINT64				m_u64TickAbortWait;	/* 장시간 대기를 멈추기 위함 */


/* 로컬 함수 */
protected:

	ENG_JWNS			StopPrinting();		/* 만약 현재 노광 중이면, 강제로 노광 해제 시킴 */
	ENG_JWNS			IsStopPrinting();	/* 현재 노광 중인지 여부 확인 */


/* 공용 함수 */
public:
	VOID				PhilInterrupsStop();

};
