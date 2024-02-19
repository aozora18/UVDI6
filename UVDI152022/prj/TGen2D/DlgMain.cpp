
/*
 desc : Main Dialog
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"


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
CDlgMain::CDlgMain(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgMain::IDD, parent)
{
	m_bDrawBG				= 0x01;
	m_hIcon					= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgMain::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_WM_SYSCOMMAND()
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_BTN_APP_EXIT, IDC_MAIN_BTN_LIB_CLOSE,	OnBtnClicked)
END_MESSAGE_MAP()

/*
 desc : 시스템 명령어 제어
 parm : id		- 선택된 항목 ID 값
		lparam	- ???
 retn : 1 - 성공 / 0 - 실패
*/
VOID CDlgMain::OnSysCommand(UINT32 id, LPARAM lparam)
{
//	switch (id)
//	{
//	}

	CMyDialog::OnSysCommand(id, lparam);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::OnInitDlg()
{
	UINT32 u32Size	= 0, i = 0;

	/* 라이브러리 초기화 */
//	if (!InitLib())		return FALSE;

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgMain::OnExitDlg()
{
	/* 라이브러리 해제 */
//	CloseLib();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgMain::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgMain::OnResizeDlg()
{
}

/*
 desc : Library Init
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::InitLib()
{
	return uvEng_Init(ENG_ERVM::en_cmps_sw);
}

/*
 desc : Library Close
 parm : None
 retn : None
*/
VOID CDlgMain::CloseLib()
{
	uvEng_Close();
}

/*
 desc : 일반 버튼 이벤트 처리
 parm : id	- [in]  Click된 버튼 ID
 retn : None
*/
VOID CDlgMain::OnBtnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_MAIN_BTN_APP_EXIT	:	PostMessage(WM_CLOSE, 0, 0L);		break;
	case IDC_MAIN_BTN_LIB_CLOSE	:	uvEng_Close();						break;
	case IDC_MAIN_BTN_LIB_INIT	:	uvEng_Init(ENG_ERVM::en_cmps_sw);	break;
	}
}