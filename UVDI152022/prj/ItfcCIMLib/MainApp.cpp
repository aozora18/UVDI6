/*
*/

#include "pch.h"
#include "MainApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/* ������ ��ü */
CMainApp theApp;

/* Message Map */
BEGIN_MESSAGE_MAP(CMainApp, CWinApp)
END_MESSAGE_MAP()

/*
 desc : ������
 parm : None
 retn : None
*/
CMainApp::CMainApp()
{
}

/*
 desc : ���� ���α׷��� �� �ν��Ͻ��� ������ �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainApp::InitInstance()
{
	CWinApp::InitInstance();

	/* COM Initialized */
#if 0
	HRESULT hRes	= CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
#else
	HRESULT hRes	= CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif
	if (hRes != S_OK)
	{
		AfxMessageBox(L"Failed to initialize the COM", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : ���� ���α׷��� �ν��Ͻ��� ����� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
INT32 CMainApp::ExitInstance()
{
	/* COM Uninitialize */
	CoUninitialize();

	return CWinApp::ExitInstance();
}
