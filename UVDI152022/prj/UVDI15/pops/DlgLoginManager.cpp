
/*
 desc : �뱤 �ݺ� Ƚ�� ���� ��ȭ ����
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgLoginManager.h"

#include "../mesg/DlgMesg.h"
#include "../db/DBMgr.h"
#include "../db/DBDefine.h"

#if USE_KEYBOARD_TYPE_TOUCH
#include "../../../inc/kybd/DlgKBDT.h"
#include "../../../inc/kybd/DlgKBDN.h"
#endif




#define DEF_DEFAULT_GRID_ROW_SIZE		40

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

//IMPLEMENT_DYNAMIC(CDlgIdManager, CDialogEx)

/*
 desc : ������
 parm : id		- [in]  �ڽ��� ���� ID
		parent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
 retn : None
*/
CDlgLoginManager::CDlgLoginManager(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgLoginManager::IDD, parent)
{
	m_bDrawBG		= TRUE;
	m_bTooltip		= TRUE;
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgLoginManager::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Title - Normal */
	u32StartID	= IDC_LOGIN_MANAGER_TTL_MANAGER;
	for (i=0; i< eMNG_TTL_MAX; i++)		DDX_Control(dx, u32StartID+i, m_ttl_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_LOGIN_MANAGER_STT_ID;
	for (i=0; i< eMNG_STT_MAX; i++)		DDX_Control(dx, u32StartID+i, m_stt_ctl[i]);
	/* Text - Normal */
	u32StartID	= IDC_LOGIN_MANAGER_TXT_ID;
	for (i=0; i< eMNG_TXT_MAX; i++)		DDX_Control(dx, u32StartID+i, m_txt_ctl[i]);
	/* Grid - Normal */
 	u32StartID = IDC_LOGIN_MANAGER_GRD_LIST;
 	for (i=0; i< eMNG_GRD_MAX; i++)		DDX_Control(dx, u32StartID+i, m_grd_ctl[i]);
	/* Button - Normal */
	u32StartID = IDC_LOGIN_MANAGER_BTN_OPERATOR;
	for (i=0; i< eMNG_BTN_MAX; i++)		DDX_Control(dx, u32StartID+i, m_btn_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgLoginManager, CMyDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_LOGIN_MANAGER_BTN_OPERATOR, IDC_LOGIN_MANAGER_BTN_OPERATOR + eMNG_BTN_MAX, OnBtnClicked)
	ON_CONTROL_RANGE(STN_CLICKED, IDC_LOGIN_MANAGER_TXT_ID, IDC_LOGIN_MANAGER_TXT_ID + eMNG_TXT_MAX, OnTxtClicked)
	ON_NOTIFY(NM_CLICK, IDC_LOGIN_MANAGER_GRD_LIST, &CDlgLoginManager::OnClickGrid)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä��
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgLoginManager::PreTranslateMessage(MSG* msg)
{

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLoginManager::OnInitDlg()
{
	CResizeUI clsResizeUI;
	clsResizeUI.ResizeControl(this, this);

	/* ��Ʈ�� �ʱ�ȭ */
	if (!InitCtrl())	return FALSE;
	if (!InitGrid())	return FALSE;

	/* TOP_MOST & Center */
	CenterParentTopMost();

	/* �ʱ� ���� ���� */
	SelectBtnLevel(eLOGIN_LEVEL_OPERATOR);

	/* LIST ���� */
	UpdateGrid();

	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgLoginManager::OnExitDlg()
{
	for (int i = 0; i < _countof(m_grd_ctl); i++)
	{
		m_grd_ctl[i].DeleteAllItems();
	}
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgLoginManager::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgLoginManager::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLoginManager::InitCtrl()
{
	CResizeUI clsResizeUI;

	INT32 i = 0;
	PTCHAR pText	= NULL;

	/* title - normal */
	for (i = 0; i < eMNG_TTL_MAX; i++)
	{
		m_ttl_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
//		m_ttl_ctl[i].SetDrawBg(1);
		m_ttl_ctl[i].SetBaseProp(0, 1, 0, 0, FORD_GRAPHITE, 0, WHITE_);
		clsResizeUI.ResizeControl(this, &m_ttl_ctl[i]);
	}

	/* static - normal */
	for (i = 0; i < eMNG_STT_MAX; i++)
	{
		m_stt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_stt_ctl[i].SetDrawBg(1);
		m_stt_ctl[i].SetBaseProp(0, 1, 0, 0, DEF_COLOR_BTN_MENU_NORMAL, 0, WHITE_);
		clsResizeUI.ResizeControl(this, &m_stt_ctl[i]);
	}

	/* static - text */
	for (i = 0; i < eMNG_TXT_MAX; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetNotify();		//	set	click event
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, WHITE_, 0, BLACK_);

		clsResizeUI.ResizeControl(this, &m_txt_ctl[i]);
	}

	/* grid - normal */
	for (i = 0; i< eMNG_GRD_MAX; i++)
	{
		m_grd_ctl[i].SetParent(this);
	}

	/* button - normal */
	for (i = 0; i< eMNG_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
	}

	return TRUE;
}

/*
 desc : GRID ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLoginManager::InitGrid()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	
	CGridCtrl* pGrid = &m_grd_ctl[eMNG_GRD_LIST];

	clsResizeUI.ResizeControl(this, pGrid);

	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	/* Grid Size �籸�� */
	rGrid.bottom = rGrid.top + rGrid.Height();
	pGrid->MoveWindow(rGrid);

	/* �⺻ �Ӽ� �� ���� */
	pGrid->SetColumnResize(FALSE);
	pGrid->SetRowResize(FALSE);
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();

	return TRUE;
}

BOOL CDlgLoginManager::UpdateGrid()
{
	std::vector < ST_DB_LOGIN > vLoginInfo = CDBMgr::GetInstance()->GetLoginInfo();

	std::vector <int>			vColSize(4);
	std::vector <int>			vRowSize(vLoginInfo.size() + 1);		//	+ 1 : Ÿ��Ʋ
	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_ , WHITE_ , WHITE_ };
	TCHAR tzCols[4][16] = { L"NO", L"ID", L"Password", L"Level" };
	CRect		rGrid;
	CGridCtrl* pGrid = &m_grd_ctl[eMNG_GRD_LIST];
	pGrid->DeleteAllItems();			//	Count, Height, Width, Color ��� ����.

	pGrid->GetWindowRect(rGrid);

	int nTotalHeight = 0;
	for (auto& height : vRowSize)
	{
		height = rGrid.Height() / (int)vRowSize.size();
		nTotalHeight += height;
	}

	int nTotalWidth = 0;
	for (auto& width : vColSize)
	{
		width = rGrid.Width() / (int)vColSize.size();
		nTotalWidth += width;
	}

	vColSize.front() += rGrid.Width() - nTotalWidth - 2;

	/* ��ü �⺻ ���� */
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->SetRowCount((int)vRowSize.size());
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetBkColor(RGB(255, 255, 255));
	pGrid->SetFixedColumnSelection(0);

	for (int nRow = 0; nRow < (int)vRowSize.size(); nRow++)
	{
		pGrid->SetRowHeight(nRow, 30);

		for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
		{
			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pGrid->SetColumnWidth(nCol, vColSize[nCol]);
			pGrid->SetItemBkColour(nRow, nCol, vColColor[nCol]);

			/* Ÿ��Ʋ (ù ��° ��) */
			if (0 == nRow)
			{
				pGrid->SetItemText(nRow, nCol, tzCols[nCol]);
				pGrid->SetItemBkColour(nRow, nCol, DEF_COLOR_BTN_MENU_NORMAL);
				pGrid->SetItemFgColour(nRow, nCol, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
			}
		}
	}

	/* Grid ���� �Է� */
	CString strNo;
	int		nRow = 0;
	for (auto info : vLoginInfo)
	{
		ConvertToAsterisks(info.strPwd);

		strNo.Format(_T("%d"), ++nRow);
		pGrid->SetItemText(nRow, 0, strNo);
		pGrid->SetItemText(nRow, 1, info.strID);
		pGrid->SetItemText(nRow, 2, info.strPwd);
		pGrid->SetItemText(nRow, 3, info.strLevel);
	}


	return TRUE;
}

void CDlgLoginManager::SelectBtnLevel(EN_LOGIN_LEVEL eLevel)
{
	m_strLevel  = sstrLoginLevel[eLevel];

	//	Init
	m_btn_ctl[eMNG_BTN_OPERATOR].SetBgColor(g_clrBtnColor);
	m_btn_ctl[eMNG_BTN_OPERATOR].Invalidate(TRUE);
	m_btn_ctl[eMNG_BTN_ENGINEER].SetBgColor(g_clrBtnColor);
	m_btn_ctl[eMNG_BTN_ENGINEER].Invalidate(TRUE);
	m_btn_ctl[eMNG_BTN_ADMIN].SetBgColor(g_clrBtnColor);
	m_btn_ctl[eMNG_BTN_ADMIN].Invalidate(TRUE);

	//	Set
	switch (eLevel)
	{
	case eLOGIN_LEVEL_OPERATOR:		m_btn_ctl[eMNG_BTN_OPERATOR].SetBgColor(g_clrBtnSelColor);		break;
	case eLOGIN_LEVEL_ENGINEER:		m_btn_ctl[eMNG_BTN_ENGINEER].SetBgColor(g_clrBtnSelColor);		break;
	case eLOGIN_LEVEL_ADMIN:		m_btn_ctl[eMNG_BTN_ADMIN].SetBgColor(g_clrBtnSelColor);			break;
	}
}

void CDlgLoginManager::ConvertToAsterisks(TCHAR* str)
{
	//	�Էµ� ���ڸ� ��� ��ǥ(*)�� ��ȯ
	if (str == NULL)
		return;

	while (*str) {
		*str = _T('*');
		str++;
	}
}

void CDlgLoginManager::ConvertToAsterisks(CString& str)
{
	str = CString(_T('*'), str.GetLength());
}

BOOL CDlgLoginManager::IsInputID()
{
	CDlgMesg dlgMesg;
	CString str;

	m_txt_ctl[eMNG_TXT_ID].GetWindowTextW(str);
	if (_T("") == str)
	{
		dlgMesg.MyDoModal(L"���Է�! \n[ ID ] �� �Է����ּ���.", 0x01);
		return FALSE;
	}

	return TRUE;
}

BOOL CDlgLoginManager::IsInputPassword()
{
	CDlgMesg dlgMesg;
	CString str;

	m_txt_ctl[eMNG_TXT_PASSWORD].GetWindowTextW(str);
	if (_T("") == str)
	{
		dlgMesg.MyDoModal(L"���Է�! \n[ PASSWORD ] �� �Է����ּ���.", 0x01);
		return FALSE;
	}

	return TRUE;
}

BOOL CDlgLoginManager::IsInputPasswordConfirm()
{
	CDlgMesg dlgMesg;
	CString str;

	m_txt_ctl[eMNG_TXT_PASSWORD_CONFIRM].GetWindowTextW(str);
	if (_T("") == str)
	{
		dlgMesg.MyDoModal(L"���Է�! \n[ PASSWORD CONFIRM ] �� �Է����ּ���.", 0x01);
		return FALSE;
	}

	return TRUE;

}

BOOL CDlgLoginManager::IsMatchPassword(CString str)
{
	CDlgMesg dlgMesg;

	if (m_strPwd != str)
	{
		m_txt_ctl[eMNG_TXT_PASSWORD_CONFIRM].SetWindowTextW(_T(""));
		dlgMesg.MyDoModal(L"�Է� ����! \n�Էµ� [ PASSWORD ] �� [ PASSWORD CONFIRM ] �� ��ġ���� �ʽ��ϴ�. �ٽ� �Է����ּ���.", 0x01);
		return FALSE;
	}

	return TRUE;
}

BOOL CDlgLoginManager::IsOverlapID(CString str)
{
	CDlgMesg dlgMesg;
	std::vector < ST_DB_LOGIN > vLoginInfo = CDBMgr::GetInstance()->GetLoginInfo();

	for (auto login : vLoginInfo)
	{
		if (login.strID == str)
		{
			m_txt_ctl[eMNG_TXT_ID].SetWindowTextW(_T(""));
			dlgMesg.MyDoModal(L"�Է� ����! \n�̹� ��ϵ� [ ID ] �Դϴ�. �ٽ� �Է����ּ���.", 0x01);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CDlgLoginManager::IsSelectID()
{
	CDlgMesg dlgMesg;

	//CCellID cell = m_grd_ctl[eMNG_GRD_LIST].GetFocusCell();

	if (m_nSelRow/*cell.row*/ < 1)
	{
		dlgMesg.MyDoModal(L"���� ����! \n�����Ϸ��� [ ID ] �� LIST ���� �������ּ���.", 0x01);
		return FALSE;
	}

	m_strSel = m_grd_ctl[eMNG_GRD_LIST].GetItemText(m_nSelRow/*cell.row*/, 1);

	return TRUE;
}

BOOL CDlgLoginManager::IsDeleteID()
{
	CDlgMesg dlgMesg;
	TCHAR tzMesg[256] = { NULL };

	swprintf_s(tzMesg, 256, L"���õ� ID ( %s ) �� ���� �����Ͻðڽ��ϱ�?", (LPCTSTR)m_strSel);

	if (dlgMesg.MyDoModal(tzMesg, 0x02) != IDOK)
	{
		return FALSE;
	}

	return TRUE;
}

void CDlgLoginManager::CleanText()
{
	m_txt_ctl[eMNG_TXT_ID].SetWindowTextW(_T(""));
	m_txt_ctl[eMNG_TXT_PASSWORD].SetWindowTextW(_T(""));
	m_txt_ctl[eMNG_TXT_PASSWORD_CONFIRM].SetWindowTextW(_T(""));
}

void CDlgLoginManager::AddDBLoginInfo()
{
	ST_DB_LOGIN stInfo;

	stInfo.strID = m_strID;
	stInfo.strPwd = m_strPwd;
	stInfo.strLevel = m_strLevel;

	CDBMgr::GetInstance()->AddLoginInfo(stInfo);
}

void CDlgLoginManager::DelDBLoginInfo()
{
	ST_DB_LOGIN stInfo;

	stInfo.strID = m_strSel;

	CDBMgr::GetInstance()->DelLoginInfo(stInfo);
}

/*
 desc : ���� ��ư �̺�Ʈ
 parm : None
 retn : None
*/
VOID CDlgLoginManager::OnBtnClicked(UINT32 id)
{
	CDlgMesg dlgMesg;


	switch (id)
	{
		/* ��ư ���� �� �ش� Level ��ȯ */
	case IDC_LOGIN_MANAGER_BTN_OPERATOR:
		SelectBtnLevel(eLOGIN_LEVEL_OPERATOR);
		break;
	case IDC_LOGIN_MANAGER_BTN_ENGINEER:
		SelectBtnLevel(eLOGIN_LEVEL_ENGINEER);
		break;
	case IDC_LOGIN_MANAGER_BTN_ADMIN:
		SelectBtnLevel(eLOGIN_LEVEL_ADMIN);
		break;

	case IDC_LOGIN_MANAGER_BTN_DEL:
		if (!IsSelectID())	return;
		if (!IsDeleteID())	return;

		DelDBLoginInfo();
		UpdateGrid();
		CleanText();
		break;
	case IDC_LOGIN_MANAGER_BTN_ADD:
		if (!IsInputID()) return;
		if (!IsInputPassword()) return;
		if (!IsInputPasswordConfirm()) return;

		AddDBLoginInfo();
		UpdateGrid();
		CleanText();
		break;
	case IDC_LOGIN_MANAGER_BTN_CLOSE:
		CMyDialog::EndDialog(IDOK);
		break;
	}
}

/*
 desc : ���� �ؽ�Ʈ �̺�Ʈ
 parm : None
 retn : None
*/
VOID CDlgLoginManager::OnTxtClicked(UINT32 id)
{
	CDlgKBDT dlg;
	CString	str;
	TCHAR tzText[1024] = { NULL };

	switch (id)
	{
	case IDC_LOGIN_MANAGER_TXT_ID:
		if (IDOK == dlg.MyDoModal(32))
		{
			dlg.GetText(tzText, 1024);
			m_strID = tzText;

			//	�ߺ� Ȯ��
			if (!IsOverlapID(m_strID))	return;
			
			m_txt_ctl[eMNG_TXT_ID].SetWindowTextW(tzText);
		}
		break;

	case IDC_LOGIN_MANAGER_TXT_PASSWORD:
		//	ID �Է� Ȯ��
		if (!IsInputID()) return;

		//	PASSWORD �Է�
		if (IDOK == dlg.MyDoModal(32, TRUE))
		{
			dlg.GetText(tzText, 1024);
			m_strPwd = tzText;

			ConvertToAsterisks(tzText);
			m_txt_ctl[eMNG_TXT_PASSWORD].SetWindowTextW(tzText);
			m_txt_ctl[eMNG_TXT_PASSWORD_CONFIRM].SetWindowTextW(_T(""));
		}
		break;

	case IDC_LOGIN_MANAGER_TXT_PASSWORD_CONFIRM:
		//	ID & PASSWORD �Է� Ȯ��
		if (!IsInputID()) return;
		if (!IsInputPassword()) return;

		//	PASSWORD CONFIRM �Է�
		if (IDOK == dlg.MyDoModal(32, TRUE))
		{
			dlg.GetText(tzText, 1024);
			str = tzText;

			//	PASSWORD ��ġ Ȯ��
			if (!IsMatchPassword(str)) return;

			ConvertToAsterisks(tzText);
			m_txt_ctl[eMNG_TXT_PASSWORD_CONFIRM].SetWindowTextW(tzText);
		}
		break;
	}
}

void CDlgLoginManager::OnClickGrid(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CGridCtrl* pGrid = &m_grd_ctl[eMNG_GRD_LIST];
	COLORREF colTitle = ALICE_BLUE;
	COLORREF colItem = WHITE_;

	m_nSelRow = pItem->iRow;

	/* Reset Focus */
	pGrid->SetFocusCell(-1, -1);

	for (int nRow = 1; nRow < pGrid->GetRowCount(); nRow++)
	{
		colTitle = (nRow == pItem->iRow) ? PALE_GREEN : ALICE_BLUE;
		colItem  = (nRow == pItem->iRow) ? PALE_GREEN : WHITE_;

		for (int nCol = 0; nCol < pGrid->GetColumnCount(); nCol++)
		{
			if (0 == nCol)	pGrid->SetItemBkColour(nRow, nCol, colTitle);
			else			pGrid->SetItemBkColour(nRow, nCol, colItem);			
		}
	}

	pGrid->Refresh();
}

/*
 desc : DoModal Override �Լ�
 parm : None
 retn : 0L
*/
INT_PTR CDlgLoginManager::MyDoModal()
{
	return DoModal();
}
