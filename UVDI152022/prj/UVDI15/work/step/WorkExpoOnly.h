
#pragma once

#include "../WorkStep.h"

class CWorkExpoOnly : public CWorkStep
{
/* ������ & �ı��� */
public:

	//CWorkExpoOnly(LPG_CPHE expo);
	CWorkExpoOnly(LPG_CELA expo);
	virtual ~CWorkExpoOnly();
	bool useAFtemp = true;
/* �����Լ� ������ */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();


/* ���� ���� */
protected:

	BOOL				m_bMoveACamSide;	/* FALSE : Left, TRUE : Right */
	BOOL				m_bMovePhUpDown;	/* FALSE : Up, TRUE : Down */

	UINT32				m_u32ExpoCount;		/* ���� �뱤 �ݺ� Ƚ�� */

	STG_CPHE			m_stExpoInfo;		/* �ݺ� Ȥ�� ���� �뱤 ���� */
	STG_CELA			m_stExpoLog;

	UINT64				m_u64TickACamSide;


/* ���� �Լ� */
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
/* ���� �Լ� */
public:


};
