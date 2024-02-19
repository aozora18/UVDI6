

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

/* 유일한 개체 */
CMainApp theApp;

/*
 desc : 생성자
 parm : None
 retn : None
*/
CMainApp::CMainApp()
{
	/* 다시 시작 관리자 지원 */
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

}

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CMainApp::PreTranslateMessage(MSG* msg)
{

	return CWinApp::PreTranslateMessage(msg);
}

/*
 desc : 초기화
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

	// OLE 초기화
	AfxOleInit();
	/* Init to Rich Edit Control */
	AfxInitRichEdit();

	AfxEnableControlContainer();
	CShellManager *pShellManager = new CShellManager;
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	/* 현재 작업 폴더 얻기 */
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
		TRACE(traceAppMsg, 0, "경고: 대화 상자를 만들지 못했으므로 응용 프로그램이 예기치 않게 종료됩니다.\n");
		TRACE(traceAppMsg, 0, "경고: 대화 상자에서 MFC 컨트롤을 사용하는 경우 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS를 수행할 수 없습니다.\n");
	}

	// 위에서 만든 셸 관리자를 삭제합니다.
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
 desc : 메모리에서 해제 되었을 때 호출되는 함수
 parm : None
 retn : None
*/
INT32 CMainApp::ExitInstance()
{
#ifdef _DEBUG
	/* 메모리 leak 지점 출력 */
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
//	_CrtDumpMemoryLeaks();
#endif
	return CWinApp::ExitInstance();
}

/*
 desc : 현재 프로세스의 작업 경로를 얻기
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMainApp::GetWorkPath()
{
	TCHAR *tzBack, *tzFront;
	HINSTANCE hInst;

	// 자신을 호출한 실행 파일의 모듈 핸들 얻기
	hInst	= GetModuleHandle(NULL);
	if (!hInst)	return FALSE;
	// 현재 실행 파일 위치 얻기 (파일명 포함)
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

