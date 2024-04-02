
/*
 desc : Exposure
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgAuto.h"

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
#include "../param/IOManager.h"


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
CDlgAuto::CDlgAuto(UINT32 id, CWnd* parent)
	: CDlgMenu(id, parent)
{

	/* 자신의 다이얼로그 ID 설정 */
	m_enDlgID	= ENG_CMDI::en_menu_auto;
	m_u64ReqTime= GetTickCount64();

	for (int i = 0; i < _countof(m_pGrd); i++)
	{
		m_pGrd[i] = NULL;
	}

	for (int i = 0; i < _countof(m_pStt); i++)
	{
		m_pStt[i] = NULL;
	}

	for (int i = 0; i < _countof(m_pSttSeq); i++)
	{
		m_pSttSeq[i] = NULL;
	}

	m_pPgr = NULL;
	m_bBlink = FALSE;
	m_u64TickCount = 0;
}

CDlgAuto::~CDlgAuto()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgAuto::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	u32StartID = DEF_UI_AUTO_STT;
	for (i = 0; i < EN_AUTO_STT::_size(); i++)
	{
		m_pStt[i] = new CMyStatic();

		DDX_Control(dx, u32StartID + i, *m_pStt[i]);
	}
}

BEGIN_MESSAGE_MAP(CDlgAuto, CDlgMenu)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgAuto::PreTranslateMessage(MSG* msg)
{
	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgAuto::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();
	/* 객체 초기화 */
	if (!InitObject())	return FALSE;

	InitGridProcess();
	InitGridProduct();
	InitQuickIOGrid();
	UpdateQuickIOStatus();
	
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgAuto::OnExitDlg()
{
	CloseObject();

	m_vQuickIO.clear();
	m_vQuickIO.shrink_to_fit();

	for (int i = 0; i < _countof(m_pGrd); i++)
	{
		if (m_pGrd[i])
		{
			m_pGrd[i]->DeleteAllItems();
			delete m_pGrd[i];
			m_pGrd[i] = NULL;
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

	for (int i = 0; i < _countof(m_pSttSeq); i++)
	{
		if (m_pSttSeq[i])
		{
			delete m_pSttSeq[i];
			m_pSttSeq[i] = NULL;
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
VOID CDlgAuto::OnPaintDlg(CDC *dc)
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
VOID CDlgAuto::OnResizeDlg()
{
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgAuto::UpdatePeriod(UINT64 tick, BOOL is_busy)
{
	if (m_pPgr)
	{
		m_pPgr->SetPos((int)uvEng_GetWorkStepRate());
	}

	if (m_pDlgMain)
	{
		UpdateSttSeq(tick, is_busy);
	}

	m_u64TickCount++;
	UpdateGridProcess();
	UpdateGridProduct();
	UpdateQuickIOStatus();
	//DrawMarkData();

	if (!is_busy)
	{
		/* Luria가 초기화 되었는지 여부 확인 */
		if (uvEng_Luria_IsServiceInited())
		{
			/* 임의 주기마다 온도 값 요청 */
			if (m_u64ReqTime + 2000 < GetTickCount64())
			{
				/* 가장 최근에 요청한 시간 갱신 */
				m_u64ReqTime = GetTickCount64();
				/* 모든 광학계 내의 LED와 Board의 온도 값 요청 */
#if 0
				uvEng_Luria_ReqGetPhLedTempFreqAll(0);
#else
				uvEng_Luria_ReqGetPhLedTempAll();
#endif
			}
		}
	}
	
	m_bBlink = !m_bBlink;
}

/*
 desc : 컨트롤
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgAuto::UpdateControl(UINT64 tick, BOOL is_busy)
{

}


VOID CDlgAuto::UpdateSttSeq(UINT64 tick, BOOL is_busy)
{
	for (int i = EN_AUTO_STT_SEQ::READY; i <= EN_AUTO_STT_SEQ::UNLOAD; i++)
	{
		m_pSttSeq[i]->SetBgColor(LIGHT_GRAY);
	}

	int nSttSeqIndex = -1;
	if (100 > uvEng_GetWorkStepRate())
	{
		switch (m_pDlgMain->GetWorkJobID())
		{
		case ENG_BWOK::en_work_ready:
			nSttSeqIndex = EN_AUTO_STT_SEQ::LOAD;
			break;
		case ENG_BWOK::en_work_busy:
			break;
		case ENG_BWOK::en_work_init:
			nSttSeqIndex = EN_AUTO_STT_SEQ::READY;
			break;
		case ENG_BWOK::en_work_home:
			nSttSeqIndex = EN_AUTO_STT_SEQ::READY;
			break;
		case ENG_BWOK::en_work_stop:
			break;
		case ENG_BWOK::en_gerb_load:
			nSttSeqIndex = EN_AUTO_STT_SEQ::LOAD;
			break;
		case ENG_BWOK::en_gerb_unload:
			nSttSeqIndex = EN_AUTO_STT_SEQ::UNLOAD;
			break;
		case ENG_BWOK::en_mark_move:
			nSttSeqIndex = EN_AUTO_STT_SEQ::ALIGN;
			break;
		case ENG_BWOK::en_mark_test:
			nSttSeqIndex = EN_AUTO_STT_SEQ::ALIGN;
			break;
		case ENG_BWOK::en_expo_only:
			nSttSeqIndex = EN_AUTO_STT_SEQ::EXPOSE;
			break;
		case ENG_BWOK::en_expo_align:
			nSttSeqIndex = EN_AUTO_STT_SEQ::EXPOSE;
			break;
		case ENG_BWOK::en_expo_cali:
			nSttSeqIndex = EN_AUTO_STT_SEQ::EXPOSE;
			break;
		default:
			break;
		}

		if (0 > nSttSeqIndex)
		{
			return;
		}

		if (m_bBlink)
		{
			m_pSttSeq[nSttSeqIndex]->SetBgColor(SEA_GREEN);
			if (m_pSttSeq[nSttSeqIndex]->GetBgColor() != SEA_GREEN)
			{
				m_pSttSeq[nSttSeqIndex]->RedrawWindow();
				TRACE(_T("m_pSttSeq[%d]=GREEN\n"), nSttSeqIndex);
			}
		}
		else
		{
			m_pSttSeq[nSttSeqIndex]->SetBgColor(LIGHT_GRAY);
			if (m_pSttSeq[nSttSeqIndex]->GetBgColor() != LIGHT_GRAY)
			{
				m_pSttSeq[nSttSeqIndex]->RedrawWindow();
				TRACE(_T("m_pSttSeq[%d]=NORMAL\n"), nSttSeqIndex);
			}
		}
	}
	else
	{
		switch (m_pDlgMain->GetWorkJobID())
		{
		case ENG_BWOK::en_work_ready:
			nSttSeqIndex = EN_AUTO_STT_SEQ::LOAD;
			break;
		case ENG_BWOK::en_work_busy:
			break;
		case ENG_BWOK::en_work_init:
			nSttSeqIndex = EN_AUTO_STT_SEQ::READY;
			break;
		case ENG_BWOK::en_work_home:
			nSttSeqIndex = EN_AUTO_STT_SEQ::READY;
			break;
		case ENG_BWOK::en_work_stop:
			break;
		case ENG_BWOK::en_gerb_load:
			nSttSeqIndex = EN_AUTO_STT_SEQ::LOAD;
			break;
		case ENG_BWOK::en_gerb_unload:
			nSttSeqIndex = EN_AUTO_STT_SEQ::UNLOAD;
			break;
		case ENG_BWOK::en_mark_move:
			nSttSeqIndex = EN_AUTO_STT_SEQ::ALIGN;
			break;
		case ENG_BWOK::en_mark_test:
			nSttSeqIndex = EN_AUTO_STT_SEQ::ALIGN;
			break;
		case ENG_BWOK::en_expo_only:
			nSttSeqIndex = EN_AUTO_STT_SEQ::EXPOSE;
			break;
		case ENG_BWOK::en_expo_align:
			nSttSeqIndex = EN_AUTO_STT_SEQ::EXPOSE;
			break;
		case ENG_BWOK::en_expo_cali:
			nSttSeqIndex = EN_AUTO_STT_SEQ::EXPOSE;
			break;
		default:
			break;
		}

		if (0 > nSttSeqIndex)
		{
			return;
		}

		m_pSttSeq[nSttSeqIndex]->SetBgColor(DEF_COLOR_BTN_PAGE_SELECT);
		if (m_pSttSeq[nSttSeqIndex]->GetBgColor() != DEF_COLOR_BTN_PAGE_SELECT)
		{
			m_pSttSeq[nSttSeqIndex]->RedrawWindow();
			TRACE(_T("m_pSttSeq[%d]=SELECT\n"), nSttSeqIndex);
		}
	}
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgAuto::InitCtrl()
{
	INT32 i = 0;

	CResizeUI clsResizeUI;
	for (i = 0; i < _countof(m_pStt); i++)
	{
		m_pStt[i]->SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_pStt[i]->SetDrawBg(TRUE);
		m_pStt[i]->SetBaseProp(0, 1, 0, 0, FORD_GRAPHITE, 0, WHITE_);

		clsResizeUI.ResizeControl(this, m_pStt[i]);
	}

	CRect rtStt;
	m_pStt[EN_AUTO_STT::SEQUENCE_STATUS]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	CRect	rtSttSeq;;
	UINT	uiSttSeqWidth = (UINT)((rtStt.Width() - DEF_UI_OFFSET * (EN_AUTO_STT_SEQ::_size() - 1)) / EN_AUTO_STT_SEQ::_size());
	UINT	uiSttSeqHeight = rtStt.Height();

	rtSttSeq.left	= rtStt.left;
	rtSttSeq.top	= rtStt.bottom + DEF_UI_OFFSET;
	rtSttSeq.right	= rtSttSeq.left + uiSttSeqWidth;
	rtSttSeq.bottom = rtSttSeq.top + uiSttSeqHeight;
	CString strText;
	int nSttSeqIndex = 0;
	for (nSttSeqIndex = 0; nSttSeqIndex < _countof(m_pSttSeq); nSttSeqIndex++)
	{
		if (EN_AUTO_STT_SEQ::READY < nSttSeqIndex)
		{
			rtSttSeq.OffsetRect(uiSttSeqWidth + DEF_UI_OFFSET, 0);
		}

		strText = (CString)EN_AUTO_STT_SEQ::_from_index(nSttSeqIndex)._to_string();
		strText.Replace(_T("_"), _T(" "));

		m_pSttSeq[nSttSeqIndex] = new CMyStatic();
		m_pSttSeq[nSttSeqIndex]->Create(strText, WS_BORDER | WS_VISIBLE | BS_CENTER | BS_VCENTER | SS_CENTER, rtSttSeq, this, DEF_UI_AUTO_STT + (int)EN_AUTO_STT::_size() + nSttSeqIndex);
		m_pSttSeq[nSttSeqIndex]->SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_pSttSeq[nSttSeqIndex]->SetDrawBg(TRUE);
		m_pSttSeq[nSttSeqIndex]->SetBaseProp(0, 1, 0, 0, LIGHT_GRAY, 0, WHITE_);
	}

	m_pStt[EN_AUTO_STT::SEQUENCE_STATUS]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	CRect rtPrg;
	rtPrg.left	= rtStt.left;
	rtPrg.top	= rtStt.bottom + uiSttSeqHeight + DEF_UI_OFFSET;
	rtPrg.right = rtStt.right;
	rtPrg.bottom = rtPrg.top + rtStt.Height();
	m_pPgr = new CProgressCtrlX();
	m_pPgr->Create(WS_BORDER | WS_VISIBLE, rtPrg, this, DEF_UI_AUTO_PRG);
	m_pPgr->SetBkColor(ALICE_BLUE);
	m_pPgr->SetTextColor(BLACK_);
	m_pPgr->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	m_pPgr->SetShowPercent(TRUE);
	m_pPgr->SetRange(0, 100);

	
	m_pStt[EN_AUTO_STT::PREVIEW]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	CRect rtPreview;
	GetDlgItem(IDC_AUTO_PIC_PREVIEW)->GetWindowRect(rtPreview);
	this->ScreenToClient(rtPreview);

	int nHeight			= (int)(rtPreview.Height() * clsResizeUI.GetRateY());
	rtPreview			= rtStt;
	rtPreview.top		= rtStt.bottom + DEF_UI_OFFSET;
	rtPreview.bottom	= rtPreview.top + DEF_UI_OFFSET + nHeight;
	GetDlgItem(IDC_AUTO_PIC_PREVIEW)->MoveWindow(rtPreview);
	CRect rtOffset;
	m_pDrawPrev = new CDrawPrev(GetDlgItem(IDC_AUTO_PIC_PREVIEW)->GetSafeHwnd(), rtOffset);

	//////////////////////////////////////////////////////////////////////////
	// IO QUICK
	int nGridIndex = eGRD_IO_QUICK1;
	m_pStt[EN_AUTO_STT::IO_STATUS]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	CRect rtGrd = rtStt;
	for (int i = nGridIndex; i < eGRD_MAX; i++)
	{
		int nRowHeight = rtStt.Height();
		rtGrd.top = rtGrd.bottom;
		rtGrd.bottom = rtGrd.top + nRowHeight * 2;

		m_pGrd[i] = new CGridCtrl();
		m_pGrd[i]->Create(rtGrd, this, DEF_UI_AUTO_GRD + i, (WS_CHILD | WS_TABSTOP | WS_VISIBLE));
	}
	//////////////////////////////////////////////////////////////////////////

	m_pdlgMarkShow = new CDlgMarkShow(this);
	m_pdlgMarkShow->Create();
	m_pdlgMarkShow->ShowWindow(SW_HIDE);
}

/*
 desc : 객체 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgAuto::InitObject()
{
	LPG_RJAF pstJob = uvEng_JobRecipe_GetSelectRecipe();
	if (pstJob)
	{
		m_pDrawPrev->LoadMark(uvEng_JobRecipe_GetSelectRecipe());
		m_pDrawPrev->DrawMem(uvEng_JobRecipe_GetSelectRecipe());
	}
	
	return TRUE;
}

/*
 desc : 객체 해제
 parm : None
 retn : None
*/
VOID CDlgAuto::CloseObject()
{
	/* 마크 검색 결과 출력 객체 */
	if (m_pDrawPrev)	delete m_pDrawPrev;
}

void CDlgAuto::InitGridProcess()
{
	CRect rtStt;
	CRect rctSize;										/* 작업 영역	*/

	double	dCellRatio[EN_GRD_PROCESS_COL::_size() + 1];/* 그리드 비율	*/
	int		nHeightSize		= DEF_UI_GRD_ROW_OFFSET;	/* 셀 높이		*/
	int		nWidthDiffer	= 1;						/* 셀 너비 오차값 */

	m_pStt[EN_AUTO_STT::HEAD_PROCESS_INFORMATION]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);
	
	dCellRatio[0] = ((double)DEF_UI_GRD_COL_TITLE / (double)rtStt.Width());
	for (int i = 0; i < (int)EN_GRD_PROCESS_COL::_size(); i++)
	{
		dCellRatio[i+1] = ((double)(rtStt.Width() - DEF_UI_GRD_COL_TITLE) / (double)EN_GRD_PROCESS_COL::_size()) / (double)rtStt.Width();
	}

	UINT nCenterAlignText = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;	/* 그리드 옵션 */

	// 그리드 기본 설정
	int	nGridIndex = eGRD_PROCESS;
		
	rctSize			= rtStt;
	rctSize.top		= rtStt.bottom + DEF_UI_OFFSET;
	rctSize.bottom	= rctSize.top + (nHeightSize * (int)(EN_GRD_PROCESS_ROW::_size() + 1)) + 1;

	if (NULL == m_pGrd[nGridIndex])
	{
		m_pGrd[nGridIndex] = new CGridCtrl();
		m_pGrd[nGridIndex]->Create(rctSize, this, DEF_UI_AUTO_GRD + nGridIndex, (WS_CHILD | WS_TABSTOP | WS_VISIBLE));
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
	if (rctSize.Height() - 1 < nHeightSize * (int)(EN_GRD_PROCESS_ROW::_size() + 1))
	{
		nWidthDiffer = ::GetSystemMetrics(SM_CXVSCROLL);
	}

	// 셀의 개수 지정
	m_pGrd[nGridIndex]->SetColumnCount(_countof(dCellRatio));
	m_pGrd[nGridIndex]->SetRowCount((int)EN_GRD_PROCESS_ROW::_size() + 1);

	CString strText;
	for (int nRow = 0; nRow < (int)EN_GRD_PROCESS_ROW::_size() + 1; nRow++)
	{
		m_pGrd[nGridIndex]->SetRowHeight(nRow, nHeightSize);
		
		for (int nCol = 0; nCol < _countof(dCellRatio); nCol++)
		{
			m_pGrd[nGridIndex]->SetColumnWidth(nCol, (int)((rctSize.Width() - nWidthDiffer) * dCellRatio[nCol]));
			m_pGrd[nGridIndex]->SetItemFormat(nRow, nCol, nCenterAlignText);

			if (0 == nRow && 0 < nCol)
			{
				strText = (CString)EN_GRD_PROCESS_COL::_from_index(nCol - 1)._to_string();
				strText.Replace(_T("_"), _T(" "));
				m_pGrd[nGridIndex]->SetItemText(nRow, nCol, strText);
				m_pGrd[nGridIndex]->SetItemBkColour(nRow, nCol, DEF_COLOR_BTN_MENU_NORMAL);
				m_pGrd[nGridIndex]->SetItemFgColour(nRow, nCol, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
			}
			else
			{
				if (0 == nCol)
				{
					m_pGrd[nGridIndex]->SetItemBkColour(nRow, nCol, DEF_COLOR_BTN_MENU_NORMAL);
					m_pGrd[nGridIndex]->SetItemFgColour(nRow, nCol, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
				}
				else
				{
					m_pGrd[nGridIndex]->SetItemBkColour(nRow, nCol, ALICE_BLUE);
					m_pGrd[nGridIndex]->SetItemFgColour(nRow, nCol, BLACK_);
				}
			}
		}

		if (EN_GRD_PROCESS_ROW::TEMP_LED + 1 < nRow)
		{
			m_pGrd[nGridIndex]->MergeCells(CCellRange(nRow, 1, nRow, _countof(dCellRatio) - 1));
		}

		if (0 < nRow)
		{
			strText = (CString)EN_GRD_PROCESS_ROW::_from_index(nRow - 1)._to_string();
			strText.Replace(_T("_"), _T(" "));
			m_pGrd[nGridIndex]->SetItemText(nRow, 0, strText);
		}
	}

	m_pGrd[nGridIndex]->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	m_pGrd[nGridIndex]->Refresh();

	UpdateGridProcess();
}

void CDlgAuto::UpdateGridProcess()
{
	int	nGridIndex = eGRD_PROCESS;
	CUniToChar	clsCvt;

	LPG_RJAF	pstJob = uvEng_JobRecipe_GetSelectRecipe();
	if (!pstJob) return;

	LPG_REAF	pstExpo = uvEng_ExpoRecipe_GetRecipeOnlyName(clsCvt.Ansi2Uni(pstJob->expo_recipe));
	if (!pstExpo) return;
	
	LPG_PLPI	pstPowerI = uvEng_LedPower_GetLedPowerName(clsCvt.Ansi2Uni(pstExpo->power_name));
	if (!pstPowerI) return;

	DOUBLE dbTotal = 0.0f, dbPowerWatt[MAX_LED] = { NULL }, dbSpeed = 0.0f, dLedTemp = 0.0f;
	double dPosition = 0.0f;
	UINT16(*pLed)[8] = uvEng_ShMem_GetLuria()->directph.light_source_driver_temp_led;
	UINT16 u16TempLed, u16TempBoard;

	for (int i = 0; i < (int)EN_GRD_PROCESS_COL::_size(); i++)
	{
		if (m_pGrd[nGridIndex]->GetColumnCount() <= i)
		{
			break;
		}

		dPosition = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI((int)ENG_MMDI::en_axis_ph1 + i));
		m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_PROCESS_ROW::AF_Z + 1, i + 1, _T("%.1f [mm]"), dPosition);

		//dbTotal = pstPowerI->led_watt[i][0];
		//dLedTemp = pLed[i][0] / 10.0f;
		dbTotal = 0;
		for (int j = 0; j < 4; j++)
		{
			dbTotal += pstPowerI->led_watt[i][j];
			dLedTemp += pLed[i][j] / 10.0f;
		}

		/* 현재 광학계의 가장 높은 온도를 가진 Led와 Board 값 얻기 */
		uvEng_ShMem_GetLuria()->directph.GetMaxTempLedBD(i + 1, u16TempLed, u16TempBoard);

		m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_PROCESS_ROW::TEMP_LED + 1, i + 1, _T("%.1f [°c]"), u16TempLed / 10.0f);

		m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_PROCESS_ROW::POWER + 1, i + 1, _T("%.1f [W]"), dbTotal);
	}

	m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_PROCESS_ROW::ENERGE + 1, 1, _T("%.3f [mJ]"), pstJob->expo_energy);

	m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_PROCESS_ROW::SPEED + 1, 1, _T("%.1f [%%]"), pstJob->expo_speed * 100.0);

	m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_PROCESS_ROW::GERBER_NAME + 1, 1, _T("%s"), clsCvt.Ansi2Uni(pstJob->gerber_name));

	m_pGrd[nGridIndex]->Refresh();
}

void CDlgAuto::InitGridProduct()
{
	CRect rtStt;
	CRect rctSize;										/* 작업 영역	*/

	m_pStt[EN_AUTO_STT::PRODUCT_INFORMATION]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	double	dCellRatio[7] = { 
	(double)DEF_UI_GRD_COL_TITLE / (double)rtStt.Width()
	, (double)(rtStt.Width() - DEF_UI_GRD_COL_TITLE) / (double)rtStt.Width() / (double)6.0
	, (double)(rtStt.Width() - DEF_UI_GRD_COL_TITLE) / (double)rtStt.Width() / (double)6.0
	, (double)(rtStt.Width() - DEF_UI_GRD_COL_TITLE) / (double)rtStt.Width() / (double)6.0
	, (double)(rtStt.Width() - DEF_UI_GRD_COL_TITLE) / (double)rtStt.Width() / (double)6.0
	, (double)(rtStt.Width() - DEF_UI_GRD_COL_TITLE) / (double)rtStt.Width() / (double)6.0
	, (double)(rtStt.Width() - DEF_UI_GRD_COL_TITLE) / (double)rtStt.Width() / (double)6.0
	};				/* 그리드 비율	*/

	int		nHeightSize = DEF_UI_GRD_ROW_OFFSET;	/* 셀 높이		*/
	int		nWidthDiffer = 1;						/* 셀 너비 오차값 */

	UINT nCenterAlignText = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;	/* 그리드 옵션 */

	// 그리드 기본 설정
	int	nGridIndex = eGRD_PRODUCT;

	rctSize			= rtStt;
	rctSize.top		= rtStt.bottom + DEF_UI_OFFSET;
	rctSize.bottom	= rctSize.top + (nHeightSize * (int)EN_GRD_PRODUCT_ROW::_size()) + 1;

	if (NULL == m_pGrd[nGridIndex])
	{
		m_pGrd[nGridIndex] = new CGridCtrl();
		m_pGrd[nGridIndex]->Create(rctSize, this, DEF_UI_AUTO_GRD + nGridIndex, (WS_CHILD | WS_TABSTOP | WS_VISIBLE));
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
	if (rctSize.Height() - 1 < nHeightSize * (int)EN_GRD_PRODUCT_ROW::_size())
	{
		nWidthDiffer = ::GetSystemMetrics(SM_CXVSCROLL);
	}

	// 셀의 개수 지정
	m_pGrd[nGridIndex]->SetColumnCount(7);
	m_pGrd[nGridIndex]->SetRowCount((int)EN_GRD_PRODUCT_ROW::_size());

	CString strText;
	for (int nRow = 0; nRow < (int)EN_GRD_PRODUCT_ROW::_size(); nRow++)
	{
		m_pGrd[nGridIndex]->SetRowHeight(nRow, nHeightSize);

		strText = (CString)EN_GRD_PRODUCT_ROW::_from_index(nRow)._to_string();
		strText.Replace(_T("_"), _T(" "));
		m_pGrd[nGridIndex]->SetItemText(nRow, 0, strText);

		for (int nCol = 0; nCol < 7; nCol++)
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
				m_pGrd[nGridIndex]->SetItemBkColour(nRow, nCol, ALICE_BLUE);
				m_pGrd[nGridIndex]->SetItemFgColour(nRow, nCol, BLACK_);
			}
		}

		if (nRow != EN_GRD_PRODUCT_ROW::MARK_DIST_ERROR)
		{
			m_pGrd[nGridIndex]->MergeCells(CCellRange(nRow, 1, nRow, 6));
		}
	}

	m_pGrd[nGridIndex]->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	m_pGrd[nGridIndex]->Refresh();

	UpdateGridProduct();
}

void CDlgAuto::UpdateGridProduct()
{
	int	nGridIndex = eGRD_PRODUCT;
	CUniToChar	clsCvt;
	LPG_RJAF	pstJob = uvEng_JobRecipe_GetSelectRecipe();
	if (!pstJob) return;

	UINT32 u32JobCount	= uvEng_GetJobWorkCount();
	UINT64 u64JobTime	= uvEng_GetJobWorkTime();
	UINT64 u64JobTimeAvg= uvEng_GetJobWorkTimeAverage();

	m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_PRODUCT_ROW::PRODUCT_COUNT, 1, _T("%d"), u32JobCount);
	m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_PRODUCT_ROW::LAST_TACT_TIME, 1, _T("Last : %u m %02u s"), uvCmn_GetTimeToType(u64JobTime, 0x01), uvCmn_GetTimeToType(u64JobTime, 0x02));
	m_pGrd[nGridIndex]->SetItemTextFmt(EN_GRD_PRODUCT_ROW::AVERAGE_TACT_TIME, 1, _T("Avgs : %u m %02u s"), uvCmn_GetTimeToType(u64JobTimeAvg, 0x01), uvCmn_GetTimeToType(u64JobTimeAvg, 0x02));
	 	 
  	int nMaxCol = m_pGrd[nGridIndex]->GetColumnCount();
  	CString strArrMarkErr[6] = { _T("[ ￣ ]"),_T("[ ＿ ]"),_T("[│  ]"),_T("[  │]"),_T("[ ＼ ]"),_T("[ ／ ]") };
  	for (int nCol = 1; nCol < nMaxCol; nCol++)
  	{
		m_pGrd[nGridIndex]->SetItemText(EN_GRD_PRODUCT_ROW::MARK_DIST_ERROR , nCol, strArrMarkErr[nCol - 1]);
  
  		switch (uvEng_GetConfig()->mark_diff.result[nCol - 1].len_valid)	/* 0x00 : Not set or Reset, 0x01 : Set (Succ), 0x02 : Set (Fail) */
  		{
  		case 0x00:
  			m_pGrd[nGridIndex]->SetItemBkColour(EN_GRD_PRODUCT_ROW::MARK_DIST_ERROR, nCol, ALICE_BLUE);
  			m_pGrd[nGridIndex]->SetItemFgColour(EN_GRD_PRODUCT_ROW::MARK_DIST_ERROR, nCol, BLACK_);
  			break;
  		case 0x01:
  			m_pGrd[nGridIndex]->SetItemBkColour(EN_GRD_PRODUCT_ROW::MARK_DIST_ERROR, nCol, SEA_GREEN);
  			m_pGrd[nGridIndex]->SetItemFgColour(EN_GRD_PRODUCT_ROW::MARK_DIST_ERROR, nCol, WHITE_);
  			break;
  		case 0x02:
  			m_pGrd[nGridIndex]->SetItemBkColour(EN_GRD_PRODUCT_ROW::MARK_DIST_ERROR, nCol, TOMATO);
  			m_pGrd[nGridIndex]->SetItemFgColour(EN_GRD_PRODUCT_ROW::MARK_DIST_ERROR, nCol, WHITE_);
  			break;
  		default:
  			break;
  		}
  	}
  
	m_pGrd[nGridIndex]->Refresh();
}


