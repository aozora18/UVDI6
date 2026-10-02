
/*
 desc : 프로그램 종료될 때, 대기 알림
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgMotr.h"
#include "../mesg/DlgMesg.h"
#include "../param/InterLockManager.h"
#include <afxtaskdialog.h>
#include "..\GlobalVariables.h"

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
	const int thetaTable = 1;
	// Stage 개수(x, y축) + Head 개수 + Align Camera 개수
	m_u8AllMotorCount = (m_u8StageCount * 2) + m_u8HeadCount + (m_u8ACamCount * 2) + thetaTable;
	
	/* 멤버 변수 초기화 */
	m_u8SelMotor = 0;
	m_stVctMotion.clear();
	m_bMoveType = eCELL_TAB_ABSOLUTE_MOVE;
	m_dSetSpeed = 0.;
	m_dSetPosition = 0.;
	m_nSelUserPos = 1;
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
	ON_NOTIFY_RANGE(NM_DBLCLK, IDC_GRD_MOTOR, IDC_GRD_MOTOR+ eGRD_MAX, OnGrdDblClickedEvent)
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

	/*유저 지정 위치값 읽기*/
	LoadUserPosition();
	InitUserPosGrid();

	/*User Position 기능 숨기긱 적용*/
	//ShowUserPositionUI(FALSE);
	ShowUserPositionUI(TRUE);

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

	CString strArrCaption[eBTN_MAX] = { _T("+"), _T("-"), _T("STOP"),
		_T("Name"), _T("Use Position2"), _T("Get Pos"), _T("Move Pos"), _T("Save Name ") };	/* 버튼에 사용될 문자 */

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

	/*상단 영역 좌표 계산*/

	// Motor 항목 좌표 값
	rctGrdSize[eGRD_MOTOR].left = rctDlgSize.left + 1;
	rctGrdSize[eGRD_MOTOR].top = rctTitleSize[eTITLE_MOTOR].bottom;
	rctGrdSize[eGRD_MOTOR].right = rctTitleSize[eTITLE_CONTROL].left;
	rctGrdSize[eGRD_MOTOR].bottom = rctTitleSize[eTITLE_USER_POSITION].top;

	// Control 항목 좌표 값
	rctGrdSize[eGRD_CONTROL].left = rctTitleSize[eTITLE_CONTROL].left;
	rctGrdSize[eGRD_CONTROL].top = rctTitleSize[eTITLE_CONTROL].bottom;
	rctGrdSize[eGRD_CONTROL].right = rctDlgSize.right;
	rctGrdSize[eGRD_CONTROL].bottom = rctTitleSize[eTITLE_OPERATION].top;

	// Operation 항목 좌표 값
	lBottomBlockSize = (rctTitleSize[eTITLE_USER_POSITION].top - rctTitleSize[eTITLE_OPERATION].bottom) / 5;

	rctGrdSize[eGRD_OPERATION_TAB].left = rctTitleSize[eTITLE_CONTROL].left;
	rctGrdSize[eGRD_OPERATION_TAB].top = rctTitleSize[eTITLE_OPERATION].bottom;
	rctGrdSize[eGRD_OPERATION_TAB].right = rctDlgSize.right - 1;
	rctGrdSize[eGRD_OPERATION_TAB].bottom = rctGrdSize[eGRD_OPERATION_TAB].top + lBottomBlockSize;

	rctGrdSize[eGRD_OPERATION_INPUT].left = rctTitleSize[eTITLE_CONTROL].left;
	rctGrdSize[eGRD_OPERATION_INPUT].top = rctGrdSize[eGRD_OPERATION_TAB].bottom;
	rctGrdSize[eGRD_OPERATION_INPUT].right = rctDlgSize.right - 1;
	rctGrdSize[eGRD_OPERATION_INPUT].bottom = rctGrdSize[eGRD_OPERATION_INPUT].top + (lBottomBlockSize * 2);

	/*User Position 기능*/
	CRect rctUserPosArea;
	rctUserPosArea.left = rctDlgSize.left + 1;
	rctUserPosArea.top = rctTitleSize[eTITLE_USER_POSITION].bottom;
	rctUserPosArea.right = rctDlgSize.right - 1;
	rctUserPosArea.bottom = rctDlgSize.bottom - 1;

	//Grid 좌표(하단 영역의 상단 60% 차지)
	rctGrdSize[eGRD_USER_POS].left = rctUserPosArea.left;
	rctGrdSize[eGRD_USER_POS].top = rctUserPosArea.top;
	rctGrdSize[eGRD_USER_POS].right = rctUserPosArea.right;
	rctGrdSize[eGRD_USER_POS].bottom = rctUserPosArea.top + (rctUserPosArea.Height() * 0.8);

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
	rctBtnSize[eBTN_STOP].bottom = rctTitleSize[eTITLE_USER_POSITION].top;

	//하단 버튼 영역 좌표 계산
	CRect rctUposBtnArea;
	rctUposBtnArea.left = rctUserPosArea.left;
	rctUposBtnArea.top = rctGrdSize[eGRD_USER_POS].bottom;
	rctUposBtnArea.right = rctUserPosArea.right;
	rctUposBtnArea.bottom = rctUserPosArea.bottom;

	int nBtnHeight = rctUposBtnArea.Height();
	int nBtnWidth = rctUposBtnArea.Width() / 5;

	//Name 버튼 및 텍스트 버튼 (왼쪽)
	rctBtnSize[eBTN_UPOS_NAME].SetRect(rctUposBtnArea.left, rctUposBtnArea.top,
		nBtnWidth,
		rctUposBtnArea.top + nBtnHeight);
	rctBtnSize[eBTN_UPOS_DISP].SetRect(rctBtnSize[eBTN_UPOS_NAME].right, rctUposBtnArea.top,
		rctBtnSize[eBTN_UPOS_NAME].right + (nBtnWidth * 2),
		rctUposBtnArea.top + nBtnHeight);

	// 우측 Set/Get/Save 버튼
	rctBtnSize[eBTN_UPOS_GETPOS].SetRect(rctUposBtnArea.right - (nBtnWidth * 2),
		rctUposBtnArea.top, rctUposBtnArea.right - nBtnWidth, rctUposBtnArea.top + nBtnHeight/2);
	rctBtnSize[eBTN_UPOS_MOVE].SetRect(rctBtnSize[eBTN_UPOS_GETPOS].right, rctUposBtnArea.top,
		rctUposBtnArea.right, rctUposBtnArea.top + nBtnHeight/2);
	rctBtnSize[eBTN_UPOS_SAVE].SetRect(rctBtnSize[eBTN_UPOS_GETPOS].left, rctUposBtnArea.top + nBtnHeight / 2,
		rctUposBtnArea.right, rctUposBtnArea.top + nBtnHeight);

	// 버튼을 생성한다.
	for (int nBtnNum = 0; nBtnNum < eBTN_MAX; nBtnNum++)
	{
		m_pButton[nBtnNum] = new CMacButton;
		ASSERT(m_pButton[nBtnNum]);

		m_pButton[nBtnNum]->Create(strArrCaption[nBtnNum], WS_BORDER | WS_VISIBLE, rctBtnSize[nBtnNum], this, IDC_BTN_PLUS + nBtnNum);
		m_pButton[nBtnNum]->SetLogFont(lfBold);

		if (nBtnNum == eBTN_UPOS_DISP)
		{
			m_pButton[nBtnNum]->SetBgColor(WHITE_);
			m_pButton[nBtnNum]->SetTextColor(BLACK_);
		}
		else
		{
			m_pButton[nBtnNum]->SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
			m_pButton[nBtnNum]->SetTextColor(DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		}

		m_pButton[nBtnNum]->Invalidate(TRUE);
	}

	////Button 속성 적용 
	//CString strUposCaption[] = { _T("Name"),_T("Use Position2"),_T("Set Pos"), _T("Get Pos"), _T("Save Pos") };
	//int nCaptionIdx = 0;
	//for (int nBtnNum = eBTN_UPOS_NAME;nBtnNum <= eBTN_UPOS_SAVE;nBtnNum++)
	//{
	//	m_pButton[nBtnNum]->SetWindowText(strUposCaption[nCaptionIdx++]);

	//	if (nBtnNum == eBTN_UPOS_DISP)
	//	{
	//		m_pButton[nBtnNum]->SetBgColor(WHITE_);		//텍스트 표시용은 하얀색 배경
	//		m_pButton[nBtnNum]->SetTextColor(BLACK_);
	//	} 
	//	else
	//	{
	//		m_pButton[nBtnNum]->SetBgColor(DEF_COLOR_BTN_PAGE_NORMAL);
	//		m_pButton[nBtnNum]->SetTextColor(DEF_COLOR_BTN_MENU_NORMAL_TEXT);
	//	}
	//	m_pButton[nBtnNum]->Invalidate(TRUE);
	//}
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

	// 세타테이블
	for (int i = 0; i < 1; i++)
	{
		stTemp.strMotorName.Format(_T("Table θ"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((int)ENG_MMDI::en_axis_theta);
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
	int nHeightSize = 30;	/* 셀 높이 */
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
VOID CDlgMotr::InitUserPosGrid()
{
	CRect rctSize;
	LOGFONT lfFont = GetLogFont(20, TRUE);
	double dCellRation[eCELL_UPOS_MAX] = { 0.28, 0.18, 0.18, 0.18, 0.18 };
	int nHeightSize = 30;
	int nWidthDiffer = 0;

	CString strHeader[eCELL_UPOS_MAX] = { _T("Name"), _T("Pos X"), _T("Pos Y"),_T("Cam1X"),_T("Cam2X") };
	UINT nCenterAlignText = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;

	m_pGrid[eGRD_USER_POS]->GetClientRect(rctSize);
	m_pGrid[eGRD_USER_POS]->SetColumnResize(FALSE);
	m_pGrid[eGRD_USER_POS]->SetRowResize(FALSE);
	m_pGrid[eGRD_USER_POS]->SetEditable(FALSE);
	m_pGrid[eGRD_USER_POS]->EnableSelection(FALSE);
	m_pGrid[eGRD_USER_POS]->ModifyStyle(WS_HSCROLL, 0);

	m_pGrid[eGRD_USER_POS]->SetGridLineColor(BLACK_);
	m_pGrid[eGRD_USER_POS]->SetTextColor(BLACK_);

	m_pGrid[eGRD_USER_POS]->DeleteAllItems();

	int nRowCount = MAX_USER_POS + 1;
	m_pGrid[eGRD_USER_POS]->SetColumnCount(eCELL_UPOS_MAX);
	m_pGrid[eGRD_USER_POS]->SetRowCount(nRowCount);
	m_pGrid[eGRD_USER_POS]->SetFixedRowCount(1);

	if (rctSize.Height() - 1 < nHeightSize * nRowCount)
	{
		nWidthDiffer = ::GetSystemMetrics(SM_CXVSCROLL);
	}
	for (int nCol = 0;nCol < eCELL_UPOS_MAX; nCol++)
	{
		m_pGrid[eGRD_USER_POS]->SetColumnWidth(nCol, (int)(rctSize.Width() - nWidthDiffer)*dCellRation[nCol]);

		for (int nRow = 0;nRow < nRowCount;nRow++)
		{
			m_pGrid[eGRD_USER_POS]->SetRowHeight(nRow, nHeightSize);
			m_pGrid[eGRD_USER_POS]->SetItemFormat(nRow, nCol, nCenterAlignText);
			m_pGrid[eGRD_USER_POS]->SetItemFont(nRow, nCol, &lfFont);

			if (nRow == 0)
			{
				m_pGrid[eGRD_USER_POS]->SetItemText(nRow, nCol, strHeader[nCol]);
				m_pGrid[eGRD_USER_POS]->SetItemBkColour(nRow, nCol, RGB(91, 155, 231));
				m_pGrid[eGRD_USER_POS]->SetItemFgColour(nRow, nCol, WHITE_);
			}
			else
			{
				//데이터 행 기본 디자인
				if (eCELL_UPOS_NAME == nCol)
				{
					CString strName;
					strName.Format(_T("User Position%d"),nRow);
					m_pGrid[eGRD_USER_POS]->SetItemText(nRow, nCol, strName);
					m_pGrid[eGRD_USER_POS]->SetItemBkColour(nRow, nCol, RGB(242, 242, 242));
				}
				else
				{
					m_pGrid[eGRD_USER_POS]->SetItemText(nRow, nCol, _T("-"));
					m_pGrid[eGRD_USER_POS]->SetItemBkColour(nRow, nCol, WHITE_);
				}
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
	else
	{
		int axisIdx = (int)drv_id - (int)ENG_MMDI::en_axis_acam1;
		auto& ajinInst = GlobalVariables::GetInstance()->GetAjinMotion();
		auto* axisInfo = ajinInst.GetAxisInfo();
		bool postiveDir = dTargetPos > 0;
		CString strTemp;

		if (postiveDir)
		{
			if(axisInfo[axisIdx].isMaxLimit)
			strTemp.Format(_T("%d 축이 이동할 Position이 Max Position을 넘어서 움직일수 없습니다."), (int)axisIdx);
			
		}
		else
		{
			if (axisInfo[axisIdx].IsMinlimit)
				strTemp.Format(_T("%d 축이 이동할 Position이 Min Position을 넘어서 움직일수 없습니다."), (int)axisIdx);
		}
		if (strTemp.GetLength() != 0)
		{
			AfxMessageBox(strTemp);
			return;
		}
		string errDesc;
		if (FALSE == bIsRel) //상대
		{
			auto res = ajinInst.MoveAbs(axisIdx, dTargetPos, 5, errDesc);
		}
		else //절대
		{
			auto res = ajinInst.MoveRel(axisIdx, dTargetPos, 5, errDesc);
		}

		if (errDesc.length() != 0)
		{
			strTemp = CString(errDesc.c_str());
			AfxMessageBox(strTemp);
			return;
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
	const int ajinMotorAxisConut = 4;
	for (int nRow = 0; nRow < m_u8AllMotorCount- ajinMotorAxisConut; nRow++)
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

	auto* axisInfo = GlobalVariables::GetInstance()->GetAjinMotion().GetAxisInfo();

	for (int nRow = m_u8AllMotorCount - ajinMotorAxisConut,j=0; nRow < m_u8AllMotorCount; nRow++,j++)
	{

		dPosition = axisInfo == nullptr ? 0 : axisInfo[j].position;

		bool moving = axisInfo == nullptr ? false : axisInfo[j].isInposition;
		bool enable = axisInfo == nullptr ? false : axisInfo[j].isEnable;
		bool fault = axisInfo == nullptr ? false : axisInfo[j].isFault;
		bool hommed = axisInfo == nullptr ? false : axisInfo[j].isHommed;

		// On/Off 상태 확인
		// 230516 mhbaek Modify : Flag 반대
		if (!enable)
		{
			m_pGrid[eGRD_MOTOR]->SetItemBkColour(nRow, eCELL_MOTOR_LED_STATUS, DEF_RGB_TAB_NORMAL);
		}
		// 에러 상태 확인
		else if (fault)
		{
			m_pGrid[eGRD_MOTOR]->SetItemBkColour(nRow, eCELL_MOTOR_LED_STATUS, LIGHT_RED);
		}
		// 동작이 가능한 상태
		else
		{
			m_pGrid[eGRD_MOTOR]->SetItemBkColour(nRow, eCELL_MOTOR_LED_STATUS, LIGHT_GREEN);
		}

		dPosition = axisInfo == nullptr ? 0 : axisInfo[j].position;
		strPosition.Format(_T("%.4f"), dPosition);

		m_pGrid[eGRD_MOTOR]->SetItemText(nRow, eCELL_MOTOR_POS_VALUE, strPosition);
	}

	UpdataUserPosGrid();
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
	{
		// 전체 정지
		uvEng_MC2_SendDevStopped(m_stVctMotion[m_u8SelMotor].DeviceNum);
		auto& ajinInst = GlobalVariables::GetInstance()->GetAjinMotion();
		const int ajinMotorAxisConut = 4;

		for (int i = 0; i < ajinMotorAxisConut; i++)
			ajinInst.StopMotor(0, true);
	}
	break;
	case eBTN_UPOS_DISP:
	{
		if (m_nSelUserPos<1 || m_nSelUserPos>MAX_USER_POS)
		{
			AfxMessageBox(_T("변경할 행을 리스트에서 먼저 선택해 주세요."));
			return;
		}
		//문자열 입력 키보드 
		CDlgKBDT dlgKeyPad;
		TCHAR tzText[RECIPE_NAME_LENGTH] = { NULL };
		CString strNewName;
		if (IDOK == dlgKeyPad.MyDoModal(RECIPE_NAME_LENGTH))
		{
			//CString strNewName = dlgKeyPad.GetValusString();
			dlgKeyPad.GetText(tzText, RECIPE_NAME_LENGTH);
			strNewName.Format(_T("%s"), tzText);

			m_stUserPos[m_nSelUserPos - 1].strName = strNewName;
			m_pButton[eBTN_UPOS_DISP]->SetWindowText(strNewName);
			m_pButton[eBTN_UPOS_DISP]->Invalidate(TRUE);

			m_pGrid[eGRD_USER_POS]->SetItemText(m_nSelUserPos, eCELL_UPOS_NAME, strNewName);
			m_pGrid[eGRD_USER_POS]->Refresh();
		}

	}
		break;
	case eBTN_UPOS_GETPOS:
		GetPosUserPosition();
		break;
	case eBTN_UPOS_MOVE:
		MovePosUserPosition();
		break;
	case eBTN_UPOS_SAVE:
		SaveUserPosition();
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
	CString strSelName;
	
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
	case eGRD_USER_POS:
		if (pItem->iRow > 0)
		{
			UserPosSelect(pItem->iRow);

			strSelName = m_pGrid[eGRD_USER_POS]->GetItemText(pItem->iRow, eCELL_UPOS_NAME);
			m_pButton[eBTN_UPOS_DISP]->SetWindowText(strSelName);
			m_pButton[eBTN_UPOS_DISP]->Invalidate();
		}
		break;
	default:
		break;
	}
}

/*
 desc : 그리드의 더블 클릭 이벤트를 처리한다.
 parm : 그리드 리소스 ID, 이벤트가 발생된 셀 정보, 결과
 retn : None
*/
void CDlgMotr::OnGrdDblClickedEvent(UINT ID, NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	int nCommand = ID - IDC_GRD_MOTOR;

	if (pItem == nullptr || pItem->iRow <= 0 || pItem->iColumn == -1)
		return;

	switch (nCommand)
	{
	case eGRD_USER_POS:
	{
		//더블 클릭한 열(Column)이 좌표인지 확인
		if (pItem->iColumn >= eCELL_UPOS_POSX && pItem->iColumn <= eCELL_UPOS_CAM2X)
		{
			int nIdx = pItem->iRow - 1;

			//클릭한 위치의 좌표 데이터를 DEF_IGNORE_POS(-1.0)으로 변경
			switch (pItem->iColumn)
			{
			case eCELL_UPOS_POSX: m_stUserPos[nIdx].dPosX = DEF_IGNORE_POS; break;
			case eCELL_UPOS_POSY: m_stUserPos[nIdx].dPosY = DEF_IGNORE_POS; break;
			case eCELL_UPOS_CAM1X: m_stUserPos[nIdx].dCam1X = DEF_IGNORE_POS; break;
			case eCELL_UPOS_CAM2X: m_stUserPos[nIdx].dCam2X = DEF_IGNORE_POS; break;

			}
			UpdataUserPosGrid();
		}
	}
		break;
	defaule:
		break;
	
	}
	*pResult = 0;
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


VOID CDlgMotr::UpdataUserPosGrid()
{
	CString strTemp;

	//double 값 아치 비교 람다 함수(0.0001 이상 차이나면 변경된 것으로 간주)
	auto IsModified = [](double a, double b)->bool {
		return(a > b ? a - b : b - a) > 0.0001;
		};

	for (int i = 0;i < MAX_USER_POS;i++)
	{
		int nRow = i + 1;
		//Name 텍스트 반영
		m_pGrid[eGRD_USER_POS]->SetItemText(nRow, eCELL_UPOS_NAME, m_stUserPos[i].strName);

		auto SetGridText = [&](int nCol, double dVal) {
			if (dVal < 0.0) {
				m_pGrid[eGRD_USER_POS]->SetItemText(nRow, nCol, _T("-"));
			}
			else {
				strTemp.Format(_T("%.4f"), dVal);
				m_pGrid[eGRD_USER_POS]->SetItemText(nRow, nCol, strTemp);
			}
		};

		SetGridText(eCELL_UPOS_POSX, m_stUserPos[i].dPosX);
		SetGridText(eCELL_UPOS_POSY, m_stUserPos[i].dPosY);
		SetGridText(eCELL_UPOS_CAM1X, m_stUserPos[i].dCam1X);
		SetGridText(eCELL_UPOS_CAM2X, m_stUserPos[i].dCam2X);


		//if (nRow == m_nSelUserPos)
		//{
		//	for (int nCol = 0;nCol < eCELL_UPOS_MAX;nCol++)
		//	{
		//		m_pGrid[eGRD_USER_POS]->SetItemBkColour(nRow, nCol, RGB(0, 112, 192));
		//		m_pGrid[eGRD_USER_POS]->SetItemFgColour(nRow, nCol, WHITE_);
		//	}
		//}
		//else
		//{
		//	for (int nCol = 0;nCol < eCELL_UPOS_MAX;nCol++)
		//	{
		//		if (nCol == eCELL_UPOS_NAME) {
		//			m_pGrid[eGRD_USER_POS]->SetItemBkColour(nRow, nCol, RGB(242, 242, 242));
		//		}
		//		else{
		//			m_pGrid[eGRD_USER_POS]->SetItemBkColour(nRow, nCol, WHITE_);
		//		}
		//		m_pGrid[eGRD_USER_POS]->SetItemFgColour(nRow, nCol, BLACK_);
		//	}
		//}

		// 각 셀마다 변경(Dirty) 상태를 파악하여 색상 적용
		for (int nCol = 0; nCol < eCELL_UPOS_MAX; nCol++)
		{
			bool bIsChanged = false; // 변경 여부 플래그

			// 현재 셀의 데이터가 원본 백업본과 다른지 검사
			switch (nCol)
			{
			case eCELL_UPOS_NAME:  bIsChanged = (m_stUserPos[i].strName != m_stSaveUserPos[i].strName); break;
			case eCELL_UPOS_POSX:  bIsChanged = IsModified(m_stUserPos[i].dPosX, m_stSaveUserPos[i].dPosX); break;
			case eCELL_UPOS_POSY:  bIsChanged = IsModified(m_stUserPos[i].dPosY, m_stSaveUserPos[i].dPosY); break;
			case eCELL_UPOS_CAM1X: bIsChanged = IsModified(m_stUserPos[i].dCam1X, m_stSaveUserPos[i].dCam1X); break;
			case eCELL_UPOS_CAM2X: bIsChanged = IsModified(m_stUserPos[i].dCam2X, m_stSaveUserPos[i].dCam2X); break;
			}

			// 배경 및 텍스트 색상 결정
			COLORREF clrBg, clrText;
			if (nRow == m_nSelUserPos)
			{
				// 선택된 행 (파란색 배경)
				clrBg = RGB(0, 112, 192);
				clrText = bIsChanged ? RGB(255, 255, 0) : WHITE_; // 변경 시 노란색, 아니면 흰색
			}
			else
			{
				// 선택되지 않은 행
				clrBg = (nCol == eCELL_UPOS_NAME) ? RGB(242, 242, 242) : WHITE_;
				clrText = bIsChanged ? RGB(255, 0, 0) : BLACK_; // 변경 시 빨간색, 아니면 검은색
			}

			m_pGrid[eGRD_USER_POS]->SetItemBkColour(nRow, nCol, clrBg);
			m_pGrid[eGRD_USER_POS]->SetItemFgColour(nRow, nCol, clrText);
		}
	}
	m_pGrid[eGRD_USER_POS]->Refresh();
}

VOID CDlgMotr::LoadUserPosition()
{
	uvEng_UserPosition_LoadFile();

	for (int i = 0; i < MAX_USER_POS;i++)
	{
		LPG_UPTP pData = uvEng_UserPosition_GetUserPosData(i);

		if (pData != NULL)
		{
			m_stUserPos[i].strName = pData->strName;
			m_stUserPos[i].dPosX = pData->dPosX;
			m_stUserPos[i].dPosY = pData->dPosY;
			m_stUserPos[i].dCam1X = pData->dCam1X;
			m_stUserPos[i].dCam2X = pData->dCam2X;
		}
		else
		{
			m_stUserPos[i].strName.Format(_T("User Position%d"), i + 1);
			m_stUserPos[i].dPosX = DEF_IGNORE_POS;
			m_stUserPos[i].dPosY = DEF_IGNORE_POS;
			m_stUserPos[i].dCam1X = DEF_IGNORE_POS;
			m_stUserPos[i].dCam2X = DEF_IGNORE_POS;
		}
		//로드된 데이터를 백업 배열에도 복사(변경 사항 비교)
		m_stSaveUserPos[i].strName = m_stUserPos[i].strName;
		m_stSaveUserPos[i].dPosX = m_stUserPos[i].dPosX;
		m_stSaveUserPos[i].dPosY = m_stUserPos[i].dPosY;
		m_stSaveUserPos[i].dCam1X = m_stUserPos[i].dCam1X;
		m_stSaveUserPos[i].dCam2X = m_stUserPos[i].dCam2X;
	}
}

VOID CDlgMotr::SaveUserPosition()
{
	for (int i = 0;i < MAX_USER_POS;i++)
	{
		//LPG_UPTP pData = uvEng_UserPosition_GetUserPosData(i);

		for (int i = 0;i < MAX_USER_POS;i++)
		{
			uvEng_UserPosition_SetUserPosData(
				i,
				(LPCTSTR)m_stUserPos[i].strName,
				m_stUserPos[i].dPosX,
				m_stUserPos[i].dPosY,
				m_stUserPos[i].dCam1X,
				m_stUserPos[i].dCam2X
			);
		}
	}
	//엔진 API를 호출하여 메모리 데이터를 파일에 저장
	if (uvEng_UserPosition_SaveFile())
	{
		//저장이 성공하면 현재 데이터를 원본 데이터로 덮어씌움
		for (int i = 0; i < MAX_USER_POS; i++)
		{
			m_stSaveUserPos[i].strName = m_stUserPos[i].strName;
			m_stSaveUserPos[i].dPosX = m_stUserPos[i].dPosX;
			m_stSaveUserPos[i].dPosY = m_stUserPos[i].dPosY;
			m_stSaveUserPos[i].dCam1X = m_stUserPos[i].dCam1X;
			m_stSaveUserPos[i].dCam2X = m_stUserPos[i].dCam2X;
		}

		//색상을 원래대로 되돌리기 위해 
		UpdataUserPosGrid();
		AfxMessageBox(_T("User Position 데이터가 성공적으로 저장되었습니다"));
	}
	else
	{
		AfxMessageBox(_T("파일 저장에 실패했습니다"),MB_ICONERROR);
	}
}

VOID CDlgMotr::UserPosSelect(int nRow)
{
	if (nRow <= 0 || nRow == m_nSelUserPos) return;

	if (m_nSelUserPos > 0 && m_nSelUserPos <= MAX_USER_POS)
	{
		for (int nCol = 0;nCol < eCELL_UPOS_MAX;nCol++)
		{
			if (nCol == eCELL_UPOS_NAME) {
				m_pGrid[eGRD_USER_POS]->SetItemBkColour(m_nSelUserPos, nCol, RGB(242, 242, 242));
			}
			else{
				m_pGrid[eGRD_USER_POS]->SetItemBkColour(m_nSelUserPos, nCol, WHITE_);
			}
			m_pGrid[eGRD_USER_POS]->SetItemFgColour(m_nSelUserPos, nCol, BLACK_);
		}
	}
	for (int nCol = 0;nCol < eCELL_UPOS_MAX; nCol++)
	{
		m_pGrid[eGRD_USER_POS]->SetItemBkColour(nRow, nCol, RGB(0, 112, 192));
		m_pGrid[eGRD_USER_POS]->SetItemFgColour(nRow, nCol, WHITE_);
	}

	m_nSelUserPos = nRow;
	m_pGrid[eGRD_USER_POS]->Refresh();
}

VOID CDlgMotr::GetPosUserPosition()
{
	if (m_nSelUserPos<1 || m_nSelUserPos>MAX_USER_POS)
	{
		AfxMessageBox(_T("리스트에서 위치를 저장할 핼을먼저 선택해 주세요."));
	}

	int nIdx = m_nSelUserPos - 1;

	//현재 장비의 절대 좌표 읽어오기
	m_stUserPos[nIdx].dPosX = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
	m_stUserPos[nIdx].dPosY = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
	m_stUserPos[nIdx].dCam1X = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_axis_acam1);
	m_stUserPos[nIdx].dCam2X = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_axis_acam2);

	//갤싱된 데이터를 화면에 다시 그리기
	UpdataUserPosGrid();
}

VOID CDlgMotr::MovePosUserPosition()
{
	if (m_nSelUserPos<1 || m_nSelUserPos>MAX_USER_POS) return;
	if (m_nSelUserPos <= 0.0) return;

	int nIdx = m_nSelUserPos - 1;

	if (m_stUserPos[nIdx].dPosX >= 0.0) {
		MoveStart(ENG_MMDI::en_stage_x, m_stUserPos[nIdx].dPosX, m_dSetSpeed, FALSE);
	}
	if (m_stUserPos[nIdx].dPosY >= 0.0) {
		MoveStart(ENG_MMDI::en_stage_y, m_stUserPos[nIdx].dPosY, m_dSetSpeed, FALSE);
	}
	if (m_stUserPos[nIdx].dCam1X >= 0.0) {
		MoveStart(ENG_MMDI::en_axis_acam1, m_stUserPos[nIdx].dCam1X, m_dSetSpeed, FALSE);
	}
	if (m_stUserPos[nIdx].dCam2X >= 0.0) {
		MoveStart(ENG_MMDI::en_axis_acam2, m_stUserPos[nIdx].dCam2X, m_dSetSpeed, FALSE);
	}
}

/*
 desc : User Position 관련 UI(타이틀, 그리드, 버튼)를 화면에서 숨기거나 표시한다.
 parm : bShow - TRUE(표시) / FALSE(숨김 및 창 크기 축소)
 retn : None
*/
VOID CDlgMotr::ShowUserPositionUI(BOOL bShow)
{
	int nCmdShow = bShow ? SW_SHOW : SW_HIDE;

	//User Positin 타이틀 바 숨기기
	m_sttTitle[eTITLE_USER_POSITION].ShowWindow(nCmdShow);

	//User Position 그리드 숨기기
	if (m_pGrid[eGRD_USER_POS])
	{
		m_pGrid[eGRD_USER_POS]->ShowWindow(nCmdShow);
	}

	//하단 버튼들 숨기기
	for (int i = eBTN_UPOS_NAME;i <= eBTN_UPOS_SAVE;i++)
	{
		if (m_pButton[i])
		{
			m_pButton[i]->ShowWindow(nCmdShow);
		}
	}

	//숨김(FALSE) 처리 시, 밑에 남는 빈 공간을 잘라내어 다어얼로그 크기를 줄입
	if (bShow == FALSE)
	{
		CRect rctDlg, rctTitle;
		GetWindowRect(rctDlg);

		m_sttTitle[eTITLE_USER_POSITION].GetWindowRect(rctTitle);

		rctDlg.bottom = rctTitle.top;
		MoveWindow(rctDlg);
	}
}