
/*
 desc : Gerber Recipe
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgGerb.h"

#include "./gerb/GridGerb.h"	/* Grid Control */
#include "./gerb/DrawType.h"	/* Align Mark Array Type */
#include "./gerb/DrawPrev.h"	/* Gerber Preivew File */

#include "../pops/DlgSped.h"
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
CDlgGerb::CDlgGerb(UINT32 id, CWnd* parent)
	: CDlgMenu(id, parent)
{
	/* 자신의 다이얼로그 ID 설정 */
	m_enDlgID	= ENG_CMDI::en_menu_gerb;
}

CDlgGerb::~CDlgGerb()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgGerb::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* groupbox - normal */
	u32StartID	= IDC_GERB_GRP_RECIPE_LIST;
	for (i=0; i<eGERB_GRP_MAX; i++)				DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* static */
	u32StartID	= IDC_GERB_TXT_SEARCH_SCORE;
	for (i=0; i<eGERB_TXT_MAX; i++)				DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* edit - normal */
	u32StartID	= IDC_GERB_EDT_GERBER_FILE;
	for (i=0; i< eGERB_EDT_CTL_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
	/* edit - normal */
	u32StartID	= IDC_GERB_EDT_EXPO_SPEED;
	for (i=0; i< eGERB_EDT_SPD_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_edt_spd[i]);
	/* edit - text */
	u32StartID	= IDC_GERB_EDT_RECIPE_NAME;
	for (i=0; i< eGERB_EDT_TXT_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_edt_txt[i]);
	/* edit - integer */
	u32StartID	= IDC_GERB_EDT_EXPO_STEP;
	for (i=0; i< eGERB_EDT_INT_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_edt_int[i]);
	/* edit - float */
	u32StartID	= IDC_GERB_EDT_SEARCH_SCALE;
	for (i=0; i< eGERB_EDT_FLOAT_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_edt_flt[i]);
	/* checkbox - normal */
	u32StartID	= IDC_GERB_CHK_MARK_ON_THE_FLY;
	for (i=0; i< eGERB_CHK_MAX; i++)			DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
	/* button - normal */
	u32StartID	= IDC_GERB_BTN_FILE_SELECT;
	for (i=0; i< eGERB_BTN_MAX; i++)			DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgGerb, CDlgMenu)
	ON_NOTIFY(GVN_COLUMNCLICK, IDC_GRID_GERB_GERBER_LIST, OnGridGerbList)
	ON_NOTIFY(GVN_COLUMNCLICK, IDC_GRID_GERB_LED_LIST, OnGridLedList)
	ON_BN_CLICKED(IDC_GERB_EDT_EXPO_SPEED, OnEdtExpoSpeed)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_GERB_BTN_FILE_SELECT, IDC_GERB_BTN_DIST_OFFSET, OnBtnClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_GERB_CHK_MARK_ON_THE_FLY, IDC_GERB_CHK_MARK_TYPE_MULTI, OnChkClick)
END_MESSAGE_MAP()


/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgGerb::PreTranslateMessage(MSG* msg)
{
	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgGerb::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();
	if (!InitGrid())	return FALSE;
	/* 객체 초기화 */
	if (!InitObject())	return FALSE;
	/* 기본 설정 값 */
	SetBaseData();

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgGerb::OnExitDlg()
{
	CloseObject();
	CloseGrid();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgGerb::OnPaintDlg(CDC *dc)
{
	if (m_pDrawType)	m_pDrawType->DrawType(m_pDrawType->GetType());
	if (m_pDrawPrev)	m_pDrawPrev->Draw();

	/* 자식 윈도 호출 */
	CDlgMenu::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgGerb::OnResizeDlg()
{
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgGerb::UpdatePeriod(UINT64 tick, BOOL is_busy)
{
	/* Control Enable or Disable */
	UpdateControl(tick, is_busy);
}

/*
 desc : 컨트롤
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgGerb::UpdateControl(UINT64 tick, BOOL is_busy)
{
	BOOL bSelect	= m_pGridRecipe->GetSelected() ? TRUE : FALSE;

	if (m_btn_ctl[eGERB_BTN_RECIPE_SELECT].IsWindowEnabled() != (bSelect && !is_busy))	m_btn_ctl[eGERB_BTN_RECIPE_SELECT].EnableWindow(bSelect && !is_busy);		/* Recipe Selected */
	if (m_btn_ctl[eGERB_BTN_RECIPE_DELETE].IsWindowEnabled() != (bSelect && !is_busy))	m_btn_ctl[eGERB_BTN_RECIPE_DELETE].EnableWindow(bSelect && !is_busy);		/* Recipe Deleted */
	if (m_btn_ctl[eGERB_BTN_RECIPE_MODIFY].IsWindowEnabled() != (bSelect && !is_busy))	m_btn_ctl[eGERB_BTN_RECIPE_MODIFY].EnableWindow(bSelect && !is_busy);		/* Recipe Modified */
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgGerb::InitCtrl()
{
	// by sysandj : Resize UI
	CResizeUI clsResizeUI;
	// by sysandj : Resize UI

	INT32 i = 0;
	INT32 i32MaxStr[eGERB_EDT_TXT_MAX]	= { RECIPE_NAME_LENGTH, MAX_LOT_DATE_LENGTH, LED_POWER_NAME_LENGTH };
	INT64 i64Min[eGERB_EDT_INT_MAX]		= { 1,    1,  1000,  1000,  1,  240,  240, 10, 10, 10,   1,   1,   1,   1,   5,   0,   0 };
	INT64 i64Max[eGERB_EDT_INT_MAX]		= { 4, 9999, 99999, 99999, 24, 4096, 4096, 15, 15, 15, 999, 999, 999, 999, 100, 255, 255 };
	DOUBLE dbMin[eGERB_EDT_FLOAT_MAX]	= { 50.000f, 0.001f,  0.001f };
	DOUBLE dbMax[eGERB_EDT_FLOAT_MAX]	= { 99.999f, 99.999f, 0.999f };

	/* group box */
	for (i = 0; i < eGERB_GRP_MAX; i++)
	{
		m_grp_ctl[i].SetLogFont(&g_lf[eFONT_LEVEL2_BOLD]);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_grp_ctl[i]);
		// by sysandj : Resize UI
	}
	/* static - normal */
	for (i=0; i< eGERB_TXT_MAX; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_ctl[i]);
		// by sysandj : Resize UI
	}
	/* edit - normal */
	for (i=0; i< eGERB_EDT_CTL_MAX; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_ctl[i].SetReadOnly(TRUE);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_edt_ctl[i]);
		// by sysandj : Resize UI
	}
	/* edit - normal for frame rate */
	for (i=0; i< eGERB_EDT_SPD_MAX; i++)
	{
		m_edt_spd[i].SetEditFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_edt_spd[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_spd[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_spd[i].SetReadOnly(TRUE);
		m_edt_spd[i].SetBtnEvent(TRUE);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_edt_spd[i]);
		// by sysandj : Resize UI
	}
	/* edit - text */
	for (i=0; i< eGERB_EDT_TXT_MAX; i++)
	{
		m_edt_txt[i].SetEditFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_edt_txt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_txt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_txt[i].SetReadOnly(TRUE);
		m_edt_txt[i].SetInputMaxStr(i32MaxStr[i]);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_edt_txt[i]);
		// by sysandj : Resize UI
	}
	/* edit - keyboard - integer */
	for (i=0; i< eGERB_EDT_INT_MAX; i++)
	{
		m_edt_int[i].SetEditFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_edt_int[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_int[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_int[i].SetReadOnly(TRUE);
		m_edt_int[i].SetInputType(ENM_DITM::en_uint32);
		m_edt_int[i].SetInputSign(ENM_MNUS::en_unsign);
		m_edt_int[i].SetMinMaxNum(i64Min[i], i64Max[i]);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_edt_int[i]);
		// by sysandj : Resize UI
	}
	m_edt_int[eGERB_EDT_INT_EXPO_STEP].SetDisableKBDN();
	/* 마크 4군데의 길이 (6곳) 값 입력 부분 마우스 선택 못하도록 설정 */
	for (i= eGERB_EDT_INT_MARK_TOP_HORZ; i<= eGERB_EDT_INT_MARK_RIGHT_VERT; i++)	m_edt_int[i].SetMouseClick(FALSE);
	/* 초기 실행될 경우, 얼라인 마크가 Normal 인 경우이므로 */
	m_edt_int[eGERB_EDT_INT_AREA_WIDTH].EnableWindow(FALSE);
	m_edt_int[eGERB_EDT_INT_AREA_HEIGHT].EnableWindow(FALSE);
	m_edt_int[eGERB_EDT_INT_MARK_COUNT].EnableWindow(FALSE);

	/* edit - keyboard - float */
	for (i=0; i< eGERB_EDT_FLOAT_MAX; i++)
	{
		m_edt_flt[i].SetEditFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_edt_flt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_flt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_flt[i].SetReadOnly(TRUE);
		m_edt_flt[i].SetInputType(ENM_DITM::en_float);
		m_edt_flt[i].SetInputSign(ENM_MNUS::en_unsign);
		if (i != 0x02)	m_edt_flt[i].SetInputPoint(3);
		else			m_edt_flt[i].SetInputPoint(4);
		m_edt_flt[i].SetMinMaxNum(dbMin[i], dbMax[i]);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_edt_flt[i]);
		// by sysandj : Resize UI
	}
	/* checkbox - normal */
	for (i=0; i< eGERB_CHK_MAX; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_chk_ctl[i].SetBgColor(RGB(255, 255, 255));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_chk_ctl[i]);
		// by sysandj : Resize UI
	}
	/* 현재 Step by Step 방식은 지원하고 있지 않음 */
	m_chk_ctl[eGERB_CHK_MARK_STEP_BY_STEP].EnableWindow(FALSE);
	/* button - normal */
	for (i=0; i< eGERB_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}
}

