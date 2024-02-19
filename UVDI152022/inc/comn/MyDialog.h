
#pragma once

#include "Picture.h"
#include <afxdialogex.h>

#include "MacButton.h"
#include "MacProgress.h"
#include "MyGrpBox.h"
#include "MyStatic.h"
#include "MyEdit.h"
#include "MacProgress.h"
#include "MyCombo.h"
#include "MyListBox.h"

/* ------------------------------------------------- */
/* MFC Dialog ���� (ȣ��)�� ��, ����Ǵ� �޽��� ���� */
/* ------------------------------------------------- */
/* 01.DoModal() */
/* 02.PreSubclassWindow() */
/* 03.OnNcCreate() */
/* 04.OnNcCalcSize() */
/* 05.OnCreate() */
/* 06.OnSize() */
/* 07.OnMove() */
/* 08.OnSetFont() */
/* 09.OnInitDialog() */
/* 10.OnWindowPosChanging() */
/* 11.OnMove() */
/* 12.OnWindowPosChanged() */
/* 13.OnWindowPosChanging() */
/* 14.OnNcActivate() */
/* 15.OnActivate() */
/* 16.OnShowWindow() */
/* 17.OnWindowPosChanging() */
/* 18.OnNcPaint() */
/* 19.OnEraseBkgnd() */
/* 20.OnChildNotify() */
/* 21.OnCtlColor() */
/* 22.OnWindowPosChanged() */
/* 23.OnPaint() */
/* 24.OnCtlColor() */
/* 25.OnCtlColor() */
/* 26.OnNcHitTest() */
/* 27.OnSetCursor() */
/* 28.PreTranslateMessage() */
/* 29.OnMouseMove() */
/* 30.OnNcHitTest() */
/* 31.OnSetCursor() */
/* 32.PreTranslateMessage() */

class CMyDialog : public CDialogEx
{

public:
	CMyDialog(UINT32 id, CWnd* parent=NULL);
	virtual ~CMyDialog();
	BOOL				Create();

/* �����Լ� ���� */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL		OnInitDialog();
	virtual VOID		DoDataExchange(CDataExchange* dx);

public:

	virtual BOOL		OnInitDlg()			= 0;
	virtual VOID		OnExitDlg()			= 0;
	virtual VOID		OnPaintDlg(CDC *dc)	= 0;
	virtual VOID		OnResizeDlg()		= 0;
	virtual void		RegisterUILevel()	= 0;
	virtual void		UpdateUILevel();

/* ���� ���� */
protected:

	/* ��� �̹��� ���� ��ġ */
	typedef enum class __en_bg_image_align__ : UINT8
	{
		en_none			= 0x00,		/* �ƹ��ǹ� ����!!!(�� �ڵ���� ������) */
		en_lefttop		= 0x01,		/* ���� ��� */
		en_righttop		= 0x02,		/* ���� ��� */
		en_topcent		= 0x03,		/* �߾� ��� */
		en_btmcent		= 0x04,		/* �߾� �ϴ� */
		en_centcent		= 0x05,		/* �߾� �߾� */
		en_centexcept	= 0x06,		/* �߾��ε� Ư�� �߾� */

	}	EN_BIA;

	TCHAR				m_tzBgFile[_MAX_PATH];	// ��� �̹��� ���� (��� ����)
	TCHAR				m_tzErrMsg[1024];
	TCHAR				m_tzTitle[1024];		// ���̾�α� ���� ����

	INT32				m_i32FixedWidth;		// ���� ���� ũ�� ��. ����
	INT32				m_i32FixedHeight;		// ���� ���� ũ�� ��. ����
	INT32				m_i32SizeMinWidth;		// ���� �ּ� ũ�� ��. ����
	INT32				m_i32SizeMinHeight;		// ���� �ּ� ũ�� ��. ����

	BOOL				m_bTooltip;				// ���� ��ȭ ���� ��� ����
	BOOL				m_bKeyPushEnter;
	BOOL				m_bKeyEnterApply;		// Enter Key ������ ��, Apply �Լ� �����ϵ��� ...
	BOOL				m_bKeyESCCancel;		// Enter Key ������ ��, Cancel �Լ� �����ϵ���
	BOOL				m_bKeyPushESC;
	BOOL				m_bDragMoveWnd;			// ��濡�ٰ� ���콺 ���� ��ư�� Ŭ���� �Ŀ� �̵��� ��� ���� �̵� ����
	BOOL				m_bExitProc;
	BOOL				m_bDrawBG;				// ���̾�α� ���� ���� ����
	BOOL				m_bMoveWnd;				// ������ �̵��� ������
	BOOL				m_bMinSizeWidth;		// ���� ũ�⸦ �ּ� �� ���Ϸ� ������ ���ϵ��� ó��
	BOOL				m_bMinSizeHeight;		// ���� ũ�⸦ �ּ� �� ���Ϸ� ������ ���ϵ��� ó��
	BOOL				m_bSizeLeftRight;		// ���� ũ�⸦ ���� �����ϵ��� �� ������..
	BOOL				m_bSizeTopBottom;		// ���� ũ�⸦ ���� �����ϵ��� �� ������..
	BOOL				m_bMagnetic;			/* ���� �����ڸ� ��ó�� ���� �ڵ����� �ٴ� ��� */
	BOOL				m_bEnableMinimizeBox;	/* ���� �ּ�ȭ �ý��� ��ư Enable(TRUE) / Disable(FALSE) ���� */
	BOOL				m_bEnableMaximizeBox;	/* ���� �ִ�ȭ �ý��� ��ư Enable(TRUE) / Disable(FALSE) ���� */

	UINT32				m_u32DlgID;				/* Window (Dialog) Resource ID */

	/* ���� m_bMagnetic�� Enable�� ��, ���� �ڼ� ����� ���۵� �� �ִ� X/Y ���� �ڸ� ���� �� */
	UINT32				m_u32MagneticX;
	UINT32				m_u32MagneticY;

	HBRUSH				m_brushBg;
	HICON				m_hIcon;

	EN_BIA				m_enBIA;				// �̹��� ��� ��ġ

	LOGFONT				m_lf;					// ���̾�α� �⺻ ��Ʈ ����

	COLORREF			m_clrBg;				// ����

	CFont				m_ftToolTip;
	CPoint				m_csPointMove;
	CRect				m_rPicBg;

	CPicture			m_csPicBg;

	CToolTipCtrl		m_csToolTip;
	CWnd				*m_pChildWnd;			// ������ ��µǴ� ��ü ������
	CWnd				*m_pParentWnd;			// �ڽ� ������ ȣ��(����)�� �θ� ��ü ������

	vST_UI_LEVEL		m_vUILevel;

	BOOL				LoadImages();

/* ���� �Լ� */
protected:
	
	VOID				ExitProc();
	VOID				SetErrMsg(TCHAR *msg, UINT8 flag=0x00);
	VOID				AddToolTip(UINT32 ctrl_id, TCHAR *tooltip);
	VOID				CenterParentTopMost(HWND parent=NULL);

/* ���� �Լ� */
public:

	TCHAR				*GetErrMsg()	{	return m_tzErrMsg;	}
	UINT32				GetWndID()		{	return m_u32DlgID;	}
	LPLOGFONT			GetLogFont()	{	return &m_lf;		}

	VOID				CenterDialog(CWnd *parent);
	VOID				ResizeDialog(CRect rect);
	VOID				SetColorBG(COLORREF color)	{	m_clrBg = color;		}

	VOID				SetParentWnd(CWnd *parent)	{	m_pParentWnd = parent;	}

/* �޽��� �� */
protected:

	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH		OnCtlColor(CDC* dc, CWnd* wnd, UINT32 ctl_color);
	afx_msg HCURSOR		OnQueryDragIcon();
	afx_msg VOID		OnPaint();
	afx_msg VOID		OnLButtonDown(UINT32 flags, CPoint point);
	afx_msg VOID		OnSize(UINT32 nType, int cx, int cy);
	afx_msg VOID		OnClose();
	afx_msg VOID		OnDestroy();
	afx_msg VOID		OnGetMinMaxInfo(MINMAXINFO* mm);
	afx_msg VOID		OnWindowPosChanging(WINDOWPOS *lpwndpos);
};
