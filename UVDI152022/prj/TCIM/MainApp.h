
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"

class CMainApp : public CWinApp
{
public:

	CMainApp();

// �����Լ� ������
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual INT32		ExitInstance();
	virtual BOOL		InitInstance();

// ���� ����
protected:


// ���� �Լ�
protected:

	BOOL				GetWorkPath();


// ���� �Լ�
public:



// �޽��� ��
protected:
	DECLARE_MESSAGE_MAP()
};

extern CMainApp theApp;