
#pragma once

using namespace ATL;

class CLogData
{
/* ������ & �ı��� */
public:
	CLogData();
	~CLogData();

/* ����ü ���� */
protected:

/* ���� ���� */
protected:

	PTCHAR				m_ptzErrMsg;
	CMyMutex			m_mtxData;	/* ������ ����ȭ ��ü */
	CAtlList <STG_CLLM>	m_lstLogMesg;

/* ���� �Լ� */
protected:

	BOOL				GetResultError(HRESULT hr);
	VOID				GetErrorInfo(HRESULT hr);


/* ���� �Լ� */
public:

	HRESULT				CheckResult(HRESULT hr, PTCHAR mesg);

	VOID				RemoveAll();
	VOID				PushLogs(PTCHAR mesg, ENG_GALC level=ENG_GALC::en_normal);
	VOID				PushLogs(PTCHAR mesg, LONG value, ENG_GALC level=ENG_GALC::en_normal);
	VOID				PushLogs(PTCHAR mesg1, PTCHAR mesg2, ENG_GALC level=ENG_GALC::en_normal);
	BOOL				PopLogs(STG_CLLM &mesg);

};

