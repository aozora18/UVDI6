
/*
 desc : 프로그램 종료될 때, 대기 알림
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgWait.h"
#include "../mesg/DlgMesg.h"


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
CDlgWait::CDlgWait(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgWait::IDD, parent)
{
	m_bDrawBG	= TRUE;
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgWait::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	DDX_Control(dx, IDC_WAIT_TXT_ANI, m_csWaitAni);
}

BEGIN_MESSAGE_MAP(CDlgWait, CMyDialog)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgWait::PreTranslateMessage(MSG* msg)
{

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgWait::OnInitDlg()
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	CDlgMesg dlgMesg;

	/* 타이틀 이미지 적재 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\ui_img\\logo\\wait.jpg", g_tzWorkDir);
	if (!m_csWaitPic.Load(tzFile))
	{
		dlgMesg.MyDoModal(L"Failed to load the image (wait.jpg)", 0x01);
	}

	/* 애니메이션 이미지 적재 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\ui_img\\ani\\shutdown_wait.gif", g_tzWorkDir);
	if (!m_csWaitAni.Load(tzFile, FALSE))
	{
		dlgMesg.MyDoModal(L"Failed to load the image (shutdown_wait.gif)", 0x01);
	}
	m_csWaitAni.Play();

	/* TOP_MOST & Center */
	CenterParentTopMost();

	return TRUE;
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgWait::OnPaintDlg(CDC *dc)
{
	if (m_csWaitPic.IsDrawImage())
	{
		CRect rDraw;
		GetClientRect(rDraw);
		rDraw.left	+= 13;
		rDraw.top	+= 13;
		rDraw.right	= rDraw.left + m_csWaitPic.GetWidth();
		rDraw.bottom= rDraw.top + m_csWaitPic.GetHeight();
		m_csWaitPic.Show(dc, rDraw);
	}
}