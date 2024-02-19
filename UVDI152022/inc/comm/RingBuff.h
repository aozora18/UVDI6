/* --------------------------------------------------------- */
/* RingBuffer.h: Interface and implementation of CRingBuffer */
/* reference info : http://larry.antram.co/fast-ring-buffer/ */
/* --------------------------------------------------------- */

#pragma once

class CRingBuff
{
public:
	CRingBuff();
	virtual ~CRingBuff();

// 로컬 변수
protected:

	UINT8				*m_pRingBuff;
	UINT32				m_u32BuffSize;	// the size of the ring buffer
	UINT32				m_u32ReadPtr;	// the read pointer
	UINT32				m_u32WritePtr;	// the write pointer

// 로컬 함수
protected:

// 공용 함수
public:

	BOOL				Create(UINT32 buff_size);
	VOID				Destroy();

	UINT32				GetReadSize();
	UINT32				GetWriteSize();
	BOOL				WriteBinary(UINT8 *data, UINT32 size);	/* 데이터를 추가함 (덮어써버림) */
	BOOL				CopyBinary(UINT8 *data, UINT32 size);	/* 데이터를 복사함, 그대로 놔둠 */
	BOOL				SkipBinary(UINT32 size);				/* 데이터 읽는 위치를 변경(이동 시킴, 읽어버리는 효과 발생) */
	BOOL				ReadBinary(UINT8 *data, UINT32 size);
	BOOL				PopBinary(UINT32 size);
	BOOL				PeekChar(UINT32 pos, UINT8 &data);
	BOOL				FindChar(UINT8 find, UINT32 &pos);
	BOOL				FindChar(UINT8 find1, UINT8 find2, UINT32 &pos);
	VOID				ResetBuff();
	UINT32				GetRingBuffSize()	{	return m_u32BuffSize;	}

	/* 읽기/쓰기 버퍼 포인터 반환 */
	UINT8				*GetReadBuffPtr();
	UINT8				*GetWriteBuffPtr();
	/* 읽기/쓰기 버퍼 인덱스 반환 */
	UINT32				GetReadBuffIndex()	{	return m_u32ReadPtr;	}
	UINT32				GetWriteBuffIndex()	{	return m_u32WritePtr;	}
};
