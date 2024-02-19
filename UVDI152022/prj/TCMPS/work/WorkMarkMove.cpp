
/*
 desc : Align Mark 중 임의의 한 점으로 이동
*/

#include "pch.h"
#include "../MainApp.h"
#include "WorkMarkMove.h"


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
CWorkMarkMove::CWorkMarkMove()
	: CWork()
{
	m_enWorkJobID	= ENG_BWOK::en_mark_move;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWorkMarkMove::~CWorkMarkMove()
{
}


/*
 desc : 초기 작업 수행
 parm : mark_no	- [in]  검색 대상의 Mark Number (0x1 or Later)
 retn : TRUE or FALSE
*/
BOOL CWorkMarkMove::InitWork(UINT16 mark_no)
{
	CWork::InitWork();

	m_u16MarkNo		= mark_no;
	/* 전체 작업 단계 */
	m_u8WorkTotal	= 0x04;

	return TRUE;
}

/*
 desc : 주기적으로 작업 수행
 parm : None
 retn : None
*/
VOID CWorkMarkMove::RunWork()
{
	switch (m_u8WorkStep)
	{
	case 0x01 : m_enWorkState = IsRegistGerberCheck();		break;
	case 0x02 : m_enWorkState = IsMotorDriveStopAll();		break;
	case 0x03 : m_enWorkState = SetMovingAlignMark();		break;
	case 0x04 : m_enWorkState = IsMovedAlignMark();			break;
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
VOID CWorkMarkMove::SetWorkNext()
{
	/* 작업이 에러이거나 종료되었을 때 */
	if (ENG_JWNS::en_error == m_enWorkState || ENG_JWNS::en_comp == m_enWorkState)
	{
		m_u8WorkStep	= 0x00;
		return;
	}

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

