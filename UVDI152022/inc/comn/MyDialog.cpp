
#include "pch.h"
#include "MyDialog.h"
#include "afxdialogex.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


CMyDialog::CMyDialog(UINT32 id,CWnd* parent)
 : CDialogEx(id, parent)
{
	m_hIcon					= NULL;
	m_pParentWnd			= parent;	// �ڽ��� ȣ���� �θ� ��ü ������
	m_bKeyPushEnter			= 0;
	m_bKeyPushESC			= 0;
	m_bDragMoveWnd			= 0;
	m_bTooltip				= 0;
	m_bExitProc				= 0;
	m_bDrawBG				= 0;
	m_bMoveWnd				= 0;
	m_bSizeLeftRight		= 1;
	m_bSizeTopBottom		= 1;
	m_bMinSizeWidth			= 0;
	m_bMinSizeHeight		= 0;
	m_bMagnetic				= 1;
	m_u32MagneticX			= 30;
	m_u32MagneticY			= 30;
	m_enBIA					= EN_BIA::en_lefttop;
	m_csPointMove.x			= 0;
	m_csPointMove.y			= 0;
	m_i32FixedWidth			= 0;
	m_i32FixedHeight		= 0;
	m_i32SizeMinWidth		= 0;
	m_i32SizeMinHeight		= 0;
	m_u32DlgID				= id;
	m_clrBg					= RGB(255, 255, 255);
	m_bEnableMinimizeBox	= TRUE;
	m_bEnableMaximizeBox	= TRUE;

	ZeroMemory(m_tzBgFile, _MAX_PATH);
	ZeroMemory(m_tzErrMsg, 1024);
	ZeroMemory(m_tzTitle, 1024);
}

CMyDialog::~CMyDialog()
{
	if (m_bDrawBG)	::DeleteObject(m_brushBg);
}

VOID CMyDialog::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);

}

void CMyDialog::UpdateUILevel()
{
	for (auto& ui : m_vUILevel)
	{
		if ((int)ui.nLevel <= (int)g_loginLevel)
		{
			ui.pwndControl->EnableWindow(TRUE);
		}
		else
		{
			ui.pwndControl->EnableWindow(FALSE);
		}
	}
}

BEGIN_MESSAGE_MAP(CMyDialog, CDialogEx)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_GETMINMAXINFO()
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CMyDialog::PreTranslateMessage(MSG* msg)
{
	switch (msg->message)
	{
	case  WM_KEYDOWN	:
		if (msg->wParam==VK_RETURN)
		{
			if (!m_bKeyPushEnter)	return TRUE;
		}
		if (msg->wParam==VK_ESCAPE)
		{
			if (!m_bKeyPushESC)	return TRUE;
		}
		break;
		// ���� ����� ���ؼ�
	case WM_LBUTTONDOWN	:
	case WM_LBUTTONUP	:
	case WM_MOUSEMOVE	:
		if (m_bTooltip && m_csToolTip.GetSafeHwnd())
		{
			m_csToolTip.Activate(TRUE);
			m_csToolTip.RelayEvent(msg);
		}
		break;
	}

	return CDialogEx::PreTranslateMessage(msg);
}

/*
 desc : PreCreateWindow �����Լ� ������
 parm : cs	- [in]  CREATESTRUCT ����ü
 retn : TRUE or FALSE
*/
BOOL CMyDialog::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CDialogEx::PreCreateWindow(cs))	return FALSE;

	/* ���� �ý��� ������ (�ּ�ȭ / �ִ�ȭ) ��ư ���̱�/����� ���� */
	if (!m_bEnableMinimizeBox)	cs.style &= ~ (LONG)WS_MINIMIZEBOX;	/* �ּ�ȭ ��ư Disable */
	if (!m_bEnableMaximizeBox)	cs.style &= ~ (LONG)WS_MAXIMIZEBOX;	/* �ּ�ȭ ��ư Disable */

	return TRUE;
}

/*
 desc : ��޸��� ���̾�α� ������ ����
 parm : None
 retn : 1-��޸��� ���� ���� / 0-����
*/
BOOL CMyDialog::Create()
{
	return CDialogEx::Create(m_u32DlgID, m_pParentWnd);
}