/*
 desc : GridControl 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgGerb::InitGrid()
{
	/* Recipe Grid */
	m_pGridRecipe = new CGridRecipe(m_hWnd, m_grp_ctl[eGERB_GRP_RECIPE_LIST].GetSafeHwnd());
	ASSERT(m_pGridRecipe);
	if (!m_pGridRecipe->Init())	return FALSE;
	/* LED Power Items & Values Grid */
	m_pGridPowerI = new CGridPowerI(m_hWnd, m_grp_ctl[eGERB_GRP_LED_LIST].GetSafeHwnd());
	ASSERT(m_pGridPowerI);
	if (!m_pGridPowerI->Init())	return FALSE;
	m_pGridPowerV = new CGridPowerV(m_hWnd, m_grp_ctl[eGERB_GRP_LED_LIST].GetSafeHwnd());
	ASSERT(m_pGridPowerV);
	if (!m_pGridPowerV->Init())	return FALSE;
	/* Material Thick Calibration Grid for Align Camera */
	m_pGridThick = new CGridThick(m_hWnd, m_grp_ctl[eGERB_GRP_CALI_THICK].GetSafeHwnd());
	ASSERT(m_pGridThick);
	if (!m_pGridThick->Init())	return FALSE;

	/* Align Mark Array Type */
	m_pDrawType	= new CDrawType(m_grp_ctl[eGERB_GRP_ALIGN_MARK_TYPE].GetSafeHwnd());
	ASSERT(m_pDrawType);
	/* Gerber Preivew File */
	m_pDrawPrev	= new CDrawPrev(m_grp_ctl[eGERB_GRP_GERBER_PREVIEW].GetSafeHwnd());
	ASSERT(m_pDrawPrev);

	return TRUE;
}

/*
 desc : GridControl 해제
 parm : None
 retn : None
*/
VOID CDlgGerb::CloseGrid()
{
	if (m_pGridRecipe)	delete m_pGridRecipe;
	if (m_pGridThick)	delete m_pGridPowerI;
	if (m_pGridPowerV)	delete m_pGridPowerV;
	if (m_pGridThick)	delete m_pGridThick;
	if (m_pDrawType)	delete m_pDrawType;
	if (m_pDrawPrev)	delete m_pDrawPrev;
}

/*
 desc : 객체 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgGerb::InitObject()
{

	return TRUE;
}

/*
 desc : 객체 해제
 parm : None
 retn : None
*/
VOID CDlgGerb::CloseObject()
{
}

