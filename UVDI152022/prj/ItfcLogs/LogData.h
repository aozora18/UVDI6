#pragma once

#include "Base.h"

#ifdef _DEBUG
#define LAST_MESG_COUNT	8
#else
#define LAST_MESG_COUNT	32
#endif
class CLogData : public CBase
{
/* ������ & �ı��� */
public:

	CLogData(BOOL saved, UINT8 type, ENG_LNWE mode);
	virtual ~CLogData();


/* ���� ���� */
protected:

	UINT8				m_u8LastError;		/* ���� �ֱٿ� �߻��� ���� ���� ��ġ */

	PTCHAR				m_ptzMsgLog;		/* �ֱ� �߻��� �α� ������ ���� �� ���� */
	PTCHAR				m_ptzMsgErr;		/* �ֱ� �߻��� �α� ������ ���� �� ���� */
	PTCHAR				*m_ptzMsgPtr;		/* 1���� �迭�� 2���� �迭ó�� ����ϱ� ���� */

	UINT64				m_u64LastTick;		/* ���� �ֱٿ� ����� �α� �ð� */

	ENG_LNWE			m_enErrorLevelLast;	/* Log Level (Normal, Warn, Error, etc) */
	PUINT8				m_pLogType;			/* ���� �߻��� �α��� ���� �ӽ� ���� */

/* ���� �Լ� */
protected:

	ENG_EDIT			GetAppsType(ENG_EDIC apps);


/* ���� �Լ� */
public:

	VOID				SaveLogs(ENG_EDIC apps, ENG_LNWE level, PTCHAR mesg,
								 PTCHAR file, PTCHAR func, UINT32 line);
	VOID				ResetErrorMesg();
	PTCHAR				GetErrorMesgLast();
	PTCHAR				GetErrorMesg(UINT8 index);
	ENG_LNWE			GetErrorLevelLast()	{	return m_enErrorLevelLast;	}
	BOOL				IsErrorMesg();
};
