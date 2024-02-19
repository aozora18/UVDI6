
#pragma once

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/comn/AniGif.h"

class CDlgWait : public CMyDialog
{
public:

	CDlgWait(CWnd* parent = NULL);
	enum { IDD = IDD_WAIT };

/* 가상 함수 */
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


/* 로컬 변수 */
protected:

	CPicture			m_csWaitPic;
	CAniGif				m_csWaitAni;


/* 로컬 함수 */
protected:



/* 공용 함수 */
public:



/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
};
