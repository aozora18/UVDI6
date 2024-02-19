
#pragma once

#include "DlgMenu.h"

class CDlgExpo : public CDlgMenu
{
public:

	CDlgExpo(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgExpo();

/* 구조체 / 공용체 */
protected:


/* 가상 함수 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();

public:

	virtual VOID		UpdateCtrl();
	virtual VOID		ResetData()	{};

/* 로컬 변수 */
protected:

	BOOL				m_bIsExpoUpdate;	/* Exposure 상태를 갱신할 필요가 있는지 여부 */

	UINT16				**m_pLedAmplitue;
	UINT32				m_u32ExpoSpeed;		/* 가장 최근의 Exposure Speed (um/sec) 값 임시 저장 */
	UINT64				m_u64TickLoadState;	/* Job Load State 요청 시간 값 임시 저장 */
	UINT64				m_u64TickExpoState;	/* Exposure State 요청 시간 값 임시 저장 */

	ENG_LPES			m_enExpoStatus;		/* 가장 최근의 Exposure Status 값 임시 저장 */
	ENG_LSLS			m_enLastJobLoadStatus;	/* 가장 최근의 Job Load 상태 값 임시 저장 */

	CMyGrpBox			m_grp_ctl[4];
	CMacProgress		m_pgs_ctl[1];
	CMyListBox			m_box_ctl[1];
	CMacButton			m_btn_ctl[10];
	CMyStatic			m_txt_ctl[11];
	CMyEdit				m_edt_ctl[6];

	CGridCtrl			*m_pGridLed;


/* 로컬 함수 */
protected:

	VOID				InitMem();
	VOID				InitCtrl();
	VOID				InitGrid();

	VOID				UpdateJobList();
	VOID				UpdateJobLoad();
	VOID				UpdateExposure();
	VOID				UpdateLedAmpl();

	VOID				JobMgtAdd();
	VOID				JobMgtDel();
	VOID				JobMgtLoad();
	VOID				ExpoParamSet();
	VOID				ExpoStart(UINT8 mode);
	VOID				GetLedLightLvl();
	VOID				SetLedLightLvl();


/* 공용 함수 */
public:



/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg VOID		OnBnClicked(UINT32 id);
	afx_msg VOID		OnBoxSelchangeJobList();
};
