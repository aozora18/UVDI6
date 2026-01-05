// DlgStepOffsetCalc.cpp: 구현 파일
//

#include "pch.h"
//#include "afxdialogex.h"
//#include "../../../pops/DlgParam.h"
#include "../../../MainApp.h"
#include "DlgStepOffsetCalc.h"


/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgStepOffsetCalc::CDlgStepOffsetCalc(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgStepOffsetCalc::IDD, parent)
{

}

//CDlgStepOffsetCalc::~CDlgStepOffsetCalc()
//{
//
//}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgStepOffsetCalc::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;



	u32StartID = IDC_CALB_EXPOSURE_STEPOFFSET_BTN_LOAD;
	for (i = 0; i < eCALB_EXPOSURE_STEPOFFSET_BTN_MAX; i++)		DDX_Control(dx, u32StartID + i, m_btn_ctl[i]);

	u32StartID = IDC_CALB_EXPOSURE_STEPOFFSET_GRD_PH_STEP;
	for (i = 0; i < eCALB_EXPOSURE_STEPOFFSET_GRD_MAX; i++)		DDX_Control(dx, u32StartID + i, m_grd_ctl[i]);

}

BEGIN_MESSAGE_MAP(CDlgStepOffsetCalc, CMyDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CALB_EXPOSURE_STEPOFFSET_BTN_LOAD, &CDlgStepOffsetCalc::OnBnClickedCalbExposureStepoffsetBtnLoad)
	ON_BN_CLICKED(IDC_CALB_EXPOSURE_STEPOFFSET_BTN_SAVE, &CDlgStepOffsetCalc::OnBnClickedCalbExposureStepoffsetBtnSave)
	ON_NOTIFY(NM_CLICK, IDC_CALB_EXPOSURE_STEPOFFSET_GRD_PH_STEP, &CDlgStepOffsetCalc::OnClickGridOption)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgStepOffsetCalc::PreTranslateMessage(MSG* msg)
{

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgStepOffsetCalc::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();
	InitCalcGrid();

	return TRUE;
}