/*
 desc : 기본 데이터 값을 각 컨트롤에 설정
 parm : None
 retn : None
*/
VOID CDlgGerb::SetBaseData()
{
	TCHAR tzValue[128]	= {NULL};
	LPG_RBGF pstRecipe	= NULL;
	CUniToChar csCnv;

	/* 기존 선택된 레시피가 있다면, 관련 정보 출력 */
	if ((pstRecipe = uvEng_JobRecipe_GetSelectRecipe()))
	{
		m_pGridRecipe->Select(csCnv.Ansi2Uni(pstRecipe->recipe_name));
		UpdateRecipe(pstRecipe);
	}
	else
	{
		/* 화면 갱신 취소 */
		SetRedraw(FALSE);
#if 0
		SYSTEMTIME stNow;
		/* 현재 시간 */
		GetLocalTime(&stNow);
		/* Recipe Name */
		swprintf_s(tzValue, 128, L"SKC_RECIPE_%uH_%02d%02d%02d",
				   uvEng_GetConfig()->luria_svc.ph_count, stNow.wHour, stNow.wMinute, stNow.wSecond);
		m_edt_txt[0].SetWindowTextW(tzValue);	m_edt_txt[0].Invalidate(TRUE);
		/* Lot Date */
		swprintf_s(tzValue, 128, L"%04d-%02d-%02d", stNow.wYear, stNow.wMonth, stNow.wDay);
		m_edt_txt[1].SetWindowTextW(tzValue);	m_edt_txt[1].Invalidate(TRUE);
#endif
		/* check button */
		m_chk_ctl[eGERB_CHK_MARK_ON_THE_FLY].SetCheck(1);						/* On-the-fly */
		m_chk_ctl[eGERB_CHK_MARK_STEP_BY_STEP].SetCheck(0);						/* Step by Step */
		m_chk_ctl[eGERB_CHK_MARK_TYPE_NORMAL].SetCheck(1);						/* Mark : Normal */
		m_chk_ctl[eGERB_CHK_MARK_TYPE_MULTI].SetCheck(0);						/* Mark : Multi */
		m_chk_ctl[eGERB_CHK_FLIP_SERIAL_HORZ].SetCheck(0);						/* Flip : Serial Horz */
		m_chk_ctl[eGERB_CHK_FLIP_SERIAL_VERT].SetCheck(0);						/* Flip : Serial Vert */
		m_chk_ctl[eGERB_CHK_FLIP_SCALE_HORZ].SetCheck(0);						/* Flip : Scale Horz */
		m_chk_ctl[eGERB_CHK_FLIP_SCALE_VERT].SetCheck(0);						/* Flip : Scale Vert */
		m_chk_ctl[eGERB_CHK_FLIP_TEXT_HORZ].SetCheck(0);						/* Flip : Text Horz */
		m_chk_ctl[eGERB_CHK_FLIP_TEXT_VERT].SetCheck(0);						/* Flip : Text Vert */

		m_edt_int[eGERB_EDT_INT_AREA_WIDTH].EnableWindow(FALSE);					/* Area Width */
		m_edt_int[eGERB_EDT_INT_AREA_HEIGHT].EnableWindow(FALSE);					/* Area Height */
		m_edt_int[eGERB_EDT_INT_MARK_COUNT].EnableWindow(FALSE);					/* Mark Count */

		m_edt_int[eGERB_EDT_INT_EXPO_STEP].SetTextToNum(UINT32(1));					/* Expose Step */
		m_edt_int[eGERB_EDT_INT_EXPO_THICK].SetTextToNum(UINT32(120));				/* Material Thick */
		m_edt_int[eGERB_EDT_INT_AREA_WIDTH].SetTextToNum(UINT32(2000));				/* Area Width. Multi Mark의 diameter */
		m_edt_int[eGERB_EDT_INT_AREA_HEIGHT].SetTextToNum(UINT32(2000));			/* Area Height. Multi Mark의 diameter */
		m_edt_int[eGERB_EDT_INT_MARK_COUNT].SetTextToNum(UINT32(1));				/* Mark Count */
		m_edt_int[eGERB_EDT_INT_DECODE_SERIAL].SetTextToNum(UINT32(0));				/* DCode Serial */
		m_edt_int[eGERB_EDT_INT_DECODE_SCALE].SetTextToNum(UINT32(0));				/* DCode Scale */
		m_edt_int[eGERB_EDT_INT_DECODE_TEXT].SetTextToNum(UINT32(0));				/* DCode Text  */
		m_edt_int[eGERB_EDT_INT_MARK_TOP_HORZ].SetTextToNum(UINT32(0));				/* Mark Dist. Offset (TH)  */
		m_edt_int[eGERB_EDT_INT_MARK_BTM_HORZ].SetTextToNum(UINT32(0));				/* Mark Dist. Offset (BH) */
		m_edt_int[eGERB_EDT_INT_MARK_LEFT_VERT].SetTextToNum(UINT32(0));			/* Mark Dist. Offset (LV) */
		m_edt_int[eGERB_EDT_INT_MARK_RIGHT_VERT].SetTextToNum(UINT32(0));			/* Mark Dist. Offset (RV) */
		m_edt_int[eGERB_EDT_INT_MARK_LEFT_DIAG].SetTextToNum(UINT32(0));			/* Mark Dist. Offset (LD) */
		m_edt_int[eGERB_EDT_INT_MARK_RIGHT_DIAG].SetTextToNum(UINT32(0));			/* Mark Dist. Offset (RD) */
		m_edt_int[eGERB_EDT_INT_EXPO_DUTY].SetTextToNum(UINT32(90));				/* Duty Cycle */
		m_edt_int[eGERB_EDT_INT_ACAM_GAIN_1].SetTextToNum(UINT32(0));				/* Align Camera Gain Level */
		m_edt_int[eGERB_EDT_INT_ACAM_GAIN_2].SetTextToNum(UINT32(0));				/* Align Camera Gain Level */

		m_edt_flt[eGERB_EDT_FLOAT_SEARCH_SCALE].SetTextToNum(DOUBLE(50.0000), 4);		/* Search Range: Scale (Grabbed Image에서 등록된 마크를 검색하는데, 검색 마크의 크기 비율을 이 비율만큼 축소 ~ 확대하여 검색한다. 보통 50% ) */
		m_edt_flt[eGERB_EDT_FLOAT_SEARCH_SCORE].SetTextToNum(DOUBLE(80.0000), 4);		/* Search Value: Score (검색된 마크의 점수가 이 값보다 작으면 안됨. 보통 80%) */
		m_edt_flt[eGERB_EDT_FLOAT_MARK_DIST_OFFSET].SetTextToNum(DOUBLE(0.0001), 4);	/* Mark Dist. Rate*/
		m_edt_spd[eGERB_EDT_SPD_EXPO_SPEED].SetTextToNum(DOUBLE(0.950), 3);			/* Frame Rate (Speed) */

		/* 화면 갱신 활성화 */
		SetRedraw(TRUE);
		Invalidate(FALSE);
	}
}

/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgGerb::OnBtnClick(UINT32 id)
{
	switch (id)
	{
	case IDC_GERB_BTN_RECIPE_RESET	:	SetBaseData();		break;
	case IDC_GERB_BTN_MARK_ARRAY	:	UpdateMarkArray();	break;
	case IDC_GERB_BTN_FILE_SELECT	:	SelectGerber();		break;
	case IDC_GERB_BTN_RECIPE_APPEND	:	RecipeAppend(0x00);	break;
	case IDC_GERB_BTN_RECIPE_MODIFY	:	RecipeAppend(0x01);	break;
	case IDC_GERB_BTN_RECIPE_DELETE	:	RecipeAppend(0x02);	break;
	case IDC_GERB_BTN_RECIPE_SELECT	:	RecipeSelect();		break;
	case IDC_GERB_BTN_CALC_ENERGY	:	CalcEnergy();		break;
	case IDC_GERB_BTN_DIST_OFFSET	:	UpdateDistCalc();	break;
	}
}

/*
 desc : Frame Rate 에디트 박스 누른 경우
 parm : None
 retn : None
*/
VOID CDlgGerb::OnEdtExpoSpeed()
{
	LPG_OSSD pstPhStep	= NULL;
	CDlgMesg dlgMesg;
	CDlgSped dlgFrameRate;
	if (dlgFrameRate.MyDoModal() == IDOK)
	{
		DOUBLE dbFrameRate	= dlgFrameRate.GetFrameRate();
		m_edt_spd[eGERB_EDT_SPD_EXPO_SPEED].SetTextToNum(dbFrameRate, 3);
		/* Scroll Mode (즉, Step Size) 설정 */
		pstPhStep	= uvEng_PhStep_GetRecipeData((UINT16)ROUNDED(dbFrameRate * 1000, 0));
		if (!pstPhStep)	dlgMesg.MyDoModal(L"There is no ph step recipe", 0x01);
		else			m_edt_int[eGERB_EDT_INT_EXPO_STEP].SetTextToNum(pstPhStep->scroll_mode);
	}
	Invalidate(TRUE);
}

/*
 desc : 체크 버튼 클릭한 경우
 parm : id	- [in]  체크 버튼 ID
 retn : None
*/
VOID CDlgGerb::OnChkClick(UINT32 id)
{
	switch (id)
	{
	case IDC_GERB_CHK_MARK_ON_THE_FLY	:	m_chk_ctl[eGERB_CHK_MARK_ON_THE_FLY		].SetCheck(1);
											m_chk_ctl[eGERB_CHK_MARK_STEP_BY_STEP	].SetCheck(0);				break;
	case IDC_GERB_CHK_MARK_STEP_BY_STEP	:	m_chk_ctl[eGERB_CHK_MARK_ON_THE_FLY		].SetCheck(0);
											m_chk_ctl[eGERB_CHK_MARK_STEP_BY_STEP	].SetCheck(1);				break;
	case IDC_GERB_CHK_MARK_TYPE_NORMAL	:	m_chk_ctl[eGERB_CHK_MARK_TYPE_NORMAL	].SetCheck(1);
											m_chk_ctl[eGERB_CHK_MARK_TYPE_MULTI		].SetCheck(0);
											m_edt_int[eGERB_EDT_INT_AREA_WIDTH		].EnableWindow(FALSE);
											m_edt_int[eGERB_EDT_INT_AREA_HEIGHT		].EnableWindow(FALSE);
											m_edt_int[eGERB_EDT_INT_MARK_COUNT		].EnableWindow(FALSE);
											m_edt_int[eGERB_EDT_INT_MARK_COUNT		].SetTextToNum(UINT32(1));	break;
	case IDC_GERB_CHK_MARK_TYPE_MULTI	:	m_chk_ctl[eGERB_CHK_MARK_TYPE_NORMAL	].SetCheck(0);
											m_chk_ctl[eGERB_CHK_MARK_TYPE_MULTI		].SetCheck(1);
											m_edt_int[eGERB_EDT_INT_AREA_WIDTH		].EnableWindow(TRUE);
											m_edt_int[eGERB_EDT_INT_AREA_HEIGHT		].EnableWindow(TRUE);
											m_edt_int[eGERB_EDT_INT_MARK_COUNT		].EnableWindow(TRUE);		break;
	}
}

