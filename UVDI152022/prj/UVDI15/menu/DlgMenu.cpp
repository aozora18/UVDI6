
/*
 desc : ���� �޴��� ������ ���� �⺻ ��ü
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgMenu.h"
#include <afxtaskdialog.h>

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : ������
 parm : id		- [in]  �ڽ��� ���� ID
		parent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
 retn : None
*/
CDlgMenu::CDlgMenu(UINT32 id, CWnd* parent)
	: CMyDialog(id, parent)
{
	m_bDrawBG		= TRUE;
	m_bTooltip		= TRUE;
	m_bMagnetic		= FALSE;
	m_pDlgMain		= (CDlgMain*)parent;

	/* ��� ���� �ʱ�ȭ */
	m_enDlgID		= ENG_CMDI::en_menu_none;
}

CDlgMenu::~CDlgMenu()
{
}

BEGIN_MESSAGE_MAP(CDlgMenu, CMyDialog)
END_MESSAGE_MAP()

/*
 desc : ��޸��� ���� ����
 parm : h_view	- [in]  �ڽ��� ���� (���̾�α�)�� ��µǴ� ������ �ڵ�
 retn : DI_TRUE or DI_FALSE
*/
BOOL CDlgMenu::Create(HWND h_view)
{
	if (!CMyDialog::Create())	return FALSE;

	ShowWindow(SW_HIDE);

	INT32 i32Width, i32Height;
	CRect rView, rChild;
	CWnd *pView	= CWnd::FromHandle(h_view);

	/* �ڽ��� ���� ��ġ �� ũ�� ��� */
	GetClientRect(rChild);

	// by sysandj : ���� �ּ�ó��
	//i32Width	= rChild.Width();
	//i32Height	= rChild.Height();
	// by sysandj : ���� �ּ�ó��

	/* Output Box ��Ʈ���� ��ġ ��� */
	pView->GetWindowRect(rView);
	ScreenToClient(rView);

	// by sysandj : TAB ȭ�鿡 ����
	i32Width = rView.Width();
	i32Height = rView.Height();
	// by sysandj : TAB ȭ�鿡 ����

	/* Ư�� ��ġ�� �̵� */
	rChild.left		= rView.left /*+ 1*/;
	rChild.top		= rView.top /*+ 1*/;
	rChild.right	= rChild.left + i32Width;
	rChild.bottom	= rChild.top + i32Height;

 	MoveWindow(rChild);
	SetParent(m_pParentWnd);	/* �θ� ������ �������� ������ POPUP â���� �θ� ���� ��ġ�� */
	ShowWindow(SW_SHOW);

	return TRUE;
}

/*
 desc : ���� ���ŵ� ������, �ܰ��� �簢�� ���� �׸���
 parm : None
 retn : None
*/
VOID CDlgMenu::DrawOutLine() // 
{
	HDC hDC	= NULL;
	HPEN hPen, hOldPen = NULL;
	CRect rClient;

	/* �ڽ��� ���� ũ�� ��� */
	GetClientRect(rClient);

	/* Device Context ��� */
	hDC		= ::GetDC(m_hWnd);
	if (!hDC)	return;
	/* Pen ���� �� ���� */
	hPen	= ::CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
	hOldPen	= (HPEN)::SelectObject(hDC, hPen);
	/* �ڽ��� ������ �����ڸ��� ������ ���� �׸��� */
	::MoveToEx(hDC, 0, 0, NULL);
	::LineTo(hDC, rClient.right-1, 0);
	::LineTo(hDC, rClient.right-1, rClient.bottom-1);
	::LineTo(hDC, 0, rClient.bottom-1);
	::LineTo(hDC, 0, 0);

	/* Pen ���� */
	if (hOldPen)::SelectObject(hDC, hOldPen);
	if (hPen)	::DeleteObject(hPen);
	/* DC �ݱ� */
	if (hDC)	::ReleaseDC(m_hWnd, hDC);
}

BOOL CDlgMenu::PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts/* = 0*/)
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
		//swprintf_s(tzPoint, 512, L"%s:%%.%df,%s:%%.%df", tzMinMax[0], u8DecPts, tzMinMax[1], u8DecPts);
		swprintf_s(tzPoint, 512, L"%s:%%.%df,%s:%%.%df", tzMinMax[0], u8DecPts, tzMinMax[1], u8DecPts);
		swprintf_s(tzTitle, 1024, tzPoint, dMin, dMax);
		if (IDOK != dlg.MyDoModal(tzTitle, TRUE, TRUE, dMin, dMax))	return FALSE;	break;
	}
	//% .4f
	switch (enType)
	{
	case ENM_DITM::en_int8:
	case ENM_DITM::en_int16:
		strOutput.Format(_T("%d"), dlg.GetValueInt16());
		break;
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

int CDlgMenu::ShowMsg(EN_MSG_BOX_TYPE mType, LPCTSTR lpszMsg, ...)
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

int CDlgMenu::ShowYesNoMsg(EN_MSG_BOX_TYPE mType, LPCTSTR lpszMsg, ...)
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

int CDlgMenu::ShowMultiSelectMsg(EN_MSG_BOX_TYPE mType, CString strTitle, CStringArray& strArrCommand)
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