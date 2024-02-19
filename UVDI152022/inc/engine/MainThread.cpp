
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

	/* �µ� �� �ʱ�ȭ */
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
 desc : ������ ���۽ÿ� �ʱ� �ѹ� ȣ���
 parm : None
 retn : DI_TRUE or DI_FALSE
*/
BOOL CMainThread::StartWork()
{
	/* ������ 1 �� ���� ���� �����ϵ��� �� */
	CThinThread::Sleep(1000);

	return TRUE;
}

/*
 desc : ������ ���۽ÿ� �ֱ������� ȣ���
 parm : None
 retn : None
*/
VOID CMainThread::RunWork()
{
	UINT64 u64Tick	= GetTickCount64();

	/* �ֱ������� ȣ�� */
	if (u64Tick > m_u64TickNormal + 100)
	{
		m_u64TickNormal	= u64Tick;
		ProcNormal();
	}
}

/*
 desc : ������ ����ÿ� ���������� �ѹ� ȣ���
 parm : None
 retn : None
*/
VOID CMainThread::EndWork()
{
}

/*
 desc : �Ϲ����� �۾� ó��
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
 desc : DI ���� �µ� ������ ����
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

	/* ���� DI ���� �µ� �� ��� */
	for (; i<4; i++)
	{
		u32Addr	= (u32PLCAddr[i] >> 4);	/* ���� 4 bit�� ���, ��Ʈ ��ġ ���̹Ƿ� */
		/* ���� ��� ���� �µ� (���� ���� ���߰ų�, ���� ���� �����ϸ� ó������ ���� */
		// by sysandj : MCQ��ü �߰� �ʿ� : m_pShMemPLC->GetDWordValue(u32Addr, u32TempDI);
		if (u32TempDI > 0 && u32TempDI != m_u32TempDI[i])
		{
			/* ���� �ֱٿ� �߻��� �µ� �� �ӽ� ���� */
			m_u32TempDI[i]	= u32TempDI;

			/* ���� ����� �µ� �� ��� */
			m_lstTempDI[i].AddTail(u32TempDI);
			/* ���� ��ϵ� ��� */
			u32Count	= (UINT32)m_lstTempDI[i].GetCount();
			/* ������� ������ �µ� �� ���� ���ϱ� */
			m_u64TempDITot[i]	+= u32TempDI;
			/* ���� ��ϵ� ������ �ʰ��Ǿ�����, ���� ������ �׸� 1�� ���� */
			if (MAX_COLLECT_TEMP_DI < u32Count)
			{
				/* ���տ��� ������ �µ� �� ���� */
				m_u64TempDITot[i]	-= m_lstTempDI[i].GetTail();
				/* ���� ������ �µ� �� ���� */
				m_lstTempDI[i].RemoveTail();
				u32Count	= MAX_COLLECT_TEMP_DI;
			}

			/* ���, �ּ�, �ִ� ��� */
			if (m_u32TempDIMin[i] > u32TempDI)	m_u32TempDIMin[i] = u32TempDI;
			if (m_u32TempDIMax[i] < u32TempDI)	m_u32TempDIMax[i] = u32TempDI;
			if (u32Count > 1)	m_dbTempDIAvg[i] = (DOUBLE(m_u64TempDITot[i]) / u32Count) / 1000.0f;
		}
	}
}