/*
*/

#include "pch.h"
#include "MainApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/* 유일한 개체 */
CMainApp theApp;

/* Message Map */
BEGIN_MESSAGE_MAP(CMainApp, CWinApp)
END_MESSAGE_MAP()

/*
 desc : 생성자
 parm : None
 retn : None
*/
CMainApp::CMainApp()
{
}

/*
 desc : 응용 프로그램의 새 인스턴스를 실행할 때 호출됨
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
 desc : 응용 프로그램의 인스턴스가 종료될 때 호출됨
 parm : None
 retn : TRUE or FALSE
*/
INT32 CMainApp::ExitInstance()
{
	/* COM Uninitialize */
	CoUninitialize();

	return CWinApp::ExitInstance();
}
