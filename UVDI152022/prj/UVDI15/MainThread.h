
#pragma once

#include "../../inc/comn/ThinThread.h"
#include "./work/WorkStep.h"

class LparamExtension
{
public:
	LparamExtension(LONG lParam, INT32 extensions)
	{
		this->lParam = lParam;
		this->extenstion = extensions;
	}
	LONG lParam;
	INT32 extenstion;
};

class CMainThread : public CThinThread
{
/* ������ / �ı��� */
public:
	CMainThread(HWND parent);
	virtual ~CMainThread();
	void DeleteCurrentJob();
/* ���� �Լ� ������ */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

/* ���� ���� */
protected:

	UINT8				m_u8StartButton;	/* ��� ���鿡 ������ �뱤 ���� ��ư */
	BOOL				m_bAppExit;			/* ���α׷� ���� �޽��� ���� ���� */

	UINT64				m_u64TickMesg;
	UINT64				m_u64TickFile;		/* ���� ���� �˻� �ֱ� */
	UINT64				m_u64TickTemp;		/* ��� ���� �µ� ������ ���� */
	UINT64				m_u64TickExit;		/* ���α׷� ���� ��� �ް�, ���� �ð� ��� ���� */
	UINT64				m_u64TickHotMon;	/* ������ �µ� ���� �ֱ� */
	UINT64				m_u64TickHotCtrl;	/* ������ �µ� ���� �ֱ� */

	CMySection			m_csSyncWork;
	CWorkStep			*m_pWorkJob;


/* ���� �Լ� */
protected:

	VOID				SendMesgParent(ENG_BWOK msg_id, UINT32 timeout=100, UINT32 lParamExtenstion=0);

	VOID				RunWorkJob();
	BOOL				ResetWorkJob();
	
	VOID				UpdateWorkName();

	BOOL				CheckStartButton();
	BOOL				IsSetAlignedMark();

	VOID				DeleteHistFiles();
	VOID				SaveTempFiles();

	VOID				SetAutoHotAir(UINT64 tick);


/* ���� �Լ� */
public:

	BOOL				RunWorkJob(ENG_BWOK job_id, PUINT64 data=NULL);
	BOOL				IsBusyWorkJob();

	ENG_BWOK			GetWorkJobID();

	VOID				SetExitApp(UINT64 delay);
};
