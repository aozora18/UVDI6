
/*
 desc : Main Dialog
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : 자신의 윈도 ID, 부모 윈도 객체
 retn : None
*/
CDlgMain::CDlgMain(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgMain::IDD, parent)
{
	m_bDrawBG	= 0x01;
	m_hIcon		= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgMain::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;
	/* group box */
	u32StartID	= IDC_MAIN_GRP_RESULT;
	for (i=0; i<6; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Button - Normal */
	u32StartID	= IDC_MAIN_BTN_EXIT;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Checkbox - Normal */
	u32StartID	= IDC_MAIN_CHK_ACAM1;
	for (i=0; i<4; i++)		DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_MAIN_TXT_MEAS_PERIOD_ROWS;
	for (i=0; i<7; i++)		DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Edit - integer */
	u32StartID	= IDC_MAIN_EDT_MEAS_COUNT_ROWS;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_edt_int[i]);
	/* edit - float */
	u32StartID	= IDC_MAIN_EDT_MEAS_PERIOD_ROWS;
	for (i=0; i<5; i++)		DDX_Control(dx, u32StartID+i,	m_edt_flt[i]);
}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_WM_SYSCOMMAND()
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_BTN_EXIT, IDC_MAIN_BTN_SAVE,	OnBtnClick)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MAIN_CHK_ACAM1, IDC_MAIN_CHK_CALI_Y,OnChkClick)
END_MESSAGE_MAP()

