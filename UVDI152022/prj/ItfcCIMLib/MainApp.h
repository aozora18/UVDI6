
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


class CMainApp : public CWinApp
{
public:
	CMainApp();

/* ���� �Լ� ������ */
public:
	virtual BOOL		InitInstance();
	virtual INT32		ExitInstance();

/* ���� ���� */
protected:

/* ���� �Լ� */
protected:

/* ���� �Լ� */
public:


/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
};
