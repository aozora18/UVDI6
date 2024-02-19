
/*
 desc : 광학계 단차 조정
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgStep.h"

#include "../../inc/grid/GridCtrl.h"

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
	m_bDrawBG	= TRUE;
	m_bTooltip	= TRUE;
	m_clrBg		= RGB(255, 255, 255);
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

	/* Groupbox - Normal */
	u32StartID	= IDC_STEP_GRP_STEP_XY;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Button - Normal */
	u32StartID	= IDC_STEP_BTN_APPLY;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_STEP_TXT_NEGA_OFFSET;
	for (i=0; i<6; i++)		DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Edit - Normal */
	u32StartID	= IDC_STEP_EDT_NEGA_OFFSET;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgStep, CMyDialog)
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
	InitCtrl();
	InitGrid();

	/* Load Data */
	LoadStepXY();

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgStep::OnExitDlg()
{
	CloseGrid();
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
 retn : None
*/
VOID CDlgStep::InitCtrl()
{
	INT32 i, i32Min[5] = {1, 0, 0, 0, 1};
	PTCHAR pText	= NULL;

	/* Normal - Group box */
	for (i=0; i<1; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* button - normal */
	for (i=0; i<2; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}
	/* text - normal */
	for (i=0; i<6; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* edit - Normal Value */
	for (i=0; i<3; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_ctl[i].SetInputType(ENM_DITM::en_int8);
		m_edt_ctl[i].SetMouseClick(FALSE);
		m_edt_ctl[i].SetReadOnly(TRUE);
	}
}

/*
 desc : 그리드 컨트롤 제거
 parm : None
 retn : None
*/
VOID CDlgStep::CloseGrid()
{
	if (m_pGridStepXY)
	{
		m_pGridStepXY->DestroyWindow();
		delete m_pGridStepXY;
	}
}

/*
 desc : 단차 설정 정보
 parm : None
 retn : None
*/
VOID CDlgStep::InitGrid()
{
	TCHAR tzCols[3][16]	= { L"Item", L"StepX (um)", L"StepY (um)" }, tzRows[32] = {NULL};
	INT32 i32Width[3]	= { 65, 80, 80 }, i, j=1;
	INT32 i32ColCnt		= 3, i32RowCnt = 0;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	::GetWindowRect(m_grp_ctl[0].GetSafeHwnd(), rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 15;
	rGrid.right	-= 15;
	rGrid.top	+= 22;
	rGrid.bottom-= 13;

	/* 총 출력될 행 (Row)의 개수 구하기 */
	i32RowCnt	= pstCfg->luria_svc.ph_count;
	/* 그리드 컨트롤 기본 공통 속성 */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* 객체 생성 */
	m_pGridStepXY	= new CGridCtrl;
	ASSERT(m_pGridStepXY);
	m_pGridStepXY->SetDrawScrollBarHorz(FALSE);
	m_pGridStepXY->SetDrawScrollBarVert(FALSE);
	if (!m_pGridStepXY->Create(rGrid, this, IDC_STEP_GRID_STEP_XY, dwStyle))
	{
		delete m_pGridStepXY;
		m_pGridStepXY = NULL;
		return;
	}
	/* 객체 기본 설정 */
	m_pGridStepXY->SetLogFont(g_lf);
	m_pGridStepXY->SetRowCount(i32RowCnt);
	m_pGridStepXY->SetColumnCount(i32ColCnt);
	m_pGridStepXY->SetFixedRowCount(1);
	m_pGridStepXY->SetRowHeight(0, 24);
	m_pGridStepXY->SetFixedColumnCount(0);
	m_pGridStepXY->SetBkColor(RGB(255, 255, 255));
	m_pGridStepXY->SetFixedColumnSelection(0);

	/* 타이틀 (첫 번째 행) */
	stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr = RGB(214, 214, 214);
	stGV.crFgClr = RGB(0, 0, 0);
	stGV.lfFont	= g_lf;
	for (i=0; i<i32ColCnt; i++)
	{
		stGV.nFormat= DT_VCENTER | DT_SINGLELINE | DT_CENTER;
		stGV.row	= 0;
		stGV.col	= i;
		stGV.strText= tzCols[i];
		m_pGridStepXY->SetItem(&stGV);
		m_pGridStepXY->SetColumnWidth(i, i32Width[i]);
	}

	/* 본문 배경색 */
	stGV.crBkClr = RGB(255, 255, 255);

	/* 본문 설정 - for Photohead Position */
	for (i=0x01,j=0x01; i<pstCfg->luria_svc.ph_count; i++,j++)
	{
		m_pGridStepXY->SetRowHeight(j, 24);

		/* for Subject */
		stGV.strText.Format(L"PH %d", i+1);
		stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.row	= j;
		stGV.col	= 0;
		m_pGridStepXY->SetItem(&stGV);

		/* for Subject */
		stGV.strText= L"0.000";
		stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.col	= 1;
		m_pGridStepXY->SetItem(&stGV);

		/* for Subject */
		stGV.col	= 2;
		m_pGridStepXY->SetItem(&stGV);
	}

	m_pGridStepXY->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGridStepXY->UpdateGrid();
}

/*
 desc : 현재 발생된 에러 정보 출력
 parm : None
 retn : None
*/
VOID CDlgStep::OnBtnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_STEP_BTN_APPLY		:	ApplyStepXY();			break;
	case IDC_STEP_BTN_CANCEL	:	CMyDialog::OnCancel();	break;
	}
}

/*
 desc : DoModal Override
 parm : data	- [in]  기존 광학계 단차 정보가 저장된 구조체 포인터
 retn : ?
*/
INT_PTR CDlgStep::MyDoModal(LPG_OSSD data)
{
	memcpy(&m_stStep, data, sizeof(STG_OSSD));

	return DoModal();
}

/*
 desc : Model 등록 작업 수행
 parm : None
 retn : None
*/
VOID CDlgStep::ApplyStepXY()
{
	UINT8 i		= 0x01;
	BOOL bSucc	= FALSE;

	/* 값 유효성 확인 */
	if (abs(INT32(m_stStep.delay_posi_nsec) - INT32(m_stStep.delay_nega_nsec)) > 1)
	{
		AfxMessageBox(L"The value entered is not valid", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
#if 0	/* 현재 적재된 거버가 있으면 동작 안되므로, 환경 파일에만 업데이트 수행 */
	/* 바로 광학계 단차를 적용할 것인지 여부 */
	if (IDYES == AfxMessageBox(L"Shall we apply the PH Offset values right away?", MB_YESNO))
	{
		if (!uvEng_Luria_ReqSetMotionType1Hysteresis(m_stStep.scroll_mode, m_stStep.nega_offset_px,
													 m_stStep.delay_posi_nsec, m_stStep.delay_nega_nsec))
		{
			AfxMessageBox(L"Failed to apply the PH Offset values", MB_ICONSTOP|MB_TOPMOST);
			return;
		}
		for (i=1; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
		{
			uvEng_Luria_ReqSetSpecPhotoHeadOffset(i+1,
												  m_stStep.step_x_nm[i], m_stStep.step_y_nm[i]);
		}
	}
#endif
	/* 환경 파일에 광학계 단차 값 저장 */
	for (i=0x01; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		uvEng_GetConfig()->luria_svc.ph_offset_x[i]	= m_stStep.step_x_nm[i]/1000000.0f;
		uvEng_GetConfig()->luria_svc.ph_offset_y[i]	= m_stStep.step_y_nm[i]/1000000.0f;
	}
	uvEng_GetConfig()->luria_svc.hys_type_1_scroll_mode		= m_stStep.scroll_mode;
	uvEng_GetConfig()->luria_svc.hys_type_1_negative_offset	= m_stStep.nega_offset_px;
	uvEng_GetConfig()->luria_svc.hys_type_1_delay_offset[0]	= m_stStep.delay_posi_nsec/1000000.0f;
	uvEng_GetConfig()->luria_svc.hys_type_1_delay_offset[1]	= m_stStep.delay_nega_nsec/1000000.0f;
	/* 환경 파일 저장 */
	uvEng_SaveConfig();
#if 1
	/* 기존에 존재하는 값이면 기존 데이터 삭제 진행 */
	if (uvEng_PhStep_RecipeFind(m_stStep.frame_rate))
	{
		uvEng_PhStep_RecipeDelete(m_stStep.frame_rate);
	}
	/* 새로 갱신된 레시피 추가 */
	if (!uvEng_PhStep_RecipeAppend(&m_stStep))
	{
		AfxMessageBox(L"Failed to save the offset value for PH", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 파일로 저장 */
	uvEng_PhStep_RecipeSave();
#endif
	/* 저장 작업 수행 */
	CMyDialog::OnOK();
}

/*
 desc : Edge Find Result Data 적재
 parm : None
 retn : None
*/
VOID CDlgStep::LoadStepXY()
{
	UINT8 i;

	for (i=0x01; i<uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		m_pGridStepXY->SetItemDouble(i, 1, m_stStep.step_x_nm[i]/1000.0f, 3);
		m_pGridStepXY->SetItemDouble(i, 2, m_stStep.step_y_nm[i]/1000.0f, 3);
	}

	/* 현재 설정되어 있는 Hysteresis 값 입력 */
	m_edt_ctl[0].SetTextToNum(m_stStep.nega_offset_px);
	m_edt_ctl[1].SetTextToNum(m_stStep.delay_posi_nsec);
	m_edt_ctl[2].SetTextToNum(m_stStep.delay_nega_nsec);
}
