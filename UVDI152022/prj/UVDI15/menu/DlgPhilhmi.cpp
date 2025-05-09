
/*
 desc : Exposure
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgPhilhmi.h"

#include "./expo/DrawMark.h"	/* Mark Search Results Object */
#include "./expo/DrawDev.h"		/* TCP/IP Communication Status */
#include "./expo/MarkDist.h"	/* Distance between marks */
#include "./expo/ExpoVal.h"		/* Update to realtime value */
#include "./expo/TempAlarm.h"	/* Temperature Alarm */

#include "../pops/DlgStep.h"
#include "../pops/DlgRept.h"
#include "../mesg/DlgMesg.h"

#include "../../../inc/kybd/DlgKBDT.h"
#include <afxtaskdialog.h>

#include "../param/RecipeManager.h"



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
CDlgPhilhmi::CDlgPhilhmi(UINT32 id, CWnd* parent)
	: CDlgMenu(id, parent)
{

	/* 자신의 다이얼로그 ID 설정 */
	m_enDlgID	= ENG_CMDI::en_menu_philhmi;

	m_nSelectRecipe[eRECIPE_MODE_SEL] = -1;
	m_nSelectRecipe[eRECIPE_MODE_VIEW] = -1;

	m_nSelectRecipeOld[eRECIPE_MODE_SEL] = -1;
	m_nSelectRecipeOld[eRECIPE_MODE_VIEW] = -1;
}

CDlgPhilhmi::~CDlgPhilhmi()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgPhilhmi::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	u32StartID = DEF_UI_PHILHMI_BTN;
	for (i = 0; i < eBTN_MAX; i++)
	{
		DDX_Control(dx, u32StartID + i, m_btn[i]);
	}

	/* groupbox - normal */
	u32StartID = IDC_PHILHMI_BTN_RECIPE_ADD;
	for (i = 0; i < ePHILHMI_BTN_MAX; i++)				DDX_Control(dx, u32StartID + i, m_btn_ctl[i]);
	/* static */
	u32StartID = IDC_PHILHMI_STT_RECIPE_LIST;
	for (i = 0; i < ePHILHMI_TXT_MAX; i++)				DDX_Control(dx, u32StartID + i, m_txt_ctl[i]);
	/* btn - normal */
	u32StartID = IDC_PHILHMI_GRD_RECIPE_LIST;
	for (i = 0; i < ePHILHMI_GRD_MAX; i++)				DDX_Control(dx, u32StartID + i, m_grd_ctl[i]);

	u32StartID = IDC_PHILHMI_GRP_PHIL_LOG;
	for (i = 0; i < ePHILHMI_GRP_MAX; i++)				DDX_Control(dx, u32StartID + i, m_grp_ctl[i]);

	u32StartID = IDC_PHILHMI_BOX_PHIL_LOG;
	for (i = 0; i < ePHILHMI_BOX_MAX; i++)				DDX_Control(dx, u32StartID + i, m_box_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgPhilhmi, CDlgMenu)
	//ON_MESSAGE(WM_PHILHMI_MSG_THREAD, OnMsgMainPHILHMI)
	ON_NOTIFY(NM_CLICK, IDC_PHILHMI_GRD_RECIPE_LIST, OnClickGridRecipeList)
	ON_NOTIFY(NM_CLICK, IDC_PHILHMI_GRD_JOB_PARAMETER, OnClickGridJobParameter)
	ON_NOTIFY(NM_CLICK, IDC_PHILHMI_GRD_PHIL_PARAMETER, OnClickGridPhilParameter)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_PHILHMI_BTN_RECIPE_ADD, IDC_PHILHMI_BTN_LOG_CLEAR,	OnBtnClick)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgPhilhmi::PreTranslateMessage(MSG* msg)
{
	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgPhilhmi::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();
	if (!InitGrid())	return FALSE;
	/* 객체 초기화 */
	if (!InitObject())	return FALSE;

	TCHAR szTemp[512] = { NULL };
	swprintf_s(szTemp, 512, _T("시스템 시작") );
	AddListBox(szTemp);
	return TRUE;
}
/*


/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgPhilhmi::OnExitDlg()
{
	CloseObject();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgPhilhmi::OnPaintDlg(CDC *dc)
{
	/* 자식 윈도 호출 */
	CDlgMenu::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgPhilhmi::OnResizeDlg()
{
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgPhilhmi::UpdatePeriod(UINT64 tick, BOOL is_busy)
{
	
}

/*
 desc : 컨트롤
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgPhilhmi::UpdateControl(UINT64 tick, BOOL is_busy)
{

}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgPhilhmi::InitCtrl()
{
	CResizeUI clsResizeUI;
	INT32 i = 0;

	for (i = 0; i < eBTN_MAX; i++)
	{
		m_btn[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn[i].SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
		m_btn[i].SetTextColor(DEF_COLOR_BTN_PAGE_NORMAL_TEXT);
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn[i]);
		// by sysandj : Resize UI
	}

	/* button - normal */
	for (i = 0; i < ePHILHMI_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
		m_btn_ctl[i].SetTextColor(DEF_COLOR_BTN_PAGE_NORMAL_TEXT);
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}

	/* group box */
	for (i = 0; i < ePHILHMI_GRP_MAX; i++)
	{
		m_grp_ctl[i].SetLogFont(&g_lf[eFONT_LEVEL2_BOLD]);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_grp_ctl[i]);
		// by sysandj : Resize UI
	}
	/* static - normal */
	for (i = 0; i < ePHILHMI_TXT_MAX; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, FORD_GRAPHITE, 0, WHITE_);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_ctl[i]);
		// by sysandj : Resize UI
	}

	/* box - edge detection results */
	for (i = 0; i < 1; i++)
	{
		m_box_ctl[i].SetLogFont(&g_lf[eFONT_LEVEL2_BOLD]);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_box_ctl[i]);
		// by sysandj : Resize UI

	}
}

/*
 desc : 객체 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgPhilhmi::InitObject()
{
	
	return TRUE;
}

/*
 desc : 객체 해제
 parm : None
 retn : None
*/
VOID CDlgPhilhmi::CloseObject()
{
	DeleteGridParam(EN_RECIPE_TAB::JOB);
}

/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgPhilhmi::OnBtnClick(UINT32 id)
{
	switch (id)
	{
	case IDC_PHILHMI_BTN_RECIPE_ADD		:			RecipeControl(0x00);	break;
	case IDC_PHILHMI_BTN_RECIPE_DEL		:			RecipeControl(0x02);	break;
	case IDC_PHILHMI_BTN_RECIPE_SELECT	:			RecipeSelect();			break;
	case IDC_PHILHMI_BTN_RECIPE_SAVE	:			RecipeControl(0x01);	break;
	case IDC_PHILHMI_BTN_IO_STATUS		:			IOStatus();				break;
	case IDC_PHILHMI_BTN_STATUS_SEND	:			InterruptStop();		break;
	case IDC_PHILHMI_BTN_INTERR_STOP	:			SendStatus();			break;
	case IDC_PHILHMI_BTN_LOG_CLEAR		:			ClearListBox();			break;
	}
}


void CDlgPhilhmi::OnClickGridRecipeList(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	CString strSelectRecipe = m_grd_ctl[ePHILHMI_GRD_RECIPE_LIST].GetItemText(pItem->iRow, ePHILHMI_GRD_COL_RECIPE_LIST_NAME);
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetRecipeOnlyName(strSelectRecipe.GetBuffer());
	strSelectRecipe.ReleaseBuffer();

	if (pstRecipe)
	{
		TCHAR tzPath[MAX_PATH_LEN] = { NULL };
		CUniToChar csCnv1, csCnv2;
		swprintf_s(tzPath, MAX_PATH_LEN, L"%s\\%s",
			csCnv1.Ansi2Uni(pstRecipe->gerber_path), csCnv2.Ansi2Uni(pstRecipe->gerber_name));
		if (!uvCmn_FindPath(tzPath))
		{
			if (ShowYesNoMsg(eQUEST, _T("Failed to find the path of gerber file\nDo you want to delete the recipe?")) == IDYES)
			{
				/* 등록 성공 여부 */
				if (!CRecipeManager::GetInstance()->DeleteRecipe(strSelectRecipe))	return;

				// 삭제 후 현재 레시피로 변경
				SelectRecipe(m_nSelectRecipe[eRECIPE_MODE_SEL], eRECIPE_MODE_VIEW);

				/* 새롭게 등록된 레시피 갱신 */
				InitGridRecipeList();
				return;
			}
		}
		else
		{
			//선택되어있는것을 클릭했을때 처리
			if (m_nSelectRecipe[eRECIPE_MODE_VIEW] == pItem->iRow)
			{
				return;
			}

			//////////////////////////////////////////////////////////////////////////
			//에디팅 상태체크
			//하나라도 수정중이라면 메세지
			//if (TRUE == CheckEditing())
			//{
			//	if (ShowYesNoMsg(eQUEST, _T("수정 중입니다. 저장하지 않고 이동할 경우, 변경이 취소됩니다.\r\n이동하시겠습니까?"), 0x00) == IDNO)
			//	{
			//		return;
			//	}
			//	else //이동한다면 클리어
			//	{
			//		SetEditing(FALSE);
			//	}
			//}
			//////////////////////////////////////////////////////////////////////////
		}

		SelectRecipe(pItem->iRow, eRECIPE_MODE_VIEW);
	}
	Invalidate(TRUE);
}

