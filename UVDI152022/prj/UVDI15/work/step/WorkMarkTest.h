
#pragma once

#include "../WorkStep.h"
#include <map>
#include "../../../../inc/itfe/EItfcLuria.h"

class CWorkMarkTest : public CWorkStep
{
	/* 생성자 & 파괴자 */
public:

	CWorkMarkTest(LPG_CELA expo);
	virtual ~CWorkMarkTest();

	/* 가상함수 재정의 */
protected:
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();

	enum class AlignMotionMode : UINT8
	{
		toInitialMoving = 0, //최초 스캔을 위해 검사 초기위치로 이동하는 스탭
		toScanMoving = 1,    //스캔을 위해 이동하는 스텝
	};
	
	
	/* 로컬 변수 */
protected:

	int globalMarkCnt, localMarkCnt = 0;
	UINT8				m_u8MarkCount;	/* 총 검색될 마크 개수 */
	UINT32				m_u32ExpoCount;		/* 현재 노광 반복 횟수 */
	STG_CELA			m_stExpoLog;
	int scanCount = 0;

	/* 로컬 함수 */
protected:

	VOID				SetWorkNext();

	ENG_JWNS			SetHomingACamSide();
	ENG_JWNS			IsHomedACamSide();

	VOID				SaveExpoResult(UINT8 state);

	/* 공용 함수 */
public:




};
