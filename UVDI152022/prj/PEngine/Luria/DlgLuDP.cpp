
/*
 desc : Monitoring - Luria Service - Direct Photohead
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgLuDP.h"


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
CDlgLuDP::CDlgLuDP(UINT32 id, CWnd *parent)
	: CDlgLuria(id, parent)
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgLuDP::~CDlgLuDP()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgLuDP::DoDataExchange(CDataExchange* dx)
{
	CDlgLuria::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Group box */
	u32StartID	= IDC_LUDP_GRP_INFO;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Static */
	u32StartID	= IDC_LUDP_TXT_LED_LEVEL;
	for (i=0; i<13; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Edit box */
	u32StartID	= IDC_LUDP_EDT_LED_LEVEL;
	for (i=0; i<22; i++)	DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
	/* Combo box */
	u32StartID	= IDC_LUDP_CMB_PH_NO;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_cmb_ctl[i]);
	/* Check box */
	u32StartID	= IDC_LUDP_CHK_BUSY_MOVING;
	for (i=0; i<12; i++)	DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgLuDP, CDlgLuria)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgLuDP::PreTranslateMessage(MSG* msg)
{

	return CDlgLuria::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLuDP::OnInitDlg()
{
	if (!CDlgLuria::OnInitDlg())	return FALSE;

	/* 컨트롤 초기화 */
	InitCtrl();

	/* 상위 객체 호출 */
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgLuDP::OnExitDlg()
{
	CDlgLuria::OnExitDlg();

}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgLuDP::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgLuDP::OnResizeDlg()
{
}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgLuDP::InitCtrl()
{
	UINT8 i	= 0x00;

	/* Groupbox */
	for (i=0; i<3; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* text - normal */
	for (i=0; i<13; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* Edit box */
	for (i=0; i<22; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_ctl[i].SetInputType(EN_DITM::en_string);
		m_edt_ctl[i].SetReadOnly(TRUE);
		m_edt_ctl[i].SetMouseClick(FALSE);
	}
	/* Combo box - normal */
	for (i=0; i<1; i++)
	{
		m_cmb_ctl[i].SetLogFont(&g_lf);
	}
	/* check box - normal */
	for (i=0; i<12; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf);
		m_chk_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_chk_ctl[i].SetReadOnly(TRUE);
	}
	/* Combo Box 설정 */
	m_cmb_ctl[0].ResetContent();
	for (i=0; i<uvEng_Conf_GetLuria()->ph_count; i++)
	{
		m_cmb_ctl[0].AddNumber(UINT64(i+1));
	}
	if (i > 0)	m_cmb_ctl[0].SetCurSel(0);
}

/*
 desc : 현재 윈도 컨트롤 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgLuDP::UpdateCtrl()
{
	UINT8 i	= 0x00;
	INT8 i8Sel		= (INT8)m_cmb_ctl[0].GetCurSel();
	UINT16 u16Status= 0x0000;
	TCHAR tzVal[64]	= {NULL};
	LPG_LDSS pstSS	= &uvEng_ShMem_GetLuria()->system;		/* Luria Shared Memory */
	LPG_LDDP pstDP	= &uvEng_ShMem_GetLuria()->directph;	/* Luria Shared Memory */

	/* 현재 등록된 포토헤드 개수가 없는지 확인 */
	if (i8Sel < 0)	return;

	/* Update Motor status */
	u16Status	= pstDP->focus_motor_status[i8Sel];
	m_chk_ctl[0].SetCheck((u16Status & 0x0001) > 0 ? 1 : 0);
	m_chk_ctl[1].SetCheck((u16Status & 0x0002) > 0 ? 1 : 0);
	m_chk_ctl[2].SetCheck((u16Status & 0x0004) > 0 ? 1 : 0);
	m_chk_ctl[3].SetCheck((u16Status & 0x0008) > 0 ? 1 : 0);
	m_chk_ctl[4].SetCheck((u16Status & 0x0010) > 0 ? 1 : 0);
	m_chk_ctl[5].SetCheck((u16Status & 0x0020) > 0 ? 1 : 0);
	m_chk_ctl[6].SetCheck((u16Status & 0x0040) > 0 ? 1 : 0);
	m_chk_ctl[7].SetCheck((u16Status & 0x0080) > 0 ? 1 : 0);
	m_chk_ctl[8].SetCheck((u16Status & 0x0100) > 0 ? 1 : 0);
	m_chk_ctl[9].SetCheck((u16Status & 0x0200) > 0 ? 1 : 0);
	m_chk_ctl[10].SetCheck((u16Status& 0x0400) > 0 ? 1 : 0);
	m_chk_ctl[11].SetCheck((u16Status& 0x0800) > 0 ? 1 : 0);

	m_edt_ctl[0].SetTextToNum(pstDP->camera_illumination_led[i8Sel], TRUE);
	swprintf_s(tzVal, 64, L"%.3f mm", pstDP->focus_motor_move_abs_position[i8Sel] / 1000.0f);
	m_edt_ctl[1].SetTextToStr(tzVal, TRUE);
	m_edt_ctl[2].SetTextToStr(pstDP->light_source_driver_light_onoff[i8Sel][0] ? L"Yes" : L"No", TRUE);
	m_edt_ctl[3].SetTextToStr(pstDP->light_source_driver_light_onoff[i8Sel][1] ? L"Yes" : L"No", TRUE);
	m_edt_ctl[4].SetTextToStr(pstDP->light_source_driver_light_onoff[i8Sel][2] ? L"Yes" : L"No", TRUE);
	m_edt_ctl[5].SetTextToStr(pstDP->light_source_driver_light_onoff[i8Sel][3] ? L"Yes" : L"No", TRUE);
	m_edt_ctl[6].SetTextToStr(pstDP->camera_led_power_onoff[i8Sel] ? L"On" : L"Off", TRUE);
	m_edt_ctl[7].SetTextToStr(pstDP->camera_led_power_onoff[i8Sel] ? L"On" : L"Off", TRUE);
	m_edt_ctl[8].SetTextToStr(pstDP->camera_led_power_onoff[i8Sel] ? L"On" : L"Off", TRUE);
	m_edt_ctl[9].SetTextToStr(pstDP->camera_led_power_onoff[i8Sel] ? L"On" : L"Off", TRUE);
	m_edt_ctl[10].SetTextToNum(pstDP->light_source_driver_temp_led[i8Sel][0]/10.0f, 1, TRUE);
	m_edt_ctl[11].SetTextToNum(pstDP->light_source_driver_temp_led[i8Sel][1]/10.0f, 1, TRUE);
	m_edt_ctl[12].SetTextToNum(pstDP->light_source_driver_temp_led[i8Sel][2]/10.0f, 1, TRUE);
	m_edt_ctl[13].SetTextToNum(pstDP->light_source_driver_temp_led[i8Sel][3]/10.0f, 1, TRUE);
	m_edt_ctl[14].SetTextToNum(pstDP->light_source_driver_temp_board[i8Sel][0]/10.0f, 1, TRUE);
	m_edt_ctl[15].SetTextToNum(pstDP->light_source_driver_temp_board[i8Sel][1]/10.0f, 1, TRUE);
	m_edt_ctl[16].SetTextToNum(pstDP->light_source_driver_temp_board[i8Sel][2]/10.0f, 1, TRUE);
	m_edt_ctl[17].SetTextToNum(pstDP->light_source_driver_temp_board[i8Sel][3]/10.0f, 1, TRUE);
	m_edt_ctl[18].SetTextToNum(UINT32(pstDP->light_source_on_time_counter[i8Sel][0]/(60 * 60)), TRUE);
	m_edt_ctl[19].SetTextToNum(UINT32(pstDP->light_source_on_time_counter[i8Sel][1]/(60 * 60)), TRUE);
	m_edt_ctl[20].SetTextToNum(UINT32(pstDP->light_source_on_time_counter[i8Sel][2]/(60 * 60)), TRUE);
	m_edt_ctl[21].SetTextToNum(UINT32(pstDP->light_source_on_time_counter[i8Sel][3]/(60 * 60)), TRUE);
}