void CDlgPhilhmi::OnClickGridJobParameter(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	if (ePHILHMI_GRD_COL_PARAMETER_NAME >= pItem->iColumn)
	{
		return;
	}

	CGridCtrl* pGrid = &m_grd_ctl[ePHILHMI_GRD_JOB_PARAMETER + EN_RECIPE_TAB::JOB];
	int				nParamID = _ttoi(pGrid->GetItemText(pItem->iRow, ePHILHMI_GRD_COL_PARAMETER_ID));

	if (EN_RECIPE_JOB::JOB_NAME == nParamID || EN_RECIPE_JOB::GERBER_PATH == nParamID)
	{
		return;
	}

	ST_RECIPE_PARAM stParam = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->GetParam(EN_RECIPE_TAB::JOB, nParamID);

	if ((UINT)stParam.GetUILevel() > g_loginLevel || eLOGIN_LEVEL_UNKNOWN == g_loginLevel)
	{
		CDlgMesg dlgMesg;
		dlgMesg.MyDoModal(L"로그인 레벨 제한! \n해당 파라미터를 수정할 수 없습니다.", 0x01);
		return;
	}

	CUniToChar		csCnv;
	if (ePHILHMI_GRD_COL_PARAMETER_UNIT == pItem->iColumn)
	{
		if (DEF_DATA_TYPE_LOAD == stParam.strDataType)
		{
			if (EN_RECIPE_JOB::GERBER_NAME == nParamID)
			{
				CString strPathName = SelectGerber();
				if (FALSE == strPathName.IsEmpty())
				{
					int nFindIndex = strPathName.ReverseFind('\\');
					CString strFileName = strPathName.Right(strPathName.GetLength() - nFindIndex - 1);
					strPathName = strPathName.Left(nFindIndex);
					// 파일 경로를 가져와 사용할 경우, Edit Control에 값 저장
					CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->SetValue(EN_RECIPE_TAB::JOB, EN_RECIPE_JOB::GERBER_PATH, strPathName);
					stParam.SetValue(strFileName);
				}
				else
				{
					return;
				}
			}
			if (EN_RECIPE_JOB::EXPO_RECIPE == nParamID)
			{
				// Load
				CStringArray	strArrRecipe;
				strArrRecipe.Add(_T("[CANCEL]"));
				int nCount = uvEng_ExpoRecipe_GetCount();
				for (int i = 0; i < nCount; i++)
				{
					LPG_REAF pstRecipe = uvEng_ExpoRecipe_GetRecipeIndex(i);

					strArrRecipe.Add((CString)pstRecipe->expo_name);
				}

				int nSelectRecipe = ShowMultiSelectMsg(EN_MSG_BOX_TYPE::eQUEST, _T("LOAD EXPO RECIPE"), strArrRecipe);

				if (0 == nSelectRecipe)
				{
					// cancel
					return;
				}
				else
				{
					CString strRecipe = strArrRecipe.GetAt(nSelectRecipe);
					stParam.SetValue(strRecipe);
				}
			}
			if (EN_RECIPE_JOB::ALIGN_RECIPE == nParamID)
			{
				CStringArray	strArrRecipe;
				strArrRecipe.Add(_T("[CANCEL]"));
				int nCount = uvEng_Mark_GetAlignRecipeCount();
				for (int i = 0; i < nCount; i++)
				{
					LPG_RAAF pstRecipe = uvEng_Mark_GetAlignRecipeIndex(i);

					strArrRecipe.Add((CString)pstRecipe->align_name);
				}

				int nSelectRecipe = ShowMultiSelectMsg(EN_MSG_BOX_TYPE::eQUEST, _T("LOAD ALIGN RECIPE"), strArrRecipe);

				if (0 == nSelectRecipe)
				{
					// cancel
					return;
				}
				else
				{
					CString strRecipe = strArrRecipe.GetAt(nSelectRecipe);
					stParam.SetValue(strRecipe);
				}
			}
		}
	}
	else
	{
		if (pItem->iColumn != ePHILHMI_GRD_COL_PARAMETER_VALUE)
		{
			return;
		}

		if (DEF_DATA_TYPE_STRING == stParam.strDataType)
		{
			TCHAR tzText[RECIPE_NAME_LENGTH] = { NULL };
			CDlgKBDT dlg;
			if (IDOK == dlg.MyDoModal(RECIPE_NAME_LENGTH))
			{
				dlg.GetText(tzText, RECIPE_NAME_LENGTH);
				stParam.SetValue(tzText);
				pGrid->SetItemText(pItem->iRow, pItem->iColumn, tzText);
			}
		}
		else
		{
			CString strOutput;
			CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);

			double	dMin = stParam.dMin;
			double	dMax = stParam.dMax;

			if (DEF_DATA_TYPE_DOUBLE == stParam.strDataType)
			{
				if (PopupKBDN(ENM_DITM::en_double, strInput, strOutput, dMin, dMax))
				{
					stParam.SetValue(strOutput);
					pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
				}
			}
			else if (DEF_DATA_TYPE_BOOL == stParam.strDataType)
			{
				return;
			}
			else if (DEF_DATA_TYPE_COMBO == stParam.strDataType)
			{
				return;
			}
			else if (DEF_DATA_TYPE_INT == stParam.strDataType)
			{
				if (PopupKBDN(ENM_DITM::en_int16, strInput, strOutput, dMin, dMax))
				{
					stParam.SetValue(strOutput);
					pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
				}
			}

		}
	}

	//SetEditing(TRUE);
	CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->SetParam(EN_RECIPE_TAB::JOB, stParam);
	UpdateGridParam(EN_RECIPE_TAB::JOB);
	pGrid->Refresh();
}

void CDlgPhilhmi::OnClickGridPhilParameter(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	CString strName;
	
	CGridCtrl* pGrid = &m_grd_ctl[ePHILHMI_GRD_PHIL_PARAMETER];

	CString strOutput;
	CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);
	CString strValue;
	CString strGlass, strProStep;

	int options = 0;
	//double	dMin = stParam.dMin;
	//double	dMax = stParam.dMax;
	double	dMin = 0;
	double	dMax = 999;
	TCHAR szTemp[512] = { NULL };

	if (pItem->iColumn == eGRD_COL_PHIL_TITLE + 1)
	{

		switch (pItem->iRow)
		{
		case eGRD_ROW_OUTPUT_NUM:
			if (PopupKBDN(ENM_DITM::en_double, strInput, strOutput, dMin, dMax))
			{
				//stParam.SetValue(strOutput);
				pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);

			}
			break;
		case eGRD_ROW_OUTPUT_ONOFF:
			break;
		case eGRD_ROW_PROCESS_STEP:
			break;
		case eGRD_ROW_PROCESS_COMP:
			break;
		case eGRD_ROW_SUB_PROCESS_COMP:
			break;
		case eGRD_ROW_EC_MODIFY:
			break;
		case eGRD_ROW_INITIAL_COMP:
			break;
		case eGRD_ROW_ALARM_OCCUR:
			break;
		case eGRD_ROW_EVENT_NOTIFY:
			break;
		case eGRD_ROW_INTERRUPT_STOP:
			break;
		}
	}

	unsigned short usInput;
	if (pItem->iColumn == eGRD_COL_PHIL_TITLE + 2)
	{
		switch (pItem->iRow)
		{
		case eGRD_ROW_OUTPUT_NUM:
		case eGRD_ROW_OUTPUT_ONOFF:
			usInput = pGrid->GetItemTextToInt(eGRD_ROW_OUTPUT_NUM, eGRD_COL_PHIL_TITLE + 1);
			strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn - 1);

			if (_T("On") == strInput)	options = 1;
			else if (_T("Off") == strInput)	options = 0;

			STG_PP_P2C_OUTPUT_ONOFF stSend1;
			STG_PP_P2C_OUTPUT_ONOFF_ACK stRecv1;
			stSend1.Reset();
			stRecv1.Reset();

			stSend1.usIndex = usInput;
			stSend1.usSignal = options;
			uvEng_Philhmi_Send_P2C_OUTPUT_ONOFF(stSend1, stRecv1);

			AddListBox(_T("P2C Output OnOff"));
			swprintf_s(szTemp, 512, L"I/O : %d - %s   [error:%d]", usInput, strInput.GetString(), stRecv1.ulUniqueID);

			AddListBox(szTemp);
			break;
		case eGRD_ROW_PROCESS_STEP:
			strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn - 1);

			STG_PP_P2C_PROCESS_STEP			stSend2;
			STG_PP_P2C_PROCESS_STEP_ACK		stRecv2;
			stSend2.Reset();
			stRecv2.Reset();

			sprintf_s(stSend2.szGlassName, DEF_MAX_GLASS_NAME_LENGTH, "Glass1000");
			sprintf_s(stSend2.szProcessStepName, DEF_MAX_SUBPROCESS_NAME_LENGTH, "%S", strInput.GetString());

			uvEng_Philhmi_Send_P2C_PROCESS_STEP(stSend2);

			AddListBox(_T("P2C Process Step"));
			swprintf_s(szTemp, 512, L"Process Step : %S - %S   [error:%d]", stSend2.szGlassName, stSend2.szProcessStepName, stRecv2.ulUniqueID);

			//strName.Format(_T("Glass1000"));
			//swprintf_s(szTemp, 512, L"Process Step : %s ", strName.GetString());

			AddListBox(szTemp);
			break;
		case eGRD_ROW_PROCESS_COMP:
			STG_PP_P2C_PROCESS_COMP stSend3;
			STG_PP_P2C_PROCESS_COMP_ACK stRecv3;
			stSend3.Reset();
			stRecv3.Reset();

			sprintf_s(stSend3.szRecipeName, DEF_MAX_RECIPE_NAME_LENGTH, "Job1");
			sprintf_s(stSend3.szGlassID, DEF_MAX_GLASS_NAME_LENGTH, "1000");
			stSend3.usCount = 1;
			sprintf_s(stSend3.stVar[0].szParameterType, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "STRING");
			sprintf_s(stSend3.stVar[0].szParameterName, DEF_MAX_RECIPE_PARAM_NAME_LENGTH, "Glass");
			sprintf_s(stSend3.stVar[0].szParameterValue, DEF_MAX_RECIPE_PARAM_VALUE_LENGTH, "1000");

			stSend3.usProgress = 1; // DONE

			uvEng_Philhmi_Send_P2C_PROCESS_COMP(stSend3, stRecv3);

			AddListBox(_T("P2C Process Complete"));
			swprintf_s(szTemp, 512, L"Process Step : %S - %S   [error:%d]", stRecv3.szRecipeName, stRecv3.szGlassID, stRecv3.ulUniqueID);
			AddListBox(szTemp);
			break;
		case eGRD_ROW_SUB_PROCESS_COMP:
			strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn - 1);

			STG_PP_P2C_SUBPROCESS_COMP		stSend4;
			STG_PP_P2C_SUBPROCESS_COMP_ACK	stRecv4;
			stSend4.Reset();
			stRecv4.Reset();

			sprintf_s(stSend4.szSubProcessName, DEF_MAX_SUBPROCESS_NAME_LENGTH, "%S", strInput.GetString());

			uvEng_Philhmi_Send_P2C_SUBPROCESS_COMP(stSend4, stRecv4);

			AddListBox(_T("P2C Sub Process Complete"));
			swprintf_s(szTemp, 512, L"Sub Process Complete : %S [error:%d]", stSend4.szSubProcessName, stRecv4.ulUniqueID);
			AddListBox(szTemp);
			break;
		case eGRD_ROW_EC_MODIFY:
			AddListBox(_T("P2C Ec Modify"));
			break;
		case eGRD_ROW_INITIAL_COMP:
			strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn - 1);
			if (_T("Comp") == strInput)			options = 0;
			else if (_T("Error") == strInput)	options = 1000;

			STG_PP_P2C_INITIAL_COMPLETE stSend6;
			STG_PP_P2C_INITIAL_COMPLETE_ACK stRecv6;
			stSend6.Reset();
			stRecv6.Reset();
			stSend6.usErrorCode = options;
			uvEng_Philhmi_Send_P2C_INITIAL_COMPLETE(stSend6, stRecv6);

			AddListBox(_T("P2C Initial Complete"));
			swprintf_s(szTemp, 512, _T("Initial Complete : [error:%d]"), stRecv6.ulUniqueID);
			AddListBox(szTemp);
			break;
		case eGRD_ROW_ALARM_OCCUR:
			strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn - 1);
			if (_T("Comp") == strInput)			options = 0;
			else if (_T("Error") == strInput)	options = 1000;

			STG_PP_P2C_ALARM_OCCUR		stSend7;
			STG_PP_P2C_ALARM_OCCUR_ACK	stRecv7;
			stSend7.Reset();
			stRecv7.Reset();
			stSend7.usErrorCode = options;
			uvEng_Philhmi_Send_P2C_ALARM_OCCUR(stSend7, stRecv7);
			
			AddListBox(_T("P2C Alarm Occur"));
			swprintf_s(szTemp, 512, _T("Alarm Occur : [error:%d]"), stRecv7.ulUniqueID);
			AddListBox(szTemp);
			break;
		case eGRD_ROW_EVENT_NOTIFY:
			strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn - 1);

			STG_PP_P2C_EVENT_NOTIFY stSend8;
			STG_PP_P2C_EVENT_NOTIFY_ACK stRecv8;
			stSend8.Reset();
			stRecv8.Reset();

			sprintf_s(stSend8.szEventName, DEF_MAX_EVENT_NAME_LENGTH, "%S", strInput.GetString());
			uvEng_Philhmi_Send_P2C_EVENT_NOTIFY(stSend8, stRecv8);


			AddListBox(_T("P2C Event Notify"));
			swprintf_s(szTemp, 512, L"Event Notify : %S [error:%d]", stSend8.szEventName, stRecv8.ulUniqueID);
			AddListBox(szTemp);

			break;

		case eGRD_ROW_INTERRUPT_STOP:
			strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn - 1);
			if (_T("Comp") == strInput)			options = 0;
			else if (_T("Error") == strInput)	options = 1000;

			STG_PP_P2C_INTERRUPT_STOP stSend9;
			STG_PP_P2C_INTERRUPT_STOP_ACK stRecv9;
			stSend9.Reset();
			stRecv9.Reset();
			stSend9.usErrorCode = options;
			uvEng_Philhmi_Send_P2C_INTERRUPT_STOP(stSend9, stRecv9);

			AddListBox(_T("P2C Interrupt Stop"));
			swprintf_s(szTemp, 512, _T("Interrupt Stop : [error:%d]"), stRecv9.ulUniqueID);
			AddListBox(szTemp);
			break;

		case eGRD_ROW_ABS_MOVE:
			strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn - 1);

			STG_PP_P2C_ABS_MOVE stSend10;
			STG_PP_P2C_ABS_MOVE_ACK stRecv10;
			stSend10.Reset();
			stRecv10.Reset();

			stSend10.usCount = 1;
			sprintf_s(stSend10.stMove[0].szAxisName, DEF_MAX_RECIPE_NAME_LENGTH, "%S", strInput.GetString());
			stSend10.stMove[0].dPosition = 100;
			stSend10.stMove[0].dSpeed = 10;
			
			uvEng_Philhmi_Send_P2C_ABS_MOVE(stSend10, stRecv10);


			AddListBox(_T("P2C Abs Move"));
			swprintf_s(szTemp, 512, L"%S Axis Position:%f Spee:%f ", stSend10.stMove[0].szAxisName,
				stSend10.stMove[0].dPosition, stSend10.stMove[0].dSpeed);
			AddListBox(szTemp);

			break;
		
		case eGRD_ROW_ABS_MOVE_COMP:
			strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn - 1);
			if (_T("Comp") == strInput)			options = 0;
			else if (_T("Error") == strInput)	options = 1000;

			STG_PP_P2C_ABS_MOVE_COMP stSend11;
			STG_PP_P2C_ABS_MOVE_COMP_ACK stRecv11;
			stSend11.Reset();
			stRecv11.Reset();
			stSend11.usErrorCode = options;

			uvEng_Philhmi_Send_P2C_ABS_MOVE_COMP(stSend11, stRecv11);

			AddListBox(_T("P2C Abs Move Comp"));
			swprintf_s(szTemp, 512, _T("Abs Move Comp : [error:%d]"), stRecv11.ulUniqueID);
			AddListBox(szTemp);

			break;
		}
	}
	pGrid->Refresh();
}

