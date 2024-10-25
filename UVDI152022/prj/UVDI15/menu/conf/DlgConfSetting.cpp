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
	DeleteMenu();
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
	//for (int i=0; i < EN_SETTING_OPTION::MAX; i++)
	//{
	//	switch (i)
	//	{
	//	case EN_SETTING_OPTION::LOG_FILE_SAVED:
	//		break;
	//	default:
	//		break;
	//	}
	//}
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
		if (PopupKBDN(ENM_DITM::en_float, _T(""), strValue, stParam.dMin, stParam.dMax,3))
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
		if (PopupKBDN(ENM_DITM::en_float, _T(""), strValue, stParam.dMin, stParam.dMax, 3))
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

	LoadSetupAlign(pstCfg);
	LoadMarkFind(pstCfg);
	LoadStrobeLamp(pstCfg);
	LoadFlatness(pstCfg);
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

	SaveSetupAlign(pstCfg);
	SeveMarkFind(pstCfg);
	SaveStrobeLamp(pstCfg);
	SaveFlatness(pstCfg);
#if 0	/* Not currently in use */
	SaveMarkFind(pstCfg);
#endif
	/* 전체 환경 파일 저장 */
	uvEng_SaveConfig();

	CDlgMesg dlgMesg;
	dlgMesg.MyDoModal(L"Config Save succeed", 0x01);
}


/*
 desc : 환경 설정 파일 설정 - Align Camera의 Gain 값 저장 후, 카메라 파라미터 다시 적용
 parm : None
 retn : None
*/
VOID CDlgConfSetting::SetAnalogGain()
{

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

	/* Analog Gain */
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::CAM_1_GAIN_LEVEL, pstBasler->cam_gain_level[0]);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::CAM_2_GAIN_LEVEL, pstBasler->cam_gain_level[1]);

	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TIME_GRAB_1, (int)pstBasler->grab_times[0]);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TIME_STEP_1, (int)pstBasler->step_times[0]);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TIME_GRAB_2, (int)pstBasler->grab_times[1]);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TIME_STEP_2, (int)pstBasler->step_times[1]);

	//CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::CAM_REVERSE_X, pstBasler->cam_reverse_x);
	//CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::CAM_REVERSE_Y, pstBasler->cam_reverse_y);


#if(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::CAM_3_GAIN_LEVEL, pstBasler->cam_gain_level[2]);

	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TIME_GRAB_3, (int)pstBasler->grab_times[2]);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TIME_STEP_3, (int)pstBasler->step_times[2]);
#endif

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

	/* Analog Gain */
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::CAM_1_GAIN_LEVEL);
	pstBasler->cam_gain_level[0] = _ttoi(strValue);
	//SAVE 시 바로 셋 하도록 추가
	uvEng_Camera_SetGainLevel(0x01, pstBasler->cam_gain_level[0]);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::CAM_2_GAIN_LEVEL);
	pstBasler->cam_gain_level[1] = _ttoi(strValue);
	uvEng_Camera_SetGainLevel(0x02, pstBasler->cam_gain_level[1]);	


#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	if (conf->set_cams.acam_count == 3)
	{
		strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::CAM_3_GAIN_LEVEL);
		pstBasler->cam_gain_level[2] = _ttoi(strValue);
		uvEng_Camera_SetGainLevel(0x02, pstBasler->cam_gain_level[2]);
	}
#endif

	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TIME_GRAB_1);
	pstBasler->grab_times[0] = _ttoi(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TIME_STEP_1);
	pstBasler->step_times[0] = _ttoi(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TIME_GRAB_2);
	pstBasler->grab_times[1] = _ttoi(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TIME_STEP_2);
	pstBasler->step_times[1] = _ttoi(strValue);


#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	if (conf->set_cams.acam_count == 3)
	{
		strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TIME_GRAB_3);
		pstBasler->grab_times[2] = _ttoi(strValue);
		strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TIME_STEP_3);
		pstBasler->step_times[2] = _ttoi(strValue);
	}
#endif

}

/*
 desc : 환경 설정 파일 적재 - Temperature & Range
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::LoadTempRange(LPG_CIEA conf)
{
}

/*
 desc : 환경 설정 파일 저장 - Temperature & Range
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::SaveTempRange(LPG_CIEA conf)
{
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
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_AF,			conf->luria_svc.useAF);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::AF_GAIN,			conf->luria_svc.afGain);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::AF_RANGE_MIN,		conf->luria_svc.afWorkRangeWithinMicrometer[0]);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::AF_RANGE_MAX,		conf->luria_svc.afWorkRangeWithinMicrometer[1]);
}

/*
 desc : 환경 설정 파일 저장 - Luria
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::SaveLuria(LPG_CIEA conf)
{
	CString strValue;

	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_AF);
	conf->luria_svc.useAF = _ttoi(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::AF_GAIN);
	conf->luria_svc.afGain = _ttoi(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::AF_RANGE_MIN);
	conf->luria_svc.afWorkRangeWithinMicrometer[0] = _ttof(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::AF_RANGE_MAX);
	conf->luria_svc.afWorkRangeWithinMicrometer[1] = _ttof(strValue);
}

/*
 desc : 환경 설정 파일 적재 - Trigger Grab
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::LoadTrigGrab(LPG_CIEA conf)
{
	//CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TRIG_FORWARD, conf->trig_grab.trig_forward);
	//CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TRIG_BACKWARD, conf->trig_grab.trig_backward);
}

/*
 desc : 환경 설정 파일 저장 - Trigger Grab
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::SaveTrigGrab(LPG_CIEA conf)
{
	CString strValue;

	///* Trigger Plus count */
	//strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TRIG_FORWARD);
	//conf->trig_grab.trig_forward = _ttoi(strValue);
	///* Trigger Minus count */
	//strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::TRIG_BACKWARD);
	//conf->trig_grab.trig_backward = _ttoi(strValue);

}

