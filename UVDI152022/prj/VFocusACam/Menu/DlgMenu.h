
#pragma once

#include "../DlgMain.h"
#include "../../../inc/base/DlgChild.h"

class CDlgMain;

class CDlgMenu : public CDlgChild
{
public:

	CDlgMenu(UINT32 id, CWnd *parent = NULL);
	virtual ~CDlgMenu();


/* �����Լ� */
protected:

	virtual BOOL		OnInitDlg();
	virtual VOID		OnExitDlg();

public:

	virtual VOID		ResetData() = 0;


/* ���� ���� */
protected:

	CDlgMain			*m_pDlgMain;


/* ���� �Լ� */
protected:



/* ���� �Լ� */
public:



/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
};
