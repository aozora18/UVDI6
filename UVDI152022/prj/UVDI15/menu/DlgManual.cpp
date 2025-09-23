
/*
 desc : Exposure
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgManual.h"

#include "./expo/DrawMark.h"	/* Mark Search Results Object */
#include "./expo/DrawDev.h"		/* TCP/IP Communication Status */
#include "./expo/MarkDist.h"	/* Distance between marks */
#include "./expo/ExpoVal.h"		/* Update to realtime value */
#include "./expo/TempAlarm.h"	/* Temperature Alarm */

#include "../pops/DlgStep.h"
#include "../pops/DlgRept.h"
#include "../mesg/DlgMesg.h"
#include "../param/RecipeManager.h"
#include "../../../inc/kybd/DlgKBDT.h"
#include "DlgMmpm.h"
#include <atlcoll.h>
#include "../../../inc/conf/vision_uvdi15.h"
#include "../GlobalVariables.h"
#include "../pops/DlgParam.h"
#include <atlstr.h>
#include <tuple>
#include <vector>
#include <map>

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

class GlobalVariables;


/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgManual::CDlgManual(UINT32 id, CWnd* parent)
	: CDlgMenu(id, parent)
{

	/* 자신의 다이얼로그 ID 설정 */
	m_enDlgID	= ENG_CMDI::en_menu_manual;
	m_u64ReqTime= GetTickCount64();

	for (int i = 0; i < _countof(m_pGrd); i++)
	{
		m_pGrd[i] = NULL;
	}

	for (int i = 0; i < _countof(m_pBtn); i++)
	{
		m_pBtn[i] = NULL;
	}

	for (int i = 0; i < _countof(m_pStt); i++)
	{
		m_pStt[i] = NULL;
	}

	m_pPgr = NULL;
	m_bBlink = FALSE;
	m_u64TickCount = 0;
	//uvEng_Luria_ReqGetGetTransformationParams(0);

	/*페이지 시작 마다 재 설정*/
	CRecipeManager::GetInstance()->GetLEDFrameRate();
}

CDlgManual::~CDlgManual()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgManual::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	u32StartID = DEF_UI_MANUAL_STT;
	for (i = 0; i < EN_MANUAL_STT::_size(); i++)
	{
		m_pStt[i] = new CMyStatic();

		DDX_Control(dx, u32StartID + i, *m_pStt[i]);
	}
}

