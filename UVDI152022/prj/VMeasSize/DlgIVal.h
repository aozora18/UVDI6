
#pragma once

#include "../../inc/comn/MyDialog.h"
#include "../../inc/comn/MyGrpBox.h"
#include "../../inc/comn/MacButton.h"
#include "../../inc/comn/MyEdit.h"

class CDlgIVal : public CMyDialog
{
public:

	CDlgIVal(CWnd* parent = NULL);
	enum { IDD = IDD_IVAL };

	BOOL				Create(UINT8 type, INT32 item, INT32 val);


/* Virtual function */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();

public:



/* Local parameter */
protected:

	UINT8				m_u8Type;	/* 0x00:Integer, 0x01:Double */

	INT32				m_i32Item;
	INT32				m_i32Val;
	DOUBLE				m_dbVal;

	CMyGrpBox			m_grp_ctl[1];
	CMacButton			m_btn_ctl[2];
	CMyEdit				m_edt_ctl[1];


/* Local function */
protected:

	VOID				InitCtrl();
	VOID				ApplyValue();
	VOID				CancelValue();
	VOID				SetValue();


/* Public function */
public:

	UINT8				GetType()		{	return m_u8Type;	}
	INT32				GetItem()		{	return m_i32Item;	}
	INT64				GetValInt32()	{	return m_i32Val;	}
	DOUBLE				GetValDbl()		{	return m_dbVal;		};
	VOID				SetValue(UINT8 type, INT32 item, INT32 value);


/* Message map */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
};
