
#pragma once

#include "../comn/MyDialog.h"
#include "../comn/MyGrpBox.h"
#include "../comn/MacButton.h"
#include "../comn/MyStatic.h"
#include "../comn/MyEdit.h"
#include "../comn/MyCombo.h"
#include "../comn/MyListBox.h"
#include "../comn/MacProgress.h"
#include "../comn/MyRich.h"
#include "../comn/MyTabCtrl.h"
#include "../grid/GridCtrl.h"


#define MAX_CTRL_COUNT	128

class CDlgChild : public CMyDialog
{
public:

	CDlgChild(UINT32 dlg_id, CWnd *parent=NULL, BOOL magnet=FALSE);
	virtual ~CDlgChild();


/* virtual function */
protected:

	virtual BOOL		OnInitDlg();

public:

	virtual VOID		UpdateCtrl() = 0;


/* Local Parameter */
protected:

	BOOL				m_bMagnet;	/* Whether the window magnet is set */



/* Local Function */
protected:


/* Public Function */
public:


/* Message Map */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnWindowPosChanging(WINDOWPOS *lpwndpos);
};