BEGIN_MESSAGE_MAP(CDlgManual, CDlgMenu)
	ON_WM_LBUTTONDOWN()
	ON_CONTROL_RANGE(BN_CLICKED, DEF_UI_MANUAL_BTN, DEF_UI_MANUAL_BTN + (UINT)EN_MANUAL_BTN::_size(), OnBtnClick)
	ON_NOTIFY_RANGE(NM_CLICK, DEF_UI_MANUAL_GRD, DEF_UI_MANUAL_GRD + eGRD_MAX, OnGrdClick)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgManual::PreTranslateMessage(MSG* msg)
{
	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgManual::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();
	/* 객체 초기화 */
	if (!InitObject())	return FALSE;

	InitGridInformation();
	InitGridParameter();

	//CalcEnergy();
	
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgManual::OnExitDlg()
{
	CloseObject();

	for (int i = 0; i < _countof(m_pGrd); i++)
	{
		if (m_pGrd[i])
		{
			m_pGrd[i]->DeleteAllItems();
			delete m_pGrd[i];
			m_pGrd[i] = NULL;
		}
	}

	for (int i = 0; i < _countof(m_pBtn); i++)
	{
		if (m_pBtn[i])
		{
			delete m_pBtn[i];
			m_pBtn[i] = NULL;
		}
	}

	for (int i = 0; i < _countof(m_pStt); i++)
	{
		if (m_pStt[i])
		{
			delete m_pStt[i];
			m_pStt[i] = NULL;
		}
	}

	if (m_pPgr)
	{
		delete m_pPgr;
		m_pPgr = NULL;
	}

	delete m_pdlgMarkShow;
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgManual::OnPaintDlg(CDC *dc)
{
	if (m_pDrawPrev)	m_pDrawPrev->Draw();

	/* 자식 윈도 호출 */
	CDlgMenu::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgManual::OnResizeDlg()
{
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgManual::UpdatePeriod(UINT64 tick, BOOL is_busy)
{
	if (m_pPgr)
	{
		m_pPgr->SetPos((int)uvEng_GetWorkStepRate());

		m_pPgr->m_strTestText = uvEng_GetWorkStepName();
	}

	if (m_pDlgMain)
	{
		UpdateBtn(tick, is_busy);
	}

	m_u64TickCount++;
	m_bBlink = !m_bBlink;
	DrawMarkData();

	

	AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();
	auto motionType = motions.markParams.alignMotion;


	
	
	

	if (motionType == ENG_AMOS::none)
		return;

	stringstream temp;
	//temp <<  "CHANGE ALIGN MODE\n" << (motionType == ENG_AMOS::en_onthefly_2cam ? "[refind]" : "[normal]");
	temp << "CHANGE ALIGN MODE\n" << (motionType == ENG_AMOS::en_onthefly_2cam ? "[normal]" : "[refind]");

	ENG_WETE workerror = GlobalVariables::GetInstance()->GetAlignMotion().markParams.workErrorType;

	/*LDS Thick error 버튼 표시*/
	if (workerror == ENG_WETE::en_none)
	{
		m_pBtn[EN_MANUAL_BTN::ERROR_THICK]->SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
		m_pBtn[EN_MANUAL_BTN::ERROR_THICK]->RedrawWindow();
	}
	else if (workerror == ENG_WETE::en_lds_thick_check)
	{
		m_pBtn[EN_MANUAL_BTN::ERROR_THICK]->SetBgColor(LIGHT_YELLOW);
		m_pBtn[EN_MANUAL_BTN::ERROR_THICK]->RedrawWindow();
	}

	/*자동화 동작시 AlignExpo 동작 비활성*/
	if (g_u8Romote == en_menu_phil_mode_auto)
	{
		m_pBtn[EN_MANUAL_BTN::ALIGN_EXPOSE]->SetBgColor(DEF_COLOR_BTN_MENU_NORMAL);
		m_pBtn[EN_MANUAL_BTN::ALIGN_EXPOSE]->SetTextColor(DEF_COLOR_BTN_PAGE_NORMAL_TEXT);
	}

	

	USES_CONVERSION;
	m_pBtn[EN_MANUAL_BTN::CHANGE_ALIGN_MODE]->SetWindowTextW(A2T(temp.str().c_str()));

}

/*
 desc : 컨트롤
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgManual::UpdateControl(UINT64 tick, BOOL is_busy)
{

}


VOID CDlgManual::UpdateBtn(UINT64 tick, BOOL is_busy)
{
	for (int i = EN_MANUAL_BTN::DIRECT_EXPOSE; i <= EN_MANUAL_BTN::STOP; i++)
	{
		m_pBtn[i]->SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
	}

	m_pBtn[EN_MANUAL_BTN::USE_AF]->SetBgColor(GlobalVariables::GetInstance()->GetAutofocus().GetUseAF() ? LIGHT_RED : DEF_COLOR_BTN_PAGE_NORMAL);
	m_pBtn[EN_MANUAL_BTN::USE_AF]->RedrawWindow();
	


	int nBtnIndex = -1;
	if (100 > uvEng_GetWorkStepRate())
	{
		switch (m_pDlgMain->GetWorkJobID())
		{
		case ENG_BWOK::en_work_ready:
			break;
		case ENG_BWOK::en_work_busy:
			break;
		case ENG_BWOK::en_work_init:
			break;
		case ENG_BWOK::en_work_home:
			break;
		case ENG_BWOK::en_work_stop:
			break;
		case ENG_BWOK::en_gerb_load:
		case ENG_BWOK::en_local_gerb_load:
			nBtnIndex = EN_MANUAL_BTN::RECIPE_LOAD;
			break;
		case ENG_BWOK::en_gerb_unload:
			nBtnIndex = EN_MANUAL_BTN::RECIPE_UNLOAD;
			break;
		case ENG_BWOK::en_mark_move:
			nBtnIndex = EN_MANUAL_BTN::MARK;
			break;
		case ENG_BWOK::en_mark_test:
			nBtnIndex = EN_MANUAL_BTN::ALIGN_TEST;
			break;
		case ENG_BWOK::en_expo_only:
			nBtnIndex = EN_MANUAL_BTN::DIRECT_EXPOSE;
			break;
		case ENG_BWOK::en_expo_align:
			nBtnIndex = EN_MANUAL_BTN::ALIGN_EXPOSE;
			break;
		case ENG_BWOK::en_expo_cali:
			break;
		default:
			break;
		}

		if (0 > nBtnIndex)
		{
			return;
		}

		if (m_bBlink)
		{
			m_pBtn[nBtnIndex]->SetBgColor(SEA_GREEN);
			//if (m_pBtn[nBtnIndex]->GetBgColor() != SEA_GREEN)
			{
				m_pBtn[nBtnIndex]->RedrawWindow();
				TRACE(_T("m_pBtn[%d]=GREEN\n"), nBtnIndex);
			}
		}
		else
		{
			m_pBtn[nBtnIndex]->SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
			//if (m_pBtn[nBtnIndex]->GetBgColor() != DEF_COLOR_BTN_PAGE_NORMAL)
			{
				m_pBtn[nBtnIndex]->RedrawWindow();
				TRACE(_T("m_pBtn[%d]=NORMAL\n"), nBtnIndex);
			}
		}


		/*동작 초기헤 UI 파라미터값 초기화*/
		if (uvEng_GetWorkStepRate() < 10)
		{

			if (m_pDlgMain->GetWorkJobID() == ENG_BWOK::en_gerb_load ||
				m_pDlgMain->GetWorkJobID() == ENG_BWOK::en_mark_test ||
				m_pDlgMain->GetWorkJobID() == ENG_BWOK::en_expo_align)
			{
				UpdateGridInformation();
			}
		}
	}
	else
	{
		switch (m_pDlgMain->GetWorkJobID())
		{
		case ENG_BWOK::en_work_ready:
			break;
		case ENG_BWOK::en_work_busy:
			break;
		case ENG_BWOK::en_work_init:
			break;
		case ENG_BWOK::en_work_home:
			break;
		case ENG_BWOK::en_work_stop:
			nBtnIndex = EN_MANUAL_BTN::STOP;
			break;
		case ENG_BWOK::en_gerb_load:
			nBtnIndex = EN_MANUAL_BTN::RECIPE_LOAD;
			break;
		case ENG_BWOK::en_gerb_unload:
			nBtnIndex = EN_MANUAL_BTN::RECIPE_UNLOAD;
			break;
		case ENG_BWOK::en_mark_move:
			nBtnIndex = EN_MANUAL_BTN::MARK;
			break;
		case ENG_BWOK::en_mark_test:
			nBtnIndex = EN_MANUAL_BTN::ALIGN_TEST;
			break;
		case ENG_BWOK::en_expo_only:
			nBtnIndex = EN_MANUAL_BTN::DIRECT_EXPOSE;
			break;
		case ENG_BWOK::en_expo_align:
			nBtnIndex = EN_MANUAL_BTN::ALIGN_EXPOSE;
			break;
		case ENG_BWOK::en_expo_cali:
			break;
		default:
			break;
		}

		if (0 > nBtnIndex)
		{
			return;
		}

		m_pBtn[nBtnIndex]->SetBgColor(DEF_COLOR_BTN_PAGE_SELECT);
		//if (m_pBtn[nBtnIndex]->GetBgColor() != DEF_COLOR_BTN_PAGE_SELECT)
		{
			m_pBtn[nBtnIndex]->RedrawWindow();
			TRACE(_T("m_pBtn[%d]=SELECT\n"), nBtnIndex);
		}
		/*동작 완료때 UI 파라미터값 초기화*/
		if (m_pDlgMain->GetWorkJobID() == ENG_BWOK::en_work_stop ||
			m_pDlgMain->GetWorkJobID() == ENG_BWOK::en_gerb_load ||
			m_pDlgMain->GetWorkJobID() == ENG_BWOK::en_mark_test ||
			m_pDlgMain->GetWorkJobID() == ENG_BWOK::en_expo_align)
		{
			UpdateGridInformation();
		}
	}
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgManual::InitCtrl()
{
	INT32 i = 0;

	CResizeUI clsResizeUI;
	for (i = 0; i < _countof(m_pStt); i++)
	{
		m_pStt[i]->SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_pStt[i]->SetDrawBg(1);
		m_pStt[i]->SetBaseProp(0, 1, 0, 0, FORD_GRAPHITE, 0, WHITE_);

		clsResizeUI.ResizeControl(this, m_pStt[i]);
	}

	CRect rtStt;
	m_pStt[EN_MANUAL_STT::PREVIEW]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	CRect rtPreview;
	GetDlgItem(IDC_MANUAL_PIC_PREVIEW)->GetWindowRect(rtPreview);
	int nHeight			= (int)(rtPreview.Height() * clsResizeUI.GetRateY());
	rtPreview			= rtStt;
	rtPreview.top		= rtStt.bottom + DEF_UI_OFFSET;
	rtPreview.bottom	= rtPreview.top + DEF_UI_OFFSET + nHeight;
	GetDlgItem(IDC_MANUAL_PIC_PREVIEW)->MoveWindow(rtPreview);
	CRect rtOffset;
	m_pDrawPrev = new CDrawPrev(GetDlgItem(IDC_MANUAL_PIC_PREVIEW)->GetSafeHwnd(), rtOffset);

	
	m_pStt[EN_MANUAL_STT::PROCESS]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	CRect rtPrg;
	rtPrg.left		= rtStt.left;
	rtPrg.top		= rtStt.bottom + DEF_UI_OFFSET;
	rtPrg.right		= rtStt.right;
	rtPrg.bottom	= rtPrg.top + rtStt.Height();
	m_pPgr = new CProgressCtrlX();
	m_pPgr->Create(WS_BORDER | WS_VISIBLE, rtPrg, this, DEF_UI_MANUAL_PRG);
	m_pPgr->SetBkColor(ALICE_BLUE);
	m_pPgr->SetTextColor(BLACK_);
	m_pPgr->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	m_pPgr->SetShowPercent(TRUE);
	m_pPgr->SetRange(0, 100);

	CRect rtBtn;;
	UINT  uiBtnWidth = (rtStt.Width() - DEF_UI_OFFSET * 3) / 4;
	UINT  uiBtnHeight = rtStt.Height();

	rtBtn.left = rtStt.left;
	rtBtn.top = rtPrg.bottom + DEF_UI_OFFSET;
	rtBtn.right = rtBtn.left + uiBtnWidth;
	rtBtn.bottom = rtBtn.top + uiBtnHeight;
	CString strText;

	//////////////////////////////////////////////////////////////////////////
	int nBtnIndex = EN_MANUAL_BTN::DIRECT_EXPOSE;
	for (nBtnIndex = EN_MANUAL_BTN::DIRECT_EXPOSE; nBtnIndex <= EN_MANUAL_BTN::ALIGN_EXPOSE; nBtnIndex++)
	{
		if (EN_MANUAL_BTN::DIRECT_EXPOSE < nBtnIndex)
		{
			rtBtn.OffsetRect(uiBtnWidth + DEF_UI_OFFSET, 0);
		}
		m_pBtn[nBtnIndex] = new CMacButton();
		strText = (CString)EN_MANUAL_BTN::_from_index(nBtnIndex)._to_string();
		strText.Replace(_T("_"), _T(" "));
		m_pBtn[nBtnIndex]->Create(strText, WS_BORDER | WS_VISIBLE, rtBtn, this, DEF_UI_MANUAL_BTN + nBtnIndex);
		m_pBtn[nBtnIndex]->SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_pBtn[nBtnIndex]->SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
		m_pBtn[nBtnIndex]->SetTextColor(DEF_COLOR_BTN_PAGE_NORMAL_TEXT);
	}

	uiBtnWidth	= (rtStt.Width() - DEF_UI_OFFSET * 3) / 4;
	rtBtn.left	= rtStt.left;
	rtBtn.right = rtBtn.left + uiBtnWidth;
	rtBtn.OffsetRect(0, uiBtnHeight + DEF_UI_OFFSET);
	nBtnIndex = EN_MANUAL_BTN::RECIPE_LOAD;
	for (nBtnIndex = EN_MANUAL_BTN::RECIPE_LOAD; nBtnIndex <= EN_MANUAL_BTN::STOP; nBtnIndex++)
	{
		if (EN_MANUAL_BTN::RECIPE_LOAD < nBtnIndex)
		{
			rtBtn.OffsetRect(uiBtnWidth + DEF_UI_OFFSET, 0);
		}
		m_pBtn[nBtnIndex] = new CMacButton();
		strText = (CString)EN_MANUAL_BTN::_from_index(nBtnIndex)._to_string();
		strText.Replace(_T("_"), _T(" "));
		m_pBtn[nBtnIndex]->Create(strText, WS_BORDER | WS_VISIBLE, rtBtn, this, DEF_UI_MANUAL_BTN + nBtnIndex);
		m_pBtn[nBtnIndex]->SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_pBtn[nBtnIndex]->SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
		m_pBtn[nBtnIndex]->SetTextColor(DEF_COLOR_BTN_PAGE_NORMAL_TEXT);
	}
	//////////////////////////////////////////////////////////////////////////

	//uiBtnWidth = (rtStt.Width() - DEF_UI_OFFSET * 2) / 3;
	//rtBtn = rtStt;
	//rtBtn.OffsetRect(0, uiBtnHeight + DEF_UI_OFFSET);
	//rtBtn.OffsetRect(0, uiBtnHeight + DEF_UI_OFFSET);
	//rtBtn.OffsetRect(0, uiBtnHeight + DEF_UI_OFFSET);
	//rtBtn.OffsetRect(0, uiBtnHeight + DEF_UI_OFFSET);

	//uiBtnWidth = (rtStt.Width() - DEF_UI_OFFSET * 2) / 3;
	uiBtnWidth = (rtStt.Width() - DEF_UI_OFFSET * 3) / 4;
	rtBtn.left = rtStt.left;
	rtBtn.right = rtBtn.left + uiBtnWidth;
	rtBtn.OffsetRect(0, uiBtnHeight + DEF_UI_OFFSET);

	nBtnIndex = EN_MANUAL_BTN::CALIBRATION_MARK;
	//for (nBtnIndex = EN_MANUAL_BTN::CALIBRATION_MARK; nBtnIndex <= EN_MANUAL_BTN::CHANGE_ALIGN_MODE; nBtnIndex++)
	for (nBtnIndex = EN_MANUAL_BTN::CALIBRATION_MARK; nBtnIndex <= EN_MANUAL_BTN::ERROR_THICK; nBtnIndex++)
	{
		if (EN_MANUAL_BTN::CALIBRATION_MARK < nBtnIndex)
		{
			rtBtn.OffsetRect(uiBtnWidth + DEF_UI_OFFSET, 0);
		}
		m_pBtn[nBtnIndex] = new CMacButton();
		strText = (CString)EN_MANUAL_BTN::_from_index(nBtnIndex)._to_string();
		strText.Replace(_T("_"), _T(" "));
		m_pBtn[nBtnIndex]->Create(strText, WS_BORDER | WS_VISIBLE | BS_MULTILINE | BS_VCENTER, rtBtn, this, DEF_UI_MANUAL_BTN + nBtnIndex);
		m_pBtn[nBtnIndex]->SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_pBtn[nBtnIndex]->SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
		m_pBtn[nBtnIndex]->SetTextColor(DEF_COLOR_BTN_PAGE_NORMAL_TEXT);
	}


	m_pStt[EN_MANUAL_STT::PARAMETER]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	uiBtnWidth	= rtStt.Width() / 3 - DEF_UI_OFFSET;
	uiBtnHeight	= (DEF_UI_GRD_ROW_OFFSET * 4 - DEF_UI_OFFSET) / 2;
	rtBtn.left	= rtStt.left + rtStt.Width() / 3 * 2 + DEF_UI_OFFSET;
	rtBtn.right = rtBtn.left + uiBtnWidth;
	rtBtn.top	= rtStt.bottom + DEF_UI_OFFSET;
	rtBtn.bottom = rtBtn.top + uiBtnHeight;
	
	///////////////////////////////////////////////////////////////////////
	nBtnIndex = EN_MANUAL_BTN::ENERGE;
	for (nBtnIndex = EN_MANUAL_BTN::ENERGE; nBtnIndex <= EN_MANUAL_BTN::LOAD; nBtnIndex++)
	{
		if (EN_MANUAL_BTN::ENERGE < nBtnIndex)
		{
			rtBtn.OffsetRect(0, uiBtnHeight + DEF_UI_OFFSET);
		}
		m_pBtn[nBtnIndex] = new CMacButton();
		strText = (CString)EN_MANUAL_BTN::_from_index(nBtnIndex)._to_string();
		strText.Replace(_T("_"), _T(" "));
		m_pBtn[nBtnIndex]->Create(strText, WS_BORDER | WS_VISIBLE | BS_MULTILINE | BS_VCENTER, rtBtn, this, DEF_UI_MANUAL_BTN + nBtnIndex);
		m_pBtn[nBtnIndex]->SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_pBtn[nBtnIndex]->SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
		m_pBtn[nBtnIndex]->SetTextColor(DEF_COLOR_BTN_PAGE_NORMAL_TEXT);
	}
	//////////////////////////////////////////////////////////////////////////

	m_pBtn[EN_MANUAL_BTN::MARK]->SetWindowTextW(_T("MARK [ALL]"));
	m_pBtn[EN_MANUAL_BTN::MARK]->RedrawWindow();

	m_pBtn[EN_MANUAL_BTN::ENERGE]->SetMultiLines(3);

	m_pdlgMarkShow = new CDlgMarkShow(this);
	m_pdlgMarkShow->Create();
	m_pdlgMarkShow->ShowWindow(SW_HIDE);
}

/*
 desc : 객체 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgManual::InitObject()
{
	m_stJob.Init();
	LPG_RJAF pJob = uvEng_JobRecipe_GetSelectRecipe(uvEng_JobRecipe_WhatLastSelectIsLocal());

	try
	{
		if (pJob == nullptr) 
			return FALSE;

		m_stJob.material_thick = pJob->material_thick;
		m_stJob.expo_energy = pJob->expo_energy;
		m_stJob.expo_speed = pJob->expo_speed;
		m_stJob.step_size = pJob->step_size;
		m_stJob.ldsThreshold = pJob->ldsThreshold;

		CUniToChar	csCnv;
		UINT8 i = 0, j = 0;
		DOUBLE dbTotal = 0.0f, dbPowerWatt[MAX_LED] = { NULL }, dbSpeed = 0.0f;
		LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pJob->expo_recipe));
		LPG_PLPI	pstPowerI = uvEng_LedPower_GetLedPowerName(csCnv.Ansi2Uni(pstExpoRecipe->power_name));
		/* 광량 계산 */
		for (; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
		{
			for (j = 0; j < MAX_LED; j++)	dbPowerWatt[j] = pstPowerI->led_watt[i][j];
			dbTotal += uvCmn_Luria_GetEnergyToSpeed(pJob->step_size, pJob->expo_energy,
				pstExpoRecipe->led_duty_cycle, dbPowerWatt);

		}
		m_stJob.frame_rate = (UINT16)(dbTotal / DOUBLE(i));


		strcpy_s(m_stJob.job_name, RECIPE_NAME_LENGTH, pJob->job_name);
		strcpy_s(m_stJob.gerber_path, MAX_PATH_LEN, pJob->gerber_path);
		strcpy_s(m_stJob.gerber_name, MAX_GERBER_NAME, pJob->gerber_name);
		strcpy_s(m_stJob.align_recipe, RECIPE_NAME_LENGTH, pJob->align_recipe);
		strcpy_s(m_stJob.expo_recipe, RECIPE_NAME_LENGTH, pJob->expo_recipe);

		if (m_stJob.IsValid())
		{
			m_pDrawPrev->LoadMark(uvEng_JobRecipe_GetSelectRecipe(uvEng_JobRecipe_WhatLastSelectIsLocal()));
			m_pDrawPrev->DrawMem(uvEng_JobRecipe_GetSelectRecipe(uvEng_JobRecipe_WhatLastSelectIsLocal()));
		}
	}
	catch(exception e)
	{
		return FALSE;
	}
	
	return TRUE;
}

/*
 desc : 객체 해제
 parm : None
 retn : None
*/
VOID CDlgManual::CloseObject()
{
	/* 마크 검색 결과 출력 객체 */
	if (m_pDrawPrev)	delete m_pDrawPrev;
}

void CDlgManual::InitGridInformation()
{
	CRect rtStt;
	CRect rctSize;										/* 작업 영역	*/

	double	dCellRatio[7]	= { 0.33, 0.112, 0.112, 0.112, 0.112, 0.112, 0.112 };				/* 그리드 비율	*/
	int		nHeightSize		= DEF_UI_GRD_ROW_OFFSET;	/* 셀 높이		*/
	int		nWidthDiffer	= 1;						/* 셀 너비 오차값 */

	UINT nCenterAlignText = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;	/* 그리드 옵션 */

	// 그리드 기본 설정
	int	nGridIndex = eGRD_INFORMATION;

	m_pStt[EN_MANUAL_STT::INFORMATION]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	rctSize			= rtStt;
	rctSize.top		= rtStt.bottom + DEF_UI_OFFSET;
	rctSize.bottom	= rctSize.top + (nHeightSize * (int)EN_GRD_INFORMATION_ROW::_size()) + 1;

	if (NULL == m_pGrd[nGridIndex])
	{
		m_pGrd[nGridIndex] = new CGridCtrl();
		m_pGrd[nGridIndex]->Create(rctSize, this, DEF_UI_MANUAL_GRD + nGridIndex, (WS_CHILD | WS_TABSTOP | WS_VISIBLE));
	}

	m_pGrd[nGridIndex]->MoveWindow(rctSize);
	m_pGrd[nGridIndex]->SetColumnResize(FALSE);
	m_pGrd[nGridIndex]->SetRowResize(FALSE);
	m_pGrd[nGridIndex]->SetEditable(FALSE);
	m_pGrd[nGridIndex]->EnableSelection(FALSE);
	m_pGrd[nGridIndex]->ModifyStyle(WS_HSCROLL, 0);

	m_pGrd[nGridIndex]->SetGridLineColor(LIGHT_GRAY);
	m_pGrd[nGridIndex]->SetTextColor(BLACK_);
	m_pGrd[nGridIndex]->DeleteAllItems();

	// 그리드 영역이 작업 영역을 초과하면 스크롤 바 만큼의 오차값 저장
	if (rctSize.Height() - 1 < nHeightSize * (int)EN_GRD_INFORMATION_ROW::_size())
	{
		nWidthDiffer = ::GetSystemMetrics(SM_CXVSCROLL);
	}

	// 셀의 개수 지정
	m_pGrd[nGridIndex]->SetColumnCount(_countof(dCellRatio));
	m_pGrd[nGridIndex]->SetRowCount((int)EN_GRD_INFORMATION_ROW::_size());

	CString strText;
	for (int nRow = 0; nRow < (int)EN_GRD_INFORMATION_ROW::_size(); nRow++)
	{
		m_pGrd[nGridIndex]->SetRowHeight(nRow, nHeightSize);
		m_pGrd[nGridIndex]->SetItemText(nRow, 0, (CString)EN_GRD_INFORMATION_ROW::_from_index(nRow)._to_string());

		for (int nCol = 0; nCol < _countof(dCellRatio); nCol++)
		{
			m_pGrd[nGridIndex]->SetColumnWidth(nCol, (int)((rctSize.Width() - nWidthDiffer) * dCellRatio[nCol]));
			m_pGrd[nGridIndex]->SetItemFormat(nRow, nCol, nCenterAlignText);

			if (0 == nCol)
			{
				m_pGrd[nGridIndex]->SetItemBkColour(nRow, nCol, DEF_COLOR_BTN_MENU_NORMAL);
				m_pGrd[nGridIndex]->SetItemFgColour(nRow, nCol, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
			}
			else
			{
				m_pGrd[nGridIndex]->SetItemBkColour(nRow, nCol, WHITE_);
				m_pGrd[nGridIndex]->SetItemFgColour(nRow, nCol, BLACK_);
			}
		}

		if (0 < nRow)
		{
			m_pGrd[nGridIndex]->MergeCells(CCellRange(nRow, 1, nRow, _countof(dCellRatio) - 1));
		}
	}

	m_pGrd[nGridIndex]->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	m_pGrd[nGridIndex]->Refresh();

	UpdateGridInformation();
}

void CDlgManual::UpdateGridInformation()
{
	int	nGridIndex = eGRD_INFORMATION;
	CUniToChar csCnv;
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe(uvEng_JobRecipe_WhatLastSelectIsLocal());
	if (pstRecipe == nullptr) return;

	LPG_REAF pstRecipeExpo = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstRecipe->expo_recipe));


	int nMaxCol = 7;
	CString strArrMarkErr[6] = { _T("[ ￣ ]"),_T("[ ＿ ]"),_T("[│  ]"),_T("[  │]"),_T("[ ＼ ]"),_T("[ ／ ]") };
	for (int nCol = 1; nCol < nMaxCol; nCol++)
	{
		m_pGrd[nGridIndex]->SetItemText(EN_GRD_INFORMATION_ROW::MARK_DIST_ERR, nCol, strArrMarkErr[nCol - 1]);

		switch (uvEng_GetConfig()->mark_diff.result[nCol - 1].len_valid)	/* 0x00 : Not set or Reset, 0x01 : Set (Succ), 0x02 : Set (Fail) */
		{
		case 0x00:
			m_pGrd[nGridIndex]->SetItemBkColour(EN_GRD_INFORMATION_ROW::MARK_DIST_ERR, nCol, WHITE_);
			m_pGrd[nGridIndex]->SetItemFgColour(EN_GRD_INFORMATION_ROW::MARK_DIST_ERR, nCol, BLACK_);
			break;
		case 0x01:
			m_pGrd[nGridIndex]->SetItemBkColour(EN_GRD_INFORMATION_ROW::MARK_DIST_ERR, nCol, SEA_GREEN);
			m_pGrd[nGridIndex]->SetItemFgColour(EN_GRD_INFORMATION_ROW::MARK_DIST_ERR, nCol, WHITE_);
			break;
		case 0x02:
			m_pGrd[nGridIndex]->SetItemBkColour(EN_GRD_INFORMATION_ROW::MARK_DIST_ERR, nCol, TOMATO);
			m_pGrd[nGridIndex]->SetItemFgColour(EN_GRD_INFORMATION_ROW::MARK_DIST_ERR, nCol, WHITE_);
			break;
		default:
			break;
		}
	}

	/*Tack Time*/
	UINT64 u64JobTime = uvEng_GetJobWorkTime();
	m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_INFORMATION_ROW::TACT_TIME, 1, L"Last : %u m %02u s", uvCmn_GetTimeToType(u64JobTime, 0x01), uvCmn_GetTimeToType(u64JobTime, 0x02));

	/*Gerber Name*/
	LPG_RJAF pJob = uvEng_JobRecipe_GetSelectRecipe(uvEng_JobRecipe_WhatLastSelectIsLocal());
	m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_INFORMATION_ROW::GERBER_NAME, 1, L"%S", pJob->gerber_name);

	///*Roation*/
	//LPG_PPTP pstParams = &uvEng_ShMem_GetLuria()->panel.get_transformation_params;
	//m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_INFORMATION_ROW::REAL_ROTATION, 1, L"%d", pstParams->rotation);
	//if (pstParams->rotation == 0)
	//{
	//	m_pGrd[nGridIndex]->SetItemBkColour(EN_GRD_INFORMATION_ROW::REAL_ROTATION, 1, WHITE_);
	//	m_pGrd[nGridIndex]->SetItemFgColour(EN_GRD_INFORMATION_ROW::REAL_ROTATION, 1, BLACK_);
	//}
	//else
	//{
	//	m_pGrd[nGridIndex]->SetItemBkColour(EN_GRD_INFORMATION_ROW::REAL_ROTATION, 1, SEA_GREEN);
	//	m_pGrd[nGridIndex]->SetItemFgColour(EN_GRD_INFORMATION_ROW::REAL_ROTATION, 1, WHITE_);
	//}

	/*scale_xy*/
	LPG_PPTP pstParams = &uvEng_ShMem_GetLuria()->panel.get_transformation_params;
	m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_INFORMATION_ROW::REAL_SCALE, 1, L"ScaleX = %.5f  ScaleY = %.5f", pstParams->scale_xy[0] / 1000000.0f, pstParams->scale_xy[1] / 1000000.0f);
	if ((pstParams->scale_xy[0] + pstParams->scale_xy[1]) / 2 == 0)
	{
		m_pGrd[nGridIndex]->SetItemBkColour(EN_GRD_INFORMATION_ROW::REAL_SCALE, 1, WHITE_);
		m_pGrd[nGridIndex]->SetItemFgColour(EN_GRD_INFORMATION_ROW::REAL_SCALE, 1, BLACK_);
	}
	else
	{
		m_pGrd[nGridIndex]->SetItemBkColour(EN_GRD_INFORMATION_ROW::REAL_SCALE, 1, SEA_GREEN);
		m_pGrd[nGridIndex]->SetItemFgColour(EN_GRD_INFORMATION_ROW::REAL_SCALE, 1, WHITE_);
	}


	/*현재 측정 LDS 측정값에 장비 옵셋값 추가 하여 실제 소재 측정값 계산*/
	DOUBLE LimitZPos = pstRecipe->ldsThreshold / 1000.0f; //uvEng_GetConfig()->measure_flat.dLimitZPOS;

	auto& measureFlat = uvEng_GetConfig()->measure_flat;
	auto mean = measureFlat.GetThickMeasureMean();

	//DOUBLE LDSToThickOffset = 1.3;
	DOUBLE LDSToThickOffset = uvEng_GetConfig()->measure_flat.dOffsetZPOS;
	DOUBLE dmater = measureFlat.GetCheckedMaterialThick();
	DOUBLE RealThick = measureFlat.GetFinalThick();

	/*현재 측정 LDS 측정값에 장비 옵셋값 추가 하여 실제 소재 측정값 계산*/
	DOUBLE MaxZPos = LimitZPos; //uvEng_GetConfig()->measure_flat.dLimitZPOS;
	DOUBLE MinZPos = LimitZPos * -1.0f; //uvEng_GetConfig()->measure_flat.dLimitZPOS * -1;

	/*LDS Thick*/
	//m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_INFORMATION_ROW::REAL_THICK, 1, L"Real Thick :%.3f > LimitZ Pos : %.3f", RealThick, LimitZPos);
	m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_INFORMATION_ROW::REAL_THICK, 1, L"Real Thick :%.3f > Material Thick : %.3f + Limit : %.3f", RealThick, dmater, LimitZPos);
	/*LDS에서 측정한 값과 옵셋값 더한값이 Limit 범위*/
	if (mean == 0)
	{
		m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_INFORMATION_ROW::REAL_THICK, 1, L"Real Thick :0 > Material Thick : 0 + Limit : 0");
		m_pGrd[nGridIndex]->SetItemBkColour(EN_GRD_INFORMATION_ROW::REAL_THICK, 1, WHITE_);
		m_pGrd[nGridIndex]->SetItemFgColour(EN_GRD_INFORMATION_ROW::REAL_THICK, 1, BLACK_);
	}
	//else if ((RealThick > (MaxZPos + dmater)) || (RealThick < (MinZPos - dmater)))
	else if ((RealThick > (dmater + MaxZPos)) || (RealThick < (dmater + MinZPos)))
	{
		m_pGrd[nGridIndex]->SetItemBkColour(EN_GRD_INFORMATION_ROW::REAL_THICK, 1, TOMATO);
		m_pGrd[nGridIndex]->SetItemFgColour(EN_GRD_INFORMATION_ROW::REAL_THICK, 1, WHITE_);
	}
	else
	{
		m_pGrd[nGridIndex]->SetItemBkColour(EN_GRD_INFORMATION_ROW::REAL_THICK, 1, SEA_GREEN);
		m_pGrd[nGridIndex]->SetItemFgColour(EN_GRD_INFORMATION_ROW::REAL_THICK, 1, WHITE_);
	}



	m_pGrd[nGridIndex]->Refresh();
}


