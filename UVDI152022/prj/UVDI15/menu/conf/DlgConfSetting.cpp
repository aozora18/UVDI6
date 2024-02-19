// DlgCalb.cpp: 구현 파일
//

#include "pch.h"
#include "DlgConfSetting.h"

#include "../../mesg/DlgMesg.h"
#include "../../../../inc/kybd/DlgKBDT.h"
#include "afxdialogex.h"


/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgConfSetting::CDlgConfSetting(UINT32 id, CWnd* parent)
	: CDlgSubMenu(id, parent)
{
	m_enDlgID = ENG_CMDI_SUB::en_menu_sub_conf_setting_option;
	m_pDlgMenu = NULL;
}

CDlgConfSetting::~CDlgConfSetting()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgConfSetting::DoDataExchange(CDataExchange* dx)
{
	CDlgSubMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

 	/* button - normal */
 	u32StartID = IDC_CONF_SETTING_OPTION_PREV;
 	for (i = 0; i < eCONF_SETTING_OPTION_BTN_MAX; i++)		DDX_Control(dx, u32StartID + i, m_btn_ctl[i]);

	u32StartID = IDC_CONF_GRD_SETTING_OPTION;
	for (i = 0; i < eCONF_SETTING_OPTION_GRID_MAX; i++)			DDX_Control(dx, u32StartID + i, m_pGrd[i]);
}

