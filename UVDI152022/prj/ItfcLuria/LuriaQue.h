
#pragma once

class CLuriaQue
{
// 생성자 & 파괴자
public:

	CLuriaQue(UINT32 count);
	~CLuriaQue();


// 로컬 변수
protected:

	UINT32				m_u32QueCount;	// 환형 큐 버퍼 개수

	// 환형 큐 버퍼 인덱스
	UINT32				m_u32Rear;		// 뒷 부분
	UINT32				m_u32Front;		// 앞 부분

	LPG_PCLR			m_pstQue;		// 환형 큐 버퍼

// 로컬 함수
protected:

	UINT32				PushRearIndex();
	UINT32				PopFrontIndex();

// 공용 함수
public:

	BOOL				IsQueFull();
	BOOL				IsQueEmpty();

	BOOL				PushPktData(LPG_PCLR data);
	LPG_PCLR			PopPktData();
};
