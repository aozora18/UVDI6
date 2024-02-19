// DlgCalb.cpp: 구현 파일
//

#include "pch.h"
#include "DlgConfTeach.h"
#include "afxdialogex.h"
#include <locale.h>
#include "../../mesg/DlgMesg.h"

#define DEF_UI_MAX_TEACH_ROW 15
/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgConfTeach::CDlgConfTeach(UINT32 id, CWnd* parent)
	: CDlgSubMenu(id, parent)
{
	m_enDlgID = ENG_CMDI_SUB::en_menu_sub_conf_teach;
	m_pDlgMenu = NULL;

	m_nMaxPage = EN_SETTING_TEACH::MAX / DEF_UI_MAX_TEACH_ROW + 1;
	m_nSelectPage = 0;
}

CDlgConfTeach::~CDlgConfTeach()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgConfTeach::DoDataExchange(CDataExchange* dx)
{
	CDlgSubMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* button - normal */
	u32StartID = IDC_CONF_SETTING_TEACH_SAVE;
	for (i = 0; i < eCONF_TEACH_BTN_MAX; i++)		DDX_Control(dx, u32StartID + i, m_btn_ctl[i]);

	u32StartID = IDC_CONF_GRD_SETTING_TEACH;
	for (i = 0; i < eCONF_TEACH_GRID_MAX; i++)			DDX_Control(dx, u32StartID + i, m_pGrd[i]);

}

