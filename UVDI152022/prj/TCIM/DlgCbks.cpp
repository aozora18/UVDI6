
/*
 desc : Callback Event
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgCbks.h"


IMPLEMENT_DYNAMIC(CDlgCbks, CDialogEx)

/*
 desc : 생성자
 parm : None
 retn : None
*/
CDlgCbks::CDlgCbks(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCbks::IDD, pParent)
{
	m_bExitProc	= FALSE;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgCbks::~CDlgCbks()
{
}

/* 컨트롤 연결 */
void CDlgCbks::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);

}

/* 메시지 맵 */
BEGIN_MESSAGE_MAP(CDlgCbks, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/*
 desc : 다이얼로그 생성 후 초기화
*/
BOOL CDlgCbks::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	/* 컨트롤 초기화 */
	InitCtrl();

	return TRUE;
}

/*
 desc : 다이얼로그가 종료될 때 마지막 한번 호출
 parm : None
 retn : None
*/
VOID CDlgCbks::OnClose()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnClose();
}
VOID CDlgCbks::OnDestroy()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnDestroy();
}
VOID CDlgCbks::ExitProc()
{
	/* 이미 이전에 호출된적이 있음 */
	m_bExitProc	= TRUE;

}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgCbks::InitCtrl()
{
}

/*
 desc : 주기적으로 호출되는 함수
 parm : None
 retn : None
*/
VOID CDlgCbks::UpdateCtrl()
{
}

/*
 desc : 이벤트 정보가 수신된 경우 (부모에 의해서 호출됨)
 parm : event	- [in]  이벤트 정보가 저장된 구조체 포인터
 retn : None
*/
VOID CDlgCbks::UpdateCtrl(STG_CCED *event)
{
}

