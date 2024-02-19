
#pragma once

#include "../../inc/comn/ThinThread.h"

class CMainThread : public CThinThread
{
/* Constructor / Destructor */
public:
	CMainThread();
	virtual ~CMainThread();

/* Override the virtual function */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();


/* Local parameters */
protected:

	UINT8				m_u8NormalNext;

	UINT64				m_u64TickNormal;

	/* 현재 수집된 DI 내부 온도 */
	UINT32				m_u32TempDI[4];		/* 가장 최근 온도 값 */
	UINT32				m_u32TempDIMin[4];	/* 최소 값 */
	UINT32				m_u32TempDIMax[4];	/* 최대 값 */
	UINT64				m_u64TempDITot[4];	/* 전체 온도 합계 */
	DOUBLE				m_dbTempDIAvg[4];	/* 평균 값 */

	CAtlList <UINT32>	m_lstTempDI[4];


/* Local functions */
protected:

	VOID				ProcNormal();
	VOID				GetTempDI();

/* Public functions */
public:

	DOUBLE				GetLastTempDI(UINT8 index)	{	return DOUBLE(m_u32TempDI[index]/1000.0f);		}
	DOUBLE				GetTempDIMin(UINT8 index)	{	return DOUBLE(m_u32TempDIMin[index]/1000.0f);	}
	DOUBLE				GetTempDIMax(UINT8 index)	{	return DOUBLE(m_u32TempDIMax[index]/1000.0f);	}
	DOUBLE				GetTempDIAvg(UINT8 index)	{	return m_dbTempDIAvg[index];					}
};