BEGIN_MESSAGE_MAP(CDlgConfSetting, CDlgSubMenu)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CONF_SETTING_OPTION_PREV, IDC_CONF_SETTING_OPTION_SAVE, OnBtnClick)
	ON_NOTIFY(NM_CLICK, IDC_CONF_GRD_SETTING_OPTION, &CDlgConfSetting::OnGrdSettingBasicParamClick)
	ON_NOTIFY(NM_CLICK, IDC_CONF_GRD_SETTING_OPTION2, &CDlgConfSetting::OnGrdSettingBasicParamClick2)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_CONF_GRD_SETTING_OPTION, &CDlgConfSetting::OnGrdSettingBasicParamChanged)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_CONF_GRD_SETTING_OPTION2, &CDlgConfSetting::OnGrdSettingBasicParamChanged2)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgConfSetting::PreTranslateMessage(MSG* msg)
{
	return CDlgSubMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgConfSetting::OnInitDlg()
{
	CSettingManager::GetInstance()->Init();
	
	/* 컨트롤 초기화 */
	LoadConfig();
	InitCtrl();
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgConfSetting::OnExitDlg()
{
	for (int i = 0; i < _countof(m_pGrd); i++)
	{
		if (m_pGrd[i])
		{
			m_pGrd[i].DeleteAllItems();
		}
	}

}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgConfSetting::OnPaintDlg(CDC* dc)
{

	/* 자식 윈도 호출 */
	CDlgSubMenu::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgConfSetting::OnResizeDlg()
{
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgConfSetting::UpdatePeriod(UINT64 tick, BOOL is_busy)
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
VOID CDlgConfSetting::UpdateControl(UINT64 tick, BOOL is_busy)
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgConfSetting::InitCtrl()
{
	CResizeUI clsResizeUI;

	/* button - normal */
 	for (int i = 0; i < eCONF_SETTING_OPTION_BTN_MAX; i++)
 	{
 		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
 		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
 		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);
 
 		// by sysandj : Resize UI
 		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
 		// by sysandj : Resize UI
 	}

	for (int i = 0; i < eCONF_SETTING_OPTION_GRID_MAX; i++)
	{
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_pGrd[i]);
		// by sysandj : Resize UI
	}

	CRect rcRect;
	m_pGrd[eCONF_SETTING_OPTION_GRID].GetWindowRect(&rcRect);
	m_nGrdSizeY = rcRect.Height();
	

	int m_nSelectTab = 0;
	int	nHeight = (int)(DEF_UI_GRD_ROW_OFFSET * clsResizeUI.GetRateY());
	int	nMaxRowDisplay = m_nGrdSizeY / nHeight;
	
	int nMaxParam = CSettingManager::GetInstance()->GetSetting()->GetParamCount(m_nSelectTab);
	CString strGroupOld = _T("");
	//파라미터 배열
	CArrGrdParam* pArrGrdParamTemp = NULL;
	//////////////////////////////////////////////////////////////////////////
	//그룹으로 정리 : 그룹- 파람배열로 정리한 단계
	//임시로 그룹별로 정리
	CArrGrdPage arrGrdGroup;
	for (int nCntParam = 0; nCntParam < nMaxParam; nCntParam++)
	{
		if (FALSE == CSettingManager::GetInstance()->GetSetting()->GetShow(m_nSelectTab, nCntParam))
			continue;

		//사용하지않는 파라미터는 패스
		if (FALSE == CSettingManager::GetInstance()->GetSetting()->GetUse(m_nSelectTab, nCntParam))
			continue;

		if (strGroupOld != CSettingManager::GetInstance()->GetSetting()->GetParam(m_nSelectTab, nCntParam).strGroup)
		{
			//빈파라미터 타이틀용
			ST_GRD_PARAM* pStGrdParam = new ST_GRD_PARAM();
			pStGrdParam->bIsGroup = TRUE;
			pStGrdParam->nIndexParam = nCntParam;
			//파라미터 배열
			pArrGrdParamTemp = new CArrGrdParam();
			pArrGrdParamTemp->Add(pStGrdParam);
			//그룹 배열			
			arrGrdGroup.Add(pArrGrdParamTemp);

			strGroupOld = CSettingManager::GetInstance()->GetSetting()->GetParam(m_nSelectTab, nCntParam).strGroup;
		}

		//실제 파라미터 매칭
		ST_GRD_PARAM* pStGrdParam = new ST_GRD_PARAM();
		pStGrdParam->bIsGroup = FALSE;
		pStGrdParam->nIndexParam = nCntParam;
		//임시 저장관리	InitGridParam시점에 Setting의 값을 가져온다.	
		pStGrdParam->strValue = CSettingManager::GetInstance()->GetSetting()->GetParam(m_nSelectTab, nCntParam).GetScaledValue();
		//실제 파라미터 넣기
		pArrGrdParamTemp->Add(pStGrdParam);
	}
	//////////////////////////////////////////////////////////////////////////
	//페이지로 정리 : 페이지 - 파람배열로 정리 완료
	CArrGrdParam* pArrGrdParam = NULL;
	int nCntParamInPage = 0;
	for (int nCntGroup = 0; nCntGroup < arrGrdGroup.GetCount(); nCntGroup++)
	{
		pArrGrdParamTemp = arrGrdGroup.GetAt(nCntGroup);
		int nCntCurGroup = (int)pArrGrdParamTemp->GetCount();
		//0일때 무조건 새로 생성

		if (0 == nCntParamInPage || nCntParamInPage + nCntCurGroup > nMaxRowDisplay)
		{
			//
			pArrGrdParam = new CArrGrdParam();

			for (int nCntParam = 0; nCntParam < nCntCurGroup; nCntParam++)
			{
				pArrGrdParam->Add(pArrGrdParamTemp->GetAt(nCntParam));
			}
			//페이지에 그룹추가
			m_arrGrdPage.Add(pArrGrdParam);
			if (nCntCurGroup > nMaxRowDisplay) //한그룹만으로도 한페이지가 꽉찰때, 반복문의 다음번에 새로 페이지를 만들수 있도록
			{
				nCntParamInPage = 0;
			}
			else
			{
				nCntParamInPage = 0;
				nCntParamInPage += nCntCurGroup;
			}

		}
		else if (nCntParamInPage + nCntCurGroup <= nMaxRowDisplay)
		{
			for (int nCntParam = 0; nCntParam < nCntCurGroup; nCntParam++)
			{
				pArrGrdParam->Add(pArrGrdParamTemp->GetAt(nCntParam));
			}
			nCntParamInPage += nCntCurGroup;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//템프 제거
	for (int nCntGroup = 0; nCntGroup < arrGrdGroup.GetCount(); nCntGroup++)
	{
		pArrGrdParamTemp = arrGrdGroup.GetAt(nCntGroup);
		delete pArrGrdParamTemp;
	}
	arrGrdGroup.RemoveAll();
	//////////////////////////////////////////////////////////////////////////
	//페이지 관련 초기화
	m_nMaxPage = (int)m_arrGrdPage.GetCount();
	m_nSelectPage = 0;
	//////////////////////////////////////////////////////////////////////////

	UpdateGridParam();

}

void CDlgConfSetting::UpdateGridParam()
{
	//////////////////////////////////////////////////////////////////////////
	//UI 	
	CResizeUI clsResizeUI;
	int nMaxCol = 3;
	int nMaxRow = 0;
	int nHeight = (int)(DEF_UI_GRD_ROW_OFFSET * clsResizeUI.GetRateY());
	int nMaxRowDisplay = m_nGrdSizeY / nHeight;

	CRect rtWnd;
	m_pGrd[eCONF_SETTING_OPTION_GRID].GetWindowRect(&rtWnd);

	int nWidthParameter = (int)(rtWnd.Width() * 0.5);
	int nWidthValue = (int)(rtWnd.Width() * 0.3);
	int nWidthUnit = (int)(rtWnd.Width() * 0.2);

	int nWidth[3] = { nWidthParameter, nWidthValue, nWidthUnit };

	UINT nForamt = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;
	//////////////////////////////////////////////////////////////////////////

	int nMaxPage = m_nMaxPage;
	if (nMaxPage > 2)
	{
		nMaxPage = 2;
	}

	CArrGrdParam* pArrGrdParam = NULL;
	ST_GRD_PARAM* pStGrdPram = NULL;
	for (int nCntPage = 0; nCntPage < nMaxPage; nCntPage++)
	{
		//초기화 필요
		nWidth[2] = nWidthUnit;
		//MaxRow 판단		
		pArrGrdParam = m_arrGrdPage.GetAt(m_nSelectPage + nCntPage);
		nMaxRow = (int)pArrGrdParam->GetCount();
		//UI초기화		
		m_pGrd[nCntPage].DeleteAllItems();
		m_pGrd[nCntPage].ReleaseMergeCells();
		m_pGrd[nCntPage].SetRowCount(nMaxRow);
		m_pGrd[nCntPage].SetColumnCount(nMaxCol);
		//20210513 JBP
		//모든 그리드의 첫줄은 Fix한다.
		//GridCtrl.cpp에 //20210513 JBP 로 검색하면, MergeCell일경우의 처리가 되어있어 주석처리했음 문제될경우 확인필요
		m_pGrd[nCntPage].SetFixedRowCount(1);
		//Col 사이즈 및 윈도우 렉트 변경
		m_pGrd[nCntPage].GetWindowRect(&rtWnd);
		ScreenToClient(rtWnd);
		if (nMaxRow > nMaxRowDisplay)
		{
			rtWnd.bottom = rtWnd.top + m_nGrdSizeY;
			nWidth[2] = nWidthUnit - ::GetSystemMetrics(SM_CXVSCROLL);
			m_pGrd[nCntPage].MoveWindow(rtWnd);
		}
		else
		{
			rtWnd.bottom = rtWnd.top + nHeight * nMaxRow;
			m_pGrd[nCntPage].MoveWindow(rtWnd);
		}

		for (int nCol = 0; nCol < nMaxCol; nCol++)
		{
			m_pGrd[nCntPage].SetColumnWidth(nCol, nWidth[nCol]);
		}

		for (int nRow = 0; nRow < pArrGrdParam->GetCount(); nRow++)
		{
			pStGrdPram = pArrGrdParam->GetAt(nRow);
			ST_SETTING_PARAM stParam = CSettingManager::GetInstance()->GetSetting()->GetParam(0, pStGrdPram->nIndexParam);

			//20221107 by Choi : Grid Edit 안되도록 추가.
			m_pGrd[nCntPage].SetItemState(nRow, 0, GVIS_READONLY);
			m_pGrd[nCntPage].SetItemState(nRow, 1, GVIS_READONLY);
			m_pGrd[nCntPage].SetItemState(nRow, 2, GVIS_READONLY);

			if (TRUE == pStGrdPram->bIsGroup)
			{
				m_pGrd[nCntPage].MergeCells(CCellRange(nRow, 0, nRow, 2));
				//m_pGrd[nCntPage].SetItemState(0, 0, GetItemState(0, 0) | GVIS_FIXED | GVIS_FIXEDROW);
				m_pGrd[nCntPage].SetItemText(nRow, 0, stParam.strGroup);
				m_pGrd[nCntPage].SetItemFormat(nRow, 0, nForamt);
				//m_pGrd[nCntPage].SetItemBkColour(nRow, 0, SKY_BLUE);
				m_pGrd[nCntPage].SetItemBkColour(nRow, 0, RGB(65, 65, 65));
				m_pGrd[nCntPage].SetItemFgColour(nRow, 0, RGB(255, 255, 255));
				m_pGrd[nCntPage].SetRowHeight(nRow, nHeight);
			}
			else
			{
				//폼을 위한 for 문
				for (int nCol = 0; nCol < nMaxCol; nCol++)
				{
					m_pGrd[nCntPage].SetItemFormat(nRow, nCol, nForamt);
					if (1 != nCol) //파라미터 입력란만 흰색으로 사용
					{
						m_pGrd[nCntPage].SetItemBkColour(nRow, nCol, ALICE_BLUE);
					}
				}
				//디스플레이 네임
				m_pGrd[nCntPage].SetItemText(nRow, 0, stParam.strDisplayName);
				m_pGrd[nCntPage].SetItemTextDes(nRow, 0, stParam.strDescript);
				m_pGrd[nCntPage].SetItemTextDes(nRow, 1, stParam.strDescript);
				m_pGrd[nCntPage].SetItemText(nRow, 2, stParam.strUnit);

				if (DEF_DATA_TYPE_BOOL == stParam.strDataType)
				{
					m_pGrd[nCntPage].SetCellType(nRow, 1, RUNTIME_CLASS(CGridCellCombo));
					CStringArray options;
					options.Add(_T("NO"));
					options.Add(_T("YES"));

					CGridCellCombo* pComboCell = (CGridCellCombo*)m_pGrd[nCntPage].GetCell(nRow, 1);

					m_pGrd[nCntPage].SetListMode(FALSE);
					m_pGrd[nCntPage].EnableDragAndDrop(TRUE);

					//20221107 by Choi : ComboBox는 수정되어야한다.
					m_pGrd[nCntPage].SetItemState(nRow, 1, 0x00);

					pComboCell->SetOptions(options);
					pComboCell->SetStyle(CBS_DROPDOWN);

					//임시 저장관리로 인해 제거
					//if (0 == stParam.GetInt())
					//{
					//	pComboCell->SetText(_T("NO"));
					//}
					//else
					//{
					//	pComboCell->SetText(_T("YES"));
					//}

					//임시저장된 데이터를 사용
					if (FALSE == _ttoi(pStGrdPram->strValue))
					{
						pComboCell->SetText(_T("NO"));
					}
					else
					{
						pComboCell->SetText(_T("YES"));
					}
				}
				else
				{
					//임시저장 관리로 인해 제거
					//CString strTemp = stParam.GetValue();
					//m_pGrd[nCntPage].SetItemText(nRow, 1, strTemp);

					//임시저장된 데이터를 사용
					m_pGrd[nCntPage].SetItemText(nRow, 1, pStGrdPram->strValue);
				}
			}
			m_pGrd[nCntPage].SetRowHeight(nRow, nHeight);
			m_pGrd[nCntPage].SetRowHeight(nRow, nHeight);
		}

		m_pGrd[nCntPage].SetColumnResize(FALSE);
		m_pGrd[nCntPage].SetRowResize(FALSE);
		m_pGrd[nCntPage].EnableSelection(FALSE);
		//m_pGrd[nCntPage].SetEditable(m_bEnableEdit);
		m_pGrd[nCntPage].SetEditable(TRUE);
		m_pGrd[nCntPage].SetFont(&g_font[eFONT_LEVEL3_BOLD]);
		m_pGrd[nCntPage].Refresh();
	}
}

void CDlgConfSetting::SaveSettingParam()
{
// 	if (IDNO == ShowYesNoMsg(eQUEST, _T("변경사항을 저장하시겠습니까?")))
// 	{
// 		return;
// 	}

	CArrGrdParam* pArrGrdParam = NULL;
	ST_GRD_PARAM* pStGrdPram = NULL;
	ST_SETTING_PARAM stParam;

	for (int nCntPage = 0; nCntPage < m_nMaxPage; nCntPage++)
	{
		pArrGrdParam = m_arrGrdPage.GetAt(nCntPage);
		for (int nCntParam = 0; nCntParam < pArrGrdParam->GetCount(); nCntParam++)
		{
			pStGrdPram = pArrGrdParam->GetAt(nCntParam);
			if (TRUE == pStGrdPram->bIsGroup)
				continue;
			stParam = CSettingManager::GetInstance()->GetSetting()->GetParam(EN_SETTING_TAB::OPTION, pStGrdPram->nIndexParam);

			//CString strText = m_pGrd[nCntPage]->GetItemText(nCntParam, 1);

			if (DEF_DATA_TYPE_BOOL == stParam.strDataType)
			{
				BOOL bIsTrue;
				if (TRUE == _ttoi(pStGrdPram->strValue))
				{
					bIsTrue = TRUE;
				}
				else
				{
					bIsTrue = FALSE;
				}
				stParam.SetValue(bIsTrue);
			}
			else
			{
				stParam.SetScaledValue(pStGrdPram->strValue);
			}

			CSettingManager::GetInstance()->GetSetting()->SetParam(EN_SETTING_TAB::OPTION, pStGrdPram->nIndexParam, stParam);
		}
	}

	SaveConfig();
	//여기에서 골라내서 저장함수랑 매칭해서 저장시키기

}

void CDlgConfSetting::SaveSettingParamMatch()
{
	for (int i=0; i < EN_SETTING_OPTION::MAX; i++)
	{
		switch (i)
		{
		case EN_SETTING_OPTION::LOG_FILE_SAVED:
			break;
		default:
			break;
		}
	}
}

VOID CDlgConfSetting::OnGrdSettingBasicParamClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	//if (FALSE == m_bEnableEdit)
	//	return;

	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	int nMaxRow = 0;

	CArrGrdParam* pArrGrdParam = NULL;
	ST_GRD_PARAM* pStGrdParam = NULL;
	pArrGrdParam = m_arrGrdPage.GetAt(m_nSelectPage);
	nMaxRow = (int)pArrGrdParam->GetCount();
	//그룹명을 클릭했을땐 column이 0이라 무조건 탈출됨
	if (pItem->iRow < 1 || pItem->iRow > nMaxRow || pItem->iColumn != 1)
	{
		return;
	}


	pStGrdParam = pArrGrdParam->GetAt(pItem->iRow);
	ST_SETTING_PARAM stParam = CSettingManager::GetInstance()->GetSetting()->GetParam(0, pStGrdParam->nIndexParam);

	if ((UINT)stParam.GetUILevel() > g_loginLevel || eLOGIN_LEVEL_UNKNOWN == g_loginLevel)
	{
		CDlgMesg dlgMesg;
		dlgMesg.MyDoModal(L"로그인 레벨 제한! \n해당 파라미터를 수정할 수 없습니다.", 0x01);
		return;
	}

	double dScale = stParam.dScale;
	if (0 == dScale)
		dScale = 1;

	CString strValueOld = pStGrdParam->strValue; //스캐일이 반영된 상태
	CString strValue = pStGrdParam->strValue; //스캐일이 반영된 상태
	if (DEF_DATA_TYPE_BOOL == stParam.strDataType) //아무것도 하지않음
	{
		//SetFocusCell을 안타게 하기위해...
		return;
	}
	else if (DEF_DATA_TYPE_INT == stParam.strDataType)
	{

		if (PopupKBDN(ENM_DITM::en_int16, _T(""), strValue, stParam.dMin, stParam.dMax))
		{
			if (-1 != strValue.Find('.'))
			{
				//LANGUAGE 추가완료
				AfxMessageBox(L"정수를 입력하십시오.");
				m_pGrd[eCONF_SETTING_OPTION_GRID].SetFocusCell(CCellID(pItem->iRow, pItem->iColumn - 1));
				return;
			}

			int nValueOld = _ttoi(strValueOld);
			int nValue = _ttoi(strValue);

			if (stParam.dMin * dScale > nValue || stParam.dMax * dScale < nValue)
			{
				//LANGUAGE 추가완료
				AfxMessageBox(L"변경 범위를 벗어났습니다.");
				m_pGrd[eCONF_SETTING_OPTION_GRID].SetFocusCell(CCellID(pItem->iRow, pItem->iColumn - 1));
				return;
			}
			else if (0 != stParam.dChangeLimit && fabs(nValue - nValueOld) > stParam.dChangeLimit * dScale)
			{
				//LANGUAGE 추가완료
				AfxMessageBox(L"변경 범위를 벗어났습니다.");
				m_pGrd[eCONF_SETTING_OPTION_GRID].SetFocusCell(CCellID(pItem->iRow, pItem->iColumn - 1));
				return;
			}

			m_pGrd[eCONF_SETTING_OPTION_GRID].SetItemText(pItem->iRow, 1, strValue);
		}
		else
		{
			m_pGrd[eCONF_SETTING_OPTION_GRID].SetFocusCell(CCellID(pItem->iRow, pItem->iColumn - 1));
			return;
		}
	}
	else if (DEF_DATA_TYPE_DOUBLE == stParam.strDataType) //Num
	{
		if (PopupKBDN(ENM_DITM::en_float, _T(""), strValue, stParam.dMin, stParam.dMax))
		{
			double dValueOld = _ttof(strValueOld);
			double dValue = _ttof(strValue);

			//수정시 Grid는 표현만 담당
			if (stParam.dMin * dScale > dValue || stParam.dMax * dScale < dValue)
			{
				AfxMessageBox(L"정수를 입력하십시오.");
				m_pGrd[eCONF_SETTING_OPTION_GRID].SetFocusCell(CCellID(pItem->iRow, pItem->iColumn - 1));
				return;
			}
			else if (0 != stParam.dChangeLimit && fabs(dValue - dValueOld) > stParam.dChangeLimit * dScale)
			{
				AfxMessageBox(L"변경 범위를 벗어났습니다.");
				m_pGrd[eCONF_SETTING_OPTION_GRID].SetFocusCell(CCellID(pItem->iRow, pItem->iColumn - 1));
				return;
			}

			m_pGrd[eCONF_SETTING_OPTION_GRID].SetItemText(pItem->iRow, 1, strValue);
		}
		else
		{
			m_pGrd[eCONF_SETTING_OPTION_GRID].SetFocusCell(CCellID(pItem->iRow, pItem->iColumn - 1));
			return;
		}
	}
	else if (DEF_DATA_TYPE_STRING == stParam.strDataType)
	{
		if (stParam.strUnit == _T("PATH"))
		{
			SelectPath(strValue);
			m_pGrd[eCONF_SETTING_OPTION_GRID].SetItemText(pItem->iRow, 1, strValue);
		}
		else
		{
			CDlgKBDT dlg;
			TCHAR tzText[1024] = { NULL };
			if (IDOK == dlg.MyDoModal(DEF_TEXT_MAX_LENGTH))
			{
				dlg.GetText(tzText, DEF_TEXT_MAX_LENGTH);
				strValue = (LPCTSTR)tzText;
				m_pGrd[eCONF_SETTING_OPTION_GRID].SetItemText(pItem->iRow, 1, strValue);
			}
			else
			{
				m_pGrd[eCONF_SETTING_OPTION_GRID].SetFocusCell(CCellID(pItem->iRow, pItem->iColumn - 1));
				return;
			}
		}


	}

	//임시 저장관리
	pStGrdParam->strValue = strValue;
	//SetEditable(TRUE)이면 클릭한곳 다시클릭하면 수정되서 방지용
	m_pGrd[eCONF_SETTING_OPTION_GRID].SetFocusCell(CCellID(pItem->iRow, pItem->iColumn - 1));
	//화면갱신
	m_pGrd[eCONF_SETTING_OPTION_GRID].Refresh();
	//에디팅 상태로 변경
	//SetEditing(TRUE);

}

VOID CDlgConfSetting::OnGrdSettingBasicParamClick2(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	//if (FALSE == m_bEnableEdit)
//	return;

	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	int nMaxRow = 0;

	CArrGrdParam* pArrGrdParam = NULL;
	ST_GRD_PARAM* pStGrdParam = NULL;
	pArrGrdParam = m_arrGrdPage.GetAt(m_nSelectPage + 1);
	nMaxRow = (int)pArrGrdParam->GetCount();
	//그룹명을 클릭했을땐 column이 0이라 무조건 탈출됨
	if (pItem->iRow < 1 || pItem->iRow > nMaxRow || pItem->iColumn != 1)
	{
		return;
	}


	pStGrdParam = pArrGrdParam->GetAt(pItem->iRow);
	ST_SETTING_PARAM stParam = CSettingManager::GetInstance()->GetSetting()->GetParam(0, pStGrdParam->nIndexParam);

	if ((UINT)stParam.GetUILevel() > g_loginLevel || eLOGIN_LEVEL_UNKNOWN == g_loginLevel)
	{
		CDlgMesg dlgMesg;
		dlgMesg.MyDoModal(L"로그인 레벨 제한! \n해당 파라미터를 수정할 수 없습니다.", 0x01);
		return;
	}

	double dScale = stParam.dScale;
	if (0 == dScale)
		dScale = 1;

	CString strValueOld = pStGrdParam->strValue; //스캐일이 반영된 상태
	CString strValue = pStGrdParam->strValue; //스캐일이 반영된 상태
	if (DEF_DATA_TYPE_BOOL == stParam.strDataType) //아무것도 하지않음
	{
		//SetFocusCell을 안타게 하기위해...
		return;
	}
	else if (DEF_DATA_TYPE_INT == stParam.strDataType)
	{

		if (PopupKBDN(ENM_DITM::en_int16, _T(""), strValue, stParam.dMin, stParam.dMax))
		{
			if (-1 != strValue.Find('.'))
			{
				//LANGUAGE 추가완료
				AfxMessageBox(L"정수를 입력하십시오.");
				m_pGrd[eCONF_SETTING_OPTION_GRID2].SetFocusCell(CCellID(pItem->iRow, pItem->iColumn - 1));
				return;
			}

			int nValueOld = _ttoi(strValueOld);
			int nValue = _ttoi(strValue);

			if (stParam.dMin * dScale > nValue || stParam.dMax * dScale < nValue)
			{
				//LANGUAGE 추가완료
				AfxMessageBox(L"변경 범위를 벗어났습니다.");
				m_pGrd[eCONF_SETTING_OPTION_GRID2].SetFocusCell(CCellID(pItem->iRow, pItem->iColumn - 1));
				return;
			}
			else if (0 != stParam.dChangeLimit && fabs(nValue - nValueOld) > stParam.dChangeLimit * dScale)
			{
				//LANGUAGE 추가완료
				AfxMessageBox(L"변경 범위를 벗어났습니다.");
				m_pGrd[eCONF_SETTING_OPTION_GRID2].SetFocusCell(CCellID(pItem->iRow, pItem->iColumn - 1));
				return;
			}

			m_pGrd[eCONF_SETTING_OPTION_GRID2].SetItemText(pItem->iRow, 1, strValue);
		}
		else
		{
			m_pGrd[eCONF_SETTING_OPTION_GRID2].SetFocusCell(CCellID(pItem->iRow, pItem->iColumn - 1));
			return;
		}
	}
	else if (DEF_DATA_TYPE_DOUBLE == stParam.strDataType) //Num
	{
		if (PopupKBDN(ENM_DITM::en_float, _T(""), strValue, stParam.dMin, stParam.dMax, 6))
		{
			double dValueOld = _ttof(strValueOld);
			double dValue = _ttof(strValue);

			//수정시 Grid는 표현만 담당
			if (stParam.dMin * dScale > dValue || stParam.dMax * dScale < dValue)
			{
				AfxMessageBox(L"정수를 입력하십시오.");
				m_pGrd[eCONF_SETTING_OPTION_GRID2].SetFocusCell(CCellID(pItem->iRow, pItem->iColumn - 1));
				return;
			}
			else if (0 != stParam.dChangeLimit && fabs(dValue - dValueOld) > stParam.dChangeLimit * dScale)
			{
				AfxMessageBox(L"변경 범위를 벗어났습니다.");
				m_pGrd[eCONF_SETTING_OPTION_GRID2].SetFocusCell(CCellID(pItem->iRow, pItem->iColumn - 1));
				return;
			}

			m_pGrd[eCONF_SETTING_OPTION_GRID2].SetItemText(pItem->iRow, 1, strValue);
		}
		else
		{
			m_pGrd[eCONF_SETTING_OPTION_GRID2].SetFocusCell(CCellID(pItem->iRow, pItem->iColumn - 1));
			return;
		}
	}
	else if (DEF_DATA_TYPE_STRING == stParam.strDataType)
	{
		if (stParam.strUnit == _T("PATH"))
		{
			SelectPath(strValue);
			m_pGrd[eCONF_SETTING_OPTION_GRID2].SetItemText(pItem->iRow, 1, strValue);
		}
		else
		{
			CDlgKBDT dlg;
			TCHAR tzText[1024] = { NULL };
			if (IDOK == dlg.MyDoModal(DEF_TEXT_MAX_LENGTH))
			{
				dlg.GetText(tzText, DEF_TEXT_MAX_LENGTH);
				strValue = (LPCTSTR)tzText;
				m_pGrd[eCONF_SETTING_OPTION_GRID2].SetItemText(pItem->iRow, 1, strValue);
			}
			else
			{
				m_pGrd[eCONF_SETTING_OPTION_GRID2].SetFocusCell(CCellID(pItem->iRow, pItem->iColumn - 1));
				return;
			}
		}


	}

	//임시 저장관리
	pStGrdParam->strValue = strValue;
	//SetEditable(TRUE)이면 클릭한곳 다시클릭하면 수정되서 방지용
	m_pGrd[eCONF_SETTING_OPTION_GRID2].SetFocusCell(CCellID(pItem->iRow, pItem->iColumn - 1));
	//화면갱신
	m_pGrd[eCONF_SETTING_OPTION_GRID2].Refresh();
	//에디팅 상태로 변경
	//SetEditing(TRUE);
}

VOID CDlgConfSetting::OnGrdSettingBasicParamChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	int nMaxRow = 0;

	CArrGrdParam* pArrGrdParam = NULL;
	ST_GRD_PARAM* pStGrdParam = NULL;
	pArrGrdParam = m_arrGrdPage.GetAt(m_nSelectPage);
	nMaxRow = (int)pArrGrdParam->GetCount();
	//그룹명을 클릭했을땐 column이 0이라 무조건 탈출됨
	if (pItem->iRow < 1 || pItem->iRow > nMaxRow || pItem->iColumn != 1)	return;

	pStGrdParam = pArrGrdParam->GetAt(pItem->iRow);
	ST_SETTING_PARAM stParam = CSettingManager::GetInstance()->GetSetting()->GetParam(EN_SETTING_TAB::OPTION, pStGrdParam->nIndexParam);

	if ((UINT)stParam.GetUILevel() > g_loginLevel || eLOGIN_LEVEL_UNKNOWN == g_loginLevel)
	{
		CDlgMesg dlgMesg;
		dlgMesg.MyDoModal(L"로그인 레벨 제한! \n해당 파라미터를 수정할 수 없습니다.", 0x01);
		return;
	}

	CString strValue = m_pGrd[eCONF_SETTING_OPTION_GRID].GetItemText(pItem->iRow, 1);

	//바로 파람수정
	BOOL bIsTrue = FALSE;
	if (_T("YES") == strValue)
	{
		bIsTrue = TRUE;
		//임시 저장관리
		pStGrdParam->strValue = _T("1");
	}
	else
	{
		bIsTrue = FALSE;
		//임시 저장관리
		pStGrdParam->strValue = _T("0");
	}

	//에디팅 상태로 변경
	//SetEditing(TRUE);
}

VOID CDlgConfSetting::OnGrdSettingBasicParamChanged2(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	int nMaxRow = 0;

	CArrGrdParam* pArrGrdParam = NULL;
	ST_GRD_PARAM* pStGrdParam = NULL;
	pArrGrdParam = m_arrGrdPage.GetAt(m_nSelectPage + 1);
	nMaxRow = (int)pArrGrdParam->GetCount();
	//그룹명을 클릭했을땐 column이 0이라 무조건 탈출됨
	if (pItem->iRow < 1 || pItem->iRow > nMaxRow || pItem->iColumn != 1)	return;

	pStGrdParam = pArrGrdParam->GetAt(pItem->iRow);
	ST_SETTING_PARAM stParam = CSettingManager::GetInstance()->GetSetting()->GetParam(EN_SETTING_TAB::OPTION, pStGrdParam->nIndexParam);

	if ((UINT)stParam.GetUILevel() > g_loginLevel || eLOGIN_LEVEL_UNKNOWN == g_loginLevel)
	{
		CDlgMesg dlgMesg;
		dlgMesg.MyDoModal(L"로그인 레벨 제한! \n해당 파라미터를 수정할 수 없습니다.", 0x01);
		return;
	}

	CString strValue = m_pGrd[eCONF_SETTING_OPTION_GRID2].GetItemText(pItem->iRow, 1);

	//바로 파람수정
	BOOL bIsTrue = FALSE;
	if (_T("YES") == strValue)
	{
		bIsTrue = TRUE;
		//임시 저장관리
		pStGrdParam->strValue = _T("1");
	}
	else
	{
		bIsTrue = FALSE;
		//임시 저장관리
		pStGrdParam->strValue = _T("0");
	}
}

/*
 desc : 객체 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgConfSetting::InitObject()
{

	return TRUE;
}

/*
 desc : 객체 해제
 parm : None
 retn : None
*/
VOID CDlgConfSetting::CloseObject()
{
}

/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgConfSetting::OnBtnClick(UINT32 id)
{
	switch (id)
	{
	case IDC_CONF_SETTING_OPTION_PREV:	
		if (0 < m_nSelectPage)
		{
			//이동전 세이브
			//Save();
			//
			m_nSelectPage--;
			UpdateGridParam();
		}
		break;
	case IDC_CONF_SETTING_OPTION_NEXT:	
		if (m_nMaxPage - 1 > m_nSelectPage + 1)
		{
			//이동전 세이브
			//Save();
			//
			m_nSelectPage++;
			UpdateGridParam();
		}
		break;
	case IDC_CONF_SETTING_OPTION_SAVE:	
		SaveSettingParam();
		break;
	}
}

/*
 desc : 현재 실행된 자식 윈도 메모리 해제
 parm : None
 retn : None
*/
VOID CDlgConfSetting::DeleteMenu()
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
BOOL CDlgConfSetting::CreateMenu(UINT32 id)
{
	UINT32 u32DlgID = 0;

	/* 기존 실행된 자식 윈도가 있다면 메모리 해제 */
	DeleteMenu();

	/* 생성하고자 하는 윈도 (메뉴 다이얼로그) ID 설정 */
// 	u32DlgID = IDD_CALB_ACCURACY_MEASURE + (id - IDC_CALB_ACCURACY_BTN_SUBMENU_POWER_MEASURE);
// 	/* 해당 자식 (메뉴) 윈도 메모리 할당 */
// 	switch (id)
// 	{
// 	case IDC_CALB_ACCURACY_BTN_SUBMENU_POWER_MEASURE: m_pDlgMenu = new CDlgConfSettingMeasure(u32DlgID, this);	break;
// 	}
// 
// 	/* 자식 (메뉴) 윈도 생성 */
// 	if (!m_pDlgMenu->Create(m_pic_ctl[eCALB_PIC_MENU].GetSafeHwnd()))
// 	{
// 		delete m_pDlgMenu;
// 		m_pDlgMenu = NULL;
// 		return FALSE;
// 	}

	return TRUE;
}

BOOL CDlgConfSetting::PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts/* = 0*/)
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

BOOL CDlgConfSetting::SelectPath(CString& strPath)
{
	TCHAR tzPath[MAX_PATH_LEN] = { NULL };
	if (!uvCmn_GetSelectPath(m_hWnd, tzPath))
	{
		return FALSE;
	}
	strPath = (LPCTSTR)tzPath;
	return TRUE;
}

VOID CDlgConfSetting::LoadConfig()
{
	LPG_CIEA pstCfg = uvEng_GetConfig();

	/* 파일에 저장된 환경 파일 부터 적재 */
	uvEng_ReLoadConfig();
	/* Calibration file 재적재 */
	uvEng_ReloadCaliFile();

	LoadCommon(pstCfg);
	LoadAlignCamera(pstCfg);
	LoadTempRange(pstCfg);
	LoadEtc(pstCfg);
	LoadLuria(pstCfg);
	LoadTrigGrab(pstCfg);
	LoadTransformation(pstCfg);
#if 0	/* Not currently in use */
	LoadMarkFind(pstCfg);
#endif
}

/*
 desc : 환경 설정 파일 저장
 parm : None
 retn : None
*/
VOID CDlgConfSetting::SaveConfig()
{
	LPG_CIEA pstCfg = uvEng_GetConfig();
	SaveCommon(pstCfg);
	SaveAlignCamera(pstCfg);
	SaveTempRange(pstCfg);
	SaveEtc(pstCfg);
	SaveLuria(pstCfg);
	SaveTrigGrab(pstCfg);
	SaveTransformation(pstCfg);
#if 0	/* Not currently in use */
	SaveMarkFind(pstCfg);
#endif
	/* 전체 환경 파일 저장 */
	uvEng_SaveConfig();
}

/*
 desc : 환경 설정 파일 적재 - Common
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::LoadCommon(LPG_CIEA conf)
{
	UINT8 u8Check = 0x00;
	LPG_CCGS pstConf = &conf->set_comn;
	LPG_CIPC pstUvdi15 = &conf->set_uvdi15;

	/* Gerber path */
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::GERBER_PATH, (LPCTSTR)pstConf->gerber_path);
	/* Strobe Type */
	//항목에 없으므로 패스
	//m_chk_ctl[eCONF_CHK_LAMP_TYPE_1].SetCheck(!pstConf->strobe_lamp_type);
	//m_chk_ctl[eCONF_CHK_LAMP_TYPE_2].SetCheck(pstConf->strobe_lamp_type);
	/* Align Camera Type */
	//항목에 없으므로 패스
	//u8Check = ENG_VCPK(pstConf->align_camera_kind) == ENG_VCPK::en_camera_basler_ipv4 ? 1 : 0;
	//m_chk_ctl[eCONF_CHK_CAMERA_BASLER].SetCheck(u8Check);
	//u8Check = ENG_VCPK(pstConf->align_camera_kind) == ENG_VCPK::en_camera_ids_ph ? 1 : 0;
	//m_chk_ctl[eCONF_CHK_CAMERA_IDS].SetCheck(u8Check);
	/* Whether to save mark grabbed file */
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::GRAB_MARK_SAVED, pstConf->grab_mark_saved);
	/* Whether to save log file */
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::LOG_FILE_SAVED, pstConf->log_file_saved);
	/* Run Recipe Homing */
	//항목에 없으므로 패스
	//m_chk_ctl[eCONF_CHK_RUN_RECIPE_HOMING].SetCheck(pstUvdi15->load_recipe_homing);
	/* Material Detecting */
	//항목에 없으므로 패스
	//m_chk_ctl[eCONF_CHK_MATERIAL_DETECT].SetCheck(pstConf->run_material_detect);
	
}

