
/*
 desc : Frame Rate 선택 대화 상자
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgStep.h"

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
CDlgStep::CDlgStep(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgStep::IDD, parent)
{
	m_bDrawBG		= TRUE;
	m_bTooltip		= TRUE;

	m_u8Offset		= 0xff;
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgStep::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Button - Normal */
	u32StartID	= IDC_STEP_BTN_APPLY;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Checkbox - Normal */
	u32StartID	= IDC_STEP_CHK_OFFSET_0;
	for (i=0; i<7; i++)		DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgStep, CMyDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_STEP_CHK_OFFSET_0, IDC_STEP_CHK_OFFSET_NONE, OnChkClicked)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_STEP_BTN_APPLY, IDC_STEP_BTN_CANCEL, OnBtnClicked)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgStep::PreTranslateMessage(MSG* msg)
{

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgStep::OnInitDlg()
{
	/* 컨트롤 초기화 */
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
VOID CDlgStep::OnExitDlg()
{
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgStep::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgStep::OnResizeDlg()
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgStep::InitCtrl()
{
	INT32 i;
	PTCHAR pText	= NULL;

	/* button - normal */
	for (i=0; i<2; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);
	}
	/* checkbox - normal */
	for (i=0; i<7; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_chk_ctl[i].SetBgColor(RGB(255, 255, 255));
	}
	m_chk_ctl[6].SetCheck(1);

	return TRUE;
}


/*
 desc : 현재 발생된 에러 정보 출력
 parm : None
 retn : None
*/
VOID CDlgStep::OnBtnClicked(UINT32 id)
{
	if (id == IDC_STEP_BTN_APPLY)
	{
		CMyDialog::EndDialog(IDOK);
	}
	else
	{
		CMyDialog::EndDialog(IDCANCEL);
	}
}

/*
 desc : 현재 발생된 에러 정보 출력
 parm : None
 retn : None
*/
VOID CDlgStep::OnChkClicked(UINT32 id)
{
	for (UINT8 i=0; i<7; i++)	m_chk_ctl[i].SetCheck(0);
	
	m_chk_ctl[id - IDC_STEP_CHK_OFFSET_0].SetCheck(1);
	m_u8Offset	= UINT8(id - IDC_STEP_CHK_OFFSET_0);

	if (m_u8Offset == 0x06)	m_u8Offset	= 0xff;
}

/*
 desc : DoModal Override 함수
 parm : None
 retn : 0L
*/
INT_PTR CDlgStep::MyDoModal()
{
	return DoModal();
}
