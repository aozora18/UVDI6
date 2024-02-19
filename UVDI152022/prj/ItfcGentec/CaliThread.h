
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
/* 생성자 & 파괴자 */
public:

	CArrQueEx();
	~CArrQueEx();

/* 로컬 변수 */
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
	// 생성자/파괴자
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

	// 가상함수 재정의
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

	// 로컬 구조체
protected:


	// 로컬 변수
protected:

	ENG_MDST			m_enMeasType;		/* (0x01: Power, 0x02: Energy, 0x03: Frequency, 0x04: Density) */

	BOOL				m_bIsOpened;		/* Open 여부 */
	BOOL				m_bRunMeasure;		/* 조도 측정 여부 */

	UINT64				m_u64ConnectTime;	/* 연결 주기 시간 때문에 */

	LPG_PADV			m_pstShMemGentec;

	CArrQueEx*			m_pArrQue;
	CMySection			m_syncData;			// 동기화 객체

	BOOL				m_bListen;

	std::string			m_strBuffer;

	// 로컬 함수
protected:

	BOOL				OpenDev();
	VOID				CloseDev();

	BOOL				QueryDevID();
	BOOL				QueryTotal();

	/* 현재 장비에 설정된 정보 요청 */
	BOOL				GetDevSetData();
	BOOL				ErrosMessage(char* pszMesg);

	// 공용 함수
public:

	/* 연결 여부 상태 */
	BOOL				IsConnected() 
	{ 
#if (DEF_COMMUNICATION_TYPE == DEF_SOCKET)
		return ChkConnect();
#elif (DEF_COMMUNICATION_TYPE == DEF_SERIAL)
		return IsOpen();
#endif
	};

	/* 조도 측정 개수가 거의 Full 상태인 경우, 즉, Full - 1인 경우 */
	BOOL				IsQueFulled() { return m_pArrQue->IsQueFulled(); }
	/* 현재 수집된 버퍼 비율이 저장할 수 있는 큐의 버퍼 공간 대비 얼마만큼 저장되어 있는지 확인 */
	BOOL				IsQueFull(FLOAT rate) { return m_pArrQue->IsQueFull(rate); }

	/* 임의 시간 동안 발생된 평균 측정 값 초기화 */
	VOID				ResetValue();
	/* Gentec 로부터 데이터 수집 여부 설정 */
	BOOL				RunMeasure(BOOL flag);
	BOOL				IsRunMeasure() { return IsConnected() && m_bRunMeasure; }
	/* 현재 수집된 버퍼 개수 확인 */
	INT32				GetQueCount() { return m_pArrQue->GetQueCount(); }

	BOOL				QueryWaveLength(INT16 wave);
	INT16				QueryWaveLength();

	LPG_PSDV			GetSetupValue() { return &m_pstShMemGentec->set; }
	LPG_PGDV			GetDataValue() { return &m_pstShMemGentec->get; }
};