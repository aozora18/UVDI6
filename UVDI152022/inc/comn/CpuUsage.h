
#pragma once


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CCpuUsage
{
public:
	CCpuUsage();
	virtual ~CCpuUsage();
 
/* 로컬 변수 */
protected:

	/* CPU / Process 점유율 값 (0 ~ 100) */
	DOUBLE				m_dbRateCPU;
	DOUBLE				m_dbRateProc;

	FILETIME			m_stTimeCpuK;	/* Kernel Time for CPU */
	FILETIME			m_stTimeCpuU;	/* User Time for CPU */
	FILETIME			m_stTimeCpuI;	/* Idle Time for CPU */
	FILETIME			m_stTimeProcK;	/* Kernel Time for Process */
	FILETIME			m_stTimeProcU;	/* User Time for Process */


/* 로컬 함수 */
protected:

	UINT32				FileTimeDiff(FILETIME *old_t, FILETIME *new_t);

/* 공용 함수 */
public:

	BOOL				Init();
	BOOL				GetUsageRate(BOOL mode);					/* 전체 CPU 및 프로세스 사용률 */
	DOUBLE				GetRateCPU()	{	return m_dbRateCPU;	}
	DOUBLE				GetRateProc()	{	return m_dbRateProc;}
};