/*
 desc : 환경 설정 파일 저장 - Common
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::SaveCommon(LPG_CIEA conf)
{
	UINT8 u8Check = 0x00;
	LPG_CCGS pstConf = &conf->set_comn;
	LPG_CIPC pstUvdi15 = &conf->set_uvdi15;
	CUniToChar csCnv;
	CString strValue;
	/* Gerber path */
	wmemset(pstConf->gerber_path, 0x00, MAX_PATH_LEN);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::GERBER_PATH);
	_tcscpy_s(pstConf->gerber_path, MAX_PATH_LEN, strValue.GetBuffer());
	strValue.ReleaseBuffer();
	/* Strobe Type */
	//항목에 없으므로 패스
	//pstConf->strobe_lamp_type = (UINT8)m_chk_ctl[eCONF_CHK_LAMP_TYPE_2].GetCheck();
	/* Align Camera Type */
	//항목에 없으므로 패스
// 	pstConf->align_camera_kind = 0x00;
// 	if (m_chk_ctl[eCONF_CHK_CAMERA_BASLER].GetCheck())	pstConf->align_camera_kind = (UINT8)ENG_VCPK::en_camera_basler_ipv4;
// 	if (m_chk_ctl[eCONF_CHK_CAMERA_IDS].GetCheck())	pstConf->align_camera_kind = (UINT8)ENG_VCPK::en_camera_ids_ph;
	/* Whether to save mark grabbed file */
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::GRAB_MARK_SAVED);
	pstConf->grab_mark_saved = _ttoi(strValue);
	/* Whether to save log file */
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::LOG_FILE_SAVED);
	pstConf->log_file_saved = _ttoi(strValue);
	/* Run recipe homing */
	//항목에 없으므로 패스
	//pstUvdi15->load_recipe_homing = (UINT8)m_chk_ctl[eCONF_CHK_RUN_RECIPE_HOMING].GetCheck();
	/* Material Detecting */
	//항목에 없으므로 패스
	//pstConf->run_material_detect = (UINT8)m_chk_ctl[eCONF_CHK_MATERIAL_DETECT].GetCheck();
}

