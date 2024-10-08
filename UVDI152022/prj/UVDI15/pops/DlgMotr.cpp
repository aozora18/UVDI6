
/*
 desc : 프로그램 종료될 때, 대기 알림
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgMotr.h"
#include "../mesg/DlgMesg.h"
#include "../param/InterLockManager.h"
#include <afxtaskdialog.h>


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

IMPLEMENT_DYNAMIC(CDlgMotr, CDialogEx)

/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		pParent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgMotr::CDlgMotr(CWnd* pParent /*=NULL*/)
	: CMyDialog(CDlgMotr::IDD, pParent)
{
	for (int i = 0; i < eGRD_MAX; i++)
	{
		m_pGrid[i] = NULL;
	}
	for (int i = 0; i < eBTN_MAX; i++)
	{
		m_pButton[i] = NULL;
	}

	m_u8ACamCount = uvEng_GetConfig()->set_cams.acam_count;		/* 카메라 개수를 가져온다. */
	m_u8HeadCount = uvEng_GetConfig()->luria_svc.ph_count;		/* Photo Head 개수를 가져온다. */
	m_u8StageCount = uvEng_GetConfig()->luria_svc.table_count;	/* Table의 개수를 가져온다. */

	// Stage 개수(x, y축) + Head 개수 + Align Camera 개수
	m_u8AllMotorCount = (m_u8StageCount * 2) + m_u8HeadCount + (m_u8ACamCount * 2);
	
	/* 멤버 변수 초기화 */
	m_u8SelMotor = 0;
	m_stVctMotion.clear();
	m_bMoveType = eCELL_TAB_ABSOLUTE_MOVE;
	m_dSetSpeed = 0.;
	m_dSetPosition = 0.;
}

/*
 desc : 소멸자
 parm : None
 retn : None
*/
CDlgMotr::~CDlgMotr()
{
	m_stVctMotion.clear();
	m_stVctMotion.shrink_to_fit();

	for (int i = 0; i < eGRD_MAX; i++)
	{
		if (NULL != m_pGrid[i])
		{
			delete m_pGrid[i];
		}
	}

	for (int i = 0; i < eBTN_MAX; i++)
	{
		if (NULL != m_pButton[i])
		{
			delete m_pButton[i];
		}
	}
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgMotr::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	for (int i = 0; i < eTITLE_MAX; i++)	DDX_Control(pDX, IDC_TITLE_MOTOR + i, m_sttTitle[i]);
}

BEGIN_MESSAGE_MAP(CDlgMotr, CMyDialog)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_WM_SYSCOMMAND()
	ON_COMMAND_RANGE(IDC_BTN_PLUS, IDC_BTN_PLUS + eBTN_MAX, OnClickButtonEvent)
	ON_NOTIFY_RANGE(NM_CLICK, IDC_GRD_MOTOR, IDC_GRD_MOTOR + eGRD_MAX, OnGrdClickedEvent)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgMotr::PreTranslateMessage(MSG* msg)
{
	if (msg->message == WM_KEYDOWN)
	{
		if (msg->wParam == VK_RETURN || msg->wParam == VK_ESCAPE)
		{
			return TRUE;                // Do not process further
		}
	}

	return CDialogEx::PreTranslateMessage(msg);
}