/*
 설명 : Panel의 Transformation 값 읽어들이기
 변수 : conf	- [in]  환경 파일 구조체 포인터
 반환 : None
*/
VOID CDlgConfSetting::LoadTransformation(LPG_CIEA conf)
{
	//CString strTemp;
	//BOOL bEnable = TRUE;

	//CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_RECTANGLE, conf->global_trans.use_rectangle);
	//CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_ROTATION, conf->global_trans.use_rotation_mode);
	//CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_SCALING, conf->global_trans.use_scaling_mode);
	//CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_OFFSET, conf->global_trans.use_offset_mode);
	//strTemp.Format(_T("%.3f"), conf->global_trans.rotation / 1000.0f);
	//CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_ROTATION, strTemp);
	//strTemp.Format(_T("%.6f"), conf->global_trans.scaling[0] / 1000000.0f);
	//CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_SCALING_X, strTemp);
	//strTemp.Format(_T("%.6f"), conf->global_trans.scaling[1] / 1000000.0f);
	//CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_SCALING_Y, strTemp);
	//strTemp.Format(_T("%.6f"), conf->global_trans.offset[0] / 1000000.0f);
	//CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_OFFSET_X, strTemp);
	//strTemp.Format(_T("%.6f"), conf->global_trans.offset[1] / 1000000.0f);
	//CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_OFFSET_Y, strTemp);
}

/*
 설명 : Panel의 Transformation 값 저장하기
 변수 : conf	- [in]  환경 파일 구조체 포인터
 반환 : None
*/
VOID CDlgConfSetting::SaveTransformation(LPG_CIEA conf)
{
	//CString strValue;
	//strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_RECTANGLE);
	//conf->global_trans.use_rectangle = _ttoi(strValue);
	//strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_ROTATION);
	//conf->global_trans.use_rotation_mode = _ttoi(strValue);
	//strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_SCALING);
	//conf->global_trans.use_scaling_mode = _ttoi(strValue);
	//strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_OFFSET);
	//conf->global_trans.use_offset_mode = _ttoi(strValue);

	//strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_ROTATION);
	//conf->global_trans.rotation = (INT32)ROUNDED(_ttof(strValue) * 1000.0f, 0);
	//strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_SCALING_X);
	//conf->global_trans.scaling[0] = (INT32)ROUNDED(_ttof(strValue) * 1000000.0f, 0);
	//strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_SCALING_Y);
	//conf->global_trans.scaling[1] = (INT32)ROUNDED(_ttof(strValue) * 1000000.0f, 0);
	//strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_OFFSET_X);
	//conf->global_trans.offset[0] = (INT32)ROUNDED(_ttof(strValue) * 1000000.0f, 0);
	//strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::VAL_OFFSET_Y);
	//conf->global_trans.offset[1] = (INT32)ROUNDED(_ttof(strValue) * 1000000.0f, 0);
}

