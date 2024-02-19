
/*
 desc : 미니 문자 키보드 출력
*/

#include "pch.h"
#include "DlgKBDT.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgKBDT::CDlgKBDT(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgKBDT::IDD, parent)
{
	m_bShift			= 0x00;	// 기본적으로 대문자
	m_bDrawBG			= 0x01;
	m_bKeyPushEnter		= 0x01;
	m_bKeyPushESC		= 0x01;
	m_bKeyEnterApply	= 0x01;
	m_bKeyESCCancel		= 0x01;
	m_pParentWnd		= NULL;
	m_bPassword			= FALSE;
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgKBDT::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32ID;

	DDX_Control(dx, IDC_KBDT_EDT_TEXT,	m_edt_ctl[0]);
#if 0
	DDX_Control(dx, IDC_KBDT_BTN_NUM0,	m_btn_ctl[0]);
	DDX_Control(dx, IDC_KBDT_BTN_NUM1,	m_btn_ctl[1]);
	DDX_Control(dx, IDC_KBDT_BTN_NUM2,	m_btn_ctl[2]);
	DDX_Control(dx, IDC_KBDT_BTN_NUM3,	m_btn_ctl[3]);
	DDX_Control(dx, IDC_KBDT_BTN_NUM4,	m_btn_ctl[4]);
	DDX_Control(dx, IDC_KBDT_BTN_NUM5,	m_btn_ctl[5]);
	DDX_Control(dx, IDC_KBDT_BTN_NUM6,	m_btn_ctl[6]);
	DDX_Control(dx, IDC_KBDT_BTN_NUM7,	m_btn_ctl[7]);
	DDX_Control(dx, IDC_KBDT_BTN_NUM8,	m_btn_ctl[8]);
	DDX_Control(dx, IDC_KBDT_BTN_NUM9,	m_btn_ctl[9]);
#else
	u32ID	= IDC_KBDT_BTN_NUM0;
	for (i=0; i<10; i++)
	{
		DDX_Control(dx, u32ID+i,	m_btn_ctl[0+i]);
	}
	u32ID	= IDC_KBDT_BTN_CHA;
	for (i=0; i<26; i++)
	{
		DDX_Control(dx, u32ID+i,	m_btn_ctl[10+i]);
	}
#endif
#if (CUSTOM_CODE_UTRONIX  != DELIVERY_PRODUCT_ID)
	DDX_Control(dx, IDC_KBDT_BTN_SLASH,	m_btn_ctl[36]);
	DDX_Control(dx, IDC_KBDT_BTN_SHIFT,	m_btn_ctl[37]);
#endif
	DDX_Control(dx, IDC_KBDT_BTN_CLOSE,	m_btn_ctl[38]);
	DDX_Control(dx, IDC_KBDT_BTN_BACK,	m_btn_ctl[39]);
	DDX_Control(dx, IDC_KBDT_BTN_CLEAR,	m_btn_ctl[40]);
	DDX_Control(dx, IDC_KBDT_BTN_ENTER,	m_btn_ctl[41]);
	DDX_Control(dx, IDC_KBDT_BTN_UBAR,	m_btn_ctl[42]);
	DDX_Control(dx, IDC_KBDT_BTN_SEL,	m_btn_ctl[43]);
	DDX_Control(dx, IDC_KBDT_BTN_MINUS,	m_btn_ctl[44]);
#if (CUSTOM_CODE_UTRONIX  != DELIVERY_PRODUCT_ID)
	DDX_Control(dx, IDC_KBDT_BTN_SPACE,	m_btn_ctl[45]);
#endif
}