/*
 desc : 환경 설정 파일 설정 - Align Camera의 Gain 값 저장 후, 카메라 파라미터 다시 적용
 parm : None
 retn : None
*/
VOID CDlgConfSetting::SetAnalogGain()
{
#if 0
	LPG_CCDB pstBasler = &uvEng_GetConfig()->set_basler;
	/* Analog Gain */
	pstBasler->cam_gain_level[0] = (UINT8)m_edt_int[eCONF_EDT_INT_ACAM_ANALOG_GAIN_1].GetTextToNum();
	pstBasler->cam_gain_level[1] = (UINT8)m_edt_int[eCONF_EDT_INT_ACAM_ANALOG_GAIN_2].GetTextToNum();

	/* 얼라인 카메라의 파라미터 값 다시 적용하기 위한 호출 */
	if (!uvEng_Camera_UpdateSetParam())
	{
		AfxMessageBox(L"Failed to set the parameter for align camera", MB_ICONSTOP | MB_TOPMOST);
	}
#else
 	//uvEng_Camera_SetGainLevel(0x01, (UINT8)m_edt_int[eCONF_EDT_INT_ACAM_ANALOG_GAIN_1].GetTextToNum());
 	//uvEng_Camera_SetGainLevel(0x02, (UINT8)m_edt_int[eCONF_EDT_INT_ACAM_ANALOG_GAIN_2].GetTextToNum());
#endif
}

