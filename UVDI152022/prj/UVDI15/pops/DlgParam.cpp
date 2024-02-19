
/*
 desc : ���α׷� ����� ��, ��� �˸�
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgParam.h"
#include "../mesg/DlgMesg.h"
#include "../../../inc/kybd/DlgKBDT.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

IMPLEMENT_DYNAMIC(CDlgParam, CDialogEx)

/*
 desc : ������
 parm : id		- [in]  �ڽ��� ���� ID
		pParent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
 retn : None
*/
CDlgParam::CDlgParam(CWnd* pParent /*=NULL*/)
	: CMyDialog(CDlgParam::IDD, pParent)
{
	m_pstVctParam = nullptr;
}

/*
 desc : �Ҹ���
 parm : None
 retn : None
*/
CDlgParam::~CDlgParam()
{
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgParam::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PARAM_BTN_CONFIRM, m_btnConfirm);
	DDX_Control(pDX, IDC_PARAM_GRD_PARAM, m_grdParam);
}

BEGIN_MESSAGE_MAP(CDlgParam, CMyDialog)
	ON_BN_CLICKED(IDC_PARAM_BTN_CONFIRM, &CDlgParam::OnBnClickedParamBtnConfirm)
	ON_NOTIFY(NM_CLICK, IDC_PARAM_GRD_PARAM, &CDlgParam::OnClickGridParam)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgParam::PreTranslateMessage(MSG* msg)
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


/*
 desc : DoModal Overriding
 parm : �Ķ���� �Է� ����ü �迭
 retn : INT_PTR
*/
INT_PTR CDlgParam::MyDoModal(VCT_DLG_PARAM &stVctParam)
{
	if (0 >= stVctParam.size())
	{
		return -1;
	}

	m_pstVctParam = &stVctParam;
	m_stVctBackUp = *m_pstVctParam;

	return CMyDialog::DoModal();
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgParam::OnInitDlg()
{
	CDialogEx::OnInitDialog();

	InitBtn();
	InitGrid();
	AttachButton();
	ResizeUI();

	return TRUE;
}


/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgParam::OnExitDlg()
{
}


/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgParam::OnPaintDlg(CDC* dc)
{
}


/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgParam::OnResizeDlg()
{
	CRect rCtrl;
	// ���� Client ����
	GetClientRect(rCtrl);
}


VOID CDlgParam::InitBtn()
{
	m_btnConfirm.SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
	m_btnConfirm.SetBgColor(g_clrBtnColor);
	m_btnConfirm.SetTextColor(g_clrBtnTextColor);
}


VOID CDlgParam::InitGrid()
{
	CRect						rGrid;
	std::vector <int>			vColSize(eGRD_COL_MAX);
	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_, ALICE_BLUE };

	int nTotalHeight = DEF_DEFAULT_GRID_ROW_SIZE * (int)m_stVctBackUp.size();
	int nTotalWidth = 0;
	int nWidthDiffer = 0;

	m_grdParam.GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	if (rGrid.Height() < nTotalHeight)
	{
		nWidthDiffer = ::GetSystemMetrics(SM_CXVSCROLL);
	}
	else
	{
		rGrid.bottom = rGrid.top + nTotalHeight + 1;
	}

	for (auto& width : vColSize)
	{
		width = (rGrid.Width() - nWidthDiffer) / (int)vColSize.size() - 20;
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - nWidthDiffer - 1;

	/* ��ü �⺻ ���� */
	m_grdParam.SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	m_grdParam.SetRowCount((int)m_stVctBackUp.size());
	m_grdParam.SetColumnCount((int)vColSize.size());
	m_grdParam.SetFixedColumnCount(0);
	m_grdParam.SetBkColor(WHITE_);
	m_grdParam.SetFixedColumnSelection(0);

	/* Ÿ��Ʋ (ù ��° ��) */
	for (int nRow = 0; nRow < (int)m_stVctBackUp.size(); nRow++)
	{
		m_grdParam.SetRowHeight(nRow, DEF_DEFAULT_GRID_ROW_SIZE);

		m_grdParam.SetItemText(nRow, eGRD_COL_NAME, m_stVctBackUp[nRow].strName);
		m_grdParam.SetItemText(nRow, eGRD_COL_VALUE, m_stVctBackUp[nRow].strValue);
		m_grdParam.SetItemText(nRow, eGRD_COL_UNIT, m_stVctBackUp[nRow].strUnit);

		for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
		{
			m_grdParam.SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			m_grdParam.SetColumnWidth(nCol, vColSize[nCol]);
			m_grdParam.SetItemBkColour(nRow, nCol, vColColor[nCol]);
		}
	}

	/* �⺻ �Ӽ� �� ���� */
	m_grdParam.EnableDragAndDrop(FALSE);
	m_grdParam.EnableSelection(FALSE);

	m_grdParam.MoveWindow(rGrid);
	m_grdParam.Refresh();
}


VOID CDlgParam::AttachButton()
{
	CGridCtrl* pGrid = &m_grdParam;
	CMacButton* pBtn = &m_btnConfirm;
	CRect rGrid, rBtn;
	int nHeight = 0;

	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	pBtn->GetWindowRect(rBtn);
	this->ScreenToClient(rBtn);

	nHeight = rBtn.Height();

	rBtn.top = rGrid.bottom + 2;
	rBtn.bottom = rBtn.top + nHeight;

	pBtn->MoveWindow(rBtn);
	Invalidate();
}

VOID CDlgParam::ResizeUI()
{
	CRect rWnd, rBtn;
	CMacButton* pBtn = &m_btnConfirm;

	this->GetWindowRect(rWnd);
	pBtn->GetWindowRect(rBtn);

	rWnd.bottom = rBtn.bottom + 8;
	

	this->MoveWindow(rWnd);
}


void CDlgParam::OnBnClickedParamBtnConfirm()
{
	if (0 < m_pstVctParam->size())
	{
		m_pstVctParam->clear();
		m_pstVctParam->shrink_to_fit();
	}

	*m_pstVctParam = m_stVctBackUp;
	CMyDialog::OnOK();
}


void CDlgParam::OnClickGridParam(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	m_grdParam.SetFocusCell(-1, -1);

	if (eGRD_COL_VALUE != pItem->iColumn)
	{
		return;
	}

	if (pItem->iRow >= m_stVctBackUp.size())
	{
		return;
	}

	if (ENM_DITM::en_string > m_stVctBackUp[pItem->iRow].enFormat)
	{
		if (PopupKBDN(m_stVctBackUp[pItem->iRow].enFormat,
			m_stVctBackUp[pItem->iRow].strValue,
			m_stVctBackUp[pItem->iRow].dMin,
			m_stVctBackUp[pItem->iRow].dMax,
			m_stVctBackUp[pItem->iRow].u8DecPts))
		{
			m_grdParam.SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), m_stVctBackUp[pItem->iRow].strValue);
		}
	}
	else
	{
		CDlgKBDT dlg;
		TCHAR *pstrText = NULL;

		if (IDOK == dlg.MyDoModal(DEF_STR_MAX_LENGTH))
		{
			pstrText = dlg.GetText();
			m_stVctBackUp[pItem->iRow].strValue = W2CT(pstrText);
			m_grdParam.SetItemText(pItem->iRow, pItem->iColumn, m_stVctBackUp[pItem->iRow].strValue);
		}
	}

	m_grdParam.SetScrollCell(pItem->iRow, 0, 0, 0);
}


BOOL CDlgParam::PopupKBDN(ENM_DITM enType, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts/* = 0*/)
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