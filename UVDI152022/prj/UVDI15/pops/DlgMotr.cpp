
/*
 desc : ���α׷� ����� ��, ��� �˸�
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
 desc : ������
 parm : id		- [in]  �ڽ��� ���� ID
		pParent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
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

	m_u8ACamCount = uvEng_GetConfig()->set_cams.acam_count;		/* ī�޶� ������ �����´�. */
	m_u8HeadCount = uvEng_GetConfig()->luria_svc.ph_count;		/* Photo Head ������ �����´�. */
	m_u8StageCount = uvEng_GetConfig()->luria_svc.table_count;	/* Table�� ������ �����´�. */

	// Stage ����(x, y��) + Head ���� + Align Camera ����
	m_u8AllMotorCount = (m_u8StageCount * 2) + m_u8HeadCount + (m_u8ACamCount * 2);
	
	/* ��� ���� �ʱ�ȭ */
	m_u8SelMotor = 0;
	m_stVctMotion.clear();
	m_bMoveType = eCELL_TAB_ABSOLUTE_MOVE;
	m_dSetSpeed = 0.;
	m_dSetPosition = 0.;
}

/*
 desc : �Ҹ���
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
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
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
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
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
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMotr::OnInitDlg()
{
	/* TOP_MOST & Center */
	// CenterParentTopMost();
	CDialogEx::OnInitDialog();

	/* ���� ���� �ʱ�ȭ */
	InitMotionIndex();

	/* ��Ʈ�� ���� */
	CreateControl();

	/* Grid �ʱ�ȭ */
	InitMotorGrid();
	InitOpTabGrid();
	InitOpInputGrid();
	InitControlGrid();

	return TRUE;
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgMotr::OnPaintDlg(CDC * pDc)
{
}

