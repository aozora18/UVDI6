
#pragma once

#include "../DlgMain.h"
#include "../../../inc/base/DlgChild.h"

class CDlgMain;

class CDlgMenu : public CDlgChild
{
public:

	CDlgMenu(UINT32 id, CWnd *parent = NULL);
	virtual ~CDlgMenu();


/* 가상함수 */
protected:

	virtual BOOL		OnInitDlg();
	virtual VOID		OnExitDlg();

public:

	virtual VOID		ResetData() = 0;


/* 로컬 변수 */
protected:

	CDlgMain			*m_pDlgMain;


/* 로컬 함수 */
protected:



/* 공용 함수 */
public:



/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
};
