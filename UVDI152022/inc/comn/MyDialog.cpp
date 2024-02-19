
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
	m_pParentWnd			= parent;	// 자신을 호출한 부모 객체 포인터
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
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
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
		// 툴팁 사용을 위해서
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
 desc : PreCreateWindow 가상함수 재정의
 parm : cs	- [in]  CREATESTRUCT 구조체
 retn : TRUE or FALSE
*/
BOOL CMyDialog::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CDialogEx::PreCreateWindow(cs))	return FALSE;

	/* 윈도 시스템 아이콘 (최소화 / 최대화) 버튼 보이기/숨기기 설정 */
	if (!m_bEnableMinimizeBox)	cs.style &= ~ (LONG)WS_MINIMIZEBOX;	/* 최소화 버튼 Disable */
	if (!m_bEnableMaximizeBox)	cs.style &= ~ (LONG)WS_MAXIMIZEBOX;	/* 최소화 버튼 Disable */

	return TRUE;
}

/*
 desc : 모달리스 다이얼로그 생성을 위함
 parm : None
 retn : 1-모달리스 생성 성공 / 0-실패
*/
BOOL CMyDialog::Create()
{
	return CDialogEx::Create(m_u32DlgID, m_pParentWnd);
}

/*
 desc : 다이얼로그 생성될 때 처음 실행되는 부분
 parm : None
 retn : 1-성공, 0-실패
*/
BOOL CMyDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_bExitProc		= 0x00;	/* !!! 반드시 초기화 !!! */
	CFont *pFont	= NULL;

	if (m_hIcon)
	{
		SetIcon(m_hIcon, 1);	// 큰 아이콘을 설정합니다.
		SetIcon(m_hIcon, 0);	// 작은 아이콘을 설정합니다.
	}

	// 배경색 설정 여부
	if (m_bDrawBG)
	{
		m_brushBg = ::CreateSolidBrush(m_clrBg);
	}

	// 이미지 적재
	LoadImages();

	// 툴팁 생성
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
			// 툴팁 초기화
			m_csToolTip.SetMaxTipWidth(1000);	// 툴팁 메시지 길이 범위 (픽셀)
			m_csToolTip.SetDelayTime(500);		// 툴팁 출력 지연 시간(밀리세컨트. 0.5초 후에 출력됨)
		}

		// 다이얼로그 기본 폰트 정보
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
	// 다이얼로그 제목 얻기
	GetWindowText(m_tzTitle, 1024);
	// 다이얼로그 초기화를 위함 함수 호출
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
		// 윈도 해당 마우스 포인터 위치로 배치 하기
		GetClientRect(rClient);
		MoveWindow(m_csPointMove.x, m_csPointMove.y, rClient.Width(), rClient.Height());
	}

	// 다이얼로그 크기 재조정 호출(?)
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
		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// 왼쪽 상단 부터 출력
		if (m_enBIA == EN_BIA::en_lefttop)
		{
			if(m_csPicBg.IsDrawImage())	m_csPicBg.Show(&dc, m_rPicBg);
		}
		// 정중앙에 배치
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
		// 특외 중앙
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
		// 기타
		else
		{
		}

		// 윈도가 갱신될 때마다 호출됨. 화면 정보를 다시 그리기 위해서
		OnPaintDlg(&dc);

		CDialogEx::OnPaint();
	}

	UpdateUILevel();
}

