
/*
 desc : 현재 위치에서 Align Mark 인식 수행
*/

#include "pch.h"
#include "../MainApp.h"
#include "WorkMarkGrab.h"


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
CWorkMarkGrab::CWorkMarkGrab()
	: CWork()
{
	m_enWorkJobID	= ENG_BWOK::en_mark_grab;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CWorkMarkGrab::~CWorkMarkGrab()
{
}


/*
 desc : 초기 작업 수행
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
 retn : TRUE or FALSE
*/
BOOL CWorkMarkGrab::InitWork(UINT8 cam_id)
{
	CWork::InitWork();

	m_u8ACamID		= cam_id;
	/* 전체 작업 단계 */
	m_u8WorkTotal	= 0x03;

	return TRUE;
}

/*
 desc : 주기적으로 작업 수행
 parm : None
 retn : None
*/
VOID CWorkMarkGrab::RunWork()
{
	switch (m_u8WorkStep)
	{
	case 0x01 : m_enWorkState = PutOneTrigger();	break;
	case 0x02 : m_enWorkState = IsWorkWaitTime();	break;
	case 0x03 : m_enWorkState = GetGrabResult();	break;
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
VOID CWorkMarkGrab::SetWorkNext()
{
	/* 작업이 에러이거나 종료되었을 때 */
	if (ENG_JWNS::en_error == m_enWorkState || ENG_JWNS::en_comp == m_enWorkState)
	{
		m_u8WorkStep	= 0x00;
		return;
	}

	if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* 현재 단계가 트리거 발생인 경우, 2초 동안 대기 후 결과 값 받기 위함 */
		if (0x01  == m_u8WorkStep)	SetWorkWaitTime(2000);

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