/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgStepOffsetCalc::OnExitDlg()
{
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgStepOffsetCalc::OnPaintDlg(CDC * dc)
{

}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgStepOffsetCalc::OnResizeDlg()
{
}

/*
 desc : DoModal Override 함수
 parm : None
 retn : 0L
*/
INT_PTR CDlgStepOffsetCalc::MyDoModal()
{
	return DoModal();
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgStepOffsetCalc::InitCtrl()
{
	INT32 i;
	PTCHAR pText = NULL;

	for (int i = 0; i < eCALB_EXPOSURE_STEPOFFSET_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

	}

	for (int i = 0; i < eCALB_EXPOSURE_STEPOFFSET_GRD_MAX; i++)
	{
		m_grd_ctl[i].SetParent(this);
	}

	return TRUE;
}

void CDlgStepOffsetCalc::InitCalcGrid()
{

	CResizeUI	clsResizeUI;
	CRect		rGrid;

	// 이미지와 타이틀 개수를 고려하여 컬럼 수 설정 (PH 1~5까지 표시 시 14, PH 6까지 표시 시 16)
	int nColMaxCnt = 14;

	std::vector <CString> vTitlenRow = { _T("NO"), _T("PH X 2"), _T("PH Y 2"), _T("PH X 3"), _T("PH Y 3"), _T("PH X 4"), _T("PH Y 4"), _T("PH X 5"), _T("PH Y 5"), _T("PH X 6"), _T("PH Y 6") };
	std::vector <CString> vTitlennCol = { _T(""), _T("RESULT"), _T("="), _T("PH ORG"), _T(" "), _T("PH 1"), _T(" "), _T("PH 2"), _T(" "), _T("PH 3"), _T(" "), _T("PH 4"), _T(" "), _T("PH 5"), _T(" "), _T("PH 6") };

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_STEPOFFSET_GRD_PH_STEP];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	// 스크롤바 너비를 제외한 실제 사용 가능한 너비 계산
	int nScrollWidth = ::GetSystemMetrics(SM_CXVSCROLL);
	int nAvailableWidth = rGrid.Width() - nScrollWidth - 4; // 테두리 여유분 제외

	/* 1. 기본 너비 설정 (고정 크기 지정) */
	// 기호 컬럼(=, +)은 좁게(30), 데이터 컬럼은 기본 넓게(80)
	std::vector<int> vWidths(nColMaxCnt, 80);
	int nFixedWidthSum = 0;
	int nDynamicColCount = 0;

	for (int i = 0; i < nColMaxCnt; i++)
	{
		if (i == 2 || (i >= 4 && i % 2 == 0)) // =, 그리고 PH 기호 컬럼들
		{
			vWidths[i] = 30;
			nFixedWidthSum += 30;
		}
		else if (i == 0) // NO 컬럼
		{
			vWidths[i] = 70;
			nFixedWidthSum += 70;
		}
		else // RESULT, PH ORG, PH 1~5 값 컬럼
		{
			nDynamicColCount++;
		}
	}

	/* 2. 잔여 너비 배분 */
	// 사용 가능 너비에서 고정 너비를 뺀 나머지를 데이터 컬럼(Dynamic)에 배분
	int nRemainedWidth = nAvailableWidth - nFixedWidthSum;
	int nAllocWidth = (nDynamicColCount > 0) ? (nRemainedWidth / nDynamicColCount) : 80;

	for (int i = 0; i < nColMaxCnt; i++)
	{
		if (i != 0 && i != 2 && !(i >= 4 && i % 2 == 0))
		{
			vWidths[i] = nAllocWidth;
		}
	}

	/* 그리드 기본 설정 */
	int nHeight = (int)((rGrid.Height() - ::GetSystemMetrics(SM_CYHSCROLL) - 2) / ePHSTEP_ROW_MAX);
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->SetGridLineColor(BLACK_);
	pGrid->SetRowCount(ePHSTEP_ROW_MAX);
	pGrid->SetColumnCount(nColMaxCnt);
	pGrid->SetFixedRowCount(1);
	pGrid->SetFixedColumnCount(1);
	pGrid->SetFixedRowSelection(FALSE);
	pGrid->SetFixedColumnSelection(FALSE);

	for (int nRow = 0; nRow < ePHSTEP_ROW_MAX; nRow++)
	{
		pGrid->SetRowHeight(nRow, nHeight);
		if (nRow < (int)vTitlenRow.size()) pGrid->SetItemText(nRow, 0, vTitlenRow[nRow]);

		for (int nCol = 0; nCol < nColMaxCnt; nCol++)
		{
			// 타이틀 및 배경색 설정
			if (nRow == 0 || nCol == 0)
			{
				pGrid->SetItemBkColour(nRow, nCol, ALICE_BLUE);
				if (nRow == 0 && nCol < (int)vTitlennCol.size())
					pGrid->SetItemText(0, nCol, vTitlennCol[nCol]);
			}
			else
			{
				pGrid->SetItemBkColour(nRow, nCol, WHITE_);
			}

			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pGrid->SetColumnWidth(nCol, vWidths[nCol]);
		}
	}

	CString strText;
	for (int nRow = ePHSTEP_ROW_NO; nRow < ePHSTEP_ROW_MAX; nRow++)
	{
		strText.Format(_T("="));
		pGrid->SetItemText(nRow, 2, strText);
	}

	/* 환경 파일에 광학계 단차 값 저장 */
	DOUBLE ph_offset_x[MAX_PH];
	DOUBLE ph_offset_y[MAX_PH];

	for (int nRow = ePHSTEP_ROW_NO+1; nRow < ePHSTEP_ROW_MAX/2+1; nRow++)
	{
		ph_offset_x[nRow] = uvEng_GetConfig()->luria_svc.ph_offset_x[nRow];
		ph_offset_y[nRow] = uvEng_GetConfig()->luria_svc.ph_offset_y[nRow];

		//1, 3, 5, 7, 9
		strText.Format(_T("%.3f"), ph_offset_x[nRow]);
		pGrid->SetItemText(nRow*2-1, 3, strText);
		//2, 4, 6, 8
		strText.Format(_T("%.3f"), ph_offset_y[nRow]);
		pGrid->SetItemText(nRow*2, 3, strText);
	}


	/* 기본 속성 및 갱신 */
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}

void CDlgStepOffsetCalc::OnBnClickedCalbExposureStepoffsetBtnLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog dlg(TRUE, _T("csv"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("Excel CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||"), this);

	if (dlg.DoModal() == IDOK)
	{
		CString strPath = dlg.GetPathName();
		if (LoadExcelData(strPath))
		{
			CalculateOffsets(); // 로드 성공 시 바로 계산 및 그리드 업데이트
			AfxMessageBox(_T("Data Loaded and Calculated Successfully."));
		}
	}
}

void CDlgStepOffsetCalc::OnBnClickedCalbExposureStepoffsetBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (AfxMessageBox(_T("계산된 데이터를 시스템에 적용하시겠습니까?"), MB_YESNO) == IDNO) return;

	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_STEPOFFSET_GRD_PH_STEP];

	for (int i = 2; i <= 6; i++)
	{
		double dFinalX = _ttof(pGrid->GetItemText((i - 1) * 2 + 1, 1));
		double dFinalY = _ttof(pGrid->GetItemText((i - 1) * 2 + 2, 1));

		// 실제 엔진 설정 구조체에 저장
		uvEng_GetConfig()->luria_svc.ph_offset_x[i] = dFinalX;
		uvEng_GetConfig()->luria_svc.ph_offset_y[i] = dFinalY;
	}

	// 설정 파일 실제 저장 명령 (엔진 함수 호출)
	 uvEng_SaveConfig(); 

	AfxMessageBox(_T("저장이 완료되었습니다."));
}


