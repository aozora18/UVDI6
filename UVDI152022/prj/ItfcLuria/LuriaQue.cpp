/*
 desc : 패킷 수신 등록 대기 관리 환형 큐 객체
*/

#include "pch.h"
#include "LuriaQue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

/*
 desc : 생성자
 parm : count	- [in]  큐 개수
 retn : None
*/
CLuriaQue::CLuriaQue(UINT32 count)
{
	/* 멤버 변수 초기화 */
	m_u32Front		= 0;
	m_u32Rear		= 0;
	/* 환형 큐 버퍼 개수 저장 */
	m_u32QueCount	= count;

	/* 개수만큼 환형 큐 버퍼 크기 메모리 할당 */
	m_pstQue = new STG_PCLR[count];
	ASSERT(m_pstQue);
	memset(m_pstQue, 0x00, sizeof(STG_PCLR)*count);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CLuriaQue::~CLuriaQue()
{
	/* 환형 큐 버퍼 메모리 해제 */
	if (m_pstQue)	delete[] m_pstQue;
}

/*
 desc : 환형 큐 버퍼 FULL인지 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaQue::IsQueFull()
{
	return (((m_u32Rear + 1) % m_u32QueCount) == m_u32Front) ? TRUE : FALSE;
}

/*
 desc : 환형 큐 버퍼 Empty인지 확인
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaQue::IsQueEmpty()
{
	return (m_u32Front == m_u32Rear) ? TRUE : FALSE;
}

/*
 desc : 큐에 새로운 데이터를 추가하고자 하는 공간 (인덱스) 반환
		중요!!! - 이 함수 호출되면, Rear 값이 증가됨
 parm : None
 retn : 새로 추가 (저장)하고자 하는 배열의 인덱스 값
*/
UINT32 CLuriaQue::PushRearIndex()
{
	return (m_u32Rear = (m_u32Rear + 1) % m_u32QueCount);
}

/*
 desc : 큐에 기존에 데이터를 가져오고자 하는 공간 (인덱스) 반환
		중요!!! - 이 함수 호출되면, Front 값이 증가됨
 parm : None
 retn : 기존 가져(제거)하고자 하는 배열의 인덱스 값
*/
UINT32 CLuriaQue::PopFrontIndex()
{
	return (m_u32Front = (m_u32Front + 1) % m_u32QueCount);
}

/*
 desc : 환형 큐에 수신된 패킷 정보 등록
 parm : data- [in]  수신된 패킷 데이터
 retn : TRUE or FALSE
*/
BOOL CLuriaQue::PushPktData(LPG_PCLR data)
{
	UINT32 u32RearIdx = 0;
	LPG_PCLR pstData = NULL;

	/* 현재 등록하고자 하는 큐 공간이 없는지 확인 */
	if (IsQueFull())
	{
		LOG_ERROR(ENG_EDIC::en_luria, L"There is not enough space in the queue. It is full.");
		return FALSE;
	}
	/* 다음에 저장될 Rear Index 값 얻고, 큐 포인터 연결 */
	u32RearIdx = PushRearIndex();
	pstData = &m_pstQue[u32RearIdx];
	/* 패킷 데이터를 큐에 등록 */
	memcpy(pstData, data, sizeof(STG_PCLR));

	return TRUE;
}

/*
 desc : 환형 큐로부터 수신된 패킷 정보 반환
 parm : None
 retn : NULL - 큐에 수신된 패킷이 없는 경우
*/
LPG_PCLR CLuriaQue::PopPktData()
{
	UINT32 u32FrontIdx = 0;

	/* 현재 큐가 비어 있는지 확인 */
	if (IsQueEmpty())	return NULL;
	/* 다음에 저장될 Rear Index 값 얻고, 큐 포인터 연결 */
	u32FrontIdx = PopFrontIndex();
	return &m_pstQue[u32FrontIdx];
}