/*
 desc : �� ��Ʈ�ѵ��� �����Ѵ�.
 parm : None
 retn : None
*/
VOID CDlgMotr::CreateControl()
{
	CRect rctDlgSize;						/* �۾� ���� */
	CRect rctTitleSize[eTITLE_MAX];			/* Ÿ��Ʋ �� ���� */
	CRect rctGrdSize[eGRD_MAX];				/* �׸��� ���� */
	CRect rctBtnSize[eBTN_MAX];				/* ��ư ���� */
	CPoint ptErrorSize;						/* �����쿡���� ��ġ�� �۾� �������� ���� ��ǥ */
	LONG lBottomBlockSize = 0;				/* Operation �������� 1Cell�� ���� �� */

	LOGFONT	lfBold = GetLogFont(28, TRUE);	/* ��Ʈ ���� */

	CString strArrCaption[eBTN_MAX] = { _T("+"), _T("-"), _T("STOP") };	/* ��ư�� ���� ���� */

	// ��ü �۾� ������ �����´�.
	GetClientRect(rctDlgSize);

	// Ÿ��Ʋ �� ����� �������� �׸����� ������ ���� ��
	for (int nTitleNum = 0; nTitleNum < eTITLE_MAX; nTitleNum++)
	{
		// Ÿ��Ʋ ���� ����� �����´�.
		GetDlgItem(IDC_TITLE_MOTOR + nTitleNum)->GetWindowRect(rctTitleSize[nTitleNum]);

		// ù��° Ÿ��Ʋ ���� ������ �����´ٸ� Window������ ��ġ�� �۾� ��ġ�� �������� �����Ѵ�.
		if (eTITLE_MOTOR == nTitleNum)
		{
			ptErrorSize.x = rctTitleSize[nTitleNum].left;
			ptErrorSize.y = rctTitleSize[nTitleNum].top;
		}

		// ���� ���� �ݿ��Ͽ� �����Ѵ�.
		rctTitleSize[nTitleNum].left -= ptErrorSize.x;
		rctTitleSize[nTitleNum].top -= ptErrorSize.y;
		rctTitleSize[nTitleNum].right -= ptErrorSize.x;
		rctTitleSize[nTitleNum].bottom -= ptErrorSize.y;

		m_sttTitle[nTitleNum].SetTextFont(&lfBold);
		m_sttTitle[nTitleNum].SetDrawBg(TRUE);
		m_sttTitle[nTitleNum].SetBgColor(DEF_COLOR_BTN_MENU_NORMAL);
		m_sttTitle[nTitleNum].SetTextColor(DEF_COLOR_BTN_MENU_NORMAL_TEXT);
	}

	// Motor �׸� ��ǥ ��
	rctGrdSize[eGRD_MOTOR].left = rctDlgSize.left + 1;
	rctGrdSize[eGRD_MOTOR].top = rctTitleSize[eTITLE_MOTOR].bottom;
	rctGrdSize[eGRD_MOTOR].right = rctTitleSize[eTITLE_CONTROL].left;
	rctGrdSize[eGRD_MOTOR].bottom = rctDlgSize.bottom;

	// Control �׸� ��ǥ ��
	rctGrdSize[eGRD_CONTROL].left = rctTitleSize[eTITLE_CONTROL].left;
	rctGrdSize[eGRD_CONTROL].top = rctTitleSize[eTITLE_CONTROL].bottom;
	rctGrdSize[eGRD_CONTROL].right = rctDlgSize.right;
	rctGrdSize[eGRD_CONTROL].bottom = rctTitleSize[eTITLE_OPERATION].top;

	// Operation �׸� ��ǥ ��
	lBottomBlockSize = (rctDlgSize.bottom - rctTitleSize[eTITLE_OPERATION].bottom) / 5;

	rctGrdSize[eGRD_OPERATION_TAB].left = rctTitleSize[eTITLE_CONTROL].left;
	rctGrdSize[eGRD_OPERATION_TAB].top = rctTitleSize[eTITLE_OPERATION].bottom;
	rctGrdSize[eGRD_OPERATION_TAB].right = rctDlgSize.right - 1;
	rctGrdSize[eGRD_OPERATION_TAB].bottom = rctGrdSize[eGRD_OPERATION_TAB].top + lBottomBlockSize;

	rctGrdSize[eGRD_OPERATION_INPUT].left = rctTitleSize[eTITLE_CONTROL].left;
	rctGrdSize[eGRD_OPERATION_INPUT].top = rctGrdSize[eGRD_OPERATION_TAB].bottom;
	rctGrdSize[eGRD_OPERATION_INPUT].right = rctDlgSize.right - 1;
	rctGrdSize[eGRD_OPERATION_INPUT].bottom = rctGrdSize[eGRD_OPERATION_INPUT].top + (lBottomBlockSize * 2);

	// Grid�� �����Ѵ�.
	for (int nGrdNum = 0; nGrdNum < eGRD_MAX; nGrdNum++)
	{
		m_pGrid[nGrdNum] = new CGridCtrl;
		ASSERT(m_pGrid[nGrdNum]);

		m_pGrid[nGrdNum]->Create(rctGrdSize[nGrdNum], this, IDC_GRD_MOTOR + nGrdNum, (WS_CHILD | WS_TABSTOP | WS_VISIBLE));
	}

	// ��ư���� ��ǥ ��
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

	// ��ư�� �����Ѵ�.
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
 desc : ������ ������ �ɹ� ���� ���Ϳ� �����Ѵ�.
 parm : None
 retn : None
*/
VOID CDlgMotr::InitMotionIndex()
{
	ST_MOTION stTemp;		/* ���� �ӽ� ���� */

	m_stVctMotion.clear();

	// Stage X, Y �׸� ����
	for (int i = 0; i < m_u8StageCount; i++)
	{
		stTemp.strMotorName.Format(_T("STAGE%d X"), i + 1);
		stTemp.DeviceNum = ENG_MMDI(i * 2);
		m_stVctMotion.push_back(stTemp);

		stTemp.strMotorName.Format(_T("STAGE%d Y"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((i * 2) + 1);
		m_stVctMotion.push_back(stTemp);
	}

	// Align Camera X �׸� ����
	for (int i = 0; i < m_u8ACamCount; i++)
	{
		stTemp.strMotorName.Format(_T("CAM%d X"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((int)ENG_MMDI::en_align_cam1 + i);
		m_stVctMotion.push_back(stTemp);
	}

	// Photo Head Z �׸� ����
	for (int i = 0; i < m_u8HeadCount; i++)
	{
		stTemp.strMotorName.Format(_T("PH%d Z"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((int)ENG_MMDI::en_axis_ph1 + i);
		m_stVctMotion.push_back(stTemp);
	}

	// Align Camera Z �׸� ����
	for (int i = 0; i < m_u8ACamCount; i++)
	{
		stTemp.strMotorName.Format(_T("CAM%d Z"), i + 1);
		stTemp.DeviceNum = ENG_MMDI((int)ENG_MMDI::en_axis_acam1 + i);
		m_stVctMotion.push_back(stTemp);
	}
}

/*
 desc : Motor Grid�� �ʱ�ȭ�Ѵ�.
 parm : None
 retn : None
*/
VOID CDlgMotr::InitMotorGrid()
{
	CRect rctSize;					/* �۾� ���� */

	LOGFONT	lfFont = GetLogFont(20, TRUE);	/* ��Ʈ ���� */

	double dCellRatio[eCELL_MOTOR_MAX] = { 0.1, 0.3, 0.45, 0.15 };	/* �׸��� ���� */
	int nHeightSize = 40;	/* �� ���� */
	int	nWidthDiffer = 0;	/* �� �ʺ� ������ */

	CString strUnit = _T("mm");			/* ���� ���� */

	UINT nCenterAlignText = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;	/* �׸��� �ɼ� */

	// �׸��� �⺻ ����
	m_pGrid[eGRD_MOTOR]->GetClientRect(rctSize);
	m_pGrid[eGRD_MOTOR]->SetColumnResize(FALSE);
	m_pGrid[eGRD_MOTOR]->SetRowResize(FALSE);
	m_pGrid[eGRD_MOTOR]->SetEditable(FALSE);
	m_pGrid[eGRD_MOTOR]->EnableSelection(FALSE);
	m_pGrid[eGRD_MOTOR]->ModifyStyle(WS_HSCROLL, 0);

	m_pGrid[eGRD_MOTOR]->SetGridLineColor(BLACK_);
	m_pGrid[eGRD_MOTOR]->SetTextColor(BLACK_);

	m_pGrid[eGRD_MOTOR]->DeleteAllItems();

	// �׸��� ������ �۾� ������ �ʰ��ϸ� ��ũ�� �� ��ŭ�� ������ ����
	if (rctSize.Height() - 1 < nHeightSize * m_u8AllMotorCount)
	{
		nWidthDiffer = ::GetSystemMetrics(SM_CXVSCROLL);
	}

	// ���� ���� ����
	m_pGrid[eGRD_MOTOR]->SetColumnCount(eCELL_MOTOR_MAX);
	m_pGrid[eGRD_MOTOR]->SetRowCount(m_u8AllMotorCount);

	// ����, �ʺ�, ����, ���� ����
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
 desc : Control Grid�� �ʱ�ȭ�Ѵ�.
 parm : None
 retn : None
*/
VOID CDlgMotr::InitControlGrid()
{
	CRect rctSize;	/* �۾� ���� */
	LOGFONT	lfFont = GetLogFont(20, TRUE);	/* ��Ʈ ���� */
	CString strCaption[eCELL_CTRL_MAX] = { _T("INIT"), _T("ON/OFF"), _T("RESET") };	/* ���� �Էµ� ���� */
	UINT nCenterAlignText = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;	/* �� �Ӽ� */

	// �׸��� �⺻ ����
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

	// ���� ���� ����
	m_pGrid[eGRD_CONTROL]->SetColumnCount(eCELL_CTRL_MAX);
	m_pGrid[eGRD_CONTROL]->SetRowCount(1);

	// �� ���� ����
	m_pGrid[eGRD_CONTROL]->SetRowHeight(0, rctSize.Height() - 1);

	// �� �ʺ� �� ����, ��Ʈ ����
	for (int nCol = 0; nCol < eCELL_CTRL_MAX; nCol++)
	{
		m_pGrid[eGRD_CONTROL]->SetColumnWidth(nCol, (int)((rctSize.Width()) / eCELL_CTRL_MAX));
		m_pGrid[eGRD_CONTROL]->SetItemFormat(0, nCol, nCenterAlignText);
		m_pGrid[eGRD_CONTROL]->SetItemText(0, nCol, strCaption[nCol]);
		m_pGrid[eGRD_CONTROL]->SetItemFont(0, nCol, &lfFont);
	}
}

/*
 desc : Tap Grid�� �ʱ�ȭ�Ѵ�.
 parm : None
 retn : None
*/
VOID CDlgMotr::InitOpTabGrid()
{
	CRect rctSize;	/* �۾� ���� */
	LOGFONT	lfFont = GetLogFont(20, TRUE);	/* ��Ʈ ���� */
	CString strCaption[eCELL_TAB_MAX] = { _T("ABS"), _T("REL") };	/* ���� �Էµ� ���� */
	UINT nCenterAlignText = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;	/* �� �Ӽ� */

	// �׸��� �⺻ ����
	m_pGrid[eGRD_OPERATION_TAB]->GetClientRect(rctSize);
	m_pGrid[eGRD_OPERATION_TAB]->SetColumnResize(FALSE);
	m_pGrid[eGRD_OPERATION_TAB]->SetRowResize(FALSE);
	m_pGrid[eGRD_OPERATION_TAB]->SetEditable(FALSE);
	m_pGrid[eGRD_OPERATION_TAB]->EnableSelection(FALSE);
	m_pGrid[eGRD_OPERATION_TAB]->ModifyStyle(WS_HSCROLL, 0);

	m_pGrid[eGRD_OPERATION_TAB]->SetTextColor(WHITE_);
	m_pGrid[eGRD_OPERATION_TAB]->SetGridLineColor(BLACK_);

	m_pGrid[eGRD_OPERATION_TAB]->DeleteAllItems();

	// ���� ���� ����
	m_pGrid[eGRD_OPERATION_TAB]->SetColumnCount(eCELL_TAB_MAX);
	m_pGrid[eGRD_OPERATION_TAB]->SetRowCount(1);

	// �� ���� ����
	m_pGrid[eGRD_OPERATION_TAB]->SetRowHeight(0, rctSize.Height() - 1);

	// �� �ʺ� �� ����, ��Ʈ ����
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
 desc : Input Grid�� �ʱ�ȭ�Ѵ�.
 parm : None
 retn : None
*/
VOID CDlgMotr::InitOpInputGrid()
{
	CRect rctSize;	/* �۾� ���� */
	LOGFONT	lfFont = GetLogFont(20, TRUE);	/* ��Ʈ ���� */

	double dCellRatio[eCELL_INPUT_COL_MAX] = { 0.3, 0.5, 0.2 };	/* �� ���� */
	
	CString strCaption[eCELL_INPUT_ROW_MAX][eCELL_INPUT_COL_MAX] = {	/* ���� �Էµ� ���� */
		{_T("SPEED"), _T("0"), _T("mm/s")},
		{_T("POSITION"), _T("0"), _T("mm")}
	};

	UINT nCenterAlignText = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;	/* �� �Ӽ� */

	// �׸��� �⺻ ����
	m_pGrid[eGRD_OPERATION_INPUT]->GetClientRect(rctSize);
	m_pGrid[eGRD_OPERATION_INPUT]->SetColumnResize(FALSE);
	m_pGrid[eGRD_OPERATION_INPUT]->SetRowResize(FALSE);
	m_pGrid[eGRD_OPERATION_INPUT]->SetEditable(FALSE);
	m_pGrid[eGRD_OPERATION_INPUT]->EnableSelection(FALSE);
	m_pGrid[eGRD_OPERATION_INPUT]->ModifyStyle(WS_HSCROLL, 0);

	m_pGrid[eGRD_OPERATION_INPUT]->SetTextColor(BLACK_);
	m_pGrid[eGRD_OPERATION_INPUT]->SetGridLineColor(BLACK_);

	m_pGrid[eGRD_OPERATION_INPUT]->DeleteAllItems();

	// ���� ���� ����
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
 desc : LogFont�� �����Ѵ�.
 parm : ��Ʈ ������, ���� �۾� ��� ���� 
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
	
	/*Mc2 ����*/
	if (drv_id < ENG_MMDI::en_axis_acam1)
	{
		if (FALSE == uvCmn_MC2_IsDevLocked(drv_id))
		{
			AfxMessageBox(_T("���� ���Ͱ� Off �����Դϴ�."));
			return;
		}
		// ���� ���� Ȯ��
		else if (TRUE == uvCmn_MC2_IsDriveError(drv_id))
		{
			AfxMessageBox(_T("���� ���Ͱ� ���� �����Դϴ�."));
			return;
		}
		// Busy ��ȣ Ȯ��
		else if (TRUE == uvCmn_MC2_IsDriveBusy(drv_id))
		{
			AfxMessageBox(_T("���� ���Ͱ� ���� �����Դϴ�."));
			return;
		}

		// ��� �̵��� ���
		if (TRUE == bIsRel)
		{
			dTargetPos = uvCmn_MC2_GetDrvAbsPos(drv_id) + dPosition;
		}

		if (drv_id < ENG_MMDI::en_axis_ph1)
		{
			if (uvEng_GetConfig()->mc2_svc.max_dist[(int)drv_id] < dTargetPos)
			{
				CString strTemp;
				strTemp.Format(_T("%d ���� �̵��� Position�� Max Position�� �Ѿ �����ϼ� �����ϴ�."), (int)drv_id);
				AfxMessageBox(strTemp);
				return;
			}
		}
		else if (drv_id < ENG_MMDI::en_axis_acam1 && drv_id > ENG_MMDI::en_align_cam2)
		{
			if (uvEng_GetConfig()->luria_svc.ph_z_move_max < dTargetPos)
			{
				CString strTemp;
				strTemp.Format(_T("%d ���� �̵��� Position�� Max Position�� �Ѿ �����ϼ� �����ϴ�."), (int)drv_id);
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
	/*Philhmi ����*/
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
 desc : ���� ���¸� �����Ѵ�.
 parm : ���� ���¸� �Է� �޴´�. (0 : ABS, 1 : REL)
 retn : None
*/
VOID CDlgMotr::ChangeMoveTpye(UINT8 u8Type)
{
	// ���� ������ ���� ������ ���� �Է� �޾Ҵٸ� �����Ѵ�.
	if ((BOOL)u8Type == m_bMoveType)	return;

	// �Է� ���� ������ ����
	m_bMoveType = (BOOL)u8Type;

	// UI ȭ�鿡 ����
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

	// Grid ����
	m_pGrid[eGRD_OPERATION_TAB]->Refresh();
}

/*
 desc : �Է¶��� �Է� ������ �����Ѵ�.
 parm : �Է��� �Ķ���� ����, ���� ��
 retn : None
*/
VOID CDlgMotr::InputParameter(UINT8 u8Sel)
{
	// ���� ������ ���� ������ ���� �Է� �޾Ҵٸ� �����Ѵ�.
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

	


	// UI ȭ�鿡 ����
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

	// Grid ����
	strValue.Format(_T("%.4f"), dValue);

	// �Ҽ��� �ڸ� �������� 0�� ������ ��� �ڸ���
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
 desc : ���͸� �����Ѵ�.
 parm : ���õ� ����
 retn : None
*/
VOID CDlgMotr::MotorSelect(UINT8 u8Sel)
{
	// ���� ������ ���� ������ ���� �Է� �޾Ҵٸ� �����Ѵ�.
	if (u8Sel == m_u8SelMotor)	return;

	// ���ſ� ���õǾ��� �׸��� ������ �����Ѵ�.
	for (int nCol = 1; nCol < eCELL_MOTOR_MAX; nCol++)
	{
		if (eCELL_MOTOR_POS_VALUE == nCol)
			m_pGrid[eGRD_MOTOR]->SetItemBkColour(m_u8SelMotor, nCol, WHITE_);
		else
			m_pGrid[eGRD_MOTOR]->SetItemBkColour(m_u8SelMotor, nCol, DEF_RGB_LABEL);
	}

	// ���� ���õ� �׸��� ������ �����Ѵ�.
	for (int nCol = 1; nCol < eCELL_MOTOR_MAX; nCol++)
	{
		m_pGrid[eGRD_MOTOR]->SetItemBkColour(u8Sel, nCol, DEF_RGB_LABEL_SEL);
	}

	// ȭ�� ����
	m_pGrid[eGRD_MOTOR]->Refresh();
	m_u8SelMotor = u8Sel;
}

/*
desc : ���� ������ �����Ѵ�.
parm : ������ ����
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
 desc : Motor Grid�� �����Ѵ�.
 parm : None
 retn : None
*/
VOID CDlgMotr::UpdateMotorStatus()
{
	double dPosition = 0.;
	CString strPosition;

	for (int nRow = 0; nRow < m_u8AllMotorCount; nRow++)
	{
		// On/Off ���� Ȯ��
		// 230516 mhbaek Modify : Flag �ݴ�
		if (FALSE == uvCmn_MC2_IsDevLocked(m_stVctMotion[nRow].DeviceNum))
		{
			m_pGrid[eGRD_MOTOR]->SetItemBkColour(nRow, eCELL_MOTOR_LED_STATUS, DEF_RGB_TAB_NORMAL);
		}
		// ���� ���� Ȯ��
		else if (TRUE == uvCmn_MC2_IsDriveError(m_stVctMotion[nRow].DeviceNum))
		{
			m_pGrid[eGRD_MOTOR]->SetItemBkColour(nRow, eCELL_MOTOR_LED_STATUS, LIGHT_RED);
		}
		// ������ ������ ����
		else
		{
			m_pGrid[eGRD_MOTOR]->SetItemBkColour(nRow, eCELL_MOTOR_LED_STATUS, LIGHT_GREEN);
		}

		dPosition = uvCmn_MC2_GetDrvAbsPos(m_stVctMotion[nRow].DeviceNum);
		strPosition.Format(_T("%.4f"), dPosition);

		m_pGrid[eGRD_MOTOR]->SetItemText(nRow, eCELL_MOTOR_POS_VALUE, strPosition);
	}

	// ȭ�� ����
	m_pGrid[eGRD_MOTOR]->Refresh();
}

/*
 desc : ��ư �̺�Ʈ�� ó���Ѵ�.
 parm : ��ư ���ҽ� ID
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
		// ������
		dPos = (m_bMoveType == eCELL_TAB_RELATIVE_MOVE) ? dPos * -1.0 : dPos;
		MoveStart(m_stVctMotion[m_u8SelMotor].DeviceNum, dPos, m_dSetSpeed, m_bMoveType);
		break;
	case eBTN_STOP:
		// ��ü ����
		uvEng_MC2_SendDevStopped(m_stVctMotion[m_u8SelMotor].DeviceNum);
		break;
	default:
		break;
	}
}

/*
 desc : �׸����� �̺�Ʈ�� ó���Ѵ�.
 parm : �׸��� ���ҽ� ID, �̺�Ʈ�� �߻��� ��
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
 desc : ���� �ð����� ������ �����Ѵ�.
 parm : Ÿ�̸� ID
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
 desc : ȭ���� ����� �ִ��� Ȯ���Ѵ�.
 parm : ���� ȭ���� ���̴����� ���� ���ο� ����
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
		// ȭ�鿡 ������ �ʴ� ��Ȳ���� Ÿ�̸Ӹ� ���� �ʿ�� ����.
		KillTimer(DEF_TIMER_MOTOR_CONSOLE);
	}
}

/*
 desc : ��ܹٿ� ���� �̺�Ʈ�� �޴´�.
 parm : ��ܹٿ��� �߻��� �̺�Ʈ ID
 retn : None
*/
void CDlgMotr::OnSysCommand(UINT nID, LPARAM lParam)
{
	//�����ư ���� ��
	if (nID == SC_CLOSE)
	{
		// ȭ�鿡 ������ �ʴ� ��Ȳ���� Ÿ�̸Ӹ� ���� �ʿ�� ����.
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