
#pragma once

#include "../../inc/comn/MyDialog.h"
#include "../../inc/comn/MacButton.h"
#include "../../inc/comn/MyGrpBox.h"
#include "../../inc/comn/MyStatic.h"
#include "../../inc/comn/MyEdit.h"
#include "../../inc/grid/GridCtrl.h"

class CDlgMain : public CMyDialog
{
public:
	CDlgMain(CWnd* parent = NULL);
	enum { IDD = IDD_MAIN };

/* ���� �Լ� */
protected:

	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual VOID		UpdatePeriod()	{};

/* ����ü ���� */
protected:

#pragma pack (push, 8)
	typedef struct __st_align_camera_calibration_information__
	{
		INT32			acam_x;			/* Align Camera X : Motion Position (����: 0.1um or 100nm) */
		INT32			stage_x;		/* Stage X : Motion Position (����: 0.1um or 100nm) */
		INT32			stage_y;		/* Stage Y : Motion Position (����: 0.1um or 100nm) */
		UINT32			row_cnt;		/* �� ������ ���� ���� */
		UINT32			col_cnt;		/* �� ������ ���� ���� */
		UINT32			row_gap;		/* ��� �� ������ ���� ���� (����: 0.1um or 100nm) */
		UINT32			col_gap;		/* ���� �� ������ ���� ���� (����: 0.1um or 100nm) */

		LPG_I32XY		pos_xy;			/* ����: 100 nm or 0.1 um */

	}	STM_ACCI,	*LPM_ACCI;
#pragma pack (pop)	/* 8 bytes order */


/* ���� ���� */
protected:

	CMyGrpBox			m_grp_ctl[6];
	CMyStatic			m_txt_ctl[7];
	CMacButton			m_btn_ctl[4];
	CMacCheckBox		m_chk_ctl[4];
	CMyEdit				m_edt_int[3];
	CMyEdit				m_edt_flt[5];

	STM_ACCI			m_stCali;

	CGridCtrl			*m_pGrid;


/* ���� �Լ� */
protected:

	VOID				InitCtrl();
	VOID				InitGrid();
	VOID				CloseGrid();

	VOID				OpenFile();
	VOID				SaveFile();

	BOOL				LoadData(PTCHAR file);
	VOID				DrawGrid();
	VOID				DrawData();


/* ���� �Լ� */
public:



/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnSysCommand(UINT32 id, LPARAM lparam);
	afx_msg VOID		OnBtnClick(UINT32 id);
	afx_msg VOID		OnChkClick(UINT32 id);
};
