
/*
 desc : Motion & PLC Monitoring
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgCtrl.h"

#include "./ctrl/GridMotor.h"
#include "./ctrl/GridPLC.h"
#include <bitset>


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
CDlgCtrl::CDlgCtrl(UINT32 id, CWnd* parent)
	: CDlgMenu(id, parent)
{
	/* 자신의 다이얼로그 ID 설정 */
	m_enDlgID		= ENG_CMDI::en_menu_ctrl;

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

	m_u8ACamCount = uvEng_GetConfig()->set_cams.acam_count;		/* 카메라 개수를 가져온다. */
	m_u8HeadCount = uvEng_GetConfig()->luria_svc.ph_count;		/* Photo Head 개수를 가져온다. */
	m_u8StageCount = uvEng_GetConfig()->luria_svc.table_count;	/* Table의 개수를 가져온다. */

	// Stage 개수(x, y축) + Head 개수 + Align Camera 개수
	m_u8AllMotorCount = (m_u8StageCount * 2) + m_u8HeadCount + (m_u8ACamCount * 2);

	m_vMotor.clear();

	m_nPage		= 0;
	m_nMaxPage	= ((CIOManager::GetInstance()->GetInputCount() + 16) / 32) + ((CIOManager::GetInstance()->GetOutputCount() + 16) / 32);

	CIOManager::GetInstance()->GetQuickIOList(m_vQuickIO2);

	//ST_IO IO;
	//CIOManager::GetInstance()->GetIO(eIO_INPUT, _T("CHUCK_VACUUM_VAC_STATUS"), IO);
}

CDlgCtrl::~CDlgCtrl()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgCtrl::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	u32StartID	= DEF_UI_CTRL_STT;
	for (i = 0; i < EN_CTRL_STT::_size(); i++)
	{
		m_pStt[i] = new CMyStatic();

		DDX_Control(dx, u32StartID + i, *m_pStt[i]);
	}
	
}