/*
 desc : ���̾�α� ������ �� ó�� ����Ǵ� �κ�
 parm : None
 retn : 1-����, 0-����
*/
BOOL CMyDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_bExitProc		= 0x00;	/* !!! �ݵ�� �ʱ�ȭ !!! */
	CFont *pFont	= NULL;

	if (m_hIcon)
	{
		SetIcon(m_hIcon, 1);	// ū �������� �����մϴ�.
		SetIcon(m_hIcon, 0);	// ���� �������� �����մϴ�.
	}

	// ���� ���� ����
	if (m_bDrawBG)
	{
		m_brushBg = ::CreateSolidBrush(m_clrBg);
	}

	// �̹��� ����
	LoadImages();

	// ���� ����
	if (m_bTooltip && !m_csToolTip.GetSafeHwnd())
	{
		if (!m_csToolTip.Create(this))
		{
#ifdef	_UNICODE
			AfxMessageBox(_T("Fail to create tooltip windows"), MB_ICONSTOP|MB_TOPMOST);
#else
			AfxMessageBox("Fail to create tooltip windows", MB_ICONSTOP|MB_TOPMOST);
#endif
			PostMessage(WM_CLOSE, 0, 0L);
			return FALSE;
		}
		else
		{
			// ���� �ʱ�ȭ
			m_csToolTip.SetMaxTipWidth(1000);	// ���� �޽��� ���� ���� (�ȼ�)
			m_csToolTip.SetDelayTime(500);		// ���� ��� ���� �ð�(�и�����Ʈ. 0.5�� �Ŀ� ��µ�)
		}

		// ���̾�α� �⺻ ��Ʈ ����
		pFont = GetFont();
		if (pFont)
		{
			pFont->GetLogFont(&m_lf);
			if (!m_ftToolTip.GetSafeHandle() && m_ftToolTip.CreateFontIndirect(&m_lf))
			{
				m_csToolTip.SetFont(&m_ftToolTip);
			}
		}
	}
	// ���̾�α� ���� ���
	GetWindowText(m_tzTitle, 1024);
	// ���̾�α� �ʱ�ȭ�� ���� �Լ� ȣ��
	if (!OnInitDlg())
	{
#ifdef	_UNICODE
		AfxMessageBox(_T("Failed to initialize dialog window"), MB_ICONSTOP|MB_TOPMOST);
#else
		AfxMessageBox("Failed to initialize dialog window", MB_ICONSTOP|MB_TOPMOST);
#endif
		PostMessage(WM_CLOSE, 0, 0L);
		return FALSE;
	}

	if (m_bMoveWnd)
	{
		CRect rClient;
		// ���� �ش� ���콺 ������ ��ġ�� ��ġ �ϱ�
		GetClientRect(rClient);
		MoveWindow(m_csPointMove.x, m_csPointMove.y, rClient.Width(), rClient.Height());
	}

	// ���̾�α� ũ�� ������ ȣ��(?)
	OnResizeDlg();
	RegisterUILevel();
	UpdateUILevel();
	return TRUE;  // return 1 unless you set the focus to a control
}

VOID CMyDialog::OnPaint()
{
	CPaintDC dc(this);

	CRect rClient, rPicBg;
	GetClientRect(rClient);

	if (IsIconic())
	{
		CRect rect;
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// ���� ��� ���� ���
		if (m_enBIA == EN_BIA::en_lefttop)
		{
			if(m_csPicBg.IsDrawImage())	m_csPicBg.Show(&dc, m_rPicBg);
		}
		// ���߾ӿ� ��ġ
		else if(m_enBIA == EN_BIA::en_centcent)
		{
			if(m_csPicBg.IsDrawImage())
			{
				rPicBg.left		= rClient.Width() / 2 - m_rPicBg.Width() / 2;
				rPicBg.top		= rClient.Height() / 2 - m_rPicBg.Height() / 2;
				rPicBg.right	= rPicBg.left + m_rPicBg.Width();
				rPicBg.bottom	= rPicBg.top + m_rPicBg.Height();
				m_csPicBg.Show(&dc, rPicBg);
			}
		}
		// Ư�� �߾�
		else if(m_enBIA == EN_BIA::en_centexcept)
		{
			if(m_csPicBg.IsDrawImage())
			{
				rPicBg.left		= rClient.Width() / 2 - m_rPicBg.Width() / 2 + 110;
				rPicBg.top		= rClient.Height() / 2 - m_rPicBg.Height() / 2;
				rPicBg.right	= rPicBg.left + m_rPicBg.Width();
				rPicBg.bottom	= rPicBg.top + m_rPicBg.Height();
				m_csPicBg.Show(&dc, rPicBg);
			}
		}
		// ��Ÿ
		else
		{
		}

		// ������ ���ŵ� ������ ȣ���. ȭ�� ������ �ٽ� �׸��� ���ؼ�
		OnPaintDlg(&dc);

		CDialogEx::OnPaint();
	}

	UpdateUILevel();
}

