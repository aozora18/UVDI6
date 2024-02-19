
#pragma once

#include "DlgMenu.h"

class CWorkFocus;
class CGridCtrl;

class CDlgFocu : public CDlgMenu
{
public:

	CDlgFocu(UINT32 id, LPG_MICL recipe, UINT8 menu_id, CWnd* parent = NULL);
	virtual ~CDlgFocu();

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

	UINT64				m_u64CorrectCnt;

	DOUBLE				m_dbCorrectTotal;	/* (Unit : mW) */

	STM_GMMV			m_stMaxPwr[3];		/* Maxium power information (0x00: Min/Max Power, 0x01:Average, 0x02 : Standard Deviation) */

	CMyGrpBox			m_grp_ctl[6];
	CMyStatic			m_txt_ctl[10];
	CMyCombo			m_cmb_ctl[2];
	CEditNum			m_edt_int[6];
	CEditNum			m_edt_flt[2];

	CWorkFocus			*m_pWorkFocus;
	CGridCtrl			*m_pGridFocus;


/* Local Function */
protected:

	BOOL				IsValidFocusPH();

	VOID				InitCtrl();
	VOID				InitData();
	VOID				InitGrid();

	VOID				EnableCtrl();


/* Public Function */
public:


/* Message Map */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnGridSelChanged(NMHDR *nmhdr, LRESULT *result);
};
