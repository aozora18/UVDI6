
/*
 desc : Main Thread
*/

#include "pch.h"
#include "MainThread.h"

/* Shared Memory */
#if (CUSTOM_CODE_UVDI15_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_UVDI15_LLS06 == DELIVERY_PRODUCT_ID)
#include "MemUvdi15.h"
#elif (CUSTOM_CODE_GEN2I == DELIVERY_PRODUCT_ID)
#include "MemGen2i.h"
#elif (CUSTOM_CODE_UVDI15 == DELIVERY_PRODUCT_ID || \
CUSTOM_CODE_HDDI6== DELIVERY_PRODUCT_ID)
#include "MemUVDI15.h"

#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
#define MAX_COLLECT_TEMP_DI	128
#else
#define MAX_COLLECT_TEMP_DI	512
#endif

/*
 desc : Constructor
 parm : mem_plc	- [in]  Shared Memory Object (for PLC)
 retn : None
*/
CMainThread::CMainThread()
	: CThinThread()
{
	m_u64TickNormal	= 0;
	m_u8NormalNext	= 0;

	/* Shared Memory Object */

	/* 온도 값 초기화 */
	memset(m_u32TempDI,		0x00, sizeof(UINT32) * 4);
	memset(m_u32TempDIMin,	0xff, sizeof(UINT64) * 4);
	memset(m_u32TempDIMax,	0x00, sizeof(UINT64) * 4);
	memset(m_u64TempDITot,	0x00, sizeof(UINT64) * 4);
	memset(m_dbTempDIAvg,	0x00, sizeof(DOUBLE) * 4);
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CMainThread::~CMainThread()
{
	for (UINT8 i=0x00; i<4; i++)	m_lstTempDI[i].RemoveAll();
}

/*
 desc : 스레드 동작시에 초기 한번 호출됨
 parm : None
 retn : DI_TRUE or DI_FALSE
*/
BOOL CMainThread::StartWork()
{
	/* 무조건 1 초 정도 쉬고 동작하도록 함 */
	CThinThread::Sleep(1000);

	return TRUE;
}

/*
 desc : 스레드 동작시에 주기적으로 호출됨
 parm : None
 retn : None
*/
VOID CMainThread::RunWork()
{
	UINT64 u64Tick	= GetTickCount64();

	/* 주기적으로 호출 */
	if (u64Tick > m_u64TickNormal + 100)
	{
		m_u64TickNormal	= u64Tick;
		ProcNormal();
	}
}

/*
 desc : 스레드 종료시에 마지막으로 한번 호출됨
 parm : None
 retn : None
*/
VOID CMainThread::EndWork()
{
}

/*
 desc : 일반적인 작업 처리
 parm : None
 retn : None
*/
VOID CMainThread::ProcNormal()
{
	switch (m_u8NormalNext)
	{
	case 0x00	:	GetTempDI();	break;
	}
	if (++m_u8NormalNext == 1)	m_u8NormalNext	= 0x00;
}

/*
 desc : DI 내부 온도 데이터 수집
 parm : None
 retn : TRUE or FALSE
*/
VOID CMainThread::GetTempDI()
{
	UINT32 u32PLCAddr[4]= { UINT32(ENG_PIOA::en_internal_monitoring_temp),
							UINT32(ENG_PIOA::en_internal_monitoring_temp_2_only_4th),
							UINT32(ENG_PIOA::en_internal_monitoring_temp_3_only_4th),
							UINT32(ENG_PIOA::en_internal_monitoring_temp_4_only_4th) };
	UINT32 u32TempDI	= 0, i = 0, u32Count, u32Addr;

	/* 현재 DI 내부 온도 값 얻기 */
	for (; i<4; i++)
	{
		u32Addr	= (u32PLCAddr[i] >> 4);	/* 하위 4 bit의 경우, 비트 위치 값이므로 */
		/* 현재 장비 내부 온도 (값을 읽지 못했거나, 이전 값과 동일하면 처리하지 않음 */
		// by sysandj : MCQ대체 추가 필요 : m_pShMemPLC->GetDWordValue(u32Addr, u32TempDI);
		if (u32TempDI > 0 && u32TempDI != m_u32TempDI[i])
		{
			/* 가장 최근에 발생된 온도 값 임시 저장 */
			m_u32TempDI[i]	= u32TempDI;

			/* 새로 변경된 온도 값 등록 */
			m_lstTempDI[i].AddTail(u32TempDI);
			/* 현재 등록된 계수 */
			u32Count	= (UINT32)m_lstTempDI[i].GetCount();
			/* 현재까지 수집된 온도 값 총합 구하기 */
			m_u64TempDITot[i]	+= u32TempDI;
			/* 현재 등록된 개수가 초과되었으면, 가장 오래된 항목 1개 제거 */
			if (MAX_COLLECT_TEMP_DI < u32Count)
			{
				/* 총합에서 마지막 온도 값 제외 */
				m_u64TempDITot[i]	-= m_lstTempDI[i].GetTail();
				/* 가장 오래된 온도 값 제거 */
				m_lstTempDI[i].RemoveTail();
				u32Count	= MAX_COLLECT_TEMP_DI;
			}

			/* 평균, 최소, 최대 계산 */
			if (m_u32TempDIMin[i] > u32TempDI)	m_u32TempDIMin[i] = u32TempDI;
			if (m_u32TempDIMax[i] < u32TempDI)	m_u32TempDIMax[i] = u32TempDI;
			if (u32Count > 1)	m_dbTempDIAvg[i] = (DOUBLE(m_u64TempDITot[i]) / u32Count) / 1000.0f;
		}
	}
}