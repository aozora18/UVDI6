
/*
 desc : Result - Base
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgMenu.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : constructor
 parm : id		- [in]  dialog id
		recipe	- [in]  Measurement Information
		parent	- [in]  parent object pointer
 retn : None
*/
CDlgMenu::CDlgMenu(UINT32 id, LPG_MICL recipe, UINT8 menu_id, CWnd *parent)
	: CDlgChild(id, parent)
{
	m_u8MenuID	= menu_id;
	m_pstRecipe	= recipe;
	m_pDlgMain	= (CDlgMain *)m_pParentWnd;

	m_clrGridFg	= RGB(16, 16, 16);
	m_clrGridBg	= RGB(245, 245, 245);
}

/*
 desc : destructor
 parm : None
 retn : None
*/
CDlgMenu::~CDlgMenu()
{
}

BEGIN_MESSAGE_MAP(CDlgMenu, CDlgChild)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/*
 desc : When the windows is running, it is initially called once
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMenu::OnInitDlg()
{
	if (!CDlgChild::OnInitDlg())	return FALSE;

	/* 상위 객체 호출 */
	return TRUE;
}

/*
 desc : When the window is stopping, it is called once at the last
 parm : None
 retn : None
*/
VOID CDlgMenu::OnExitDlg()
{
	ReleaseWork();
}

/*
 desc : Called automatically when window is show or hide
 parm : show	- [in]  TRUE : show, FALSE : hide
		status	- [in]  Specifies the status of the window being shown
						It is 0 if the message is sent because of a ShowWindow member function call; otherwise nStatus is one of the following:
						SW_PARENTCLOSING : Parent window is closing (being made iconic) or a pop-up window is being hidden.
						SW_PARENTOPENING : Parent window is opening (being displayed) or a pop-up window is being shown.
 retn : None
*/
VOID CDlgMenu::OnShowWindow(BOOL show, UINT32 status)
{
	ShowHide(show);
}

/*
 desc : Return the file name for storing the contents of the grid
 parm : title	- [in]  Title Name
 retn : Grid File Name
*/
PTCHAR CDlgMenu::GetGridFile(PTCHAR title)
{
	TCHAR tzLed[4][8]	= { L"365nm", L"385nm", L"395nm", L"405nm" };
	SYSTEMTIME stTm	= {NULL};
	/* Get the current local time */
	GetLocalTime(&stTm);
	/* Set the file name */
	swprintf_s(m_tzGFile, MAX_PATH_LEN, L"%s\\illu\\%s_%04u%02u%02u_%02u%02u%02u(PH%u_%s).csv",
			   g_tzWorkDir, title, stTm.wYear, stTm.wMonth, stTm.wDay,
			   stTm.wHour, stTm.wMinute, stTm.wSecond,
			   m_pstRecipe->ph_no, tzLed[m_pstRecipe->led_no-1]);
	/* Check whether it is an existing file */
	if (uvCmn_FindFile(m_tzGFile))
	{
		if (IDYES != AfxMessageBox(L"An existing file exists. Overwrite it?", MB_YESNO))
			return NULL;
	}

	return m_tzGFile;
}

/*
 desc : Initializes for LED Measurement Area
 parm : run_mode- [in]  0x00 : Low Speed (Safe Mode), 0x01 : High Speed (Incomplete Mode)
		max_min	- [in]  Max-Min Condition
						If the difference between the maximum and minimum values of the data stored
						in the queue buffer is less than this value, collection is possible
 retn : TRUE or FALSE
*/
BOOL CDlgMenu::StartWork(UINT8 run_mode, DOUBLE max_min)
{
	m_u8RunMode	= run_mode;
	m_dbMaxMin	= max_min;

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
	/* Check if it is in safe position */
	if (!uvEng_GetConfig()->IsRunDemo())
	{
		if (!uvEng_ShMem_GetPLCStruct()->IsLiftPinSafePos())
		{
			AfxMessageBox(L"It is not in safe position", MB_ICONSTOP|MB_TOPMOST);
			return FALSE;
		}
	}
#endif
	return TRUE;
}