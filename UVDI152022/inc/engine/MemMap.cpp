
/*
 desc : 장비 (혹은 장치) 기본 객체
*/

#include "pch.h"
#include "MemMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : None
 retn : None
*/
CMemMap::CMemMap()
{
	m_hMemMap	= NULL;
	m_pMemMap	= NULL;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMemMap::~CMemMap()
{
	if (m_pMemMap)	UnmapViewOfFile(m_pMemMap);
	if (m_hMemMap)	CloseHandle(m_hMemMap);
}

/*
 desc : 공유 메모리 생성 및 연결
 parm : key		- [in]  공유 메모리 유일한 Key Name (Mutex key Name과 유사)
		size	- [in]  공유 메모리 영역 크기 (단위: Bytes. 8 Bytes 배수로 입력해 줘야 함)
 retn : TRUE or FALSE
*/
BOOL CMemMap::CreateMap(PTCHAR key, UINT32 size)
{
	UINT32 u32Size	= size, u32Error = 0;

	/* 메모리 할당 크기 값 재설정 (64 bit 운영체제에서는 8 Bytes 단위로 할당) */
	if ((u32Size % 8) != 0)	u32Size	= (u32Size / 8) * u32Size + 8;

	/* 운영체제의 물리적인 메모리 매핑 수행 */
	m_hMemMap	= CreateFileMapping(INVALID_HANDLE_VALUE,	/* 하드 디스크로 연결하려면 파일 핸들 넣어주면 됨*/
									NULL,					/* 보안 관련 부분이므로, Pass */
									PAGE_READWRITE,			/* 물리적인 메모리 속성 */
									0,						/* 대용량 메모리 공유할 때, 사용됨 즉, 4 GB 이상 사용하는 경우임 */
									size,
									key);					/* 유일한 Key Name */
	if (!m_hMemMap)
	{
		AfxMessageBox(_T("Failed to CreateFileMapping"), MB_ICONSTOP|MB_TOPMOST);
		/* 에러 코드 출력 */
		u32Error	= GetLastError();

		return FALSE;
	}

	/* 물리적인 저장 공간 연결 작업 */
	m_pMemMap	= (PUINT8)MapViewOfFile(m_hMemMap,				/* CreatFileMapping에 의해 생성된 핸들 */
										FILE_MAP_ALL_ACCESS,	/* CreateFileMapping에서 READWRITE 했기 때문에 */
										0, 0, 0);				/* 0 값으로 설정하면, offset 부터 파일의 끝까지 */
	if (!m_pMemMap)
	{
		CloseHandle(m_hMemMap);
		AfxMessageBox(_T("Failed to MapViewOfFile"), MB_ICONSTOP|MB_TOPMOST);
		/* 에러 코드 출력 */
		u32Error	= GetLastError();

		return FALSE;
	}

	/* 공유 메모리 초기화 */
	memset(m_pMemMap, 0x00, size);

	return TRUE;
}

/*
 desc : 공유 메모리 연결
 parm : key	- [in]  공유 메모리 유일한 Key Name (Mutex key Name과 유사)
 retn : TRUE or FALSE
*/
BOOL CMemMap::OpenMap(PTCHAR key)
{
	/* 운영체제의 물리적인 메모리 매핑 수행 */
	m_hMemMap	= OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE,	/* 물리적인 메모리 접근 속성 */
								  FALSE,
								  key);				/* 유일한 Key Name */
	if (!m_hMemMap)
	{
		AfxMessageBox(_T("Failed to OpenFileMapping"), MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 물리적인 저장 공간 연결 작업 */
	m_pMemMap	= (PUINT8)MapViewOfFile(m_hMemMap,				/* CreatFileMapping에 의해 생성된 핸들 */
										FILE_MAP_READ|FILE_MAP_WRITE/*FILE_MAP_ALL_ACCESS*/,	/* CreateFileMapping에서 READWRITE 했기 때문에 */
										0, 0, 0);				/* 0 값으로 설정하면, offset 부터 파일의 끝까지 */
	if (!m_pMemMap)
	{
		CloseHandle(m_hMemMap);
		AfxMessageBox(_T("Failed to MapViewOfFile"), MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 공유 메모리 연결
 parm : None
 retn : TRUE or FALSE
*/
VOID CMemMap::CloseMap()
{
	if (m_pMemMap)
	{
		UnmapViewOfFile(m_pMemMap);
		m_pMemMap	= NULL;
	}
	if (m_hMemMap)
	{
		CloseHandle(m_hMemMap);
		m_hMemMap	= NULL;
	}
}