void CDlgManual::InitGridParameter()
{
	CRect rtStt;
	CRect rctSize;										/* 작업 영역	*/

	double	dCellRatio[2] = { 0.5, 0.5 };				/* 그리드 비율	*/
	int		nHeightSize = DEF_UI_GRD_ROW_OFFSET;	/* 셀 높이		*/
	int		nWidthDiffer = 1;						/* 셀 너비 오차값 */

	UINT nCenterAlignText = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;	/* 그리드 옵션 */

	// 그리드 기본 설정
	int	nGridIndex = eGRD_PARAMETER;

	m_pStt[EN_MANUAL_STT::PARAMETER]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	rctSize			= rtStt;
	rctSize.right	= rctSize.left + (rtStt.Width() / 3 * 2);
	rctSize.top		= rtStt.bottom + DEF_UI_OFFSET;
	rctSize.bottom	= rctSize.top + (nHeightSize * (int)EN_GRD_PARAMETER_ROW::_size()) + 1;

	if (NULL == m_pGrd[nGridIndex])
	{
		m_pGrd[nGridIndex] = new CGridCtrl();
		m_pGrd[nGridIndex]->Create(rctSize, this, DEF_UI_MANUAL_GRD + nGridIndex, (WS_CHILD | WS_TABSTOP | WS_VISIBLE));
	}

	m_pGrd[nGridIndex]->MoveWindow(rctSize);
	m_pGrd[nGridIndex]->SetColumnResize(FALSE);
	m_pGrd[nGridIndex]->SetRowResize(FALSE);
	m_pGrd[nGridIndex]->SetEditable(FALSE);
	m_pGrd[nGridIndex]->EnableSelection(FALSE);
	m_pGrd[nGridIndex]->ModifyStyle(WS_HSCROLL, 0);

	m_pGrd[nGridIndex]->SetGridLineColor(LIGHT_GRAY);
	m_pGrd[nGridIndex]->SetTextColor(BLACK_);
	m_pGrd[nGridIndex]->DeleteAllItems();

	// 그리드 영역이 작업 영역을 초과하면 스크롤 바 만큼의 오차값 저장
	if (rctSize.Height() - 1 < nHeightSize * (int)EN_GRD_PARAMETER_ROW::_size())
	{
		nWidthDiffer = ::GetSystemMetrics(SM_CXVSCROLL);
	}

	// 셀의 개수 지정
	m_pGrd[nGridIndex]->SetColumnCount(2);
	m_pGrd[nGridIndex]->SetRowCount((int)EN_GRD_PARAMETER_ROW::_size());

	CString strText;
	for (int nRow = 0; nRow < (int)EN_GRD_PARAMETER_ROW::_size(); nRow++)
	{
		m_pGrd[nGridIndex]->SetRowHeight(nRow, nHeightSize);
		m_pGrd[nGridIndex]->SetItemText(nRow, 0, (CString)EN_GRD_PARAMETER_ROW::_from_index(nRow)._to_string());

		for (int nCol = 0; nCol < 2; nCol++)
		{
			m_pGrd[nGridIndex]->SetColumnWidth(nCol, (int)((rctSize.Width() - nWidthDiffer) * dCellRatio[nCol]));
			m_pGrd[nGridIndex]->SetItemFormat(nRow, nCol, nCenterAlignText);

			if (0 == nCol)
			{
				m_pGrd[nGridIndex]->SetItemBkColour(nRow, nCol, DEF_COLOR_BTN_MENU_NORMAL);
				m_pGrd[nGridIndex]->SetItemFgColour(nRow, nCol, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
			}
			else
			{
				if (EN_GRD_PARAMETER_ROW::DUTY == nRow)
				{
					m_pGrd[nGridIndex]->SetItemBkColour(nRow, nCol, ALICE_BLUE);
				}
				else
				{
					m_pGrd[nGridIndex]->SetItemBkColour(nRow, nCol, WHITE_);
				}
				
				m_pGrd[nGridIndex]->SetItemFgColour(nRow, nCol, BLACK_);
			}
		}
	}

	m_pGrd[nGridIndex]->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	m_pGrd[nGridIndex]->Refresh();

	UpdateGridParameter();
}

void CDlgManual::UpdateGridParameter()
{
	int	nGridIndex = eGRD_PARAMETER;
	CUniToChar	clsCvt;
	LPG_REAF	pstExpo = uvEng_ExpoRecipe_GetRecipeOnlyName(clsCvt.Ansi2Uni(m_stJob.expo_recipe));
	LPG_LDEW	psLDEW = &uvEng_ShMem_GetLuria()->exposure;
	
	if (pstExpo)
	{
		m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_PARAMETER_ROW::DUTY, 1, _T("%d [%%]"), pstExpo->led_duty_cycle);
	}
	
	if (m_stJob.IsValid())
	{
		//m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_PARAMETER_ROW::SPEED, 1, _T("%.1f [%%]"), m_stJob.expo_speed * 100.0);
		m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_PARAMETER_ROW::SPEED, 1, _T("%.1f [%%]"), m_stJob.frame_rate / 10.0f);
		m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_PARAMETER_ROW::STEP, 1, _T("%d"), m_stJob.step_size);
		m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_PARAMETER_ROW::THICKNESS, 1, _T("%d [um]"), m_stJob.material_thick);
	}
	

	m_pGrd[nGridIndex]->Refresh();
}

