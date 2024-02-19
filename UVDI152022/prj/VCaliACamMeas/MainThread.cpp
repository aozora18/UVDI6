
/*
 desc : Main Thread
*/

#include "pch.h"
#include "MainApp.h"
#include "MainThread.h"

#include "./Work/WorkMeas.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : parent	- [in]  자신을 호출한 부모 윈도 핸들
 retn : None
*/
CMainThread::CMainThread(HWND parent)
	: CThinThread()
{
	m_bRunCali	= FALSE;
	m_bWorking	= FALSE;
	m_hParent	= parent;
	/* 객체 생성 */
	m_pWorkMeas	= new CWorkMeas();
	ASSERT(m_pWorkMeas);
}

/*
 desc : 파괴자
 parm : None
*/
CMainThread::~CMainThread()
{
	delete m_pWorkMeas;
}

/*
 desc : 스레드 동작시에 초기 한번 호출됨
 parm : None
 retn : DI_TRUE or DI_FALSE
*/
BOOL CMainThread::StartWork()
{
	/* 무조건 1 초 정도 쉬고 동작하도록 함 */
	CThinThread::Sleep(1000);

	return TRUE;
}

/*
 desc : 스레드 동작시에 주기적으로 호출됨
 parm : None
 retn : None
*/
VOID CMainThread::RunWork()
{
	/* 동기 진입 */
	if (m_syncCali.Enter())
	{
		/* 일반적인 모니터링 호출 */
		SendMesgParent(MSG_ID_NORMAL, 100);

		/* 작업 진행 여부에 따라 */
		if (m_bRunCali)
		{
			m_bWorking	= TRUE;	/* 현재 작업 중 ... */
			DoWorkMeas();
		}
		else
		{
			m_bWorking	= FALSE;	/* 현재 작업 중이지 않음 */
		}

		/* 동기 해제 */
		m_syncCali.Leave();
	}
}

/*
 desc : 스레드 종료시에 마지막으로 한번 호출됨
 parm : None
 retn : None
*/
VOID CMainThread::EndWork()
{
}

/*
 desc : 부모에게 메시지 전달 (SendMessage)
 parm : msg_id	- [in]  메시지 ID (0, 1, 2, 4)
		timeout	- [in]  SendMessag Lifetime (unit: msec)
 retn : None
*/
VOID CMainThread::SendMesgParent(UINT16 msg_id, UINT32 timeout)
{
	DWORD_PTR dwResult	= 0;
	LRESULT lResult		= 0;

	/* 부모에게 이벤트 메시지 전달 */
#if 1
	lResult	= ::SendMessageTimeout(m_hParent, WM_MAIN_THREAD, msg_id, 0L,
								   SMTO_NORMAL, timeout, &dwResult);
	if (0 == lResult)
	{
#if 1
		/* 현재 발생된 에러 값 저장 */
		TRACE("MainThread : SendMessage Time out <Normal> = %d \n", GetLastError());
#endif
	}
#else
	::PostMessage(m_hParent, WM_MAIN_THREAD, msg_id, 0L);
#endif
}

/*
 desc : 작업 진행
 parm : mode	- [in]  0x00 : 오차 진행, 0x01 : 검증 진행
		cam_id	- [in]  Align Camera Index (1 or 2)
		thick	- [in]  Material Thickness (unit: um)
		meas	- [in]  Measurement Information
 retn : None
*/
VOID CMainThread::StartCali(UINT8 cam_id, UINT16 thick, LPG_ACCS meas)
{
	/* 동기 진입 */
	if (m_syncCali.Enter())
	{
		/* Align Camera is Calirbated Mode */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);

		m_bRunCali	= TRUE;
		m_pWorkMeas->InitWork(cam_id, thick, meas);

		/* 동기 해제 */
		m_syncCali.Leave();
	}
}

/*
 desc : 작업 중지
 parm : None
 retn : None
*/
VOID CMainThread::StopCali()
{
	/* 동기 진입 */
	if (m_syncCali.Enter())
	{
		/* Align Camera is Calirbated Mode */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
		m_bRunCali	= FALSE;

		/* 동기 해제 */
		m_syncCali.Leave();
	}
}