/*
 desc : 광량 및 노광 속도 초기화
 parm : None
 retn : None
*/
VOID CDlgGerb::ResetEnergySpeed()
{
	m_btn_ctl[eGERB_BTN_CALC_ENERGY].SetWindowTextW(L"Energy (000.0 mJ)\nSpeed (000.0f mm/s)");
	m_btn_ctl[eGERB_BTN_CALC_ENERGY].Invalidate(TRUE);
}

/*
 desc : Gerber Recipe 항목을 선택한 경우
 parm : nm_hdr	- [in]  Grid Control 정보가 저장된 구조체 포인터
		result	- [in]  fixed value (0)
 retn : None
*/
VOID CDlgGerb::OnGridGerbList(NMHDR *nm_hdr, LRESULT *result)
{
	NM_GRIDVIEW *pstGrid= (NM_GRIDVIEW*)nm_hdr;
	LPG_RBGF pstRecipe	= m_pGridRecipe->GetRecipe(pstGrid->iRow);

	/* 광량 및 노광 속도 초기화 */
	ResetEnergySpeed();

	if (pstRecipe)
	{
		TCHAR tzPath[MAX_PATH_LEN]	= {NULL};
		CUniToChar csCnv1, csCnv2;
		swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\%s",
				   csCnv1.Ansi2Uni(pstRecipe->gerber_path), csCnv2.Ansi2Uni(pstRecipe->gerber_name));
		if (!uvCmn_FindPath(tzPath))
		{
			CDlgMesg dlgMesg;
			if (dlgMesg.MyDoModal(L"Failed to find the path of gerber file\n"
								  L"Do you want to delete the recipe?", 0x00) == IDOK)
			{
				uvEng_JobRecipe_RecipeDelete(csCnv1.Ansi2Uni(pstRecipe->recipe_name));
				/* 기존 레시피 정보 초기화 */
				RecipeReset();
				/* 새롭게 등록된 레시피 갱신 */
				if (m_pGridRecipe)	m_pGridRecipe->Load();
				/* 광량 및 노광 속도 초기화 */
				ResetEnergySpeed();
				/* 전체 화면 갱신 */
				Invalidate(TRUE);
			}
		}
		else
		{
			UpdateRecipe(pstRecipe);
		}
	}
	Invalidate(TRUE);
}

/*
 desc : LED Recipe 항목을 선택한 경우
 parm : nm_hdr	- [in]  Grid Control 정보가 저장된 구조체 포인터
		result	- [in]  fixed value (0)
 retn : None
*/
VOID CDlgGerb::OnGridLedList(NMHDR *nm_hdr, LRESULT *result)
{
	NM_GRIDVIEW *pstGrid= (NM_GRIDVIEW*)nm_hdr;
	LPG_PLPI pstRecipe	= m_pGridPowerI->GetSelect(pstGrid->iRow);
	/* LED Power 값 입력 */
	if (pstRecipe)	m_pGridPowerV->SetRecipe(pstRecipe);
	Invalidate(TRUE);
}

/*
 desc : Gerber Recipe 정보를 화면에 갱신
 parm : recipe	- [in]  Gerber Recipe 정보가 저장된 구조체 포인터
 retn : None
*/
VOID CDlgGerb::UpdateRecipe(LPG_RBGF recipe)
{
	UINT8 i	= 0x00;
	BOOL bEnable				= FALSE;
	CHAR szFile[MAX_PATH_LEN]	= {NULL};
	LPG_PLPI pstPowerI			= NULL;
	CUniToChar csCnv;

	/* 화면 갱신 취소 */
	SetRedraw(FALSE);
	
	/* Recipe Name / Lot Date / Gerber Path */
	m_edt_txt[eGERB_EDT_TXT_RECIPE_NAME	].SetWindowTextW(csCnv.Ansi2Uni(recipe->recipe_name));
	m_edt_txt[eGERB_EDT_TXT_LOT_DATE	].SetWindowTextW(csCnv.Ansi2Uni(recipe->lot_date));
	m_edt_txt[eGERB_EDT_TXT_TEXT_STRING	].SetWindowTextW(csCnv.Ansi2Uni(recipe->text_string));
	sprintf_s(szFile, MAX_PATH_LEN, "%s\\%s", recipe->gerber_path, recipe->gerber_name);
	m_edt_ctl[eGERB_EDT_CTL_GERBER_FILE].SetWindowTextW(csCnv.Ansi2Uni(szFile));

	/* Step / Material Thick / Mark Area Size / Mark Count / Grab Size / DCode Number / Gerber Mark Dist / Duty Cycle*/
	m_edt_int[eGERB_EDT_INT_EXPO_STEP	].SetTextToNum(recipe->step_size);
	m_edt_int[eGERB_EDT_INT_EXPO_THICK	].SetTextToNum(recipe->material_thick);
	m_edt_int[eGERB_EDT_INT_AREA_WIDTH	].SetTextToNum(recipe->mark_area[0]);
	m_edt_int[eGERB_EDT_INT_AREA_HEIGHT	].SetTextToNum(recipe->mark_area[1]);
	m_edt_int[eGERB_EDT_INT_MARK_COUNT	].SetTextToNum(recipe->mark_count);
	bEnable	= (recipe->mark_model == (UINT8)(ENG_MMSM::en_single)) ? FALSE : TRUE;
	for (i= eGERB_EDT_INT_AREA_WIDTH; i<= eGERB_EDT_INT_AREA_HEIGHT; i++)
	{
		if (m_edt_int[i].IsWindowEnabled() != bEnable)	m_edt_int[i].EnableWindow(bEnable);
	}
	m_edt_int[eGERB_EDT_INT_DECODE_SERIAL	].SetTextToNum(recipe->dcode_serial);
	m_edt_int[eGERB_EDT_INT_DECODE_SCALE	].SetTextToNum(recipe->dcode_scale);
	m_edt_int[eGERB_EDT_INT_DECODE_TEXT		].SetTextToNum(recipe->dcode_text);
	m_edt_int[eGERB_EDT_INT_MARK_TOP_HORZ	].SetTextToNum(recipe->global_mark_dist[0]);
	m_edt_int[eGERB_EDT_INT_MARK_BTM_HORZ	].SetTextToNum(recipe->global_mark_dist[1]);
	m_edt_int[eGERB_EDT_INT_MARK_LEFT_VERT	].SetTextToNum(recipe->global_mark_dist[2]);
	m_edt_int[eGERB_EDT_INT_MARK_RIGHT_VERT	].SetTextToNum(recipe->global_mark_dist[3]);
	m_edt_int[eGERB_EDT_INT_MARK_LEFT_DIAG	].SetTextToNum(recipe->global_mark_dist[4]);
	m_edt_int[eGERB_EDT_INT_MARK_RIGHT_DIAG	].SetTextToNum(recipe->global_mark_dist[5]);
	m_edt_int[eGERB_EDT_INT_EXPO_DUTY		].SetTextToNum(recipe->led_duty_cycle);
	m_edt_int[eGERB_EDT_INT_ACAM_GAIN_1		].SetTextToNum(recipe->gain_level[0]);
	m_edt_int[eGERB_EDT_INT_ACAM_GAIN_2		].SetTextToNum(recipe->gain_level[1]);
	/* Scale / Score / Speed */
	m_edt_flt[eGERB_EDT_FLOAT_SEARCH_SCALE	].SetTextToNum(recipe->mark_scale_range, 4);		/* Grabbed Image에서 등록된 마크를 검색하는데, 검색 마크의 크기 비율을 이 비율만큼 축소 ~ 확대하여 검색한다. 보통 50% */
	m_edt_flt[eGERB_EDT_FLOAT_SEARCH_SCORE	].SetTextToNum(recipe->mark_score_accept, 4);	/* 검색된 마크의 점수가 이 값보다 작으면 안됨. 보통 80% */
	m_edt_flt[eGERB_EDT_FLOAT_MARK_DIST_OFFSET].SetTextToNum(DOUBLE(0.0001), 4);	/* Mark Dist. Rate */
	m_edt_spd[eGERB_EDT_SPD_EXPO_SPEED		].SetTextToNum(DOUBLE(recipe->frame_rate/1000.0f), 3);
	/* otf or step by step / normal or mixed / flip_seria / flip_scale / flip_text */
	m_chk_ctl[eGERB_CHK_MARK_ON_THE_FLY		].SetCheck(!recipe->align_method);
	m_chk_ctl[eGERB_CHK_MARK_STEP_BY_STEP	].SetCheck(recipe->align_method);
	m_chk_ctl[eGERB_CHK_MARK_TYPE_NORMAL	].SetCheck((recipe->mark_model == (UINT8)(ENG_MMSM::en_single)));
	m_chk_ctl[eGERB_CHK_MARK_TYPE_MULTI		].SetCheck((recipe->mark_model == (UINT8)(ENG_MMSM::en_multi_only)));
	/* Flip Horz,Vert */
	m_chk_ctl[eGERB_CHK_FLIP_SERIAL_HORZ	].SetCheck(recipe->serial_flip_h);
	m_chk_ctl[eGERB_CHK_FLIP_SERIAL_VERT	].SetCheck(recipe->serial_flip_v);
	m_chk_ctl[eGERB_CHK_FLIP_SCALE_HORZ		].SetCheck(recipe->scale_flip_h);
	m_chk_ctl[eGERB_CHK_FLIP_SCALE_VERT		].SetCheck(recipe->scale_flip_v);
	m_chk_ctl[eGERB_CHK_FLIP_TEXT_HORZ		].SetCheck(recipe->text_flip_h);
	m_chk_ctl[eGERB_CHK_FLIP_TEXT_VERT		].SetCheck(recipe->text_flip_v);

	/* Align Mark Array Type */
	m_pDrawType->DrawType(ENG_ATGL(recipe->align_type));
	/* Selection for Calibration Thick */
	m_pGridThick->Select(recipe->cali_thick);
	/* Selection for Led Power Recipe */
	m_pGridPowerI->Select(csCnv.Ansi2Uni(recipe->power_name));
	/* Get the Power LED Value */
	pstPowerI	= m_pGridPowerI->GetSelected();
	if (pstPowerI)	m_pGridPowerV->SetRecipe(pstPowerI);
	/* Memory DC 영역에 출력 */
	m_pDrawPrev->DrawMem(recipe);

	/* 에너지 및 노광 속도 재계산 */
	CalcEnergy();
	/* 화면 갱신 활성화 */
	SetRedraw(TRUE);
	Invalidate(FALSE);
}