VOID CDlgAuto::OnLButtonDown(UINT32 nFlags, CPoint point)
{
	if (m_pDrawPrev)
	{
		CRect rtDlg;
		CRect rtPreview;
		CRect rtPreviewOrg;

		GetWindowRect(rtDlg);
		GetDlgItem(IDC_AUTO_PIC_PREVIEW)->GetWindowRect(rtPreview);

		rtPreviewOrg = rtPreview;
		rtPreview.OffsetRect(-rtDlg.left, -rtDlg.top);
		
		if (rtPreview.PtInRect(point))
		{
			m_pDrawPrev->OnMouseClick(point.x - rtPreview.left, point.y - rtPreview.top);
			m_pDrawPrev->DrawMem(uvEng_JobRecipe_GetSelectRecipe());
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

VOID CDlgAuto::InitQuickIOGrid()
{
	m_vQuickIO2.clear();
	CIOManager::GetInstance()->GetQuickIOList(m_vQuickIO2);

	CRect rctSize;						/* 작업 영역 */
	CRect rctOldSize;					/* 작업 영역 */

	double dCellRatio[2] = { 0.5, 0.5 };	/* 그리드 비율 */
	int nHeightSize = 25;	/* 셀 높이 */
	int	nWidthDiffer = 1;	/* 셀 너비 오차값 */

	UINT nCenterAlignText = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;	/* 그리드 옵션 */

	// 그리드 기본 설정
	int	nGridIndex = eGRD_IO_QUICK1;
	m_pStt[EN_AUTO_STT::IO_STATUS]->GetWindowRect(rctOldSize);
	this->ScreenToClient(rctOldSize);

	for (const auto& io : m_vQuickIO2)
	{
		if (nGridIndex >= eGRD_MAX)
		{
			break;
		}

		int nRowCount = 2;

		rctSize			= rctOldSize;
		rctSize.top		= rctOldSize.bottom + DEF_UI_OFFSET;
		rctSize.bottom	= rctSize.top + (nRowCount * nHeightSize) + 1;
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
		if (rctSize.Height() - 1 < nHeightSize * nRowCount)
		{
			nWidthDiffer = ::GetSystemMetrics(SM_CXVSCROLL);
		}

		// 셀의 개수 지정
		m_pGrd[nGridIndex]->SetColumnCount((int)1);
		m_pGrd[nGridIndex]->SetRowCount(nRowCount);

		CString strText;
		// 높이, 너비, 색상, 문자 적용

		for (int nRow = 0; nRow < nRowCount; nRow++)
		{
			m_pGrd[nGridIndex]->SetRowHeight(nRow, nHeightSize);
			for (int nCol = 0; nCol < 1; nCol++)
			{
				if (0 == nRow)
				{
					m_pGrd[nGridIndex]->SetColumnWidth(nCol, (int)((rctSize.Width())));
					m_pGrd[nGridIndex]->SetItemBkColour(nRow, nCol, DEF_COLOR_BTN_MENU_NORMAL);
					m_pGrd[nGridIndex]->SetItemFgColour(nRow, nCol, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
					m_pGrd[nGridIndex]->SetItemFormat(nRow, nCol, nCenterAlignText);
				}
				else
				{
					m_pGrd[nGridIndex]->SetItemFormat(nRow, nCol, nCenterAlignText);
				}
			}
		}

 		int nRowIndex = 0;
 		strText = io.strName.c_str();
 		strText.Replace(_T("_"), _T(" "));
 		m_pGrd[nGridIndex]->SetItemText(nRowIndex, 0, strText);
 		nRowIndex++;
 		m_pGrd[nGridIndex]->SetItemText(nRowIndex, 0, _T("ON"));


		m_pGrd[nGridIndex]->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
		m_pGrd[nGridIndex]->Refresh();
		nGridIndex++;

		rctOldSize = rctSize;
	}

	for (; nGridIndex < eGRD_MAX; nGridIndex++)
	{
		m_pGrd[nGridIndex]->ShowWindow(SW_HIDE);
	}
}

VOID CDlgAuto::UpdateQuickIOStatus()
{
	m_vQuickIO2.clear();
	CIOManager::GetInstance()->UpdateQUICKIO();
	CIOManager::GetInstance()->GetQuickIOList(m_vQuickIO2);
	int	nGridIndex = eGRD_IO_QUICK1;

	for (const auto& io : m_vQuickIO2)
	{
		if (nGridIndex >= eGRD_MAX)
		{
			break;
		}

		int	nRowCount = 2;
		int nRowIndex = 0;
		nRowIndex++;
		if (io.bOn)
		{
			m_pGrd[nGridIndex]->SetItemBkColour(nRowIndex, 0, GetIOColorBg(_T("B")));
			m_pGrd[nGridIndex]->SetItemFgColour(nRowIndex, 0, GetIOColorFg(_T("B")));
			m_pGrd[nGridIndex]->SetItemText(nRowIndex, 0, _T("ON"));

		}
		else
		{
			m_pGrd[nGridIndex]->SetItemBkColour(nRowIndex, 0, GetIOColorBg(_T("W")));
			m_pGrd[nGridIndex]->SetItemFgColour(nRowIndex, 0, GetIOColorFg(_T("W")));
			m_pGrd[nGridIndex]->SetItemText(nRowIndex, 0, _T("OFF"));

		}



		m_pGrd[nGridIndex]->Refresh();
		nGridIndex++;
	}
}


COLORREF CDlgAuto::GetIOColorBg(CString strColor)
{
	if (_T("R") == strColor)
	{
		return TOMATO;
	}
	else if (_T("G") == strColor)
	{
		return SEA_GREEN;
	}
	else if (_T("B") == strColor)
	{
		return MEDIUM_BLUE;
	}
	else if (_T("W") == strColor)
	{
		return WHITE_;
	}

	return LIGHT_GRAY;
}

COLORREF CDlgAuto::GetIOColorFg(CString strColor)
{
	if (_T("R") == strColor)
	{
		return WHITE_;
	}
	else if (_T("G") == strColor)
	{
		return WHITE_;
	}
	else if (_T("B") == strColor)
	{
		return WHITE_;
	}
	else if (_T("W") == strColor)
	{
		return BLACK_;
	}

	return BLACK_;
}

VOID CDlgAuto::DrawMarkData()
{
	if (0 != m_u64TickCount%10)
	{
		return;
	}

	// TEST CODE
// 	STG_MARK stMark;
// 	stMark.stInfo.dOffsetX = 0;
// 	stMark.stInfo.dOffsetY = 0;
// 	stMark.stInfo.dScore = 100;
// 	stMark.stInfo.nResult = 2;
// 	m_pDrawPrev->SetGlobalMarkResult(2, stMark.stInfo);
// 	m_pDrawPrev->SetGlobalMarkResult(3, stMark.stInfo);

	LPG_RJAF pstJob = uvEng_JobRecipe_GetSelectRecipe();
	if (m_pdlgMarkShow && pstJob)
	{
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

		m_pDrawPrev->DrawMem(uvEng_JobRecipe_GetSelectRecipe());
		m_pDrawPrev->Draw();
	}
}