#pragma region Initalize
/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMotr::OnInitDlg()
{
	/* TOP_MOST & Center */
	// CenterParentTopMost();
	CDialogEx::OnInitDialog();

	/* 모터 정보 초기화 */
	InitMotionIndex();

	/* 컨트롤 생성 */
	CreateControl();

	/* Grid 초기화 */
	InitMotorGrid();
	InitOpTabGrid();
	InitOpInputGrid();
	InitControlGrid();

	return TRUE;
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgMotr::OnPaintDlg(CDC * pDc)
{
}

/*
 desc : 각 컨트롤들을 생성한다.
 parm : None
 retn : None
*/
VOID CDlgMotr::CreateControl()
{
	CRect rctDlgSize;						/* 작업 영역 */
	CRect rctTitleSize[eTITLE_MAX];			/* 타이틀 바 영역 */
	CRect rctGrdSize[eGRD_MAX];				/* 그리드 영역 */
	CRect rctBtnSize[eBTN_MAX];				/* 버튼 영역 */
	CPoint ptErrorSize;						/* 윈도우에서의 위치와 작업 영역과의 오차 좌표 */
	LONG lBottomBlockSize = 0;				/* Operation 영역에서 1Cell의 비율 값 */

	LOGFONT	lfBold = GetLogFont(28, TRUE);	/* 폰트 생성 */

	CString strArrCaption[eBTN_MAX] = { _T("+"), _T("-"), _T("STOP") };	/* 버튼에 사용될 문자 */

	// 전체 작업 영역을 가져온다.
	GetClientRect(rctDlgSize);

	// 타이틀 바 사이즈를 기준으로 그리드의 비율을 정할 것
	for (int nTitleNum = 0; nTitleNum < eTITLE_MAX; nTitleNum++)
	{
		// 타이틀 바의 사이즈를 가져온다.
		GetDlgItem(IDC_TITLE_MOTOR + nTitleNum)->GetWindowRect(rctTitleSize[nTitleNum]);

		// 첫번째 타이틀 바의 영역을 가져온다면 Window에서의 위치와 작업 위치의 오차값을 저장한다.
		if (eTITLE_MOTOR == nTitleNum)
		{
			ptErrorSize.x = rctTitleSize[nTitleNum].left;
			ptErrorSize.y = rctTitleSize[nTitleNum].top;
		}

		// 오차 값을 반영하여 저장한다.
		rctTitleSize[nTitleNum].left -= ptErrorSize.x;
		rctTitleSize[nTitleNum].top -= ptErrorSize.y;
		rctTitleSize[nTitleNum].right -= ptErrorSize.x;
		rctTitleSize[nTitleNum].bottom -= ptErrorSize.y;

		m_sttTitle[nTitleNum].SetTextFont(&lfBold);
		m_sttTitle[nTitleNum].SetDrawBg(TRUE);
		m_sttTitle[nTitleNum].SetBgColor(DEF_COLOR_BTN_MENU_NORMAL);
		m_sttTitle[nTitleNum].SetTextColor(DEF_COLOR_BTN_MENU_NORMAL_TEXT);
	}

	// Motor 항목 좌표 값
	rctGrdSize[eGRD_MOTOR].left = rctDlgSize.left + 1;
	rctGrdSize[eGRD_MOTOR].top = rctTitleSize[eTITLE_MOTOR].bottom;
	rctGrdSize[eGRD_MOTOR].right = rctTitleSize[eTITLE_CONTROL].left;
	rctGrdSize[eGRD_MOTOR].bottom = rctDlgSize.bottom;

	// Control 항목 좌표 값
	rctGrdSize[eGRD_CONTROL].left = rctTitleSize[eTITLE_CONTROL].left;
	rctGrdSize[eGRD_CONTROL].top = rctTitleSize[eTITLE_CONTROL].bottom;
	rctGrdSize[eGRD_CONTROL].right = rctDlgSize.right;
	rctGrdSize[eGRD_CONTROL].bottom = rctTitleSize[eTITLE_OPERATION].top;

	// Operation 항목 좌표 값
	lBottomBlockSize = (rctDlgSize.bottom - rctTitleSize[eTITLE_OPERATION].bottom) / 5;

	rctGrdSize[eGRD_OPERATION_TAB].left = rctTitleSize[eTITLE_CONTROL].left;
	rctGrdSize[eGRD_OPERATION_TAB].top = rctTitleSize[eTITLE_OPERATION].bottom;
	rctGrdSize[eGRD_OPERATION_TAB].right = rctDlgSize.right - 1;
	rctGrdSize[eGRD_OPERATION_TAB].bottom = rctGrdSize[eGRD_OPERATION_TAB].top + lBottomBlockSize;

	rctGrdSize[eGRD_OPERATION_INPUT].left = rctTitleSize[eTITLE_CONTROL].left;
	rctGrdSize[eGRD_OPERATION_INPUT].top = rctGrdSize[eGRD_OPERATION_TAB].bottom;
	rctGrdSize[eGRD_OPERATION_INPUT].right = rctDlgSize.right - 1;
	rctGrdSize[eGRD_OPERATION_INPUT].bottom = rctGrdSize[eGRD_OPERATION_INPUT].top + (lBottomBlockSize * 2);

	// Grid를 생성한다.
	for (int nGrdNum = 0; nGrdNum < eGRD_MAX; nGrdNum++)
	{
		m_pGrid[nGrdNum] = new CGridCtrl;
		ASSERT(m_pGrid[nGrdNum]);

		m_pGrid[nGrdNum]->Create(rctGrdSize[nGrdNum], this, IDC_GRD_MOTOR + nGrdNum, (WS_CHILD | WS_TABSTOP | WS_VISIBLE));
	}

	// 버튼들의 좌표 값
	rctBtnSize[eBTN_PLUS].left = rctTitleSize[eTITLE_CONTROL].left;
	rctBtnSize[eBTN_PLUS].top = rctGrdSize[eGRD_OPERATION_INPUT].bottom;
	rctBtnSize[eBTN_PLUS].right = rctTitleSize[eTITLE_CONTROL].left + (rctTitleSize[eTITLE_CONTROL].Width() / 2);
	rctBtnSize[eBTN_PLUS].bottom = rctBtnSize[eBTN_PLUS].top + lBottomBlockSize;

	rctBtnSize[eBTN_MINUS].left = rctBtnSize[eBTN_PLUS].right;
	rctBtnSize[eBTN_MINUS].top = rctGrdSize[eGRD_OPERATION_INPUT].bottom;
	rctBtnSize[eBTN_MINUS].right = rctDlgSize.right - 1;
	rctBtnSize[eBTN_MINUS].bottom = rctBtnSize[eBTN_PLUS].top + lBottomBlockSize;

	rctBtnSize[eBTN_STOP].left = rctTitleSize[eTITLE_CONTROL].left;
	rctBtnSize[eBTN_STOP].top = rctBtnSize[eBTN_PLUS].top + lBottomBlockSize;
	rctBtnSize[eBTN_STOP].right = rctDlgSize.right - 1;
	rctBtnSize[eBTN_STOP].bottom = rctDlgSize.bottom - 1;

	// 버튼을 생성한다.
	for (int nBtnNum = 0; nBtnNum < eBTN_MAX; nBtnNum++)
	{
		m_pButton[nBtnNum] = new CMacButton;
		ASSERT(m_pButton[nBtnNum]);

		m_pButton[nBtnNum]->Create(strArrCaption[nBtnNum], WS_BORDER | WS_VISIBLE, rctBtnSize[nBtnNum], this, IDC_BTN_PLUS + nBtnNum);

		m_pButton[nBtnNum]->SetLogFont(lfBold);
		m_pButton[nBtnNum]->SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
		m_pButton[nBtnNum]->SetTextColor(DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		m_pButton[nBtnNum]->Invalidate(TRUE);
	}
}

/*
 desc : 모터의 정보를 맴버 변수 벡터에 저장한다.
 parm : None
 retn : None
*/
VOID CDlgMotr::InitMotionIndex()
{
	ST_MOTION stTemp;		/* 모터 임시 변수 */

	m_stVctMotion.clear();

	// Stage X, Y 항목 저장
	for (int i = 0; i < m_u8StageCount; i++)
	{
		stTemp.strMotorName.Format(_T("STAGE%d X"), i + 1);
		stTemp.DeviceNum = ENG_MMDI(i * 2);
		m_stVctMotion.push_back(stTemp);

		stTemp.strMotorName.Format(_T("STAGE%d Y"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((i * 2) + 1);
		m_stVctMotion.push_back(stTemp);
	}

	// Align Camera X 항목 저장
	for (int i = 0; i < m_u8ACamCount; i++)
	{
		stTemp.strMotorName.Format(_T("CAM%d X"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((int)ENG_MMDI::en_align_cam1 + i);
		m_stVctMotion.push_back(stTemp);
	}

	// Photo Head Z 항목 저장
	for (int i = 0; i < m_u8HeadCount; i++)
	{
		stTemp.strMotorName.Format(_T("PH%d Z"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((int)ENG_MMDI::en_axis_ph1 + i);
		m_stVctMotion.push_back(stTemp);
	}

	// Align Camera Z 항목 저장
	for (int i = 0; i < m_u8ACamCount; i++)
	{
		stTemp.strMotorName.Format(_T("CAM%d Z"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((int)ENG_MMDI::en_axis_acam1 + i);
		m_stVctMotion.push_back(stTemp);
	}
}

/*
 desc : Motor Grid를 초기화한다.
 parm : None
 retn : None
*/
VOID CDlgMotr::InitMotorGrid()
{
	CRect rctSize;					/* 작업 영역 */

	LOGFONT	lfFont = GetLogFont(20, TRUE);	/* 폰트 생성 */

	double dCellRatio[eCELL_MOTOR_MAX] = { 0.1, 0.3, 0.45, 0.15 };	/* 그리드 비율 */
	int nHeightSize = 40;	/* 셀 높이 */
	int	nWidthDiffer = 0;	/* 셀 너비 오차값 */

	CString strUnit = _T("mm");			/* 단위 문자 */

	UINT nCenterAlignText = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;	/* 그리드 옵션 */

	// 그리드 기본 설정
	m_pGrid[eGRD_MOTOR]->GetClientRect(rctSize);
	m_pGrid[eGRD_MOTOR]->SetColumnResize(FALSE);
	m_pGrid[eGRD_MOTOR]->SetRowResize(FALSE);
	m_pGrid[eGRD_MOTOR]->SetEditable(FALSE);
	m_pGrid[eGRD_MOTOR]->EnableSelection(FALSE);
	m_pGrid[eGRD_MOTOR]->ModifyStyle(WS_HSCROLL, 0);

	m_pGrid[eGRD_MOTOR]->SetGridLineColor(BLACK_);
	m_pGrid[eGRD_MOTOR]->SetTextColor(BLACK_);

	m_pGrid[eGRD_MOTOR]->DeleteAllItems();

	// 그리드 영역이 작업 영역을 초과하면 스크롤 바 만큼의 오차값 저장
	if (rctSize.Height() - 1 < nHeightSize * m_u8AllMotorCount)
	{
		nWidthDiffer = ::GetSystemMetrics(SM_CXVSCROLL);
	}

	// 셀의 개수 지정
	m_pGrid[eGRD_MOTOR]->SetColumnCount(eCELL_MOTOR_MAX);
	m_pGrid[eGRD_MOTOR]->SetRowCount(m_u8AllMotorCount);

	// 높이, 너비, 색상, 문자 적용
	for (int nCol = 0; nCol < eCELL_MOTOR_MAX; nCol++)
	{
		m_pGrid[eGRD_MOTOR]->SetColumnWidth(nCol, (int)((rctSize.Width() - nWidthDiffer) * dCellRatio[nCol]));

		for (int nRow = 0; nRow < m_u8AllMotorCount; nRow++)
		{
			m_pGrid[eGRD_MOTOR]->SetRowHeight(nRow, nHeightSize);
			m_pGrid[eGRD_MOTOR]->SetItemFormat(nRow, nCol, nCenterAlignText);
			m_pGrid[eGRD_MOTOR]->SetItemFont(nRow, nCol, &lfFont);
			
			if (eCELL_MOTOR_LED_STATUS == nCol)
			{
				m_pGrid[eGRD_MOTOR]->SetItemBkColour(nRow, nCol, DEF_RGB_TAB_NORMAL);
			}
			else if (eCELL_MOTOR_MOTOR_NAME == nCol)
			{
				m_pGrid[eGRD_MOTOR]->SetItemText(nRow, nCol, m_stVctMotion[nRow].strMotorName);
				m_pGrid[eGRD_MOTOR]->SetItemBkColour(nRow, nCol, DEF_RGB_LABEL);
			}
			else if (eCELL_MOTOR_POS_VALUE == nCol)
			{
				m_pGrid[eGRD_MOTOR]->SetItemText(nRow, nCol, _T("0.0000"));
			}
			else if (eCELL_MOTOR_POS_UNIT == nCol)
			{
				m_pGrid[eGRD_MOTOR]->SetItemText(nRow, nCol, strUnit);
				m_pGrid[eGRD_MOTOR]->SetItemBkColour(nRow, nCol, DEF_RGB_LABEL);
			}
		}
	}

	for (int nCol = 1; nCol < eCELL_MOTOR_MAX; nCol++)
	{
		m_pGrid[eGRD_MOTOR]->SetItemBkColour(0, nCol, DEF_RGB_LABEL_SEL);
	}
}

/*
 desc : Control Grid를 초기화한다.
 parm : None
 retn : None
*/
VOID CDlgMotr::InitControlGrid()
{
	CRect rctSize;	/* 작업 영역 */
	LOGFONT	lfFont = GetLogFont(20, TRUE);	/* 폰트 생성 */
	CString strCaption[eCELL_CTRL_MAX] = { _T("INIT"), _T("ON/OFF"), _T("RESET") };	/* 셀에 입력될 문자 */
	UINT nCenterAlignText = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;	/* 셀 속성 */

	// 그리드 기본 설정
	m_pGrid[eGRD_CONTROL]->GetClientRect(rctSize);
	m_pGrid[eGRD_CONTROL]->SetColumnResize(FALSE);
	m_pGrid[eGRD_CONTROL]->SetRowResize(FALSE);
	m_pGrid[eGRD_CONTROL]->SetEditable(FALSE);
	m_pGrid[eGRD_CONTROL]->EnableSelection(FALSE);
	m_pGrid[eGRD_CONTROL]->ModifyStyle(WS_HSCROLL, 0);

	m_pGrid[eGRD_CONTROL]->SetTextColor(DEF_COLOR_BTN_MENU_NORMAL_TEXT);
	m_pGrid[eGRD_CONTROL]->SetTextBkColor(DEF_COLOR_BTN_PAGE_NORMAL);
	m_pGrid[eGRD_CONTROL]->SetGridLineColor(BLACK_);

	m_pGrid[eGRD_CONTROL]->DeleteAllItems();

	// 셀의 개수 지정
	m_pGrid[eGRD_CONTROL]->SetColumnCount(eCELL_CTRL_MAX);
	m_pGrid[eGRD_CONTROL]->SetRowCount(1);

	// 셀 높이 지정
	m_pGrid[eGRD_CONTROL]->SetRowHeight(0, rctSize.Height() - 1);

	// 셀 너비 및 문자, 폰트 적용
	for (int nCol = 0; nCol < eCELL_CTRL_MAX; nCol++)
	{
		m_pGrid[eGRD_CONTROL]->SetColumnWidth(nCol, (int)((rctSize.Width()) / eCELL_CTRL_MAX));
		m_pGrid[eGRD_CONTROL]->SetItemFormat(0, nCol, nCenterAlignText);
		m_pGrid[eGRD_CONTROL]->SetItemText(0, nCol, strCaption[nCol]);
		m_pGrid[eGRD_CONTROL]->SetItemFont(0, nCol, &lfFont);
	}
}

/*
 desc : Tap Grid를 초기화한다.
 parm : None
 retn : None
*/
VOID CDlgMotr::InitOpTabGrid()
{
	CRect rctSize;	/* 작업 영역 */
	LOGFONT	lfFont = GetLogFont(20, TRUE);	/* 폰트 생성 */
	CString strCaption[eCELL_TAB_MAX] = { _T("ABS"), _T("REL") };	/* 셀에 입력될 문자 */
	UINT nCenterAlignText = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;	/* 셀 속성 */

	// 그리드 기본 설정
	m_pGrid[eGRD_OPERATION_TAB]->GetClientRect(rctSize);
	m_pGrid[eGRD_OPERATION_TAB]->SetColumnResize(FALSE);
	m_pGrid[eGRD_OPERATION_TAB]->SetRowResize(FALSE);
	m_pGrid[eGRD_OPERATION_TAB]->SetEditable(FALSE);
	m_pGrid[eGRD_OPERATION_TAB]->EnableSelection(FALSE);
	m_pGrid[eGRD_OPERATION_TAB]->ModifyStyle(WS_HSCROLL, 0);

	m_pGrid[eGRD_OPERATION_TAB]->SetTextColor(WHITE_);
	m_pGrid[eGRD_OPERATION_TAB]->SetGridLineColor(BLACK_);

	m_pGrid[eGRD_OPERATION_TAB]->DeleteAllItems();

	// 셀의 개수 지정
	m_pGrid[eGRD_OPERATION_TAB]->SetColumnCount(eCELL_TAB_MAX);
	m_pGrid[eGRD_OPERATION_TAB]->SetRowCount(1);

	// 셀 높이 지정
	m_pGrid[eGRD_OPERATION_TAB]->SetRowHeight(0, rctSize.Height() - 1);

	// 셀 너비 및 문자, 폰트 적용
	for (int nCol = 0; nCol < eCELL_TAB_MAX; nCol++)
	{
		m_pGrid[eGRD_OPERATION_TAB]->SetColumnWidth(nCol, (int)((rctSize.Width()) / eCELL_TAB_MAX));
		m_pGrid[eGRD_OPERATION_TAB]->SetItemFormat(0, nCol, nCenterAlignText);
		m_pGrid[eGRD_OPERATION_TAB]->SetItemText(0, nCol, strCaption[nCol]);
		m_pGrid[eGRD_OPERATION_TAB]->SetItemFont(0, nCol, &lfFont);
	}

	m_pGrid[eGRD_OPERATION_TAB]->SetItemBkColour(0, eCELL_TAB_ABSOLUTE_MOVE, DEF_RGB_TAB_SELECT);
	m_pGrid[eGRD_OPERATION_TAB]->SetItemBkColour(0, eCELL_TAB_RELATIVE_MOVE, DEF_RGB_TAB_NORMAL);
}

/*
 desc : Input Grid를 초기화한다.
 parm : None
 retn : None
*/
VOID CDlgMotr::InitOpInputGrid()
{
	CRect rctSize;	/* 작업 영역 */
	LOGFONT	lfFont = GetLogFont(20, TRUE);	/* 폰트 생성 */

	double dCellRatio[eCELL_INPUT_COL_MAX] = { 0.3, 0.5, 0.2 };	/* 셀 비율 */
	
	CString strCaption[eCELL_INPUT_ROW_MAX][eCELL_INPUT_COL_MAX] = {	/* 셀에 입력될 문자 */
		{_T("SPEED"), _T("0"), _T("mm/s")},
		{_T("POSITION"), _T("0"), _T("mm")}
	};

	UINT nCenterAlignText = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;	/* 셀 속성 */

	// 그리드 기본 설정
	m_pGrid[eGRD_OPERATION_INPUT]->GetClientRect(rctSize);
	m_pGrid[eGRD_OPERATION_INPUT]->SetColumnResize(FALSE);
	m_pGrid[eGRD_OPERATION_INPUT]->SetRowResize(FALSE);
	m_pGrid[eGRD_OPERATION_INPUT]->SetEditable(FALSE);
	m_pGrid[eGRD_OPERATION_INPUT]->EnableSelection(FALSE);
	m_pGrid[eGRD_OPERATION_INPUT]->ModifyStyle(WS_HSCROLL, 0);

	m_pGrid[eGRD_OPERATION_INPUT]->SetTextColor(BLACK_);
	m_pGrid[eGRD_OPERATION_INPUT]->SetGridLineColor(BLACK_);

	m_pGrid[eGRD_OPERATION_INPUT]->DeleteAllItems();

	// 셀의 개수 지정
	m_pGrid[eGRD_OPERATION_INPUT]->SetColumnCount(eCELL_INPUT_COL_MAX);
	m_pGrid[eGRD_OPERATION_INPUT]->SetRowCount(eCELL_INPUT_ROW_MAX);

	for (int nCol = 0; nCol < eCELL_INPUT_COL_MAX; nCol++)
	{
		m_pGrid[eGRD_OPERATION_INPUT]->SetColumnWidth(nCol, (int)(rctSize.Width() * dCellRatio[nCol]));

		for (int nRow = 0; nRow < eCELL_INPUT_ROW_MAX; nRow++)
		{
			m_pGrid[eGRD_OPERATION_INPUT]->SetRowHeight(nRow, (int)(rctSize.Height() / eCELL_INPUT_ROW_MAX));
			m_pGrid[eGRD_OPERATION_INPUT]->SetItemFormat(nRow, nCol, nCenterAlignText);
			m_pGrid[eGRD_OPERATION_INPUT]->SetItemText(nRow, nCol, strCaption[nRow][nCol]);
			m_pGrid[eGRD_OPERATION_INPUT]->SetItemFont(nRow, nCol, &lfFont);

			if (eCELL_INPUT_VALUE != nCol)
			{
				m_pGrid[eGRD_OPERATION_INPUT]->SetItemBkColour(nRow, nCol, DEF_RGB_LABEL);
			}
		}
	}
}
#pragma endregion

/*
 desc : LogFont를 설정한다.
 parm : 폰트 사이즈, 굵은 글씨 사용 여부 
 retn : LOGFONT
*/
LOGFONT CDlgMotr::GetLogFont(int nSize, BOOL bIsBold)
{
	LOGFONT lfFont;

	lfFont.lfHeight = nSize;
	lfFont.lfWidth = 0;
	lfFont.lfEscapement = 0;
	lfFont.lfOrientation = 0;
	lfFont.lfItalic = false;
	lfFont.lfUnderline = false;
	lfFont.lfStrikeOut = false;
	lfFont.lfCharSet = DEFAULT_CHARSET;
	lfFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lfFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lfFont.lfQuality = CLEARTYPE_NATURAL_QUALITY;
	lfFont.lfPitchAndFamily = DEFAULT_PITCH;
	StrCpy(lfFont.lfFaceName, DEF_FONT_NAME);

	lfFont.lfWeight = bIsBold ? FW_BOLD : FW_NORMAL;

	return lfFont;
}

VOID CDlgMotr::MoveStart(ENG_MMDI drv_id, double dPosition, double dSpeed, BOOL bIsRel/* = FALSE*/)
{
	double dTargetPos = dPosition;
	
	/*Mc2 제어*/
	if (drv_id < ENG_MMDI::en_axis_acam1)
	{
		if (FALSE == uvCmn_MC2_IsDevLocked(drv_id))
		{
			AfxMessageBox(_T("현재 모터가 Off 상태입니다."));
			return;
		}
		// 에러 상태 확인
		else if (TRUE == uvCmn_MC2_IsDriveError(drv_id))
		{
			AfxMessageBox(_T("현재 모터가 에러 상태입니다."));
			return;
		}
		// Busy 신호 확인
		else if (TRUE == uvCmn_MC2_IsDriveBusy(drv_id))
		{
			AfxMessageBox(_T("현재 모터가 동작 상태입니다."));
			return;
		}

		// 상대 이동일 경우
		if (TRUE == bIsRel)
		{
			dTargetPos = uvCmn_MC2_GetDrvAbsPos(drv_id) + dPosition;
		}

		if (drv_id < ENG_MMDI::en_axis_ph1)
		{
			if (uvEng_GetConfig()->mc2_svc.max_dist[(int)drv_id] < dTargetPos)
			{
				CString strTemp;
				strTemp.Format(_T("%d 축이 이동할 Position이 Max Position을 넘어서 움직일수 없습니다."), (int)drv_id);
				AfxMessageBox(strTemp);
				return;
			}
		}
		else if (drv_id < ENG_MMDI::en_axis_acam1 && drv_id > ENG_MMDI::en_align_cam2)
		{
			if (uvEng_GetConfig()->luria_svc.ph_z_move_max < dTargetPos)
			{
				CString strTemp;
				strTemp.Format(_T("%d 축이 이동할 Position이 Max Position을 넘어서 움직일수 없습니다."), (int)drv_id);
				AfxMessageBox(strTemp);
				return;
			}
		}

		
		

		if (CInterLockManager::GetInstance()->CheckMoveInterlock(drv_id, dTargetPos))
		{
			AfxMessageBox(CInterLockManager::GetInstance()->GetLastError());
			return;
		}
		
		uvEng_MC2_SendDevAbsMove(drv_id, dTargetPos, dSpeed);
	}
	/*Philhmi 제어*/
	else
	{
		if (FALSE == bIsRel)
		{
			STG_PP_P2C_ABS_MOVE stSend;
			STG_PP_P2C_ABS_MOVE_ACK stRecv;

			stSend.Reset();
			stRecv.Reset();

			stSend.usCount = 1;
			if (drv_id == ENG_MMDI::en_axis_acam1)
				strncpy_s(stSend.stMove[0].szAxisName, "ALIGN_CAMERA_Z1", DEF_MAX_RECIPE_NAME_LENGTH);
			else if (drv_id == ENG_MMDI::en_axis_acam2)
				strncpy_s(stSend.stMove[0].szAxisName, "ALIGN_CAMERA_Z2", DEF_MAX_RECIPE_NAME_LENGTH);
			else if (drv_id == ENG_MMDI::en_axis_acam3)
				strncpy_s(stSend.stMove[0].szAxisName, "ALIGN_CAMERA_Z3", DEF_MAX_RECIPE_NAME_LENGTH);

			stSend.stMove[0].dPosition = dPosition;
			stSend.stMove[0].dSpeed = 10;// dSpeed;
			stSend.stMove[0].dAcc = DEF_DEFAULT_ACC;
			uvEng_Philhmi_Send_P2C_ABS_MOVE(stSend, stRecv);
		}
		else
		{
			STG_PP_P2C_REL_MOVE stSend;
			STG_PP_P2C_REL_MOVE_ACK stRecv;

			stSend.Reset();
			stRecv.Reset();

			stSend.usCount = 1;
			if (drv_id == ENG_MMDI::en_axis_acam1)
				strncpy_s(stSend.stMove[0].szAxisName, "ALIGN CAMERA Z 1", DEF_MAX_RECIPE_NAME_LENGTH);
			else if (drv_id == ENG_MMDI::en_axis_acam2)
				strncpy_s(stSend.stMove[0].szAxisName, "ALIGN CAMERA Z 2", DEF_MAX_RECIPE_NAME_LENGTH);
			stSend.stMove[0].dPosition = dPosition;
			stSend.stMove[0].dSpeed = 10;
			stSend.stMove[0].dAcc = DEF_DEFAULT_ACC;
			uvEng_Philhmi_Send_P2C_REL_MOVE(stSend, stRecv);
		}
	}

}

/*
 desc : 동작 형태를 변경한다.
 parm : 동작 형태를 입력 받는다. (0 : ABS, 1 : REL)
 retn : None
*/
VOID CDlgMotr::ChangeMoveTpye(UINT8 u8Type)
{
	// 현재 설정된 값과 동일한 값을 입력 받았다면 종료한다.
	if ((BOOL)u8Type == m_bMoveType)	return;

	// 입력 받은 값으로 설정
	m_bMoveType = (BOOL)u8Type;

	// UI 화면에 적용
	switch (u8Type)
	{
	case eCELL_TAB_ABSOLUTE_MOVE:
		m_pGrid[eGRD_OPERATION_TAB]->SetItemBkColour(0, eCELL_TAB_ABSOLUTE_MOVE, DEF_RGB_TAB_SELECT);
		m_pGrid[eGRD_OPERATION_TAB]->SetItemBkColour(0, eCELL_TAB_RELATIVE_MOVE, DEF_RGB_TAB_NORMAL);
		break;
	case eCELL_TAB_RELATIVE_MOVE:
		m_pGrid[eGRD_OPERATION_TAB]->SetItemBkColour(0, eCELL_TAB_ABSOLUTE_MOVE, DEF_RGB_TAB_NORMAL);
		m_pGrid[eGRD_OPERATION_TAB]->SetItemBkColour(0, eCELL_TAB_RELATIVE_MOVE, DEF_RGB_TAB_SELECT);
		break;
	default:
		return;
	}

	// Grid 갱신
	m_pGrid[eGRD_OPERATION_TAB]->Refresh();
}

/*
 desc : 입력란에 입력 동작을 수행한다.
 parm : 입력할 파라미터 설정, 설정 값
 retn : None
*/
VOID CDlgMotr::InputParameter(UINT8 u8Sel)
{
	// 현재 설정된 값과 동일한 값을 입력 받았다면 종료한다.
	if (eCELL_INPUT_SPEED > u8Sel || eCELL_INPUT_ROW_MAX <= u8Sel)	return;

	CDlgKBDN dlgKeyPad;
	CString strValue = _T("");
	double dValue = 0.;

	CString strTemp;
	CStringArray	strArrVelo;

	if (u8Sel == eCELL_INPUT_SPEED)
	{
		strArrVelo.Add(_T("[CANCEL]"));
		if (m_stVctMotion[m_u8SelMotor].DeviceNum < ENG_MMDI::en_axis_ph1)
		{
			for (int i = 0; i < MAX_SELECT_VELO; i++)
			{
				strTemp.Format(_T("%.4f"), uvEng_GetConfig()->mc2_svc.select_velo[(int)m_stVctMotion[m_u8SelMotor].DeviceNum][i]);
				strArrVelo.Add(strTemp);
			}
		}
		else if (m_stVctMotion[m_u8SelMotor].DeviceNum < ENG_MMDI::en_axis_acam1 && m_stVctMotion[m_u8SelMotor].DeviceNum > ENG_MMDI::en_align_cam2)
		{
			for (int i = 0; i < MAX_SELECT_VELO; i++)
			{
				strTemp.Format(_T("%.4f"), uvEng_GetConfig()->luria_svc.ph_z_select_velo[i]);
				strArrVelo.Add(strTemp);
			}
		}

		int nSelectRecipe = ShowMultiSelectMsg(EN_MSG_BOX_TYPE::eQUEST, _T("SELECT VELOCITY"), strArrVelo);

		if (0 == nSelectRecipe)
		{
			// cancel
			return;
		}
		else
		{
			strTemp = strArrVelo.GetAt(nSelectRecipe);
		}
	}

	


	// UI 화면에 적용
	switch (u8Sel)
	{
	case eCELL_INPUT_SPEED:
		m_dSetSpeed = _ttof(strTemp);
		dValue = m_dSetSpeed;
		break;
	case eCELL_INPUT_POSITION:
		if (IDOK != dlgKeyPad.MyDoModal(_T("Keypad"), TRUE, TRUE, -9999.9999, 9999.9999))	return;

		dValue = dlgKeyPad.GetValueDouble();
		m_dSetPosition = dValue;
		break;
	default:
		return;
	}

	// Grid 갱신
	strValue.Format(_T("%.4f"), dValue);

	// 소숫점 자리 마지막에 0이 존재할 경우 자르기
	for (int i = strValue.GetLength() - 1; i >= 0; i--)
	{
		if ('0' != strValue.GetAt(i))
		{
			if ('.' == strValue.GetAt(i))
				strValue = strValue.Left(i);
			else
				strValue = strValue.Left(i + 1);

			break;
		}
	}

	m_pGrid[eGRD_OPERATION_INPUT]->SetItemText(u8Sel, eCELL_INPUT_VALUE, strValue);
	m_pGrid[eGRD_OPERATION_INPUT]->Refresh();
}

/*
 desc : 모터를 선택한다.
 parm : 선택된 모터
 retn : None
*/
VOID CDlgMotr::MotorSelect(UINT8 u8Sel)
{
	// 현재 설정된 값과 동일한 값을 입력 받았다면 종료한다.
	if (u8Sel == m_u8SelMotor)	return;

	// 과거에 선택되었던 항목의 색상을 변경한다.
	for (int nCol = 1; nCol < eCELL_MOTOR_MAX; nCol++)
	{
		if (eCELL_MOTOR_POS_VALUE == nCol)
			m_pGrid[eGRD_MOTOR]->SetItemBkColour(m_u8SelMotor, nCol, WHITE_);
		else
			m_pGrid[eGRD_MOTOR]->SetItemBkColour(m_u8SelMotor, nCol, DEF_RGB_LABEL);
	}

	// 새로 선택된 항목의 색상을 변경한다.
	for (int nCol = 1; nCol < eCELL_MOTOR_MAX; nCol++)
	{
		m_pGrid[eGRD_MOTOR]->SetItemBkColour(u8Sel, nCol, DEF_RGB_LABEL_SEL);
	}

	// 화면 갱신
	m_pGrid[eGRD_MOTOR]->Refresh();
	m_u8SelMotor = u8Sel;
}

/*
desc : 모터 동작을 수행한다.
parm : 동작할 내용
retn : None
*/
VOID CDlgMotr::MotorControl(UINT8 u8Sel)
{
	switch (u8Sel)
	{
	case eCELL_CTRL_INITIALIZE:
		uvEng_MC2_SendDevHoming(m_stVctMotion[m_u8SelMotor].DeviceNum);
		break;
	case eCELL_CTRL_SERVO_ON_OFF:
		uvEng_MC2_SendDevLocked(m_stVctMotion[m_u8SelMotor].DeviceNum, !uvCmn_MC2_IsDevLocked(m_stVctMotion[m_u8SelMotor].DeviceNum));
		break;
	case eCELL_CTRL_ERROR_RESET:
		uvEng_MC2_SendDevFaultReset(m_stVctMotion[m_u8SelMotor].DeviceNum);
		break;
	default:
		return;
	}
}

/*
 desc : Motor Grid를 갱신한다.
 parm : None
 retn : None
*/
VOID CDlgMotr::UpdateMotorStatus()
{
	double dPosition = 0.;
	CString strPosition;

	for (int nRow = 0; nRow < m_u8AllMotorCount; nRow++)
	{
		// On/Off 상태 확인
		// 230516 mhbaek Modify : Flag 반대
		if (FALSE == uvCmn_MC2_IsDevLocked(m_stVctMotion[nRow].DeviceNum))
		{
			m_pGrid[eGRD_MOTOR]->SetItemBkColour(nRow, eCELL_MOTOR_LED_STATUS, DEF_RGB_TAB_NORMAL);
		}
		// 에러 상태 확인
		else if (TRUE == uvCmn_MC2_IsDriveError(m_stVctMotion[nRow].DeviceNum))
		{
			m_pGrid[eGRD_MOTOR]->SetItemBkColour(nRow, eCELL_MOTOR_LED_STATUS, LIGHT_RED);
		}
		// 동작이 가능한 상태
		else
		{
			m_pGrid[eGRD_MOTOR]->SetItemBkColour(nRow, eCELL_MOTOR_LED_STATUS, LIGHT_GREEN);
		}

		dPosition = uvCmn_MC2_GetDrvAbsPos(m_stVctMotion[nRow].DeviceNum);
		strPosition.Format(_T("%.4f"), dPosition);

		m_pGrid[eGRD_MOTOR]->SetItemText(nRow, eCELL_MOTOR_POS_VALUE, strPosition);
	}

	// 화면 갱신
	m_pGrid[eGRD_MOTOR]->Refresh();
}

/*
 desc : 버튼 이벤트를 처리한다.
 parm : 버튼 리소스 ID
 retn : None
*/
void CDlgMotr::OnClickButtonEvent(UINT ID)
{
	int nCommand = ID - IDC_BTN_PLUS;
	double dPos = m_dSetPosition;

	switch (nCommand)
	{
	case eBTN_PLUS:
		MoveStart(m_stVctMotion[m_u8SelMotor].DeviceNum, dPos, m_dSetSpeed, m_bMoveType);
		break;
	case eBTN_MINUS:
		// 정방향
		dPos = (m_bMoveType == eCELL_TAB_RELATIVE_MOVE) ? dPos * -1.0 : dPos;
		MoveStart(m_stVctMotion[m_u8SelMotor].DeviceNum, dPos, m_dSetSpeed, m_bMoveType);
		break;
	case eBTN_STOP:
		// 전체 정지
		uvEng_MC2_SendDevStopped(m_stVctMotion[m_u8SelMotor].DeviceNum);
		break;
	default:
		break;
	}
}

/*
 desc : 그리드의 이벤트를 처리한다.
 parm : 그리드 리소스 ID, 이벤트가 발생된 셀
 retn : None
*/
void CDlgMotr::OnGrdClickedEvent(UINT ID, NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	int nCommand = ID - IDC_GRD_MOTOR;
	
	if (pItem == nullptr || pItem->iRow == -1 || pItem->iColumn == -1)
		return;

	switch (nCommand)
	{
	case eGRD_MOTOR:
		MotorSelect(pItem->iRow);
		break;
	case eGRD_CONTROL:
		MotorControl(pItem->iColumn);
		break;
	case eGRD_OPERATION_TAB:
		ChangeMoveTpye(pItem->iColumn);
		break;
	case eGRD_OPERATION_INPUT:
		InputParameter(pItem->iRow);
		break;
	default:
		break;
	}
}

/*
 desc : 일정 시간마다 동작을 수행한다.
 parm : 타이머 ID
 retn : None
*/
void CDlgMotr::OnTimer(UINT_PTR nIDEvent)
{
	if (FALSE == IsWindowVisible())
	{
		return;
	}

	if (DEF_TIMER_MOTOR_CONSOLE == nIDEvent)
	{
		UpdateMotorStatus();
	}

	CMyDialog::OnTimer(nIDEvent);
}

/*
 desc : 화면이 띄워져 있는지 확인한다.
 parm : 지금 화면이 보이는지의 대한 여부와 상태
 retn : None
*/
void CDlgMotr::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CMyDialog::OnShowWindow(bShow, nStatus);

	if (TRUE == bShow)
	{
		SetTimer(DEF_TIMER_MOTOR_CONSOLE, 50, NULL);
	}
	else
	{
		// 화면에 보이지 않는 상황에서 타이머를 돌릴 필요는 없다.
		KillTimer(DEF_TIMER_MOTOR_CONSOLE);
	}
}

/*
 desc : 상단바에 대한 이벤트를 받는다.
 parm : 상단바에서 발생된 이벤트 ID
 retn : None
*/
void CDlgMotr::OnSysCommand(UINT nID, LPARAM lParam)
{
	//종료버튼 눌릴 시
	if (nID == SC_CLOSE)
	{
		// 화면에 보이지 않는 상황에서 타이머를 돌릴 필요는 없다.
		KillTimer(DEF_TIMER_MOTOR_CONSOLE);
	}

	CMyDialog::OnSysCommand(nID, lParam);
}

int CDlgMotr::ShowMultiSelectMsg(EN_MSG_BOX_TYPE mType, CString strTitle, CStringArray& strArrCommand)
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