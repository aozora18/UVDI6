
#pragma once

#include "../WorkStep.h"

class CWorkFEMExpo : public CWorkStep
{
/* ������ & �ı��� */
public:

	CWorkFEMExpo(HWND hHwnd);
	virtual ~CWorkFEMExpo();

/* �����Լ� ������ */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();


/* ���� ���� */
protected:

	HWND				m_hHwnd;

	UINT8				m_u8WaitLoaded;

	UINT64				m_u64TickLoaded;

	UINT8				m_u8PrintType;	/* 0 : X = Focus Y = Energy, 1 : X = Energy Y = Focus */
	UINT8				m_u8TotalX;		/*X�� �� �뱤 Ƚ��*/
	UINT8				m_u8TotalY;		/*Y�� �� �뱤 Ƚ��*/
	UINT8				m_u8PritCntX;	/*X�� ���� �뱤 Ƚ��*/
	UINT8				m_u8PritCntY;	/*Y�� ���� �뱤 Ƚ��*/
	DOUBLE				m_dPerioX;		/*X�� �뱤 ���� (����: mm)*/	
	DOUBLE				m_dPerioY;		/*Y�� �뱤 ���� (����: mm)*/

	DOUBLE				m_dStartFocus[MAX_PH];	/*��Ŀ�� ���� ��ġ (����: mm)*/
	DOUBLE				m_dStartEnergy;	/*���� ���� ��ġ*/
	DOUBLE				m_dStepFocus;	/*��Ŀ�� ������ (����: mm)*/
	DOUBLE				m_dStepEnergy;	/*���� ������*/

	DOUBLE				m_dbStartX;		/*Stage �뱤 ���� X ��ġ (����: mm)*/
	DOUBLE				m_dbStartY;		/*Stage �뱤 ���� Y ��ġ (����: mm)*/

/* ���� �Լ� */
protected:

	VOID				InitParameter();
	VOID				ResetParameter();

	VOID				SetWorkNext();
	VOID				CalcStepRate();

	ENG_JWNS			SetExposeStartXY();
	ENG_JWNS			IsExposeStartXY();

	ENG_JWNS			SetPhZAxisMovingAll();
	ENG_JWNS			IsPhZAxisMovedAll();

	ENG_JWNS			SetStepDutyFrame();
	ENG_JWNS			IsStepDutyFrame();

	ENG_JWNS			CheckMeasCount();

// #if 1
// 	ENG_JWNS			IsGerberJobLoaded();
// #endif

/* ���� �Լ� */
public:

};
