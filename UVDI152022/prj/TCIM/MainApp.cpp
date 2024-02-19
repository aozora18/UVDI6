

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/* Message Map */
BEGIN_MESSAGE_MAP(CMainApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

/* ������ ��ü */
CMainApp theApp;

/*
 desc : ������
 parm : None
 retn : None
*/
CMainApp::CMainApp()
{
	/* �ٽ� ���� ������ ���� */
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
 desc : �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// OLE �ʱ�ȭ
	AfxOleInit();
	/* Init to Rich Edit Control */
	AfxInitRichEdit();

	AfxEnableControlContainer();
	CShellManager *pShellManager = new CShellManager;
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));

	/* ���� �۾� ���� ��� */
	if (!GetWorkPath())
	{
		AfxMessageBox(L"Failed to get the current working folder", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	CDlgMain dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "���: ��ȭ ���ڸ� ������ �������Ƿ� ���� ���α׷��� ����ġ �ʰ� ����˴ϴ�.\n");
		TRACE(traceAppMsg, 0, "���: ��ȭ ���ڿ��� MFC ��Ʈ���� ����ϴ� ��� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS�� ������ �� �����ϴ�.\n");
	}

	// ������ ���� �� �����ڸ� �����մϴ�.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	return FALSE;
}

/*
 desc : �޸𸮿��� ���� �Ǿ��� �� ȣ��Ǵ� �Լ�
 parm : None
 retn : None
*/
INT32 CMainApp::ExitInstance()
{
#ifdef _DEBUG
	/* �޸� leak ���� ��� */
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
//	_CrtDumpMemoryLeaks();
#endif
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