BEGIN_MESSAGE_MAP(CDlgCtrl, CDlgMenu)
 	ON_CONTROL_RANGE(BN_CLICKED, DEF_UI_CTRL_BTN, DEF_UI_CTRL_BTN + (UINT)EN_CTRL_BTN::_size(), OnBtnClick)
	ON_NOTIFY_RANGE(NM_CLICK, DEF_UI_CTRL_GRD, DEF_UI_CTRL_GRD + eGRD_MAX, OnGrdClick)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgCtrl::PreTranslateMessage(MSG* msg)
{
	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgCtrl::OnInitDlg()
{
	// by sysandj : Resize UI
	CResizeUI clsResizeUI;

	/* 컨트롤 초기화 */
	InitCtrl();
	if (!InitGrid())	return FALSE;

	for (int i = 0; i < _countof(m_pStt); i++)
	{
		clsResizeUI.ResizeControl(this, m_pStt[i]);
	}

	for (int i = 0; i < _countof(m_pGrd); i++)
	{
		clsResizeUI.ResizeControl(this, m_pGrd[i]);
	}

	for (int i = 0; i < _countof(m_pBtn); i++)
	{
		clsResizeUI.ResizeControl(this, m_pBtn[i]);
	}

	InitValue();
	InitMotorGrid();
	InitIOGrid();
	InitQuickIOGrid();

	UpdateIOStatus();
	UpdateQuickIOStatus();
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgCtrl::OnExitDlg()
{
	m_vMotor.clear();
	m_vMotor.shrink_to_fit();

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

	CloseGrid();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgCtrl::OnPaintDlg(CDC *dc)
{

	/* 자식 윈도 호출 */
	CDlgMenu::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgCtrl::OnResizeDlg()
{
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgCtrl::UpdatePeriod(UINT64 tick, BOOL is_busy)
{
	UpdateMotorStatus();
	UpdateQuickIOStatus();
}

/*
 desc : 컨트롤
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgCtrl::UpdateControl(UINT64 tick, BOOL is_busy)
{
	
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgCtrl::InitCtrl()
{
	INT32 i = 0;

	for (i = 0; i < _countof(m_pStt); i++)
	{
		m_pStt[i]->SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_pStt[i]->SetDrawBg(1);
		m_pStt[i]->SetBaseProp(0, 1, 0, 0, FORD_GRAPHITE, 0, WHITE_);
	}
	
	CRect rtStt;
	m_pStt[EN_CTRL_STT::ALL_MOTOR]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	CRect rtBtn;;
	UINT  uiBtnWidth = (rtStt.Width() - DEF_UI_OFFSET * 2) / 3;
	UINT  uiBtnHeight = rtStt.Height();
	
	rtBtn.left	= rtStt.left;
	rtBtn.top	= rtStt.bottom + DEF_UI_OFFSET;
	rtBtn.right = rtBtn.left + uiBtnWidth;
	rtBtn.bottom = rtBtn.top + uiBtnHeight;
	CString strText;

	//////////////////////////////////////////////////////////////////////////
	/// ALL_STOP/ALL_HOME/ALL_RESET
	int nBtnIndex = EN_CTRL_BTN::ALL_STOP;
	for (nBtnIndex = EN_CTRL_BTN::ALL_STOP; nBtnIndex <= EN_CTRL_BTN::ALL_RESET; nBtnIndex++)
	{
		if (EN_CTRL_BTN::ALL_STOP < nBtnIndex)
		{
			rtBtn.OffsetRect(uiBtnWidth + DEF_UI_OFFSET, 0);
		}
		m_pBtn[nBtnIndex] = new CMacButton();
		strText = (CString)EN_CTRL_BTN::_from_index(nBtnIndex)._to_string();
		strText.Replace(_T("_"), _T(" "));
		m_pBtn[nBtnIndex]->Create(strText, WS_BORDER | WS_VISIBLE, rtBtn, this, DEF_UI_CTRL_BTN + nBtnIndex);
		m_pBtn[nBtnIndex]->SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_pBtn[nBtnIndex]->SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
		m_pBtn[nBtnIndex]->SetTextColor(DEF_COLOR_BTN_PAGE_NORMAL_TEXT);
	}
	//////////////////////////////////////////////////////////////////////////
	
	m_pStt[EN_CTRL_STT::MOTOR_CONTROL]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	uiBtnWidth		= (rtStt.Width());
	uiBtnHeight		= rtStt.Height();

	rtBtn.left		= rtStt.left;
	rtBtn.top		= rtStt.bottom + DEF_UI_OFFSET;
	rtBtn.right		= rtBtn.left + uiBtnWidth;
	rtBtn.bottom	= rtBtn.top + uiBtnHeight;

	nBtnIndex = EN_CTRL_BTN::OPEN_CONTROL_PANEL;
	m_pBtn[nBtnIndex] = new CMacButton();
	strText = (CString)EN_CTRL_BTN::_from_index(nBtnIndex)._to_string();
	strText.Replace(_T("_"), _T(" "));
	m_pBtn[nBtnIndex]->Create(strText, WS_BORDER | WS_VISIBLE, rtBtn, this, DEF_UI_CTRL_BTN + nBtnIndex);
	m_pBtn[nBtnIndex]->SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
	m_pBtn[nBtnIndex]->SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
	m_pBtn[nBtnIndex]->SetTextColor(DEF_COLOR_BTN_PAGE_NORMAL_TEXT);

	//////////////////////////////////////////////////////////////////////////

	m_pStt[EN_CTRL_STT::ETC_CONTROL]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	uiBtnWidth		= (rtStt.Width() - DEF_UI_OFFSET * 2) / 3;
	uiBtnHeight		= rtStt.Height();

	rtBtn.left		= rtStt.left;
	rtBtn.top		= rtStt.bottom + DEF_UI_OFFSET;
	rtBtn.right		= rtBtn.left + uiBtnWidth;
	rtBtn.bottom	= rtBtn.top + uiBtnHeight;

	nBtnIndex = EN_CTRL_BTN::RESET_TRIGGER;
	m_pBtn[nBtnIndex] = new CMacButton();
	strText = (CString)EN_CTRL_BTN::_from_index(nBtnIndex)._to_string();
	strText.Replace(_T("_"), _T(" "));
	m_pBtn[nBtnIndex]->Create(strText, WS_BORDER | WS_VISIBLE, rtBtn, this, DEF_UI_CTRL_BTN + nBtnIndex);
	m_pBtn[nBtnIndex]->SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
	m_pBtn[nBtnIndex]->SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
	m_pBtn[nBtnIndex]->SetTextColor(DEF_COLOR_BTN_PAGE_NORMAL_TEXT);

	//////////////////////////////////////////////////////////////////////////

	m_pStt[EN_CTRL_STT::ETC_CONTROL]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	uiBtnWidth = (rtStt.Width() - DEF_UI_OFFSET * 2) / 3;
	uiBtnHeight = rtStt.Height();

	rtBtn.left = rtStt.left + uiBtnWidth+3;
	rtBtn.top = rtStt.bottom + DEF_UI_OFFSET;
	rtBtn.right = rtBtn.left + uiBtnWidth;
	rtBtn.bottom = rtBtn.top + uiBtnHeight;

	nBtnIndex = EN_CTRL_BTN::HEAD_HWINIT;
	m_pBtn[nBtnIndex] = new CMacButton();
	strText = (CString)EN_CTRL_BTN::_from_index(nBtnIndex)._to_string();
	strText.Replace(_T("_"), _T(" "));
	m_pBtn[nBtnIndex]->Create(strText, WS_BORDER | WS_VISIBLE, rtBtn, this, DEF_UI_CTRL_BTN + nBtnIndex);
	m_pBtn[nBtnIndex]->SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
	m_pBtn[nBtnIndex]->SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
	m_pBtn[nBtnIndex]->SetTextColor(DEF_COLOR_BTN_PAGE_NORMAL_TEXT);

	//////////////////////////////////////////////////////////////////////////

	m_pStt[EN_CTRL_STT::ETC_CONTROL]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	uiBtnWidth = (rtStt.Width() - DEF_UI_OFFSET * 2) / 3;
	uiBtnHeight = rtStt.Height();

	rtBtn.left = rtStt.left + uiBtnWidth + 3 + uiBtnWidth + 3;
	rtBtn.top = rtStt.bottom + DEF_UI_OFFSET;
	rtBtn.right = rtBtn.left + uiBtnWidth + 4;
	rtBtn.bottom = rtBtn.top + uiBtnHeight;

	nBtnIndex = EN_CTRL_BTN::STROBE_LAMP;
	m_pBtn[nBtnIndex] = new CMacButton();
	strText = (CString)EN_CTRL_BTN::_from_index(nBtnIndex)._to_string();
	strText.Replace(_T("_"), _T(" "));
	m_pBtn[nBtnIndex]->Create(strText, WS_BORDER | WS_VISIBLE, rtBtn, this, DEF_UI_CTRL_BTN + nBtnIndex);
	m_pBtn[nBtnIndex]->SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
	m_pBtn[nBtnIndex]->SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
	m_pBtn[nBtnIndex]->SetTextColor(DEF_COLOR_BTN_PAGE_NORMAL_TEXT);
}

/*
 desc : GridControl 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgCtrl::InitGrid()
{
	InitMotionIndex();
	int nGridIndex = eGRD_MOTOR_LIST;
	CRect rtStt;
	CRect rtGrd;
	CRect rtBtn;
	CString strText;
	m_pStt[EN_CTRL_STT::MOTOR_STATUS]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	int nRowHeight		= rtStt.Height();
	rtGrd				= rtStt;
	rtGrd.top			= rtStt.bottom;
	rtGrd.bottom		= rtGrd.top + nRowHeight * 10;

	m_pGrd[nGridIndex] = new CGridCtrl();
	m_pGrd[nGridIndex]->Create(rtGrd, this, DEF_UI_CTRL_GRD + nGridIndex, (WS_CHILD | WS_TABSTOP | WS_VISIBLE));

	//////////////////////////////////////////////////////////////////////////
	// IO STATUS
	nGridIndex = eGRD_IO_LIST;
	m_pStt[EN_CTRL_STT::IO_STATUS]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	nRowHeight		= rtStt.Height() / 2;
	rtGrd			= rtStt;
	rtGrd.top		= rtStt.bottom;
	rtGrd.bottom	= rtGrd.top + nRowHeight * 33;

	m_pGrd[nGridIndex] = new CGridCtrl();
	m_pGrd[nGridIndex]->Create(rtGrd, this, DEF_UI_CTRL_GRD + nGridIndex, (WS_CHILD | WS_TABSTOP | WS_VISIBLE));

	UINT uiBtnWidth = (rtStt.Width() - DEF_UI_OFFSET * 2) / 3;
	UINT uiBtnHeight = rtStt.Height();

	rtBtn.left = rtGrd.left;
	rtBtn.top = rtGrd.bottom + DEF_UI_OFFSET;
	rtBtn.right = rtGrd.left + uiBtnWidth;
	rtBtn.bottom = rtBtn.top + uiBtnHeight;

	//////////////////////////////////////////////////////////////////////////
	// PREV/UPDATE/NEXT BTN
	int nBtnIndex = EN_CTRL_BTN::PREV;
	for (nBtnIndex = EN_CTRL_BTN::PREV; nBtnIndex <= EN_CTRL_BTN::NEXT; nBtnIndex++)
	{
		if (EN_CTRL_BTN::PREV < nBtnIndex)
		{
			rtBtn.OffsetRect(uiBtnWidth + DEF_UI_OFFSET, 0);
		}
		m_pBtn[nBtnIndex] = new CMacButton();
		strText = (CString)EN_CTRL_BTN::_from_index(nBtnIndex)._to_string();
		strText.Replace(_T("_"), _T(" "));
		m_pBtn[nBtnIndex]->Create(strText, WS_BORDER | WS_VISIBLE, rtBtn, this, DEF_UI_CTRL_BTN + nBtnIndex);
		m_pBtn[nBtnIndex]->SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_pBtn[nBtnIndex]->SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
		m_pBtn[nBtnIndex]->SetTextColor(DEF_COLOR_BTN_PAGE_NORMAL_TEXT);
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// IO QUICK
	nGridIndex = eGRD_IO_QUICK1;
	m_pStt[EN_CTRL_STT::IO_QUICK]->GetWindowRect(rtStt);
	this->ScreenToClient(rtStt);

	rtGrd = rtStt;
	for (int i = nGridIndex; i < eGRD_MAX; i++)
	{
		nRowHeight = rtStt.Height();
		rtGrd.top = rtGrd.bottom;
		rtGrd.bottom = rtGrd.top + nRowHeight * 2;

		m_pGrd[i] = new CGridCtrl();
		m_pGrd[i]->Create(rtGrd, this, DEF_UI_CTRL_GRD + i, (WS_CHILD | WS_TABSTOP | WS_VISIBLE));
	}
	//////////////////////////////////////////////////////////////////////////

	return TRUE;
}

VOID CDlgCtrl::InitMotionIndex()
{
	ST_MOTION stTemp;		/* 모터 임시 변수 */

	m_vMotor.clear();

	// Stage X, Y 항목 저장
	for (int i = 0; i < m_u8StageCount; i++)
	{
		stTemp.strMotorName.Format(_T("STAGE%d X"), i + 1);
		stTemp.DeviceNum = ENG_MMDI(i * 2);
		m_vMotor.push_back(stTemp);

		stTemp.strMotorName.Format(_T("STAGE%d Y"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((i * 2) + 1);
		m_vMotor.push_back(stTemp);
	}

	// Align Camera X 항목 저장
	for (int i = 0; i < m_u8ACamCount; i++)
	{
		stTemp.strMotorName.Format(_T("CAM%d X"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((int)ENG_MMDI::en_align_cam1 + i);
		m_vMotor.push_back(stTemp);
	}

	// Photo Head Z 항목 저장
	for (int i = 0; i < m_u8HeadCount; i++)
	{
		stTemp.strMotorName.Format(_T("PH%d Z"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((int)ENG_MMDI::en_axis_ph1 + i);
		m_vMotor.push_back(stTemp);
	}

	// Align Camera Z 항목 저장
	for (int i = 0; i < m_u8ACamCount; i++)
	{
		stTemp.strMotorName.Format(_T("CAM%d Z"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((int)ENG_MMDI::en_axis_acam1 + i);
		m_vMotor.push_back(stTemp);
	}
}

/*
 desc : Motor Grid를 초기화한다.
 parm : None
 retn : None
*/
VOID CDlgCtrl::InitMotorGrid()
{
	CRect rctSize;					/* 작업 영역 */

	double dCellRatio[EN_MOTOR_STATUS::_size()] = { 0.2, 0.2, 0.2, 0.2, 0.2 };	/* 그리드 비율 */
	int nHeightSize = 0;	/* 셀 높이 */
	int	nWidthDiffer = 0;	/* 셀 너비 오차값 */

	UINT nCenterAlignText = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;	/* 그리드 옵션 */

	// 그리드 기본 설정
	m_pGrd[eGRD_MOTOR_LIST]->GetClientRect(rctSize);
	m_pGrd[eGRD_MOTOR_LIST]->SetColumnResize(FALSE);
	m_pGrd[eGRD_MOTOR_LIST]->SetRowResize(FALSE);
	m_pGrd[eGRD_MOTOR_LIST]->SetEditable(FALSE);
	m_pGrd[eGRD_MOTOR_LIST]->EnableSelection(FALSE);
	m_pGrd[eGRD_MOTOR_LIST]->ModifyStyle(WS_HSCROLL, 0);

	m_pGrd[eGRD_MOTOR_LIST]->SetGridLineColor(LIGHT_GRAY);
	m_pGrd[eGRD_MOTOR_LIST]->SetTextColor(BLACK_);

	m_pGrd[eGRD_MOTOR_LIST]->DeleteAllItems();

	nHeightSize = rctSize.Height() / (m_u8AllMotorCount + 1);

	// 그리드 영역이 작업 영역을 초과하면 스크롤 바 만큼의 오차값 저장
	if (rctSize.Height() - 1 < nHeightSize * m_u8AllMotorCount)
	{
		nWidthDiffer = ::GetSystemMetrics(SM_CXVSCROLL);
	}

	// 셀의 개수 지정
	m_pGrd[eGRD_MOTOR_LIST]->SetColumnCount((int)EN_MOTOR_STATUS::_size());
	m_pGrd[eGRD_MOTOR_LIST]->SetRowCount(m_u8AllMotorCount + 1);

	CString strText;
	// 높이, 너비, 색상, 문자 적용
	for (int nCol = 0; nCol < (int)EN_MOTOR_STATUS::_size(); nCol++)
	{
		m_pGrd[eGRD_MOTOR_LIST]->SetColumnWidth(nCol, (int)((rctSize.Width() - nWidthDiffer) * dCellRatio[nCol]));
		strText = (CString)EN_MOTOR_STATUS::_from_index(nCol)._to_string();
		strText.Replace(_T("_"), _T(" "));
		m_pGrd[eGRD_MOTOR_LIST]->SetItemText(0, nCol, strText);
		m_pGrd[eGRD_MOTOR_LIST]->SetItemBkColour(0, nCol, DEF_COLOR_BTN_MENU_NORMAL);
		m_pGrd[eGRD_MOTOR_LIST]->SetItemFgColour(0, nCol, DEF_COLOR_BTN_MENU_NORMAL_TEXT);

		for (int nRow = 0; nRow < m_u8AllMotorCount + 1; nRow++)
		{
			m_pGrd[eGRD_MOTOR_LIST]->SetRowHeight(nRow, nHeightSize);
			m_pGrd[eGRD_MOTOR_LIST]->SetItemFormat(nRow, nCol, nCenterAlignText);

			if (0 < nRow)
			{
				if (EN_MOTOR_STATUS::STATUS == nCol || EN_MOTOR_STATUS::SERVO == nCol || EN_MOTOR_STATUS::HOME == nCol)
				{
					m_pGrd[eGRD_MOTOR_LIST]->SetItemBkColour(nRow, nCol, DEF_COLOR_BTN_PAGE_NORMAL);
					m_pGrd[eGRD_MOTOR_LIST]->SetItemFgColour(nRow, nCol, DEF_COLOR_BTN_PAGE_NORMAL_TEXT);
				}
				else
				{
					m_pGrd[eGRD_MOTOR_LIST]->SetItemBkColour(nRow, nCol, WHITE_);
				}
			}
		}
	}

	m_pGrd[eGRD_MOTOR_LIST]->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	m_pGrd[eGRD_MOTOR_LIST]->Refresh();
}

VOID CDlgCtrl::UpdateMotorStatus()
{
	double dPosition = 0.;
	CString strPosition;

	for (int nRow = 0; nRow < m_u8AllMotorCount; nRow++)
	{
		
		m_pGrd[eGRD_MOTOR_LIST]->SetItemText(nRow + 1, EN_MOTOR_STATUS::MOTOR_NAME, m_vMotor.at(nRow).strMotorName);

		dPosition = uvCmn_MC2_GetDrvAbsPos(m_vMotor[nRow].DeviceNum);
		strPosition.Format(_T("%.4f [mm]"), dPosition);
		m_pGrd[eGRD_MOTOR_LIST]->SetItemText(nRow + 1, EN_MOTOR_STATUS::POS, strPosition);

		if (FALSE == uvCmn_MC2_IsDriveMoving(m_vMotor[nRow].DeviceNum))
		{
			m_pGrd[eGRD_MOTOR_LIST]->SetItemBkColour(nRow + 1, EN_MOTOR_STATUS::POS, WHITE_);
			m_pGrd[eGRD_MOTOR_LIST]->SetItemBkColour(nRow + 1, EN_MOTOR_STATUS::MOTOR_NAME, WHITE_);
		}
		else
		{
			m_pGrd[eGRD_MOTOR_LIST]->SetItemBkColour(nRow + 1, EN_MOTOR_STATUS::POS, SEA_GREEN);
			m_pGrd[eGRD_MOTOR_LIST]->SetItemBkColour(nRow + 1, EN_MOTOR_STATUS::MOTOR_NAME, SEA_GREEN);
		}

		if (FALSE == uvCmn_MC2_IsDevLocked(m_vMotor[nRow].DeviceNum))
		{
			m_pGrd[eGRD_MOTOR_LIST]->SetItemBkColour(nRow + 1, EN_MOTOR_STATUS::SERVO, DEF_COLOR_BTN_PAGE_NORMAL);
			m_pGrd[eGRD_MOTOR_LIST]->SetItemText(nRow + 1, EN_MOTOR_STATUS::SERVO, _T("SERVO OFF"));
		}
		else
		{
			m_pGrd[eGRD_MOTOR_LIST]->SetItemBkColour(nRow + 1, EN_MOTOR_STATUS::SERVO, SEA_GREEN);
			m_pGrd[eGRD_MOTOR_LIST]->SetItemText(nRow + 1, EN_MOTOR_STATUS::SERVO, _T("SERVO ON"));
		}

		// 에러 상태 확인
		if (TRUE == uvCmn_MC2_IsDriveError(m_vMotor[nRow].DeviceNum))
		{
			int nErrorCode = uvCmn_MC2_GetDriveError(m_vMotor[nRow].DeviceNum);
			m_pGrd[eGRD_MOTOR_LIST]->SetItemBkColour(nRow + 1, EN_MOTOR_STATUS::STATUS, TOMATO);
			m_pGrd[eGRD_MOTOR_LIST]->SetItemTextFmt(nRow + 1, EN_MOTOR_STATUS::STATUS, _T("ERR-%d RESET"), nErrorCode);
		}
		// 동작이 가능한 상태
		else
		{
			m_pGrd[eGRD_MOTOR_LIST]->SetItemBkColour(nRow + 1, EN_MOTOR_STATUS::STATUS, SEA_GREEN);
			m_pGrd[eGRD_MOTOR_LIST]->SetItemText(nRow + 1, EN_MOTOR_STATUS::STATUS, _T("OK"));
		}

		if (TRUE == uvCmn_MC2_IsDriveHomed(m_vMotor[nRow].DeviceNum))
		{
			int nErrorCode = uvCmn_MC2_GetDriveError(m_vMotor[nRow].DeviceNum);
			m_pGrd[eGRD_MOTOR_LIST]->SetItemBkColour(nRow + 1, EN_MOTOR_STATUS::HOME, SEA_GREEN);
			m_pGrd[eGRD_MOTOR_LIST]->SetItemText(nRow + 1, EN_MOTOR_STATUS::HOME, _T("HOME COMP"));
		}
		// 동작이 가능한 상태
		else
		{
			m_pGrd[eGRD_MOTOR_LIST]->SetItemBkColour(nRow + 1, EN_MOTOR_STATUS::HOME, DEF_COLOR_BTN_PAGE_NORMAL);
			m_pGrd[eGRD_MOTOR_LIST]->SetItemText(nRow + 1, EN_MOTOR_STATUS::HOME, _T("DO HOME"));
		}
	}

	// 화면 갱신
	m_pGrd[eGRD_MOTOR_LIST]->Refresh();
}

VOID CDlgCtrl::InitIOGrid()
{
	CRect rctSize;					/* 작업 영역 */

	double dCellRatio[EN_IO_STATUS::_size()] = { 0.2, 0.6, 0.2, };	/* 그리드 비율 */
	int nHeightSize = 0;	/* 셀 높이 */
	int	nWidthDiffer = 0;	/* 셀 너비 오차값 */
	int	nIOCount = 32;

	UINT nCenterAlignText = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;	/* 그리드 옵션 */

	// 그리드 기본 설정
	m_pGrd[eGRD_IO_LIST]->GetClientRect(rctSize);
	m_pGrd[eGRD_IO_LIST]->SetColumnResize(FALSE);
	m_pGrd[eGRD_IO_LIST]->SetRowResize(FALSE);
	m_pGrd[eGRD_IO_LIST]->SetEditable(FALSE);
	m_pGrd[eGRD_IO_LIST]->EnableSelection(FALSE);
	m_pGrd[eGRD_IO_LIST]->ModifyStyle(WS_HSCROLL, 0);

	m_pGrd[eGRD_IO_LIST]->SetGridLineColor(LIGHT_GRAY);
	m_pGrd[eGRD_IO_LIST]->SetTextColor(BLACK_);

	m_pGrd[eGRD_IO_LIST]->DeleteAllItems();

	nHeightSize = rctSize.Height() / (nIOCount + 1);

	// 그리드 영역이 작업 영역을 초과하면 스크롤 바 만큼의 오차값 저장
	if (rctSize.Height() - 1 < nHeightSize * nIOCount)
	{
		nWidthDiffer = ::GetSystemMetrics(SM_CXVSCROLL);
	}

	// 셀의 개수 지정
	m_pGrd[eGRD_IO_LIST]->SetColumnCount((int)EN_IO_STATUS::_size());
	m_pGrd[eGRD_IO_LIST]->SetRowCount(nIOCount + 1);

	CString strText;
	// 높이, 너비, 색상, 문자 적용
	for (int nCol = 0; nCol < (int)EN_IO_STATUS::_size(); nCol++)
	{
		m_pGrd[eGRD_IO_LIST]->SetColumnWidth(nCol, (int)((rctSize.Width() - nWidthDiffer) * dCellRatio[nCol]));
		strText = (CString)EN_IO_STATUS::_from_index(nCol)._to_string();
		strText.Replace(_T("_"), _T(" "));
		m_pGrd[eGRD_IO_LIST]->SetItemText(0, nCol, strText);
		m_pGrd[eGRD_IO_LIST]->SetItemBkColour(0, nCol, DEF_COLOR_BTN_MENU_NORMAL);
		m_pGrd[eGRD_IO_LIST]->SetItemFgColour(0, nCol, DEF_COLOR_BTN_MENU_NORMAL_TEXT);

		for (int nRow = 0; nRow < nIOCount + 1; nRow++)
		{
			m_pGrd[eGRD_IO_LIST]->SetRowHeight(nRow, nHeightSize);
			m_pGrd[eGRD_IO_LIST]->SetItemFormat(nRow, nCol, nCenterAlignText);
		}
	}

	m_pGrd[eGRD_IO_LIST]->SetFont(&g_font[eFONT_LEVEL2_BOLD]);
	m_pGrd[eGRD_IO_LIST]->Refresh();
}

VOID CDlgCtrl::UpdateIOStatus()
{
	CIOManager::GetInstance()->UpdateIO();

	int nIOCount = 32;
	int nIOIndex = m_nPage * nIOCount;
	EN_IO_TYPE eType = eIO_INPUT;

	if (nIOIndex >= CIOManager::GetInstance()->GetInputCount())
	{
		eType		= eIO_OUTPUT;
		nIOIndex	-= CIOManager::GetInstance()->GetInputCount();

		if (nIOIndex >= CIOManager::GetInstance()->GetOutputCount())
		{
			return;
		}
	}

	vST_IO vIOList;
	if (0 == CIOManager::GetInstance()->GetIOList(eType, vIOList))
	{
		return;
	}

	for (int nIndex = 0; nIndex < nIOCount; nIndex++)
	{
		if (nIOIndex < vIOList.size())
		{
			m_pGrd[eGRD_IO_LIST]->SetItemText(nIndex + 1, EN_IO_STATUS::ADDR, vIOList.at(nIOIndex).strAddr.c_str());
			m_pGrd[eGRD_IO_LIST]->SetItemText(nIndex + 1, EN_IO_STATUS::NAME, vIOList.at(nIOIndex).strName.c_str());

			if (vIOList.at(nIOIndex).bOn)
			{
				m_pGrd[eGRD_IO_LIST]->SetItemText(nIndex + 1, EN_IO_STATUS::STATUS, vIOList.at(nIOIndex).strOn.c_str());
				m_pGrd[eGRD_IO_LIST]->SetItemBkColour(nIndex + 1, EN_IO_STATUS::STATUS, GetIOColorBg(vIOList.at(nIOIndex).strClrOn.c_str()));
				m_pGrd[eGRD_IO_LIST]->SetItemFgColour(nIndex + 1, EN_IO_STATUS::STATUS, GetIOColorFg(vIOList.at(nIOIndex).strClrOn.c_str()));
			}
			else
			{
				m_pGrd[eGRD_IO_LIST]->SetItemText(nIndex + 1, EN_IO_STATUS::STATUS, vIOList.at(nIOIndex).strOff.c_str());
				m_pGrd[eGRD_IO_LIST]->SetItemBkColour(nIndex + 1, EN_IO_STATUS::STATUS, GetIOColorBg(vIOList.at(nIOIndex).strClrOff.c_str()));
				m_pGrd[eGRD_IO_LIST]->SetItemFgColour(nIndex + 1, EN_IO_STATUS::STATUS, GetIOColorFg(vIOList.at(nIOIndex).strClrOff.c_str()));
			}

			nIOIndex++;
		}
		else
		{
			m_pGrd[eGRD_IO_LIST]->SetItemText(nIndex + 1, EN_IO_STATUS::ADDR, _T(""));
			m_pGrd[eGRD_IO_LIST]->SetItemBkColour(nIndex + 1, EN_IO_STATUS::ADDR, WHITE_);
			m_pGrd[eGRD_IO_LIST]->SetItemFgColour(nIndex + 1, EN_IO_STATUS::ADDR, BLACK_);
			m_pGrd[eGRD_IO_LIST]->SetItemText(nIndex + 1, EN_IO_STATUS::NAME, _T(""));
			m_pGrd[eGRD_IO_LIST]->SetItemBkColour(nIndex + 1, EN_IO_STATUS::NAME, WHITE_);
			m_pGrd[eGRD_IO_LIST]->SetItemFgColour(nIndex + 1, EN_IO_STATUS::NAME, BLACK_);
			m_pGrd[eGRD_IO_LIST]->SetItemText(nIndex + 1, EN_IO_STATUS::STATUS, _T(""));
			m_pGrd[eGRD_IO_LIST]->SetItemBkColour(nIndex + 1, EN_IO_STATUS::STATUS, WHITE_);
			m_pGrd[eGRD_IO_LIST]->SetItemFgColour(nIndex + 1, EN_IO_STATUS::STATUS, BLACK_);
		}
	}

	vIOList.clear();
	/* 화면 갱신 활성화 */
	m_pGrd[eGRD_IO_LIST]->Refresh();

	CString strPage;
	strPage.Format(_T("UPDATE[%d/%d]"), m_nPage + 1, m_nMaxPage);
	m_pBtn[EN_CTRL_BTN::UPDATE]->SetWindowTextW(strPage);
}

VOID CDlgCtrl::InitQuickIOGrid()
{
	CRect rctSize;						/* 작업 영역 */
	CRect rctOldSize;					/* 작업 영역 */

	double dCellRatio[EN_IO_QUICK::_size()] = { 0.5, 0.5 };	/* 그리드 비율 */
	int nHeightSize		= 25;	/* 셀 높이 */
	int	nWidthDiffer	= 1;	/* 셀 너비 오차값 */
	
	UINT nCenterAlignText = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;	/* 그리드 옵션 */

	// 그리드 기본 설정
	int	nGridIndex = eGRD_IO_QUICK1;

	m_pStt[EN_CTRL_STT::IO_QUICK]->GetWindowRect(rctOldSize);
	this->ScreenToClient(rctOldSize);

	for (const auto& io : m_vQuickIO2)
	{
		if (nGridIndex >= eGRD_MAX)
		{
			break;
		}

		int nRowCount = 2;

		rctSize = rctOldSize;
		rctSize.top = rctOldSize.bottom + DEF_UI_OFFSET;
		rctSize.bottom = rctSize.top + (nRowCount * nHeightSize) + 1;
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

VOID CDlgCtrl::UpdateQuickIOStatus()
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

COLORREF CDlgCtrl::GetIOColorBg(CString strColor)
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

COLORREF CDlgCtrl::GetIOColorFg(CString strColor)
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

/*
 desc : GridControl 해제
 parm : None
 retn : None
*/
VOID CDlgCtrl::CloseGrid()
{

}

/*
 desc : 초기 값 설정
 parm : None
 retn : None
*/
VOID CDlgCtrl::InitValue()
{

}

/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgCtrl::OnBtnClick(UINT32 id)
{
	CString strMsg;
	UINT nID = id - DEF_UI_CTRL_BTN;
	switch (nID)
	{
	case EN_CTRL_BTN::ALL_STOP:		
		uvEng_MC2_SendDevStoppedAll();
		break;
	case EN_CTRL_BTN::ALL_HOMING:	
		strMsg.Format(_T("Do you want to all motor homming?"));
		if (IDYES == AfxMessageBox(strMsg, MB_YESNO))
		{
			//SetMotionHoming(ENG_MMDI::en_all);
			m_pDlgMain->RunWorkJob(ENG_BWOK::en_work_init);
		}
		
		break;
	case EN_CTRL_BTN::ALL_RESET:		
		strMsg.Format(_T("Do you want to all motor reset?"));
		if (IDYES == AfxMessageBox(strMsg, MB_YESNO))
		{
			uvEng_MC2_SendDevFaultResetAll();
		}
		
		break;
	case EN_CTRL_BTN::OPEN_CONTROL_PANEL:	
		ShowCtrlPanel();
		break;
	case EN_CTRL_BTN::RESET_TRIGGER:
		strMsg.Format(_T("Do you want to trigger board reset?"));
		if (IDYES == AfxMessageBox(strMsg, MB_YESNO))
		{
			SetTriggerReset();
		}
		break;
	case EN_CTRL_BTN::HEAD_HWINIT:
		strMsg.Format(_T("Do you want to Head HW Init?"));
		if (IDYES == AfxMessageBox(strMsg, MB_YESNO))
		{
			//uvEng_Luria_ReqSetHWInit();
			SetHeadHWInit();
		}
		break;
	case EN_CTRL_BTN::STROBE_LAMP:
		StrobeLampSampleFunction();
		break;
	case EN_CTRL_BTN::PREV:		
		if (m_nPage > 0)
		{
			m_nPage--;
		}
		UpdateIOStatus();
		UpdateQuickIOStatus();
		break;
	case EN_CTRL_BTN::UPDATE:		
		UpdateIOStatus();
		UpdateQuickIOStatus();
		break;
	case EN_CTRL_BTN::NEXT:		
		if (m_nPage < m_nMaxPage - 1)
		{
			m_nPage++;
		}
		UpdateIOStatus();
		UpdateQuickIOStatus();
		break;
	}				  
}

void CDlgCtrl::OnGrdClick(UINT ID, NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	int nCommand = ID - DEF_UI_CTRL_GRD;

	switch (nCommand)
	{
	case eGRD_MOTOR_LIST:
		OnGrdMotorList(pNotifyStruct);
		break;
	case eGRD_IO_QUICK1:
	case eGRD_IO_QUICK2:
	case eGRD_IO_QUICK3:
	case eGRD_IO_QUICK4:
	case eGRD_IO_QUICK5:
	case eGRD_IO_QUICK6:
	case eGRD_IO_QUICK7:
	case eGRD_IO_QUICK8:
	case eGRD_IO_QUICK9:
	case eGRD_IO_QUICK10:
		OnGrdIOQuick(nCommand, pNotifyStruct);
		break;
	default:
		break;
	}
}

void CDlgCtrl::OnGrdMotorList(NMHDR* pNotifyStruct)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	if (0 == pItem->iRow)
	{
		return;
	}

	int nIndex = pItem->iRow - 1;

	if (nIndex >= (int)m_vMotor.size())
	{
		return;
	}

	CString strMsg;
	switch (pItem->iColumn)
	{
	case EN_MOTOR_STATUS::SERVO:
	{
		BOOL bOnOff = uvCmn_MC2_IsDevLocked(m_vMotor.at(nIndex).DeviceNum);

		if (bOnOff)
		{
			strMsg.Format(_T("Do you want to motor servo off?"));
		}
		else
		{
			strMsg.Format(_T("Do you want to motor servo on?"));
		}

		if (IDYES == AfxMessageBox(strMsg, MB_YESNO))
		{
			uvEng_MC2_SendDevLocked(m_vMotor.at(nIndex).DeviceNum, !bOnOff); break;
		}
	}
		break;
	case EN_MOTOR_STATUS::STATUS:
	{
		if (TRUE == uvCmn_MC2_IsDriveError(m_vMotor.at(nIndex).DeviceNum))
		{
			strMsg.Format(_T("Do you want to motor error reset?"));

			if (IDYES == AfxMessageBox(strMsg, MB_YESNO))
			{
				uvEng_MC2_SendDevFaultReset(m_vMotor.at(nIndex).DeviceNum);
			}
		}
	}
		break;
	case EN_MOTOR_STATUS::HOME:
	{
		strMsg.Format(_T("Do you want to motor homming?"));

		if (IDYES == AfxMessageBox(strMsg, MB_YESNO))
		{
			SetMotionHoming(m_vMotor.at(nIndex).DeviceNum);
		}
	}
		break;
	default:
		break;
	}
}

void CDlgCtrl::OnGrdIOQuick(int nGrdIndex, NMHDR* pNotifyStruct)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	if (1 != pItem->iRow)
	{
		return;
	}

	int nIndex = nGrdIndex - eGRD_IO_QUICK1;

	if (nIndex >= (int)m_vQuickIO2.size())
	{
		return;
	}

	ST_QUICK_IO_ACT stIO = m_vQuickIO2.at(nIndex);

	CString strMsg;

		if (FALSE == stIO.bOn)
		{
			strMsg.Format(_T("Do you want to change to %s : ON?"), stIO.strName.c_str());
			if (IDNO == AfxMessageBox(strMsg, MB_YESNO))
			{
				return;
			}
		}
		else
		{
			strMsg.Format(_T("Do you want to change to %s : OFF?"), stIO.strName.c_str());
			if (IDNO == AfxMessageBox(strMsg, MB_YESNO))
			{
				return;
			}
		}

	CIOManager::GetInstance()->SetQuickIOThread(nIndex);
// 	if (CIOManager::GetInstance()->SetQuickIOThread(nIndex))
// 	{
// 		UpdateQuickIOStatus();
// 	}
}

/*
 desc : Motion Homing
 parm : drv_id	- [in]  Drive ID
 retn : None
*/
VOID CDlgCtrl::SetMotionHoming(ENG_MMDI drv_id)
{
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	if (CInterLockManager::GetInstance()->CheckMoveInterlock(ENG_MMDI::en_stage_y, 0))
	{
		AfxMessageBox(CInterLockManager::GetInstance()->GetLastError());
	}
	else
	{
		uvEng_MC2_SendDevHoming(drv_id);
	}
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	uvEng_MC2_SendDevHoming(drv_id);
#endif
}

/*
 desc : Trigger Power Reset
 parm : None
 retn : None
*/
VOID CDlgCtrl::SetTriggerReset()
{
	//ST_IO stIO;
	//int IOindex= 51;
	//CIOManager::GetInstance()->GetIO(EN_IO_TYPE::eIO_OUTPUT, IOindex, stIO);
	uvEng_Mvenc_ReqTriggerStrobe(FALSE);
}

/*
 desc : 포토헤드 HW 초기화
 parm : None
 retn : None
*/
VOID CDlgCtrl::SetHeadHWInit()
{
	UINT8 LastRecipe;
	/* Job Name 삭제 전에, 등록된 Job 개수 임시 저장 */
	LastRecipe = uvCmn_Luria_GetJobCount();

	/* Job Delete 요청 */
	if (LastRecipe && !uvEng_Luria_ReqSetDeleteSelectedJob())
	{
		LOG_ERROR(ENG_EDIC::en_uvdi15, L"Failed to send the cmd (ReqSetDeleteSelectedJob)");
	}

	uvEng_Luria_ReqSetHWInit();
}

void CDlgCtrl::ShowCtrlPanel()
{
	if (AfxGetMainWnd()->GetSafeHwnd())
	{
		::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), eMSG_MAIN_OPEN_CONSOLE, NULL, NULL);
	}
}

void CDlgCtrl::StrobeLampSampleFunction()
{
	uint8_t  page = 0;				//	Spec ( 0 ~ 7	)
	uint8_t  channel = 0;			//	Spec ( 0 ~ 7	)
	uint16_t delayValue = 4000;		//	Spec ( 0 ~ 6000 )		10진수로 입력하시면 됩니다. ( 라이브러리 내부에서 16진수로 변환되어 전송됩니다. )
	uint16_t strobeValue = 500;		//	Spec ( 0 ~ 1000 )		10진수로 입력하시면 됩니다. ( 라이브러리 내부에서 16진수로 변환되어 전송됩니다. )
	uint8_t  mode = 0;				//	Spec ( 0 ~ 5	)

	//	Ex Send 1. 
//	uvEng_StrobeLamp_Send_ChannelDelayControl(page, channel, delayValue);
 
// 	//	Ex Send 2. 
 	uvEng_StrobeLamp_Send_ChannelStrobeControl(page, channel, strobeValue);
// 
// 	//	Ex Send 3. 
// 	uvEng_StrobeLamp_Send_ChannelWrite(page, channel, delayValue, strobeValue);
// 
// 	//	Ex Send 4. 
// 	uvEng_StrobeLamp_Send_PageDataReadRequest(page);
// 
// 	//	Ex Send 5. 
// 	uvEng_StrobeLamp_Send_PageLoopDataRead(page);
// 
// 	//	Ex Send 6. 
// 	uvEng_StrobeLamp_Send_StrobeMode(mode);


	////////////////////////////////////////////////////////////////////////////
	// 
	//	컨트롤러를 통해 받은 응답 데이터는
	//	LRESULT CDlgMain::OnMsgMainStrobeLamp(WPARAM wparam, LPARAM lparam)
	//	에서 확인 하실 수 있습니다.
	// 
	////////////////////////////////////////////////////////////////////////////
}
