#pragma once

#include "../../inc/comn/ThinThread.h"

class CMainThread : public CThinThread
{
/* ������ & �ı��� */
public:
	CMainThread(HWND parent);
	virtual ~CMainThread();

// ���� �Լ� ������
protected:

	virtual BOOL		StartWork();
	virtual VOID		RunWork();
	virtual VOID		EndWork();

// ���� ����
protected:

// ���� �Լ�
protected:


// ���� �Լ�
public:
};

