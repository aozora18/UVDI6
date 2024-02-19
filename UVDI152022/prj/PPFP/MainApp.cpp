
/*
 desc : Main App.
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


CMainApp theApp;

// �޽��� ��
BEGIN_MESSAGE_MAP(CMainApp, CWinApp)
END_MESSAGE_MAP()



/*
 desc : ������
 parm : None
 retn : None
*/
CMainApp::CMainApp()
{
	m_hMutexApp	= NULL;
	// �ٽ� ���� ������ ����
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CMainApp::PreTranslateMessage(MSG* msg)
{

	return CWinApp::PreTranslateMessage(msg);
}


/*
 desc : ó�� �ε� �Ǿ��� �� ȣ��Ǵ� �Լ�
 parm : None
 retn : TRUE - �ʱ�ȭ ���� / FALSE - �ʱ�ȭ ����
*/
BOOL CMainApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize= sizeof(InitCtrls);
	InitCtrls.dwICC	= ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// OLE �ʱ�ȭ
	AfxOleInit();

	AfxEnableControlContainer();

	CShellManager *pShellManager = new CShellManager;

	SetRegistryKey(_T("Philoptics - SiLuria Services"));

	/* ��Ʈ ���� ���� ��� */
	if (!uvCmn_GetLogFont(g_lf, L"D2Coding", 15, DEFAULT_CHARSET))
	{
		AfxMessageBox(L"Can't read font information", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* ���� �۾� ���� ��� */
	if (!GetWorkPath())
	{
		AfxMessageBox(L"Failed to get the current working folder", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	CDlgMain dlg;
	m_pMainWnd	= &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	return FALSE;
}

/*
 desc : �޸𸮿��� ���� �Ǿ��� �� ȣ��Ǵ� �Լ�
 parm : None
 retn : None
*/
INT32 CMainApp::ExitInstance()
{

	return CWinApp::ExitInstance();
}

/*
 desc : ���� ���μ����� �۾� ��θ� ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainApp::GetWorkPath()
{
	TCHAR *tzBack, *tzFront;
	HINSTANCE hInst;

	// �ڽ��� ȣ���� ���� ������ ��� �ڵ� ���
	hInst	= GetModuleHandle(NULL);
	if (!hInst)	return FALSE;
	// ���� ���� ���� ��ġ ��� (���ϸ� ����)
	if (GetModuleFileName(hInst, g_tzWorkDir, MAX_PATH_LEN) < 1)	return FALSE;

	// find the last forward or backward whack ...
	tzBack	= _tcsrchr (g_tzWorkDir, '\\');
	tzFront	= _tcsrchr (g_tzWorkDir, '/');

	if (tzBack != NULL || tzFront != NULL)
	{
		if (tzFront == NULL)	tzFront	= g_tzWorkDir;
		if (tzBack == NULL)		tzBack	= g_tzWorkDir;

		// from the start to the last whack is the root
		if (tzFront >= tzBack)	*tzFront	= '\0';
		else					*tzBack		= '\0';
	}

	return TRUE;
}