/*
 desc : IO 상태 상태 요청
 parm : None
 retn : None
*/
VOID CDlgPhilhmi::IOStatus()
{
	TCHAR szTemp[512] = { NULL };
	swprintf_s(szTemp, 512, L"IO Status");
	AddListBox(szTemp);

	//int a = 11;
	//swprintf_s(szTemp, 512, _T("IO Status  %d"),a);
	//AddListBox(szTemp); 
	//AddListBox(_T("IO Status"));
	/*트리거 세팅및 초기화 완료 후에 Cam mode 변경*/
	//uvEng_Camera_ResetGrabbedImage();
	uvEng_Camera_ResetGrabbedImage();
	uvEng_Camera_SetCamMode(ENG_VCCM::en_grab_mode);
}
/*
 desc : 장비 중단 요청
 parm : None
 retn : None
*/
VOID CDlgPhilhmi::InterruptStop()
{
	TCHAR szTemp[512] = { NULL };
	swprintf_s(szTemp, 512, L"Interrupt Stop");
	AddListBox(szTemp);

	uvEng_Mvenc_ResetTrigPosAll();
}
/*
 desc : Philhmi 상태 요청
 parm : None
 retn : None
*/
VOID CDlgPhilhmi::SendStatus()
{
	TCHAR szTemp[512] = { NULL };
	//swprintf_s(szTemp, 512, L"Send Status");
	//AddListBox(szTemp);

	UINT8 u8Count = 0x05;
	//PUINT32 p32Trig1 = {100, 200, 300};
	//PUINT32 p32Trig2 = { 100, 200, 300 };

	UINT32 u32Trig[MAX_ALIGN_CAMERA][MAX_SCAN_MARK_COUNT] = { NULL };

	//u32Trig[0][0] = u32Trig[1][0] = { 500000 };
	//u32Trig[0][1] = u32Trig[1][1] = { 1000000 };
	//u32Trig[0][2] = u32Trig[1][2] = { 2000000 };
	//u32Trig[0][3] = u32Trig[1][3] = { 3000000 };
	//u32Trig[0][4] = u32Trig[1][4] = { 4000000 };
	//u32Trig[0][5] = u32Trig[1][5] = { 5000000 };


	//if (uvEng_Mvenc_ReqWriteAreaTrigPos(TRUE /* 정방향 : 전진 */,
	//	0, u8Count, u32Trig[0],
	//	0, u8Count, u32Trig[1],
	//	ENG_TEED::en_positive, TRUE))
	//{
	//	swprintf_s(szTemp, 512, L"Set Trigger Position");
	//	AddListBox(szTemp);
	//}

	if (uvEng_MC2_SendDevLuriaMode(ENG_MMDI::en_stage_y, ENG_MTAE::en_area))
	{
		swprintf_s(szTemp, 512, L"Set Luria Mode");
		AddListBox(szTemp);
	}
}
/*
 desc : List Box 초기화
 parm : None
 retn : None
*/
VOID CDlgPhilhmi::ClearListBox()
{
	m_box_ctl[ePHILHMI_BOX_PHIL_LOG].ResetContent();
}
/*
설명 : List Box에 문자열 등록
변수 : mesg	- [in]  등록하려는 문자열이 저장된 버퍼 포인터
반환 : None
*/
VOID CDlgPhilhmi::AddListBox(TCHAR* mesg)
{
	PTCHAR	ptmsg = NULL;
	ptmsg = new TCHAR[512];
	ASSERT(ptmsg);
	wmemset(ptmsg, 0x00, 512);
	swprintf_s(ptmsg, 512, L"%s", mesg);

	m_box_ctl[ePHILHMI_BOX_PHIL_LOG].AddString(ptmsg);

	delete ptmsg;
}




