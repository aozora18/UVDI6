
/*
 desc : ��� (Ȥ�� ��ġ) �⺻ ��ü
*/

#include "pch.h"
#include "MemMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : None
 retn : None
*/
CMemMap::CMemMap()
{
	m_hMemMap	= NULL;
	m_pMemMap	= NULL;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CMemMap::~CMemMap()
{
	if (m_pMemMap)	UnmapViewOfFile(m_pMemMap);
	if (m_hMemMap)	CloseHandle(m_hMemMap);
}

/*
 desc : ���� �޸� ���� �� ����
 parm : key		- [in]  ���� �޸� ������ Key Name (Mutex key Name�� ����)
		size	- [in]  ���� �޸� ���� ũ�� (����: Bytes. 8 Bytes ����� �Է��� ��� ��)
 retn : TRUE or FALSE
*/
BOOL CMemMap::CreateMap(PTCHAR key, UINT32 size)
{
	UINT32 u32Size	= size, u32Error = 0;

	/* �޸� �Ҵ� ũ�� �� �缳�� (64 bit �ü�������� 8 Bytes ������ �Ҵ�) */
	if ((u32Size % 8) != 0)	u32Size	= (u32Size / 8) * u32Size + 8;

	/* �ü���� �������� �޸� ���� ���� */
	m_hMemMap	= CreateFileMapping(INVALID_HANDLE_VALUE,	/* �ϵ� ��ũ�� �����Ϸ��� ���� �ڵ� �־��ָ� ��*/
									NULL,					/* ���� ���� �κ��̹Ƿ�, Pass */
									PAGE_READWRITE,			/* �������� �޸� �Ӽ� */
									0,						/* ��뷮 �޸� ������ ��, ���� ��, 4 GB �̻� ����ϴ� ����� */
									size,
									key);					/* ������ Key Name */
	if (!m_hMemMap)
	{
		AfxMessageBox(_T("Failed to CreateFileMapping"), MB_ICONSTOP|MB_TOPMOST);
		/* ���� �ڵ� ��� */
		u32Error	= GetLastError();

		return FALSE;
	}

	/* �������� ���� ���� ���� �۾� */
	m_pMemMap	= (PUINT8)MapViewOfFile(m_hMemMap,				/* CreatFileMapping�� ���� ������ �ڵ� */
										FILE_MAP_ALL_ACCESS,	/* CreateFileMapping���� READWRITE �߱� ������ */
										0, 0, 0);				/* 0 ������ �����ϸ�, offset ���� ������ ������ */
	if (!m_pMemMap)
	{
		CloseHandle(m_hMemMap);
		AfxMessageBox(_T("Failed to MapViewOfFile"), MB_ICONSTOP|MB_TOPMOST);
		/* ���� �ڵ� ��� */
		u32Error	= GetLastError();

		return FALSE;
	}

	/* ���� �޸� �ʱ�ȭ */
	memset(m_pMemMap, 0x00, size);

	return TRUE;
}

/*
 desc : ���� �޸� ����
 parm : key	- [in]  ���� �޸� ������ Key Name (Mutex key Name�� ����)
 retn : TRUE or FALSE
*/
BOOL CMemMap::OpenMap(PTCHAR key)
{
	/* �ü���� �������� �޸� ���� ���� */
	m_hMemMap	= OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE,	/* �������� �޸� ���� �Ӽ� */
								  FALSE,
								  key);				/* ������ Key Name */
	if (!m_hMemMap)
	{
		AfxMessageBox(_T("Failed to OpenFileMapping"), MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* �������� ���� ���� ���� �۾� */
	m_pMemMap	= (PUINT8)MapViewOfFile(m_hMemMap,				/* CreatFileMapping�� ���� ������ �ڵ� */
										FILE_MAP_READ|FILE_MAP_WRITE/*FILE_MAP_ALL_ACCESS*/,	/* CreateFileMapping���� READWRITE �߱� ������ */
										0, 0, 0);				/* 0 ������ �����ϸ�, offset ���� ������ ������ */
	if (!m_pMemMap)
	{
		CloseHandle(m_hMemMap);
		AfxMessageBox(_T("Failed to MapViewOfFile"), MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : ���� �޸� ����
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
