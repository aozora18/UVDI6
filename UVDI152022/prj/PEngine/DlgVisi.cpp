
/*
 desc : Monitoring - Vision
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgVisi.h"


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
CDlgVisi::CDlgVisi(UINT32 id, CWnd *parent)
	: CDlgChild(id, parent)
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgVisi::~CDlgVisi()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgVisi::DoDataExchange(CDataExchange* dx)
{
	CDlgChild::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Group box */
	u32StartID	= IDC_VISI_GRP_ACAM_NO;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Static */
	u32StartID	= IDC_VISI_TXT_MOD_IMAGE_INDEX;
	for (i=0; i<24; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Edit box */
	u32StartID	= IDC_VISI_EDT_MOD_MARK_METHOD;
	for (i=0; i<21; i++)	DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
	/* Combo box */
	u32StartID	= IDC_VISI_CMB_ACAM_NO;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_cmb_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgVisi, CDlgChild)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgVisi::PreTranslateMessage(MSG* msg)
{

	return CDlgChild::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgVisi::OnInitDlg()
{
	if (!CDlgChild::OnInitDlg())	return FALSE;

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
VOID CDlgVisi::OnExitDlg()
{
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgVisi::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgVisi::OnResizeDlg()
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : TRUE or FALSE
*/
VOID CDlgVisi::InitCtrl()
{
	INT32 i;

	/* Groupbox */
	for (i=0; i<4; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* Combobox */
	for (i=0; i<4; i++)
	{
		m_cmb_ctl[i].SetLogFont(&g_lf);
	}
	/* text - normal */
	for (i=0; i<24; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* Edit box */
	for (i=0; i<21; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_ctl[i].SetInputType(EN_DITM::en_string);
		m_edt_ctl[i].SetReadOnly(TRUE);
		m_edt_ctl[i].SetMouseClick(FALSE);
	}

	/* 콤보 박스 값 초기화 */
	for (i=0; i<uvEng_GetConfig()->set_cams.acam_count; i++)
		m_cmb_ctl[0].AddNumber(UINT64(i+1));
	if (i > 0)	m_cmb_ctl[0].SetCurSel(0);
	for (i=0; i<uvEng_GetConfig()->mark_find.max_find_count; i++)
		m_cmb_ctl[1].AddNumber(UINT64(i+1));
	if (i > 0)	m_cmb_ctl[1].SetCurSel(0);
	for (i=0; i<(INT32)uvEng_GetConfig()->edge_find.max_find_count; i++)
		m_cmb_ctl[2].AddNumber(UINT64(i+1));
	if (i > 0)	m_cmb_ctl[2].SetCurSel(0);
	for (i=0; i<uvEng_GetConfig()->line_find.max_find_count; i++)
		m_cmb_ctl[3].AddNumber(UINT64(i+1));
	if (i > 0)	m_cmb_ctl[3].SetCurSel(0);
}

/*
 desc : 현재 윈도 컨트롤 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgVisi::UpdateCtrl()
{
	UpdateMoni();
}

/*
 desc : Memory Data (Value) 갱신
 parm : None
 retn : None
*/
VOID CDlgVisi::UpdateMoni()
{
	TCHAR tzVals[64]= {NULL};
	INT8 i8ACamNo	= m_cmb_ctl[0].GetCurSel();
	INT8 i8Index	= 0;
	if (i8ACamNo < 0)	return;
	LPG_GMFR pstMod	= NULL;
	LPG_EDFR pstEdge= NULL;
	LPG_ELFR pstLine= NULL;

	/* Model Search Results */
	i8Index	= m_cmb_ctl[1].GetCurSel();
	if (i8Index >= 0)
	{
		pstMod	= &uvEng_ShMem_GetVisi()->mark_result[i8ACamNo][i8Index];
		if (pstMod)
		{
			/* Mark Method */
			switch (pstMod->mark_method)
			{
			case ENG_MMSM::en_single		:	m_edt_ctl[0].SetTextToStr(L"Single", TRUE);			break;
			case ENG_MMSM::en_cent_side		:	m_edt_ctl[0].SetTextToStr(L"Center-Side", TRUE);	break;
			case ENG_MMSM::en_multi_only	:	m_edt_ctl[0].SetTextToStr(L"Multi-Only", TRUE);		break;
			case ENG_MMSM::en_ring_circle	:	m_edt_ctl[0].SetTextToStr(L"Ring-Side", TRUE);		break;
			case ENG_MMSM::en_none			:	m_edt_ctl[0].SetTextToStr(L"Unknown", TRUE);		break;
			}
			/* Mark Width / Height */
			swprintf_s(tzVals, 64, L"%u px", pstMod->mark_width);
			m_edt_ctl[1].SetTextToStr(tzVals, TRUE);
			swprintf_s(tzVals, 64, L"%u px", pstMod->mark_height);
			m_edt_ctl[2].SetTextToStr(tzVals, TRUE);
			/* Scale & Score Rate */
			swprintf_s(tzVals, 64, L"%.2f %%", pstMod->scale_rate);
			m_edt_ctl[3].SetTextToStr(tzVals);
			swprintf_s(tzVals, 64, L"%.2f %%", pstMod->score_rate);
			m_edt_ctl[4].SetTextToStr(tzVals);
			/* Center Distance */
			swprintf_s(tzVals, 64, L"%.2f px", pstMod->cent_dist);
			m_edt_ctl[5].SetTextToStr(tzVals);
			/* Square Rate */
			swprintf_s(tzVals, 64, L"%.2f %%", pstMod->square_rate);
			m_edt_ctl[6].SetTextToStr(tzVals);
		}
	}

	/* Edge Search Results */
	i8Index	= m_cmb_ctl[2].GetCurSel();
	if (i8Index >= 0)
	{
		pstEdge	= &uvEng_ShMem_GetVisi()->edge_result[i8ACamNo][i8Index];
		if (pstEdge)
		{
			/* Diameter (Pixel / Micro-meter) */
			swprintf_s(tzVals, 64, L"%.2f px", pstEdge->dia_meter_px);
			m_edt_ctl[7].SetTextToStr(tzVals, TRUE);
			swprintf_s(tzVals, 64, L"%.3f um", pstEdge->dia_meter_um);
			m_edt_ctl[8].SetTextToStr(tzVals, TRUE);
			/* Width / Height Size (um) */
			swprintf_s(tzVals, 64, L"%.3f um", pstEdge->width_um);
			m_edt_ctl[9].SetTextToStr(tzVals, TRUE);
			swprintf_s(tzVals, 64, L"%.3f um", pstEdge->height_um);
			m_edt_ctl[10].SetTextToStr(tzVals, TRUE);
			/* Angle */
			swprintf_s(tzVals, 64, L"%.3f °", pstEdge->angle);
			m_edt_ctl[11].SetTextToStr(tzVals, TRUE);
			/* Strength */
			swprintf_s(tzVals, 64, L"%.3f", pstEdge->strength);
			m_edt_ctl[12].SetTextToStr(tzVals, TRUE);
			/* Length */
			swprintf_s(tzVals, 64, L"%.3f", pstEdge->length);
			m_edt_ctl[13].SetTextToStr(tzVals, TRUE);
		}
	}

	/* Line Search Results */
	i8Index	= m_cmb_ctl[3].GetCurSel();
	if (i8Index >= 0)
	{
		pstLine	= &uvEng_ShMem_GetVisi()->line_result[i8ACamNo][i8Index];
		if (pstLine)
		{
			/* Angle */
			swprintf_s(tzVals, 64, L"%.2f °", pstLine->angle /100.0f);
			m_edt_ctl[14].SetTextToStr(tzVals, TRUE);
			/* Strength */
			swprintf_s(tzVals, 64, L"%.2f", pstLine->strength/100.0f);
			m_edt_ctl[15].SetTextToStr(tzVals, TRUE);
			/* Contrast */
			swprintf_s(tzVals, 64, L"%.1f", pstLine->contrast/10.0f);
			m_edt_ctl[16].SetTextToStr(tzVals, TRUE);
			/* Score */
			swprintf_s(tzVals, 64, L"%.3f %%", pstLine->percent_score/1000.0f);
			m_edt_ctl[17].SetTextToStr(tzVals, TRUE);
			/* Center Distance */
			swprintf_s(tzVals, 64, L"%.3f um", pstLine->cent_diff_um);
			m_edt_ctl[18].SetTextToStr(tzVals, TRUE);
			/* Mark Position XY */
			swprintf_s(tzVals, 64, L"%.3f px", pstLine->position[0] / 1000.0f);
			m_edt_ctl[19].SetTextToStr(tzVals, TRUE);
			swprintf_s(tzVals, 64, L"%.3f px", pstLine->position[1] / 1000.0f);
			m_edt_ctl[20].SetTextToStr(tzVals, TRUE);
		}
	}
}