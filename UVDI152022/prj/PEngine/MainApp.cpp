
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

// 메시지 맵
BEGIN_MESSAGE_MAP(CMainApp, CWinApp)
END_MESSAGE_MAP()



/*
 desc : 생성자
 parm : None
 retn : None
*/
CMainApp::CMainApp()
{
	m_hMutexApp	= NULL;
	// 다시 시작 관리자 지원
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
 desc : 처음 로드 되었을 때 호출되는 함수
 parm : None
 retn : TRUE - 초기화 성공 / FALSE - 초기화 실패
*/
BOOL CMainApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize= sizeof(InitCtrls);
	InitCtrls.dwICC	= ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// OLE 초기화
	AfxOleInit();

	// 소켓 초기화
	if (!AfxSocketInit())
	{
		AfxMessageBox(_T("Failed to initialize winsock"));
		return FALSE;
	}

	AfxEnableControlContainer();
	AfxInitRichEdit2();	/* RichEdit 초기화 */

	CShellManager *pShellManager = new CShellManager;

	SetRegistryKey(_T("Philoptics - PODIS Engine"));

	/* 폰트 관련 정보 얻기 */
	if (!uvCmn_GetLogFont(g_lf, L"D2Coding", 15, DEFAULT_CHARSET))
	{
		AfxMessageBox(L"Can't read font information", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}
	/* 현재 작업 폴더 얻기 */
	if (!GetWorkPath())
	{
		AfxMessageBox(L"Failed to get the current working folder", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 중복 실행 확인 */
	m_hMutexApp	= ::CreateMutex(FALSE, 0, L"PODIS.BRainX1.PEngine");
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		AfxMessageBox(L"The program is already running", MB_ICONSTOP|MB_TOPMOST);
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
 desc : 메모리에서 해제 되었을 때 호출되는 함수
 parm : None
 retn : None
*/
INT32 CMainApp::ExitInstance()
{
	/* 중복 실행 방지 핸들 해제 */
	if (m_hMutexApp)	::ReleaseMutex(m_hMutexApp);

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
