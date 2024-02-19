
#pragma once

#include "../../../inc/comn/MyDialog.h"
#include "DlgMenu.h"

// by sys&j : �������� ���� enum �߰�
enum EnMmpmGrp
{
	eMMPM_GRP_STEP	,
	eMMPM_GRP_SEARCH,
	eMMPM_GRP_MAX
};

enum EnMmpmPic
{
	eMMPM_PIC_VIEW,
	eMMPM_PIC_MAX
};

enum EnMmpmChk
{
	eMMPM_CHK_STEP_01 ,
	eMMPM_CHK_STEP_02 ,
	eMMPM_CHK_STEP_05 ,
	eMMPM_CHK_STEP_10 ,
	eMMPM_CHK_MAX
};

enum EnMmpmBtn
{
	eMMPM_BTN_HORZ_LEFT		,
	eMMPM_BTN_HORZ_RIGHT		,
	eMMPM_BTN_VERT_UP		,
	eMMPM_BTN_VERT_DOWN		,
	eMMPM_BTN_UNDO			,
	eMMPM_BTN_PREV			,
	eMMPM_BTN_NEXT			,
	eMMPM_BTN_CHECK			,
	eMMPM_BTN_APPLY			,
	eMMPM_BTN_CANCEL			,
	eMMPM_BTN_MAX
};

//class CDPoint
//{
//public:
//	double x, y;
//	CDPoint()
//	{
//		x = y = 0.0;
//	}
//};


class CDlgMmpm : public CMyDialog
{
public:

	CDlgMmpm(CWnd* parent = NULL);
	enum { IDD = IDD_MMPM };

/* ���� �Լ� */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel() {}

public:

	virtual VOID		UpdatePeriod()	{};

/* ���� ���� */
protected:

	UINT8				m_u8Index;			/* ���� Grab.Failed ��ġ (Zero based) */
	UINT8				m_u8LeftClick;		/* ���콺 ���� ��ư Ŭ�� Down �ߴ��� ���� */

	HCURSOR				m_hPrvCursor, m_hNowCursor;	/* ���콺 Ŀ�� �ڵ� */

	DOUBLE				m_dbStep;			/* Ű���峪 ��ư�� �̵��� ��, �̵� ���� (Pixel) */
	HDC					m_hDCDraw;			/* ���� Align Mark ����� ���� ���� DC �ڵ� */
	RECT				m_rAreaDraw;		/* ���� Align Mark ����� ���� ���� ���� */

	CPoint				m_ptClickDown;		/* ���콺 ���� ��ư Ŭ���� ��ġ ���� */
	CRect				um_rectArea;

	CMyGrpBox			m_grp_ctl[eMMPM_GRP_MAX];
	CMacButton			m_btn_ctl[eMMPM_BTN_MAX];		/* Normal */
	CMacCheckBox		m_chk_ctl[eMMPM_CHK_MAX];
	CMyStatic			m_pic_ctl[eMMPM_PIC_MAX];		/* Mark Drawing Area */

	/* ���� ���õ� ���� ���� */
	LPG_ACGR			m_pstGrab;

	CAtlList <LPG_ACGR>	m_lstFind;			/* Find.Failed�� ������ ���� */
	CAtlList <LPG_ACGR>	m_lstGrab;			/* Grab.Failed�� ������ ���� */

	CDPoint				m_pt3Point_Img[3];		/* 3�� ���� ����, �̹��� ��ǥ */
	CDPoint				m_ptCenter; // �»��(0,0)�������� ��ǥ ����
/* ���� �Լ� */
protected:

	BOOL				InitCtrl();
	BOOL				GetMarkData(UINT8 type);

	VOID				FindData(UINT8 direct);

	VOID				UndoCenter();
	VOID				MoveCenter(UINT8 type);

	VOID				WorkApply();
	VOID				WorkCancel();
	VOID				KeyUpDown(UINT16 key);

	VOID				Restore();

	VOID				InitDispMark();
	VOID				DispResize(CWnd* pWnd);
	BOOL				ZoomFlag;
	BOOL				OldZoomFlag;
	BOOL				um_bMoveFlag;
	void				MenuZoomIn();
	void				MenuZoomOut();
	void				MenuZoomFit();
	int					menuPart; // menu ����, 1:zoom, 2,3,4:3point set, 5:measure, 99 : �ƹ��͵� ���þȵ� ����
	double				tgt_rate; // ����

/* ���� �Լ� */
public:

	INT_PTR				MyDoModal();

/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnChkClicked(UINT32 id);
	afx_msg VOID		OnLButtonDown(UINT flags, CPoint point);
	afx_msg VOID		OnLButtonUp(UINT flags, CPoint point);
	afx_msg VOID		OnMouseMove(UINT flags, CPoint point);
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};