/*
 desc : 온조기 온도 제어
 parm : None
 retn : None
*/
VOID CDlgConfSetting::SetTempHotAir()
{

}

/*
 desc : 환경 설정 파일 적재 - Align Camera
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::LoadAlignCamera(LPG_CIEA conf)
{
	LPG_CSCI pstCams = &conf->set_cams;
	LPG_CASI pstSpec = &conf->acam_spec;
	LPG_CCDB pstBasler = &conf->set_basler;
	/* ACam Width / Height */
	//항목에 없으므로 패스
	//m_edt_int[eCONF_EDT_INT_ACAM_SPEC_HORZ].SetTextToNum(pstCams->acam_size[0]);
	//m_edt_int[eCONF_EDT_INT_ACAM_SPEC_VERT].SetTextToNum(pstCams->acam_size[1]);
	/* 스펙 검사를 위한 좌/우 이동 거리 */
	//항목에 없으므로 패스
	//m_edt_flt[eCONF_EDT_FLOAT_ACAM_POINT_DIST].SetTextToNum(pstSpec->point_dist, 4);
	/* Install Angle */
	//항목에 없으므로 패스
	//m_chk_ctl[eCONF_CHK_ACAM_ROTATE_INST].SetCheck(pstCams->acam_inst_angle);
	/* Analog Gain */
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::CAM_1_GAIN_LEVEL, pstBasler->cam_gain_level[0]);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::CAM_2_GAIN_LEVEL, pstBasler->cam_gain_level[1]);

	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::CAM_REVERSE_X, pstBasler->cam_reverse_x);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::CAM_REVERSE_Y, pstBasler->cam_reverse_y);
 	
	//CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::CAM_IPv4_1, pstBasler->cam_ip[0]);
 	//CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::CAM_IPv4_2, pstBasler->cam_ip[1]);
}

