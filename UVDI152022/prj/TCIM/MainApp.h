
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"

class CMainApp : public CWinApp
{
public:

	CMainApp();

// 가상함수 재정의
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual INT32		ExitInstance();
	virtual BOOL		InitInstance();

// 로컬 변수
protected:


// 로컬 함수
protected:

	BOOL				GetWorkPath();


// 공용 함수
public:



// 메시지 맵
protected:
	DECLARE_MESSAGE_MAP()
};

extern CMainApp theApp;