
#pragma once

#define DEF_SOCKET	1
#define DEF_SERIAL	2
#define DEF_COMMUNICATION_TYPE		DEF_SERIAL

#include "../../inc/comn/ThinThread.h"
#include "SerialComm.h"
#include "SocketComm.h"

#define ILLUM_MEAS_COUNT	20	/* About 20 measurements per second */
#define DEF_CMD_END			"\r\n"

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

#if (DEF_COMMUNICATION_TYPE == DEF_SOCKET)
class CCaliThread : public CThinThread, CSocketCComm
#elif (DEF_COMMUNICATION_TYPE == DEF_SERIAL)
class CCaliThread : public CThinThread, CSerialComm
#endif
{
	// ������/�ı���
public:

	CCaliThread(UINT32 comPort,LPG_PADV shmem, ENG_MDST type);
	CCaliThread(UINT32 source_ip, UINT32 target_ip, UINT16 u16Port, LPG_PADV shmem, ENG_MDST type);
	virtual ~CCaliThread();

#if (DEF_COMMUNICATION_TYPE == DEF_SOCKET)
	long				RecvProcess(long lSize, BYTE* pbyData);
	BOOL				WaitReply(char* pszRecv, DWORD dwTimeout);

	BOOL				ReadData(CString strSend, char* pchRead, DWORD dwTimeout = 3000);
#elif (DEF_COMMUNICATION_TYPE == DEF_SERIAL)
	BOOL				WaitResponse(char* pchRead, DWORD dwTimeout = 3000);
#endif

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

	LPG_PADV			m_pstShMemGentec;

	CArrQueEx*			m_pArrQue;
	CMySection			m_syncData;			// ����ȭ ��ü

	BOOL				m_bListen;

	std::string			m_strBuffer;

	// ���� �Լ�
protected:

	BOOL				OpenDev();
	VOID				CloseDev();

	BOOL				QueryDevID();
	BOOL				QueryTotal();

	/* ���� ��� ������ ���� ��û */
	BOOL				GetDevSetData();
	BOOL				ErrosMessage(char* pszMesg);

	// ���� �Լ�
public:

	/* ���� ���� ���� */
	BOOL				IsConnected() 
	{ 
#if (DEF_COMMUNICATION_TYPE == DEF_SOCKET)
		return ChkConnect();
#elif (DEF_COMMUNICATION_TYPE == DEF_SERIAL)
		return IsOpen();
#endif
	};

	/* ���� ���� ������ ���� Full ������ ���, ��, Full - 1�� ��� */
	BOOL				IsQueFulled() { return m_pArrQue->IsQueFulled(); }
	/* ���� ������ ���� ������ ������ �� �ִ� ť�� ���� ���� ��� �󸶸�ŭ ����Ǿ� �ִ��� Ȯ�� */
	BOOL				IsQueFull(FLOAT rate) { return m_pArrQue->IsQueFull(rate); }

	/* ���� �ð� ���� �߻��� ��� ���� �� �ʱ�ȭ */
	VOID				ResetValue();
	/* Gentec �κ��� ������ ���� ���� ���� */
	BOOL				RunMeasure(BOOL flag);
	BOOL				IsRunMeasure() { return IsConnected() && m_bRunMeasure; }
	/* ���� ������ ���� ���� Ȯ�� */
	INT32				GetQueCount() { return m_pArrQue->GetQueCount(); }

	BOOL				QueryWaveLength(INT16 wave);
	INT16				QueryWaveLength();

	LPG_PSDV			GetSetupValue() { return &m_pstShMemGentec->set; }
	LPG_PGDV			GetDataValue() { return &m_pstShMemGentec->get; }
};