/*
 desc : 환경 설정 파일 저장 - Align Camera
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::SaveAlignCamera(LPG_CIEA conf)
{
	LPG_CSCI pstCams = &conf->set_cams;
	LPG_CASI pstSpec = &conf->acam_spec;
	LPG_CCDB pstBasler = &conf->set_basler;
	CString strValue;
	/* ACam Width / Height */
	//항목에 없으므로 패스
	//pstCams->acam_size[0] = (UINT16)m_edt_int[eCONF_EDT_INT_ACAM_SPEC_HORZ].GetTextToNum();
	//pstCams->acam_size[1] = (UINT16)m_edt_int[eCONF_EDT_INT_ACAM_SPEC_VERT].GetTextToNum();
	/* 스펙 검사를 위한 좌/우 이동 거리 */
	//항목에 없으므로 패스
	//pstSpec->point_dist = m_edt_flt[eCONF_EDT_FLOAT_ACAM_POINT_DIST].GetTextToDouble();
	/* Install Angle */
	//항목에 없으므로 패스
	//pstCams->acam_inst_angle = (UINT8)m_chk_ctl[eCONF_CHK_ACAM_ROTATE_INST].GetCheck();
	/* Analog Gain */
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::CAM_1_GAIN_LEVEL);
	pstBasler->cam_gain_level[0] = _ttoi(strValue);
	//SAVE 시 바로 셋 하도록 추가
	uvEng_Camera_SetGainLevel(0x01, pstBasler->cam_gain_level[0]);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::CAM_2_GAIN_LEVEL);
	pstBasler->cam_gain_level[1] = _ttoi(strValue);
	//SAVE 시 바로 셋 하도록 추가
	uvEng_Camera_SetGainLevel(0x02, pstBasler->cam_gain_level[1]);	
}

