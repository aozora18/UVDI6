/*
 desc : Main App
*/

#include "pch.h"
#include "framework.h"
#include "MainApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// 유일한 객체
CMainApp theApp;

// 메시지 맵
BEGIN_MESSAGE_MAP(CMainApp, CWinApp)
END_MESSAGE_MAP()

// CMainAppApp 생성
CMainApp::CMainApp()
{
}

/*
 desc : 응용 프로그램의 새 인스턴스를 시작할 때 호출됨
 parm : None
 retn : TRUE - 성공 / FALSE - 실패
*/
BOOL CMainApp::InitInstance()
{
	CWinApp::InitInstance();

	// 소켓 초기화
	if (!AfxSocketInit())
	{
		AfxMessageBox(_T("Failed to initialize winsock"), MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 현재 작업 폴더 얻기 */
	if (!GetWorkPath())
	{
		AfxMessageBox(L"Failed to get the current working folder", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	return TRUE;
}

/*
 desc : 응용 프로그램의 인스턴스가 종료될 때 호출됨
 parm : None
 retn : 종료될 때 결과 값 반환
*/
INT32 CMainApp::ExitInstance()
{

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
