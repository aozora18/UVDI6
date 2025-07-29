
#pragma once

#include "../WorkStep.h"

class CWorkExpoOnly : public CWorkStep
{
/* 생성자 & 파괴자 */
public:

	//CWorkExpoOnly(LPG_CPHE expo);
	CWorkExpoOnly(LPG_CELA expo);
	virtual ~CWorkExpoOnly();
	bool useAFtemp = true;
/* 가상함수 재정의 */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();


/* 로컬 변수 */
protected:

	BOOL				m_bMoveACamSide;	/* FALSE : Left, TRUE : Right */
	BOOL				m_bMovePhUpDown;	/* FALSE : Up, TRUE : Down */

	UINT32				m_u32ExpoCount;		/* 현재 노광 반복 횟수 */

	STG_CPHE			m_stExpoInfo;		/* 반복 혹은 직접 노광 정보 */
	STG_CELA			m_stExpoLog;

	UINT64				m_u64TickACamSide;


/* 로컬 함수 */
protected:

	VOID				SetWorkNext();
	void ClearAF();
	VOID				CalcStepRate();

	BOOL				SetMovingACamSide();
	BOOL				IsMovedACamSide();

	BOOL				SetMovingPhUpDown();
	BOOL				IsMovedPhUpDown();

	CString				m_strLog;
	VOID				txtWrite(CString msg);

	VOID				SaveExpoLog(UINT8 state);
	VOID				SetPhilProcessCompelet();
/* 공용 함수 */
public:


};
