
/*
 desc : Align Mark 검사 (마크 개수만큼 정상적으로 인식하는지 검사)
*/

#include "pch.h"
#include "../MainApp.h"
#include "WorkMarkTest.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : None
 retn : None
*/
CWorkMarkTest::CWorkMarkTest()
	: CWork()
{
	m_enWorkJobID	= ENG_BWOK::en_mark_test;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWorkMarkTest::~CWorkMarkTest()
{
}


/*
 desc : 초기 작업 수행
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkMarkTest::InitWork()
{
	CWork::InitWork();

	/* 전체 작업 단계 */
	m_u8WorkTotal	= 0x11;

	return TRUE;
}

/*
 desc : 주기적으로 작업 수행
 parm : None
 retn : None
*/
VOID CWorkMarkTest::RunWork()
{
	switch (m_u8WorkStep)
	{
	case 0x01 : m_enWorkState = IsRegistGerberCheck();			break;
	case 0x02 : m_enWorkState = IsMotorDriveStopAll();			break;
	case 0x03 : m_enWorkState = SetTrigDisabled();				break;
	/* ------------------------------------------------------------ */
	/*               Global Mark  Scan 위치로 이동                  */
	/* ------------------------------------------------------------ */
	case 0x04 : m_enWorkState = SetAlignMovingInit();			break;
	case 0x05 : m_enWorkState = IsAlignMovedInit();				break;
	case 0x06 : m_enWorkState = IsTrigDisabled();				break;
	/* ------------------------------------------------------------ */
	/*           Trigger 발생 모드 (Area Mode) 로 변경              */
	/* ------------------------------------------------------------ */
	case 0x07 : m_enWorkState = SetLuriaAreaMode();				break;
	case 0x08 : m_enWorkState = IsLuriaAreaMode();				break;
	/* ------------------------------------------------------------ */
	/* 거버에서 전체 Mark 를 읽은 후 각각 Trigger 위치 계산 및 저장 */
	/* ------------------------------------------------------------ */
	case 0x09 : m_enWorkState = SetTrigCalcGlobal4();			break;
	/* ------------------------------------------------------------ */
	/*                   Global Mark 4 points 측정                  */
	/* ------------------------------------------------------------ */
	case 0x0a : m_enWorkState = SetTrigRegistGlobal4();			break;
	case 0x0b : m_enWorkState = IsTrigRegistedGlobal4();		break;
	case 0x0c : m_enWorkState = SetAlignMovingGlobal4();		break;
	case 0x0d : m_enWorkState = IsAlignMovedGlobal();			break;
	case 0x0e : m_enWorkState = SetTrigDisabled();				break;
	case 0x0f : m_enWorkState = IsTrigDisabled();				break;
	/* ------------------------------------------------------------ */
	/*                   Unload 위치로 복귀 시킴                    */
	/* ------------------------------------------------------------ */
	/* Unload 위치로 복귀 시킴 */
	case 0x10 : m_enWorkState = SetMovingUnloader();			break;
	case 0x11 : m_enWorkState = IsMovedUnloader();				break;
	}

	/* 다음 작업 진행 여부 판단 */
	SetWorkNext();
	/* 장시간 동안 동일 위치를 반복 수행한다면 에러 처리 */
	IsWorkTimeOut();
}

/*
 desc : 다음 단계로 이동하기 위한 처리
 parm : None
 retn : None
*/
VOID CWorkMarkTest::SetWorkNext()
{
	/* 작업이 에러이거나 종료되었을 때 */
	if (ENG_JWNS::en_error == m_enWorkState || ENG_JWNS::en_comp == m_enWorkState)	return;
	if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* 모든 작업이 종료되었다면 ... */
		if (m_u8WorkStep == m_u8WorkTotal)
		{
			m_enWorkState	= ENG_JWNS::en_comp;
		}

		/* 무조건 다음 Step으로 이동 */
		m_u8WorkStep++;
		/* 가장 최근에 Waiting 한 시간 저장 */
		m_u64DelayTime	= GetTickCount64();
	}
}