void CDlgManual::OnGrdParameter(NMHDR* pNotifyStruct)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	if (1 > pItem->iRow)
	{
		return;
	}
	
	if (1 != pItem->iColumn)
	{
		return;
	}

	CGridCtrl* pGrid = m_pGrd[eGRD_PARAMETER];

	CString strOutput;
	CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);

	double	dMin = 0;
	double	dMax = 0;

	if (EN_GRD_PARAMETER_ROW::SPEED == pItem->iRow)
	{
		dMin = 0;
		dMax = 100;
		if (PopupKBDN(ENM_DITM::en_double, strInput, strOutput, dMin, dMax))
		{
			pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s [%%]"), strOutput);
		}
	}
	else if (EN_GRD_PARAMETER_ROW::STEP == pItem->iRow)
	{
		dMin = 1;
		dMax = 5;

		if (PopupKBDN(ENM_DITM::en_int8, strInput, strOutput, dMin, dMax))
		{
			pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
		}
	}
	else if (EN_GRD_PARAMETER_ROW::THICKNESS == pItem->iRow)
	{
		ST_RECIPE_PARAM stParam = CRecipeManager::GetInstance()->GetRecipe()->GetParam(EN_RECIPE_TAB::JOB, EN_RECIPE_JOB::MATERIAL_THICK);

		dMin = stParam.dMin;
		dMax = stParam.dMax;

		if (PopupKBDN(ENM_DITM::en_int16, strInput, strOutput, dMin, dMax))
		{
			pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s [um]"), strOutput);
		}
	}

	pGrid->Refresh();
}

