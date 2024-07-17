
/*
 desc : 노광에 필요한 전체 레시피 적재
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkOnlyFEM.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : offset	- [in]  직접 노광할 때, 강제로 적용될 단차 값 (Pixel)
						다만, 0xff이면, 기존 환경 파일에 설정된 단차 값 그대로 노광
						Hysterisys의 Negative Pixel 값
 retn : None
*/
CWorkOnlyFEM::CWorkOnlyFEM()
	: CWorkStep()
{
	m_enWorkJobID	= ENG_BWOK::en_gerb_onlyfem;
	m_lastUniqueID	= 0;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWorkOnlyFEM::~CWorkOnlyFEM()
{
}

/*
 desc : 초기 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkOnlyFEM::InitWork()
{
	/* 내부 멤버 변수 값 초기화 */
	if (!CWork::InitWork())	return FALSE;

	/* 전체 작업 단계 */
	m_u8StepTotal = 0x09;

	return TRUE;
}

/*
 desc : 주기적으로 작업 수행
 parm : None
 retn : None
*/
VOID CWorkOnlyFEM::DoWork()
{
	/* 작업 단계 별로 동작 처리 */
	switch (m_u8StepIt)
	{
	case 0x01: m_enWorkState = CheckValidRecipe();				break;
		/* Exposure Start XY */
	case 0x02: m_enWorkState = SetExposeStartXY();				break;
	case 0x03: m_enWorkState = IsExposeStartXY();				break;
		/* Photohead Z Axis Up & Down */
	case 0x04: m_enWorkState = SetPhZAxisMovingAll();			break;
	case 0x05: m_enWorkState = IsPhZAxisMovedAll();				break;
		/* Align Camera Z Axis Up & Down */
	case 0x06: m_enWorkState = SetACamZAxisMovingAll(m_lastUniqueID);			break;
	case 0x07: m_enWorkState = IsACamZAxisMovedAll(m_lastUniqueID);			break;
		/* Led Duty Cycle & Frame Rate */
	case 0x08: m_enWorkState = SetStepDutyFrame();				break;
	case 0x09: m_enWorkState = IsStepDutyFrame();				break;
// #if 1
// 	/* 작업 적재 여부 화면 */
// 	case 0x0e : m_enWorkState = IsGerberJobLoaded();			break;
// #endif
	}

	/* 다음 작업 진행 여부 판단 */
	CWorkOnlyFEM::SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	CWork::CheckWorkTimeout();
}

/*
 desc : 다음 단계로 이동하기 위한 처리
 parm : None
 retn : None
*/
VOID CWorkOnlyFEM::SetWorkNext()
{
	UINT64 u64JobTime	= GetTickCount64() - m_u64StartTime;

	if (GetAbort())
	{
		CWork::EndWork();
		return;
	}

	/* 매 작업 구간마다 시간 값 증가 처리 */
	uvEng_UpdateJobWorkTime(u64JobTime);

	/* 설치된 광학계 개수 */
	UINT8 u8PhCount		= uvEng_GetConfig()->luria_svc.ph_count;
	UINT8 u8ACamCount	= uvEng_GetConfig()->set_cams.acam_count;

	if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* 작업률 계산 후 임시 저장 */
		CWorkOnlyFEM::CalcStepRate();

		if (m_u8StepIt == m_u8StepTotal)
		{
			m_enWorkState = ENG_JWNS::en_comp;
			/* 초기 Luria Error 값 초기화 */
			uvCmn_Luria_ResetLastErrorStatus();

			/* 항상 호출*/
			CWork::EndWork();
		}
		else
		{
		}
		/* 다음 작업을 처리하기 위함 */
		m_u8StepIt++;
		m_u64DelayTime	= GetTickCount64();
	}

	else if (ENG_JWNS::en_error == m_enWorkState)
	{
		m_enWorkState = ENG_JWNS::en_comp;
	}


}

/*
 desc : 작업 진행률 설정 (percent; %)
 parm : None
 retn : None
*/
VOID CWorkOnlyFEM::CalcStepRate()
{
	UINT8 u8PhCount	= uvEng_GetConfig()->luria_svc.ph_count;
	//DOUBLE dbRate	= DOUBLE(m_u8StepIt + m_u8SetPhNo) / DOUBLE(m_u8StepTotal + u8PhCount) * 100.0f;
	DOUBLE dbRate = DOUBLE(m_u8StepIt) / DOUBLE(m_u8StepTotal) * 100.0f;
	if (dbRate > 100.0f)	dbRate = 100.0f;
	/* 전역 메모리에 설정 */
	uvEng_SetWorkStepRate(dbRate);
}