BEGIN_MESSAGE_MAP(CDlgConfTeach, CDlgSubMenu)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CONF_SETTING_TEACH_SAVE, IDC_CONF_SETTING_TEACH_SAVE + eCONF_TEACH_BTN_MAX, OnBtnClick)
	ON_NOTIFY(NM_CLICK, IDC_CONF_GRD_SETTING_TEACH, OnGrdTeachClick)
	ON_NOTIFY(NM_CLICK, IDC_CONF_GRD_SETTING_TEACH2, OnGrdTeachClick2)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgConfTeach::PreTranslateMessage(MSG* msg)
{
	return CDlgSubMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgConfTeach::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();
	LoadSettingTeach();
	InitGridTeach();
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgConfTeach::OnExitDlg()
{
	for (int i = 0; i < _countof(m_pGrd); i++)
	{
		if (m_pGrd[i])
		{
			m_pGrd[i].DeleteAllItems();
		}
	}

	m_vTeachPosition.clear();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgConfTeach::OnPaintDlg(CDC* dc)
{

	/* 자식 윈도 호출 */
	CDlgSubMenu::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgConfTeach::OnResizeDlg()
{
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgConfTeach::UpdatePeriod(UINT64 tick, BOOL is_busy)
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
VOID CDlgConfTeach::UpdateControl(UINT64 tick, BOOL is_busy)
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgConfTeach::InitCtrl()
{
	CResizeUI clsResizeUI;

	/* button - normal */
	for (int i = 0; i < eCONF_TEACH_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}

	for (int i = 0; i < eCONF_TEACH_GRID_MAX; i++)
	{
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_pGrd[i]);
		// by sysandj : Resize UI
	}
}

/*
 desc : 객체 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgConfTeach::InitObject()
{

	return TRUE;
}

/*
 desc : 객체 해제
 parm : None
 retn : None
*/
VOID CDlgConfTeach::CloseObject()
{
}

VOID CDlgConfTeach::InitGridTeach()
{
	//////////////////////////////////////////////////////////////////////////
	CResizeUI clsResizeUI;
	CRect rcGrdRect;
	int	nMaxCol = eCONF_TEACH_LABEL_MAX;
	int	nHeight = (int)(DEF_UI_GRD_ROW_OFFSET * clsResizeUI.GetRateY());
	int	nWidthDiffer = 0;
	int nMaxRow = 0;

	for (int nPage = 0; nPage < 2; nPage++)
	{
		if (m_nSelectPage + 1 + nPage == m_nMaxPage)
		{
			nMaxRow = EN_SETTING_TEACH::MAX % DEF_UI_MAX_TEACH_ROW + 1;
		}
		else
		{
			nMaxRow = DEF_UI_MAX_TEACH_ROW + 1;
		}

		nWidthDiffer = 0;
		if (nPage >= m_nMaxPage)
		{
			m_pGrd[eCONF_TEACH_GRID_MAIN2].ShowWindow(SW_HIDE);
			break;
		}

		//임시 사이즈
		m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].GetWindowRect(rcGrdRect);
		ScreenToClient(rcGrdRect);
		rcGrdRect.bottom = rcGrdRect.top + (nHeight * 19) + 1;
		m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].MoveWindow(rcGrdRect);

		ST_GRID_TEACH_COL stCol[eCONF_TEACH_LABEL_MAX] =
		{
			{ _T("POSITION NAME"),			DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS,	(int)((rcGrdRect.Width() / nMaxCol) * 1.2) },
			{ _T("POSITION"),				DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS,	(int)((rcGrdRect.Width() / nMaxCol) * 1) },
		};

		// 남은 간격 보상
		for (size_t i = 0; i < eCONF_TEACH_LABEL_MAX; i++)
		{
			nWidthDiffer += stCol[i].nWidth;
		}

		nWidthDiffer = rcGrdRect.Width() - nWidthDiffer - 1;

		if (rcGrdRect.Height() < nHeight * nMaxRow)
		{
			nWidthDiffer -= ::GetSystemMetrics(SM_CXVSCROLL);
		}
		else
		{
			rcGrdRect.bottom = rcGrdRect.top + nHeight * nMaxRow + 1;

			m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].MoveWindow(rcGrdRect);
		}

		stCol[eCONF_TEACH_LABEL_POSITION].nWidth += nWidthDiffer;

		m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].SetRowCount(nMaxRow);
		m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].SetColumnCount(nMaxCol);
		m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].SetFixedRowCount(1);

		for (int nRow = 0; nRow < nMaxRow; nRow++)
		{
			for (int nCol = 0; nCol < nMaxCol; nCol++)
			{
				if (0 == nRow)
				{
					m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].SetColumnWidth(nCol, stCol[nCol].nWidth);
					m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].SetItemText(0, nCol, stCol[nCol].strTitle);
					m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS);
					m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].SetItemBkColour(nRow, nCol, RGB(65, 65, 65));
					m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].SetItemFgColour(nRow, nCol, RGB(255, 255, 255));
				}
				else
				{
					if (nCol == eCONF_TEACH_LABEL_POSITION_NAME)
					{
						m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].SetItemBkColour(nRow, nCol, RGB(240, 248, 255));
						m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].SetItemFormat(nRow, nCol, stCol[nCol].nFormat);
						m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].SetItemText(nRow, nCol, m_vTeachPosition.at(((m_nSelectPage + nPage) * DEF_UI_MAX_TEACH_ROW) + nRow - 1).GetPositionName());
					}
					else
					{
						m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].SetItemBkColour(nRow, nCol, RGB(255, 255, 255));
						m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].SetItemFormat(nRow, nCol, stCol[nCol].nFormat);
						m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].SetItemTextFmt(nRow, nCol, _T("%.3f"), m_vTeachPosition.at(((m_nSelectPage + nPage) * DEF_UI_MAX_TEACH_ROW) + nRow - 1).GetPosition());
					}
				}
			}

			m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].SetRowHeight(nRow, nHeight);
		}
		m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].SetFont(&g_font[eFONT_LEVEL1_BOLD]);

		m_pGrd[eCONF_TEACH_GRID_MAIN1 + nPage].Refresh();
	}
}


/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgConfTeach::OnBtnClick(UINT32 id)
{
	switch (id)
	{
	case IDC_CONF_SETTING_TEACH_SAVE:
		SaveSettingTeach();
		break;
	case IDC_CONF_SETTING_TEACH_OPEN_CONTROL_PANEL:
		ShowCtrlPanel();
		break;

	case IDC_CONF_SETTING_TEACH_PREV:
		if (0 < m_nSelectPage)
		{
			m_nSelectPage--;
			InitGridTeach();
		}
		break;
	case IDC_CONF_SETTING_TEACH_NEXT:
		if (m_nMaxPage - 2 > m_nSelectPage)
		{
			m_nSelectPage++;
			InitGridTeach();
		}
		break;
	}
}