/*
 desc : GridControl 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgPhilhmi::InitGrid()
{
	InitGridParam(EN_RECIPE_TAB::JOB);
	//InitGridParam(EN_RECIPE_TAB::EXPOSE);
	InitGridMeasureManualSet();
	InitGridRecipeList(FALSE);

	return TRUE;
}

void CDlgPhilhmi::DeleteGridParam(int nRecipeTab)
{
	CArrGrdParam* pArrGrdParam = NULL;
	ST_GRD_PARAM* pStGrdPram = NULL;

	for (int nCntPage = 0; nCntPage < m_arrGrdPage[nRecipeTab].GetCount(); nCntPage++)
	{
		pArrGrdParam = m_arrGrdPage[nRecipeTab].GetAt(nCntPage);
		for (int nCntParam = 0; nCntParam < pArrGrdParam->GetCount(); nCntParam++)
		{
			pStGrdPram = pArrGrdParam->GetAt(nCntParam);
			delete pStGrdPram;
		}

		pArrGrdParam->RemoveAll();
		delete pArrGrdParam;
	}

	m_arrGrdPage[nRecipeTab].RemoveAll();

	m_grd_ctl[ePHILHMI_GRD_JOB_PARAMETER + nRecipeTab].ReleaseMergeCells();
	m_grd_ctl[ePHILHMI_GRD_JOB_PARAMETER + nRecipeTab].DeleteAllItems();
}

BOOL CDlgPhilhmi::InitGridRecipeList(BOOL bUpdate)
{
	CResizeUI					clsResizeUI;
	CRect						rGrid;

	CStringArray				strArrRecipeList;
	CRecipeManager::GetInstance()->GetRecipeList(strArrRecipeList);

	CGridCtrl* pGrid = &m_grd_ctl[ePHILHMI_GRD_RECIPE_LIST];
	pGrid->DeleteAllItems();

	if (FALSE == bUpdate)
	{
		clsResizeUI.ResizeControl(this, pGrid);
	}

	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	if (0 == strArrRecipeList.GetCount())
	{
		return FALSE;
	}

	std::vector <int>			vRowSize(strArrRecipeList.GetCount());
	std::vector <int>			vColSize(ePHILHMI_GRD_COL_RECIPE_LIST_MAX);

	vColSize[ePHILHMI_GRD_COL_RECIPE_LIST_NO] = (int)((double)rGrid.Width() * 0.2);
	vColSize[ePHILHMI_GRD_COL_RECIPE_LIST_NAME] = (int)((double)rGrid.Width() * 0.8);

	int nHeight = (int)(DEF_DEFAULT_GRID_ROW_SIZE * clsResizeUI.GetRateY());
	int nTotalHeight = 0;
	int nTotalWidth = 0;

	for (auto& height : vRowSize)
	{
		height = nHeight;
	}

	// 	if (rGrid.Height() < nTotalHeight)
	// 	{
	// 		nTotalHeight = 0;
	// 
	// 		for (auto& height : vRowSize)
	// 		{
	// 			height = (rGrid.Height()) / (int)vRowSize.size();
	// 			nTotalHeight += height;
	// 		}
	// 	}

	for (auto& width : vColSize)
	{
		width = width;
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->SetRowCount((int)vRowSize.size());
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(WHITE_);
	pGrid->SetFixedColumnSelection(0);

	/* 타이틀 (첫 번째 행) */
	for (int i = 0; i < (int)vColSize.size(); i++)
	{
		pGrid->SetItemFormat(0, i, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pGrid->SetColumnWidth(i, vColSize[i]);
	}

	for (int nRow = 0; nRow < (int)vRowSize.size(); nRow++)
	{
		pGrid->SetRowHeight(nRow, vRowSize[nRow]);

		for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
		{
			pGrid->SetColumnWidth(nCol, vColSize[nCol]);
			if (0 == nCol)
			{
				pGrid->SetItemBkColour(nRow, nCol, ALICE_BLUE);
			}
			else
			{
				pGrid->SetItemBkColour(nRow, nCol, WHITE_);
			}

			pGrid->SetItemFgColour(nRow, nCol, BLACK_);

			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}

		pGrid->SetItemTextFmt(nRow, 0, _T("%d"), nRow + 1);
		pGrid->SetItemText(nRow, 1, strArrRecipeList.GetAt(nRow));
	}


	int nSelRecipe = CRecipeManager::GetInstance()->GetSelectRecipeIndex();
	if (0 <= nSelRecipe)
	{
		SelectRecipe(nSelRecipe, eRECIPE_MODE_SEL);
	}

	/* 기본 속성 및 갱신 */
	pGrid->SetColumnResize(FALSE);
	pGrid->SetRowResize(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->SetEditable(FALSE);
	pGrid->Refresh();

	return TRUE;
}

void CDlgPhilhmi::InitGridParam(int nRecipeTab)
{
	DeleteGridParam(nRecipeTab);

	BOOL bPageSingle = TRUE;

	CGridCtrl* pGrid = &m_grd_ctl[ePHILHMI_GRD_JOB_PARAMETER + nRecipeTab];
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int	nHeight = (int)((double)DEF_DEFAULT_GRID_ROW_SIZE * clsResizeUI.GetRateY());
	int	nMaxRowDisplay = rGrid.Height() / nHeight;

	int nMaxParam = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->GetParamCount(nRecipeTab);
	CString strGroupOld = _T("");
	//파라미터 배열
	CArrGrdParam* pArrGrdParamTemp = NULL;
	//////////////////////////////////////////////////////////////////////////
	//그룹으로 정리 : 그룹- 파람배열로 정리한 단계
	//임시로 그룹별로 정리
	CArrGrdPage arrGrdGroup;
	for (int nCntParam = 0; nCntParam < nMaxParam; nCntParam++)
	{
		ST_RECIPE_PARAM stParam = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->GetParam(nRecipeTab, nCntParam);
		if (FALSE == stParam.bShow)
			continue;

		//사용하지않는 파라미터는 패스
		if (FALSE == stParam.bUse)
			continue;

		if (strGroupOld != stParam.strGroup)
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

			strGroupOld = stParam.strGroup;
		}

		//실제 파라미터 매칭
		ST_GRD_PARAM* pStGrdParam = new ST_GRD_PARAM();
		pStGrdParam->bIsGroup = FALSE;
		pStGrdParam->nIndexParam = nCntParam;
		//실제 파라미터 넣기
		pArrGrdParamTemp->Add(pStGrdParam);
	}
	//////////////////////////////////////////////////////////////////////////
	//페이지로 정리 : 페이지 - 파람배열로 정리 완료
	//Param Page는 2칸 View는 1칸 
	if (FALSE == bPageSingle)
	{
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
				m_arrGrdPage[nRecipeTab].Add(pArrGrdParam);
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
	}
	else //Param Page는 1칸 View는 2칸 
	{
		CArrGrdParam* pArrGrdParam = NULL;
		int nCntParamInPage = 0;
		pArrGrdParam = new CArrGrdParam();
		m_arrGrdPage[nRecipeTab].Add(pArrGrdParam);

		for (int nCntGroup = 0; nCntGroup < arrGrdGroup.GetCount(); nCntGroup++)
		{
			pArrGrdParamTemp = arrGrdGroup.GetAt(nCntGroup);
			int nCntCurGroup = (int)pArrGrdParamTemp->GetCount();

			for (int nCntParam = 0; nCntParam < nCntCurGroup; nCntParam++)
			{
				pArrGrdParam->Add(pArrGrdParamTemp->GetAt(nCntParam));
			}
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
	UpdateGridParam(nRecipeTab);
}

void CDlgPhilhmi::UpdateGridParam(int nRecipeTab)
{
	//////////////////////////////////////////////////////////////////////////
	//UI 	
	CGridCtrl* pGrid = &m_grd_ctl[ePHILHMI_GRD_JOB_PARAMETER + nRecipeTab];
	CResizeUI	clsResizeUI;
	CRect		rGrid;

	//Col 사이즈 및 윈도우 렉트 변경
	pGrid->GetWindowRect(&rGrid);
	ScreenToClient(rGrid);

	int nMaxCol = ePHILHMI_GRD_COL_PARAMETER_MAX;
	int nMaxRow = 0;
	int nHeightRow = (int)((double)DEF_DEFAULT_GRID_ROW_SIZE * clsResizeUI.GetRateY());
	int nMaxRowDisplay = rGrid.Height() / nHeightRow;

	int nWidthID = 0;
	int nWidthParameter = (int)((double)rGrid.Width() * 0.5);
	int nWidthValue = (int)((double)rGrid.Width() * 0.3);
	int nWidthUnit = (int)((double)rGrid.Width() * 0.2);

	nWidthValue += rGrid.Width() - (nWidthParameter + nWidthValue + nWidthUnit + 1);

	int nWidth[ePHILHMI_GRD_COL_PARAMETER_MAX] = { nWidthID, nWidthParameter, nWidthValue, nWidthUnit };

	UINT nForamt = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;
	//////////////////////////////////////////////////////////////////////////

	CArrGrdParam* pArrGrdParam = NULL;
	ST_GRD_PARAM* pStGrdPram = NULL;

	//초기화 필요
	nWidth[ePHILHMI_GRD_COL_PARAMETER_UNIT] = nWidthUnit;

	//MaxRow 판단		
	pArrGrdParam = m_arrGrdPage[nRecipeTab].GetAt(0);
	nMaxRow = (int)pArrGrdParam->GetCount();

	//UI초기화
	pGrid->ReleaseMergeCells();
	pGrid->DeleteAllItems();
	pGrid->SetRowCount(nMaxRow);
	pGrid->SetColumnCount(nMaxCol);
	pGrid->SetFixedRowCount(1);


	if (nMaxRow > nMaxRowDisplay)
	{
		nWidth[ePHILHMI_GRD_COL_PARAMETER_UNIT] = nWidthUnit - ::GetSystemMetrics(SM_CXVSCROLL);
	}
	else
	{
		rGrid.bottom = rGrid.top + nHeightRow * nMaxRow + 1;
		pGrid->MoveWindow(rGrid);
	}

	for (int nCol = 0; nCol < nMaxCol; nCol++)
	{
		pGrid->SetColumnWidth(nCol, nWidth[nCol]);
	}

	for (int nRow = 0; nRow < pArrGrdParam->GetCount(); nRow++)
	{
		pStGrdPram = pArrGrdParam->GetAt(nRow);
		ST_RECIPE_PARAM stParam = CRecipeManager::GetInstance()->GetRecipe(eRECIPE_MODE_VIEW)->GetParam(nRecipeTab, pStGrdPram->nIndexParam);

		for (int nCol = 0; nCol < nMaxCol; nCol++)
		{
			pGrid->SetItemState(nRow, nCol, GVIS_READONLY);
		}

		if (TRUE == pStGrdPram->bIsGroup)
		{
			pGrid->MergeCells(CCellRange(nRow, ePHILHMI_GRD_COL_PARAMETER_ID, nRow, ePHILHMI_GRD_COL_PARAMETER_UNIT));
			pGrid->SetItemText(nRow, 0, stParam.strGroup);
			pGrid->SetItemFormat(nRow, 0, nForamt);
			pGrid->SetItemBkColour(nRow, 0, DEF_COLOR_BTN_MENU_NORMAL);
			pGrid->SetItemFgColour(nRow, 0, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		}
		else
		{
			//폼을 위한 for 문
			for (int nCol = 0; nCol < nMaxCol; nCol++)
			{
				pGrid->SetItemFormat(nRow, nCol, nForamt);
				if (ePHILHMI_GRD_COL_PARAMETER_VALUE != nCol) //파라미터 입력란만 흰색으로 사용
				{
					pGrid->SetItemBkColour(nRow, nCol, ALICE_BLUE);
				}
			}
			//디스플레이 네임
			pGrid->SetItemTextFmt(nRow, ePHILHMI_GRD_COL_PARAMETER_ID, _T("%d"), stParam.nIndexParam);
			pGrid->SetItemText(nRow, ePHILHMI_GRD_COL_PARAMETER_NAME, stParam.strDisplayName);
			pGrid->SetItemTextDes(nRow, ePHILHMI_GRD_COL_PARAMETER_NAME, stParam.strDescript);
			pGrid->SetItemText(nRow, ePHILHMI_GRD_COL_PARAMETER_UNIT, stParam.strUnit);

			if (DEF_DATA_TYPE_BOOL == stParam.strDataType)
			{
				pGrid->SetCellType(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE, RUNTIME_CLASS(CGridCellCombo));

				if (stParam.strName == _T("SERIAL_FLIP_HORZ") ||
					stParam.strName == _T("SCALE_FLIP_HORZ") ||
					stParam.strName == _T("TEXT_FLIP_HORZ"))
				{
					CStringArray options;
					options.Add(_T("Up"));
					options.Add(_T("Down"));

					CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE);

					pGrid->SetListMode(FALSE);
					pGrid->EnableDragAndDrop(TRUE);
					pGrid->SetItemState(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE, 0x00);

					pComboCell->SetOptions(options);
					pComboCell->SetStyle(CBS_DROPDOWN);
					if (0 == stParam.GetInt())
					{
						pComboCell->SetText(_T("Up"));
					}
					else
					{
						pComboCell->SetText(_T("Down"));
					}
				}
				else if (stParam.strName == _T("SERIAL_FLIP_VERT") ||
					stParam.strName == _T("SCALE_FLIP_VERT") ||
					stParam.strName == _T("TEXT_FLIP_VERT"))
				{
					CStringArray options;
					options.Add(_T("Left"));
					options.Add(_T("Right"));

					CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE);

					pGrid->SetListMode(FALSE);
					pGrid->EnableDragAndDrop(TRUE);
					pGrid->SetItemState(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE, 0x00);

					pComboCell->SetOptions(options);
					pComboCell->SetStyle(CBS_DROPDOWN);
					if (0 == stParam.GetInt())
					{
						pComboCell->SetText(_T("Left"));
					}
					else
					{
						pComboCell->SetText(_T("Right"));
					}
				}
				else if (stParam.strName == _T("MARK_TYPE"))
				{
					CStringArray options;
					options.Add(_T("Geomatrix"));
					options.Add(_T("Pattern Image"));

					CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE);

					pGrid->SetListMode(FALSE);
					pGrid->EnableDragAndDrop(TRUE);
					pGrid->SetItemState(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE, 0x00);

					pComboCell->SetOptions(options);
					pComboCell->SetStyle(CBS_DROPDOWN);
					if (0 == stParam.GetInt())
					{
						pComboCell->SetText(_T("Geomatrix"));
					}
					else
					{
						pComboCell->SetText(_T("Pattern Image"));
					}
				}
				else if (stParam.strName == _T("LAMP_TYPE"))
				{
					CStringArray options;
					options.Add(_T("Ring"));
					options.Add(_T("Coaxial"));

					CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE);

					pGrid->SetListMode(FALSE);
					pGrid->EnableDragAndDrop(TRUE);
					pGrid->SetItemState(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE, 0x00);

					pComboCell->SetOptions(options);
					pComboCell->SetStyle(CBS_DROPDOWN);
					if (0 == stParam.GetInt())
					{
						pComboCell->SetText(_T("Ring"));
					}
					else
					{
						pComboCell->SetText(_T("Coaxial"));
					}
				}
				else
				{
					CStringArray options;
					options.Add(_T("NO"));
					options.Add(_T("YES"));

					CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE);

					pGrid->SetListMode(FALSE);
					pGrid->EnableDragAndDrop(TRUE);
					pGrid->SetItemState(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE, 0x00);

					pComboCell->SetOptions(options);
					pComboCell->SetStyle(CBS_DROPDOWN);
					if (0 == stParam.GetInt())
					{
						pComboCell->SetText(_T("NO"));
					}
					else
					{
						pComboCell->SetText(_T("YES"));
					}
				}
			}
			else if (DEF_DATA_TYPE_INT == stParam.strDataType)
			{

				if (stParam.strName == _T("SEARCH_TYPE"))
				{
					pGrid->SetCellType(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE, RUNTIME_CLASS(CGridCellCombo));

					CStringArray options;
					options.Add(_T("Single"));
					options.Add(_T("Cent_side"));
					options.Add(_T("Multi_only"));
					options.Add(_T("Ring_cirecle"));
					options.Add(_T("Ph_step"));

					CGridCellCombo* pComboCell1 = (CGridCellCombo*)pGrid->GetCell(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE);

					pGrid->SetListMode(FALSE);
					pGrid->EnableDragAndDrop(TRUE);
					pGrid->SetItemState(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE, 0x00);

					pComboCell1->SetOptions(options);
					pComboCell1->SetStyle(CBS_DROPDOWN);
					if (0 == stParam.GetInt() || 1 == stParam.GetInt())
					{
						pComboCell1->SetText(_T("Single"));
					}
					else if (2 == stParam.GetInt())
					{
						pComboCell1->SetText(_T("Cent_side"));
					}
					else if (3 == stParam.GetInt())
					{
						pComboCell1->SetText(_T("Multi_only"));
					}
					else if (4 == stParam.GetInt())
					{
						pComboCell1->SetText(_T("Ring_cirecle"));
					}
					else if (5 == stParam.GetInt())
					{
						pComboCell1->SetText(_T("Ph_step"));
					}
				}
				else
				{
					CString strTemp = stParam.GetScaledValue();
					pGrid->SetItemText(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE, strTemp);
				}
			}
			else if (DEF_DATA_TYPE_LOAD == stParam.strDataType)
			{
				CString strTemp = stParam.GetScaledValue();
				pGrid->SetItemText(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE, strTemp);
				pGrid->SetItemText(nRow, ePHILHMI_GRD_COL_PARAMETER_UNIT, stParam.strDataType);
				pGrid->SetItemBkColour(nRow, ePHILHMI_GRD_COL_PARAMETER_UNIT, DEF_COLOR_BTN_PAGE_NORMAL);
				pGrid->SetItemFgColour(nRow, ePHILHMI_GRD_COL_PARAMETER_UNIT, DEF_COLOR_BTN_PAGE_NORMAL_TEXT);
			}
			else if (DEF_DATA_TYPE_COMBO == stParam.strDataType)
			{
				CString strTemp = stParam.GetDefaultValue();
				CStringArray strArrData;
				CString strData;
				while (AfxExtractSubString(strData, strTemp, (int)strArrData.GetCount(), ';'))
				{
					strArrData.Add(strData);
				}
				pGrid->SetCellType(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE, RUNTIME_CLASS(CGridCellCombo));
				CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE);

				pGrid->SetListMode(FALSE);
				pGrid->EnableDragAndDrop(TRUE);
				pGrid->SetItemState(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE, 0x00);

				pComboCell->SetOptions(strArrData);
				pComboCell->SetStyle(CBS_DROPDOWN);

				BOOL bIsExist = FALSE;
				for (int i = 0; i < strArrData.GetCount(); i++)
				{
					if (0 == strArrData.GetAt(i).Compare(stParam.GetValue()))
					{
						bIsExist = TRUE;
						break;
					}
				}

				if (bIsExist)
				{
					pComboCell->SetText(stParam.GetValue());
				}
				else
				{
					pComboCell->SetText(strArrData.GetAt(0));
				}
			}
			else if (DEF_DATA_TYPE_STRING == stParam.strDataType)
			{
				pGrid->MergeCells(CCellRange(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE, nRow, ePHILHMI_GRD_COL_PARAMETER_UNIT));
				CString strTemp = stParam.GetScaledValue();
				pGrid->SetItemText(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE, strTemp);
			}
			else
			{
				CString strTemp = stParam.GetScaledValue();
				pGrid->SetItemText(nRow, ePHILHMI_GRD_COL_PARAMETER_VALUE, strTemp);
			}
		}
		pGrid->SetRowHeight(nRow, nHeightRow);
	}
	pGrid->SetColumnResize(FALSE);
	pGrid->SetRowResize(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->SetEditable(m_bEnableEdit);
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->Refresh();
}