VOID CDlgManual::CalcEnergy()
{
	TCHAR tzMesg[128] = { NULL };
	UINT8 i = 0, j = 0;

	if (FALSE == m_stJob.IsValid())
	{
		return;
	}

	CGridCtrl* pGrid = m_pGrd[eGRD_PARAMETER];
	CString str = pGrid->GetItemText(EN_GRD_PARAMETER_ROW::SPEED, 1);
	int nEndValue = str.Find('['); if (0 < nEndValue) { str = str.Left(nEndValue); }
	UINT16 u16FrameRate = (UINT16)ROUNDED(_ttof(str) * 10.0f, 0);
	DOUBLE dbTotal = 0.0f, dbPowerWatt[MAX_LED] = { NULL }, dbSpeed = 0.0f;

	str = pGrid->GetItemText(EN_GRD_PARAMETER_ROW::STEP, 1);
	nEndValue = str.Find('['); if (0 < nEndValue) { str = str.Left(nEndValue); }
	UINT8 u8Step = (UINT8)_ttof(str);

	str = pGrid->GetItemText(EN_GRD_PARAMETER_ROW::DUTY, 1);
	nEndValue = str.Find('['); if (0 < nEndValue) { str = str.Left(nEndValue); }
	UINT8 u8Duty = (UINT8)_ttof(str);
	UINT16 u16Rate = u16FrameRate;

	str = pGrid->GetItemText(EN_GRD_PARAMETER_ROW::THICKNESS, 1);
	nEndValue = str.Find('['); if (0 < nEndValue) { str = str.Left(nEndValue); }
	UINT16 u16Thickness = _ttoi(str);

	CUniToChar	clsCvt;
	LPG_REAF	pstExpo = uvEng_ExpoRecipe_GetRecipeOnlyName(clsCvt.Ansi2Uni(m_stJob.expo_recipe));

	if (NULL == pstExpo)
	{
		return;
	}

	LPG_PLPI	pstPowerI = uvEng_LedPower_GetLedPowerName(clsCvt.Ansi2Uni(pstExpo->power_name));

	if (NULL == pstPowerI)
	{
		return;
	}

	/* 광량 계산 */
	for (; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
	{
		for (j = 0; j < MAX_LED; j++)	dbPowerWatt[j] = pstPowerI->led_watt[i][j];
		dbTotal += uvCmn_Luria_GetExposeEnergy(u8Step, u8Duty, u16Rate, dbPowerWatt);
	}
	/* 노광 속도 */
	dbSpeed = uvCmn_Luria_GetExposeSpeed(u16FrameRate, u8Step);

	swprintf_s(tzMesg, 128, L"\nEnergy (%4.1f mJ)\nSpeed (%4.1f mm/s)", dbTotal / DOUBLE(i), dbSpeed);

	m_pBtn[EN_MANUAL_BTN::ENERGE]->SetWindowTextW(tzMesg);

	//m_stJob.expo_speed = (float)u16Rate / 1000.0f;
	m_stJob.expo_energy = (float)(dbTotal / DOUBLE(i));
	m_stJob.step_size = u8Step;
	m_stJob.material_thick = u16Thickness;
	m_stJob.frame_rate = u16FrameRate;

	uvEng_JobRecipe_RecipeModify(&m_stJob);
	UpdateGridParameter();
}


/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgManual::OnBtnClick(UINT32 id)
{
	CUniToChar csCnv;
	bool isLocalSelRecipe = uvEng_JobRecipe_WhatLastSelectIsLocal();
	LPG_RJAF pstJob = uvEng_JobRecipe_GetSelectRecipe(isLocalSelRecipe);
	LPG_RAAF pstAlign = uvEng_Mark_GetAlignRecipeName(csCnv.Ansi2Uni(pstJob->align_recipe));
	

	CString strMsg;
	UINT nID = id - DEF_UI_MANUAL_BTN;

	/*자동화 동작시 AlignExpo 동작 비활성*/
	if ((g_u8Romote == en_menu_phil_mode_auto) && (nID == EN_MANUAL_BTN::ALIGN_EXPOSE))
	{
		CDlgMesg dlgMesg;
		dlgMesg.MyDoModal(L"Align Expose not exected in auto mode", 0x01);
		return;
	}

	switch (nID)
	{
	case EN_MANUAL_BTN::USE_AF:
	{
		if (m_pDlgMain->IsBusyWorkJob())
		{
			CDlgMesg dlgMesg;
			dlgMesg.MyDoModal(L"cannnot change af method, on working.", 0x01);
			return;
		}

		GlobalVariables::GetInstance()->GetAutofocus().SetUseAF(!GlobalVariables::GetInstance()->GetAutofocus().GetUseAF());
		
	}
	break;

	case EN_MANUAL_BTN::DIRECT_EXPOSE:
		ExposeDirect();
		break;
	case EN_MANUAL_BTN::MARK:
		MoveCheckMarkNo();
		break;
	case EN_MANUAL_BTN::ALIGN_TEST:
	{
		if (!pstJob || !pstAlign)
		{
			CDlgMesg dlgMesg;
			dlgMesg.MyDoModal(L"The recipe (mark or gerber) does not exist", 0x01);
			return;
		}
		/* 반드시 2개 검색해야 하므로... 검색 개수 설정 */
		uvEng_Camera_SetMarkMethod(ENG_MMSM(pstAlign->search_type), pstAlign->search_count);
		TestAlign();
	}
		break;
	case EN_MANUAL_BTN::ALIGN_EXPOSE:
	{
		if (!pstJob || !pstAlign)
		{
			CDlgMesg dlgMesg;
			dlgMesg.MyDoModal(L"The recipe (mark or gerber) does not exist", 0x01);
			return;
		}
		/* 반드시 2개 검색해야 하므로... 검색 개수 설정 */
		uvEng_Camera_SetMarkMethod(ENG_MMSM(pstAlign->search_type), pstAlign->search_count);
		ExposeAlign();
	}
		break;
	case EN_MANUAL_BTN::RECIPE_LOAD:
		
		RecipeLoad();
		
		break;
	case EN_MANUAL_BTN::RECIPE_UNLOAD:
		RecipeUnload();
		break;
	case EN_MANUAL_BTN::STOP:
		m_pDlgMain->WorkStop();
		break;
	case EN_MANUAL_BTN::CALIBRATION_MARK:
	{
		MakeMarkOffsetField();
	}
	break;

	case EN_MANUAL_BTN::MARK_ZERO:
	{
		MarkZero();
	}
		break;
	case EN_MANUAL_BTN::ENERGE:
		CalcEnergy();
		break;
	case EN_MANUAL_BTN::LOAD:
		OnlyFEMLoad();
		break;
	case EN_MANUAL_BTN::ERROR_THICK:
		ErrorThick();
		break;
	case EN_MANUAL_BTN::CHANGE_ALIGN_MODE:
		ChangeAlignMode();
	break;

	}
}



VOID CDlgManual::MakeMarkOffsetField()
{
	CDlgParam dlg;
	DLG_PARAM stParam;
	VCT_DLG_PARAM stVctParam;
	CString strName;
	CDPoint dpStartPos;
	vector<LPG_XMXY> marks;
	const int MARK_PAIR = 2;
	LPG_CIEA cfg = uvEng_GetConfig();

	auto grabFindFunc = [&](int camIdx, int markIdx,bool isGlobal, CAtlList <LPG_ACGR>* grabPtr , LPG_XMXY storedVal ) -> LPG_XMXY
	{
		if (grabPtr == NULL || grabPtr->GetCount() == 0)
			return nullptr;

		for (int i = 0; i < grabPtr->GetCount(); i++)
		{
			auto reserveFlag = isGlobal ? STG_XMXY_RESERVE_FLAG::GLOBAL : STG_XMXY_RESERVE_FLAG::LOCAL;
			auto grab = grabPtr->GetAt(grabPtr->FindIndex(i));
			if (grab != nullptr && grab->cam_id == camIdx && grab->fiducialMarkIndex == markIdx && (grab->reserve & reserveFlag) == reserveFlag)
			{
				storedVal->mark_x = grab->move_mm_x;
				storedVal->mark_y = grab->move_mm_y;
				storedVal->reserve = grab->reserve;

				storedVal->tgt_id = grab->fiducialMarkIndex;
				return storedVal;
			}
		}
		return nullptr;
	};

	auto uiWorks = [&](std::vector<LPG_XMXY> values, ENG_AMOS motionType)
	{
		stVctParam.clear();
		int valueCount = values.size();
		CString temp;

		vector<double> valuesCpy;
		
		int localCnt = 0;
		int globalCnt = 0;

		if (cfg->set_align.markOffsetPtr == nullptr) return;

		for (int i = 0; i < valueCount; i++)
		{
			for (int j = 0; j < MARK_PAIR; j++)
			{

				const TCHAR* flag = values[i]->GetFlag(STG_XMXY_RESERVE_FLAG::GLOBAL) ? _T("G") : _T("L");
				const TCHAR* axis = (j == 0 ? _T("x") : _T("y"));
				temp.Format(_T("%s_Mark%d offset %s"), flag, i + 1, axis);

				//temp.Format(_T("%s_Mark%d offset %s"), values[i]->GetFlag(STG_XMXY_RESERVE_FLAG::GLOBAL) ? "G" : "L", i + 1, (j == 0 ? "x" : "y"));
				stParam.Init();
				stParam.strName = temp;
				stParam.strValue = CStringA(values[i] == nullptr ? "" :  std::to_string( j==0 ? values[i]->mark_x : values[i]->mark_y).c_str());
				stParam.strUnit = _T("um");
				stParam.enFormat = ENM_DITM::en_double;
				stParam.u8DecPts = 4;
				stVctParam.push_back(stParam);
			}
			
		}

		if (IDOK == dlg.MyDoModal(stVctParam))
		{
			for(int i=0;i<stVctParam.size();i++)
				valuesCpy.push_back(_ttof(stVctParam[i].strValue));

			temp.Format(_T("%s method expo offset Save?"), motionType == ENG_AMOS::en_static_3cam ? _T("Static") : _T("OnTheFly"));
			if (MessageBoxEx(nullptr, temp, _T("notice"), MB_YESNO | MB_ICONINFORMATION, LANG_ENGLISH) == IDYES)
			{
				cfg->set_align.markOffsetPtr->Clear(motionType);

				for (int i = 0; i < valueCount; i++)
					cfg->set_align.markOffsetPtr->Push(motionType, values[i]->GetFlag(STG_XMXY_RESERVE_FLAG::GLOBAL), std::make_tuple(values[i]->tgt_id, valuesCpy[(i * MARK_PAIR)], valuesCpy[(i * MARK_PAIR) + 1])); 
			
				uvEng_SaveConfig();
			}
		}

		for (int i = 0; i < valueCount; i++)
		{
			delete values[i]; values[i] = nullptr;
		}
	};

	AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();
	
	int localCnt = motions.status.localMarkCnt;
	int globalCnt = motions.status.globalMarkCnt;
	auto motionType = motions.markParams.alignMotion;

	try
	{

		CAtlList <LPG_ACGR>* grabMark = uvEng_Camera_GetGrabbedMarkAll();

		auto markCount = grabMark->GetCount();
		
		const int CAM1 = 1; const int CAM2 = 2;
		
		switch (motions.markParams.alignMotion)
		{
		case ENG_AMOS::en_onthefly_2cam:
		{
			//marks = vector<LPG_XMXY>{ grabFindFunc(CAM1, 0,true, grabMark,new STG_XMXY()), 
			//					grabFindFunc(CAM1, 1,true,grabMark, new STG_XMXY()),
			//					grabFindFunc(CAM2, 2,true,grabMark, new STG_XMXY()),
			//					grabFindFunc(CAM2, 3,true,grabMark, new STG_XMXY()) }; //이건 고정이니깐.

			//if (motions.status.localMarkCnt == 0)
			//	break;

			auto cam1FidPool = motions.GetFiducialPool(CAM1);
			auto cam2FidPool = motions.GetFiducialPool(CAM2);

			for_each(cam1FidPool.begin(), cam1FidPool.end(), [&](const STG_XMXY& v) {marks.push_back(grabFindFunc(CAM1, v.tgt_id, ((v.reserve & STG_XMXY_RESERVE_FLAG::GLOBAL) != 0 ? true : false), grabMark, new STG_XMXY())); });
			for_each(cam2FidPool.begin(), cam2FidPool.end(), [&](const STG_XMXY& v) {marks.push_back(grabFindFunc(CAM2, v.tgt_id, ((v.reserve & STG_XMXY_RESERVE_FLAG::GLOBAL) != 0 ? true : false), grabMark, new STG_XMXY())); });
		}
		break;

		case ENG_AMOS::en_static_3cam:
		{
			int centerCam = motions.markParams.centerCamIdx;

			auto fidPool = motions.GetFiducialPool(centerCam);

			/*marks = vector<LPG_XMXY>{ grabFindFunc(centerCam, 0,true,grabMark, new STG_XMXY()),
						   grabFindFunc(centerCam, 1,true,grabMark,new STG_XMXY()),
						   grabFindFunc(centerCam, 2,true,grabMark,new STG_XMXY()),
						   grabFindFunc(centerCam, 3,true,grabMark,new STG_XMXY()) };*/

			for_each(fidPool.begin(), fidPool.end(), [&](const STG_XMXY& v)
				{
					marks.push_back(grabFindFunc(centerCam, v.tgt_id, ((v.reserve & STG_XMXY_RESERVE_FLAG::GLOBAL) != 0 ? true : false), grabMark, new STG_XMXY()));
				});
		}
		break;

		default:
		{
			throw exception("not implement.");
		}
		break;
		}
	}
	catch (const std::exception&)
	{
		return;
	}
	
	auto res = std::find_if(marks.begin(), marks.end(), [](LPG_XMXY v) {return v == nullptr; });
	if (marks.empty() || res != marks.end()) return;
	
	uiWorks(marks,motionType);
		
}


void CDlgManual::OnGrdClick(UINT ID, NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	int nCommand = ID - DEF_UI_MANUAL_GRD;

	switch (nCommand)
	{
	case eGRD_INFORMATION:
		break;
	case eGRD_PARAMETER:
		OnGrdParameter(pNotifyStruct);
		break;
	default:
		break;
	}
}


VOID CDlgManual::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_pDrawPrev)
	{
		CRect rtDlg;
		CRect rtPreview;
		CRect rtPreviewOrg;

		GetWindowRect(rtDlg);
		GetDlgItem(IDC_MANUAL_PIC_PREVIEW)->GetWindowRect(rtPreview);

		rtPreviewOrg = rtPreview;
		rtPreview.OffsetRect(-rtDlg.left, -rtDlg.top);

		if (rtPreview.PtInRect(point))
		{
			m_pDrawPrev->OnMouseDblClick();
		}
	}
}


