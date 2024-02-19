#pragma once


/* --------------------------------------------------------------------------------------------- */
/*                                  Shared Memory - Map Object                                   */
/* --------------------------------------------------------------------------------------------- */

class CMemMap
{
public:

	CMemMap();
	virtual ~CMemMap();


/* 멤버 변수 */
protected:

	HANDLE				m_hMemMap;
	PUINT8				m_pMemMap;


/* 공용 함수 */
public:

	BOOL				CreateMap(PTCHAR key, UINT32 size);
	BOOL				OpenMap(PTCHAR key);
	VOID				CloseMap();

	PUINT8				GetMemMap()	{	return m_pMemMap;	}	/* 공유된 메모리 포인터 반환 */
};