void CDlgPhilhmi::SelectRecipe(int nSelect, EN_RECIPE_MODE eRecipeMode)
{
	if (eRECIPE_MODE_SEL == eRecipeMode)
	{
		m_nSelectRecipeOld[eRECIPE_MODE_SEL] = m_nSelectRecipe[eRECIPE_MODE_SEL];
		m_nSelectRecipe[eRECIPE_MODE_SEL] = nSelect;
	}
	else if (eRECIPE_MODE_VIEW == eRecipeMode)
	{
		m_nSelectRecipeOld[eRECIPE_MODE_VIEW] = m_nSelectRecipe[eRECIPE_MODE_VIEW];
		m_nSelectRecipe[eRECIPE_MODE_VIEW] = nSelect;
	}

	//Old부터 처리
	for (int nCnt = 0; nCnt < eRECIPE_MODE_MAX; nCnt++)
	{
		if (-1 != m_nSelectRecipeOld[nCnt] && m_grd_ctl[ePHILHMI_GRD_RECIPE_LIST].GetRowCount() > m_nSelectRecipeOld[nCnt])
		{
			m_grd_ctl[ePHILHMI_GRD_RECIPE_LIST].SetItemBkColour(m_nSelectRecipeOld[nCnt], 0, ALICE_BLUE);
			m_grd_ctl[ePHILHMI_GRD_RECIPE_LIST].SetItemBkColour(m_nSelectRecipeOld[nCnt], 1, WHITE_);
		}
	}

	if (-1 != m_nSelectRecipe[eRECIPE_MODE_VIEW])
	{
		m_grd_ctl[ePHILHMI_GRD_RECIPE_LIST].SetItemBkColour(m_nSelectRecipe[eRECIPE_MODE_VIEW], 0, PALE_GREEN);
		m_grd_ctl[ePHILHMI_GRD_RECIPE_LIST].SetItemBkColour(m_nSelectRecipe[eRECIPE_MODE_VIEW], 1, PALE_GREEN);

		UpdateRecipe(m_nSelectRecipe[eRECIPE_MODE_VIEW]);
	}

	if (-1 != m_nSelectRecipe[eRECIPE_MODE_SEL])
	{
		m_grd_ctl[ePHILHMI_GRD_RECIPE_LIST].SetItemBkColour(m_nSelectRecipe[eRECIPE_MODE_SEL], 0, LIGHT_CORAL);
		m_grd_ctl[ePHILHMI_GRD_RECIPE_LIST].SetItemBkColour(m_nSelectRecipe[eRECIPE_MODE_SEL], 1, LIGHT_CORAL);
	}

	if (m_nSelectRecipe[eRECIPE_MODE_SEL] == m_nSelectRecipe[eRECIPE_MODE_VIEW] &&
		m_nSelectRecipe[eRECIPE_MODE_SEL] != -1)
	{

	}

	m_grd_ctl[ePHILHMI_GRD_RECIPE_LIST].Refresh();
}


/*
 desc : Gerber Recipe 정보를 화면에 갱신
 parm : recipe	- [in]  Gerber Recipe 정보가 저장된 구조체 포인터
 retn : None
*/
VOID CDlgPhilhmi::UpdateRecipe(int nSelectRecipe)
{
	CUniToChar csCnv;

	/* 화면 갱신 취소 */
	SetRedraw(FALSE);

	LPG_RJAF recipe = NULL;
	CString strSelectRecipe = m_grd_ctl[ePHILHMI_GRD_RECIPE_LIST].GetItemText(nSelectRecipe, ePHILHMI_GRD_COL_RECIPE_LIST_NAME);
	/* 기존 선택된 레시피가 있다면, 관련 정보 출력 */
	if ((recipe = uvEng_JobRecipe_GetRecipeOnlyName(strSelectRecipe.GetBuffer())))
	{
		strSelectRecipe.ReleaseBuffer();

		CRecipeManager::GetInstance()->LoadRecipe(strSelectRecipe, eRECIPE_MODE_VIEW);

		UpdateGridParam(EN_RECIPE_TAB::JOB);
		//UpdateGridParam(EN_RECIPE_TAB::EXPOSE);
		//UpdateGridParam(EN_RECIPE_TAB::ALIGN);

		/* 화면 갱신 활성화 */
		SetRedraw(TRUE);
		Invalidate(FALSE);
	}
}