/*
 desc : Mark Array Type 갱신
 parm : None
 retn : None
*/
VOID CDlgGerb::UpdateMarkArray()
{
	CAtlList <DOUBLE> lstX, lstY;
	TCHAR tzGerb[MAX_GERBER_NAME]	= {NULL};
	CDlgMesg dlgMesg;

	/* 현재 선택된 거버 파일 (전체 경로) 얻기 */
	m_edt_ctl[eGERB_EDT_CTL_GERBER_FILE].GetWindowTextW(tzGerb, MAX_GERBER_NAME);
	if (wcslen(tzGerb) < 1)	return;
	if (!uvCmn_FindPath(tzGerb))
	{
		dlgMesg.MyDoModal(L"The path to gerber file does not exist", 0x01);
		return;
	}

	/* 거버에 대한 마크 정보 얻기 */
	if (0x00 != uvEng_Luria_GetGlobalMarkJobName(tzGerb, lstX, lstY,
												 ENG_ATGL::en_global_4_local_0x0_n_point))
	{
		dlgMesg.MyDoModal(L"Failed to get the mark info. of gerber file", 0x01);
		return;
	}

	/* 만약 현재 등록된 마크가 없다면 */
	if (lstX.GetCount() < 4 || lstY.GetCount() < 4)	return;

	/* 다음에 출력될 마크 종류 선택 */
	if (m_pDrawType)
	{
		m_pDrawType->DrawType(m_pDrawType->GetNextType());
	}
}

/*
 desc : 새로운 Job Name 등록
 parm : None
 retn : None
*/
VOID CDlgGerb::SelectGerber()
{
	TCHAR tzJobName[MAX_PATH_LEN]	= {NULL}, tzRLTFiles[MAX_PATH_LEN] = {NULL};
	CDlgMesg dlgMesg;
	LPG_LDJM pstJobMgt	= &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* Job Name 디렉토리 선택 */
	if (!uvCmn_RootSelectPath(m_hWnd, uvEng_GetConfig()->set_comn.gerber_path, tzJobName))	return;
	/* Job Name 내에 하위 디렉토리가 존재하는지 확인 */
	if (uvCmn_GetChildPathCount(tzJobName) != 0)
	{
		dlgMesg.MyDoModal(L"This is not a gerber directory", 0x01);
		return;
	}
	/* 해당 디렉토리 내에 rlt_settings.xml 파일이 존재하는지 확인 */
	swprintf_s(tzRLTFiles, MAX_PATH_LEN, L"%s\\rlt_settings.xml", tzJobName);
	if (!uvCmn_FindFile(tzRLTFiles))
	{
		dlgMesg.MyDoModal(L"File (rlt_settings.xml) does not exist", 0x01);
		return;
	}
	/* 기존에 등록된 거버 파일 중에 동일한 거버가 있는지 여부 확인 */
	if (pstJobMgt->IsJobFullFinded(csCnv.Uni2Ansi(tzJobName)))
	{
		dlgMesg.MyDoModal(L"Job Name already registered exist", 0x01);
		return;
	}

	/* 선택된 거버 파일 출력 */
	m_edt_ctl[eGERB_EDT_CTL_GERBER_FILE].SetWindowTextW(tzJobName);

	/* 무조건 마크 관련 정보 초기화 */
	ResetMarkInfo();
}

