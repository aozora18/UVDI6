
/*
 desc : Main App Object
*/

#include "pch.h"
#include "framework.h"
#include "MainApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 유일한 객체
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
}

/*
 desc : 응용 프로그램의 새 인스턴스를 시작할 때 호출됨
 parm : None
 retn : TRUE - 성공 / FALSE - 실패
*/
BOOL CMainApp::InitInstance()
{
	CWinApp::InitInstance();

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
