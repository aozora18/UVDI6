
#pragma once

#include "DlgMenu.h"

class CWork;
class CWorkMeas;
class CGridCtrl;

class CDlgMeas : public CDlgMenu
{
public:

	CDlgMeas(UINT32 id, LPG_MICL recipe, UINT8 menu_id, CWnd* parent = NULL);
	virtual ~CDlgMeas();

/* Enum */
protected:


/* Structure */
protected:


/* Virutal Function */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg()	{};
	virtual VOID		ShowHide(BOOL show)	{};
	virtual VOID		CreateWork();
	virtual VOID		ReleaseWork();
	virtual VOID		RebuildGrid();
	virtual VOID		UpdateWork(UINT8 mode);

public:

	virtual VOID		UpdateCtrl();
	virtual VOID		DoWork();

	virtual BOOL		GetData();
	virtual VOID		SetData();
	virtual VOID		SaveGrid();

	virtual BOOL		IsWorkBusy();
	virtual BOOL		StartWork(UINT8 run_mode, DOUBLE max_min);
	virtual VOID		StopWork();

	virtual UINT8		GetPhNo() {	return 0x00; };
	virtual PTCHAR		GetStepName();


/* Local Parameter */
protected:

	STM_GMMV			m_stMinPwr[3];

	CMyGrpBox			m_grp_ctl[5];
	CMyStatic			m_txt_ctl[8];
	CMacCheckBox		m_chk_ctl[4];
	CEditNum			m_edt_int[7];
	CMyCombo			m_cmb_ctl[1];

	CWork				*m_pWork;
	CWorkMeas			*m_pWorkMeas;
	CGridCtrl			*m_pGridMeas;


/* Local Function */
protected:

	VOID				InitCtrl();
	VOID				InitData();
	VOID				InitGrid();

	VOID				EnableCtrl();

	BOOL				IsValidParam();

	UINT8				GetLedNo();


/* Public Function */
public:


/* Message Map */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnChkClicked(UINT32 id);
};
