
/*
 desc : ���� �ý����� CPU ������ ���ϱ�
*/

#include "pch.h"
#include "CpuUsage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : ������
 parm : None
 retn : None
*/
CCpuUsage::CCpuUsage()
{
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CCpuUsage::~CCpuUsage()
{
} 
 
/*
 desc : ���� Ŀ��, ����, ���� ���� �ð� ���� ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CCpuUsage::Init()
{
	FILETIME stTimeC, stTimeE;
	if (!GetSystemTimes(&m_stTimeCpuI, &m_stTimeCpuK, &m_stTimeCpuU))	return FALSE;
	if (!GetProcessTimes(GetCurrentProcess(),
						 &stTimeC, &stTimeE, &m_stTimeProcK, &m_stTimeProcU))	return FALSE;
	return TRUE;
}

/*
 desc : ���� �ð��� ���� �ð��� ���� �� ��� �� ��ȯ
 parm : old_t	- [in]  ���� �ð� ��
		new_t	- [in]  ���� �ð� ��
 retn : �ð� ���� �� ��ȯ
*/
UINT32 CCpuUsage::FileTimeDiff(FILETIME *old_t, FILETIME *new_t)
{
	UINT32 u32Time;

	if (new_t->dwHighDateTime > old_t->dwHighDateTime)
	{
		// dwHighDateTime �� 1 ������ ��츸 ó���Ѵ�.
		u32Time = UINT32_MAX - old_t->dwLowDateTime + new_t->dwLowDateTime;
	}
	else
	{
		u32Time = new_t->dwLowDateTime - old_t->dwLowDateTime;
	}

	return u32Time;
}

/*
 desc : ��ü CPU �� ���μ����� ������ �� ��ȯ
 parm : mode	- [in]  ���μ����� ������ ������ ����
 retn : ������ ��ȯ (0 ~ 100.0f %)
*/
BOOL CCpuUsage::GetUsageRate(BOOL mode)
{
	UINT32 u32TotalTime, u32IdleTime, u32ProcTime;
	FILETIME stTimeK, stTimeU, stTimeI, stTimeC, stTimeE;

	if (!GetSystemTimes(&stTimeI, &stTimeK, &stTimeU))	return FALSE;

	/* ��ü ���� �ð� �� ���� �ð� �� ���ϱ� */
	u32TotalTime	= FileTimeDiff(&m_stTimeCpuK, &stTimeK);
	u32TotalTime	+= FileTimeDiff(&m_stTimeCpuU, &stTimeU);
	u32IdleTime		= FileTimeDiff(&m_stTimeCpuI, &stTimeI);

	/* CPU ������ �� ��� */
	m_dbRateCPU		= (u32TotalTime - u32IdleTime) * 100.0f / u32TotalTime;

	/* ���� �ֱ� �ð� �� ���� */
	m_stTimeCpuK	= stTimeK;
	m_stTimeCpuU	= stTimeU;
	m_stTimeCpuI	= stTimeI;

	/* ���μ����� ������ �� ���ϱ� */
	if (mode)
	{
		if (GetProcessTimes(GetCurrentProcess(), &stTimeC, &stTimeE, &stTimeK, &stTimeU))
		{
			/* ���μ��� ���� �ð� �� ���� �ð� �� ���ϱ� */
			u32ProcTime	= FileTimeDiff(&m_stTimeProcK, &stTimeK);
			u32ProcTime	+= FileTimeDiff(&m_stTimeProcU, &stTimeU);

			/* ���μ��� ������ �� ��� */
			m_dbRateProc	= (m_dbRateCPU * u32ProcTime) / u32TotalTime;

			/* ���� �ֱ� �ð� �� ���� */
			m_stTimeProcK	= stTimeK;
			m_stTimeProcU	= stTimeU;
		}
	}

	return TRUE;
}