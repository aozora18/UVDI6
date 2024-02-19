
/*
 desc : Monitoring - Luria Service - Machine Config
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgLuMC.h"


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
CDlgLuMC::CDlgLuMC(UINT32 id, CWnd *parent)
	: CDlgLuria(id, parent)
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgLuMC::~CDlgLuMC()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgLuMC::DoDataExchange(CDataExchange* dx)
{
	CDlgLuria::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Group box */
	u32StartID	= IDC_LUMC_GRP_ALL_PH;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Static */
	u32StartID	= IDC_LUMC_TXT_PH_HEADS;
	for (i=0; i<18; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Edit box */
	u32StartID	= IDC_LUMC_EDT_PH_HEADS;
	for (i=0; i<19; i++)	DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
	/* Combo box */
	u32StartID	= IDC_LUMC_CMB_PH_NO;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_cmb_ctl[i]);
	/* Check box */
	u32StartID	= IDC_LUMC_CHK_PH_ROTATE;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgLuMC, CDlgLuria)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgLuMC::PreTranslateMessage(MSG* msg)
{

	return CDlgLuria::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLuMC::OnInitDlg()
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
VOID CDlgLuMC::OnExitDlg()
{
	CDlgLuria::OnExitDlg();

}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgLuMC::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgLuMC::OnResizeDlg()
{
}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgLuMC::InitCtrl()
{
	UINT8 i	= 0x00;

	/* Groupbox */
	for (i=0; i<4; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* Combobox */
	for (i=0; i<2; i++)
	{
		m_cmb_ctl[i].SetLogFont(&g_lf);
	}
	/* text - normal */
	for (i=0; i<18; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* Edit box */
	for (i=0; i<19; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_ctl[i].SetInputType(EN_DITM::en_string);
		m_edt_ctl[i].SetReadOnly(TRUE);
		m_edt_ctl[i].SetMouseClick(FALSE);
	}
	/* check box - normal */
	for (i=0; i<2; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf);
		m_chk_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_chk_ctl[i].SetReadOnly(TRUE);
	}

	/* 컨트롤 초기화 */
	m_cmb_ctl[0].SetCurSel(0);
	m_cmb_ctl[1].SetCurSel(0);
	/* Photohead Count */
	for (i=0; i<uvEng_Conf_GetLuria()->ph_count; i++)
	{
		m_cmb_ctl[0].AddNumber(UINT64(i+1));
	}
	if (m_cmb_ctl[0].GetCount() > 0)	m_cmb_ctl[0].SetCurSel(0);
}

/*
 desc : 현재 윈도 컨트롤 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgLuMC::UpdateCtrl()
{
	UINT8 u8Sel		= 0;
	TCHAR tzVal[64]	= {NULL};
	LPG_LDMC pstMC	= &uvEng_ShMem_GetLuria()->machine;	/* Luria Shared Memory */

	/* 값이 유효하지 여부 */
	if (pstMC->total_ph < 1)	return;

	/* All Photohead */
	m_edt_ctl[0].SetTextToNum(pstMC->total_ph, TRUE);
	m_edt_ctl[1].SetTextToNum(pstMC->ph_pitch, TRUE);
	switch (ENG_LPPI(pstMC->product_id))
	{
	case ENG_LPPI::en_lls_2500  :	m_edt_ctl[2].SetTextToStr(L"LLS2500", TRUE);	break;
	case ENG_LPPI::en_lls_04	  :	m_edt_ctl[2].SetTextToStr(L"LLS04", TRUE);		break;
	case ENG_LPPI::en_lls_10	  :	m_edt_ctl[2].SetTextToStr(L"LLS10", TRUE);		break;
	case ENG_LPPI::en_lls_15	  :	m_edt_ctl[2].SetTextToStr(L"LLS15", TRUE);		break;
	case ENG_LPPI::en_lls_25	  :	m_edt_ctl[2].SetTextToStr(L"LLS25", TRUE);		break;
	case ENG_LPPI::en_lls_30	  :	m_edt_ctl[2].SetTextToStr(L"LLS30", TRUE);		break;
	case ENG_LPPI::en_lls_50	  :	m_edt_ctl[2].SetTextToStr(L"LLS50", TRUE);		break;
	}
	swprintf_s(tzVal, 64, L"%u Hz", pstMC->scroll_rate);
	m_edt_ctl[3].SetTextToStr(tzVal, TRUE);
	m_chk_ctl[0].SetCheck(pstMC->ph_rotate);

	/* Specific Photohead */
	u8Sel	= (UINT8)m_cmb_ctl[0].GetCurSel();
	swprintf_s(tzVal, 64, L"%u.%u.%u.%u", pstMC->ph_ipv4[u8Sel][0], pstMC->ph_ipv4[u8Sel][1],
										  pstMC->ph_ipv4[u8Sel][2], pstMC->ph_ipv4[u8Sel][3]);
	m_edt_ctl[4].SetTextToStr(tzVal, TRUE);
	swprintf_s(tzVal, 64, L"%.4f|%.4f mm", pstMC->ph_offset_xy[u8Sel].pos_offset_x / 1000000.0f,
											 pstMC->ph_offset_xy[u8Sel].pos_offset_y / 1000000.0f);
	m_edt_ctl[5].SetTextToStr(tzVal, TRUE);

	/* Motion Controller */
	swprintf_s(tzVal, 64, L"%u.%u.%u.%u",pstMC->motion_control_ipv4[0], pstMC->motion_control_ipv4[1],
										 pstMC->motion_control_ipv4[2], pstMC->motion_control_ipv4[3]);
	m_edt_ctl[6].SetTextToStr(tzVal, TRUE);
	swprintf_s(tzVal, 64, L"%.3f mm",	pstMC->y_acceleration_distance / 1000.0f);
	m_edt_ctl[7].SetTextToStr(tzVal, TRUE);
	swprintf_s(tzVal, 64, L"%.3f|%.3f mm",	pstMC->x_motion_speed / 1000.0f,
											pstMC->max_y_motion_speed / 1000.0f);
	m_edt_ctl[8].SetTextToStr(tzVal, TRUE);
	swprintf_s(tzVal, 64, L"%u/%u",		pstMC->xy_drive_id[0][0], pstMC->xy_drive_id[1][0]);
	m_edt_ctl[9].SetTextToStr(tzVal, TRUE);
	swprintf_s(tzVal, 64, L"%u/%u",		pstMC->xy_drive_id[0][1], pstMC->xy_drive_id[1][1]);
	m_edt_ctl[10].SetTextToStr(tzVal, TRUE);
	m_edt_ctl[11].SetTextToNum(pstMC->hysteresis_type1.scroll_mode, TRUE);
	swprintf_s(tzVal, 64, L"%u px",		pstMC->hysteresis_type1.negative_offset);
	m_edt_ctl[12].SetTextToStr(tzVal, TRUE);
	swprintf_s(tzVal, 64, L"%u|%u nm/s",pstMC->hysteresis_type1.delay_positive,
										pstMC->hysteresis_type1.delay_negative);
	m_edt_ctl[13].SetTextToStr(tzVal, TRUE);

	/* Table Setting */
	u8Sel	= (UINT8)m_cmb_ctl[1].GetCurSel();
	swprintf_s(tzVal, 64, L"%.3f %%", pstMC->parallelogram_adjust[u8Sel] / 1000.0f);
	m_edt_ctl[14].SetTextToStr(tzVal, TRUE);
	swprintf_s(tzVal, 64, L"%.3f|%.3f mm",	pstMC->table_expo_start_xy[u8Sel].x / 1000.0f,
											pstMC->table_expo_start_xy[u8Sel].y / 1000.0f);
	m_edt_ctl[15].SetTextToStr(tzVal, TRUE);
	swprintf_s(tzVal, 64, L"%.3f|%.3f mm",	pstMC->get_table_motion_start_xy[u8Sel].x / 1000.0f,
											pstMC->get_table_motion_start_xy[u8Sel].y / 1000.0f);
	m_edt_ctl[16].SetTextToStr(tzVal, TRUE);
	swprintf_s(tzVal, 64, L"%.3f|%.3f mm",	pstMC->table_position_limit[u8Sel].min_xy[0] / 1000.0f,
											pstMC->table_position_limit[u8Sel].min_xy[1] / 1000.0f);
	m_edt_ctl[17].SetTextToStr(tzVal, TRUE);
	swprintf_s(tzVal, 64, L"%.3f|%.3f mm",	pstMC->table_position_limit[u8Sel].max_xy[0] / 1000.0f,
											pstMC->table_position_limit[u8Sel].max_xy[1] / 1000.0f);
	m_edt_ctl[18].SetTextToStr(tzVal, TRUE);
	m_chk_ctl[1].SetCheck(pstMC->table_print_direction[u8Sel]);
}
