
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
/* MFC Dialog 생성 (호출)될 때, 실행되는 메시지 순서 */
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

/* 가상함수 선언 */
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

/* 로컬 변수 */
protected:

	/* 배경 이미지 저장 위치 */
	typedef enum class __en_bg_image_align__ : UINT8
	{
		en_none			= 0x00,		/* 아무의미 없어!!!(이 코드계의 쑤레기) */
		en_lefttop		= 0x01,		/* 좌측 상단 */
		en_righttop		= 0x02,		/* 우측 상단 */
		en_topcent		= 0x03,		/* 중앙 상단 */
		en_btmcent		= 0x04,		/* 중앙 하단 */
		en_centcent		= 0x05,		/* 중앙 중앙 */
		en_centexcept	= 0x06,		/* 중앙인데 특정 중앙 */

	}	EN_BIA;

	TCHAR				m_tzBgFile[_MAX_PATH];	// 배경 이미지 파일 (경로 포함)
	TCHAR				m_tzErrMsg[1024];
	TCHAR				m_tzTitle[1024];		// 다이얼로그 윈도 제목

	INT32				m_i32FixedWidth;		// 윈도 고정 크기 값. 넓이
	INT32				m_i32FixedHeight;		// 윈도 고정 크기 값. 높이
	INT32				m_i32SizeMinWidth;		// 윈도 최소 크기 값. 넓이
	INT32				m_i32SizeMinHeight;		// 윈도 최소 크기 값. 높이

	BOOL				m_bTooltip;				// 툴팁 대화 상자 사용 여부
	BOOL				m_bKeyPushEnter;
	BOOL				m_bKeyEnterApply;		// Enter Key 눌렀을 때, Apply 함수 수행하도록 ...
	BOOL				m_bKeyESCCancel;		// Enter Key 눌렀을 때, Cancel 함수 수행하도록
	BOOL				m_bKeyPushESC;
	BOOL				m_bDragMoveWnd;			// 배경에다가 마우스 왼쪽 버튼을 클릭한 후에 이동할 경우 윈도 이동 여부
	BOOL				m_bExitProc;
	BOOL				m_bDrawBG;				// 다이얼로그 배경색 설정 여부
	BOOL				m_bMoveWnd;				// 윈도를 이동할 것인지
	BOOL				m_bMinSizeWidth;		// 윈도 크기를 최소 얼마 이하로 줄이지 못하도록 처리
	BOOL				m_bMinSizeHeight;		// 윈도 크기를 최소 얼마 이하로 줄이지 못하도록 처리
	BOOL				m_bSizeLeftRight;		// 윈도 크기를 조절 가능하도록 할 것인지..
	BOOL				m_bSizeTopBottom;		// 윈도 크기를 조절 가능하도록 할 것인지..
	BOOL				m_bMagnetic;			/* 윈도 가장자리 근처에 가면 자동으로 붙는 기능 */
	BOOL				m_bEnableMinimizeBox;	/* 윈도 최소화 시스템 버튼 Enable(TRUE) / Disable(FALSE) 여부 */
	BOOL				m_bEnableMaximizeBox;	/* 윈도 최대화 시스템 버튼 Enable(TRUE) / Disable(FALSE) 여부 */

	UINT32				m_u32DlgID;				/* Window (Dialog) Resource ID */

	/* 윈도 m_bMagnetic이 Enable일 때, 윈도 자석 기능이 동작될 수 있는 X/Y 가장 자리 여백 값 */
	UINT32				m_u32MagneticX;
	UINT32				m_u32MagneticY;

	HBRUSH				m_brushBg;
	HICON				m_hIcon;

	EN_BIA				m_enBIA;				// 이미지 출력 위치

	LOGFONT				m_lf;					// 다이얼로그 기본 폰트 정보

	COLORREF			m_clrBg;				// 배경색

	CFont				m_ftToolTip;
	CPoint				m_csPointMove;
	CRect				m_rPicBg;

	CPicture			m_csPicBg;

	CToolTipCtrl		m_csToolTip;
	CWnd				*m_pChildWnd;			// 윈도가 출력되는 객체 포인터
	CWnd				*m_pParentWnd;			// 자식 윈도를 호출(실행)한 부모 객체 포인터

	vST_UI_LEVEL		m_vUILevel;

	BOOL				LoadImages();

/* 로컬 함수 */
protected:
	
	VOID				ExitProc();
	VOID				SetErrMsg(TCHAR *msg, UINT8 flag=0x00);
	VOID				AddToolTip(UINT32 ctrl_id, TCHAR *tooltip);
	VOID				CenterParentTopMost(HWND parent=NULL);

/* 공용 함수 */
public:

	TCHAR				*GetErrMsg()	{	return m_tzErrMsg;	}
	UINT32				GetWndID()		{	return m_u32DlgID;	}
	LPLOGFONT			GetLogFont()	{	return &m_lf;		}

	VOID				CenterDialog(CWnd *parent);
	VOID				ResizeDialog(CRect rect);
	VOID				SetColorBG(COLORREF color)	{	m_clrBg = color;		}

	VOID				SetParentWnd(CWnd *parent)	{	m_pParentWnd = parent;	}

/* 메시지 맵 */
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