/*
 desc : 환경 설정 파일 적재 - Temperature & Range
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::LoadTempRange(LPG_CIEA conf)
{
// 	LPG_DITR pstConf = &conf->temp_range;
// 	m_edt_flt[eCONF_EDT_FLOAT_TEMP_HOTAIR_SET].SetTextToNum(pstConf->hot_air[0], 1);
// 	m_edt_flt[eCONF_EDT_FLOAT_TEMP_HOTAIR_RANGE].SetTextToNum(pstConf->hot_air[1], 1);
// 	m_edt_flt[eCONF_EDT_FLOAT_TEMP_DI_SET].SetTextToNum(pstConf->di_internal[0], 1);
// 	m_edt_flt[eCONF_EDT_FLOAT_TEMP_DI_RANGE].SetTextToNum(pstConf->di_internal[1], 1);
// 	m_edt_flt[eCONF_EDT_FLOAT_TEMP_LED_SET].SetTextToNum(pstConf->optic_led[0], 1);
// 	m_edt_flt[eCONF_EDT_FLOAT_TEMP_LED_RANGE].SetTextToNum(pstConf->optic_led[1], 1);
// 	m_edt_flt[eCONF_EDT_FLOAT_TEMP_BD_SET].SetTextToNum(pstConf->optic_board[0], 1);
// 	m_edt_flt[eCONF_EDT_FLOAT_TEMP_BD_RANGE].SetTextToNum(pstConf->optic_board[1], 1);
}

/*
 desc : 환경 설정 파일 저장 - Temperature & Range
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::SaveTempRange(LPG_CIEA conf)
{
// 	LPG_DITR pstConf = &conf->temp_range;
// 	pstConf->hot_air[0] = m_edt_flt[eCONF_EDT_FLOAT_TEMP_HOTAIR_SET].GetTextToDouble();
// 	pstConf->hot_air[1] = m_edt_flt[eCONF_EDT_FLOAT_TEMP_HOTAIR_RANGE].GetTextToDouble();
// 	pstConf->di_internal[0] = m_edt_flt[eCONF_EDT_FLOAT_TEMP_DI_SET].GetTextToDouble();
// 	pstConf->di_internal[1] = m_edt_flt[eCONF_EDT_FLOAT_TEMP_DI_RANGE].GetTextToDouble();
// 	pstConf->optic_led[0] = m_edt_flt[eCONF_EDT_FLOAT_TEMP_LED_SET].GetTextToDouble();
// 	pstConf->optic_led[1] = m_edt_flt[eCONF_EDT_FLOAT_TEMP_LED_RANGE].GetTextToDouble();
// 	pstConf->optic_board[0] = m_edt_flt[eCONF_EDT_FLOAT_TEMP_BD_SET].GetTextToDouble();
// 	pstConf->optic_board[1] = m_edt_flt[eCONF_EDT_FLOAT_TEMP_BD_RANGE].GetTextToDouble();
}

/*
 desc : 환경 설정 파일 적재 - ETC
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::LoadEtc(LPG_CIEA conf)
{
}

/*
 desc : 환경 설정 파일 저장 - ETC
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::SaveEtc(LPG_CIEA conf)
{
}

/*
 desc : 환경 설정 파일 적재 - Luria
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::LoadLuria(LPG_CIEA conf)
{
	// 모터 TEACH 값은 TEACH 메뉴에서 처리
// 	LPG_LDSM pstLuria = uvEng_ShMem_GetLuria();
// 	m_edt_flt[eCONF_EDT_FLOAT_EXPO_START_X].SetTextToNum(pstLuria->machine.table_expo_start_xy[0].x / 1000.0f, 3);
// 	m_edt_flt[eCONF_EDT_FLOAT_EXPO_START_Y].SetTextToNum(pstLuria->machine.table_expo_start_xy[0].y / 1000.0f, 3);
// 	m_edt_flt[eCONF_EDT_FLOAT_EXPO_START_X].Invalidate();
// 	m_edt_flt[eCONF_EDT_FLOAT_EXPO_START_Y].Invalidate();
}

/*
 desc : 환경 설정 파일 저장 - Luria
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::SaveLuria(LPG_CIEA conf)
{
	// 모터 TEACH 값은 TEACH 메뉴에서 처리
// 	LPG_LDSM pstLuria = uvEng_ShMem_GetLuria();
// 	conf->luria_svc.table_expo_start_xy[0][0] = pstLuria->machine.table_expo_start_xy[0].x / 1000.0f;
// 	conf->luria_svc.table_expo_start_xy[0][1] = pstLuria->machine.table_expo_start_xy[0].y / 1000.0f;
// }
// 
// /*
//  desc : 노광 시작 위치 설정
//  parm : None
//  retn : None
// */
// VOID CDlgConfSetting::SetExpoStartXY()
// {
// 	DOUBLE dbStartX = m_edt_flt[eCONF_EDT_FLOAT_EXPO_START_X].GetTextToDouble();
// 	DOUBLE dbStartY = m_edt_flt[eCONF_EDT_FLOAT_EXPO_START_Y].GetTextToDouble();
// 
// 	/* 노광 시작 위치 설정 */
// 	uvEng_Luria_ReqSetTableExposureStartPos(0x01,
// 		(UINT32)ROUNDED(dbStartX * 1000.0f, 0),
// 		(UINT32)ROUNDED(dbStartY * 1000.0f, 0));
}

/*
 desc : 환경 설정 파일 적재 - Trigger Grab
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::LoadTrigGrab(LPG_CIEA conf)
{
	UINT8 i = 0x03;

	/* Trigger On Time */
	//항목에 없으므로 패스
	//m_edt_int[eCONF_EDT_INT_TRIG_ON_TIME_1].SetTextToNum((UINT32)conf->trig_grab.trig_on_time[0]);
	//m_edt_int[eCONF_EDT_INT_TRIG_ON_TIME_2].SetTextToNum((UINT32)conf->trig_grab.trig_on_time[1]);
	/* Strobe On Time */
	//항목에 없으므로 패스
	//m_edt_int[eCONF_EDT_INT_STROBE_ON_TIME_1].SetTextToNum((UINT32)conf->trig_grab.strob_on_time[0]);
	//m_edt_int[eCONF_EDT_INT_STROBE_ON_TIME_2].SetTextToNum((UINT32)conf->trig_grab.strob_on_time[1]);
	/* Trigger Delay Time */
	//항목에 없으므로 패스
	//m_edt_int[eCONF_EDT_INT_TRIG_DELAY_TIME_1].SetTextToNum((UINT32)conf->trig_grab.trig_delay_time[0]);
	//m_edt_int[eCONF_EDT_INT_TRIG_DELAY_TIME_2].SetTextToNum((UINT32)conf->trig_grab.trig_delay_time[1]);
	/* Trigger Plus Time */
#if 0
	m_edt_int[eCONF_EDT_INT_TRIG_PLUS_1].SetTextToNum((UINT32)conf->trig_set.trig_plus[0]);
	m_edt_int[eCONF_EDT_INT_TRIG_PLUS_2].SetTextToNum((UINT32)conf->trig_grab.trig_plus[1]);
#else
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TRIG_FORWARD, conf->trig_grab.trig_forward);
	//	m_edt_int[eCONF_EDT_INT_TRIG_PLUS_2].SetTextToNum(conf->trig_grab.trig_forward);
#endif
	/* Trigger Minus Time */
#if 0
	m_edt_int[eCONF_EDT_INT_TRIG_MINUS_1].SetTextToNum(conf->trig_grab.trig_plus[0]);
	//	m_edt_int[eCONF_EDT_INT_TRIG_MINUS_2].SetTextToNum(conf->trig_grab.trig_plus[1]);
#else
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TRIG_BACKWARD, conf->trig_grab.trig_backward);
	//	m_edt_int[eCONF_EDT_INT_TRIG_MINUS_2].SetTextToNum(conf->trig_grab.trig_backward);
#endif
	// refresh는 다른곳에서 하므로 주석
	//for (; i < eCONF_EDT_INT_MAX; i++)	m_edt_int[i].Invalidate();
}

