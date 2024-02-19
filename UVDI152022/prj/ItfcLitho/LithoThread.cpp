
/*
 desc : PIO 변경 값 모니터링 스레드
*/

#include "pch.h"
#include "LithoThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : shmem	- [in]  공유 메모리
 retn : None
*/
CLithoThread::CLithoThread(LPG_PMDV shmem)
{
	m_pstShMem	= shmem;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CLithoThread::~CLithoThread()
{
	ResetHistPIO();
}

/*
 desc : 스레드 시작할 때, 최초 한번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLithoThread::StartWork()
{

	return TRUE;
}

/*
 desc : 스레드 동작 중, 지속적으로 호출됨
 parm : None
 retn : None
*/
VOID CLithoThread::RunWork()
{
	/* PLC가 연결된 경우만 수행 */
	if (!uvMCQDrv_IsConnected())	return;

	/* 현재 값과 바로 이전 값을 비교하여, 다르면 등록 */
	UpdatePIO();
}

/*
 desc : 스레드 중지(종료)될 때, 마지막으로 한 번 호출됨
 parm : None
 retn : None
*/
VOID CLithoThread::EndWork()
{
}

/*
 desc : PIO History 초기화
 parm : None
 retn : None
*/
VOID CLithoThread::ResetHistPIO()
{
	/* 동기 진입 */
	if (m_syncPIO.Enter())
	{
		m_lstPIO.RemoveAll();
		/* 동기 해제 */
		m_syncPIO.Leave();
	}
}

/*
 desc : PIO 이벤트 갱신 (현재 값과 이전 값을 비교하여 다른 경우만 등록 관리)
 parm : None
 retn : None
*/
VOID CLithoThread::UpdatePIO()
{
	BOOL bAdded	= TRUE;

	/* 동기 진입 */
	if (m_syncPIO.Enter())
	{
		STG_STSE stData	= {NULL};

		/* 현재 리스트 버퍼의 개수가 없다면, 현재 값 바로 등록 */
		if (m_lstPIO.GetCount() > 0)
		{
			/* 기존 등록된 버퍼가 64개 이상되면, 맨 마지막 버퍼부터 제거 */
			if (m_lstPIO.GetCount() > MAX_PIO_COUNT)	m_lstPIO.RemoveTail();
			/* 현재 PIO 상태 값과 방금 이전 (가장 최근)에 등록된 값 비교 후, 다르면 등록 */
			stData	= m_lstPIO.GetHead();
			bAdded	= m_pstShMem->IsEqualPIO(stData.in_track_expo.w_scan,
											 stData.out_expo_track.w_track);
		}

		/*현재 값 바로 등록 */
		if (bAdded)
		{
			stData.in_track_expo.w_scan		= m_pstShMem->in_track_expo.w_scan;
			stData.out_expo_track.w_track	= m_pstShMem->out_expo_track.w_track;
			/* 등록 (!!! 주의 : 가장 최근 값을 맨 앞에 등록 !!!) */
			m_lstPIO.AddHead(stData);

			/* 새로 변경된 동작 저장 */
			if (GetConfig()->set_comn.comm_log_litho)
			{
				TCHAR tzEvent[128]	= {NULL};

				swprintf_s(tzEvent, 128, L"PODIS vs. Litho [Event] [called_func=UpdatePIO]"
										 L"[in_track_expo.w_scan=0x%04x][stData.out_expo_track.w_track=0x%04x]",
						   stData.in_track_expo.w_scan, stData.out_expo_track.w_track);
				LOG_MESG(ENG_EDIC::en_litho, tzEvent);
			}
		}

		/* 동기 해제 */
		m_syncPIO.Leave();
	}
}

/*
 desc : 특정 위치의 PIO 상태 값 반환 (가장 최근부터 이전에 발생된 상태 값 가져오기)
 parm : count	- [in]  최소 1 값 (가장 최근에 발생된 순으로 값 가져오기 위한 개수 값)
		state	- [out] PIO 상태 값 저장 ('count' 개수만큼 저장될 구조체 버퍼 포인터)
 retn : TRUE or FALSE (버퍼가 비어 있거나, 해당 위치 (Index)에 값이 없거나 ...)
*/
BOOL CLithoThread::GetHistPIO(UINT8 count, LPG_STSE state)
{
	UINT8 i, u8Count= 0;
	BOOL bIsState	= FALSE;

	/* 동기 진입 */
	if (m_syncPIO.Enter())
	{
		/* 현재 등록된 개수 */
		u8Count	= (UINT8)m_lstPIO.GetCount();
		if (count > MAX_PIO_COUNT)	count = MAX_PIO_COUNT;
		if (u8Count >= count && count > 0)
		{
			/* 현재 Index에 해당되는 위치의 Scanner 및 Track의 PIO 상태 값 얻기 */
			for (i=0; i<count; i++)
			{
				state[i] = m_lstPIO.GetAt(m_lstPIO.FindIndex(i));
			}
			/* 반환 성공 */
			bIsState = TRUE;
		}
		/* 동기 해제 */
		m_syncPIO.Leave();
	}

	return bIsState;
}