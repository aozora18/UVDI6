
#pragma once

#include "../../../inc/base/DlgChild.h"


class CDlgLuria : public CDlgChild
{
public:

	CDlgLuria(UINT32 id, CWnd *parent=NULL);
	virtual ~CDlgLuria();


/* 가상함수 */
protected:

	virtual BOOL		OnInitDlg();
	virtual VOID		OnExitDlg();

public:



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