VOID CDlgManual::OnLButtonDown(UINT32 nFlags, CPoint point)
{
	if (m_pDrawPrev)
	{
		CRect rtDlg;
		CRect rtPreview;
		CRect rtPreviewOrg;

		GetWindowRect(rtDlg);
		GetDlgItem(IDC_MANUAL_PIC_PREVIEW)->GetWindowRect(rtPreview);

		rtPreviewOrg = rtPreview;
		rtPreview.OffsetRect(-rtDlg.left, -rtDlg.top);

		if (rtPreview.PtInRect(point))
		{
			m_pDrawPrev->OnMouseClick(point.x - rtPreview.left, point.y - rtPreview.top, rtPreview);
			bool isLocalSelRecipe = uvEng_JobRecipe_WhatLastSelectIsLocal();
			LPG_RJAF jobRcp = uvEng_JobRecipe_GetSelectRecipe(isLocalSelRecipe);
			m_pDrawPrev->DrawMem(jobRcp);
			m_pDrawPrev->Draw();

			if (0 <= m_pDrawPrev->GetSelectGlobalMark())
			{
				CString strText;
				strText.Format(_T("SELECT GLOBAL MARK %d]"), m_pDrawPrev->GetSelectGlobalMark());
				m_pdlgMarkShow->SetMarkType(FALSE); // global
				m_pdlgMarkShow->SetMarkIndex(m_pDrawPrev->GetSelectGlobalMark());
				m_pdlgMarkShow->ShowWindow(SW_HIDE);
				m_pdlgMarkShow->ShowWindow(SW_SHOW);
			}
			else if (0 <= m_pDrawPrev->GetSelectLocalMark())
			{
				CString strText;
				strText.Format(_T("SELECT LOCAL MARK %d"), m_pDrawPrev->GetSelectLocalMark());
				m_pdlgMarkShow->SetMarkType(TRUE); // local
				m_pdlgMarkShow->SetMarkIndex(m_pDrawPrev->GetSelectLocalMark());
				m_pdlgMarkShow->ShowWindow(SW_HIDE);
				m_pdlgMarkShow->ShowWindow(SW_SHOW);
			}
			else
			{
				// 선택한 마크 없음
			}
		}
	}

	CDlgMenu::OnLButtonDown(nFlags, point);
}

/*
 desc : 마크 출력될 페이지 번호
 parm : mode	- [in]  0x00 : 이전 페이지 보기
						0x01 : 다음 페이지 보기
 retn : None
*/
VOID CDlgManual::RedrawMarkPage(UINT8 mode)
{
	
}

/*
 desc : 현재 선택된 Recipe 적재 진행
 parm : None
 retn : None
*/
VOID CDlgManual::RecipeLoad()
{
	UINT8 u8Offset = 0xff;
	USES_CONVERSION;
	if (uvEng_GetConfig()->set_comn.run_develop_mode)
	{
		CDlgStep dlgStep;
		if (IDOK != dlgStep.MyDoModal())	return;
		u8Offset = dlgStep.GetOffset();
	}


	bool isLocalSelect = uvEng_JobRecipe_WhatLastSelectIsLocal();
	bool sameRecipe = false;
	LPG_RJAF localJob = uvEng_JobRecipe_GetSelectRecipe(true);
	LPG_RJAF hostJob = uvEng_JobRecipe_GetSelectRecipe(false);
	CDlgMesg dlgMesg;

	if (localJob && hostJob)
	{
		sameRecipe = strcmp(localJob->job_name, hostJob->job_name) == 0;
	}

	if (sameRecipe == false)
	{
		
		TCHAR temp[255] = { 0, };
		wsprintf(temp,_T("<<%s>> : <%s> selected. Load Recipe?"), isLocalSelect ? _T("LocalRecipe") : _T("HostRecipe"), isLocalSelect ? A2W(localJob->job_name) : A2W(hostJob->job_name));

		if (dlgMesg.MyDoModal(temp) != IDOK)
			return;
	}



	m_pDlgMain->RunWorkJob(ENG_BWOK::en_local_gerb_load, PUINT64(&u8Offset));
}