/*
 desc : 현재 실행된 자식 윈도 메모리 해제
 parm : None
 retn : None
*/
VOID CDlgConfTeach::DeleteMenu()
{
	if (!m_pDlgMenu)	return;

	if (m_pDlgMenu->GetSafeHwnd())	m_pDlgMenu->DestroyWindow();
	delete m_pDlgMenu;
	m_pDlgMenu = NULL;
}

/*
 desc : 자식 메뉴 화면 생성
 parm : id	- [in]  버튼 컨트롤 ID
 retn : TRUE or FALSE
*/
BOOL CDlgConfTeach::CreateMenu(UINT32 id)
{
	UINT32 u32DlgID = 0;

	/* 기존 실행된 자식 윈도가 있다면 메모리 해제 */
	DeleteMenu();

	return TRUE;
}

VOID CDlgConfTeach::OnGrdTeachClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	if (pItem->iRow < 1 || pItem->iColumn < 1)
	{
		return;
	}

	CString strPosition;
	CString strValue;
	int nDecimal[3] = {4, 1, 0};
	double dMin, dMax;
	dMin = -999999;
	dMax = 999999;
	strPosition = m_pGrd[eCONF_TEACH_GRID_MAIN1].GetItemText(pItem->iRow, eCONF_TEACH_LABEL_POSITION_NAME);

	if (TRUE == strPosition.IsEmpty())
		return;

	strValue = m_pGrd[eCONF_TEACH_GRID_MAIN1].GetItemText(pItem->iRow, pItem->iColumn);
	if (PopupKBDN(ENM_DITM::en_float, _T(""), strValue, dMin, dMax, nDecimal[pItem->iColumn -1]))
	{
		m_pGrd[eCONF_TEACH_GRID_MAIN1].SetItemText(pItem->iRow, pItem->iColumn, strValue);
	}
	else
	{
		m_pGrd[eCONF_TEACH_GRID_MAIN1].SetFocusCell(CCellID(pItem->iRow, 0));
		return;
	}
	
	//SetEditable(TRUE)이면 클릭한곳 다시클릭하면 수정되서 방지용
	m_pGrd[eCONF_TEACH_GRID_MAIN1].SetFocusCell(CCellID(pItem->iRow, 0));
	//화면갱신
	m_pGrd[eCONF_TEACH_GRID_MAIN1].Refresh();

	return;
}

VOID CDlgConfTeach::OnGrdTeachClick2(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	if (pItem->iRow < 1 || pItem->iColumn < 1)
	{
		return;
	}

	CString strPosition;
	CString strValue;
	int nDecimal[3] = { 4, 1, 0 };
	double dMin, dMax;
	dMin = -999999;
	dMax = 999999;
	strPosition = m_pGrd[eCONF_TEACH_GRID_MAIN2].GetItemText(pItem->iRow, eCONF_TEACH_LABEL_POSITION_NAME);

	if (TRUE == strPosition.IsEmpty())
		return;

	strValue = m_pGrd[eCONF_TEACH_GRID_MAIN2].GetItemText(pItem->iRow, pItem->iColumn);
	if (PopupKBDN(ENM_DITM::en_float, _T(""), strValue, dMin, dMax, nDecimal[pItem->iColumn - 1]))
	{
		m_pGrd[eCONF_TEACH_GRID_MAIN2].SetItemText(pItem->iRow, pItem->iColumn, strValue);
	}
	else
	{
		m_pGrd[eCONF_TEACH_GRID_MAIN2].SetFocusCell(CCellID(pItem->iRow, 0));
		return;
	}

	//SetEditable(TRUE)이면 클릭한곳 다시클릭하면 수정되서 방지용
	m_pGrd[eCONF_TEACH_GRID_MAIN2].SetFocusCell(CCellID(pItem->iRow, 0));
	//화면갱신
	m_pGrd[eCONF_TEACH_GRID_MAIN2].Refresh();

	return;
}

BOOL CDlgConfTeach::PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts/* = 0*/)
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

