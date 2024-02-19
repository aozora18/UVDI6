
/*
 desc : 현재 시스템의 CPU 부하율 구하기
*/

#include "pch.h"
#include "CpuUsage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : 생성자
 parm : None
 retn : None
*/
CCpuUsage::CCpuUsage()
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CCpuUsage::~CCpuUsage()
{
} 
 
/*
 desc : 현재 커널, 유휴, 유저 등의 시간 정보 얻기
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
 desc : 과거 시간과 현재 시간의 차이 값 계산 후 반환
 parm : old_t	- [in]  과거 시간 값
		new_t	- [in]  현재 시간 값
 retn : 시간 오차 값 반환
*/
UINT32 CCpuUsage::FileTimeDiff(FILETIME *old_t, FILETIME *new_t)
{
	UINT32 u32Time;

	if (new_t->dwHighDateTime > old_t->dwHighDateTime)
	{
		// dwHighDateTime 가 1 증가한 경우만 처리한다.
		u32Time = UINT32_MAX - old_t->dwLowDateTime + new_t->dwLowDateTime;
	}
	else
	{
		u32Time = new_t->dwLowDateTime - old_t->dwLowDateTime;
	}

	return u32Time;
}

/*
 desc : 전체 CPU 및 프로세스의 점유율 값 반환
 parm : mode	- [in]  프로세스의 점유율 구할지 여부
 retn : 점유율 반환 (0 ~ 100.0f %)
*/
BOOL CCpuUsage::GetUsageRate(BOOL mode)
{
	UINT32 u32TotalTime, u32IdleTime, u32ProcTime;
	FILETIME stTimeK, stTimeU, stTimeI, stTimeC, stTimeE;

	if (!GetSystemTimes(&stTimeI, &stTimeK, &stTimeU))	return FALSE;

	/* 전체 동작 시간 및 유휴 시간 값 구하기 */
	u32TotalTime	= FileTimeDiff(&m_stTimeCpuK, &stTimeK);
	u32TotalTime	+= FileTimeDiff(&m_stTimeCpuU, &stTimeU);
	u32IdleTime		= FileTimeDiff(&m_stTimeCpuI, &stTimeI);

	/* CPU 점유율 값 계산 */
	m_dbRateCPU		= (u32TotalTime - u32IdleTime) * 100.0f / u32TotalTime;

	/* 가장 최근 시간 값 저장 */
	m_stTimeCpuK	= stTimeK;
	m_stTimeCpuU	= stTimeU;
	m_stTimeCpuI	= stTimeI;

	/* 프로세스의 점유율 값 구하기 */
	if (mode)
	{
		if (GetProcessTimes(GetCurrentProcess(), &stTimeC, &stTimeE, &stTimeK, &stTimeU))
		{
			/* 프로세스 동작 시간 및 유휴 시간 값 구하기 */
			u32ProcTime	= FileTimeDiff(&m_stTimeProcK, &stTimeK);
			u32ProcTime	+= FileTimeDiff(&m_stTimeProcU, &stTimeU);

			/* 프로세스 점유율 값 계산 */
			m_dbRateProc	= (m_dbRateCPU * u32ProcTime) / u32TotalTime;

			/* 가장 최근 시간 값 저장 */
			m_stTimeProcK	= stTimeK;
			m_stTimeProcU	= stTimeU;
		}
	}

	return TRUE;
}