// 간단한 CSV 파싱 예시 (실제 Excel .xlsx라면 별도 라이브러리나 ODBC 필요)
BOOL CDlgStepOffsetCalc::LoadExcelData(CString strPath)
{
	CStdioFile file;
	// 파일 열기 시도
	if (!file.Open(strPath, CFile::modeRead | CFile::typeText)) return FALSE;

	CString strLine;
	int nRow = 0;

	// 한 줄씩 읽기
	while (file.ReadString(strLine))
	{
		nRow++;
		if (nRow < 2) continue; // 첫 번째 줄(헤더)은 건너뜁니다.

		// PH 1부터 6까지 루프를 돌며 데이터 추출
		for (int i = 1; i <= 6; i++)
		{
			// 엑셀 위치 기준: PH1 No4는 E열(인덱스 4), PH2 No4는 I열(인덱스 8)...
			// targetCol 공식: (i-1) * 4 + 4
			int targetCol = (i - 1) * 4 + 4;

			CString strValue;
			// AfxExtractSubString(결과문자열, 원본문자열, 인덱스, 구분자)
			// 성공적으로 해당 컬럼 데이터를 가져온 경우
			if (AfxExtractSubString(strValue, strLine, targetCol, ','))
			{
				strValue.Trim(); // 공백 제거
				if (nRow == 2) m_dStepX[i] = _ttof(strValue) / 1000.0f; // 2행은 STEP X
				if (nRow == 3) m_dStepY[i] = _ttof(strValue) / 1000.0f; // 3행은 STEP Y
			}
		}

		// 3행(STEP Y)까지 읽었다면 루프를 종료해도 됩니다.
		if (nRow >= 3) break;
	}

	file.Close();
	return TRUE;
}

void CDlgStepOffsetCalc::CalculateOffsets()
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_STEPOFFSET_GRD_PH_STEP];
	CString strVal, strRes, strSignX, strSignY;

	// 1. PH ORG 값 가져오기 (3번 열)
	double dOrgX[7] = { 0, }, dOrgY[7] = { 0, };
	for (int i = 2; i <= 6; i++) {
		dOrgX[i] = _ttof(pGrid->GetItemText((i - 1) * 2 - 1, 3)); // PH X n
		dOrgY[i] = _ttof(pGrid->GetItemText((i - 1) * 2, 3));     // PH Y n
	}

	// 2. 결과값 계산 및 그리드 채우기
	for (int i = 2; i <= 6; i++)
	{
		// 실제 그리드 행 번호 계산: PH X 2(1행), PH Y 2(2행)...
		int nRowX = (i - 2) * 2 + 1;
		int nRowY = (i - 2) * 2 + 2;

		// PH ORG 값 읽기 (3번 컬럼)
		double dResX = _ttof(pGrid->GetItemText(nRowX, 3));
		double dResY = _ttof(pGrid->GetItemText(nRowY, 3));

		for (int stepIdx = 1; stepIdx < i; stepIdx++) {
			double valX = m_dStepX[stepIdx];
			double valY = m_dStepY[stepIdx];

			// 이미지 패턴 적용: 
			// 홀수 Step(1, 3, 5): X는 +, Y는 -
			// 짝수 Step(2, 4): X는 -, Y는 +
			if (stepIdx % 2 == 1) { // 홀수 Step
				dResX += valX; strSignX = _T("+");
				dResY -= valY; strSignY = _T("-");
			}
			else { // 짝수 Step
				dResX -= valX; strSignX = _T("-");
				dResY += valY; strSignY = _T("+");
			}

			// 그리드 컬럼 위치 계산: 기호(4, 6, 8...), 숫자(5, 7, 9...)
			int nColSign = 4 + (stepIdx - 1) * 2;
			int nColVal = 5 + (stepIdx - 1) * 2;

			// X 행 업데이트
			pGrid->SetItemText(nRowX, nColSign, strSignX);
			strVal.Format(_T("%.4f"), valX); // Format을 먼저 호출해야 함
			pGrid->SetItemText(nRowX, nColVal, strVal);

			// Y 행 업데이트 (기존에 누락된 부분)
			pGrid->SetItemText(nRowY, nColSign, strSignY);
			strVal.Format(_T("%.4f"), valY);
			pGrid->SetItemText(nRowY, nColVal, strVal);
		}

		// 최종 RESULT (1번 컬럼) 업데이트
		strRes.Format(_T("%.4f"), dResX);
		pGrid->SetItemText(nRowX, 1, strRes);
		strRes.Format(_T("%.4f"), dResY);
		pGrid->SetItemText(nRowY, 1, strRes);
	}
	pGrid->Refresh();
}