/*
 desc : 새로운 레시피 추가
 parm : mode	- [in]  0x00:Append, 0x01:Modify, 0x02:Delete
 retn : None
*/
VOID CDlgGerb::RecipeAppend(UINT8 mode)
{
	BOOL bUpdated		= FALSE;
	TCHAR tzText[1024]	= {NULL}, tzPath[MAX_PATH_LEN] = {NULL}, tzName[MAX_GERBER_NAME] = {NULL};
	LPG_PLPI pstPowerI	= NULL;
	STG_RBGF stRecipe	= {NULL};
	CUniToChar csCnv;
	CDlgMesg dlgMesg;

	/* 레시피 저장 구조체 초기화 */
	stRecipe.Init();

	/* Recipe Name / Lot Date / Gerber Path */
	wmemset(tzText, 0x00, 1024);	m_edt_txt[eGERB_EDT_TXT_RECIPE_NAME	].GetWindowTextW(tzText, 1024);
	strcpy_s(stRecipe.recipe_name, RECIPE_NAME_LENGTH, csCnv.Uni2Ansi(tzText));
	wmemset(tzText, 0x00, 1024);	m_edt_txt[eGERB_EDT_TXT_LOT_DATE	].GetWindowTextW(tzText, 1024);
	strcpy_s(stRecipe.lot_date,		MAX_LOT_DATE_LENGTH, csCnv.Uni2Ansi(tzText));
	wmemset(tzText, 0x00, 1024);	m_edt_txt[eGERB_EDT_TXT_TEXT_STRING	].GetWindowTextW(tzText, 1024);
	strcpy_s(stRecipe.text_string,	MAX_PANEL_TEXT_STRING, csCnv.Uni2Ansi(tzText));
	m_edt_ctl[eGERB_EDT_CTL_GERBER_FILE].GetWindowTextW(tzText, 1024);
	if (!uvCmn_Luria_GetGerbPathName(tzText, tzPath, tzName))
	{
		dlgMesg.MyDoModal(L"Failed to get the gerber info (Path, Name)", 0x01);
		return;
	}
	strcpy_s(stRecipe.gerber_path, MAX_PATH_LEN, csCnv.Uni2Ansi(tzPath));
	strcpy_s(stRecipe.gerber_name, MAX_GERBER_NAME, csCnv.Uni2Ansi(tzName));

	/* Step / Material Thick / Mark Area Size / Mark Count / Grab Size / DCode Number / Gerber Mark Dist / Duty Cycle*/
	stRecipe.step_size			= ( UINT8)m_edt_int[eGERB_EDT_INT_EXPO_STEP			].GetTextToNum();
	stRecipe.material_thick		= (UINT32)m_edt_int[eGERB_EDT_INT_EXPO_THICK		].GetTextToNum();
	stRecipe.mark_area[0]		= (UINT32)m_edt_int[eGERB_EDT_INT_AREA_WIDTH		].GetTextToNum();
	stRecipe.mark_area[1]		= (UINT32)m_edt_int[eGERB_EDT_INT_AREA_HEIGHT		].GetTextToNum();
	stRecipe.mark_count			= ( UINT8)m_edt_int[eGERB_EDT_INT_MARK_COUNT		].GetTextToNum();
	stRecipe.dcode_serial		= (UINT16)m_edt_int[eGERB_EDT_INT_DECODE_SERIAL		].GetTextToNum();
	stRecipe.dcode_scale		= (UINT16)m_edt_int[eGERB_EDT_INT_DECODE_SCALE		].GetTextToNum();
	stRecipe.dcode_text			= (UINT16)m_edt_int[eGERB_EDT_INT_DECODE_TEXT		].GetTextToNum();
	stRecipe.global_mark_dist[0]= (UINT32)m_edt_int[eGERB_EDT_INT_MARK_TOP_HORZ		].GetTextToNum();
	stRecipe.global_mark_dist[1]= (UINT32)m_edt_int[eGERB_EDT_INT_MARK_BTM_HORZ		].GetTextToNum();
	stRecipe.global_mark_dist[2]= (UINT32)m_edt_int[eGERB_EDT_INT_MARK_LEFT_VERT	].GetTextToNum();
	stRecipe.global_mark_dist[3]= (UINT32)m_edt_int[eGERB_EDT_INT_MARK_RIGHT_VERT	].GetTextToNum();
	stRecipe.global_mark_dist[4]= (UINT32)m_edt_int[eGERB_EDT_INT_MARK_LEFT_DIAG	].GetTextToNum();
	stRecipe.global_mark_dist[5]= (UINT32)m_edt_int[eGERB_EDT_INT_MARK_RIGHT_DIAG	].GetTextToNum();
	stRecipe.led_duty_cycle		= ( UINT8)m_edt_int[eGERB_EDT_INT_EXPO_DUTY			].GetTextToNum();
	stRecipe.gain_level[0]		= ( UINT8)m_edt_int[eGERB_EDT_INT_ACAM_GAIN_1		].GetTextToNum();
	stRecipe.gain_level[1]		= ( UINT8)m_edt_int[eGERB_EDT_INT_ACAM_GAIN_2		].GetTextToNum();
	/* Scale / Score / Speed */
	stRecipe.mark_scale_range	= m_edt_flt[eGERB_EDT_FLOAT_SEARCH_SCALE	].GetTextToDouble();	/* Grabbed Image에서 등록된 마크를 검색하는데, 검색 마크의 크기 비율을 이 비율만큼 축소 ~ 확대하여 검색한다. 보통 50% */
	stRecipe.mark_score_accept	= m_edt_flt[eGERB_EDT_FLOAT_SEARCH_SCORE	].GetTextToDouble();	/* 검색된 마크의 점수가 이 값보다 작으면 안됨. 보통 80% */
	stRecipe.frame_rate			= (UINT16)ROUNDED(m_edt_spd[eGERB_EDT_SPD_EXPO_SPEED].GetTextToDouble() * 1000.0f, 0);
	/* otf or step by step / normal or mixed / flip_seria / flip_scale / flip_text */
#if 1
	if (m_chk_ctl[eGERB_CHK_MARK_ON_THE_FLY].GetCheck())	stRecipe.align_method	= 0x00;
	else							stRecipe.align_method	= 0x01;
#else
	stRecipe.align_method		= (UINT8)m_chk_ctl[eGERB_CHK_MARK_ON_THE_FLY].GetCheck();
	stRecipe.align_method		= (UINT8)m_chk_ctl[eGERB_CHK_MARK_STEP_BY_STEP].GetCheck();
#endif
	stRecipe.mark_model			= (m_chk_ctl[eGERB_CHK_MARK_TYPE_NORMAL].GetCheck() == 1) ? (UINT8)ENG_MMSM::en_single : (UINT8)ENG_MMSM::en_multi_only;
	/* Flip Horz,Vert */
	stRecipe.serial_flip_h		= (UINT8)m_chk_ctl[eGERB_CHK_FLIP_SERIAL_HORZ	].GetCheck();
	stRecipe.serial_flip_v		= (UINT8)m_chk_ctl[eGERB_CHK_FLIP_SERIAL_VERT	].GetCheck();
	stRecipe.scale_flip_h		= (UINT8)m_chk_ctl[eGERB_CHK_FLIP_SCALE_HORZ	].GetCheck();
	stRecipe.scale_flip_v		= (UINT8)m_chk_ctl[eGERB_CHK_FLIP_SCALE_VERT	].GetCheck();
	stRecipe.text_flip_h		= (UINT8)m_chk_ctl[eGERB_CHK_FLIP_TEXT_HORZ		].GetCheck();
	stRecipe.text_flip_v		= (UINT8)m_chk_ctl[eGERB_CHK_FLIP_TEXT_VERT		].GetCheck();

	/* Align Mark Array Type */
	stRecipe.align_type			= (UINT8)m_pDrawType->GetType();
	/* Selection for Calibration Thick */
	stRecipe.cali_thick			= m_pGridThick->GetSelectThick();
	/* Selection for Led Power Recipe */
	if (m_pGridPowerI->GetSelected())
	{
		strcpy_s(stRecipe.power_name, LED_POWER_NAME_LENGTH, m_pGridPowerI->GetSelected()->led_name);
	}

	/* 값이 정상적으로 등록되어 있는지 확인 */
	if (!stRecipe.IsValid())
	{
		dlgMesg.MyDoModal(L"The value is not set normally.", 0x01);
	}
	else
	{
		/* 레시피 등록/수정/삭제 진행 */
		switch (mode)
		{
		case 0x00 : bUpdated = uvEng_JobRecipe_RecipeAppend(&stRecipe);							break;
		case 0x01 : bUpdated = uvEng_JobRecipe_RecipeModify(&stRecipe);							break;
		case 0x02 : bUpdated = uvEng_JobRecipe_RecipeDelete(csCnv.Ansi2Uni(stRecipe.recipe_name));	break;
		}
	}
	/* 레시피 저장 구조체 해제 */
	stRecipe.Close();

	/* 등록 성공 여부 */
	if (!bUpdated)	return;
	
	/* 기존 레시피 정보 초기화 */
	RecipeReset();
	/* 새롭게 등록된 레시피 갱신 */
	if (m_pGridRecipe)	m_pGridRecipe->Load();
	switch (mode)
	{
	case 0x00 : dlgMesg.MyDoModal(L"Append recipe completed", 0x01);	break;
	case 0x01 : dlgMesg.MyDoModal(L"Modify recipe completed", 0x01);	break;
	case 0x02 : dlgMesg.MyDoModal(L"Delete recipe completed", 0x01);	break;
	}

	/* 광량 및 노광 속도 초기화 */
	ResetEnergySpeed();
	/* 전체 화면 갱신 */
	Invalidate(TRUE);
}

