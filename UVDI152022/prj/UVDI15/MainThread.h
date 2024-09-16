
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
/* 생성자 / 파괴자 */
public:
	CMainThread(HWND parent);
	virtual ~CMainThread();
	void DeleteCurrentJob();
/* 가상 함수 재정의 */
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

/* 로컬 변수 */
protected:

	UINT8				m_u8StartButton;	/* 장비 전면에 부착된 노광 시작 버튼 */
	BOOL				m_bAppExit;			/* 프로그램 종료 메시지 전달 여부 */

	UINT64				m_u64TickMesg;
	UINT64				m_u64TickFile;		/* 파일 삭제 검사 주기 */
	UINT64				m_u64TickTemp;		/* 장비 내부 온도 데이터 저장 */
	UINT64				m_u64TickExit;		/* 프로그램 종료 명령 받고, 일정 시간 대기 위해 */
	UINT64				m_u64TickHotMon;	/* 온조기 온도 감시 주기 */
	UINT64				m_u64TickHotCtrl;	/* 온조기 온도 제어 주기 */

	CMySection			m_csSyncWork;
	CWorkStep			*m_pWorkJob;


/* 로컬 함수 */
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


/* 공용 함수 */
public:

	BOOL				RunWorkJob(ENG_BWOK job_id, PUINT64 data=NULL);
	BOOL				IsBusyWorkJob();

	ENG_BWOK			GetWorkJobID();

	VOID				SetExitApp(UINT64 delay);
};
