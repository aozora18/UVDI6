
#pragma once

#include "DlgMenu.h"

class CDlgExpo : public CDlgMenu
{
public:

	CDlgExpo(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgExpo();

/* ����ü / ����ü */
protected:


/* ���� �Լ� */
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

/* ���� ���� */
protected:

	BOOL				m_bIsExpoUpdate;	/* Exposure ���¸� ������ �ʿ䰡 �ִ��� ���� */

	UINT16				**m_pLedAmplitue;
	UINT32				m_u32ExpoSpeed;		/* ���� �ֱ��� Exposure Speed (um/sec) �� �ӽ� ���� */
	UINT64				m_u64TickLoadState;	/* Job Load State ��û �ð� �� �ӽ� ���� */
	UINT64				m_u64TickExpoState;	/* Exposure State ��û �ð� �� �ӽ� ���� */

	ENG_LPES			m_enExpoStatus;		/* ���� �ֱ��� Exposure Status �� �ӽ� ���� */
	ENG_LSLS			m_enLastJobLoadStatus;	/* ���� �ֱ��� Job Load ���� �� �ӽ� ���� */

	CMyGrpBox			m_grp_ctl[4];
	CMacProgress		m_pgs_ctl[1];
	CMyListBox			m_box_ctl[1];
	CMacButton			m_btn_ctl[10];
	CMyStatic			m_txt_ctl[11];
	CMyEdit				m_edt_ctl[6];

	CGridCtrl			*m_pGridLed;


/* ���� �Լ� */
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


/* ���� �Լ� */
public:



/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg VOID		OnBnClicked(UINT32 id);
	afx_msg VOID		OnBoxSelchangeJobList();
};
