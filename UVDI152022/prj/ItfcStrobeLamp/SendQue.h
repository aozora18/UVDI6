
#pragma once

class CSendQue
{
// 생성자 & 파괴자
public:

	CSendQue(UINT32 count);
	~CSendQue();


// 로컬 변수
protected:

	UINT32				m_u32QueCount;	// 환형 큐 버퍼 개수

	// 환형 큐 버퍼 인덱스
	UINT32				m_u32Rear;		// 뒷 부분
	UINT32				m_u32Front;		// 앞 부분

	LPG_PP_PHILHMI		m_pstQue;		// 환형 큐 버퍼

// 로컬 함수
protected:


	UINT32				PushRearIndex();
	UINT32				PopFrontIndex();	// Front Index 값 증감 됨


// 공용 함수
public:

	BOOL				PushData(LPG_PP_PHILHMI data);
	LPG_PP_PHILHMI		PopData();	// Front Index 값 증가 됨

	/* 송신할 데이터가 있는지 여부 */
	BOOL				IsQueFull();
	BOOL				IsQueEmpty();
	/* Queue 모두 비우기 */
	VOID				EmptyQue();

	UINT32				GetQueCount();
};