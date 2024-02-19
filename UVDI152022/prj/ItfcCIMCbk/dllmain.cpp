// dllmain.cpp : DllMain의 구현입니다.

#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "ItfcCIMCbk_i.h"
#include "dllmain.h"

CItfcCIMCbkModule _AtlModule;

class CItfcCIMCbkApp : public CWinApp
{
public:

// 재정의입니다.
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CItfcCIMCbkApp, CWinApp)
END_MESSAGE_MAP()

CItfcCIMCbkApp theApp;

BOOL CItfcCIMCbkApp::InitInstance()
{
	return CWinApp::InitInstance();
}

int CItfcCIMCbkApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
