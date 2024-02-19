
#include "pch.h"
#include "RingBuff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
 desc : 생성자
 parm : None
 retn : None
*/
CRingBuff::CRingBuff()
{
	m_pRingBuff		= NULL;
	m_u32BuffSize	= 0;
	m_u32ReadPtr	= 0;
	m_u32WritePtr	= 0;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CRingBuff::~CRingBuff()
{
	Destroy();
}

/*
 desc : Ring Buffer 메모리 할당
 parm : buff_size	- [in]  버퍼 크기 (단위: 바이트)
 retn : 0x00 - 할당 실패, 0x01 - 할당 성공
*/
BOOL CRingBuff::Create(UINT32 buff_size)
{
	// 일단 할당된 버퍼 크기 저장
	m_u32BuffSize	= buff_size;
	// 링 버퍼 메모리 할당
	m_pRingBuff		= new UINT8 [buff_size+1];
	memset(m_pRingBuff, 0x00, buff_size+1);

	return m_pRingBuff ? TRUE : FALSE;
}
/*
 desc : 링 버퍼 메모리 해제
 parm : None
 retn : None
*/
VOID CRingBuff::Destroy()
{
	if (m_pRingBuff)
	{
		delete [] m_pRingBuff;
		m_pRingBuff	= NULL;
	}
}

/*
 desc : 버퍼 초기화
 parm : None
 retn : None
*/
VOID CRingBuff::ResetBuff()
{
	if (m_pRingBuff)	memset(m_pRingBuff, 0x00, m_u32BuffSize);

	m_u32ReadPtr	= 0;
	m_u32WritePtr	= 0;
}

/*
 desc : 현재 버퍼로부터 읽기 가능한 데이터의 양(크기; Bytes)를 반환 한다
		즉, 현재 버퍼에 저장(남아있는)되어 있는 데이터의 크기를 반환 한다
 parm : None
 retn : 읽기 가능한 데이터의 양(크기; Bytes) 반환
*/
UINT32 CRingBuff::GetReadSize()
{
	if (!m_pRingBuff)	return 0;

	if (m_u32ReadPtr == m_u32WritePtr)	return 0;
	if (m_u32ReadPtr < m_u32WritePtr)	return m_u32WritePtr - m_u32ReadPtr;
	if (m_u32ReadPtr > m_u32WritePtr)	return (m_u32BuffSize - m_u32ReadPtr) + m_u32WritePtr;
	return 0;
}

/*
 desc : 버퍼 내에 저장를 위해 남아 있는 공간의 양(크기; Bytes)을 반환 한다.
 parm : None
 retn : 저장하기 위해 남아 있는 버퍼 크기 반환
*/
UINT32 CRingBuff::GetWriteSize()
{
	if (!m_pRingBuff)	return 0;

	if (m_u32ReadPtr == m_u32WritePtr)	return m_u32BuffSize - 1;
	if (m_u32ReadPtr < m_u32WritePtr)	return (m_u32BuffSize - m_u32WritePtr) + m_u32ReadPtr - 1;
	if (m_u32ReadPtr > m_u32WritePtr)	return m_u32ReadPtr - m_u32WritePtr - 1;

	return 0;
}

/*
 desc : 현재 Ring Buffer의 읽기 시작 위치의 포인터 얻기
 parm : None
 retn : 읽기 위한 버퍼의 시작 포인터 반환
*/
UINT8 *CRingBuff::GetReadBuffPtr()
{
	return &m_pRingBuff[m_u32ReadPtr];
}

/*
 desc : 현재 Ring Buffer의 저장 시작 위치의 포인터 얻기
 parm : None
 retn : 저장하기 위한 버퍼의 시작 포인터 반환
*/
UINT8 *CRingBuff::GetWriteBuffPtr()
{
	return &m_pRingBuff[m_u32WritePtr];
}

/*
 desc : 내부 버퍼에 저장된 데이터 읽기 (데이터 추출)
 desc : data	- [in]  읽어들인 데이터가 저장될 버퍼 포인터
		size	- [in]  읽어들이고자 하는 데이터의 크기
						(실제 버퍼 크기보다 1바이트 적은 값이어야 한다.)
 retn : 0x00 - 읽기 실패, 0x01 - 읽기 성공
*/
BOOL CRingBuff::ReadBinary(UINT8 *data, UINT32 size)
{
	if (size < 1 || size > GetReadSize())	return FALSE;

	// easy case, no wrapping
	if (m_u32ReadPtr+size <= m_u32BuffSize)
	{
		memcpy(data, &m_pRingBuff[m_u32ReadPtr], size);
		m_u32ReadPtr += size;
	}
	else // harder case, buffer wraps
	{
		UINT32 u32FirstChunkSize	= m_u32BuffSize - m_u32ReadPtr;
		UINT32 u32SecondChunkSize	= size - u32FirstChunkSize;

		memcpy(data, &m_pRingBuff[m_u32ReadPtr], u32FirstChunkSize);
		memcpy(&data[u32FirstChunkSize], &m_pRingBuff[0], u32SecondChunkSize);
		m_u32ReadPtr = u32SecondChunkSize;
	}
	return TRUE;
}

/*
 desc : 내부 버퍼에 저장된 데이터를 읽어서 바로 버림 (반환 없음)
 desc : size	- [in]  읽어서 버리는 데이터 크기 (만약 0 값이면, 모두 버림)
 retn : 0x00 - 읽고리 버리기 실패, 0x01 - 읽어서 버리기 성공
*/
BOOL CRingBuff::PopBinary(UINT32 size)
{
	if (size > GetReadSize())	return FALSE;

	// easy case, no wrapping
	if (m_u32ReadPtr+size <= m_u32BuffSize)
	{
		m_u32ReadPtr += size;
	}
	else // harder case, buffer wraps
	{
		UINT32 u32FirstChunkSize	= m_u32BuffSize - m_u32ReadPtr;
		UINT32 u32SecondChunkSize	= size - u32FirstChunkSize;
		m_u32ReadPtr = u32SecondChunkSize;
	}
	return TRUE;
}

/*
 desc : 내부 버퍼에 저장된 데이터의 읽는 위치를 이동 한다. (데이터 추출)
 desc : size	- [in]  읽어서 버리고자하는 데이터의 크기
						(실제 버퍼 크기보다 1바이트 적은 값이어야 한다.)
 retn : 0x00 - 읽기 실패, 0x01 - 읽기 성공
*/
BOOL CRingBuff::SkipBinary(UINT32 size)
{
// 	if (size > GetMaxReadSize())	return 0x00;

	// easy case, no wrapping
	if (m_u32ReadPtr+size <= m_u32BuffSize)
	{
		m_u32ReadPtr += size;
	}
	else // harder case, buffer wraps
	{
		m_u32ReadPtr = size - (m_u32BuffSize - m_u32ReadPtr);
	}
	return 0x01;
}

/*
 desc : 내부 버퍼에 데이터 저장
 parm : data	- [in]  저장하고자 하는 데이터가 저장된 포인터 버퍼
		size	- [in]  저장하고자 하는 데이터의 크기 즉, 'data' 버퍼에 저장된 데이터의 크기
 retn : 0x00 - 저장 실패, 0x01 - 저장 성공
*/
BOOL CRingBuff::WriteBinary(UINT8 *data, UINT32 size)
{
	if (size < 1 || size > GetWriteSize())	return FALSE;

	/* 더 이상 저장할 공간이 없다면, 저장하지 않음 */
	if (GetWriteSize() <= size)	return FALSE;

	/* easy case, no wrapping */
	if (m_u32WritePtr+size <= m_u32BuffSize)
	{
		memcpy(&m_pRingBuff[m_u32WritePtr], data, size);
		m_u32WritePtr += size;
	}
	else /* harder case we need to wrap */
	{
		UINT32 u32FirstChunkSize	= m_u32BuffSize - m_u32WritePtr;
		UINT32 u32SecondChunkSize	= size - u32FirstChunkSize;

		memcpy(&m_pRingBuff[m_u32WritePtr], data, u32FirstChunkSize);
		memcpy(&m_pRingBuff[0], &data[u32FirstChunkSize], u32SecondChunkSize);
		m_u32WritePtr = u32SecondChunkSize;
	}

	return TRUE;
}

/*
 desc : 내부 버퍼에 저장된 데이터 복사 (데이터 복사 - 포인터 이동 없음)
 desc : data	- [in]  읽어들인 데이터가 저장될 버퍼 포인터
		size	- [in]  읽어들이고자 하는 데이터의 크기
						(실제 버퍼 크기보다 1바이트 적은 값이어야 한다.)
 retn : 0x00 - 읽기 실패, 0x01 - 읽기 성공
*/
BOOL CRingBuff::CopyBinary(UINT8 *data, UINT32 size)
{
//	if (size > GetMaxReadSize())	return FALSE;
	/* easy case, no wrapping */
	if (m_u32ReadPtr+size <= m_u32BuffSize)
	{
		memcpy(data, &m_pRingBuff[m_u32ReadPtr], size);
	}
	else /* harder case, buffer wraps */
	{
		UINT32 u32FirstChunkSize	= m_u32BuffSize - m_u32ReadPtr;
		UINT32 u32SecondChunkSize	= size - u32FirstChunkSize;
		memcpy(data, &m_pRingBuff[m_u32ReadPtr], u32FirstChunkSize);
		memcpy(&data[u32FirstChunkSize], &m_pRingBuff[0], u32SecondChunkSize);
	}
	return TRUE;
}

/*
 desc : 버퍼로부터 주어진 위치에 존재하는 문자(데이터)를 가져온다.
		 (그 값을 추출해서 제거하지는 않는다.)
 parm : pos		- [in]  버퍼로부터 가져오고자 하는 위치 (zero-based)
		data	- [in]  해당 위치의 값이 저장된 변수
 retn : 0x00 - 가져오기 실패, 0x01 - 가져오기 성공
*/
BOOL CRingBuff::PeekChar(UINT32 pos, UINT8 &data)
{
	if (pos >= GetReadSize())	return 0x00;

	if (m_u32WritePtr > m_u32ReadPtr)
	{
		// easy case, buffer doesn't wrap
		data = m_pRingBuff[m_u32ReadPtr+pos];
	}
	else if (m_u32WritePtr == m_u32ReadPtr)
	{
		// nothing in the buffer
		return 0x00;
	}
	else if (m_u32WritePtr < m_u32ReadPtr)
	{
		// harder case, buffer wraps
		if (m_u32ReadPtr+pos < m_u32BuffSize)
		{
			// pos was in first chunk
			data = m_pRingBuff[m_u32ReadPtr + pos];
		}
		else
		{
			// pos is in second chunk
			data = m_pRingBuff[pos - (m_u32BuffSize-m_u32ReadPtr)];
		}
	}

	return 0x01;
}

/*
 desc : 검색 대상인 문자 1개가 버퍼 내에 있는지, 있다면 해당 위치를 반환 (zero-based)
 parm : find	- [in]  검색하고자 하는 문자
		pos		- [out] 검색(찾은)된 위치 (zero-based)
 retn : 0x00 - 검색 실패, 0x01 - 검색 성공
*/
BOOL CRingBuff::FindChar(UINT8 find, UINT32 &pos)
{
	UINT8 u8GetCh;
	UINT32 i = 0;

	for (; i<GetReadSize(); i++)
	{
		if (PeekChar(i, u8GetCh))
		{
			if (u8GetCh == find)
			{
				pos = i;
				return TRUE;
			}
		}
	}

	return FALSE;
}

/*
 desc : 검색 대상인 문자 2 개가 버퍼 내에 있는지, 있다면 해당 위치를 반환 (zero-based)
 parm : find1	- [in]  첫 번째 검색하고자 하는 문자
		find2	- [in]  두 번째 검색하고자 하는 문자
		pos		- [out] 검색(찾은)된 위치 (zero-based) (find2의 위치 반환)
 retn : 0x00 - 검색 실패, 0x01 - 검색 성공
*/
BOOL CRingBuff::FindChar(UINT8 find1, UINT8 find2, UINT32 &pos)
{
	UINT8 u8GetCh, u8Find[2] = { find1, find2 };
	UINT32 i = 0, j = 0;

	for (; i<GetReadSize(); i++)
	{
		if (PeekChar(i, u8GetCh))
		{
			if (u8GetCh == u8Find[j])
			{
				if (++j == 0x02)
				{
					pos = i;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}