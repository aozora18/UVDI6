
#pragma once

#include "DlgMenu.h"

class CWork;
class CWorkArea;
class CGridCtrl;

class CDlgArea : public CDlgMenu
{
public:

	CDlgArea(UINT32 id, LPG_MICL meas, UINT8 menu_id, CWnd* parent = NULL);
	virtual ~CDlgArea();

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

	virtual UINT8		GetPhNo();
	virtual PTCHAR		GetStepName();


/* Local Parameter */
protected:

	UINT32				m_u32Width[3];

	STM_GMMV			m_stMaxPwr[2];		/* Maxium power information (0x00: Min/Max Power, 0x01:Average) */

	CMyGrpBox			m_grp_ctl[10];
	CMyStatic			m_txt_ctl[22];
	CMacButton			m_btn_ctl[1];
	CMacCheckBox		m_chk_ctl[4];
	CEditNum			m_edt_int[7];
	CEditNum			m_edt_flt[13];
	CMyCombo			m_cmb_ctl[2];

	CWorkArea			*m_pWorkArea;
	CGridCtrl			*m_pGridArea[4];


/* Local Function */
protected:

	VOID				InitCtrl();
	VOID				InitData();
	VOID				InitGrid();

	VOID				EnableCtrl();
	VOID				UpdateRealTimeIllum();

	VOID				UpdateFilterRate();


/* Public Function */
public:

	BOOL				IsActiveMode();


/* Message Map */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnChkClicked(UINT32 id);
	afx_msg VOID		OnGridColumnClicked(NMHDR *nmhdr, LRESULT *result);
};
