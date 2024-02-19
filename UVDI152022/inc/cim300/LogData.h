
#pragma once

using namespace ATL;

class CLogData
{
/* 생성자 & 파괴자 */
public:
	CLogData();
	~CLogData();

/* 구조체 변수 */
protected:

/* 로컬 변수 */
protected:

	PTCHAR				m_ptzErrMsg;
	CMyMutex			m_mtxData;	/* 데이터 동기화 객체 */
	CAtlList <STG_CLLM>	m_lstLogMesg;

/* 로컬 함수 */
protected:

	BOOL				GetResultError(HRESULT hr);
	VOID				GetErrorInfo(HRESULT hr);


/* 공용 함수 */
public:

	HRESULT				CheckResult(HRESULT hr, PTCHAR mesg);

	VOID				RemoveAll();
	VOID				PushLogs(PTCHAR mesg, ENG_GALC level=ENG_GALC::en_normal);
	VOID				PushLogs(PTCHAR mesg, LONG value, ENG_GALC level=ENG_GALC::en_normal);
	VOID				PushLogs(PTCHAR mesg1, PTCHAR mesg2, ENG_GALC level=ENG_GALC::en_normal);
	BOOL				PopLogs(STG_CLLM &mesg);

};

