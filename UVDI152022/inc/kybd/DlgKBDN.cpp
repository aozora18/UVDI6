
/*
 desc : 미니 숫자 키보드 출력
*/

#include "pch.h"
#include "DlgKBDN.h"


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
CDlgKBDN::CDlgKBDN(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgKBDN::IDD, parent)
{
	m_bDrawBG			= 0x01;
	m_bKeyPushEnter		= 0x01;
	m_bKeyPushESC		= 0x01;
	m_bKeyEnterApply	= 0x01;
	m_bKeyESCCancel		= 0x01;
	m_bDotUse			= TRUE;
	m_bSignUse			= TRUE;
	m_pParentWnd		= NULL;
	m_bEnableEnterKey	= FALSE;

	wmemset(m_tzTitle, 0x00, 1024);
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgKBDN::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	DDX_Control(dx, IDC_KBDN_EDT_VALUE,	m_edt_ctl[0]);
	/* button control */
	u32StartID	= IDC_KBDN_BTN_CLOSE;
	for (i=0; i<17; i++)	DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgKBDN, CMyDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_KBDN_BTN_CLOSE, IDC_KBDN_BTN_ENTER, OnBtnClicked)
//	ON_WM_KEYUP()	/* Dialog 기반에서는 KeyDown & KeyUp Event 메시지 발생 안됨 */
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgKBDN::PreTranslateMessage(MSG* msg)
{
	if (msg->message == WM_KEYUP)
	{
		if (msg->wParam != VK_RETURN && msg->wParam != VK_ESCAPE)
		{
			OnKeyUp(UINT32(msg->wParam),
					UINT32(LOWORD(msg->lParam)),
					UINT32(HIWORD(msg->lParam)));
		}
	}
	else if (msg->message == WM_KEYDOWN)
	{
		if (msg->wParam==VK_RETURN)
		{
			if (m_bKeyPushEnter && m_bEnableEnterKey)
			{
				Apply();
			}
		}
	}

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : Create 함수 오버라이딩 처리
 parm : parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
		value	- [in]  현재 설정되어 있는 값. Edit 창에 출력되는 값
		dot_use	- [in]  TRUE - 소숫점 포함, FALSE - 소숫점 미포함
		sign_use- [in]  TRUE - 음수 사용, FALSE - 음수 사용 안함
		min_val	- [in]  입력 가능한 최소 값
		max_val	- [in]  입력 가능한 최대 값
 retn : Create 함수 반환 값
*/
BOOL CDlgKBDN::Create(CWnd *parent,
					  BOOL dot_use,
					  BOOL sign_use,
					  DOUBLE min_val,
					  DOUBLE max_val)
{
	m_pParentWnd	= parent;
	m_bDotUse		= dot_use;
	m_bSignUse		= sign_use;
	m_dbMin			= min_val;
	m_dbMax			= max_val;

//	return CMyDialog::Create(IDD_LOGS, GetDesktopWindow());
	return CMyDialog::Create();
}

/*
 desc : DoModal Overriding
 parm : dot_use	- [in]  TRUE - 소숫점 포함, FALSE - 소숫점 미포함
		sign_use- [in]  TRUE - 음수 사용, FALSE - 음수 사용 안함
		min_val	- [in]  입력 가능한 최소 값
		max_val	- [in]  입력 가능한 최대 값
 retn : INT_PTR
*/
INT_PTR CDlgKBDN::MyDoModal(TCHAR *title, BOOL dot_use, BOOL sign_use, DOUBLE min_val, DOUBLE max_val)
{
	wmemset(m_tzTitle, 0x00, 1024);
	if (title)	wcscpy_s(m_tzTitle, 1024, title);

	m_pParentWnd= NULL;
	m_bDotUse	= dot_use;
	m_bSignUse	= sign_use;

	m_dbMin		= min_val;
	m_dbMax		= max_val;

	return CMyDialog::DoModal();
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgKBDN::OnInitDlg()
{
	// 자신의 부모 윈도 중앙에 위치 시킴
	CenterWindow(m_pParentWnd);
	/* 윈도 제목 설정 */
	if (wcslen(m_tzTitle))	SetWindowText(m_tzTitle);

	/* 윈도 컨트롤 초기화 */
	if (!InitCtrl())	return FALSE;
	/* TOP_MOST & Center */
	CenterParentTopMost();

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgKBDN::OnExitDlg()
{
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgKBDN::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgKBDN::OnResizeDlg()
{
	CRect rCtrl;
	// 현재 Client 영역
	GetClientRect(rCtrl);
}

void CDlgKBDN::RegisterUILevel()
{

}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgKBDN::InitCtrl()
{
	UINT32 i;
	LOGFONT lf	= g_lf[eFONT_LEVEL2_BOLD];

  	// 일반 버튼
	for (i=0; i<17; i++)
 	{
 		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
 		//m_btn_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);
 	}
	m_btn_ctl[16].EnableWindow(FALSE);
	m_btn_ctl[0].SetFocus();
	// 에디트
 	lf.lfHeight	+= 16;
 	lf.lfWeight	+= 16;
	for (i=0; i<1; i++)
	{
		m_edt_ctl[i].SetEditFont(&lf);
		m_edt_ctl[i].SetReadOnly(TRUE);
	}
	// 값 출력
	m_edt_ctl[0].SetWindowText(L"");
	m_edt_ctl[0].SetMouseClick(FALSE);

	/* 부호 포함 여부 */
	m_btn_ctl[11].EnableWindow((BOOL)m_bSignUse);
	/* 소숫점 포함 여부 */
	m_btn_ctl[12].EnableWindow((BOOL)m_bDotUse);

	return TRUE;
}

/*
 desc : 숫자 키보드가 눌렸다가 떨어진 경우 감지
 parm : key_num	- [in]  주어진 키에 대한 가상 코드 값을 지정
		rep_cnt	- [in]  사용자가 키를 누르고 있을 때, 반복 횟수
		flags	- [in]  검색 코드, 키 전환 코드, 이전 키 상태 및 컨텍스트 코드를 지정
 retn : None
*/
VOID CDlgKBDN::OnKeyUp(UINT32 key_num, UINT32 rep_cnt, UINT32 flags)
{
	BOOL bSucc[6]	= {FALSE};
	TCHAR tzNum[10]	= {L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9'};

	/* ---------------------------------------------- */
	/* 숫자 키보드 값이 눌려지지 않은 경우, 에러 처리 */
	/* ---------------------------------------------- */
	bSucc[0] = (47 < key_num && key_num < 58);
	bSucc[1] = (95 < key_num && key_num < 106);
	bSucc[2] = (190 == key_num || key_num == 110);
	bSucc[3] = (key_num == 8 || key_num == 46);
	bSucc[4] = (key_num == 189 || key_num == 109);	// '-'
	bSucc[5] = (key_num == 187 || key_num == 107);	// '+'
	// Backspace or Delete Key가 눌려진 경우
	if (bSucc[3])	DelCh();
	// 숫자 키보드에서 발생된 숫자가 입력
	else
	{
		if (bSucc[4])		SetSign(0x02);	// '-'
		else if (bSucc[5])	SetSign(0x01);	// '+'
		else if (bSucc[2])	AddCh(L'.');
		else if (bSucc[0])	AddCh(tzNum[key_num-48]);
		else if (bSucc[1])	AddCh(tzNum[key_num-96]);
	}
}

/*
 desc : 버튼 클릭 이벤트 처리
 parm : id	- [in]  클릭된 버튼 ID
 retn : None
*/
VOID CDlgKBDN::OnBtnClicked(UINT32 id)
{
	TCHAR tzNum[11]	= {L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'0', L'.'};
	switch (id)
	{
	case IDC_KBDN_BTN_CLOSE		:	Cancel();	break;
	case IDC_KBDN_BTN_ENTER		:	Apply();	break;
	case IDC_KBDN_BTN_NUM1		:
	case IDC_KBDN_BTN_NUM2		:
	case IDC_KBDN_BTN_NUM3		:
	case IDC_KBDN_BTN_NUM4		:
	case IDC_KBDN_BTN_NUM5		:
	case IDC_KBDN_BTN_NUM6		:
	case IDC_KBDN_BTN_NUM7		:
	case IDC_KBDN_BTN_NUM8		:
	case IDC_KBDN_BTN_NUM9		:
	case IDC_KBDN_BTN_NUM0		:	AddCh(tzNum[id-IDC_KBDN_BTN_NUM1]);	break;
	case IDC_KBDN_BTN_BACK		:	DelCh();			break;
	case IDC_KBDN_BTN_CLEAR		:	Clear();			break;
	case IDC_KBDN_BTN_SIGN		:	SetSign(0x00);		break;
	case IDC_KBDN_BTN_DOT		:	AddCh(tzNum[10]);	break;
	case IDC_KBDN_BTN_SEL		:	SelectAll();		break;
	}
}

/*
 desc : 키보드의 숫자 버튼 (부호 및 Dot 포함) 활성화 여부
 parm : None
 retn : None
*/
VOID CDlgKBDN::EnableEnterKey()
{
	BOOL bEnable	= FALSE;
	TCHAR tzVal[512]= {NULL};
	DOUBLE dbVal	= 0;
	/* 현재까지 입력된 값 중에서 Min 값보다 작거나 Max 값보다 큰 경우, 더 이상 입력 못하도록 처리 */
	m_edt_ctl[0].GetWindowTextW(tzVal, 512);
	dbVal	= _wtof(tzVal);
	m_bEnableEnterKey	= FALSE;
	if (dbVal >= m_dbMin && dbVal <= m_dbMax)	m_bEnableEnterKey	= TRUE;
	m_btn_ctl[16].EnableWindow(m_bEnableEnterKey);
	if (!bEnable)	m_btn_ctl[0].SetFocus();
	else			m_btn_ctl[16].SetFocus();
}

/*
 desc : Keyboard에서 Enter 키 값이 눌려진 경우
 parm : None
 retn : None
*/
VOID CDlgKBDN::Apply()
{
	/* 문자열 버퍼 초기화 */
	wmemset(m_tzValue, 0x00, 1024);

	/* 현재 설정된 값 얻기 */
	m_edt_ctl[0].GetWindowText(m_tzValue, 1024);	/* 문자열 얻기 */
	m_i16Value	= (INT16)_wtoi(m_tzValue);			/* I64 얻기 */
	m_u16Value	= (UINT16)_wtoi(m_tzValue);			/* U64 얻기 */
	m_i32Value	= (INT32)_wtoi(m_tzValue);			/* I64 얻기 */
	m_u32Value	= (UINT32)_wtoi(m_tzValue);			/* U64 얻기 */
	m_i64Value	= (INT64)_wtoi(m_tzValue);			/* I64 얻기 */
	m_u64Value	= (UINT64)_wtoi(m_tzValue);			/* U64 얻기 */
	m_dbValue	= (DOUBLE)_wtof(m_tzValue);			/* DOUBLE 얻기 */

	/* 입력된 문자열 길이가 없는지 확인 */
	if (wcslen(m_tzValue) < 1)	return;

	if (m_pParentWnd)
	{
		m_pParentWnd->PostMessage(WM_IDD_KBDM, 1, (LPARAM)m_pParentWnd);
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
VOID CDlgKBDN::Cancel()
{
	if (m_pParentWnd)
	{
		m_pParentWnd->PostMessage(WM_IDD_KBDM, 0, (LPARAM)m_pParentWnd);
	}
	else
	{
		CMyDialog::OnCancel();
	}
}


/*
 desc : 에디트 컨트롤 박스에 값 입력
 parm : ch	- [in]  입력 (추가)될 문자 값
 retn : None
*/
VOID CDlgKBDN::AddCh(TCHAR ch)
{
	TCHAR tzText[2]	= {ch, NULL}, tzData[256] = {NULL};
	INT32 i, i32Len	= m_edt_ctl[0].GetWindowTextLength();

	// 충분히 긴 텍스트가 입력된 상태인지 확인
	if (i32Len > 24)	return;
	// 현재 입력된 문자가 Dot (L'.')이고, 기존에 이미 Dot가 입력된 상태인지 확인
	for (i=0; (ch == L'.' && i<i32Len); i++)
	{
		// 현재까지 입력된 문자열 얻기
		m_edt_ctl[0].GetWindowText(tzData, 256);
		if (tzData[i] == L'.')	return;
	}
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

	/* Enable Enter Key */
	EnableEnterKey();
}

/*
 desc : 에디트 컨트롤 박스에 값 입력
 parm : None
 retn : None
*/
VOID CDlgKBDN::DelCh()
{
	TCHAR tzText[256]	= {NULL};
	INT32 i32Len		= m_edt_ctl[0].GetWindowTextLength();

	// 현재 입력된 문자 복사
	m_edt_ctl[0].GetWindowText(tzText, 256);
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
VOID CDlgKBDN::Clear()
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
VOID CDlgKBDN::SetSign(UINT8 ch)
{
	TCHAR tzText[256]	= {NULL};
	INT32 i, i32Len		= m_edt_ctl[0].GetWindowTextLength();

	// 현재 입력된 문자 복사
	m_edt_ctl[0].GetWindowText(tzText, 256);
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
 desc : 키보드 값 중 Dot '.' 버튼을 Enable or Disabl 처리
 parm : flag	- [in]  Enable or Disable
 retn : None
*/
VOID CDlgKBDN::DotEnable(BOOL enable)
{
	m_btn_ctl[12].EnableWindow(enable);
}

/*
 desc : 키보드 값 중 '+/-' 버튼을 Enable or Disabl 처리
 parm : flag	- [in]  Enable or Disable
 retn : None
*/
VOID CDlgKBDN::SignEnable(BOOL enable)
{
	m_btn_ctl[11].EnableWindow(enable);
}

/*
 desc : Edit 창의 내용을 모두 선택 한다
 parm : None
 retn : None
*/
VOID CDlgKBDN::SelectAll()
{
	// 포커스 설정
	m_edt_ctl[0].SetFocus();
	// 입력된 문자열의 전체 선택
	m_edt_ctl[0].SetSel(0, -1, TRUE);
}