/*
 desc : 작업 수행 (Measurement)
 parm : None
 retn : None
*/
VOID CMainThread::DoWorkMeas()
{
	UINT8 u8Step	= 0x00;
	UINT16 u16MsgID	= 0xffff;
	ENG_JWNS enState;

	/* 현재 위치의 작업 실행 */
	m_pWorkMeas->DoWork();

	/* 에러 혹은 동작 완료 되었는지 확인 */
	enState	= m_pWorkMeas->GetWorkState();
	switch (enState)
	{
	case ENG_JWNS::en_comp	:
	case ENG_JWNS::en_error	:	m_bRunCali	= FALSE;
								uvEng_Camera_SetCamMode(ENG_VCCM::en_none);	/* Align Camera is Calirbated Mode */
		return;																/* 작업을 더이상 하지 못하도록 중지 */
	case ENG_JWNS::en_wait	:	return;										/* 더 이상 다른 작업 없이 대기 */
	}

	/* 현재 작업 위치 확인 이벤트 메시지 보낼 곳인지 확인 */
	u8Step	= m_pWorkMeas->GetWorkStep();
	switch (u8Step)
	{
	case 0x05	:	u16MsgID = MSG_ID_MEAS_MARK;	break;
	case 0x0d	:	u16MsgID = MSG_ID_MEAS_CALI;	break;
	}
	/* 보낼 메시지가 있는지 여부에 따라 */
	if (0xffff != u16MsgID)	SendMesgParent(u16MsgID);
	/* 모든 작업이 완료되었는지 */
	if (m_pWorkMeas->IsCompleted())	SendMesgParent(MSG_ID_WORK_COMP);
}

/*
 desc : 현재 Grabbed Image의 처리 결과 값 반환
 parm : index	- [in]  측정 모드일 경우 무조건 0, 검증 모드일 경우 0 or 1
 retn : 처리 결과 값이 저장된 구조체 포인터 반환
*/
LPG_ACGR CMainThread::GetGrabbedResult(UINT8 index)
{
	return !m_bRunCali ? NULL : m_pWorkMeas->GetGrabbedResult();
}

/*
 desc : 현재 진행 중인 행과 열의 번호 반환
 parm : row	- [out] 반환되어 저장될 행의 번호 (1 based)
		col	- [out] 반환되어 저장될 열의 번호 (1 based)
 retn : None
*/
VOID CMainThread::GetCurRowCol(INT32 &row, INT32 &col)
{
	if (!m_bRunCali)	return;
	/* 행과 열의 위치 값 반환 */
	row	= m_pWorkMeas->GetCurRows();
	col	= m_pWorkMeas->GetCurCols();
}

/*
 desc : 현재 진행 중인 오차 값 반환
 parm : offset_x	- [out] 반환되어 저장될 X 축 오차 값 (단위: mm)
		offset_y	- [out] 반환되어 저장될 Y 축 오차 값 (단위: mm)
 retn : None
*/
VOID CMainThread::GetCurOffset(DOUBLE &offset_x, DOUBLE &offset_y)
{
	if (!m_bRunCali)	return;
	/* 행과 열의 위치 값 반환 */
	offset_x	= m_pWorkMeas->GetOffsetX();
	offset_y	= m_pWorkMeas->GetOffsetY();
}

/*
 desc : 현재 작업 진행률 반환
 parm : None
 retn : 작업 진행률 (단위 : Percentage, %)
*/
DOUBLE CMainThread::GetProcRate()
{
	return !m_bRunCali ? 0.0f : m_pWorkMeas->GetProcRate();
}

/*
 desc : 작업하는데 소요된 시간 반환
 parm : None
 retn : 작업 소요 시간 (단위: msec)
*/
UINT64 CMainThread::GetWorkTime()
{
	if (!m_bRunCali)	return 0;
	return m_pWorkMeas->GetWorkTime();
}