/*
 desc : 환경 설정 파일 저장 - Trigger Grab
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::SaveTrigGrab(LPG_CIEA conf)
{
	CString strValue;
	/* Trigger On Time */
	//항목에 없으므로 패스
	//conf->trig_grab.trig_on_time[0] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_ON_TIME_1].GetTextToNum();
	//conf->trig_grab.trig_on_time[1] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_ON_TIME_2].GetTextToNum();
	/* Strobe On Time */
	//항목에 없으므로 패스
	//conf->trig_grab.strob_on_time[0] = (UINT32)m_edt_int[eCONF_EDT_INT_STROBE_ON_TIME_1].GetTextToNum();
	//conf->trig_grab.strob_on_time[1] = (UINT32)m_edt_int[eCONF_EDT_INT_STROBE_ON_TIME_2].GetTextToNum();
	/* Trigger Delay Time */
	//항목에 없으므로 패스
	//conf->trig_grab.trig_delay_time[0] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_DELAY_TIME_1].GetTextToNum();
	//conf->trig_grab.trig_delay_time[1] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_DELAY_TIME_2].GetTextToNum();
#if 0
	/* Trigger Plus Time */
	conf->trig_grab.trig_plus[0] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_PLUS_1].GetTextToNum();
	conf->trig_grab.trig_plus[1] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_PLUS_2].GetTextToNum();
	conf->trig_grab.trig_plus[2] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_PLUS_1].GetTextToNum();
	conf->trig_grab.trig_plus[3] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_PLUS_2].GetTextToNum();
	/* Trigger Minus Time */
	conf->trig_grab.trig_minus[0] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_MINUS_1].GetTextToNum();
	conf->trig_grab.trig_minus[1] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_MINUS_2].GetTextToNum();
	conf->trig_grab.trig_minus[2] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_MINUS_1].GetTextToNum();
	conf->trig_grab.trig_minus[3] = (UINT32)m_edt_int[eCONF_EDT_INT_TRIG_MINUS_2].GetTextToNum();
#else
	/* Trigger Plus count */
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TRIG_FORWARD);
	conf->trig_grab.trig_forward = _ttoi(strValue);
	/* Trigger Minus count */
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TRIG_BACKWARD);
	conf->trig_grab.trig_backward = _ttoi(strValue);
#endif
}

/*
 설명 : Panel의 Transformation 값 읽어들이기
 변수 : conf	- [in]  환경 파일 구조체 포인터
 반환 : None
*/
VOID CDlgConfSetting::LoadTransformation(LPG_CIEA conf)
{
	CString strTemp;
	BOOL bEnable = TRUE;

	//m_chk_ctl[eCONF_CHK_GLOBAL_RECT].SetCheck(conf->global_trans.use_rectangle);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_RECTANGLE, conf->global_trans.use_rectangle);
	//m_chk_ctl[eCONF_CHK_GLOBAL_ROTATION].SetCheck(conf->global_trans.use_rotation_mode);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_ROTATION, conf->global_trans.use_rotation_mode);
	//m_chk_ctl[eCONF_CHK_GLOBAL_SCALE].SetCheck(conf->global_trans.use_scaling_mode);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_SCALING, conf->global_trans.use_scaling_mode);
	//m_chk_ctl[eCONF_CHK_GLOBAL_OFFSET].SetCheck(conf->global_trans.use_offset_mode);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_OFFSET, conf->global_trans.use_offset_mode);
	//m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_ROTATE].SetTextToNum(conf->global_trans.rotation / 1000.0f, 3);
	strTemp.Format(_T("%.3f"), conf->global_trans.rotation / 1000.0f);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_ROTATION, strTemp);
	//m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_SCALE_X].SetTextToNum(conf->global_trans.scaling[0] / 1000000.0f, 6);
	strTemp.Format(_T("%.6f"), conf->global_trans.scaling[0] / 1000000.0f);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_SCALING_X, strTemp);
	//m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_SCALE_Y].SetTextToNum(conf->global_trans.scaling[1] / 1000000.0f, 6);
	strTemp.Format(_T("%.6f"), conf->global_trans.scaling[1] / 1000000.0f);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_SCALING_Y, strTemp);
	//m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_OFFSET_X].SetTextToNum(conf->global_trans.offset[0] / 1000000.0f, 6);
	strTemp.Format(_T("%.6f"), conf->global_trans.offset[0] / 1000000.0f);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_OFFSET_X, strTemp);
	//m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_OFFSET_Y].SetTextToNum(conf->global_trans.offset[1] / 1000000.0f, 6);
	strTemp.Format(_T("%.6f"), conf->global_trans.offset[1] / 1000000.0f);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_OFFSET_Y, strTemp);

// 	bEnable = m_chk_ctl[eCONF_CHK_GLOBAL_ROTATION].GetCheck() == 1;
// 	m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_ROTATE].EnableWindow(bEnable);
// 	bEnable = m_chk_ctl[eCONF_CHK_GLOBAL_SCALE].GetCheck() == 1;
// 	m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_SCALE_X].EnableWindow(bEnable);
// 	m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_SCALE_Y].EnableWindow(bEnable);
// 	bEnable = m_chk_ctl[eCONF_CHK_GLOBAL_OFFSET].GetCheck() == 1;
// 	m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_OFFSET_X].EnableWindow(bEnable);
// 	m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_OFFSET_Y].EnableWindow(bEnable);
}

/*
 설명 : Panel의 Transformation 값 저장하기
 변수 : conf	- [in]  환경 파일 구조체 포인터
 반환 : None
*/
VOID CDlgConfSetting::SaveTransformation(LPG_CIEA conf)
{
	CString strValue;
	//conf->global_trans.use_rectangle = (UINT8)m_chk_ctl[eCONF_CHK_GLOBAL_RECT].GetCheck();
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_RECTANGLE);
	conf->global_trans.use_rectangle = _ttoi(strValue);
	//conf->global_trans.use_rotation_mode = (UINT8)m_chk_ctl[eCONF_CHK_GLOBAL_ROTATION].GetCheck();
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_ROTATION);
	conf->global_trans.use_rotation_mode = _ttoi(strValue);
	//conf->global_trans.use_scaling_mode = (UINT8)m_chk_ctl[eCONF_CHK_GLOBAL_SCALE].GetCheck();
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_SCALING);
	conf->global_trans.use_scaling_mode = _ttoi(strValue);
	//conf->global_trans.use_offset_mode = (UINT8)m_chk_ctl[eCONF_CHK_GLOBAL_OFFSET].GetCheck();
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_OFFSET);
	conf->global_trans.use_offset_mode = _ttoi(strValue);

	//conf->global_trans.rotation = (INT32)ROUNDED(m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_ROTATE].GetTextToDouble() * 1000.0f, 0);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_ROTATION);
	conf->global_trans.rotation = (INT32)ROUNDED(_ttof(strValue) * 1000.0f, 0);
	//conf->global_trans.scaling[0] = (UINT32)ROUNDED(m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_SCALE_X].GetTextToDouble() * 1000000.0f, 0);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_SCALING_X);
	conf->global_trans.scaling[0] = (INT32)ROUNDED(_ttof(strValue) * 1000000.0f, 0);
	//conf->global_trans.scaling[1] = (UINT32)ROUNDED(m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_SCALE_Y].GetTextToDouble() * 1000000.0f, 0);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_SCALING_Y);
	conf->global_trans.scaling[1] = (INT32)ROUNDED(_ttof(strValue) * 1000000.0f, 0);
	//conf->global_trans.offset[0] = (UINT32)ROUNDED(m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_OFFSET_X].GetTextToDouble() * 1000000.0f, 0);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_OFFSET_X);
	conf->global_trans.offset[0] = (INT32)ROUNDED(_ttof(strValue) * 1000000.0f, 0);
	//conf->global_trans.offset[1] = (UINT32)ROUNDED(m_edt_flt[eCONF_EDT_FLOAT_TRANS_GBL_OFFSET_Y].GetTextToDouble() * 1000000.0f, 0);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_OFFSET_Y);
	conf->global_trans.offset[1] = (INT32)ROUNDED(_ttof(strValue) * 1000000.0f, 0);
}
