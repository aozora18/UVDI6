
/*
 desc : Main App
*/

#include "pch.h"
#include "MainApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ������ ��ü
CMainApp theApp;

// �޽��� ��
BEGIN_MESSAGE_MAP(CMainApp, CWinApp)
END_MESSAGE_MAP()

// CMainAppApp ����
CMainApp::CMainApp()
{
}


/*
 desc : ���� ���α׷��� �� �ν��Ͻ��� ������ �� ȣ���
 parm : None
 retn : TRUE - ���� / FALSE - ����
*/
BOOL CMainApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

/*
 desc : ���� ���α׷��� �ν��Ͻ��� ����� �� ȣ���
 parm : None
 retn : ����� �� ��� �� ��ȯ
*/
INT32 CMainApp::ExitInstance()
{

	return CWinApp::ExitInstance();
}
