
/*
 desc : Align Camera - Edge Find 결과
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgFind.h"

#include "../Meas/Measure.h"

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
CDlgFind::CDlgFind(UINT32 id, CWnd* parent)
	: CDlgMenu(id, parent)
{
	m_pGrid	= NULL;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgFind::~CDlgFind()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgFind::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Button */
	u32StartID	= IDC_FIND_BTN_REFRESH;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgFind, CDlgMenu)
	ON_BN_CLICKED(IDC_MENU_GRID_EDGE_RESULT,	OnGridClicked)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_FIND_BTN_REFRESH, IDC_FIND_BTN_SAVE, OnBnClicked)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgFind::PreTranslateMessage(MSG* msg)
{

	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgFind::OnInitDlg()
{
	if (!CDlgMenu::OnInitDlg())	return FALSE;

	/* 컨트롤 초기화 */
	InitCtrl();
	InitGrid();

	/* 상위 객체 호출 */
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgFind::OnExitDlg()
{
	CDlgMenu::OnExitDlg();

	/* GridCtrl 해제 */
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
VOID CDlgFind::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgFind::OnResizeDlg()
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgFind::InitCtrl()
{
	INT32 i;

	/* Button - normal */
	for (i=0; i<3; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}
}
/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgFind::InitGrid()
{
	TCHAR tzCols[7][16]	= { L"Z (mm)", L"Strength", L"Length", L"Diameter", L"Files", L"Find", L"Rep" }, tzRows[32] = {NULL};
	INT32 i32Width[7]	= { 60, 85, 75, 80, 70, 40, 35 }, i, j=1;
	INT32 i32ColCnt		= 7, i32RowCnt = 0;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* Last Recv. Packet / PDO Header / PDO Data (Values) */
	::GetWindowRect(m_hWnd, rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 5;
	rGrid.right	-= 5;
	rGrid.top	+= 5;
	rGrid.bottom-= 60;

	/* 그리드 컨트롤 기본 공통 속성 */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* 객체 생성 */
	m_pGrid	= new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetDrawScrollBarHorz(FALSE);
	m_pGrid->SetDrawScrollBarVert(TRUE);
	if (!m_pGrid->Create(rGrid, this, IDC_MENU_GRID_EDGE_RESULT, dwStyle))
	{
		delete m_pGrid;
		m_pGrid = NULL;
		return;
	}
	/* 객체 기본 설정 */
	m_pGrid->SetLogFont(g_lf);
	m_pGrid->SetRowCount(i32RowCnt+1);
	m_pGrid->SetColumnCount(i32ColCnt);
	m_pGrid->SetFixedRowCount(1);
	m_pGrid->SetRowHeight(0, 25);
	m_pGrid->SetFixedColumnCount(0);
	m_pGrid->SetBkColor(RGB(255, 255, 255));
	m_pGrid->SetFixedColumnSelection(0);

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
		m_pGrid->SetItem(&stGV);
		m_pGrid->SetColumnWidth(i, i32Width[i]);
	}

	m_pGrid->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->UpdateGrid();
}

/*
 desc : 현재 윈도 컨트롤 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgFind::UpdateCtrl()
{
	UINT8 i	= 0x00;
	BOOL bEnable	= m_pDlgMain->IsRunFocus();
	/* 버튼 컨트롤 */
	for (i=0x00; i<3; i++)	m_btn_ctl[i].EnableWindow(!bEnable);
}

/*
 desc : GridCtrl 클릭 이벤트 발생된 경우
 parm : None
 retn : None
*/
VOID CDlgFind::OnGridClicked()
{
	TCHAR tzFile[MAX_FILE_LEN]	= {NULL};
	CMeasure *pMeasure	= m_pDlgMain->GetMeasure();

	/* 현재 선택된 행과 열의 번호 얻기 */
	PPOINT ptPos	= m_pGrid->GetSelPos();
	if (!ptPos)	return;

	/* 4 번째 컬럼의 파일 이름 얻기 */
	swprintf_s(tzFile, MAX_FILE_LEN, L"%s", m_pGrid->GetItemText(ptPos->x, 4).GetBuffer());
	if (wcslen(tzFile) > 0)	pMeasure->SelectEdgeFile(tzFile);
}

/*
 desc : 기존 출력된 데이터 모두 제거
 parm : None
 retn : None
*/
VOID CDlgFind::ResetData()
{
	/* 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);

	/* 타이틀 부분만 제외하고 모두 제거 */
	m_pGrid->DeleteNonFixedRows();

	/* 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate(FALSE);
}

/*
 desc : 버튼 이벤트 ID
 parm : id	- [in]  버튼 컨트롤 ID
 retn : None
*/
VOID CDlgFind::OnBnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_FIND_BTN_REFRESH	:	RefreshData();	break;
	case IDC_FIND_BTN_FIND		:	GetEdgeData();	break;
	case IDC_FIND_BTN_SAVE		:	SaveToOpen();	break;
	}
}

/*
 desc : 현재까지 측정된 결과 정보 가져오기
 parm : None
 retn : None
*/
VOID CDlgFind::RefreshData()
{
	TCHAR tzValue[128]	= {NULL};
	INT32 i32Item		= 0;
	UINT32 i, u32Count	= 0, u32Format	= DT_VCENTER | DT_SINGLELINE;;
	LPG_ZAAL pstData	= NULL;
	CMeasure *pMeasure	= m_pDlgMain->GetMeasure();

	/* 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);

	/* 타이틀 부분만 제외하고 모두 제거 */
	m_pGrid->DeleteNonFixedRows();

	/* 현재 등록된 개수 정보 얻기 */
	u32Count = pMeasure->GetCount();
	for (i=0; i<u32Count; i++)
	{
		pstData	= m_pDlgMain->GetGrabData(i);
		if (!pstData)	break;

		/* Z 축 높이 값 */
		i32Item	= m_pGrid->GetRowCount();
		m_pGrid->InsertRow(i32Item);
		m_pGrid->SetItemDouble(i32Item, 0, pstData->z_pos / 10000.0f, 4);
		m_pGrid->SetItemFormat(i32Item, 0, u32Format|DT_RIGHT);

		/* Strength */
		wmemset(tzValue, 0x00, 128);
		swprintf_s(tzValue, 128, L"%.3f", pstData->avg_value[0]);
//		swprintf_s(tzData, 128, L"%s (%.2f)", tzValue, pstData->std_value[0]);
		m_pGrid->SetItemText(i32Item, 1, tzValue);
		m_pGrid->SetItemFormat(i32Item, 1, u32Format|DT_RIGHT);
		if (pstData->set_value[0])	m_pGrid->SetItemBkColour(i32Item, 1, RGB(224, 224, 244));

		/* Length */
		wmemset(tzValue, 0x00, 128);
		swprintf_s(tzValue, 128, L"%.3f", pstData->avg_value[1]);
//		swprintf_s(tzData, 128, L"%s (%.3f)", tzValue, pstData->std_value[1]);
		m_pGrid->SetItemText(i32Item, 2, tzValue);
		m_pGrid->SetItemFormat(i32Item, 2, u32Format|DT_RIGHT);
		if (pstData->set_value[1])	m_pGrid->SetItemBkColour(i32Item, 2, RGB(224, 224, 244));

		/* Feret */
		wmemset(tzValue, 0x00, 128);
		swprintf_s(tzValue, 128, L"%.3f", pstData->avg_value[2]);
//		swprintf_s(tzData, 128, L"%s (%.3f)", tzValue, pstData->std_value[2]);
		m_pGrid->SetItemText(i32Item, 3, tzValue);
		m_pGrid->SetItemFormat(i32Item, 3, u32Format|DT_RIGHT);
		if (pstData->set_value[2])	m_pGrid->SetItemBkColour(i32Item, 3, RGB(224, 224, 244));

		m_pGrid->SetItemText(i32Item, 4, pstData->file);
		m_pGrid->SetItemFormat(i32Item, 4, u32Format|DT_CENTER);

		m_pGrid->SetItemText(i32Item, 5, (INT32)pstData->find_count);
		m_pGrid->SetItemFormat(i32Item, 5, u32Format|DT_RIGHT);

		m_pGrid->SetItemText(i32Item, 6, (UINT32)pstData->arrValue[0].GetCount());
		m_pGrid->SetItemFormat(i32Item, 6, u32Format|DT_CENTER);
	}

	/* 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate(FALSE);
}

/*
 desc : 엣지 데이터 측정 (테스트)
 parm : None
 retn : None
*/
VOID CDlgFind::GetEdgeData()
{
	UINT8 u8ACamNo		= m_pDlgMain->GetCheckACam();
	UINT64 u64Tick		= 0;
	LPG_EDFR pstResult	= NULL;
	CMeasure *pMeasure	= m_pDlgMain->GetMeasure();

	/* 선택된 카메라가 없는지 확인 */
	if (u8ACamNo < 1)	return;

	/* Trigger 정보 갱신 */
	m_pDlgMain->UpdateTrigParam();

	/* 기존 Grabbed Image 결과 초기화 */
	uvEng_Camera_ResetGrabbedImage();
	/* 얼라인 카메라 모드 설정 */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_edge_mode);
	/* 트리거 1개 발생 */
	if (!uvEng_Trig_ReqTrigOutOne(u8ACamNo, FALSE))
	{
		AfxMessageBox(L"Failed to generate the trigger event", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 현재 결과 수집 시간 대기 위해 */
	u64Tick	= GetTickCount64();

	/* 일정 시간 동안 수집된 데이터가 있는지 확인 */
	do {
		/* 약 3초 동안 응답이 없으면 빠져 나가기 */
#ifdef _DEBUG
		if (GetTickCount64() > u64Tick + 10000)	break;
#else
		if (GetTickCount64() > u64Tick + 3000)	break;
#endif
		/* 엣지 감지 처리 수행 (감지된 이미지 없으면 리턴) */
		if (uvEng_Camera_RunEdgeDetect(u8ACamNo))
		{
			/* 엣지 존재 여부 */
			pstResult	= uvEng_Camera_GetEdgeDetectResults(u8ACamNo);
			break;	/* 바로 빠져나가기 */
		}

	} while (1);
	
	/* 검색된 엣지 없는 경우라면 ... */
	if (!pstResult)
	{
		AfxMessageBox(L"Failed to find the edge detection", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 엣지 데이터 분석 */
	if (!pMeasure->SetResult(u8ACamNo))
	{
		AfxMessageBox(L"Failed to analyze edge detection", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	/* 가장 최근 파일 선택하도록 함 */
	pMeasure->SelectLastEdgeFile();

	/* 결과 데이터 출력 */
	RefreshData();
}

/*
 desc : 그리드 컨트롤에 있는 데이터를 파일로 저장
 parm : None
 retn : None
*/
VOID CDlgFind::SaveToOpen()
{
	TCHAR tzTempFile[MAX_PATH_LEN]	= {NULL};
	SYSTEMTIME stTm	= {NULL};

	/* 등록된 데이터가 없는지 확인 */
	if (m_pGrid->GetRowCount() < 2)	return;

	/* 현재 시간 */
	GetLocalTime(&stTm);
	/* 윈도 임시 파일로 저장 후 열기 */
	swprintf_s(tzTempFile, MAX_PATH_LEN, L"%s\\vdof\\vdof_%02u%02u%02u_%02u%02u%02u.csv",
			   g_tzWorkDir, stTm.wYear-2000, stTm.wMonth, stTm.wDay,
			   stTm.wHour, stTm.wMinute, stTm.wSecond);

	/* 파일로 저장 */
	m_pGrid->Save(tzTempFile);

	/* 저장된 파일 열기 */
	ShellExecute(NULL, _T("open"), _T("excel.exe"), tzTempFile, NULL, SW_SHOW);
}