
#pragma once

#include "../WorkStep.h"

class CWorkExpoAlign : public CWorkStep
{
/* 생성자 & 파괴자 */
public:

	CWorkExpoAlign();
	virtual ~CWorkExpoAlign();

/* 가상함수 재정의 */
protected:
	enum class AlignMotionMode : UINT8
	{
		toInitialMoving = 0, //최초 스캔을 위해 검사 초기위치로 이동하는 스탭
		toScanMoving = 1,    //스캔을 위해 이동하는 스텝
	};
public:

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();
	int scanCount = 0;
	ENG_AMOS alignMode = ENG_AMOS::en_onthefly_2cam;
/* 로컬 변수 */
protected:

	UINT8				m_u8MarkCount;	/* 총 검색될 마크 개수 */
	UINT32				m_u32ExpoCount;		/* 현재 노광 반복 횟수 */
	STG_CELA			m_stExpoLog;

/* 로컬 함수 */
protected:

	VOID				SetWorkNext();
	VOID				SaveExpoResult(UINT8 state);

	CString				m_strLog;
	VOID				txtWrite(CString msg);
	VOID				SetPhilProcessCompelet();
/* 공용 함수 */
public:


};