VOID CDlgPhilhmi::InitGridMeasureManualSet()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;

	std::vector <int>			vRowSize(16);
	std::vector <int>			vColSize(3);
	std::vector <std::wstring>	vTitle[16];

	vTitle[0] = { _T("Output Num"),			_T("1000") ,		_T("Set") };
	vTitle[1] = { _T("Output OnOff"),		_T("ON") ,		_T("Set") };
	vTitle[2] = { _T("Process Step"),		_T("Expo") ,	_T("Set") };
	vTitle[3] = { _T("Process Complete"),	_T("	") ,	_T("Set") };
	vTitle[4] = { _T("Sub Process Complete"),_T("Only Expose") ,	_T("Set") };
	vTitle[5] = { _T("EC Modify"),			_T("Foucs") ,	_T("Set") };
	vTitle[6] = { _T("Initial Complete"),	_T("Comp") ,	_T("Set") };
	vTitle[7] = { _T("Alarm Occur"),		_T("	") ,	_T("Set") };
	vTitle[8] = { _T("Event Notify"),		_T("Event1") ,	_T("Set") };
	vTitle[9] = { _T("Interupt Stop"),		_T("Comp") ,	_T("Set") };
	vTitle[10] = { _T("Abs Move"),			_T("ALIGN CAMERA Z 1") ,	_T("Set") };
	vTitle[11] = { _T("Abs Move Complete"),	_T("Comp") ,	_T("Set") };
	vTitle[12] = { _T("Rel Move"),			_T("ALIGN CAMERA Z 1") ,	_T("Set") };
	vTitle[13] = { _T("Rel Move Complete"),	_T("Comp") ,	_T("Set") };
	vTitle[14] = { _T("Char Move"),			_T("ALIGN CAMERA Z 1") ,	_T("Set") };
	vTitle[15] = { _T("Char Move Complete"),	_T("Comp") ,	_T("Set") };


	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_, ALICE_BLUE };

	CGridCtrl* pGrid = &m_grd_ctl[ePHILHMI_GRD_PHIL_PARAMETER];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int nHeight = (int)(DEF_DEFAULT_GRID_ROW_SIZE * clsResizeUI.GetRateY());
	int nTotalHeight = 0;
	for (auto& height : vRowSize)
	{
		height = nHeight;
		nTotalHeight += height;
	}

	if (rGrid.Height() < nTotalHeight)
	{
		nTotalHeight = 0;

		for (auto& height : vRowSize)
		{
			height = (rGrid.Height()) / (int)vRowSize.size();
			nTotalHeight += height;
		}
	}

	int nTotalWidth = 0;
	for (auto& width : vColSize)
	{
		width = rGrid.Width() / (int)vColSize.size();
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - 2;

	/* 객체 기본 설정 */
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->SetRowCount((int)vRowSize.size());
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(RGB(255, 255, 255));
	pGrid->SetFixedColumnSelection(0);

	/* 타이틀 (첫 번째 행) */
	for (int nRow = 0; nRow < (int)vRowSize.size(); nRow++)
	{
		pGrid->SetRowHeight(nRow, vRowSize[nRow]);
		for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
		{
			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pGrid->SetColumnWidth(nCol, vColSize[nCol]);
			pGrid->SetItemText(nRow, nCol, vTitle[nRow][nCol].c_str());
			pGrid->SetItemBkColour(nRow, nCol, vColColor[nCol]);
			
			if (nRow == eGRD_ROW_OUTPUT_NUM && nCol == eGRD_COL_PHIL_TITLE + 1)
			{

			}
			else if (nRow == eGRD_ROW_OUTPUT_ONOFF && nCol == eGRD_COL_PHIL_TITLE + 1)
			{
				pGrid->SetCellType(nRow, nCol, RUNTIME_CLASS(CGridCellCombo));
				CStringArray options;
				options.Add(_T("On"));
				options.Add(_T("Off"));
				CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, nCol);
				pComboCell->SetOptions(options);
				pComboCell->SetStyle(CBS_DROPDOWN);
			}
			else if (nRow == eGRD_ROW_PROCESS_STEP && nCol == eGRD_COL_PHIL_TITLE + 1)
			{
				pGrid->SetCellType(nRow, nCol, RUNTIME_CLASS(CGridCellCombo));
				CStringArray options;
				options.Add(_T("Expo"));
				options.Add(_T("Select"));
				CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, nCol);
				pComboCell->SetOptions(options);
				pComboCell->SetStyle(CBS_DROPDOWN);
			}
			else if (nRow == eGRD_ROW_PROCESS_COMP && nCol == eGRD_COL_PHIL_TITLE + 1)
			{
				pGrid->SetCellType(nRow, nCol, RUNTIME_CLASS(CGridCellCombo));
				CStringArray options;
				//options.Add(_T("Comp"));
				//options.Add(_T("Error")); 
				CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, nCol);
				pComboCell->SetOptions(options);
				pComboCell->SetStyle(CBS_DROPDOWN);
			}
			else if (nRow == eGRD_ROW_SUB_PROCESS_COMP && nCol == eGRD_COL_PHIL_TITLE + 1)
			{
				pGrid->SetCellType(nRow, nCol, RUNTIME_CLASS(CGridCellCombo));
				CStringArray options;
				options.Add(_T("Only Expose"));
				options.Add(_T("FEM"));
				CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, nCol);
				pComboCell->SetOptions(options);
				pComboCell->SetStyle(CBS_DROPDOWN);
			}
			else if (nRow == eGRD_ROW_EC_MODIFY && nCol == eGRD_COL_PHIL_TITLE + 1)
			{
				pGrid->SetCellType(nRow, nCol, RUNTIME_CLASS(CGridCellCombo));
				CStringArray options;
				options.Add(_T("Foucs"));
				options.Add(_T("Speed"));
				CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, nCol);
				pComboCell->SetOptions(options);
				pComboCell->SetStyle(CBS_DROPDOWN);
			}
			else if (nRow == eGRD_ROW_INITIAL_COMP && nCol == eGRD_COL_PHIL_TITLE + 1)
			{
				pGrid->SetCellType(nRow, nCol, RUNTIME_CLASS(CGridCellCombo));
				CStringArray options;
				options.Add(_T("Comp"));
				options.Add(_T("Error"));
				CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, nCol);
				pComboCell->SetOptions(options);
				pComboCell->SetStyle(CBS_DROPDOWN);
			}
			else if (nRow == eGRD_ROW_ALARM_OCCUR && nCol == eGRD_COL_PHIL_TITLE + 1)
			{
				pGrid->SetCellType(nRow, nCol, RUNTIME_CLASS(CGridCellCombo));
				CStringArray options;
				options.Add(_T("Comp"));
				options.Add(_T("Error"));
				CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, nCol);
				pComboCell->SetOptions(options);
				pComboCell->SetStyle(CBS_DROPDOWN);
			}
			else if (nRow == eGRD_ROW_EVENT_NOTIFY && nCol == eGRD_COL_PHIL_TITLE + 1)
			{
				pGrid->SetCellType(nRow, nCol, RUNTIME_CLASS(CGridCellCombo));
				CStringArray options;
				options.Add(_T("Event1"));
				options.Add(_T("Event100"));
				CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, nCol);
				pComboCell->SetOptions(options);
				pComboCell->SetStyle(CBS_DROPDOWN);
			}
			else if (nRow == eGRD_ROW_INTERRUPT_STOP && nCol == eGRD_COL_PHIL_TITLE + 1)
			{
				pGrid->SetCellType(nRow, nCol, RUNTIME_CLASS(CGridCellCombo));
				CStringArray options;
				options.Add(_T("Comp"));
				options.Add(_T("Error"));
				CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, nCol);
				pComboCell->SetOptions(options);
				pComboCell->SetStyle(CBS_DROPDOWN);
			}

			else if ((nRow == eGRD_ROW_ABS_MOVE || nRow == eGRD_ROW_REL_MOVE || nRow == eGRD_ROW_CHAR_MOVE)
				&& nCol == eGRD_COL_PHIL_TITLE + 1)
			{
				pGrid->SetCellType(nRow, nCol, RUNTIME_CLASS(CGridCellCombo));
				CStringArray options;
				options.Add(_T("ALIGN CAMERA Z 1"));
				options.Add(_T("ALIGN CAMERA Z 2"));
				CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, nCol);
				pComboCell->SetOptions(options);
				pComboCell->SetStyle(CBS_DROPDOWN);
			}
			else if ((nRow == eGRD_ROW_ABS_MOVE_COMP || nRow == eGRD_ROW_REL_MOVE_COMP || nRow == eGRD_ROW_CHAR_MOVE_COMP)
				&& nCol == eGRD_COL_PHIL_TITLE + 1)
			{
				pGrid->SetCellType(nRow, nCol, RUNTIME_CLASS(CGridCellCombo));
				CStringArray options;
				options.Add(_T("Comp"));
				options.Add(_T("Error"));
				CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(nRow, nCol);
				pComboCell->SetOptions(options);
				pComboCell->SetStyle(CBS_DROPDOWN);
			}
		}
	}

	/* 기본 속성 및 갱신 */
	//pGrid->SetBaseGridProp(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}