/*
 desc : 기존 설정된 레시피 정보 초기화
 parm : None
 retn : None
*/
VOID CDlgGerb::RecipeReset()
{
	UINT8 i	= 0x00;
	/* Recipe Name / Lot Date / Gerber Path */
	m_edt_txt[eGERB_EDT_TXT_RECIPE_NAME	].SetWindowTextW(L"");
	m_edt_txt[eGERB_EDT_TXT_LOT_DATE	].SetWindowTextW(L"");
	m_edt_txt[eGERB_EDT_TXT_TEXT_STRING	].SetWindowTextW(L"");
	m_edt_ctl[eGERB_EDT_CTL_GERBER_FILE	].SetWindowTextW(L"");

	/* Step / Material Thick / Mark Area Size / Mark Count / Grab Size / DCode Number / Gerber Mark Dist / Duty Cycle*/
	for (i=0x00; i< eGERB_EDT_INT_MAX; i++)	m_edt_int[i].SetWindowTextW(L"");
	/* Scale / Score / Speed */
	m_edt_flt[eGERB_EDT_FLOAT_SEARCH_SCALE		].SetWindowTextW(L"50.0000");
	m_edt_flt[eGERB_EDT_FLOAT_SEARCH_SCORE		].SetWindowTextW(L"80.0000");
	m_edt_flt[eGERB_EDT_FLOAT_MARK_DIST_OFFSET	].SetWindowTextW(L"0.0001");
	m_edt_spd[eGERB_EDT_SPD_EXPO_SPEED			].SetWindowTextW(L"");
	/* otf or step by step / normal or mixed / flip_seria / flip_scale / flip_text */
	m_chk_ctl[eGERB_CHK_MARK_ON_THE_FLY		].SetCheck(1);
	m_chk_ctl[eGERB_CHK_MARK_STEP_BY_STEP	].SetCheck(0);
	m_chk_ctl[eGERB_CHK_MARK_TYPE_NORMAL	].SetCheck(1);
	m_chk_ctl[eGERB_CHK_MARK_TYPE_MULTI		].SetCheck(0);
	/* Flip Horz,Vert */
	m_chk_ctl[eGERB_CHK_FLIP_SERIAL_HORZ	].SetCheck(0);
	m_chk_ctl[eGERB_CHK_FLIP_SERIAL_VERT	].SetCheck(0);
	m_chk_ctl[eGERB_CHK_FLIP_SCALE_HORZ		].SetCheck(0);
	m_chk_ctl[eGERB_CHK_FLIP_SCALE_VERT		].SetCheck(0);
	m_chk_ctl[eGERB_CHK_FLIP_TEXT_HORZ		].SetCheck(0);
	m_chk_ctl[eGERB_CHK_FLIP_TEXT_VERT		].SetCheck(0);
	/* Align Mark Array Type */
	m_pDrawType->DrawType(ENG_ATGL::en_global_0_local_0x0_n_point);
	/* Selection for Gerber Recipe */
	m_pGridRecipe->Select(L"");
	/* Selection for Calibration Thick */
	m_pGridThick->Select(0);
	/* Selection for Led Power Recipe */
	m_pGridPowerI->Select(L"");
	/* Get the Power LED Value */
	m_pGridPowerV->SetRecipe(NULL);
	/* Gerber Preview */
	m_pDrawPrev->DrawMem(NULL);
}

/*
 desc : 현재 선택된 Gerber Recipe 항목 메모리에 적재
 parm : None
 retn : None
*/
VOID CDlgGerb::RecipeSelect()
{
	CHAR szGerb[MAX_PATH_LEN]	= {NULL};
	CUniToChar csCnv;
	LPG_RBGF pstRecipe	= m_pGridRecipe->GetSelected();
	CDlgMesg dlgMesg;

	/* 거버 파일의 존재 여부 (거파 파일 검색) */
	sprintf_s(szGerb, MAX_PATH_LEN, "%s\\%s", pstRecipe->gerber_path, pstRecipe->gerber_name);
	if (!uvCmn_FindFile(csCnv.Ansi2Uni(szGerb)))
	{
		dlgMesg.MyDoModal(L"The gerber file registered in the recipe does not exist", 0x01);
		return;
	}
	/* 등록된 마크 정보가 있는지 여부 */
	if (pstRecipe->align_type != UINT8(ENG_ATGL::en_global_0_local_0x0_n_point))
	{
		/* 얼라인 카메라의 2D 보정 파일 적재 (검색) */
		if (!uvEng_ACamCali_LoadFile(pstRecipe->cali_thick))
		{
			if (IDOK != dlgMesg.MyDoModal(L"Failed to load the calibration data for align camera\n",
										  L"Continue", L"Abort"))
			{
				return;
			}
		}
	}
	/* 전역 메모리에 항목 선택 설정 진행 */
	if (!uvEng_JobRecipe_SelRecipeOnlyName(csCnv.Ansi2Uni(pstRecipe->recipe_name)))
	{
		dlgMesg.MyDoModal(L"Failed to select the gerber recipe", 0x01);
		return;
	}


#if (CUSTOM_CODE_TEST_PODIS == 0)
	/* Align Camera의 Gain Level 값 설정 */
	if (!uvEng_Camera_SetGainLevel(0x01, pstRecipe->gain_level[0]))
	{
		dlgMesg.MyDoModal(L"Failed to set the gain_level for align camera (1)", 0x01);
		return;
	}
	if (!uvEng_Camera_SetGainLevel(0x02, pstRecipe->gain_level[1]))
	{
		dlgMesg.MyDoModal(L"Failed to set the gain_level for align camera (2)", 0x01);
		return;
	}
#endif

	/* 기존 적재된 Recipe 관련 거버 정보 초기화 */
	uvEng_Mark_SelRecipeReset();
	uvCmn_Luria_ResetRegisteredJob();

#if (CUSTOM_CODE_TEST_PODIS == 0)
	/* Multi-Mark인 경우만 해당됨, 마크 검색 Area 크기 값 설정 */
	if (pstRecipe->mark_count == 1)	uvEng_Camera_SetMultiMarkArea();
	else							uvEng_Camera_SetMultiMarkArea(pstRecipe->mark_area[0], pstRecipe->mark_area[1]);


	/* 최정 검색된 마크의 검색 조건 값 설정 */
	uvEng_Camera_SetRecipeMarkRate(pstRecipe->mark_score_accept, pstRecipe->mark_scale_range);
#endif

	/* 현재 선택된 레시피 (거버)에 마크가 등록되어 있는지 여부 */
	if (uvEng_Luria_GetGerberMarkCount(csCnv.Ansi2Uni(szGerb), ENG_AMTF::en_global) < 1)
	{
		m_pDlgMain->PostMessageW(WM_MAIN_CHILD, WPARAM(ENG_CMDI::en_menu_expo), 0L);
	}
	else
	{
		/* Mark Recipe가 선택되어 있지 않다면 ... */
#if 0
		if (!uvEng_Mark_GetSelectRecipe())
#endif
		{
			m_pDlgMain->PostMessageW(WM_MAIN_CHILD, WPARAM(ENG_CMDI::en_menu_mark), 0L);
		}
#if 0
		else
		{
			m_pDlgMain->PostMessageW(WM_MAIN_CHILD, WPARAM(ENG_CMDI::en_menu_expo), 0L);
		}
#endif
	}
}

