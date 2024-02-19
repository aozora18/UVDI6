#pragma once

class CWndCtrl
{
/* Constructor and Destructor */
public:

	CWndCtrl();
	virtual ~CWndCtrl();

/* Virtual Function */
protected:

public:


/* Member Parameter : Local */
protected:



/* Member Function : Local */
protected:


/* Member Function : Public */
public:

	/* Common */
	BOOL				LoadPlacement(PTCHAR section, HWND hwnd);
	BOOL				MoveSubMonitor(HWND hwnd);
	BOOL				RefreshTrayIcon();
	BOOL				SavePlacement(PTCHAR section, HWND hwnd);

	/* Button */

	/* Combobox */

	/* Editbox */
	VOID				AppendEditMesg(CEdit *edit, PTCHAR mesg, BOOL clean, BOOL enter, UINT8 scroll);

	/* List Control */
	VOID				MoveScrollPosList(HWND ctrl, UINT32 pos, BOOL select);
};
