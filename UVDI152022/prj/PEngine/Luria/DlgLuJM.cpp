
/*
 desc : Monitoring - Luria Service - Job Management
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgLuJM.h"

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
CDlgLuJM::CDlgLuJM(UINT32 id, CWnd *parent)
	: CDlgLuria(id, parent)
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgLuJM::~CDlgLuJM()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgLuJM::DoDataExchange(CDataExchange* dx)
{
	CDlgLuria::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Group box */
	u32StartID	= IDC_LUJM_GRP_JOBS;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Static */
	u32StartID	= IDC_LUJM_TXT_COMPLEXITY;
	for (i=0; i<14; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Edit box */
	u32StartID	= IDC_LUJM_EDT_COMPLEXITY;
	for (i=0; i<14; i++)	DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
	/* List box */
	u32StartID	= IDC_LUJM_BOX_JOB_LIST;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_box_ctl[i]);
	/* Check box */
	u32StartID	= IDC_LUJM_CHK_GERBER_INVERTED;
	for (i=0; i<5; i++)		DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
	/* Button */
	u32StartID	= IDC_LUJM_BTN_FIDUCIAL;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgLuJM, CDlgLuria)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_LUJM_BTN_FIDUCIAL, IDC_LUJM_BTN_FIDUCIAL, OnBtnClicked)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgLuJM::PreTranslateMessage(MSG* msg)
{

	return CDlgLuria::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLuJM::OnInitDlg()
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
VOID CDlgLuJM::OnExitDlg()
{
	CDlgLuria::OnExitDlg();

}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgLuJM::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgLuJM::OnResizeDlg()
{
}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgLuJM::InitCtrl()
{
	UINT8 i	= 0x00;

	/* Groupbox */
	for (i=0; i<2; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* text - normal */
	for (i=0; i<14; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* Edit box */
	for (i=0; i<14; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_ctl[i].SetInputType(EN_DITM::en_string);
		m_edt_ctl[i].SetReadOnly(TRUE);
		m_edt_ctl[i].SetMouseClick(FALSE);
	}
	/* List box - normal */
	for (i=0; i<1; i++)
	{
		m_box_ctl[i].SetLogFont(&g_lf);
	}
	/* check box - normal */
	for (i=0; i<5; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf);
		m_chk_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_chk_ctl[i].SetReadOnly(TRUE);
	}
	/* Button - normal */
	for (i=0; i<1; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
}

/*
 desc : 현재 윈도 컨트롤 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgLuJM::UpdateCtrl()
{
	UINT8 i	= 0x00;
	TCHAR tzVal[64]	= {NULL};
	ENG_LGAC enCompl= (ENG_LGAC)uvEng_ShMem_GetLuria()->machine.artwork_complexity;
	LPG_LDJM pstJM	= &uvEng_ShMem_GetLuria()->jobmgt;	/* Luria Shared Memory */
	CUniToChar csCnv;

	/* 값이 유효하지 여부 */
	if (pstJM->job_regist_count < 1)	return;

	/* Jobs */
	switch (enCompl)
	{
	case ENG_LGAC::en_normal	:	m_edt_ctl[0].SetTextToStr(L"Normal",	TRUE);	break;
	case ENG_LGAC::en_high	:	m_edt_ctl[0].SetTextToStr(L"High",		TRUE);	break;
	case ENG_LGAC::en_extreme:	m_edt_ctl[0].SetTextToStr(L"eXtreme",	TRUE);	break;
	}
	m_edt_ctl[1].SetTextToNum(pstJM->max_job_regist);
	switch(pstJM->selected_job_load_state)
	{
	case ENG_SJLS::en_load_not_started	:	m_edt_ctl[2].SetTextToStr(L"Load not started");	break;
	case ENG_SJLS::en_loading			:	m_edt_ctl[2].SetTextToStr(L"Loading");			break;
	case ENG_SJLS::en_load_complete		:	m_edt_ctl[2].SetTextToStr(L"Load completed");	break;
	case ENG_SJLS::en_load_failed		:	m_edt_ctl[2].SetTextToStr(L"Load failed");		break;
	}
	m_box_ctl[0].SetRedraw(FALSE);
	m_box_ctl[0].ResetContent();
	for (i=0; i<pstJM->job_regist_count; i++)
	{
		m_box_ctl[0].AddString(csCnv.Ansi2Uni(pstJM->job_name_list[i]));
		if (0 ==strcmp(pstJM->job_name_list[i], pstJM->selected_job_name))	m_box_ctl[0].SetCurSel(i);
	}
	m_box_ctl[0].SetRedraw(TRUE);
	m_box_ctl[0].Invalidate(FALSE);

	/* The Information for JOB Selected */
	swprintf_s(tzVal, 64, L"%.3f x", pstJM->get_job_params.lens_magnification/1000.0f);
	m_edt_ctl[3].SetTextToStr(tzVal, TRUE);
	m_edt_ctl[4].SetTextToNum(pstJM->get_job_params.strips_counts, TRUE);
	m_edt_ctl[5].SetTextToNum(pstJM->get_job_params.spx, TRUE);
	swprintf_s(tzVal, 64, L"%.3f mm", pstJM->get_job_params.stripe_overlap/1000.0f);
	m_edt_ctl[6].SetTextToStr(tzVal, TRUE);
	swprintf_s(tzVal, 64, L"%.1f um", pstJM->get_job_params.pixel_size/1000.0f);
	m_edt_ctl[7].SetTextToStr(tzVal, TRUE);
	swprintf_s(tzVal, 64, L"%.3f|%.3f mm", pstJM->get_job_params.artwork_size[0]/1000.0f,
										   pstJM->get_job_params.artwork_size[1]/1000.0f);
	m_edt_ctl[8].SetTextToStr(tzVal, TRUE);
	swprintf_s(tzVal, 64, L"%.3f|%.3f mm", pstJM->get_job_params.erosion[0]/1000.0f,
										   pstJM->get_job_params.erosion[1]/1000.0f);
	m_edt_ctl[9].SetTextToStr(tzVal, TRUE);
	swprintf_s(tzVal, 64, L"%.4f mm", pstJM->get_job_params.strip_width/1000000.0f);
	m_edt_ctl[10].SetTextToStr(tzVal, TRUE);
	swprintf_s(tzVal, 64, L"%.3f|%.3f mm", pstJM->get_job_params.offset[0]/1000.0f,
										   pstJM->get_job_params.offset[1]/1000.0f);
	m_edt_ctl[11].SetTextToStr(tzVal, TRUE);
	swprintf_s(tzVal, 64, L"%u °", pstJM->get_job_params.rotation);
	m_edt_ctl[12].SetTextToStr(tzVal, TRUE);
	swprintf_s(tzVal, 64, L"%.3f|%.3f mm", pstJM->get_job_params.origin_offset[0]/1000.0f,
										   pstJM->get_job_params.origin_offset[1]/1000.0f);
	m_edt_ctl[13].SetTextToStr(tzVal, TRUE);

	/* Check box */
	m_chk_ctl[0].SetCheck(pstJM->get_job_params.gerber_inverted);
	m_chk_ctl[1].SetCheck(pstJM->get_job_params.flip[0]);
	m_chk_ctl[2].SetCheck(pstJM->get_job_params.flip[1]);
	m_chk_ctl[3].SetCheck(pstJM->get_job_params.edge_blend_enabled);
	m_chk_ctl[4].SetCheck(pstJM->get_job_params.extra_long_strips);
}

/*
 desc : 버튼 이벤트
 parm : id	- [in]  버튼 ID
 retn : None
*/
VOID CDlgLuJM::OnBtnClicked(UINT32 id)
{
	CDlgLuJF dlgJF;

	switch (id)
	{
	case IDC_LUJM_BTN_FIDUCIAL	:	dlgJF.DoModal();	break;
	}
}