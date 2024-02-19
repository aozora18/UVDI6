
/*
 desc : �뱤 �ݺ� Ƚ�� ���� ��ȭ ����
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgLoginMain.h"

#include "../mesg/DlgMesg.h"
#include "../db/DBMgr.h"
#include "../db/DBDefine.h"

#if USE_KEYBOARD_TYPE_TOUCH
#include "../../../inc/kybd/DlgKBDT.h"
#include "../../../inc/kybd/DlgKBDN.h"
#endif

#include "../DlgMain.h"


#define DEF_DEFAULT_GRID_ROW_SIZE		40
#define DEF_DEFAULT_GRID_TITLE_TEXT		_T("ID�� ������ �ּ���.")

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

//IMPLEMENT_DYNAMIC(CDlgIdLogin, CDialogEx)

/*
 desc : ������
 parm : id		- [in]  �ڽ��� ���� ID
		parent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
 retn : None
*/
CDlgLoginMain::CDlgLoginMain(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgLoginMain::IDD, parent)
{
	m_bDrawBG		= TRUE;
	m_bTooltip		= TRUE;
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgLoginMain::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Title - Normal */
	u32StartID	= IDC_LOGIN_MAIN_TTL_LOGIN;
	for (i=0; i< eLOGIN_TTL_MAX; i++)		DDX_Control(dx, u32StartID+i, m_ttl_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_LOGIN_MAIN_STT_LEVEL;
	for (i=0; i< eLOGIN_STT_MAX; i++)		DDX_Control(dx, u32StartID+i, m_stt_ctl[i]);
	/* Text - Normal */
	u32StartID	= IDC_LOGIN_MAIN_TXT_PASSWORD;
	for (i=0; i< eLOGIN_TXT_MAX; i++)		DDX_Control(dx, u32StartID+i, m_txt_ctl[i]);
	/* Grid - Normal */
	u32StartID  = IDC_LOGIN_MAIN_GRD_ID;
	for (i=0; i < eLOGIN_GRD_MAX; i++)		DDX_Control(dx, u32StartID+i, m_grd_ctl[i]);
	/* Button - Normal */
	u32StartID	= IDC_LOGIN_MAIN_BTN_OPERATOR;
	for (i=0; i< eLOGIN_BTN_MAX; i++)		DDX_Control(dx, u32StartID+i, m_btn_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgLoginMain, CMyDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_LOGIN_MAIN_BTN_OPERATOR, IDC_LOGIN_MAIN_BTN_OPERATOR + eLOGIN_BTN_MAX, OnBtnClicked)
	ON_CONTROL_RANGE(STN_CLICKED, IDC_LOGIN_MAIN_TXT_PASSWORD, IDC_LOGIN_MAIN_TXT_PASSWORD + eLOGIN_TXT_MAX, OnTxtClicked)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä��
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgLoginMain::PreTranslateMessage(MSG* msg)
{

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLoginMain::OnInitDlg()
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

	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgLoginMain::OnExitDlg()
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
VOID CDlgLoginMain::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgLoginMain::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLoginMain::InitCtrl()
{
	CResizeUI clsResizeUI;

	INT32 i = 0;
	PTCHAR pText	= NULL;

	/* title - normal */
	for (i = 0; i < eLOGIN_TTL_MAX; i++)
	{
		m_ttl_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
//		m_ttl_ctl[i].SetDrawBg(1);
		m_ttl_ctl[i].SetBaseProp(0, 1, 0, 0, FORD_GRAPHITE, 0, WHITE_);
		clsResizeUI.ResizeControl(this, &m_ttl_ctl[i]);
	}

	/* static - normal */
	for (i = 0; i < eLOGIN_STT_MAX; i++)
	{
		m_stt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_stt_ctl[i].SetDrawBg(1);
		m_stt_ctl[i].SetBaseProp(0, 1, 0, 0, DEF_COLOR_BTN_MENU_NORMAL, 0, WHITE_);
		clsResizeUI.ResizeControl(this, &m_stt_ctl[i]);
	}

	/* static - text */
	for (i = 0; i < eLOGIN_TXT_MAX; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetNotify();		//	set	click event
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, WHITE_, 0, WHITE_);
		clsResizeUI.ResizeControl(this, &m_txt_ctl[i]);
	}

	/* grid - normal */
	for (i = 0; i< eLOGIN_GRD_MAX; i++)
	{
		m_grd_ctl[i].SetParent(this);
	}

	/* button - normal */
	for (i = 0; i< eLOGIN_BTN_MAX; i++)
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
BOOL CDlgLoginMain::InitGrid()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <int>			vColSize(1);
	std::vector <int>			vRowSize(1);
	std::vector <COLORREF>		vColColor = { WHITE_ };

	CGridCtrl* pGrid = &m_grd_ctl[eLOGIN_GRD_ID];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

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

	/* Ÿ��Ʋ (ù ��° ��) */
	int nRow = 0;
	int nCol = 0;
	
	pGrid->SetRowHeight(nRow, nTotalHeight - 1);
	pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pGrid->SetColumnWidth(nCol, vColSize[nCol]);
	pGrid->SetItemText(nRow, nCol, DEF_DEFAULT_GRID_TITLE_TEXT);
	pGrid->SetItemBkColour(nRow, nCol, vColColor[nCol]);

	
	UpdateGridItem();


	/* Grid Size �籸�� */
	rGrid.bottom = rGrid.top + nTotalHeight;
	pGrid->MoveWindow(rGrid);

	/* �⺻ �Ӽ� �� ���� */
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();

	return TRUE;
}

BOOL CDlgLoginMain::UpdateGridItem()
{
	CGridCtrl* pGrid = &m_grd_ctl[eLOGIN_GRD_ID];

	pGrid->SetCellType(0, 0, RUNTIME_CLASS(CGridCellCombo));
	CStringArray options;

	std::vector < ST_DB_LOGIN > vLoginInfo = CDBMgr::GetInstance()->GetLoginInfo();

	for (auto login : vLoginInfo)
	{
		if (login.strLevel == m_strLevel)
		{
			options.Add(login.strID);
		}
	}

	CGridCellCombo* pComboCell = (CGridCellCombo*)pGrid->GetCell(0, 0);
	pComboCell->SetOptions(options);
	pComboCell->SetStyle(CBS_DROPDOWN);

	return TRUE;
}

void CDlgLoginMain::SelectBtnLevel(EN_LOGIN_LEVEL eLevel)
{
	m_eLevel = eLevel;
	m_strLevel = sstrLoginLevel[eLevel];

	//	Init
	m_btn_ctl[eLOGIN_BTN_OPERATOR].SetBgColor(g_clrBtnColor);
	m_btn_ctl[eLOGIN_BTN_OPERATOR].Invalidate(TRUE);
	m_btn_ctl[eLOGIN_BTN_ENGINEER].SetBgColor(g_clrBtnColor);
	m_btn_ctl[eLOGIN_BTN_ENGINEER].Invalidate(TRUE);
	m_btn_ctl[eLOGIN_BTN_ADMIN].SetBgColor(g_clrBtnColor);
	m_btn_ctl[eLOGIN_BTN_ADMIN].Invalidate(TRUE);

	//	Set
	switch (eLevel)
	{
	case eLOGIN_LEVEL_OPERATOR:
		m_btn_ctl[eLOGIN_BTN_OPERATOR].SetBgColor(g_clrBtnSelColor);
		break;
	case eLOGIN_LEVEL_ENGINEER:
		m_btn_ctl[eLOGIN_BTN_ENGINEER].SetBgColor(g_clrBtnSelColor);
		break;
	case eLOGIN_LEVEL_ADMIN:
		m_btn_ctl[eLOGIN_BTN_ADMIN].SetBgColor(g_clrBtnSelColor);
		break;
	}

	//	Level�� ���� ID Combo ����
	UpdateGridItem();
}

void CDlgLoginMain::ConvertToAsterisks(TCHAR* str)
{
	//	�Էµ� ���ڸ� ��� ��ǥ(*)�� ��ȯ
	if (str == NULL)
		return;

	while (*str) {
		*str = _T('*');
		str++;
	}
}

BOOL CDlgLoginMain::IsInputID()
{
	CDlgMesg dlgMesg;
	CString str;

	str = m_grd_ctl[eLOGIN_GRD_ID].GetCell(0, 0)->GetText();
	if (DEF_DEFAULT_GRID_TITLE_TEXT == str)
	{
		dlgMesg.MyDoModal(L"�α��� ID�� �������ּ���.", 0x01);
		return FALSE;
	}

	return TRUE;
}

BOOL CDlgLoginMain::IsInputPassword()
{
	CDlgMesg dlgMesg;
	CString str;

	m_txt_ctl[eLOGIN_TXT_PASSWORD].GetWindowTextW(str);
	if (_T("") == str)
	{
		dlgMesg.MyDoModal(L"���Է�! \n [ PASSWORD ] �� �Է����ּ���.", 0x01);
		return FALSE;
	}

	return TRUE;

}

BOOL CDlgLoginMain::IsEnableLogin(CString strID, CString strPwd, CString strLevel)
{
	CDlgMesg dlgMesg;

	std::vector < ST_DB_LOGIN > vLoginInfo = CDBMgr::GetInstance()->GetLoginInfo();

	for (auto login : vLoginInfo)
	{
		if (login.strID == strID)
		{
			if (login.strLevel != strLevel)
			{
				dlgMesg.MyDoModal(L"������ [ ID ]�� [ LEVEL ]�� ��ġ���� �ʽ��ϴ�. �ٽ� �������ּ���.", 0x01);
				return FALSE;
			}
			else if (login.strPwd != strPwd)
			{
				dlgMesg.MyDoModal(L"�Է��� [ PASSWORD ]�� Ʋ�Ƚ��ϴ�. �ٽ� �Է����ּ���.", 0x01);
				return FALSE;
			}
			else
			{
				//	���� ����
				return TRUE;
			}
		}
	}

	dlgMesg.MyDoModal(L"��ϵ��� ���� [ ID ]�Դϴ�. ID ���� ȭ�鿡�� Ȯ�����ּ���.", 0x01);

	return FALSE;
}

BOOL CDlgLoginMain::IsNullLoginInfo()
{
	std::vector < ST_DB_LOGIN > vLoginInfo = CDBMgr::GetInstance()->GetLoginInfo();

	if (vLoginInfo.size() == 0)
	{
		return TRUE;
	}

	return FALSE;
}

/*
 desc : ���� ��ư �̺�Ʈ
 parm : None
 retn : None
*/
VOID CDlgLoginMain::OnBtnClicked(UINT32 id)
{
	CDlgMesg dlgMesg;

	CDlgMain* pMainDlg = (CDlgMain*)::AfxGetMainWnd();

	switch (id)
	{
		/* ��ư ���� �� �ش� Level ��ȯ */
	case IDC_LOGIN_MAIN_BTN_OPERATOR:
		SelectBtnLevel(eLOGIN_LEVEL_OPERATOR);
		break;
	case IDC_LOGIN_MAIN_BTN_ENGINEER:
		SelectBtnLevel(eLOGIN_LEVEL_ENGINEER);
		break;
	case IDC_LOGIN_MAIN_BTN_ADMIN:
		SelectBtnLevel(eLOGIN_LEVEL_ADMIN);
		break;

	case IDC_LOGIN_MAIN_BTN_MANAGER:
		if (IsNullLoginInfo())
		{
			/* ID ���� ȭ�� ȣ�� */
			::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), eMSG_MAIN_MANAGER_CONSOLE, NULL, NULL);
		}
		else
		{
			/* ID ���� ȭ�� ȣ�� */
			::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), eMSG_MAIN_ADMIN_CONSOLE, NULL, NULL);
		}
		CMyDialog::EndDialog(IDOK);
		break;
	case IDC_LOGIN_MAIN_BTN_CANCEL:
		CMyDialog::EndDialog(IDCANCEL);
		break;
	case IDC_LOGIN_MAIN_BTN_LOGIN:		
		if (!IsInputID()) return;
		if (!IsInputPassword()) return;		
		if (!IsEnableLogin(m_strID, m_strPwd, m_strLevel)) return;

		m_strID = _T("");
		m_strPwd = _T("");
		m_grd_ctl[eLOGIN_GRD_ID].SetItemText(0, 0, DEF_DEFAULT_GRID_TITLE_TEXT);
		m_txt_ctl[eLOGIN_TXT_PASSWORD].SetWindowTextW(_T(""));

		g_loginLevel = (UINT)m_eLevel;

		CMyDialog::EndDialog(IDOK);
		break;
	default:
		return;
		break;
	}
}

VOID CDlgLoginMain::OnTxtClicked(UINT32 id)
{
	CDlgKBDT dlg;
	CString	str;
	TCHAR tzText[1024] = { NULL };

	switch (id)
	{
	case IDC_LOGIN_MAIN_TXT_PASSWORD:
		//	ID �Է� Ȯ��
		//if (!IsInputID()) return;

		//	PASSWORD �Է�
		if (IDOK == dlg.MyDoModal(32, TRUE))
		{
			dlg.GetText(tzText, 1024);
			m_strPwd = tzText;

			ConvertToAsterisks(tzText);
			m_txt_ctl[eLOGIN_TXT_PASSWORD].SetWindowTextW(tzText);

			m_strID = m_grd_ctl[eLOGIN_GRD_ID].GetCell(0, 0)->GetText();
		}
		break;
	}
}

/*
 desc : DoModal Override �Լ�
 parm : None
 retn : 0L
*/
INT_PTR CDlgLoginMain::MyDoModal()
{
	return DoModal();
}