/*
 desc : 시스템 명령어 제어
 parm : id		- 선택된 항목 ID 값
		lparam	- ???
 retn : 1 - 성공 / 0 - 실패
*/
VOID CDlgMain::OnSysCommand(UINT32 id, LPARAM lparam)
{
//	switch (id)
//	{
//	}

	CMyDialog::OnSysCommand(id, lparam);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::OnInitDlg()
{
	UINT32 u32Size	= 0, i = 0;

	InitCtrl();		/* 윈도 컨트롤 초기화 */
	InitGrid();		/* 그리드 초기화 */

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgMain::OnExitDlg()
{
	/* Grid Control 메모리 해제*/
	if (m_pGrid)
	{
		m_pGrid->DestroyWindow();
		delete m_pGrid;
	}
	/* XY Calibration Data 메모리 해제 */
	if (m_stCali.pos_xy)	::Free(m_stCali.pos_xy);
	m_stCali.pos_xy	= NULL;
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgMain::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgMain::OnResizeDlg()
{
}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgMain::InitCtrl()
{
	UINT8 i	= 0x00;
#if 0
	UINT32 u32Min[6]	= { 10, 10,  10,  10,    40,    40 };
	UINT32 u32Max[6]	= { 50, 50, 100, 100, 10000, 10000 };
	DOUBLE dbMin[2]		= {   0.0f,   0.0f };
	DOUBLE dbMax[2]		= { 100.0f, 100.0f };
#endif
	/* group box */
	for (i=0; i<6; i++)	m_grp_ctl[i].SetFont(&g_lf);

	/* text - normal */
	for (i=0; i<7; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* button - normal */
	for (i=0; i<4; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
	}
	/* checkbox - normal */
	for (i=0; i<4; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf);
		m_chk_ctl[i].SetBgColor(g_clrBtnColor);
	}
	m_chk_ctl[0].SetCheck(1);
	m_chk_ctl[2].SetCheck(1);
	/* edit - integer */
	for (i=0; i<3; i++)
	{
		m_edt_int[i].SetEditFont(&g_lf);
		m_edt_int[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_int[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_int[i].SetInputType(ENM_DITM::en_int8);
		if (i != 2)	m_edt_int[i].SetReadOnly(TRUE);
	}
	/* edit - float */
	for (i=0; i<5; i++)
	{
		m_edt_flt[i].SetEditFont(&g_lf);
		m_edt_flt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_flt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_flt[i].SetInputType(ENM_DITM::en_float);
	}
	m_edt_flt[4].SetTextToNum(5.0f, 1);
}

/*
 desc : 그리드 컨트롤 생성 - Step XY
 parm : None
 retn : None
*/
VOID CDlgMain::InitGrid()
{
	UINT32 dwStyle	= WS_CHILD | WS_TABSTOP | WS_VISIBLE, i = 0;

	GV_ITEM stGV = { NULL };
	CRect rGrid;

	// 현재 윈도 Client 크기
	m_grp_ctl[0].GetWindowRect(rGrid);
	ScreenToClient(rGrid);
	rGrid.left	+= 15;
	rGrid.right	-= 15;
	rGrid.top	+= 24;
	rGrid.bottom-= 15;

	// 그리드 컨트롤 기본 공통 속성
	stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr= RGB(32, 32, 32);
	/* 객체 생성 */
	m_pGrid	= new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetModifyStyleEx(WS_EX_STATICEDGE);
	m_pGrid->SetDrawScrollBarVert(TRUE);
	m_pGrid->SetDrawScrollBarHorz(TRUE);
	if (!m_pGrid->Create(rGrid, this, IDC_MAIN_GRID_CALI_TOOL, dwStyle))
	{
		delete m_pGrid;
		m_pGrid = NULL;
		return;
	}
	/* 객체 기본 설정 */
	m_pGrid->SetLogFont(g_lf);
	m_pGrid->SetRowCount(1);
	m_pGrid->SetColumnCount(1);
	m_pGrid->SetRowHeight(0, 24);
	m_pGrid->SetColumnWidth(0, 38);
	m_pGrid->SetBkColor(RGB(255, 255, 255));
	m_pGrid->SetFixedColumnSelection(0);

	m_pGrid->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
}

/*
 desc : 일반 버튼 이벤트 처리
 parm : id	- [in]  Click된 버튼 ID
 retn : None
*/
VOID CDlgMain::OnBtnClick(UINT32 id)
{
	switch (id)
	{
	case IDC_MAIN_BTN_EXIT			:	PostMessage(WM_CLOSE, 0, 0L);	break;
	case IDC_MAIN_BTN_DOF_FILE		:	OpenFile();						break;
	case IDC_MAIN_BTN_REFRESH		:	DrawGrid();	DrawData();			break;
	case IDC_MAIN_BTN_SAVE			:	SaveFile();						break;
	}
}

/*
 desc : 일반 체크 이벤트 처리
 parm : id	- [in]  Click된 버튼 ID
 retn : None
*/
VOID CDlgMain::OnChkClick(UINT32 id)
{
	switch (id)
	{
	case IDC_MAIN_CHK_ACAM1		:
	case IDC_MAIN_CHK_ACAM2		:
		m_chk_ctl[0].SetCheck(0);
		m_chk_ctl[1].SetCheck(0);
		m_chk_ctl[id-IDC_MAIN_CHK_ACAM1].SetCheck(1);
		break;
	case IDC_MAIN_CHK_CALI_X	:
	case IDC_MAIN_CHK_CALI_Y	:
		m_chk_ctl[2].SetCheck(0);
		m_chk_ctl[3].SetCheck(0);
		m_chk_ctl[id-IDC_MAIN_CHK_ACAM1].SetCheck(1);
		break;
	}
}

/*
 desc : 현재 그리드 컨트롤에 출력된 값을 Data 파일로 저장 (Camera 1 or Camera 2)
 parm : None
 retn : None
*/
VOID CDlgMain::SaveFile()
{
	UINT8 u8ACamID	= m_chk_ctl[0].GetCheck() == 1 ? 0x01 : 0x02;
	UINT16 i, j, u16Pos;
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL}, tzVal[32] = {NULL}, tzMsg[256] = {NULL};
	DOUBLE dbDiffX	= 0.0f, dbDiffY = 0.0f;
	errno_t eRet	= NULL;
	FILE *fpCali	= {NULL};
	CUniToChar csCnv;

	/* 소재 두께 값이 설정되어 있는지 여부 */
	if (m_edt_int[2].GetTextToNum() < 1)
	{
		AfxMessageBox(L"The material thickness value is not enterend", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 파일명 설정 (acam_cali_thick_acam_no) */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\xy2d\\acam_cali_%04u_%u.dat",
			   g_tzWorkDir, (UINT16)m_edt_int[2].GetTextToNum(), u8ACamID);

	/* 동일한 파일이 있으면 저장하지 못하도록 함 */
	if (uvCmn_FindFile(tzFile))
	{
		if (IDYES != AfxMessageBox(L"The same file exists\n"
								  L"Shoud I overwite the file?", MB_YESNO))	return;
		else
		{
			/* 읽기 전용 파일 무조건 삭제하기 위함 */
			if (!uvCmn_DeleteForceFile(tzFile))
			{
				AfxMessageBox(L"Failed to delete the file", MB_ICONSTOP|MB_TOPMOST);
				return;
			}
		}
	}

	/* Open the file */
	eRet = fopen_s(&fpCali, csCnv.Uni2Ansi(tzFile), "wt");
	if (0 != eRet)
	{
		swprintf_s(tzMsg, 256, L"Failed to open the file (err:%d)", eRet);
		AfxMessageBox(tzMsg, MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 측정 시작 위치 정보 저장 */
	wmemset(tzVal, 0x00, 32);	swprintf_s(tzVal, 32, L"%.4f\n", m_stCali.acam_x/10000.0f);
	fputws(tzVal, fpCali);
	wmemset(tzVal, 0x00, 32);	swprintf_s(tzVal, 32, L"%.4f\n", m_stCali.stage_x/10000.0f);
	fputws(tzVal, fpCali);
	wmemset(tzVal, 0x00, 32);	swprintf_s(tzVal, 32, L"%.4f\n", m_stCali.stage_y/10000.0f);
	fputws(tzVal, fpCali);
	/* 측정하고자 하는 데이터의 간격 (행과 열) 저장 */
	wmemset(tzVal, 0x00, 32);	swprintf_s(tzVal, 32, L"%.4f\n", m_stCali.row_gap/10000.0f);
	fputws(tzVal, fpCali);
	wmemset(tzVal, 0x00, 32);	swprintf_s(tzVal, 32, L"%.4f\n", m_stCali.col_gap/10000.0f);
	fputws(tzVal, fpCali);
	/* 측정하고자 하는 데이터의 개수 (행과 열) 저장 */
	wmemset(tzVal, 0x00, 32);	swprintf_s(tzVal, 32, L"%u\n", m_stCali.row_cnt);
	fputws(tzVal, fpCali);
	wmemset(tzVal, 0x00, 32);	swprintf_s(tzVal, 32, L"%u\n", m_stCali.col_cnt);
	fputws(tzVal, fpCali);

	/* 소재 두께에 따른 X / Y 위치 오차 값 얻기 */
	dbDiffX	= m_edt_flt[2].GetTextToDouble() / 1000.0f;	/* um */
	dbDiffY	= m_edt_flt[3].GetTextToDouble() / 1000.0f;	/* um */

	for (i=0; i<m_stCali.row_cnt; i++)
	{
		for (j=0; j<m_stCali.col_cnt; j++)
		{
			u16Pos	= i * m_stCali.row_cnt + j;
			swprintf_s(tzVal, 32, L"%+.4f,%+.4f\n",
					   m_stCali.pos_xy[u16Pos].x/10000.0f + dbDiffX,
					   m_stCali.pos_xy[u16Pos].y/10000.0f + dbDiffY);
			if (EOF == fputws(tzVal, fpCali))
			{
				AfxMessageBox(L"Failed to save to the calibration file", MB_ICONSTOP|MB_TOPMOST);
				if (0 == eRet)	fclose(fpCali);
				return;
			}
		}
	}

	/* 파일 닫기  */
	if (0 == eRet)	fclose(fpCali);
}

/*
 desc : Calibration 데이터가 저장된 Data 파일 열기
 parm : None
 retn : None
*/
VOID CDlgMain::OpenFile()
{
	TCHAR tzInit[MAX_PATH_LEN]	= {NULL};
	PTCHAR ptzFile	= NULL;

	/* 초기 폴더 설정 */
	ptzFile	= (PTCHAR)::Alloc(sizeof(TCHAR)*MAX_PATH_LEN);
	wmemset(ptzFile, 0x00, MAX_PATH_LEN);
	swprintf_s(tzInit, MAX_PATH_LEN, L"%s\\xy2d", g_tzWorkDir);
	if (uvCmn_GetSelectFile(L"Cali File (*.dat) | *.dat", MAX_PATH_LEN, ptzFile,
							L"XY2D Calibration File", tzInit))
	{
		/* 파일 내용 적재 */
		if (!LoadData(ptzFile))	return;
	}
	::Free(ptzFile);
	/* 그리드 구성 */
	DrawGrid();
	/* 그리드에 데이터 값 설정 */
	DrawData();
}

/*
 desc : DOF Film 정보가 저장된 Data 파일을 그리드에 적재
 parm : file	- [in]  2D Calibration Data File
 retn : TRUE or FALSE
*/
BOOL CDlgMain::LoadData(PTCHAR file)
{
	TCHAR *ptzVal, tzVal[32], tzFile[MAX_PATH_LEN] = {NULL}, *ptzContext;
	INT32 i, i32Total;
	errno_t eRet	= {NULL};
	LPG_I32XY pstXY	= NULL;
	FILE *fpCali	= {NULL};
	CUniToChar csCnv;

	/* XY Calibration Data 메모리 해제 */
	if (m_stCali.pos_xy)	::Free(m_stCali.pos_xy);
	m_stCali.pos_xy	= NULL;

	/* Open the file */
	eRet = fopen_s(&fpCali, csCnv.Uni2Ansi(file), "rt");
	if (0 != eRet)	return FALSE;

	/* Align Camera X : Motion Position */
	wmemset(tzVal, 0x00, 32);	fgetws(tzVal, 32, fpCali);
	m_stCali.acam_x	= (INT32)ROUNDED(_wtof(tzVal) * 10000.0f, 0);
	/* Stage X : Motion Position */
	wmemset(tzVal, 0x00, 32);	fgetws(tzVal, 32, fpCali);
	m_stCali.stage_x	= (INT32)ROUNDED(_wtof(tzVal) * 10000.0f, 0);
	/* Stage Y : Motion Position */
	wmemset(tzVal, 0x00, 32);	fgetws(tzVal, 32, fpCali);
	m_stCali.stage_y	= (INT32)ROUNDED(_wtof(tzVal) * 10000.0f, 0);
	/* 행과 열의 측정 간격 */
	wmemset(tzVal, 0x00, 32);	fgetws(tzVal, 32, fpCali);
	m_stCali.row_gap	= (INT32)ROUNDED(_wtof(tzVal) * 10000.0f, 0);
	wmemset(tzVal, 0x00, 32);	fgetws(tzVal, 32, fpCali);
	m_stCali.col_gap	= (INT32)ROUNDED(_wtof(tzVal) * 10000.0f, 0);
	/* 행과 열의 측정 개수 */
	wmemset(tzVal, 0x00, 32);	fgetws(tzVal, 32, fpCali);
	m_stCali.row_cnt	= (UINT32)_wtoi(tzVal);
	wmemset(tzVal, 0x00, 32);	fgetws(tzVal, 32, fpCali);
	m_stCali.col_cnt	= (UINT32)_wtof(tzVal);

	/* 메모리 할당 */
	i32Total		= UINT32(m_stCali.col_cnt) * UINT32(m_stCali.row_cnt);
	m_stCali.pos_xy= (LPG_I32XY)::Alloc(i32Total * sizeof(STG_I32XY));
	ASSERT(m_stCali.pos_xy);
	memset(m_stCali.pos_xy, 0x00, sizeof(STG_I32XY) * i32Total);
	/* 오차 값이 저장될 구조체 포인터 연결 */
	pstXY	= m_stCali.pos_xy;
	/* 각 개별 카메라의 Calibration 위치 값 얻기 */
	for (i=0; i<i32Total; i++)
	{
		wmemset(tzVal, 0x00, 32);
		/* 한줄씩 읽기 (읽어들이고자 하는 문자의 수 + 1 (/n; 개행문자 때문에) */
		if (!fgetws(tzVal, 32, fpCali))	break;
		/* 아무런 데이터가 존재하지 않는는지 확인 */
		if (wcslen(tzVal) < 1)	break;
		/* Align Camera X, Stage Y 단차 값 저장 */
		ptzVal		= wcstok_s(tzVal, L",", &ptzContext);
		pstXY[i].x	= (INT32)ROUNDED(_wtof(ptzVal) * 10000.0f, 0);
		ptzVal		= wcstok_s(NULL, L",", &ptzContext);
		pstXY[i].y	= (INT32)ROUNDED(_wtof(ptzVal) * 10000.0f, 0);

		/* 파일의 끝까지 읽어들였는지 */
		if (feof(fpCali))	break;
	}

	/* 정상적으로 모두 읽어들였는지 여부 */
	return (i == i32Total);
}

/*
 desc : 그리드 영역 재설계
 parm : None
 retn : None
*/
VOID CDlgMain::DrawGrid()
{
	UINT8 i;
	GV_ITEM stGV= { NULL };

	/* 현재 행과 열의 이동 거리 */
	m_edt_flt[0].SetTextToNum(m_stCali.row_gap/10000.0f, 4);
	m_edt_flt[1].SetTextToNum(m_stCali.col_cnt/10000.0f, 4);
	/* 현재 행과 열의 개수 설정 */
	m_edt_int[0].SetTextToNum(m_stCali.row_cnt);
	m_edt_int[1].SetTextToNum(m_stCali.col_cnt);
	/* 행과 열의 개수가 최소 2개 이상은 있어야 됨 */
	if (m_stCali.row_cnt < 2 || m_stCali.col_cnt < 2)
	{
		AfxMessageBox(L"There must be at least two rows and columns", MB_ICONSTOP|MB_TOPMOST);
		return;
	}

	/* 그리드 영역 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);
	/* 데이터 영역 모두 제거 */
	m_pGrid->DeleteNonFixedRows();
	
	/* 행과 열의 개수 설정 */
	m_pGrid->SetRowCount(m_stCali.row_cnt+1);
	m_pGrid->SetColumnCount(m_stCali.col_cnt+1);

	/* 타이틀 설정 */
	stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr= RGB(32, 32, 32);
	stGV.nFormat= DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr= RGB(214, 214, 214);
	stGV.row	= 0;
	stGV.col	= 0;
	stGV.strText= L"X / Y";
	stGV.lfFont	= g_lf;
	m_pGrid->SetItem(&stGV);
	m_pGrid->SetColumnWidth(0, 30);

	/* 타이틀 행과 열 설정 */
	for (i=1,stGV.col=0; i<=m_stCali.row_cnt; i++)
	{
		stGV.row	= i;
		stGV.strText.Format(L"%02u", i);
		m_pGrid->SetItem(&stGV);
		m_pGrid->SetRowHeight(i, 22);
	}
	for (i=1,stGV.row=0; i<=m_stCali.col_cnt; i++)
	{
		stGV.col	= i;
		stGV.strText.Format(L"%02u", i);
		m_pGrid->SetItem(&stGV);
		m_pGrid->SetColumnWidth(i, 45);
	}

	/* 그리드 영역 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate(FALSE);
}

/*
 desc : 그리드 영역에 값 설정
 parm : None
 retn : None
*/
VOID CDlgMain::DrawData()
{
	UINT8 u8XY		= m_chk_ctl[2].GetCheck() == 1 ? 0x00 : 0x01;
	UINT16 i, j, u16Pos;
	DOUBLE dbPosXY	= 0.0f, dbDiffXY = 0.0f;
	TCHAR tzVal[32]	= { NULL };
	GV_ITEM stGV	= { NULL };

	/* Offset XY 값 적용 */
	if (0x00 == u8XY)	dbDiffXY = m_edt_flt[2].GetTextToDouble();
	else				dbDiffXY = m_edt_flt[3].GetTextToDouble();

	/* 본문 행과 열 설정 */
	stGV.mask	= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr= RGB(16, 16, 16);
	stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr= RGB(255, 255, 255);

	/* 그리드 영역 갱신 비활성화 */
	m_pGrid->SetRedraw(FALSE);

	for (i=1; i<=m_stCali.row_cnt; i++)
	{
		for (j=1; j<=m_stCali.col_cnt; j++)
		{
			stGV.row= i;
			stGV.col= j;
			u16Pos	= (i-1) * m_stCali.col_cnt + (j-1);
			
			wmemset(tzVal, 0x00, 32);
			if (0x00 == u8XY)	dbPosXY	= (m_stCali.pos_xy[u16Pos].x / 10.0f) + dbDiffXY;
			else				dbPosXY	= (m_stCali.pos_xy[u16Pos].y / 10.0f) + dbDiffXY;

			if (abs(dbPosXY) > m_edt_flt[4].GetTextToDouble())
			{
				stGV.crFgClr	= RGB(255, 128, 0);
			}
			else
			{
				stGV.crFgClr	= RGB(0, 0, 0);
			}
			swprintf_s(tzVal, 32, L"%+.1f", dbPosXY);
			stGV.strText	= tzVal;
			m_pGrid->SetItem(&stGV);
		}
	}

	/* 그리드 영역 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate(FALSE);
}