/*
 desc : 기존 적재된 Recipe 해제
 parm : None
 retn : None
*/
VOID CDlgManual::RecipeUnload()
{
	m_pDlgMain->RunWorkJob(ENG_BWOK::en_gerb_unload);
}

/*
 설명 : Transformation Information 옵션 설정 확인
 변수 : None
 반환 : TRUE or FALSE
*/
BOOL CDlgManual::IsCheckTransRecipe()
{
	TCHAR tzMesg[256]	= {NULL}, tzYesNo[2][8]	= { L"No", L"Yes" };
	LPG_CGTI pstTrans	= NULL;
	CDlgMesg dlgMesg;

	/* Check the Global Transformation Information */
	pstTrans	= &uvEng_GetConfig()->global_trans;
	swprintf_s(tzMesg, 256, L"Apply global transformation recipe values ?\n"
							L"Rect.Lock(%s), Rotation(%s), Scaling(%s), Offset(%s)",
			   tzYesNo[pstTrans->use_rectangle], tzYesNo[pstTrans->use_rotation_mode],
			   tzYesNo[pstTrans->use_scaling_mode], tzYesNo[pstTrans->use_offset_mode]);
	if (dlgMesg.MyDoModal(tzMesg, 0x00) != IDOK)	return FALSE;

	return TRUE;
}

/*
 desc : 직접 노광 진행 (Align Mark 검사 없이)
 parm : None
 retn : None
*/
VOID CDlgManual::ExposeDirect()
{
	BOOL bPreHeated	= FALSE;
	//STG_CPHE stExpo	= {NULL};
	STG_CELA stExpo = { NULL };
	stExpo.Init();
	CDlgMesg dlgMesg;

	if (IDOK != dlgMesg.MyDoModal(L"Is Direct Exposure Working right ?"))	return;
#if 1
	if (uvEng_GetConfig()->set_comn.run_develop_mode)
#endif
	{
		CDlgRept dlgRept;
		if (IDOK != dlgRept.MyDoModal())	return;
		stExpo.expo_count	= dlgRept.GetExpoCount();
		stExpo.move_acam	= dlgRept.IsMovingACam() ? 0x01 : 0x00;
		stExpo.move_ph		= dlgRept.IsMovingPH() ? 0x01 : 0x00;
	}

	/* Transformation Recipe Information 설정 확인 */
	if (uvEng_GetConfig()->global_trans.use_trans && !IsCheckTransRecipe())	return;
	/* Vacuum과 Shutter를 동시 제어 수행 */
	

	m_pDlgMain->RunWorkJob(ENG_BWOK::en_expo_only, PUINT64(&stExpo));
}

/*
 desc : 얼라인 노광 진행 (Align Mark 검사 진행)
 parm : None
 retn : None
*/
VOID CDlgManual::ExposeAlign()
{
	CDlgMesg dlgMesg;
	STG_CELA stExpo = { NULL };
	stExpo.Init();

	if (IDOK != dlgMesg.MyDoModal(L"Is Align Exposure Working right ?"))	return;

	if (!uvEng_Luria_IsMarkGlobal())
	{
		dlgMesg.MyDoModal(L"There is no globalmark information", 0x01);
		return;
	}
	/* Transformation Recipe Information 설정 확인 */
	if (uvEng_GetConfig()->global_trans.use_trans && !IsCheckTransRecipe())	return;

	//m_pDlgMain->RunWorkJob(ENG_BWOK::en_expo_align, PUINT64(&stExpo));
	m_pDlgMain->RunWorkJob(ENG_BWOK::en_expo_align);
}

/*
 desc : 테스트 얼라인 노광 진행 (Align Mark 검사 진행)
 parm : None
 retn : None
*/
VOID CDlgManual::TestAlign()
{
	CDlgMesg dlgMesg;
	if (!uvEng_Luria_IsMarkGlobal())
	{
		dlgMesg.MyDoModal(L"There is no globalmark information", 0x01);
		return;
	}

	STG_CELA stExpo = { NULL };
	stExpo.Init();
	if (IDOK != dlgMesg.MyDoModal(L"Is Test Align Mark Working right ?"))	return;
#if 1
	if (uvEng_GetConfig()->set_comn.run_develop_mode)
#endif
	{
		CDlgRept dlgRept;
		if (IDOK != dlgRept.MyDoModal())	return;
		stExpo.expo_count = dlgRept.GetExpoCount();
		stExpo.move_acam = dlgRept.IsMovingACam() ? 0x01 : 0x00;
		stExpo.move_ph = dlgRept.IsMovingPH() ? 0x01 : 0x00;
	}

	/* Transformation Recipe Information 설정 확인 */
	if (uvEng_GetConfig()->global_trans.use_trans && !IsCheckTransRecipe())	return;

	m_pDlgMain->RunWorkJob(ENG_BWOK::en_mark_test, PUINT64(&stExpo));
}

/*
 desc : 현재 선택된 Mark Number 위치로 Camera X 축 및 Stage X / Y 축 이동하기
 parm : None
 retn : None
*/
VOID CDlgManual::MoveCheckMarkNo()
{
	/* Vacuum과 Shutter를 동시 제어 수행 */
	// by sysandj : MCQ대체 추가 필요

	if (m_pDrawPrev)
	{
		UINT8			u8MarkNo = m_pDrawPrev->GetSelectGlobalMark();
		if (u8MarkNo)	m_pDlgMain->RunWorkJob(ENG_BWOK::en_mark_move, (PUINT64) & (u8MarkNo));
		else			m_pDlgMain->RunWorkJob(ENG_BWOK::en_mark_test);
	}
}

/*
 desc : 초기화
 parm : None
 retn : None
*/
VOID CDlgManual::ResetData()
{
	/* 각종 에러 초기화 */
	uvEng_ShMem_GetLuria()->link.ResetData();
	uvEng_ResetJobWorkInfo();
	/* Expose 관련 수치 초기화 */
	//m_pExpoVal->ResetData();
#if 1
	/* Luria HW Inited (추후 안정화 (배포) 버전에서는 아래 코드 제거) */
	uvEng_Luria_ReqSetHWInit();
#endif

	Invalidate(TRUE);
}

/*
 desc : Focus, Emergy만 따로 세팅 작업 진행
 parm : None
 retn : None
*/
VOID CDlgManual::OnlyFEMLoad()
{
	/*포커스, 에너지만 다시 세팅*/
	m_pDlgMain->RunWorkJob(ENG_BWOK::en_gerb_onlyfem);

	/*FEM Expose 노광 동작*/
	//m_pDlgMain->RunWorkJob(ENG_BWOK::en_gerb_expofem);
}

