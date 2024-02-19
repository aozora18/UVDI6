
#pragma once

#include "../WorkStep.h"

class CWorkFEMExpo : public CWorkStep
{
/* 생성자 & 파괴자 */
public:

	CWorkFEMExpo(HWND hHwnd);
	virtual ~CWorkFEMExpo();

/* 가상함수 재정의 */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();


/* 로컬 변수 */
protected:

	HWND				m_hHwnd;

	UINT8				m_u8WaitLoaded;

	UINT64				m_u64TickLoaded;

	UINT8				m_u8PrintType;	/* 0 : X = Focus Y = Energy, 1 : X = Energy Y = Focus */
	UINT8				m_u8TotalX;		/*X축 총 노광 횟수*/
	UINT8				m_u8TotalY;		/*Y축 총 노광 횟수*/
	UINT8				m_u8PritCntX;	/*X축 현재 노광 횟수*/
	UINT8				m_u8PritCntY;	/*Y축 현재 노광 횟수*/
	DOUBLE				m_dPerioX;		/*X축 노광 간격 (단위: mm)*/	
	DOUBLE				m_dPerioY;		/*Y축 노광 간격 (단위: mm)*/

	DOUBLE				m_dStartFocus[MAX_PH];	/*포커스 시작 위치 (단위: mm)*/
	DOUBLE				m_dStartEnergy;	/*광량 시작 위치*/
	DOUBLE				m_dStepFocus;	/*포커스 증가값 (단위: mm)*/
	DOUBLE				m_dStepEnergy;	/*광량 증가값*/

	DOUBLE				m_dbStartX;		/*Stage 노광 시작 X 위치 (단위: mm)*/
	DOUBLE				m_dbStartY;		/*Stage 노광 시작 Y 위치 (단위: mm)*/

/* 로컬 함수 */
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

/* 공용 함수 */
public:

};