/*
 desc : �ּ�ȭ�� CWnd�� ����ڿ� ���ؼ� �巡�׵� �� ȣ��
 parm : None
 retn : Ŀ�� �ڵ� ��ȯ
*/
HCURSOR CMyDialog::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HBRUSH CMyDialog::OnCtlColor(CDC* dc, CWnd* wnd, UINT32 ctl_color)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(dc, wnd, ctl_color);
	switch (ctl_color)
	{
//	case CTLCOLOR_MSGBOX	:
//	case CTLCOLOR_EDIT		:
//	case CTLCOLOR_LISTBOX	:
//	case CTLCOLOR_BTN		:
	case CTLCOLOR_DLG		:
//	case CTLCOLOR_SCROLLBAR	:
	case CTLCOLOR_STATIC	:
//	case CTLCOLOR_MAX		:
		dc->SetBkMode(TRANSPARENT);
		if (m_bDrawBG)	hbr = m_brushBg;  // ���⼭ �����ϴ� ���� �ٲٰ� ���� ��
		break;
	}

	return hbr;
}

/*
 desc : ������ �̹��� ������ ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMyDialog::LoadImages()
{
	//	��� �̹��� �ø���
	if (_tcslen(m_tzBgFile) < 8)
	{
		SetErrMsg(_T("������ �̹��� ���� ����. ���� �� �� Ȯ��"));
		return 0;
	}
	// �̹��� ���� ����
	if (!m_csPicBg.Load(m_tzBgFile))
	{
		SetErrMsg(_T("�̹��� ���� ���� ����. ���� �� �� ��θ� Ȯ��"));
		return 0;
	}

	if (!m_csPicBg.GetImgHandle())
	{
		SetErrMsg(_T("�̹��� �ڵ� ���� ��� ����. �˼� ���� ����"));
		return 0;
	}

	RECT rImgSize;
	m_csPicBg.GetPicRect(rImgSize);
	m_rPicBg = CRect(rImgSize);

	return 1;
}

/*
 desc : ���콺 ���� ��ư�� Ŭ���� ������ ���
 parm : - flags	: ???
		- point	: ???
 retn : None
*/
VOID CMyDialog::OnLButtonDown(UINT32 flags, CPoint point)
{
	// ���̾�α� ȭ�鿡�� ���콺 ���� ��ư ������, ������ �̵��ϱ� ����
	if (m_bDragMoveWnd)
	{
		SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	}
	else
	{
		CDialogEx::OnLButtonDown(flags, point);
	}
}

/*
 desc : �ڽ��� ������ �θ� ������ �߾ӿ� ��ġ
 parm : parent	- �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
 retn : None
*/
VOID CMyDialog::CenterDialog(CWnd *parent)
{
	// ���� �θ� ���� �߾ӿ� ��ġ
	if (parent)	CenterWindow(parent);
}

/*
 desc : �ڽ��� ������ ũ�⸦ ������ �Ѵ�.
 parm : rect	- ũ�⸦ ������ ���� ����
 retn : None
*/
VOID CMyDialog::ResizeDialog(CRect rect)
{
	MoveWindow(rect);
}

/*
 desc : ���̾�αװ� ����� �� ������ �ѹ� ȣ��
 parm : None
 retn : None
*/
VOID CMyDialog::OnClose()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnClose();
}
VOID CMyDialog::OnDestroy()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnDestroy();
}
VOID CMyDialog::ExitProc()
{
	m_bExitProc	= 1;

	// ���� ��ȭ ���� ����
	if (m_bTooltip)
	{
		// ���� ��Ʈ ����
		if (m_ftToolTip.GetSafeHandle())
		{
			m_ftToolTip.DeleteObject();
		}
		if (m_csToolTip.GetSafeHwnd())
		{
			m_csToolTip.DestroyWindow();
		}
	}

	// ����� �� �Լ� ȣ��
	OnExitDlg();
}

/*
 desc : �� ���� ũ�Ⱑ ����� �� ����
 parm : type	: ũ�� ���� ���� ��, SIZE_MAXIMIZED, _MINIMIZED, _RESTORED, _MAXHIDE, _MAXSHOW
		cx,cy	: ����� ũ�� ��ŭ�� ���� ��ġ ��. ��ŭ �̵��ߴ��� �˼� ����
 retn : None
 �ۼ� : ������ / ����-2008.07.26 / ����-2008.07.26
*/
VOID CMyDialog::OnSize(UINT32 nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// ���̾�α� ũ�Ⱑ ����Ǿ��� ��� ȣ��
	OnResizeDlg();
}

/*
 desc : ������ ���� ���� ����
 parm : msg	- ���� �޽���
		flag- 0x00 : ���� �ڵ� �� �ۼ�, 0x01 : ���� �ڵ� �ۼ�
 retn : None
 �ۼ� : ������ / ����-2008.09.19 / ����-2008.09.19
*/
VOID CMyDialog::SetErrMsg(TCHAR *msg, UINT8 flag)
{
	ZeroMemory(m_tzErrMsg, 1024);
	if (flag)	swprintf_s(m_tzErrMsg, 1024, _T("%s [%d]"), msg, GetLastError());
	else		swprintf_s(m_tzErrMsg, 1024, _T("%s"), msg);
}

