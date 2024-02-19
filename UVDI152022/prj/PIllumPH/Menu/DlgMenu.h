
#pragma once

#include "../DlgMain.h"
#include "../../../inc/base/DlgChild.h"

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/comn/MacButton.h"
#include "../../../inc/comn/MyGrpBox.h"
#include "../../../inc/comn/MyStatic.h"
#include "../../../inc/comn/MyEdit.h"
#include "../../../inc/comn/MyCombo.h"
#include "../../../inc/grid/GridCtrl.h"


class CDlgMain;

class CDlgMenu : public CDlgChild
{
public:

	CDlgMenu(UINT32 id, LPG_MICL recipe, UINT8 menu_id, CWnd *parent = NULL);
	virtual ~CDlgMenu();


/* Virtual function */
protected:

	virtual VOID		ShowHide(BOOL show) = 0;

	virtual BOOL		OnInitDlg();
	virtual VOID		OnExitDlg();
	virtual VOID		CreateWork() = 0;
	virtual VOID		ReleaseWork() = 0;
	virtual VOID		RebuildGrid() = 0;
	virtual VOID		UpdateWork(UINT8 mode) = 0;

public:

	virtual VOID		DoWork() = 0;

	virtual BOOL		GetData() = 0;
	virtual VOID		SetData() = 0;
	virtual VOID		SaveGrid()= 0;

	virtual BOOL		StartWork(UINT8 run_mode, DOUBLE max_min);
	virtual VOID		StopWork() = 0;
	virtual BOOL		IsWorkBusy() = 0;
	virtual UINT8		GetPhNo() = 0;
	virtual PTCHAR		GetStepName() = 0;


/* Structure */
protected:

#pragma pack (push, 8)

	/* the maximum value in each grid control area */
	typedef struct __grid_min_max_value__
	{
		INT32			row,col;
		DOUBLE			power;	/* Unit : mW */

	}	STM_GMMV,	*LPG_GMMV;
#pragma pack (pop)


/* Local parameter */
protected:

	UINT8				m_u8RunMode;	/* 0x00 : Low Speed, 0x01 : High Speed */
	UINT8				m_u8MenuID;		/* 0x00 ~ 0x02 */

	TCHAR				m_tzGFile[MAX_PATH_LEN];

	DOUBLE				m_dbMaxMin;		/* Measurement Condition */

	COLORREF			m_clrGridFg;	/* Text color */
	COLORREF			m_clrGridBg;	/* Background color */

	LPG_MICL			m_pstRecipe;

	CDlgMain			*m_pDlgMain;


/* Local function */
protected:

	PTCHAR				GetGridFile(PTCHAR title);

/* Public function */
public:

	UINT8				GetMenuID()	{	return m_u8MenuID;	}


/* Message map */
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg VOID		OnShowWindow(BOOL show, UINT32 status);
};
