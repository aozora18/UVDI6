
/*
 desc : Input the value
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgIVal.h"

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
CDlgIVal::CDlgIVal(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgIVal::IDD, parent)
{
	m_bDrawBG	= TRUE;
	m_bTooltip	= TRUE;
	m_clrBg		= RGB(255, 255, 255);
	m_u8Type	= 0x00;

	m_i32Item	= -1;
	m_i32Val	= 0;
	m_dbVal		= 0.0f;
	m_bKeyPushEnter	= TRUE;
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgIVal::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Groupbox - Normal */
	u32StartID	= IDC_IVAL_GRP_VALUE;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Button - Normal */
	u32StartID	= IDC_IVAL_BTN_APPLY;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Edit - Normal */
	u32StartID	= IDC_IVAL_EDT_VALUE;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgIVal, CMyDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_IVAL_BTN_APPLY, IDC_IVAL_BTN_CANCEL, OnBtnClicked)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgIVal::PreTranslateMessage(MSG* msg)
{

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgIVal::OnInitDlg()
{
	TCHAR tzVal[32]	= {NULL};
	/* 컨트롤 초기화 */
	InitCtrl();
	SetValue();

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgIVal::OnExitDlg()
{
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgIVal::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgIVal::OnResizeDlg()
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgIVal::InitCtrl()
{
	INT32 i, i32Min[5] = {1, 0, 0, 0, 1};
	PTCHAR pText= NULL;
	LOGFONT lf	= g_lf;

	/* Normal - Group box */
	for (i=0; i<1; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	switch (m_u8Type)
	{
	case 0x00 : m_grp_ctl[0].SetText(L"Enter an integer value");	break;
	case 0x01 : m_grp_ctl[0].SetText(L"Enter an real value");		break;
	}
	/* button - normal */
	for (i=0; i<2; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}
	/* edit - Normal Value */
	lf.lfHeight	*= 2;
	lf.lfWeight	+= 10;
	m_edt_ctl[0].SetEditFont(&lf);
	m_edt_ctl[0].SetBackColor(RGB(0xff, 0xff, 0xff));
	m_edt_ctl[0].SetTextColor(RGB(0x00, 0x00, 0x00));
	switch (m_u8Type)
	{
	case 0x00 : m_edt_ctl[0].SetInputType(ENM_DITM::en_int64);	break;
	case 0x01 : m_edt_ctl[0].SetInputType(ENM_DITM::en_double);	break;
	}
}

/*
 desc : 현재 발생된 에러 정보 출력
 parm : None
 retn : None
*/
VOID CDlgIVal::OnBtnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_IVAL_BTN_APPLY		:	ApplyValue();	break;
	case IDC_IVAL_BTN_CANCEL	:	CancelValue();	break;
	}
}

/*
 desc : DoModal Override
 parm : type	- [in]  0x00:Integer,0x01:double
		item	- [in]  The item index of grid control
		value	- [in]  Current Value
 retn : TRUE or FALSE
*/
BOOL CDlgIVal::Create(UINT8 type, INT32 item, INT32 value)
{
	m_u8Type	= type;
	m_i32Item	= item;
	m_i32Val	= value;
	return CMyDialog::Create();
}

/*
 desc : Updates the value
 parm : None
 retn : None
*/
VOID CDlgIVal::ApplyValue()
{
	switch (m_u8Type)
	{
	case 0x00 : m_i32Val= (INT32)m_edt_ctl[0].GetTextToNum();		break;
	case 0x01 : m_dbVal	= (DOUBLE)m_edt_ctl[0].GetTextToDouble();	break;
	}
	if (m_pParentWnd)	m_pParentWnd->PostMessage(WM_MSG_DIALOG_IVAL, 1, 0L);

	/* move the focus back to the input position */
	m_edt_ctl[0].SetFocus();
	m_edt_ctl[0].SelectedAll();
}

/*
 desc : Cancel
 parm : None
 retn : None
*/
VOID CDlgIVal::CancelValue()
{
	if (m_pParentWnd)	m_pParentWnd->PostMessage(WM_MSG_DIALOG_IVAL, 0, 0L);
}

/*
 desc : Set the parameter
 parm : type	- [in]  0x00:Integer,0x01:double
		item	- [in]  The item index of grid control
		value	- [in]  Current Value
 retn : None
*/
VOID CDlgIVal::SetValue(UINT8 type, INT32 item, INT32 value)
{
	m_u8Type	= type;
	m_i32Item	= item;
	m_i32Val	= value;
	if (0x01 == m_u8Type)	m_dbVal	= value / 10000.0f;
	SetValue();
}

/*
 desc : Updates the values of control on dialog
 parm : None
 retn : None
*/
VOID CDlgIVal::SetValue()
{
	TCHAR tzVal[32]	= {NULL};
	/* Set the current value */
	if (m_u8Type == 0x00)	swprintf_s(tzVal, 32, L"%d", m_i32Val);
	else					swprintf_s(tzVal, 32, L"%.4f", m_dbVal);
	m_edt_ctl[0].SetWindowTextW(tzVal);
	m_edt_ctl[0].SelectedAll();
}