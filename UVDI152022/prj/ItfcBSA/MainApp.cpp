
/*
���� : Main App
*/

#include "pch.h"
#include "framework.h"
#include "MainApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ������ ��ü
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