/*
 desc : Energy 계산
 parm : None
 retn : None
*/
VOID CDlgGerb::CalcEnergy()
{
	TCHAR tzMesg[128]	= {NULL};
	UINT8 i	= 0, j		= 0;
	UINT16 u16FrameRate	= (UINT16)ROUNDED(m_edt_spd[eGERB_EDT_SPD_EXPO_SPEED].GetTextToDouble() * 1000, 0);
	DOUBLE dbTotal		= 0.0f, dbPowerWatt[MAX_LED] = {NULL}, dbSpeed = 0.0f;

	UINT8 u8Step	= (UINT8)m_edt_int[eGERB_EDT_INT_EXPO_STEP].GetTextToNum();
	UINT8 u8Duty	= (UINT8)m_edt_int[eGERB_EDT_INT_EXPO_DUTY].GetTextToNum();
	UINT16 u16Rate	= (UINT16)ROUNDED(m_edt_spd[eGERB_EDT_SPD_EXPO_SPEED].GetTextToDouble() * 1000.0f, 0);

	/* 광량 계산 */
	for (; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		for (j=0; j<MAX_LED; j++)	dbPowerWatt[j]	= m_pGridPowerV->GetWatt(i+1, j+1);
		dbTotal	+=	uvCmn_Luria_GetExposeEnergy(u8Step, u8Duty, u16Rate, dbPowerWatt);
	}
	/* 노광 속도 */
	dbSpeed	= uvCmn_Luria_GetExposeSpeed(u16FrameRate, u8Step);

	swprintf_s(tzMesg, 128, L"Energy (%4.1f mJ)\nSpeed (%4.1f mm/s)", dbTotal / DOUBLE(i), dbSpeed);
	m_btn_ctl[eGERB_BTN_CALC_ENERGY].SetWindowTextW(tzMesg);
}

/*
 desc : Mark 간의 거리 즉, 상/하 수평, 좌/우 수직, 대각선 2개 등의 거리 값 설정
 parm : none
 retn : None
*/
VOID CDlgGerb::CalcMarkDist()
{
	UINT32 u32Dist[6]	= {NULL};	/* um */
	DOUBLE dbRate		= 0.0003 /* Medium (Middle) */, dbDist = 0.0f /* mm */;
	CDlgMesg dlgMesg;
	CAtlList <DOUBLE> lstX, lstY;
	TCHAR tzGerb[MAX_GERBER_NAME]	= {NULL};

	/* 현재 선택된 거버 파일 (전체 경로) 얻기 */
	m_edt_ctl[eGERB_EDT_CTL_GERBER_FILE].GetWindowTextW(tzGerb, MAX_GERBER_NAME);
	if (wcslen(tzGerb) < 1)	return;
	if (!uvCmn_FindPath(tzGerb))
	{
		dlgMesg.MyDoModal(L"The path to gerber file does not exist", 0x01);
		return;
	}

	/* 거버에 대한 마크 정보 얻기 */
	if (0x00 != uvEng_Luria_GetGlobalMarkJobName(tzGerb, lstX, lstY,
												 ENG_ATGL::en_global_4_local_0x0_n_point))
	{
		dlgMesg.MyDoModal(L"Failed to get the mark info. of gerber file", 0x01);
	}
	else
	{
		dbRate	= m_edt_flt[eGERB_EDT_FLOAT_MARK_DIST_OFFSET].GetTextToDouble();
		if (dbRate < 0.0001)	dbRate	= 0.0001f;
		/* 만약 현재 등록된 마크가 없다면 */
		if (lstX.GetCount() >= 4 && lstY.GetCount() >= 4)
		{
			/* 1번 / 3번 마크 간의 수평 (거리) 길이 값에 오차 값 적용 */
			dbDist		= abs(lstX.GetAt(lstX.FindIndex(0)) - lstX.GetAt(lstX.FindIndex(2)));
			u32Dist[0]	= (UINT32)ROUNDDN(dbDist * 1000.0f * dbRate, 0);
			/* 2번 / 4번 마크 간의 수평 (거리) 길이 값에 오차 값 적용 */
			dbDist		= abs(lstX.GetAt(lstX.FindIndex(1)) - lstX.GetAt(lstX.FindIndex(3)));
			u32Dist[1]	= (UINT32)ROUNDDN(dbDist * 1000.0f * dbRate, 0);
			/* 1번 / 2번 마크 간의 수직 (거리) 길이 값에 오차 값 적용 */
			dbDist		= abs(lstY.GetAt(lstY.FindIndex(0)) - lstY.GetAt(lstY.FindIndex(1)));
			u32Dist[2]	= (UINT32)ROUNDDN(dbDist * 1000.0f * dbRate, 0);
			/* 3번 / 4번 마크 간의 수직 (거리) 길이 값에 오차 값 적용 */
			dbDist		= abs(lstY.GetAt(lstY.FindIndex(2)) - lstY.GetAt(lstY.FindIndex(3)));
			u32Dist[3]	= (UINT32)ROUNDDN(dbDist * 1000.0f * dbRate, 0);
			/* 1번 / 4번 마크 간의 대각 (거리) 길이 값에 오차 값 적용 */
			dbDist		= sqrt(pow(lstY.GetAt(lstY.FindIndex(0)) - lstY.GetAt(lstY.FindIndex(1)), 2) +
							   pow(lstX.GetAt(lstX.FindIndex(0)) - lstX.GetAt(lstX.FindIndex(3)), 2));
			u32Dist[4]	= (UINT32)ROUNDDN(dbDist * 1000.0f * dbRate, 0);
			/* 2번 / 3번 마크 간의 대각 (거리) 길이 값에 오차 값 적용 */
			dbDist		= sqrt(pow(lstY.GetAt(lstY.FindIndex(2)) - lstY.GetAt(lstY.FindIndex(3)), 2) +
							   pow(lstX.GetAt(lstX.FindIndex(1)) - lstX.GetAt(lstX.FindIndex(2)), 2));
			u32Dist[5]	= (UINT32)ROUNDDN(dbDist * 1000.0f * dbRate, 0);
		}
	}
	m_edt_int[eGERB_EDT_INT_MARK_TOP_HORZ	].SetTextToNum(u32Dist[0]);
	m_edt_int[eGERB_EDT_INT_MARK_BTM_HORZ	].SetTextToNum(u32Dist[1]);
	m_edt_int[eGERB_EDT_INT_MARK_LEFT_VERT	].SetTextToNum(u32Dist[2]);
	m_edt_int[eGERB_EDT_INT_MARK_RIGHT_VERT	].SetTextToNum(u32Dist[3]);
	m_edt_int[eGERB_EDT_INT_MARK_LEFT_DIAG	].SetTextToNum(u32Dist[4]);
	m_edt_int[eGERB_EDT_INT_MARK_RIGHT_DIAG	].SetTextToNum(u32Dist[5]);
}

/*
 desc : 마크 정보 초기화
 parm : None
 retn : None
*/
VOID CDlgGerb::ResetMarkInfo()
{
	/* 마크 선택 정보 초기화 */
	if (m_pDrawType)	m_pDrawType->DrawType(ENG_ATGL::en_global_0_local_0x0_n_point);
	/* Gerber Preview */
	if (m_pDrawPrev)	m_pDrawPrev->DrawMem(NULL);
	/* Align Method (On the fly or Step by Step */
	m_chk_ctl[eGERB_CHK_MARK_ON_THE_FLY].SetCheck(1);
	/* Mark Model Type : Normal ? Multi */
	m_chk_ctl[eGERB_CHK_MARK_TYPE_NORMAL].SetCheck(1);
	/* Mark Dist. Offset */
	m_edt_flt[eGERB_EDT_FLOAT_MARK_DIST_OFFSET].SetTextToNum(DOUBLE(0.0001), 3);
	/* Distance 갱신 (초기화) */
	CalcMarkDist();
}

/*
*/
VOID CDlgGerb::UpdateDistCalc()
{
	DOUBLE dbRate = m_edt_flt[eGERB_EDT_FLOAT_MARK_DIST_OFFSET].GetTextToDouble() + 0.0002f;
	if (dbRate > 0.001f)	dbRate	= 0.0001f;
	m_edt_flt[eGERB_EDT_FLOAT_MARK_DIST_OFFSET].SetTextToNum(DOUBLE(dbRate), 4);
	CalcMarkDist();
}