VOID  CDlgConfTeach::LoadSettingTeach()
{
	CString strLine;
	CString strValue;
	CString strPath;
	CStdioFile clsFile;
	CString strStation;
	CString strPosition;
	CString strMotor;

	ConfigToTeachFile(TRUE);

	int	nMaxCol = eCONF_TEACH_LABEL_MAX;
	int nMaxRow = EN_SETTING_TEACH::MAX + 1;
	ST_TEACH_MOTOR stTeach;
	//strPath.Format(_T("%s%s%s"), g_tzWorkDir, _T("\\%s\\setting\\", CUSTOM_DATA_CONFIG), _T("TEACH_DATA.CSV"));
	strPath.Format(_T("%s\\%s\\\setting\\TEACH_DATA.CSV"), g_tzWorkDir, CUSTOM_DATA_CONFIG);

	if (clsFile.Open(strPath, CStdioFile::modeRead | CFile::shareDenyNone))
	{
		clsFile.ReadString(strLine);

		for (int nRow = 1; nRow < nMaxRow; nRow++)
		{
			ST_TEACH_MOTOR stTeach;
			if (clsFile.ReadString(strLine))
			{
				AfxExtractSubString(strValue, strLine, eCONF_TEACH_LABEL_POSITION_NAME, ',');
				stTeach.SetPositionName(strValue);

				double dPosition = 0;
				if (GetConfigToTeachFile(nRow - 1, dPosition))
				{
					strValue.Format(_T("%.6f"), m_dTeachData[nRow - 1]);
				}
				else
				{
					AfxExtractSubString(strValue, strLine, eCONF_TEACH_LABEL_POSITION, ',');
					m_dTeachData[nRow - 1] = _ttof(strValue);
				}
				

				stTeach.dPosition = _ttof(strValue);
			}
			else
			{
				stTeach.SetPositionName((CString)EN_SETTING_TEACH::_from_index(nRow - 1)._to_string());
			}

			m_vTeachPosition.push_back(stTeach);
		}
		clsFile.Close();
	}
	else
	{
		for (int nRow = 1; nRow < nMaxRow; nRow++)
		{
			ST_TEACH_MOTOR stTeach;
			stTeach.dPosition = 0;
			stTeach.SetPositionName((CString)EN_SETTING_TEACH::_from_index(nRow - 1)._to_string());
			m_vTeachPosition.push_back(stTeach);
		}
	}
	m_pGrd[eCONF_TEACH_GRID_MAIN1].Refresh();
}

VOID CDlgConfTeach::SaveSettingTeach()
{
	CString strLine;
	CString strPath;
	CString strValue;
	CStdioFile clsFile;
	//strPath.Format(_T("%s%s%s"), g_tzWorkDir, _T("\\%s\\setting\\", CUSTOM_DATA_CONFIG), _T("TEACH_DATA.CSV"));
	strPath.Format(_T("%s\\%s\\\setting\\TEACH_DATA.CSV"), g_tzWorkDir, CUSTOM_DATA_CONFIG);

	int nMaxRow = 0;

	if (m_nSelectPage + 1 == m_nMaxPage)
	{
		nMaxRow = EN_SETTING_TEACH::MAX % DEF_UI_MAX_TEACH_ROW + 1;
	}
	else
	{
		nMaxRow = DEF_UI_MAX_TEACH_ROW + 1;
	}

	for (int nRow = 1; nRow < nMaxRow; nRow++)
	{
		strValue = m_pGrd[eCONF_TEACH_GRID_MAIN1].GetItemText(nRow, eCONF_TEACH_LABEL_POSITION_NAME);
		m_vTeachPosition.at((m_nSelectPage * DEF_UI_MAX_TEACH_ROW) + nRow - 1).SetPositionName(strValue);
		strValue = m_pGrd[eCONF_TEACH_GRID_MAIN1].GetItemText(nRow, eCONF_TEACH_LABEL_POSITION);
		m_vTeachPosition.at((m_nSelectPage * DEF_UI_MAX_TEACH_ROW) + nRow - 1).dPosition = _ttof(strValue);
		m_dTeachData[(m_nSelectPage * DEF_UI_MAX_TEACH_ROW) + nRow - 1] = _ttof(strValue);
	}

	if (1 < m_nMaxPage)
	{
		if (m_nSelectPage + 2 == m_nMaxPage)
		{
			nMaxRow = EN_SETTING_TEACH::MAX % DEF_UI_MAX_TEACH_ROW + 1;
		}
		else
		{
			nMaxRow = DEF_UI_MAX_TEACH_ROW + 1;
		}

		for (int nRow = 1; nRow < nMaxRow; nRow++)
		{
			strValue = m_pGrd[eCONF_TEACH_GRID_MAIN2].GetItemText(nRow, eCONF_TEACH_LABEL_POSITION_NAME);
			m_vTeachPosition.at(((m_nSelectPage + 1) * DEF_UI_MAX_TEACH_ROW) + nRow - 1).SetPositionName(strValue);
			strValue = m_pGrd[eCONF_TEACH_GRID_MAIN2].GetItemText(nRow, eCONF_TEACH_LABEL_POSITION);
			m_vTeachPosition.at(((m_nSelectPage + 1) * DEF_UI_MAX_TEACH_ROW) + nRow - 1).dPosition = _ttof(strValue);
			m_dTeachData[((m_nSelectPage + 1) * DEF_UI_MAX_TEACH_ROW) + nRow - 1] = _ttof(strValue);
		}
	}

	setlocale(LC_ALL, "korean");

	if (clsFile.Open(strPath, CStdioFile::modeCreate | CStdioFile::modeWrite))
	{
		strLine.Format(_T("POSITION NAME,POSITION,SPEED,ACC\n"));
		clsFile.WriteString(strLine);

		for (auto& pos : m_vTeachPosition)
		{
			strLine.Format(_T("%s,%.6f\n"), pos.GetPositionName(), pos.GetPosition());
			clsFile.WriteString(strLine);
		}

		clsFile.Close();
	}
	ConfigToTeachFile(FALSE);
	uvEng_SaveConfig();

	CDlgMesg dlgMesg;
	dlgMesg.MyDoModal(L"Teach Save succeed", 0x01);
}