/*
 desc : 환경 설정 파일 적재 - Setup Align
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::LoadSetupAlign(LPG_CIEA conf)
{
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_2D_CALI_DATA,		conf->set_align.use_2d_cali_data);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::MARK2_ORG_GERB_X,		conf->set_align.mark2_org_gerb_xy[0]);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::MARK2_ORG_GERB_Y,		conf->set_align.mark2_org_gerb_xy[1]);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::MARK2_STAGE_X,		conf->set_align.mark2_stage_x);
}

/*
 desc : 환경 설정 파일 저장 - Setip Align
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::SaveSetupAlign(LPG_CIEA conf)
{
	CString strValue;

	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_2D_CALI_DATA);
	conf->set_align.use_2d_cali_data = _ttoi(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::MARK2_ORG_GERB_X);
	conf->set_align.mark2_org_gerb_xy[0] = _ttof(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::MARK2_ORG_GERB_Y);
	conf->set_align.mark2_org_gerb_xy[1] = _ttof(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::MARK2_STAGE_X);
	conf->set_align.mark2_stage_x = _ttof(strValue);

}

/*
 desc : 환경 설정 파일 적재 - Mark Find
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::LoadMarkFind(LPG_CIEA conf)
{
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::SCORE_RATE, conf->mark_find.score_rate);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::SCALE_RATE, conf->mark_find.scale_rate);
}

/*
 desc : 환경 설정 파일 저장 - Mark Find
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::SeveMarkFind(LPG_CIEA conf)
{
	CString strValue;

	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::SCORE_RATE);
	conf->mark_find.score_rate = _ttof(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::SCALE_RATE);
	conf->mark_find.scale_rate = _ttof(strValue);
}

/*
 desc : 환경 설정 파일 적재 - Strobe Lamp
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::LoadStrobeLamp(LPG_CIEA conf)
{
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	int strobeValues[] = { 0,1,2,3,4,5 };
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	int strobeValues[] = { 0,1,2,4,5,6 };
#endif
	int idx = 0;
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::STROBE_LAMP_1, conf->set_strobe_lamp.u16StrobeValue[strobeValues[idx++]]);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::STROBE_LAMP_2, conf->set_strobe_lamp.u16StrobeValue[strobeValues[idx++]]);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::STROBE_LAMP_3, conf->set_strobe_lamp.u16StrobeValue[strobeValues[idx++]]);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::STROBE_LAMP_4, conf->set_strobe_lamp.u16StrobeValue[strobeValues[idx++]]);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::STROBE_LAMP_5, conf->set_strobe_lamp.u16StrobeValue[strobeValues[idx++]]);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::STROBE_LAMP_6, conf->set_strobe_lamp.u16StrobeValue[strobeValues[idx++]]);
}

/*
 desc : 환경 설정 파일 저장 - Strobe Lamp
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::SaveStrobeLamp(LPG_CIEA conf)
{
	CString strValue;

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	int strobeValues[] = { 0,1,2,3,4,5 };
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	int strobeValues[] = { 0,1,2,4,5,6 };
#endif

	int idx = 0;

	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::STROBE_LAMP_1);
	conf->set_strobe_lamp.u16StrobeValue[strobeValues[idx++]] = _ttoi(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::STROBE_LAMP_2);
	conf->set_strobe_lamp.u16StrobeValue[strobeValues[idx++]] = _ttoi(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::STROBE_LAMP_3);
	conf->set_strobe_lamp.u16StrobeValue[strobeValues[idx++]] = _ttoi(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::STROBE_LAMP_4);
	conf->set_strobe_lamp.u16StrobeValue[strobeValues[idx++]] = _ttoi(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::STROBE_LAMP_5);
	conf->set_strobe_lamp.u16StrobeValue[strobeValues[idx++]] = _ttoi(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::STROBE_LAMP_6);
	conf->set_strobe_lamp.u16StrobeValue[strobeValues[idx++]] = _ttoi(strValue);

}



/*
 desc : 환경 설정 파일 적재 - Flatness
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::LoadFlatness(LPG_CIEA conf)
{
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_THICK_CHECK, conf->measure_flat.u8UseThickCheck);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::RANG_START_Y, conf->measure_flat.dRangStartYPos);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::RANG_END_Y, conf->measure_flat.dRangEndYPos);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::OFFSET_Z, conf->measure_flat.dOffsetZPOS);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::LIMIT_Z, conf->measure_flat.dLimitZPOS);
}

/*
 desc : 환경 설정 파일 저장 - Setip Align
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::SaveFlatness(LPG_CIEA conf)
{
	CString strValue;

	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_THICK_CHECK);
	conf->measure_flat.u8UseThickCheck = _ttoi(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::RANG_START_Y);
	conf->measure_flat.dRangStartYPos = _ttof(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::RANG_END_Y);
	conf->measure_flat.dRangEndYPos = _ttof(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::OFFSET_Z);
	conf->measure_flat.dOffsetZPOS = _ttof(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::LIMIT_Z);
	conf->measure_flat.dLimitZPOS = _ttof(strValue);

}

/*
 desc : 환경 설정 파일 적재 - Common, Monitor
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::LoadCommon(LPG_CIEA conf)
{
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::LOG_DELECT_DAY, conf->monitor.log_delect_day);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::BMP_DELECT_DAY, conf->monitor.bmp_delect_day);
	CSettingManager::GetInstance()->GetSetting()->SetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_AUTO_ALIGN, conf->set_comn.use_auto_align);
}

/*
 desc : 환경 설정 파일 저장 - Common, Monitor
 parm : conf	- [in]  환경 파일 구조체 포인터
 retn : None
*/
VOID CDlgConfSetting::SaveCommon(LPG_CIEA conf)
{
	CString strValue;

	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::BMP_DELECT_DAY);
	conf->monitor.bmp_delect_day = _ttoi(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::LOG_DELECT_DAY);
	conf->monitor.log_delect_day = _ttoi(strValue);
	strValue = CSettingManager::GetInstance()->GetSetting()->GetValue(EN_SETTING_TAB::OPTION, EN_SETTING_OPTION::USE_AUTO_ALIGN);
	conf->set_comn.use_auto_align = _ttoi(strValue);

}