void CDlgManual::ChangeAlignMode()
{

	if (m_pDlgMain->IsBusyWorkJob())
	{
		MessageBoxEx(nullptr, _T("cannnot change align method, on working."), _T(""), MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		return;
	}
	bool isLocalSelRecipe = uvEng_JobRecipe_WhatLastSelectIsLocal();
	LPG_RJAF job = uvEng_JobRecipe_GetSelectRecipe(isLocalSelRecipe);
	AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();
	auto motionType = motions.markParams.alignMotion;

	if (job == nullptr || motionType == ENG_AMOS::none)
	{
		MessageBoxEx(nullptr, _T("no recipe loaded."), _T(""), MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		return;
	}
	
	std::stringstream combine;
	//combine << "current align method is\n" << (((int)motionType & 0b01) != 0 ? "<On The Fly>" : "<Static>") << "\n" << "change to\n" << (((int)motionType & 0b01) != 0 ? "<Static>?" : "<On The Fly>?");
	combine << "current align method is\n" << (((int)motionType & 0b01) != 0 ? "<Normal Align>" : "<Refind Align>") << "\n" << "change to\n" << (((int)motionType & 0b01) != 0 ? "<Refind Align>?" : "<Normal Align>?");

	USES_CONVERSION;
	if (MessageBoxEx(nullptr, A2T(combine.str().c_str()), _T(""), MB_OKCANCEL | MB_ICONINFORMATION, LANG_ENGLISH) == IDOK)
	{
		motions.markParams.alignMotion = ((int)motionType & 0b01) != 0 ? ENG_AMOS::en_static_3cam : ENG_AMOS::en_onthefly_2cam; //!! 레퍼런스에서 수정한것임 !!
		//부수적인 작업은 진행해야함. 레시피 변경할때 얼라인 타입에 따라 준비되는 데이터가 따로 있음.
	}
}

/*
 desc : Direct Expose 후 Align Mark 인식 후 마크 이동 값 제로값으로 수정
 parm : None
 retn : None
*/
BOOL CDlgManual::MarkZero()
{
	LPG_ACGR pstGrab[4];
	LPG_ACGR pstMark = NULL;
	bool isLocalSelRecipe = uvEng_JobRecipe_WhatLastSelectIsLocal();
	LPG_RJAF pJob = uvEng_JobRecipe_GetSelectRecipe(isLocalSelRecipe);
	LPG_MACP pstThickCali = uvEng_ThickCali_GetRecipe(pJob->cali_thick);
	pstThickCali->mark2_stage_y[0];
	pstThickCali->mark2_stage_y[1];
	pstThickCali->mark2_acam_x[0];
	pstThickCali->mark2_acam_x[1];

	pstGrab[0] = uvEng_Camera_GetGrabbedMark(0x01, 0x00);
	pstGrab[1] = uvEng_Camera_GetGrabbedMark(0x01, 0x01);
	pstGrab[2] = uvEng_Camera_GetGrabbedMark(0x02, 0x00);
	pstGrab[3] = uvEng_Camera_GetGrabbedMark(0x02, 0x01);



	//int x, y;
	TCHAR tzMesg[128] = { NULL };
	DOUBLE derrorX[2], derrorY[2];



	/* 현재 거버가 적재되어 있는지 확인 */
	if (pstGrab[0] == nullptr || pstGrab[1] == nullptr || pstGrab[2] == nullptr || pstGrab[3] == nullptr)
	{
		AfxMessageBox(L"make zero calibration need all marks are grab first.", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}


	/* 현재 거버가 적재되어 있는지 확인 */
	if (!uvCmn_Luria_IsJobNameLoaded())
	{
		AfxMessageBox(L"The gerber file did not complete loading", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	/* 현재 Loading 되어 있는 거버에 Mark가 포함되어 있는지 확인 */
	if (uvEng_Luria_GetMarkCount(ENG_AMTF::en_global) < 1)
	{
		AfxMessageBox(L"The global mark does not exist", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}

	/*Grab Image 갯수가 4개 이상일때 */
	if (uvEng_Camera_GetGrabbedCount() < 4)
	{
		AfxMessageBox(L"Is.Grabbed.Image.Count fail", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	UINT16 u16Grab = uvEng_Camera_GetGrabbedCount();
	/*Grab Image에서 Move 오차값 평균 계산*/
	derrorX[0] = (pstGrab[0]->move_mm_x + pstGrab[1]->move_mm_x) / 2.0f;
	derrorX[1] = (pstGrab[2]->move_mm_x + pstGrab[3]->move_mm_x) / 2.0f;
	derrorY[0] = (pstGrab[0]->move_mm_y + pstGrab[1]->move_mm_y) / 2.0f;
	derrorY[1] = (pstGrab[2]->move_mm_y + pstGrab[3]->move_mm_y) / 2.0f;


	//for (y = 1; y < 3; y++)
	//{
	//	derrorX = 0;
	//	derrorY = 0;
	//	for (x = 0; x < 2; x++)
	//	{
	//		/*Grab된 이미지 결과 데이터 가져오기*/
	//		//pstMark = uvEng_Camera_GetGrabbedMark(0x01, 0x00);
	//		pstMark = uvEng_Camera_GetGrabbedMark(y, x);
	//		if (pstMark)
	//		{
	//			derrorX = derrorX + pstMark->move_mm_x;
	//			derrorY = derrorY + pstMark->move_mm_y;
	//		}
	//	}
	//
	//	if (derrorX > 0 && derrorY > 0)
	//	{
	//		/*Cam 하나의 누적 오차값의 평균값을 구하여 캘리브레이션 값 수정*/
	//		pstThickCali->mark2_stage_y[y - 1] = pstThickCali->mark2_stage_y[y - 1] - (derrorY / 2.0);
	//		pstThickCali->mark2_acam_x[y - 1] = pstThickCali->mark2_acam_x[y - 1] - (derrorX / 2.0);
	//
	//
	//		swprintf_s(tzMesg, 128,
	//			L"derrorX[%d] = %.4f derrorY[%d] = %.4f", y - 1, derrorX, y - 1, derrorY);
	//		LOG_MESG(ENG_EDIC::en_podis, tzMesg);
	//	}
	//}


	if (IDYES == AfxMessageBox(L"Thick_cail.dat 문서에서 Cam 위치값을 수정 하시겠습니까?", MB_YESNO))
	{
		pstThickCali->mark2_acam_x[0] = pstThickCali->mark2_acam_x[0] - derrorX[0];
		pstThickCali->mark2_acam_x[1] = pstThickCali->mark2_acam_x[1] - derrorX[1];
		pstThickCali->mark2_stage_y[0] = pstThickCali->mark2_stage_y[0] - derrorY[0];
		pstThickCali->mark2_stage_y[1] = pstThickCali->mark2_stage_y[1] + derrorY[1];

		/*수정한 수정값 파일로 저장*/
		uvEng_ThickCali_RecipeModify(pstThickCali);
	}



	return TRUE;
}


/*
 desc : 얼라인 동작 에서 LDS 센서로 Thick 측정 후 에러 발생시 조치 사항
 parm : None
 retn : None
*/
VOID CDlgManual::ErrorThick()
{
	if (m_pDlgMain->IsBusyWorkJob())
	{
		MessageBoxEx(nullptr, _T("cannnot error thcik method, on working."), _T(""), MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		return;
	}

	bool isLocalSelRecipe = uvEng_JobRecipe_WhatLastSelectIsLocal();
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe(isLocalSelRecipe);

	AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();
	auto motionType = motions.markParams.alignMotion;

	/*Recipe Select 및 Load 확인*/
	if (pstRecipe == nullptr || motionType == ENG_AMOS::none)
	{
		MessageBoxEx(nullptr, _T("no recipe loaded."), _T(""), MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		return;
	}

	/*Thick check 동작 되었는지 확인*/
	auto& measureFlat = uvEng_GetConfig()->measure_flat;
	auto mean = measureFlat.GetThickMeasureMean();
	if (mean == 0)
	{
		MessageBoxEx(nullptr, _T("no check thick move"), _T(""), MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		return;
	}

	if (measureFlat.u8UseThickCheck)
	{
		//bool isLocalSelRecipe = uvEng_JobRecipe_WhatLastSelectIsLocal();
		//LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe(isLocalSelRecipe);
		DOUBLE dmater = measureFlat.GetCheckedMaterialThick(); //pstRecipe->material_thick / 1000.0f;
		//DOUBLE LDSToThickOffset = uvEng_GetConfig()->measure_flat.dOffsetZPOS;

		DOUBLE RealThick = measureFlat.GetFinalThick();

		DOUBLE LimitZPos = m_stJob.ldsThreshold / 1000.0f; //uvEng_GetConfig()->measure_flat.dLimitZPOS;
		DOUBLE MaxZPos = LimitZPos;    //uvEng_GetConfig()->measure_flat.dLimitZPOS;
		DOUBLE MinZPos = LimitZPos * -1.0f;   //uvEng_GetConfig()->measure_flat.dLimitZPOS * -1;

		TCHAR tzMsg[256] = { NULL };
		swprintf_s(tzMsg, 256, L"Real Thick :%.3f > Material Thick : %.3f + Limit : %.3f", RealThick, dmater, LimitZPos);
		LOG_SAVED(ENG_EDIC::en_uvdi15, ENG_LNWE::en_job_work, tzMsg);


		/*LDS에서 측정한 값과 옵셋값 더한값이 Limit 범위*/
		if ((RealThick > (dmater + MaxZPos)) || (RealThick < (dmater + MinZPos)))
		{
			swprintf_s(tzMsg, 256, L"Failed to actual material thickness tolerance range\n [Real Thick :%.3f > Material Thick : %.3f + Limit : %.3f]", RealThick, dmater, LimitZPos);

			/*측정한 Thick 수치와 Recipe 수치가 다를경우 측정한 값으로 수정하여 재로드 할지 확인*/
			CDlgMesg dlgMesg;
			UINT8  result = dlgMesg.MyDoModal(tzMsg, 0x03);
			if (IDOK == result)
			{
				//pstRecipe->material_thick = RealThick * 1000.0f;
				m_stJob.material_thick = RealThick * 1000.0f;
				if (uvEng_Philhmi_IsConnected() && uvEng_JobRecipe_RecipeModify(&m_stJob))
				{
					/*포커스, 에너지만 다시 세팅*/
					m_pDlgMain->RunWorkJob(ENG_BWOK::en_gerb_onlyfem);

					/*초기화*/
					GlobalVariables::GetInstance()->GetAlignMotion().markParams.workErrorType = ENG_WETE::en_none;
					uvEng_GetConfig()->measure_flat.MeasurePoolClear();

					UpdateGridInformation();
					UpdateGridParameter();
					CRecipeManager::GetInstance()->PhilSendModifyRecipe(pstRecipe);
				}
			}
			else if (IDIGNORE == result)
			{
				/*이번 소재만 LDS Check OFF*/
				uvEng_GetConfig()->measure_flat.bOnePass = TRUE;
				uvEng_GetConfig()->measure_flat.u8UseThickCheck = FALSE;
				//uvEng_SaveConfig();
			}

			/*얼라인 마크 인식 초기화*/
			motions.SetAlignComplete(true);
		}
		else
		{
			swprintf_s(tzMsg, 256, L"Actual material thickness tolerance range\n [Real Thick :%.3f > Material Thick : %.3f + Limit : %.3f]", RealThick, dmater, LimitZPos);

			CDlgMesg dlgMesg;
			if (IDOK != dlgMesg.MyDoModal(tzMsg, 0x01));
		}
	}
	else
	{
		MessageBoxEx(nullptr, _T("no use thikc check."), _T(""), MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		return;
	}
}


/*
 desc : 마크 검사 결과를 이미지와 측정 결과 (오차) 값 출력 (갱신)
 parm : None
 retn : None
*/
VOID CDlgManual::DrawMarkData(bool drawForce)
{
	if (0 != m_u64TickCount % 20 && drawForce == false)
	{
		return;
	}


	bool dataReady = GlobalVariables::GetInstance()->GetAlignMotion().IsDataReady();
	bool isLocalSelRecipe = uvEng_JobRecipe_WhatLastSelectIsLocal();
	LPG_RJAF pstJob = uvEng_JobRecipe_GetSelectRecipe(isLocalSelRecipe);
	if (m_pdlgMarkShow == nullptr || pstJob == nullptr || dataReady == false)
		return;

	std::vector < STG_XMXY > vMark;
	m_pDrawPrev->GetGlobalMark(vMark);
	for (const auto& mark : vMark)
	{
		LPG_ACGR pstMark = m_pdlgMarkShow->GetMarkInfo(mark.tgt_id, FALSE);

		if (pstMark)
		{
			STG_MARK stMark;
			stMark.stMark = mark;
			stMark.stInfo.dOffsetX = pstMark->cent_dist_x;
			stMark.stInfo.dOffsetY = pstMark->cent_dist_y;
			stMark.stInfo.dScore = pstMark->score_rate;
			stMark.stInfo.nResult = pstMark->marked;

			m_pDrawPrev->SetGlobalMarkResult(stMark.stMark.tgt_id, stMark.stInfo);
		}
	}

	vMark.clear();
	m_pDrawPrev->GetLocalMark(vMark);
	for (const auto& mark : vMark)
	{
		LPG_ACGR pstMark = m_pdlgMarkShow->GetMarkInfo(mark.tgt_id, TRUE);

		if (pstMark)
		{
			STG_MARK stMark;
			stMark.stMark = mark;
			stMark.stInfo.dOffsetX = pstMark->cent_dist_x;
			stMark.stInfo.dOffsetY = pstMark->cent_dist_y;
			stMark.stInfo.dScore = pstMark->score_rate;
			stMark.stInfo.nResult = pstMark->marked;

			m_pDrawPrev->SetLocalMarkResult(stMark.stMark.tgt_id, stMark.stInfo);
		}
	}

	m_pDrawPrev->DrawMem(pstJob);
	m_pDrawPrev->Draw();
	
}


