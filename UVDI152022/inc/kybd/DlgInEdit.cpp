

#include "pch.h"
#include "DlgInEdit.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : 자신의 윈도 ID, 부모 윈도 객체
 retn : None
*/
CDlgInEdit::CDlgInEdit(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgInEdit::IDD, parent)
{
	m_bDrawBG	= 0x01;
	m_hIcon		= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgInEdit::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	DDX_Control(dx, IDC_IN_EDT_BTN_APPLY,	m_btn_ctl[0]);
	DDX_Control(dx, IDC_IN_EDT_BTN_CANCEL,	m_btn_ctl[1]);
	DDX_Control(dx, IDC_IN_EDT_EDT_VALUE,	m_edt_ctl[0]);
}

BEGIN_MESSAGE_MAP(CDlgInEdit, CMyDialog)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_IN_EDT_BTN_APPLY,		OnBtnApply)
	ON_BN_CLICKED(IDC_IN_EDT_BTN_CANCEL,	OnBtnCancel)
END_MESSAGE_MAP()


BOOL CDlgInEdit::PreTranslateMessage(MSG* msg)
{
	if (msg->message == WM_KEYDOWN && msg->wParam == VK_RETURN)
	{
		OnBtnApply();
		return TRUE;
	}

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : 시스템 명령어 제어
 parm : id		- 선택된 항목 ID 값
		lparam	- ???
 retn : 1 - 성공 / 0 - 실패
*/
VOID CDlgInEdit::OnSysCommand(UINT32 id, LPARAM lparam)
{
//	switch (id)
//	{
//	}

	CMyDialog::OnSysCommand(id, lparam);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : DI_TRUE or DI_FALSE
*/
BOOL CDlgInEdit::OnInitDlg()
{
	// 윈도 컨트롤 초기화
	if (!InitControl())		return FALSE;

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgInEdit::OnExitDlg()
{
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgInEdit::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgInEdit::OnResizeDlg()
{
}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : DI_TRUE or DI_FALSE
*/
BOOL CDlgInEdit::InitControl()
{
	INT32 i;

	// 에디트
	for (i=0; i<1; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
	}
	m_edt_ctl[0].SetFocus();
	// 일반 버튼
	for (i=0; i<2; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}

	return TRUE;
}

/*
 desc : OnOK
 parm : None
 retn : None
*/
VOID CDlgInEdit::OnBtnApply()
{
	if (m_edt_ctl[0].GetTextToLen() < 1)
	{
		AfxMessageBox(L"Not value entered!", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	m_i64Value	= m_edt_ctl[0].GetTextToNum();
	m_u64Value	= (UINT64)m_edt_ctl[0].GetTextToNum();
	m_dbValue	= m_edt_ctl[0].GetTextToDouble();

	CMyDialog::OnOK();
}

/*
 desc : OnCancel
 parm : None
 retn : None
*/
VOID CDlgInEdit::OnBtnCancel()
{

	CMyDialog::OnCancel();
}
