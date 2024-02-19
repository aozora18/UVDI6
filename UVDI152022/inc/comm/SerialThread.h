
#pragma once

#include "../comn/ThinThread.h"
#include "SerialComm.h"

class CSerialThread : public CThinThread
{
/* 생성자 & 파괴자 */
public:

	CSerialThread(UINT8 port, UINT32 baud, UINT32 buff, ENG_SHOT shake);
	virtual ~ CSerialThread();

/* 가상함수 재선언 */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();


/* 로컬 변수 */
protected:

	/* 시리얼 통신 포트 (1 ~ 255) */
	UINT8				m_u8Port;

	BOOL				m_bIsReady;		// 통신 포트로부터 데이터 송/수신 작업 준비가 되었는지 여부
	BOOL				m_bIsSend;		// 통신 포트로 데이터 송신이 성공했는지 여부

	CMySection			m_syncComm;

	CSerialComm			*m_pComm;		// 시리얼 통신 객체

// 로컬 함수
protected:



// 공용 함수
public:

	VOID				Stop();
	VOID				PutErrMsg(TCHAR *msg);
	PTCHAR				GetErrMsg();

	/* 시리얼 통신 포트 번호 반환 */
	UINT8				GetPort()		{	return m_u8Port;	}

	/* 시리얼 통신 포트 환경 설정 및 얻기 */
	BOOL				GetConfigPort(STG_SSCI &data);
	BOOL				SetConfigPort(LPG_SSCI data);

	/* 통신 상태 정보 반환 */
	BOOL				IsOpenPort();
	/* 현재 패킷 전송 가능한 상태인지 여부 값 반환 */
	BOOL				IsSendData()	{	return m_bIsSend;	}

	/* 통신 데이터 전송 */
	BOOL				WriteData(PUINT8 data, UINT32 &size);
	/* 수신 데이터 반환 */
	UINT32				GetReadSize();
	BOOL				PopReadData(PUINT8 data, UINT32 &size);
	BOOL				CopyReadData(PUINT8 data, UINT32 &size);

	/* 시리얼 통신 해제 */
	VOID				CommClosed();

	/* 현재 통신 상태 반환 */
	ENG_SCSC			GetCommStatus();
};