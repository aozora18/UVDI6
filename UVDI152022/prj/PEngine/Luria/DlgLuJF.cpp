
/*
 desc : The fiducial information for global and local dcode
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgLuJF.h"


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
CDlgLuJF::CDlgLuJF(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgLuJF::IDD, parent)
{
	m_bDrawBG		= TRUE;
	m_bTooltip		= TRUE;
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgLuJF::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Groupbox */
	u32StartID	= IDC_LUJF_GRP_GLOBAL;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Static */
	u32StartID	= IDC_LUJF_TXT_GLOBAL_DCODE;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Edit box */
	u32StartID	= IDC_LUJF_EDT_GLOBAL_DCODE;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
	/* List box */
	u32StartID	= IDC_LUJF_BOX_GLOBAL;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_box_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgLuJF, CMyDialog)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgLuJF::PreTranslateMessage(MSG* msg)
{

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLuJF::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();
	LoadValue();

	/* TOP_MOST & Center */
	//CenterParentTopMost();

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgLuJF::OnExitDlg()
{
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgLuJF::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgLuJF::OnResizeDlg()
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgLuJF::InitCtrl()
{
	INT32 i;

	/* Groupbox */
	for (i=0; i<2; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* text - normal */
	for (i=0; i<2; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* edit box */
	for (i=0; i<2; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_ctl[i].SetInputType(EN_DITM::en_hex10);
		m_edt_ctl[i].SetReadOnly(TRUE);
		m_edt_ctl[i].SetMouseClick(FALSE);
	}
	/* List box - normal */
	for (i=0; i<2; i++)
	{
		m_box_ctl[i].SetLogFont(&g_lf);
	}
}

/*
 desc : Fiducial 정보 적재
 parm : None
 retn : None
*/
VOID CDlgLuJF::LoadValue()
{
	UINT16 i			= 0;
	TCHAR tzNum[2][16]	= {NULL}, tzVal[2][16] = {NULL}, tzStr[64] = {NULL};
	LPG_JMFD pstFD		= &uvEng_ShMem_GetLuria()->jobmgt.selected_job_fiducial;

	/* Global DCode */
	m_edt_ctl[0].SetTextToNum(pstFD->g_d_code, TRUE);
	for (i=0; i<pstFD->g_coord_count; i++)
	{
		/* mm 단위로 변환 후 문자열로 저장 */
		swprintf_s(tzNum[0], 16, L"X : %9.6f", pstFD->g_coord_xy[i].x / 1000000.0f);
		swprintf_s(tzNum[1], 16, L"Y : %9.6f", pstFD->g_coord_xy[i].y / 1000000.0f);
		/* 동일한 크기로 변환 */
		wmemcpy_s(tzVal[0], 16, tzNum[0], 12);
		wmemcpy_s(tzVal[1], 16, tzNum[1], 12);
		/* 하나의 결과로 병합 */
		swprintf_s(tzStr, 64, L"%s   |   %s", tzVal[0], tzVal[1]);
		m_box_ctl[0].AddString(tzStr);
	}
	/* Local DCode */
	m_edt_ctl[1].SetTextToNum(pstFD->l_d_code, TRUE);
	for (i=0; i<pstFD->l_coord_count; i++)
	{
		/* mm 단위로 변환 후 문자열로 저장 */
		swprintf_s(tzNum[0], 16, L"X : %9.6f", pstFD->l_coord_xy[i].x / 1000000.0f);
		swprintf_s(tzNum[0], 16, L"Y : %9.6f", pstFD->l_coord_xy[i].y / 1000000.0f);
		/* 동일한 크기로 변환 */
		wmemcpy_s(tzVal[0], 16, tzNum[0], 12);
		wmemcpy_s(tzVal[1], 16, tzNum[1], 12);
		/* 하나의 결과로 병합 */
		swprintf_s(tzStr, 64, L"%s  |  %s", tzVal[0], tzVal[1]);
		m_box_ctl[1].AddString(tzStr);
	}
}