BOOL CDlgConfTeach::ConfigToTeachFile(BOOL bLoadSave)
{
	LPG_CIEA pstCfg = uvEng_GetConfig();

	for (int i = 0; i < EN_SETTING_TEACH::MAX; i++)
	{
		switch (i)
		{
		case EN_SETTING_TEACH::TABLE_EXPO_START_X_1:
			if(bLoadSave)m_dTeachData[i] = pstCfg->luria_svc.table_expo_start_xy[0][0];
			else pstCfg->luria_svc.table_expo_start_xy[0][0] = m_dTeachData[i];
			
			break;
		case EN_SETTING_TEACH::TABLE_EXPO_START_Y_1:
			if (bLoadSave)m_dTeachData[i] = pstCfg->luria_svc.table_expo_start_xy[0][1];
			else pstCfg->luria_svc.table_expo_start_xy[0][1] = m_dTeachData[i];
			break;
		case EN_SETTING_TEACH::TABLE_UNLOADER_X_1:
			if (bLoadSave)m_dTeachData[i] = pstCfg->set_align.table_unloader_xy[0][0];
			else pstCfg->set_align.table_unloader_xy[0][0] = m_dTeachData[i];
			break;
		case EN_SETTING_TEACH::TABLE_UNLOADER_Y_1:
			if (bLoadSave)m_dTeachData[i] = pstCfg->set_align.table_unloader_xy[0][1];
			else pstCfg->set_align.table_unloader_xy[0][1] = m_dTeachData[i];
			break;
		case EN_SETTING_TEACH::PH_Z_AXIS_FOCUS_1:
			if (bLoadSave)m_dTeachData[i] = pstCfg->luria_svc.ph_z_focus[0];
			else pstCfg->luria_svc.ph_z_focus[0] = m_dTeachData[i];
			break;
		case EN_SETTING_TEACH::PH_Z_AXIS_FOCUS_2:
			if (bLoadSave)m_dTeachData[i] = pstCfg->luria_svc.ph_z_focus[1];
			else pstCfg->luria_svc.ph_z_focus[1] = m_dTeachData[i];
			break;
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
		case EN_SETTING_TEACH::PH_Z_AXIS_FOCUS_3:
			if (bLoadSave)m_dTeachData[i] = pstCfg->luria_svc.ph_z_focus[2];
			else pstCfg->luria_svc.ph_z_focus[2] = m_dTeachData[i];
			break;
		case EN_SETTING_TEACH::PH_Z_AXIS_FOCUS_4:
			if (bLoadSave)m_dTeachData[i] = pstCfg->luria_svc.ph_z_focus[3];
			else pstCfg->luria_svc.ph_z_focus[3] = m_dTeachData[i];
			break;
		case EN_SETTING_TEACH::PH_Z_AXIS_FOCUS_5:
			if (bLoadSave)m_dTeachData[i] = pstCfg->luria_svc.ph_z_focus[4];
			else pstCfg->luria_svc.ph_z_focus[4] = m_dTeachData[i];
			break;
		case EN_SETTING_TEACH::PH_Z_AXIS_FOCUS_6:
			if (bLoadSave)m_dTeachData[i] = pstCfg->luria_svc.ph_z_focus[5];
			else pstCfg->luria_svc.ph_z_focus[5] = m_dTeachData[i];
			break;
#endif
		case EN_SETTING_TEACH::ACAM_Z_FOCUS_1:
			if (bLoadSave)m_dTeachData[i] = pstCfg->acam_spec.acam_z_focus[0];
			else pstCfg->acam_spec.acam_z_focus[0] = m_dTeachData[i];
			break;
		case EN_SETTING_TEACH::ACAM_Z_FOCUS_2:
			if (bLoadSave)m_dTeachData[i] = pstCfg->acam_spec.acam_z_focus[1];
			else pstCfg->acam_spec.acam_z_focus[1] = m_dTeachData[i];
			break;
		case EN_SETTING_TEACH::ALIGN_MOVE_SPEED:
			if (bLoadSave)m_dTeachData[i] = pstCfg->mc2_svc.mark_velo;
			else pstCfg->mc2_svc.mark_velo = m_dTeachData[i];
			break;
		default:
			break;
		}
	}

	return TRUE;
}