BEGIN_MESSAGE_MAP(CDlgKBDT, CMyDialog)
#if (CUSTOM_CODE_UTRONIX  != DELIVERY_PRODUCT_ID)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_KBDT_BTN_ENTER, IDC_KBDT_BTN_SPACE, OnBtnClicked)
#endif
//	ON_WM_KEYUP()	/* Dialog 기반에서는 KeyDown & KeyUp Event 메시지 발생 안됨 */
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgKBDT::PreTranslateMessage(MSG* msg)
{
	switch (msg->message)
	{
	case WM_KEYUP		:
		OnKeyUp(UINT32(msg->wParam),
				UINT32(LOWORD(msg->lParam)),
				UINT32(HIWORD(msg->lParam)));
		break;
	case  WM_KEYDOWN	:
		if (msg->wParam==VK_RETURN)
		{
#if (CUSTOM_CODE_UTRONIX  != DELIVERY_PRODUCT_ID)
			OnBtnClicked(IDC_KBDT_BTN_ENTER);
#endif
		}
		break;
	}

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : Create 함수 오버라이딩 처리
 parm : parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
		value	- [in]  현재 선택된 값 즉, Edit 창에 출력될 값
 retn : Create 함수 반환 값
*/
BOOL CDlgKBDT::Create(CWnd *parent, TCHAR *value)
{
	m_pParentWnd	= parent;
	wmemset(m_tzText, 0x00, 1024);
	wcscpy_s(m_tzText, 1024, value);

//	return CMyDialog::Create(IDD_LOGS, GetDesktopWindow());
	return CMyDialog::Create();
}

/*
 desc : DoModal Overriding
 parm : max_str	- [in]  최대 입력할 수 있는 글자
 retn : INT_PTR
*/
INT_PTR CDlgKBDT::MyDoModal(UINT32 max_str, BOOL bPassword/* = FALSE*/)
{
	m_pParentWnd	= NULL;
	m_u32MaxStr		= max_str < 1023 ? max_str : 1023;
	m_bPassword		= bPassword;

	return CMyDialog::DoModal();
}

/*
 desc : DoModal Override
 변수 :	value	- [in]  현재 선택된 값 즉, Edit 창에 출력될 값
		max_str	- [in]  최대 입력할 수 있는 글자
 retn : ?
*/
INT_PTR CDlgKBDT::MyDoModal(TCHAR *value, UINT32 max_str)
{
	m_u32MaxStr	= max_str < 1023 ? max_str : 1023;

	if (value)
	{
		wmemset(m_tzText, 0x00, 1024);
		wcscpy_s(m_tzText, 1024, value);
	}

	return DoModal();
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgKBDT::OnInitDlg()
{
	// 자신의 부모 윈도 중앙에 위치 시킴
	CenterWindow(m_pParentWnd);
	/* 윈도 컨트롤 초기화 */
	if (!InitCtrl())	return FALSE;
	/* TOP_MOST & Center */
	CenterParentTopMost();

	/* 윈도 타이틀 입력 */
	TCHAR tzTitle[1024]	= {NULL}, tzMain[1024]	= {NULL};
	/* 현재 제목 */
	GetWindowText(tzMain, 1023);
	/* 최대 값 보태기 */
	swprintf_s(tzTitle, 1024, L"%s (Max : %u)", tzMain, m_u32MaxStr);
	SetWindowText(tzTitle);

	m_edt_ctl[0].SetWindowText(m_tzText);

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgKBDT::OnExitDlg()
{
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgKBDT::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgKBDT::OnResizeDlg()
{
	CRect rCtrl;
	// 현재 Client 영역
	GetClientRect(rCtrl);
}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgKBDT::InitCtrl()
{
	UINT32 i;
	LOGFONT lf	= g_lf[eFONT_LEVEL2_BOLD];

  	// 일반 버튼
	for (i=0; i<46; i++)
 	{
 		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
 		//m_btn_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);
 	}
	// 에디트
//  	lf.lfHeight	+= 16;
//  	lf.lfWeight	+= 16;
	lf.lfHeight += 8;
	lf.lfWeight += 8;
	for (i=0; i<1; i++)
	{
		m_edt_ctl[i].SetEditFont(&lf);
		m_edt_ctl[i].SetReadOnly(TRUE);
	}
	// 값 출력
	m_edt_ctl[0].SetWindowText(L"");
	m_edt_ctl[0].SetMouseClick(FALSE);


	if (m_bPassword)
	{
		m_edt_ctl[0].SetPasswordChar(_T('*'));
	}


	return TRUE;
}

/*
 desc : 숫자 키보드가 눌렸다가 떨어진 경우 감지
 parm : key_num	- [in]  주어진 키에 대한 가상 코드 값을 지정
		rep_cnt	- [in]  사용자가 키를 누르고 있을 때, 반복 횟수
		flags	- [in]  검색 코드, 키 전환 코드, 이전 키 상태 및 컨텍스트 코드를 지정
 retn : None
*/
VOID CDlgKBDT::OnKeyUp(UINT32 key_num, UINT32 rep_cnt, UINT32 flags)
{
	BOOL bSucc[7]= {FALSE};
	/*UINT8 u8Index	= 0;*/
#if 0
	TCHAR tzNum[10]	= {L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9'};
#else
	TCHAR tzNum[2]	= {0x30, 0x00};
#endif

	/* ---------------------------------------------- */
	/* 숫자 키보드 값이 눌려지지 않은 경우, 에러 처리 */
	/* ---------------------------------------------- */
	bSucc[0] = (46 < key_num && key_num < 58);
	bSucc[1] = (95 < key_num && key_num < 106);
	bSucc[2] = (190 == key_num || key_num == 110);
	bSucc[3] = (key_num == 8 || key_num == 46);
	bSucc[4] = (key_num == 189 || key_num == 109);	// '-'
	bSucc[5] = (64 < key_num && key_num < 91);
	bSucc[6] = (191 == key_num);
	// Backspace or Delete Key가 눌려진 경우
	if (bSucc[3])	DelCh();
	// 숫자 키보드에서 발생된 숫자가 입력
	else
	{
		if (bSucc[4])
		{
			if (GetKeyState(VK_SHIFT) & 0x8000)	AddCh(L'_');
			else								AddCh(L'-');
		}
		else if (bSucc[2])	AddCh(L'.');
		// 숫자 값
		else if (bSucc[0])	AddCh(tzNum[0]+TCHAR(key_num-48));
		else if (bSucc[1])	AddCh(tzNum[0]+TCHAR(key_num-96));
		// 문자 값
		else if (bSucc[5])
		{
			// Caps lock이 ON 상태인 경우
			if (GetKeyState(VK_CAPITAL) & 0x8000)
			{
				// 대문자
				if (GetKeyState(VK_SHIFT) & 0x8000)	AddCh(0x41+TCHAR(key_num-65));
				// 소문자
				else								AddCh(0x61+TCHAR(key_num-65));
			}
			else
			// Caps lock이 OFF 상태인 경우
			{
				// 소문자
				if (GetKeyState(VK_SHIFT) & 0x8000)	AddCh(0x41+TCHAR(key_num-65));
				// 대문자
				else								AddCh(0x61+TCHAR(key_num-65));
			}
		}
		else if (bSucc[6])	AddCh(L'/');
	}
}
#if (CUSTOM_CODE_UTRONIX  != DELIVERY_PRODUCT_ID)
/*
 desc : 버튼 클릭 이벤트 처리
 parm : id	- [in]  클릭된 버튼 ID
 retn : None
*/
VOID CDlgKBDT::OnBtnClicked(UINT32 id)
{
	if (id >= IDC_KBDT_BTN_NUM0 && id <= IDC_KBDT_BTN_NUM9)
	{
		AddCh(0x30+TCHAR(id-IDC_KBDT_BTN_NUM0));
	}
	else if (id >= IDC_KBDT_BTN_CHA && id <= IDC_KBDT_BTN_CHZ)
	{
		// 소문자
		if (m_bShift)	AddCh(0x61+TCHAR(id-IDC_KBDT_BTN_CHA));
		// 대문자
		else			AddCh(0x41+TCHAR(id-IDC_KBDT_BTN_CHA));
	}
	else
	{
		switch (id)
		{
		case IDC_KBDT_BTN_CLOSE		:	Cancel();			break;
		case IDC_KBDT_BTN_ENTER		:	Apply();			break;
		case IDC_KBDT_BTN_BACK		:	DelCh();			break;
		case IDC_KBDT_BTN_CLEAR		:	Clear();			break;
		case IDC_KBDT_BTN_UBAR		:	AddCh(L'_');		break;
		case IDC_KBDT_BTN_MINUS		:	AddCh(L'-');		break;
		case IDC_KBDT_BTN_SEL		:	SelectAll();		break;
#if (CUSTOM_CODE_UTRONIX  != DELIVERY_PRODUCT_ID)
		case IDC_KBDT_BTN_SHIFT		:	UpdateUpDnChar();	break;
		case IDC_KBDT_BTN_SLASH		:	AddCh(L'/');		break;
		case IDC_KBDT_BTN_SPACE		:	AddCh(L' ');		break;
#endif
		}
	}
}
#endif
/*
 desc : Keyboard에서 Enter 키 값이 눌려진 경우
 parm : None
 retn : None
*/
VOID CDlgKBDT::Apply()
{
	/* 현재 입력된 데이터 가져오기 */
	wmemset(m_tzText, 0x00, 1024);
	m_edt_ctl[0].GetWindowText(m_tzText, 1024);
	/* 입력된 문자열 길이가 없는지 확인 */
	if (wcslen(m_tzText) < 1)	return;

	if (m_pParentWnd)
	{
		m_pParentWnd->PostMessage(WM_IDD_KBDT, 1, (LPARAM)m_pParentWnd);
	}
	else
	{		
		CMyDialog::OnOK();
	}
}

/*
 desc : Keyboard에서 Cancel 키 값이 눌려진 경우
 parm : None
 retn : None
*/
VOID CDlgKBDT::Cancel()
{
	if (m_pParentWnd)
	{
		m_pParentWnd->PostMessage(WM_IDD_KBDT, 0, (LPARAM)m_pParentWnd);
	}
	else
	{
		CMyDialog::OnCancel();
	}
}

/*
 desc : 키보드의 숫자 버튼 (부호 및 Dot 포함) 활성화 여부
 parm : None
 retn : None
*/
VOID CDlgKBDT::EnableEnterKey()
{
#if 0
	BOOL bEnable	= FALSE;
	TCHAR tzVal[512]= {NULL};
	DOUBLE dbVal	= 0;
	/* 현재까지 입력된 값 중에서 Min 값보다 작거나 Max 값보다 큰 경우, 더 이상 입력 못하도록 처리 */
	m_edt_ctl[0].GetWindowTextW(tzVal, 512);
	dbVal	= _wtof(tzVal);
	if (dbVal >= m_dbMin && dbVal <= m_dbMax)	bEnable	= TRUE;
	m_btn_ctl[16].EnableWindow(bEnable);
	if (!bEnable)	m_btn_ctl[0].SetFocus();
	else			m_btn_ctl[16].SetFocus();
#endif
}

/*
 desc : 에디트 컨트롤 박스에 값 입력
 parm : ch	- [in]  입력 (추가)될 문자 값
 retn : None
*/
VOID CDlgKBDT::AddCh(TCHAR ch)
{
	TCHAR tzText[2]	= {ch, NULL}, tzData[1024] = {NULL};
	INT32 i32Len	= m_edt_ctl[0].GetWindowTextLength();

	// 충분히 긴 텍스트가 입력된 상태인지 확인
	if (i32Len > (INT32)m_u32MaxStr || i32Len > 1023)	return;
	// 현재까지 입력된 문자열 얻기
	m_edt_ctl[0].GetWindowText(tzData, 1023);
#if 0
	/* 연속된 Dot (.) 입력 방지하기 위함 */
	if (ch == L'.' && tzData[i32Len-1] == ch)	return;
	// 만약 입력된 첫 글자가 Dot (L'.')인 경우라면, 혹은 '+', '-' 이라면
	if ((i32Len == 0 && ch == L'.') ||
		(i32Len == 1 && ch == L'.' && (tzData[0] == L'+' || tzData[0] == L'-')))
	{
		tzText[0]	= L'0';
		tzText[1]	= ch;
	}
	m_edt_ctl[0].SetFocus();
	// 입력된 문자열의 마지막으로 이동
	m_edt_ctl[0].SetSel(i32Len, i32Len);
	// 마지막에 현재 문자열 추가
	m_edt_ctl[0].ReplaceSel(tzText);
#else
	tzData[i32Len]	= ch;
	m_edt_ctl[0].SetWindowTextW(tzData);
#endif
	/* Enable Enter Key */
	EnableEnterKey();
}

/*
 desc : 에디트 컨트롤 박스에 값 입력
 parm : None
 retn : None
*/
VOID CDlgKBDT::DelCh()
{
	TCHAR tzText[1024]	= {NULL};
	INT32 i32Len		= m_edt_ctl[0].GetWindowTextLength();

	// 현재 입력된 문자 복사
	m_edt_ctl[0].GetWindowText(tzText, 1023);
	// 더 이상 제거될 문자가 없는지 확인
	if (i32Len < 1)	return;
	// 마지막 문자 제거
	tzText[i32Len-1]	= NULL;
	// 수정된 문자열 붙여넣기
	m_edt_ctl[0].SetWindowText(tzText);

	/* Enable Enter Key */
	EnableEnterKey();
}

/*
 desc : 문자열 초기화
 parm : None
 retn : None
*/
VOID CDlgKBDT::Clear()
{
	// 포커스 설정
	m_edt_ctl[0].SetFocus();
	// 입력된 문자열의 전체 선택
	m_edt_ctl[0].SetSel(0, -1, TRUE);
	m_edt_ctl[0].Clear();

	/* Enable Enter Key */
	EnableEnterKey();
}

/*
 desc : 부등호 입력
 parm : ch	- [in]  0x00 - 버튼 '+/-', 0x01 - '+', 0x02 - '-'
 retn : None
*/
VOID CDlgKBDT::SetSign(UINT8 ch)
{
	TCHAR tzText[1024]	= {NULL};
	INT32 i, i32Len		= m_edt_ctl[0].GetWindowTextLength();

	// 현재 입력된 문자 복사
	m_edt_ctl[0].GetWindowText(tzText, 1023);
	// 입력된 문자열의 맨 앞의 문자가 '+' or '-'인지 확인
	if (tzText[0] == L'+' || tzText[0] == L'-')
	{
		if (tzText[0] == L'+')	tzText[0] = (0x01 == ch) ? L'+' : L'-';
		else					tzText[0] = (0x02 == ch) ? L'-' : L'+';
	}
	else
	{
		// 일단 기존 입력된 문자열을 모두 한 개씩 뒤로 이동 시킴
		for (i=i32Len-1; i>=0; i--)	tzText[i+1]	= tzText[i];
		// 맨 앞에 무조건 L'+' 값 입력
		tzText[0] = (0x01 == ch) ? L'+' : L'-';
	}

	// 문자열 입력
	m_edt_ctl[0].SetWindowText(tzText);

	/* Enable Enter Key */
	EnableEnterKey();
}

/*
 desc : 문자열 값 반환
 parm : value	- [out] 반환되어 저장될 참조 변수
		size	- [in]  'value' 버퍼 크기
 retn : None
*/
VOID CDlgKBDT::GetText(TCHAR *text, UINT32 size)
{
	if (m_pParentWnd)
	{
		m_edt_ctl[0].GetWindowText(text, size);
	}
	else
	{
		wmemcpy_s(text, size, m_tzText, wcslen(m_tzText));
	}
}

/*
 desc : 키보드 값 중 Dot '.' 버튼을 Enable or Disabl 처리
 parm : flag	- [in]  Enable or Disable
 retn : None
*/
VOID CDlgKBDT::DotEnable(BOOL enable)
{
	m_btn_ctl[15].EnableWindow(enable);
}

/*
 desc : 키보드 값 중 '+/-' 버튼을 Enable or Disabl 처리
 parm : flag	- [in]  Enable or Disable
 retn : None
*/
VOID CDlgKBDT::SignEnable(BOOL enable)
{
	m_btn_ctl[14].EnableWindow(enable);
}

/*
 desc : Edit 창의 내용을 모두 선택 한다
 parm : None
 retn : None
*/
VOID CDlgKBDT::SelectAll()
{
	// 포커스 설정
	m_edt_ctl[0].SetFocus();
	// 입력된 문자열의 전체 선택
	m_edt_ctl[0].SetSel(0, -1, TRUE);
}

/*
 desc : 대문자 or 소문자 처리
 parm : None
 retn : None
*/
VOID CDlgKBDT::UpdateUpDnChar()
{
	TCHAR tzNum	= 0x00, tzAlph[2]	= {0x00, 0x00};
	UINT32 i, u32ID	= IDC_KBDT_BTN_CHA;

	// 대소문자
	m_bShift	= !m_bShift;
	// 소문자화
	if (m_bShift)	tzNum = 0x61;
	// 대문자화
	else			tzNum = 0x41;
	// 버튼 텍스트 변경
	for (i=0; i<26; i++)
	{
		tzAlph[0]	= tzNum + TCHAR(i);
		m_btn_ctl[10+i].SetWindowText(tzAlph);
	}
}