/*
 desc : 새로운 레시피 추가
 parm : mode	- [in]  0x00:Append, 0x01:Modify, 0x02:Delete
 retn : None
*/
VOID CDlgPhilhmi::RecipeControl(UINT8 mode)
{
	BOOL bUpdated = FALSE;
	TCHAR tzText[1024] = { NULL }, tzPath[MAX_PATH_LEN] = { NULL }, tzName[MAX_GERBER_NAME] = { NULL };

	CUniToChar csCnv;
	CDlgMesg dlgMesg;
	TCHAR szTemp[512] = { NULL };

	/* 레시피 저장 구조체 초기화 */

	CString strRecipeName = m_grd_ctl[ePHILHMI_GRD_RECIPE_LIST].GetItemText(m_nSelectRecipe[eRECIPE_MODE_VIEW], 1);


	/* 레시피 등록/수정/삭제 진행 */
	switch (mode)
	{
	case 0x00:
	{
		TCHAR tzText[RECIPE_NAME_LENGTH] = { NULL };
		CDlgKBDT dlg;
		if (IDOK == dlg.MyDoModal(RECIPE_NAME_LENGTH))
		{
			dlg.GetText(tzText, RECIPE_NAME_LENGTH);
			strRecipeName = tzText;

			if (IDNO == ShowYesNoMsg(eQUEST, _T("Do you want create [%s] recipe?"), strRecipeName))
			{
				return;
			}
		}
		else
		{
			return;
		}

		/*Philhmi 통신*/
		STG_PP_P2C_RCP_CREATE stSend;
		STG_PP_P2C_RCP_CREATE_ACK stRecv;
		stSend.Reset();
		stRecv.Reset();
		sprintf_s(stSend.szRecipeName, DEF_MAX_RECIPE_NAME_LENGTH, "%S", strRecipeName.GetString());



		uvEng_Philhmi_Send_P2C_RCP_CREATE(stSend, stRecv);
		AddListBox(_T("P2C Reicpe Create"));
		swprintf_s(szTemp, 512, L"Reicpe Create: %s", strRecipeName.GetString());
		AddListBox(szTemp);

		bUpdated = CRecipeManager::GetInstance()->CreateRecipe(strRecipeName);
		/* 등록 성공 여부 */
		if (!bUpdated)	return;

		/* 새롭게 등록된 레시피 갱신 */
		InitGridRecipeList();
		break;
	}
	case 0x01:

		/*Philhmi 통신*/
		STG_PP_P2C_RCP_MODIFY stSend2;
		STG_PP_P2C_RCP_MODIFY_ACK stRecv2;
		stSend2.Reset();
		stRecv2.Reset();
		sprintf_s(stSend2.szRecipeName, DEF_MAX_RECIPE_NAME_LENGTH, "%S", strRecipeName.GetString());
		uvEng_Philhmi_Send_P2C_RCP_MODIFY(stSend2, stRecv2);
		AddListBox(_T("P2C Reicpe Modify"));
		swprintf_s(szTemp, 512, L"Reicpe Modify: %s", strRecipeName.GetString());
		AddListBox(szTemp);


		bUpdated = CRecipeManager::GetInstance()->SaveRecipe(strRecipeName, eRECIPE_MODE_VIEW);
		/* 등록 성공 여부 */
		if (!bUpdated)	return;

		break;
	case 0x02:
	{
		if (m_nSelectRecipe[eRECIPE_MODE_SEL] == m_nSelectRecipe[eRECIPE_MODE_VIEW])
		{
			ShowMsg(eWARN, _T("can not delete current recipe"));
			return;
		}

		/*Philhmi 통신*/
		STG_PP_P2C_RCP_DELETE stSend3;
		STG_PP_P2C_RCP_DELETE_ACK stRecv3;
		stSend2.Reset();
		stRecv2.Reset();
		sprintf_s(stSend2.szRecipeName, DEF_MAX_RECIPE_NAME_LENGTH, "%S", strRecipeName.GetString());
		uvEng_Philhmi_Send_P2C_RCP_DELETE(stSend3, stRecv3);
		AddListBox(_T("P2C Reicpe Delete"));
		swprintf_s(szTemp, 512, L"Reicpe Delete: %s", strRecipeName.GetString());
		AddListBox(szTemp);

		bUpdated = CRecipeManager::GetInstance()->DeleteRecipe(strRecipeName);
		/* 등록 성공 여부 */
		if (!bUpdated)	return;

		// 삭제 후 현재 레시피로 변경
		SelectRecipe(m_nSelectRecipe[eRECIPE_MODE_SEL], eRECIPE_MODE_VIEW);

		/* 새롭게 등록된 레시피 갱신 */
		InitGridRecipeList();
	}
	break;
	}

	/* 전체 화면 갱신 */
	Invalidate(TRUE);
}

/*
 desc : 현재 선택된 Gerber Recipe 항목 메모리에 적재
 parm : None
 retn : None
*/
VOID CDlgPhilhmi::RecipeSelect()
{
	CUniToChar csCnv;
	CString strRecipeName = m_grd_ctl[ePHILHMI_GRD_RECIPE_LIST].GetItemText(m_nSelectRecipe[eRECIPE_MODE_VIEW], ePHILHMI_GRD_COL_RECIPE_LIST_NAME);

	/* 전역 메모리에 항목 선택 설정 진행 */
	if (!CRecipeManager::GetInstance()->SelectRecipe(strRecipeName))
	{
		ShowMsg(eWARN, L"Failed to select the gerber recipe", 0x01);
		return;
	}

	CString strReicpe, strExpo, strAlign;
	strReicpe = CRecipeManager::GetInstance()->GetRecipeName();
	strExpo = CRecipeManager::GetInstance()->GetExpoRecipeName();
	strAlign = CRecipeManager::GetInstance()->GetAlignRecipeName();

	m_pDlgMain->SendMessageW(WM_MAIN_RECIPE_UPDATE, EN_RECIPE_TAB::JOB, (LPARAM)&strReicpe);
	m_pDlgMain->SendMessageW(WM_MAIN_RECIPE_UPDATE, EN_RECIPE_TAB::EXPOSE, (LPARAM)&strExpo);
	m_pDlgMain->SendMessageW(WM_MAIN_RECIPE_UPDATE, EN_RECIPE_TAB::ALIGN, (LPARAM)&strAlign);


	/*Philhmi 통신*/
	TCHAR szTemp[512] = { NULL };
	STG_PP_P2C_RCP_SELECT stSend;
	STG_PP_P2C_RCP_SELECT_ACK stRecv;
	stSend.Reset();
	stRecv.Reset();
	sprintf_s(stSend.szRecipeName, DEF_MAX_RECIPE_NAME_LENGTH, "%S", strReicpe.GetString());
	uvEng_Philhmi_Send_P2C_RCP_SELECT(stSend, stRecv);
	AddListBox(_T("P2C Reicpe Select"));
	swprintf_s(szTemp, 512, L"Reicpe Select: %s", strReicpe.GetString());
	AddListBox(szTemp);

	SelectRecipe(m_nSelectRecipe[eRECIPE_MODE_VIEW]);
}

/*
 desc : 새로운 Job Name 등록
 parm : None
 retn : None
*/
PTCHAR CDlgPhilhmi::SelectGerber()
{
	TCHAR  tzJobName[MAX_PATH_LEN] = { NULL }, tzRLTFiles[MAX_PATH_LEN] = { NULL };
	CDlgMesg dlgMesg;
	LPG_LDJM pstJobMgt = &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* Job Name 디렉토리 선택 */
	if (!uvCmn_RootSelectPath(m_hWnd, uvEng_GetConfig()->set_comn.gerber_path, tzJobName))	return NULL;
	/* Job Name 내에 하위 디렉토리가 존재하는지 확인 */
	if (uvCmn_GetChildPathCount(tzJobName) != 0)
	{
		dlgMesg.MyDoModal(L"This is not a gerber directory", 0x01);
		return NULL;
	}
	/* 해당 디렉토리 내에 rlt_settings.xml 파일이 존재하는지 확인 */
	swprintf_s(tzRLTFiles, MAX_PATH_LEN, L"%s\\rlt_settings.xml", tzJobName);
	if (!uvCmn_FindFile(tzRLTFiles))
	{
		dlgMesg.MyDoModal(L"File (rlt_settings.xml) does not exist", 0x01);
		return NULL;
	}
	/* 기존에 등록된 거버 파일 중에 동일한 거버가 있는지 여부 확인 */
	if (pstJobMgt->IsJobFullFinded(csCnv.Uni2Ansi(tzJobName)))
	{
		dlgMesg.MyDoModal(L"Job Name already registered exist", 0x01);
		return NULL;
	}

	return tzJobName;
}




