
/*
 desc : Frame Rate 선택 대화 상자
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgSped.h"

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
CDlgSped::CDlgSped(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgSped::IDD, parent)
{
	m_bDrawBG	= TRUE;
	m_bTooltip	= TRUE;
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgSped::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Group - Normal */
	u32StartID	= IDC_SPED_GRP_FRAME_RATE;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Button - Normal */
	u32StartID	= IDC_SPED_BTN_APPLY;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgSped, CMyDialog)
	ON_NOTIFY(NM_CLICK, IDC_GRID_SPED_FRAME_RATE, OnGridFrameRate)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_SPED_BTN_APPLY, IDC_SPED_BTN_CANCEL, OnBtnClicked)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgSped::PreTranslateMessage(MSG* msg)
{

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgSped::OnInitDlg()
{
	/* 컨트롤 초기화 */
	if (!InitCtrl())	return FALSE;
	if (!InitGrid())	return FALSE;

	/* TOP_MOST & Center */
	CenterParentTopMost();
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgSped::OnExitDlg()
{
	if (m_pGrid)
	{
		if (m_pGrid->GetSafeHwnd())	m_pGrid->DestroyWindow();
		delete m_pGrid;
	}
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgSped::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgSped::OnResizeDlg()
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgSped::InitCtrl()
{
	INT32 i;
	PTCHAR pText	= NULL;

	/* group box - normal */
	for (i=0; i<1; i++)	m_grp_ctl[i].SetLogFont(&g_lf[eFONT_LEVEL2_BOLD]);
	/* button - normal */
	for (i=0; i<2; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);
	}

	return TRUE;
}

/*
 desc : 초기화 (생성)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgSped::InitGrid()
{
	CRect rGrid;
	GV_ITEM	stGV;

	/* Grid Control 속성 값 */
	stGV.mask		= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.nFormat	= DT_LEFT | DT_VCENTER | DT_SINGLELINE;
	stGV.crFgClr	= RGB(32, 32, 32);
	stGV.crBkClr	= RGB(255, 255, 255);

	/* 현재 윈도 Client 크기 */
	m_grp_ctl[0].GetWindowRect(rGrid);
	ScreenToClient(rGrid);

	CResizeUI clsResizeUI;
	/* Grid 크기 재설정 */
	rGrid.left		+= (long)(15 * clsResizeUI.GetRateX());
	rGrid.top		+= (long)(22 * clsResizeUI.GetRateY());
	rGrid.right		-= (long)(15 * clsResizeUI.GetRateX());
	rGrid.bottom	-= (long)(15 * clsResizeUI.GetRateY());

	/* 객체 생성 */
	m_pGrid = new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetDrawScrollBarVert(TRUE);
	m_pGrid->SetDrawScrollBarHorz(FALSE);
	if (!m_pGrid->Create(rGrid, this, IDC_GRID_SPED_FRAME_RATE, WS_CHILD|WS_TABSTOP|WS_VISIBLE))
	{
		delete m_pGrid;
		return FALSE;
	}

	/* 객체 기본 설정 */
	m_pGrid->SetItem(&stGV);
	m_pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	m_pGrid->SetColumnCount(1);
	m_pGrid->SetBkColor(RGB(255, 255, 255));

	/* 본문 */
	m_pGrid->SetColumnWidth(0, rGrid.Width()-20);

	/* 기본 속성 및 갱신 */
	m_pGrid->SetBaseGridProp(0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->Refresh();

	/* 거버 레시피 적재 */
	LoadFrameRate();

	return TRUE;
}

/*
 desc : 속도 별로 단차 정보 적재
 parm : None
 retn : None
*/
VOID CDlgSped::LoadFrameRate()
{
	INT32 i32RowCnt		= 0;
	TCHAR tzValue[32]	= {NULL};
	LPG_OSSD pstPhStep	= NULL;

	/* 화면 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);
	/* 기존 항목 모두 제거 */
	m_pGrid->DeleteNonFixedRows();

	do {

		/* 항목 가져오기 */
		pstPhStep	= uvEng_PhStep_GetRecipeIndex(i32RowCnt);
		if (!pstPhStep)	break;
		/* Frame Speed 값 존재 유무 */
		if (pstPhStep->frame_rate < 1)	break;

		/* 항목 개수만큼 Row 생성 */
		m_pGrid->SetRowCount(i32RowCnt+1);
		m_pGrid->SetRowHeight(i32RowCnt, 30);
		m_pGrid->SetItemFormat(i32RowCnt, 0, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		swprintf_s(tzValue, 32, L" %.3f", pstPhStep->frame_rate/1000.0f);
		m_pGrid->SetItemText(i32RowCnt, 0, tzValue);
		/* 다음 위치의 항목 적재 */
		i32RowCnt++;

	} while (1);

	/* 오름 차순 정렬 */
	m_pGrid->SortItems(0, TRUE);
	/* 화면 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE, TRUE);
	/* 그리드 컨트롤 갱신 */
	m_pGrid->Invalidate(TRUE);
}


/*
 desc : 현재 발생된 에러 정보 출력
 parm : None
 retn : None
*/
VOID CDlgSped::OnBtnClicked(UINT32 id)
{
	if (id == IDC_SPED_BTN_APPLY)	CMyDialog::EndDialog(IDOK);
	else							CMyDialog::EndDialog(IDCANCEL);
}

/*
 desc : DoModal Override 함수
 parm : None
 retn : 0L
*/
INT_PTR CDlgSped::MyDoModal()
{
	m_dbFrameRate	= 0.0f;

	return DoModal();
}

/*
 desc : Gerber Recipe 항목을 선택한 경우
 parm : nm_hdr	- [in]  Grid Control 정보가 저장된 구조체 포인터
		result	- [in]  fixed value (0)
 retn : None
*/
VOID CDlgSped::OnGridFrameRate(NMHDR *nm_hdr, LRESULT *result)
{
	NM_GRIDVIEW *pstGrid= (NM_GRIDVIEW*)nm_hdr;
	m_dbFrameRate	= m_pGrid->GetItemTextToFloat(pstGrid->iRow, 0);
}
