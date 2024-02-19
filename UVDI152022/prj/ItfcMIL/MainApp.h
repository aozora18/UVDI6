#include "MilMain.h"
#include "MilDisp.h"
#include "MilModel.h"
#include "MilImage.h"

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#include "resource.h"

class CMainApp : public CWinApp
{
public:
	CMainApp();
	virtual ~CMainApp();

	CMilMain clMilMain;
	CMilDisp clMilDisp;

/* 가상 함수 */
protected:
	virtual BOOL		InitInstance();
	virtual INT32		ExitInstance();

/* 로컬 변수 */
protected:


/* 로컬 함수 */
protected:

/* 공용 함수 */
public:

/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()

};


extern CMainApp theApp;
