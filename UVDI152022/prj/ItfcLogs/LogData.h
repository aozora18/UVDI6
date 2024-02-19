#pragma once

#include "Base.h"

#ifdef _DEBUG
#define LAST_MESG_COUNT	8
#else
#define LAST_MESG_COUNT	32
#endif
class CLogData : public CBase
{
/* 생성자 & 파괴자 */
public:

	CLogData(BOOL saved, UINT8 type, ENG_LNWE mode);
	virtual ~CLogData();


/* 로컬 변수 */
protected:

	UINT8				m_u8LastError;		/* 가장 최근에 발생된 에러 저장 위치 */

	PTCHAR				m_ptzMsgLog;		/* 최근 발생된 로그 데이터 저장 및 관리 */
	PTCHAR				m_ptzMsgErr;		/* 최근 발생된 로그 데이터 저장 및 관리 */
	PTCHAR				*m_ptzMsgPtr;		/* 1차원 배열을 2차원 배열처럼 사용하기 위함 */

	UINT64				m_u64LastTick;		/* 가장 최근에 저장된 로그 시간 */

	ENG_LNWE			m_enErrorLevelLast;	/* Log Level (Normal, Warn, Error, etc) */
	PUINT8				m_pLogType;			/* 과거 발생된 로그의 성격 임시 저장 */

/* 로컬 함수 */
protected:

	ENG_EDIT			GetAppsType(ENG_EDIC apps);


/* 공용 함수 */
public:

	VOID				SaveLogs(ENG_EDIC apps, ENG_LNWE level, PTCHAR mesg,
								 PTCHAR file, PTCHAR func, UINT32 line);
	VOID				ResetErrorMesg();
	PTCHAR				GetErrorMesgLast();
	PTCHAR				GetErrorMesg(UINT8 index);
	ENG_LNWE			GetErrorLevelLast()	{	return m_enErrorLevelLast;	}
	BOOL				IsErrorMesg();
};