void CDlgStepOffsetCalc::OnClickGridOption(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	CGridCtrl* pGrid = &m_grd_ctl[eCALB_EXPOSURE_STEPOFFSET_GRD_PH_STEP];
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	ENM_DITM enType;
	UINT8 u8DecPts = 0;

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	//if (eOPTION_COL_VALUE != pItem->iColumn)
	//{
	//	return;
	//}
	//else if (TRUE == CFlushErrorMgr::GetInstance()->IsProcessWorking())
	//{
	//	AfxMessageBox(_T("현재 측정중입니다."), MB_ICONSTOP | MB_TOPMOST);
	//	return;
	//}

	CString strOutput;
	CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);
	double	dMin = -9999.9999;
	double	dMax = 9999.9999;

	//switch (pItem->iRow)
	//{
	//case eOPTION_ROW_STAGE_X:
	//case eOPTION_ROW_STAGE_Y:
	//case eOPTION_ROW_CAM_X:
	//	enType = ENM_DITM::en_double;
	//	u8DecPts = 4;
	//	break;

	//case eOPTION_ROW_EXPOSE_ROUND:
	//	dMin = 1;
	//	dMax = 30;
	//	enType = ENM_DITM::en_int16;
	//	u8DecPts = 1;
	//	break;

	//case eOPTION_ROW_STAGE_Y_PLUS:
	//	dMin = 1;
	//	dMax = 500;
	//	enType = ENM_DITM::en_double;

	//default:
	//	return;
	//}

	if ((pItem->iColumn % 2 == 0) || pItem->iRow == 0)
	{
		//부등호
	}
	else
	{
		enType = ENM_DITM::en_double;

		if (PopupKBDN(enType, strInput, strOutput, dMin, dMax, u8DecPts))
		{
			pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
		}
	}


	pGrid->Refresh();
}


BOOL CDlgStepOffsetCalc::PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts/* = 0*/)
{
	TCHAR tzTitle[1024] = { NULL }, tzPoint[512] = { NULL }, tzMinMax[2][32] = { NULL };
	CDlgKBDN dlg;

	wcscpy_s(tzMinMax[0], 32, L"Min");
	wcscpy_s(tzMinMax[1], 32, L"Max");

	switch (enType)
	{
	case ENM_DITM::en_int8:
	case ENM_DITM::en_int16:
	case ENM_DITM::en_int32:
	case ENM_DITM::en_int64:
	case ENM_DITM::en_uint8:
	case ENM_DITM::en_uint16:
	case ENM_DITM::en_uint32:
	case ENM_DITM::en_uint64:
		swprintf_s(tzPoint, 512, L"%s:%%.%df,%s:%%.%df", tzMinMax[0], u8DecPts, tzMinMax[1], u8DecPts);
		swprintf_s(tzTitle, 1024, tzPoint, dMin, dMax);
		if (IDOK != dlg.MyDoModal(tzTitle, FALSE, TRUE, dMin, dMax))	return FALSE;	break;
	case ENM_DITM::en_float:
	case ENM_DITM::en_double:
		swprintf_s(tzPoint, 512, L"%s:%%.%df,%s:%%.%df", tzMinMax[0], u8DecPts, tzMinMax[1], u8DecPts);
		swprintf_s(tzTitle, 1024, tzPoint, dMin, dMax);
		if (IDOK != dlg.MyDoModal(tzTitle, TRUE, TRUE, dMin, dMax))	return FALSE;	break;
	}
	switch (enType)
	{
	case ENM_DITM::en_int8:
	case ENM_DITM::en_int16:
	case ENM_DITM::en_int32:
	case ENM_DITM::en_int64:
	case ENM_DITM::en_uint8:
	case ENM_DITM::en_uint16:
	case ENM_DITM::en_uint32:
	case ENM_DITM::en_uint64:
		strOutput.Format(_T("%d"), dlg.GetValueInt32());
		break;
	case ENM_DITM::en_float:
	case ENM_DITM::en_double:
		swprintf_s(tzPoint, 512, L"%%.%df", u8DecPts);
		strOutput.Format(tzPoint, dlg.GetValueDouble());
	}

	return TRUE;
}