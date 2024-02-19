
#pragma once


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CCpuUsage
{
public:
	CCpuUsage();
	virtual ~CCpuUsage();
 
/* ���� ���� */
protected:

	/* CPU / Process ������ �� (0 ~ 100) */
	DOUBLE				m_dbRateCPU;
	DOUBLE				m_dbRateProc;

	FILETIME			m_stTimeCpuK;	/* Kernel Time for CPU */
	FILETIME			m_stTimeCpuU;	/* User Time for CPU */
	FILETIME			m_stTimeCpuI;	/* Idle Time for CPU */
	FILETIME			m_stTimeProcK;	/* Kernel Time for Process */
	FILETIME			m_stTimeProcU;	/* User Time for Process */


/* ���� �Լ� */
protected:

	UINT32				FileTimeDiff(FILETIME *old_t, FILETIME *new_t);

/* ���� �Լ� */
public:

	BOOL				Init();
	BOOL				GetUsageRate(BOOL mode);					/* ��ü CPU �� ���μ��� ���� */
	DOUBLE				GetRateCPU()	{	return m_dbRateCPU;	}
	DOUBLE				GetRateProc()	{	return m_dbRateProc;}
};