/*
 desc : 최소화된 CWnd가 사용자에 의해서 드래그될 때 호출
 parm : None
 retn : 커서 핸들 반환
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
		if (m_bDrawBG)	hbr = m_brushBg;  // 여기서 지정하는 색이 바꾸고 싶은 색
		break;
	}

	return hbr;
}

/*
 desc : 다잉한 이미지 정보를 적재
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMyDialog::LoadImages()
{
	//	배경 이미지 올리기
	if (_tcslen(m_tzBgFile) < 8)
	{
		SetErrMsg(_T("적재할 이미지 파일 오류. 파일 명 재 확인"));
		return 0;
	}
	// 이미지 파일 적재
	if (!m_csPicBg.Load(m_tzBgFile))
	{
		SetErrMsg(_T("이미지 파일 적재 실패. 파일 명 및 경로명 확인"));
		return 0;
	}

	if (!m_csPicBg.GetImgHandle())
	{
		SetErrMsg(_T("이미지 핸들 정보 얻기 실패. 알수 없는 오류"));
		return 0;
	}

	RECT rImgSize;
	m_csPicBg.GetPicRect(rImgSize);
	m_rPicBg = CRect(rImgSize);

	return 1;
}

/*
 desc : 마우스 왼쪽 버튼을 클릭한 상태일 경우
 parm : - flags	: ???
		- point	: ???
 retn : None
*/
VOID CMyDialog::OnLButtonDown(UINT32 flags, CPoint point)
{
	// 다이얼로그 화면에서 마우스 왼쪽 버튼 누르고, 윈도를 이동하기 위함
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
 desc : 자신의 윈도를 부모 윈도의 중앙에 배치
 parm : parent	- 자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
VOID CMyDialog::CenterDialog(CWnd *parent)
{
	// 윈도 부모 윈도 중앙에 배치
	if (parent)	CenterWindow(parent);
}

/*
 desc : 자신의 윈도의 크기를 재조정 한다.
 parm : rect	- 크기를 조정할 윈도 영역
 retn : None
*/
VOID CMyDialog::ResizeDialog(CRect rect)
{
	MoveWindow(rect);
}

/*
 desc : 다이얼로그가 종료될 때 마지막 한번 호출
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

	// 툴팁 대화 상자 제거
	if (m_bTooltip)
	{
		// 툴팁 폰트 해제
		if (m_ftToolTip.GetSafeHandle())
		{
			m_ftToolTip.DeleteObject();
		}
		if (m_csToolTip.GetSafeHwnd())
		{
			m_csToolTip.DestroyWindow();
		}
	}

	// 종료될 때 함수 호출
	OnExitDlg();
}

/*
 desc : 뷰 영역 크기가 변경될 때 수행
 parm : type	: 크기 변경 형식 즉, SIZE_MAXIMIZED, _MINIMIZED, _RESTORED, _MAXHIDE, _MAXSHOW
		cx,cy	: 변경된 크기 만큼의 영역 수치 값. 얼만큼 이동했는지 알수 있음
 retn : None
 작성 : 강진수 / 생성-2008.07.26 / 수정-2008.07.26
*/
VOID CMyDialog::OnSize(UINT32 nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// 다이얼로그 크기가 변경되었을 경우 호출
	OnResizeDlg();
}

/*
 desc : 에러에 대한 정보 설정
 parm : msg	- 에러 메시지
		flag- 0x00 : 에러 코드 미 작성, 0x01 : 에러 코드 작성
 retn : None
 작성 : 강진수 / 생성-2008.09.19 / 수정-2008.09.19
*/
VOID CMyDialog::SetErrMsg(TCHAR *msg, UINT8 flag)
{
	ZeroMemory(m_tzErrMsg, 1024);
	if (flag)	swprintf_s(m_tzErrMsg, 1024, _T("%s [%d]"), msg, GetLastError());
	else		swprintf_s(m_tzErrMsg, 1024, _T("%s"), msg);
}

/*
 desc : 윈도 크기 조절 막기
 parm : mm	- 윈도 크기 정보가 저장된 구조체 포인터
 retn : None
*/
VOID CMyDialog::OnGetMinMaxInfo(MINMAXINFO* mm)
{
	// 윈도 크기 중 넓이를 고정으로 한 경우
	if (!m_bSizeLeftRight)
	{
		mm->ptMinTrackSize.x	= m_i32FixedWidth;
		mm->ptMaxTrackSize.x	= m_i32FixedWidth;
	}
	// 윈도 크기 중 높이를 고정으로 한 경우
	if (!m_bSizeTopBottom)
	{
		mm->ptMinTrackSize.y	= m_i32FixedHeight;
		mm->ptMaxTrackSize.y	= m_i32FixedHeight;
	}
	// 윈도 크기 최소 값 이하로 줄이지 못하도록 설정된 경우
	if (m_bMinSizeWidth)		mm->ptMinTrackSize.x	= m_i32SizeMinWidth;
	if (m_bMinSizeHeight)		mm->ptMinTrackSize.y	= m_i32SizeMinHeight;
	CDialogEx::OnGetMinMaxInfo(mm);
}

/*
 desc : 버튼 등 자식 윈도 컨트롤에 툴팁 메시지 등록
 parm : ctrl_id	- [in]  컨트롤 ID
		tooltip	- [in]  툴팁 메시지
 retn : None
*/
VOID CMyDialog::AddToolTip(UINT32 ctrl_id, TCHAR *tooltip)
{
	m_csToolTip.AddTool(GetDlgItem(ctrl_id), tooltip);
	m_csToolTip.SetTipBkColor(RGB(255, 255, 255));
	m_csToolTip.SetTipTextColor(RGB(0, 0, 0));
}

/*
 desc : 현재 다이얼로그가 윈도 화면의 가장 자리에 들어온 경우, 자석 처럼 붙는 기능 지원
 parm : lpwndpos	- [in]  가리키는 있는 WINDOWPOS 창의 새 크기와 위치에 대 한 정보를 포함 하는 데이터 구조
 retn : None
*/
VOID CMyDialog::OnWindowPosChanging(WINDOWPOS *lpwndpos)
{
	CDialogEx::OnWindowPosChanging(lpwndpos);

	/* 기능 사용 여부 확인 */
	if (!m_bMagnetic)	return;

	// 바탕화면 해상도의 가로,세로 크기를 구한다.
	INT32 i32SizeCX = GetSystemMetrics(SM_CXSCREEN);
	INT32 i32SizeCY = GetSystemMetrics(SM_CYSCREEN);
 
	// 가로위치가 -30에서 30일 경우 가로위치를 0으로 세팅
	if((lpwndpos->x >= -30) && (lpwndpos->x <= 30)) lpwndpos->x = 0;
	if((lpwndpos->x + lpwndpos->cx >= i32SizeCX - 30)
	&& (lpwndpos->x + lpwndpos->cx <= i32SizeCX + 30))
	{
		lpwndpos->x = i32SizeCX - lpwndpos->cx;
	}
 
	// 세로위치가 -30에서 30일 경우 세로위치를 0으로 세팅
	if((lpwndpos->y >= -30) && (lpwndpos->y <= 30)) lpwndpos->y = 0;
	if((lpwndpos->y + lpwndpos->cy >= i32SizeCY - 30)
	&& (lpwndpos->y + lpwndpos->cy <= i32SizeCY + 30))
	{
		lpwndpos->y = i32SizeCY - lpwndpos->cy;
	}
}

/*
 desc : 부모 윈도 기반으로 중앙에 배치
 parm : parent	- [in]  부모 윈도 핸들
 retn : None
*/
VOID CMyDialog::CenterParentTopMost(HWND parent)
{
	RECT rParent;
	CRect rClient;
	CPoint ptPos;

	/* 부모 및 자식 윈도 영역 얻기 */
	if (parent)	(CWnd::FromHandle(parent))->GetWindowRect(&rParent);
	else		GetDesktopWindow()->GetWindowRect(&rParent);
	GetClientRect(rClient);
	/* 시작지점 얻기 */
	ptPos.x	= rParent.left + (rParent.right - rParent.left) / 2 - rClient.Width() / 2;
	ptPos.y	= rParent.top + (rParent.bottom - rParent.top) / 2 - rClient.Height() / 2;

	/* 표시 위치 지정 */
	::SetWindowPos(GetSafeHwnd(), HWND_TOPMOST, ptPos.x, ptPos.y, 0, 0, SWP_NOSIZE);
}