/*
 desc : ���� ũ�� ���� ����
 parm : mm	- ���� ũ�� ������ ����� ����ü ������
 retn : None
*/
VOID CMyDialog::OnGetMinMaxInfo(MINMAXINFO* mm)
{
	// ���� ũ�� �� ���̸� �������� �� ���
	if (!m_bSizeLeftRight)
	{
		mm->ptMinTrackSize.x	= m_i32FixedWidth;
		mm->ptMaxTrackSize.x	= m_i32FixedWidth;
	}
	// ���� ũ�� �� ���̸� �������� �� ���
	if (!m_bSizeTopBottom)
	{
		mm->ptMinTrackSize.y	= m_i32FixedHeight;
		mm->ptMaxTrackSize.y	= m_i32FixedHeight;
	}
	// ���� ũ�� �ּ� �� ���Ϸ� ������ ���ϵ��� ������ ���
	if (m_bMinSizeWidth)		mm->ptMinTrackSize.x	= m_i32SizeMinWidth;
	if (m_bMinSizeHeight)		mm->ptMinTrackSize.y	= m_i32SizeMinHeight;
	CDialogEx::OnGetMinMaxInfo(mm);
}

/*
 desc : ��ư �� �ڽ� ���� ��Ʈ�ѿ� ���� �޽��� ���
 parm : ctrl_id	- [in]  ��Ʈ�� ID
		tooltip	- [in]  ���� �޽���
 retn : None
*/
VOID CMyDialog::AddToolTip(UINT32 ctrl_id, TCHAR *tooltip)
{
	m_csToolTip.AddTool(GetDlgItem(ctrl_id), tooltip);
	m_csToolTip.SetTipBkColor(RGB(255, 255, 255));
	m_csToolTip.SetTipTextColor(RGB(0, 0, 0));
}

/*
 desc : ���� ���̾�αװ� ���� ȭ���� ���� �ڸ��� ���� ���, �ڼ� ó�� �ٴ� ��� ����
 parm : lpwndpos	- [in]  ����Ű�� �ִ� WINDOWPOS â�� �� ũ��� ��ġ�� �� �� ������ ���� �ϴ� ������ ����
 retn : None
*/
VOID CMyDialog::OnWindowPosChanging(WINDOWPOS *lpwndpos)
{
	CDialogEx::OnWindowPosChanging(lpwndpos);

	/* ��� ��� ���� Ȯ�� */
	if (!m_bMagnetic)	return;

	// ����ȭ�� �ػ��� ����,���� ũ�⸦ ���Ѵ�.
	INT32 i32SizeCX = GetSystemMetrics(SM_CXSCREEN);
	INT32 i32SizeCY = GetSystemMetrics(SM_CYSCREEN);
 
	// ������ġ�� -30���� 30�� ��� ������ġ�� 0���� ����
	if((lpwndpos->x >= -30) && (lpwndpos->x <= 30)) lpwndpos->x = 0;
	if((lpwndpos->x + lpwndpos->cx >= i32SizeCX - 30)
	&& (lpwndpos->x + lpwndpos->cx <= i32SizeCX + 30))
	{
		lpwndpos->x = i32SizeCX - lpwndpos->cx;
	}
 
	// ������ġ�� -30���� 30�� ��� ������ġ�� 0���� ����
	if((lpwndpos->y >= -30) && (lpwndpos->y <= 30)) lpwndpos->y = 0;
	if((lpwndpos->y + lpwndpos->cy >= i32SizeCY - 30)
	&& (lpwndpos->y + lpwndpos->cy <= i32SizeCY + 30))
	{
		lpwndpos->y = i32SizeCY - lpwndpos->cy;
	}
}

/*
 desc : �θ� ���� ������� �߾ӿ� ��ġ
 parm : parent	- [in]  �θ� ���� �ڵ�
 retn : None
*/
VOID CMyDialog::CenterParentTopMost(HWND parent)
{
	RECT rParent;
	CRect rClient;
	CPoint ptPos;

	/* �θ� �� �ڽ� ���� ���� ��� */
	if (parent)	(CWnd::FromHandle(parent))->GetWindowRect(&rParent);
	else		GetDesktopWindow()->GetWindowRect(&rParent);
	GetClientRect(rClient);
	/* �������� ��� */
	ptPos.x	= rParent.left + (rParent.right - rParent.left) / 2 - rClient.Width() / 2;
	ptPos.y	= rParent.top + (rParent.bottom - rParent.top) / 2 - rClient.Height() / 2;

	/* ǥ�� ��ġ ���� */
	::SetWindowPos(GetSafeHwnd(), HWND_TOPMOST, ptPos.x, ptPos.y, 0, 0, SWP_NOSIZE);
}