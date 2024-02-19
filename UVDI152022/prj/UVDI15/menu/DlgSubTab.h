
#pragma once

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/comn/MyRich.h"
#include "../../../inc/ctrl/EditCtrl.h"
#include "../../../inc/grid/GridCtrl.h"

#include "../DlgMain.h"

class CGridCtrl;

class CDlgSubTab : public CMyDialog
{
public:

	/* ������ / �ı��� */
	CDlgSubTab(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgSubTab();
	/* ��޸��� ���̾�α� ���� */
	BOOL				Create(HWND h_out);


/* �����Լ� ���� */
protected:
	virtual VOID		UpdateControl(UINT64 tick, BOOL is_busy)= 0;
public:
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy)	= 0;

/* ������ ���� */
protected:


/* ���� ���� */
protected:

	ENG_CMDI_TAB		m_enDlgID;	/* Dialog ID */

	UINT64				m_u64ReqCmdTime;

	CDlgMain			*m_pDlgMain;


/* ���� �Լ� */
protected:



/* ���� �Լ� */
public:

	VOID				DrawOutLine();
	ENG_CMDI_TAB		GetDlgID()		{	return m_enDlgID;	}


/* �޽��� �� */
protected:

	DECLARE_MESSAGE_MAP()
};
