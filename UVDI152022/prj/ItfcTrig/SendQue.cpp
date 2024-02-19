
/*
 desc : 패킷 송신 등록 대기 관리 환형 큐 객체
*/

#include "pch.h"
#include "MainApp.h"
#include "SendQue.h"


#define QUE_BUFF_SIZE		8192

/*
 desc : 생성자
 parm : count	- [in]  큐 개수
 retn : None
*/
CSendQue::CSendQue(UINT32 count)
{
	/* 멤버 변수 초기화 */
	m_u32Front		= 0;
	m_u32Rear		= 0;

	/* 환형 큐 버퍼 개수 저장 */
	m_u32QueCount	= count;

	/* -------------------------------------- */
	/* 개수만큼 환형 큐 버퍼 크기 메모리 할당 */
	/* -------------------------------------- */
	m_pstQue = new STG_TPQR[count];
	ASSERT(m_pstQue);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CSendQue::~CSendQue()
{
	// 환형 큐 버퍼 메모리 해제
	delete [] m_pstQue;
}

/*
 desc : 환형 큐 버퍼 FULL인지 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSendQue::IsQueFull()
{
	return (((m_u32Rear + 1) % m_u32QueCount) == m_u32Front) ? TRUE : FALSE;
}

/*
 desc : 환형 큐 버퍼 Empty인지 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSendQue::IsQueEmpty()
{
	return (m_u32Front == m_u32Rear) ? TRUE : FALSE;
}

/*
 desc : 모든 큐 비우기
 parm : None
 retn : None
*/
VOID CSendQue::EmptyQue()
{
	m_u32Rear = m_u32Front;
}

/*
 desc : 큐에 새로운 데이터를 추가하고자 하는 공간 (인덱스) 반환
		중요!!! - 이 함수 호출되면, Rear 값이 증가됨
 parm : None
 retn : 새로 추가 (저장)하고자 하는 배열의 인덱스 값
*/
UINT32 CSendQue::PushRearIndex()
{
	return (m_u32Rear = (m_u32Rear + 1) % m_u32QueCount);
}

/*
 desc : 큐에 기존에 데이터를 가져오고자 하는 공간 (인덱스) 반환
		중요!!! - 이 함수 호출되면, Front 값이 증가됨
 parm : None
 retn : 새로 등록된 위치의 인덱스 값 반환
*/
UINT32 CSendQue::PopFrontIndex()
{
	return (m_u32Front = (m_u32Front + 1) % m_u32QueCount);
}

/*
 desc : 환형 큐로부터 송신될 패킷 정보 반환
 parm : None
 retn : NULL - 큐에 송신될 패킷이 없는 경우
*/
LPG_TPQR CSendQue::PopData()
{
	/* 현재 큐가 비어 있는지 확인 */
	if (IsQueEmpty())	return NULL;
	/* 다음에 저장된 Front Index 값으로 이동하고, 해당 위치의 큐 포인터 연결 */
	return &m_pstQue[PopFrontIndex()];
}

/*
 desc : 송신될 버퍼에 패킷 저장
 parm : data	- [in]  저장하고자 하는 데이터 값
 retn : TRUE or FALSE
*/
BOOL CSendQue::PushData(LPG_TPQR data)
{
	UINT32 u32RearIdx	= 0;
	LPG_TPQR pstQue		= NULL;

	/* 현재 등록하고자 하는 큐 공간이 없는지 확인 */
	if (IsQueFull())
	{
		uvLogs_SaveLogs(ENG_EDIC::en_trig, ENG_LNWE::en_warning, L"The send packet buffer is full",
						WFILE, WFUNC, WLINE);
		return FALSE;
	}
	/* 다음에 저장될 Rear Index 값 얻고, 큐 포인터 연결 */
	u32RearIdx	= PushRearIndex();
	pstQue		= &m_pstQue[u32RearIdx];
	/* 송신 버퍼 메모리 복사 */
	memcpy(pstQue, data, sizeof(STG_TPQR));
	
	return TRUE;
}

/*
 desc : 현재 Queue에 등록된 개수 반환
 parm : None
 retn : 개수 반환
*/
UINT32 CSendQue::GetQueCount()
{
	if (m_u32Front > m_u32Rear)
	{
		return (m_u32QueCount - m_u32Front + m_u32Rear);
	}
	return m_u32Rear - m_u32Front;
}