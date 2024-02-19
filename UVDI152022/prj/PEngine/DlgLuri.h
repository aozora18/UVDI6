
#pragma once

#include "../../inc/base/DlgChild.h"
/* Luria - Machine Config / Job Management / Panel Data / Printing / System / etc */
#include "./Luria/DlgLuria.h"
#include "./Luria/DlgLuMC.h"	/* Machine Config */
#include "./Luria/DlgLuJM.h"	/* Job Management */
#include "./Luria/DlgLuPD.h"	/* Panel Data */
#include "./Luria/DlgLuEP.h"	/* Exposure */
#include "./Luria/DlgLuDP.h"	/* Direct Photohead */


class CDlgLuri : public CDlgChild
{
public:

	CDlgLuri(UINT32 id, CWnd *parent=NULL);
	virtual ~CDlgLuri();


/* 가상함수 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();

public:

	virtual VOID		UpdateCtrl();


/* 로컬 변수 */
protected:

	CMyTabCtrl			m_tab_ctl[1];
	CDlgLuMC			*m_pDlgMC;
	CDlgLuJM			*m_pDlgJM;
	CDlgLuPD			*m_pDlgPD;
	CDlgLuEP			*m_pDlgEP;
	CDlgLuDP			*m_pDlgDP;


/* 로컬 함수 */
protected:

	BOOL				InitCtrl();

	BOOL				InitTab();
	VOID				CloseTab();

	VOID				KillChild(CDlgLuria *dlg);


/* 공용 함수 */
public:



/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
};
