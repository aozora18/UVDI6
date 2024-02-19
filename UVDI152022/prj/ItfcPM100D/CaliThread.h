
#pragma once

#include "../../inc/comm/ClientThread.h"

#define ILLUM_MEAS_COUNT	20	/* About 20 measurements per second */

class CArrQueEx
{
/* ������ & �ı��� */
public:

	CArrQueEx();
	~CArrQueEx();

/* ���� ���� */
protected:

	DOUBLE				m_dbAvg;	/* Average */
	DOUBLE				m_dbStd;	/* Standard deviation */
	DOUBLE				m_dbDiff;	/* Max - Min */
	vector<DOUBLE>		m_vecValue;

/* public function */
public:

	inline BOOL			IsQueEmpty()			{	return INT32(m_vecValue.size()) < 1;	}
	inline BOOL			IsQueFulled()			{	return m_vecValue.size() == ILLUM_MEAS_COUNT;	}
	inline BOOL			IsQueFull(DOUBLE rate)	{	return rate < (DOUBLE(GetQueCount()) / DOUBLE(ILLUM_MEAS_COUNT));	}

	inline VOID			QueEmpty()				{	m_vecValue.clear();	/* !!! important !!! */ }
	inline VOID			PushQue(DOUBLE value);
	inline INT32		GetQueCount()			{	return INT32(m_vecValue.size());	}
	inline DOUBLE		GetAvg()				{	return m_dbAvg;		}
	inline DOUBLE		GetStd()				{	return m_dbStd;		}
	inline DOUBLE		GetDiff()				{	return m_dbDiff;	}

};

class CCaliThread : public CThinThread
{
// ������/�ı���
public:

	CCaliThread(LPG_PADV shmem, ENG_MDST type);
	virtual ~CCaliThread();

// �����Լ� ������
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

// ���� ����ü
protected:


// ���� ����
protected:

	ENG_MDST			m_enMeasType;		/* (0x01: Power, 0x02: Energy, 0x03: Frequency, 0x04: Density) */

	BOOL				m_bIsOpened;		/* Open ���� */
	BOOL				m_bRunMeasure;		/* ���� ���� ���� */

	UINT64				m_u64ConnectTime;	/* ���� �ֱ� �ð� ������ */

	ViSession			m_hDevPM100D;

	LPG_PADV			m_pstShMemPM100D;

	CArrQueEx			*m_pArrQue;
	CMySection			m_syncData;			// ����ȭ ��ü


// ���� �Լ�
protected:

	BOOL				OpenDev();
	VOID				CloseDev();

	BOOL				QueryDevID();
	BOOL				QuerySensorInfo();
	BOOL				QueryBeamDiameter();
	BOOL				QueryLineFrequency();

	BOOL				QueryTotal();

	/* ���� ��� ������ ���� ��û */
	BOOL				GetDevSetData();
	/* ��� (PM100D) �˻� */
	ViStatus			FindInstruments(ViString pattern, ViChar *res_name);
	BOOL				ErrosMessage(ViStatus err_cd);

	VOID				UpdatePM100DTime();


// ���� �Լ�
public:

	/* ���� ���� ���� */
	BOOL				IsConnected()			{	return m_bIsOpened;	};

	/* ��� �Ķ���� ���� */
	BOOL				QueryBeamDiameter(DOUBLE value);
	BOOL				QueryLineFrequency(INT16 value);
	BOOL				QuerySetDateTime();

	/* ���� ���� ������ ���� Full ������ ���, ��, Full - 1�� ��� */
	BOOL				IsQueFulled()			{	return m_pArrQue->IsQueFulled();	}
	/* ���� ������ ���� ������ ������ �� �ִ� ť�� ���� ���� ��� �󸶸�ŭ ����Ǿ� �ִ��� Ȯ�� */
	BOOL				IsQueFull(FLOAT rate)	{	return m_pArrQue->IsQueFull(rate);	}

	/* ���� �ð� ���� �߻��� ��� ���� �� �ʱ�ȭ */
	VOID				ResetValue();
	/* PM100D �κ��� ������ ���� ���� ���� */
	BOOL				RunMeasure(BOOL flag);
	BOOL				IsRunMeasure()			{	return IsConnected() && m_bRunMeasure;	}
	/* ���� ������ ���� ���� Ȯ�� */
	INT32				GetQueCount()			{	return m_pArrQue->GetQueCount();	}

	BOOL				QueryWaveLength(INT16 wave);
	INT16				QueryWaveLength();

	LPG_PSDV			GetSetupValue()			{	return &m_pstShMemPM100D->set;	}
	LPG_PGDV			GetDataValue()			{	return &m_pstShMemPM100D->get;	}
};