// by sysandj : philhmi 관련 메세지 요청
//LRESULT CDlgPhilhmi::OnMsgMainPHILHMI(WPARAM wparam, LPARAM lparam)
//{
//	STG_PP_PACKET_RECV* pstPhil = (STG_PP_PACKET_RECV*)wparam;
//	CUniToChar csCnv1;
//	CString strRecipe;
//
//	TCHAR szTemp[512] = { NULL };
//
//	switch (pstPhil->st_header.nCommand)
//	{
//		/// <summary>
//		/// PHILHMI 요청
//		/// </summary>
//	case ePHILHMI_C2P_RECIPE_CREATE:
//		STG_PP_C2P_RCP_CREATE_ACK stCreateSend;
//		strRecipe.Format(_T("%s"), csCnv1.Ansi2Uni(pstPhil->st_c2p_rcp_create.szRecipeName));
//
//		/*Recipe 생성 요청 성공*/
//		if (CRecipeManager::GetInstance()->CreateRecipe(strRecipe))
//		{
//			uvEng_Philhmi_Send_C2P_RCP_CREATE_ACK(stCreateSend);
//
//			CRecipeManager::GetInstance()->LoadRecipeList();
//		}
//		/*Recipe 생성 요청 실패*/
//		else
//		{
//			stCreateSend.usErrorCode;
//			uvEng_Philhmi_Send_C2P_RCP_CREATE_ACK(stCreateSend);
//		}
//
//		AddListBox(_T("C2P Recipe Create"));
//		break;
//	case ePHILHMI_C2P_RECIPE_DELETE:
//		STG_PP_C2P_RCP_DELETE_ACK stDeleteSend;
//
//		strRecipe.Format(_T("%s"), csCnv1.Ansi2Uni(pstPhil->st_c2p_rcp_delete.szRecipeName));
//
//		/*Recipe 삭제 요청 성공*/
//		if (CRecipeManager::GetInstance()->DeleteRecipe(strRecipe))
//		{
//			uvEng_Philhmi_Send_C2P_RCP_DELETE_ACK(stDeleteSend);
//
//			CRecipeManager::GetInstance()->LoadRecipeList();
//		}
//		/*Recipe 삭제 요청 실패*/
//		else
//		{
//			stDeleteSend.usErrorCode;
//			uvEng_Philhmi_Send_C2P_RCP_DELETE_ACK(stDeleteSend);
//		}
//
//		AddListBox(_T("C2P Recipe Delecte"));
//		break;
//	case ePHILHMI_C2P_RECIPE_MODIFY:
//		STG_PP_C2P_RCP_MODIFY_ACK stModifySend;
//		uvEng_Philhmi_Send_C2P_RCP_MODIFY_ACK(stModifySend);
//
//		strRecipe.Format(_T("%s"), csCnv1.Ansi2Uni(pstPhil->st_c2p_rcp_modify.szRecipeName));
//
//		/*Recipe 생성 요청 성공*/
//		//if (CRecipeManager::GetInstance()->UpdateRecipe(strRecipe))
//		//{
//		//	uvEng_Philhmi_Send_C2P_RCP_MODIFY_ACK(stModifySend);
//		//
//		//	CRecipeManager::GetInstance()->LoadRecipeList();
//		//}
//		//*Recipe 생성 요청 실패*/
//		//else
//		//{
//		//	stModifySend.usErrorCode;
//		//	uvEng_Philhmi_Send_C2P_RCP_MODIFY_ACK(stModifySend);
//		//}
//
//		AddListBox(_T("C2P Recipe Modify"));
//		break;
//	case ePHILHMI_C2P_RECIPE_SELECT:
//		STG_PP_C2P_RCP_SELECT_ACK stSelectSend;
//
//		strRecipe.Format(_T("%s"), csCnv1.Ansi2Uni(pstPhil->st_c2p_rcp_delete.szRecipeName));
//
//		/*Recipe 선택 요청 성공*/
//		if (CRecipeManager::GetInstance()->SelectRecipe(strRecipe))
//		{
//			uvEng_Philhmi_Send_C2P_RCP_SELECT_ACK(stSelectSend);
//
//			CRecipeManager::GetInstance()->LoadRecipeList();
//		}
//		/*Recipe 선택 요청 실패*/
//		else
//		{
//			stSelectSend.usErrorCode;
//			uvEng_Philhmi_Send_C2P_RCP_SELECT_ACK(stSelectSend);
//		}
//
//		AddListBox(_T("C2P Recipe Select"));
//		break;
//	case ePHILHMI_C2P_RECIPE_LIST:
//		//pstPhil->st_c2p_rcp_list;
//		CRecipeManager::GetInstance()->LoadRecipeList();
//
//		AddListBox(_T("C2P Recipe List"));
//		break;
//	case ePHILHMI_C2P_ABS_MOVE:
//		STG_PP_C2P_ABS_MOVE_ACK stAbsAck;
//		//pstPhil->st_c2p_abs_move;
//
//		//uvEng_Philhmi_Send_C2P_ABS_MOVE_ACK(stAbsAck);
//		//pstPhil->st_c2p_abs_move.stMove[0].szAxisName;
//
//		AddListBox(_T("C2P Abs Move"));
//		break;
//	case ePHILHMI_C2P_ABS_MOVE_COMPLETE:
//		STG_PP_C2P_ABS_MOVE_COMP_ACK stAbsComplete;
//		//pstPhil->st_c2p_abs_move_comp;
//
//		uvEng_Philhmi_Send_C2P_ABS_MOVE_COMP_ACK(stAbsComplete);
//
//		AddListBox(_T("C2P Abs Move Complete"));
//		break;
//	case ePHILHMI_C2P_REL_MOVE:
//		STG_PP_C2P_REL_MOVE_ACK stRelAck;
//		uvEng_Philhmi_Send_C2P_REL_MOVE_ACK(stRelAck);
//
//		AddListBox(_T("C2P Rel Move"));
//		break;
//	case ePHILHMI_C2P_REL_MOVE_COMPLETE:
//		STG_PP_C2P_REL_MOVE_COMP_ACK stRelComplete;
//		uvEng_Philhmi_Send_C2P_REL_MOVE_COMP_ACK(stRelComplete);
//
//		AddListBox(_T("C2P Rel Move Complete"));
//		break;
//	case ePHILHMI_C2P_CHAR_MOVE:
//		STG_PP_C2P_CHAR_MOVE_ACK stCharAck;
//		uvEng_Philhmi_Send_C2P_CHAR_MOVE_ACK(stCharAck);
//		break;
//	case ePHILHMI_C2P_CHAR_MOVE_COMPLETE:
//		STG_PP_C2P_CHAR_MOVE_COMP_ACK stCharComplete;
//		uvEng_Philhmi_Send_C2P_CHAR_MOVE_COMP_ACK(stCharComplete);
//
//		AddListBox(_T("C2P Char Move Complete"));
//		break;
//	case ePHILHMI_C2P_PROCESS_EXECUTE:
//		STG_PP_C2P_PROCESS_EXECUTE_ACK stProcessExecute;
//		uvEng_Philhmi_Send_C2P_PROCESS_EXECUTE_ACK(stProcessExecute);
//
//		AddListBox(_T("C2P Precess Execute"));
//		break;
//	case ePHILHMI_C2P_SUB_PROCESS_EXECUTE:
//		STG_PP_C2P_SUBPROCESS_EXECUTE_ACK stSubProcessExecute;
//		uvEng_Philhmi_Send_C2P_SUBPROCESS_EXECUTE_ACK(stSubProcessExecute);
//
//		AddListBox(_T("C2P Sub Process Execute"));
//		break;
//	case ePHILHMI_C2P_STATUS_VALUE:
//		STG_PP_C2P_STATUS_VALUE_ACK		stStatusValue;
//		uvEng_Philhmi_Send_C2P_STATUS_VALUE_ACK(stStatusValue);
//
//		AddListBox(_T("C2P Status Value"));
//		break;
//	case ePHILHMI_C2P_MODE_CHANGE:
//		STG_PP_C2P_MODE_CHANGE_ACK stModeChange;
//		stModeChange.usMode = pstPhil->st_c2p_mode_change.usMode;
//		AddListBox(_T("C2P Mode Change Ack"));
//		AddListBox(_T("Mode : %d", stModeChange.usMode));
//		AddListBox(szTemp);
//		uvEng_Philhmi_Send_C2P_MODE_CHANGE_ACK(stModeChange);
//		break;
//	case ePHILHMI_C2P_EVENT_NOTIFY:
//		STG_PP_C2P_EVENT_NOTIFY_ACK		stEventNotify;
//		uvEng_Philhmi_Send_C2P_EVENT_NOTIFY_ACK(stEventNotify);
//
//		AddListBox(_T("C2P Event Notify"));
//		break;
//	case ePHILHMI_C2P_TIME_SYNC:
//		STG_PP_C2P_TIME_SYNC_ACK		stTimeSync;
//		uvEng_Philhmi_Send_C2P_TIME_SYNC_ACK(stTimeSync);
//
//		AddListBox(_T("C2P Time Sync"));
//		break;
//	case ePHILHMI_C2P_INTERRUPT_STOP:
//		STG_PP_C2P_INTERRUPT_STOP_ACK	stInterruptStop;
//		uvEng_Philhmi_Send_C2P_INTERRUPT_STOP_ACK(stInterruptStop);
//
//		AddListBox(_T("C2P Interrupt Stop"));
//		break;
//
//		/// <summary>
//		/// 보낸 명령에 대한 ACK
//		/// </summary>
//	case ePHILHMI_P2C_RECIPE_CREATE:
//		break;
//	case ePHILHMI_P2C_RECIPE_DELETE:
//		break;
//	case ePHILHMI_P2C_RECIPE_MODIFY:
//		break;
//	case ePHILHMI_P2C_RECIPE_SELECT:
//		break;
//	case ePHILHMI_P2C_IO_STATUS:
//		break;
//	case ePHILHMI_P2C_OUTPUT_ONOFF:
//		break;
//	case ePHILHMI_P2C_ABS_MOVE:
//		break;
//	case ePHILHMI_P2C_ABS_MOVE_COMPLETE:
//		break;
//	case ePHILHMI_P2C_REL_MOVE:
//		break;
//	case ePHILHMI_P2C_REL_MOVE_COMPLETE:
//		break;
//	case ePHILHMI_P2C_CHAR_MOVE:
//		break;
//	case ePHILHMI_P2C_CHAR_MOVE_COMPLETE:
//		break;
//	case ePHILHMI_P2C_PROCESS_STEP:
//		break;
//	case ePHILHMI_P2C_PROCESS_COMPLETE:
//		break;
//	case ePHILHMI_P2C_SUB_PROCESS_COMPLETE:
//		break;
//	case ePHILHMI_P2C_STATUS_VALUE:
//		break;
//	case ePHILHMI_P2C_INITIAL_COMPLETE:
//		break;
//	case ePHILHMI_P2C_ALARM_OCCUR:
//		break;
//	case ePHILHMI_P2C_EVENT_NOTIFY:
//		break;
//	case ePHILHMI_P2C_INTERRUPT_STOP:
//		break;
//	default:
//		// 정의되지 않은 요청
//		break;
//	}
//
//	return 0L;
//}
























int CDlgPhilhmi::ShowMsg(EN_MSG_BOX_TYPE mType, LPCTSTR lpszMsg, ...)
{
	TCHAR		out[2048];
	va_list		va;
	int nCommButton = TDCBF_OK_BUTTON;

	CTaskDialog taskDlg(_T(""), _T(""), _T(""), nCommButton);

	va_start(va, lpszMsg);
	vswprintf_s(out, _countof(out), lpszMsg, va);
	va_end(va);

	CString strMsg = out;

	switch (mType) {
	case eINFO: taskDlg.SetMainIcon(IDI_INFORMATION); break;
	case eSTOP: taskDlg.SetMainIcon(IDI_ERROR); break;
	case eWARN: taskDlg.SetMainIcon(IDI_WARNING); break;
	case eQUEST: taskDlg.SetMainIcon(IDI_INFORMATION); break;
	default:
		taskDlg.SetMainIcon(IDI_INFORMATION);
	}

	taskDlg.SetWindowTitle(_T("MESSAGE"));
	taskDlg.SetMainInstruction(strMsg);
	taskDlg.SetDialogWidth(::GetSystemMetrics(SM_CXSCREEN) / 6);

	return (int)taskDlg.DoModal();
}

int CDlgPhilhmi::ShowYesNoMsg(EN_MSG_BOX_TYPE mType, LPCTSTR lpszMsg, ...)
{
	TCHAR		out[2048];
	va_list		va;
	LPCWSTR		curIcon;
	int nCommButton = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON;

	va_start(va, lpszMsg);
	vswprintf_s(out, _countof(out), lpszMsg, va);
	va_end(va);

	CString strMsg = out;

	switch (mType) {
	case eINFO: curIcon = TD_INFORMATION_ICON; break;
	case eSTOP: curIcon = TD_ERROR_ICON; break;
	case eWARN: curIcon = TD_WARNING_ICON; break;
	case eQUEST: curIcon = TD_INFORMATION_ICON; break;
	default:
		curIcon = TD_INFORMATION_ICON;
	}


	CTaskDialog taskDlg(_T(""), strMsg, _T(""), nCommButton);
	//taskDlg.SetMainIcon( curIcon );
	taskDlg.SetMainIcon(curIcon);
	taskDlg.SetWindowTitle(_T("SELECT MESSAGE"));
	taskDlg.SetMainInstruction(strMsg);
	taskDlg.SetDialogWidth(::GetSystemMetrics(SM_CXSCREEN) / 6);

	return (int)taskDlg.DoModal();
}

int CDlgPhilhmi::ShowMultiSelectMsg(EN_MSG_BOX_TYPE mType, CString strTitle, CStringArray& strArrCommand)
{
	// 	TCHAR		out[1024];
	// 	va_list		va;
	if (0 == strArrCommand.GetCount())
	{
		return -1;
	}

	CTaskDialog taskDlg(_T(""), _T(""), _T(""), NULL);

	switch (mType)
	{
	case eINFO: taskDlg.SetMainIcon(IDI_INFORMATION); break;
	case eSTOP: taskDlg.SetMainIcon(IDI_ERROR); break;
	case eWARN: taskDlg.SetMainIcon(IDI_WARNING); break;
	case eQUEST: taskDlg.SetMainIcon(IDI_INFORMATION); break;
	default:
		taskDlg.SetMainIcon(IDI_INFORMATION);
	}

	CStringArray strArrMsg;

	for (int i = 0; i < strArrCommand.GetCount(); i++)
	{
		strArrMsg.Add(strArrCommand.GetAt(i));
	}

	taskDlg.SetWindowTitle(strTitle);
	taskDlg.SetMainInstruction(strTitle);

	for (int i = 0; i < strArrMsg.GetCount(); i++)
	{
		taskDlg.AddCommandControl(201 + i, strArrMsg.GetAt(i));
	}

	taskDlg.SetDialogWidth(::GetSystemMetrics(SM_CXSCREEN) / 6);

	int nResult = (int)taskDlg.DoModal();

	return nResult - 201;
}

BOOL CDlgPhilhmi::PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts/* = 0*/)
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