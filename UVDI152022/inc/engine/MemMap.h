#pragma once


/* --------------------------------------------------------------------------------------------- */
/*                                  Shared Memory - Map Object                                   */
/* --------------------------------------------------------------------------------------------- */

class CMemMap
{
public:

	CMemMap();
	virtual ~CMemMap();


/* ��� ���� */
protected:

	HANDLE				m_hMemMap;
	PUINT8				m_pMemMap;


/* ���� �Լ� */
public:

	BOOL				CreateMap(PTCHAR key, UINT32 size);
	BOOL				OpenMap(PTCHAR key);
	VOID				CloseMap();

	PUINT8				GetMemMap()	{	return m_pMemMap;	}	/* ������ �޸� ������ ��ȯ */
};