BOOL CDlgConfTeach::GetConfigToTeachFile(int nIndex, double &dPosition)
{
	LPG_CIEA pstCfg = uvEng_GetConfig();
	switch (nIndex)
	{
	case EN_SETTING_TEACH::TABLE_EXPO_START_X_1:
		dPosition = pstCfg->luria_svc.table_expo_start_xy[0][0];
		break;
	case EN_SETTING_TEACH::TABLE_EXPO_START_Y_1:
		dPosition = pstCfg->luria_svc.table_expo_start_xy[0][1];
		break;
	case EN_SETTING_TEACH::TABLE_UNLOADER_X_1:
		dPosition = pstCfg->set_align.table_unloader_xy[0][0];
		break;
	case EN_SETTING_TEACH::TABLE_UNLOADER_Y_1:
		dPosition = pstCfg->set_align.table_unloader_xy[0][1];
		break;
	case EN_SETTING_TEACH::PH_Z_AXIS_FOCUS_1:
		dPosition = pstCfg->luria_svc.ph_z_focus[0];
		break;
	case EN_SETTING_TEACH::PH_Z_AXIS_FOCUS_2:
		dPosition = pstCfg->luria_svc.ph_z_focus[1];
		break;
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	case EN_SETTING_TEACH::PH_Z_AXIS_FOCUS_3:
		dPosition = pstCfg->luria_svc.ph_z_focus[2];
		break;
	case EN_SETTING_TEACH::PH_Z_AXIS_FOCUS_4:
		dPosition = pstCfg->luria_svc.ph_z_focus[3];
		break;
	case EN_SETTING_TEACH::PH_Z_AXIS_FOCUS_5:
		dPosition = pstCfg->luria_svc.ph_z_focus[4];
		break;
	case EN_SETTING_TEACH::PH_Z_AXIS_FOCUS_6:
		dPosition = pstCfg->luria_svc.ph_z_focus[5];
		break;
#endif
	case EN_SETTING_TEACH::ACAM_Z_FOCUS_1:
		dPosition = pstCfg->acam_spec.acam_z_focus[0];
		break;
	case EN_SETTING_TEACH::ACAM_Z_FOCUS_2:
		dPosition = pstCfg->acam_spec.acam_z_focus[1];
		break;
	case EN_SETTING_TEACH::ALIGN_MOVE_SPEED:
		dPosition = pstCfg->mc2_svc.mark_velo;
		break;
	default:
		dPosition = 0;
		return FALSE;
		break;
	}

	return TRUE;
}

void CDlgConfTeach::ShowCtrlPanel()
{
	if (AfxGetMainWnd()->GetSafeHwnd())
	{
		::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), eMSG_MAIN_OPEN_CONSOLE, NULL, NULL);
	}
}