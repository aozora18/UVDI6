
#pragma once

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/comn/AniGif.h"

class CDlgWait : public CMyDialog
{
public:

	CDlgWait(CWnd* parent = NULL);
	enum { IDD = IDD_WAIT };

/* ���� �Լ� */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg()		{};
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg()	{};
	virtual void		RegisterUILevel() {}
public:

	virtual VOID		UpdatePeriod()	{};


/* ���� ���� */
protected:

	CPicture			m_csWaitPic;
	CAniGif				m_csWaitAni;


/* ���� �Լ� */
protected:



/* ���� �Լ� */
public:



/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
};
