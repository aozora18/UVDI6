
#pragma once

#include "../../inc/comn/ThinThread.h"
#include "../../inc/comm/SerialComm.h"

class CMilaraThread : public CThinThread
{
/* 생성자 & 파괴자 */
public:

	CMilaraThread(ENG_EDIC apps, UINT8 port, UINT32 baud, UINT32 buff);
	virtual ~ CMilaraThread();

/* 가상함수 재선언 */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

	virtual VOID		RecvData() = 0;
	virtual BOOL		Initialized() = 0;


public:



/* 로컬 변수 */
protected:

	/* 응답 패킷 중 필요 없는 응답 문자열 Skip 개수 (Bytes) */
	UINT8				m_u8ReplySkip;
	/* 시리얼 통신 포트 (1 ~ 255) */
	UINT8				m_u8Port;
	/* 초기화 단계 실행 인덱스 값 */
	UINT8				m_u8InitStep;

	ENG_EDIC			m_enAppsID;			/* Prealigner or Robot */
	ENG_MSAS			m_enLastSendAck;	/* 가장 최근에 송신한 명령에 대한 상태 값 (0x00: FAIL, 0x01: SUCC, 0xff: Not defined) */

	ENG_MRIM			m_enInfoMode;		/* 0x00 ~ 0x05 */

	BOOL				m_bIsInited;		/* 통신 작업할 준비가 되어 있는지 여부 */

	BOOL				m_bIsTechMode;		/* Teaching Mode인지 여부 */
	BOOL				m_bIsSend;			/* 통신 포트로부터 데이터 송신이 성공했는지 여부 */
	BOOL				m_bIsRecv;			/* 통신 포트로부터 데이터 수신이 성공했는지 여부 */

	UINT32				m_u32RecvSize;		/* 가장 최근에 수신된 명령어 크기 */

	UINT64				m_u64AckTimeWait;	/* 통신 송신 후 응답이 올 때까지 최대 대기하는 시간 */

	PUINT8				m_pCmdSend;			/* 내용이 포함된 송신 버퍼 */
	PUINT8				m_pCmdRecv;			/* 내용이 포함된 수신 버퍼 */

	/* 가장 최근에 수신된 패킷 임시 저장 */
	PCHAR				m_pszLastPkt;		/* 임시로 저장 */

	CMySection			m_syncComm;

	CSerialComm			*m_pComm;			/* 시리얼 통신 객체 */


/* 로컬 함수 */
protected:

	/* 통신 데이터 전송 */
	BOOL				SendData(PCHAR cmd, UINT32 size);

	/* 수신 데이터 반환 */
	BOOL				PopReadData(PUINT8 data, UINT32 &size);
	BOOL				CopyReadData(PUINT8 data, UINT32 &size);

	VOID				ParseAnalysis();
	VOID				CommClosed();	/* 시리얼 통신 해제 */
	VOID				SaveErrorToSerial(UINT8 type, PCHAR buff);

	VOID				WaitTimeToRecv();
	VOID				PutErrMsg(TCHAR *msg);

	VOID				GetValues(vector <string> &vals);
	UINT32				RleaseCommaToNum();

	VOID				SetValues(DOUBLE *host_val, vector <string> &vals);
	VOID				SetValues(UINT32 *host_val, vector <string> &vals);
	VOID				SetValues(INT32 *host_val, vector <string> &vals);
	VOID				SetValues(UINT16 *host_val, vector <string> &vals);
	VOID				SetValues(UINT8 *host_val, vector <string> &vals);
	VOID				SetValues(INT8 *host_val, vector <string> &vals);
	VOID				SetValues(INT16 *host_val, vector <string> &vals);

	VOID				SetValue(DOUBLE &host_val, vector <string> &vals);
	VOID				SetValue(INT32 &host_val, vector <string> &vals);
	VOID				SetValue(UINT32 &host_val, vector <string> &vals);
	VOID				SetValue(UINT16 &host_val, vector <string> &vals);
	VOID				SetValue(INT16 &host_val, vector <string> &vals);
	VOID				SetValue(UINT8 &host_val, vector <string> &vals);
	VOID				SetValue(INT8 &host_val, vector <string> &vals);
	VOID				SetValue(CHAR &host_val, vector <string> &vals);

	VOID				SetValueHex(UINT32 &host_val, vector <string> &vals);
	VOID				SetValueHex(UINT16 &host_val, vector <string> &vals);

	VOID				SetInch01Value(DOUBLE &host_val, vector <string> &vals);
	VOID				SetInch001Value(DOUBLE &host_val, vector <string> &vals);
	VOID				SetInch0001Value(DOUBLE &host_val, vector <string> &vals);
	VOID				SetInch0001Values(DOUBLE *host_val, vector <string> &vals);

	DOUBLE				GetInchToUm()	{	return (2.54 /* 1 inch -> 2.54 cm */ * 10000 /* cm -> um */);	}
	DOUBLE				GetInchToPos(ENG_RITU p_type, INT32 value);


/* 공용 함수 */
public:

	VOID				Stop();

	VOID				SetAckTimeout(UINT64 wait);

	UINT8				GetPort()		{	return m_u8Port;	}

	/* MPA가 초기화 되었는지 여부 */
	BOOL				IsInitialized()	{	return m_bIsInited;	}
	/* 시리얼 통신 포트 환경 설정 및 얻기 */
	BOOL				GetConfigPort(STG_SSCI &data);
	BOOL				SetConfigPort(LPG_SSCI data);

	BOOL				IsOpenPort();
	BOOL				IsSendData()	{	return m_bIsSend;	}
	BOOL				IsRecvData()	{	return m_bIsRecv;	}

	BOOL				SendData(PCHAR cmd);

	ENG_SCSC			GetCommStatus();	/* 현재 통신 상태 반환 */
	VOID				ResetCommData();	/* 기존 송신 및 수신 관련 통신 데이터 초기화 */

	/* 가장 최근에 송신한 명령어에 대한 응답 (상태) 값 */
	ENG_MSAS			GetLastSendAckStatus()	{	return m_enLastSendAck;	}
	BOOL				IsLastSendAckRecv()		{	return m_enLastSendAck != ENG_MSAS::none;	}
	BOOL				IsLastSendAckSucc()		{	return m_enLastSendAck == ENG_MSAS::succ;	}
	UINT32				GetRecvSize()			{	return m_u32RecvSize;	}

	/* 가장 최근에 수신된 패킷 반환 */
	PCHAR				GetLastRecvPkt()		{	return m_pszLastPkt;	}

	DOUBLE				GetAxisPosApp(ENG_RANC axis, INT32 value);
	INT32				GetPosToInch(ENG_RUTI p_type, DOUBLE value);
};