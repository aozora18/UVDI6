
#pragma once

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/grid/gridctrl.h"

class CDlgSped : public CMyDialog
{
public:

	CDlgSped(CWnd* parent = NULL);
	enum { IDD = IDD_SPED };

/* ���� �Լ� */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel() {}

public:

	virtual VOID		UpdatePeriod()	{};


/* ���� ���� */
protected:

	DOUBLE				m_dbFrameRate;	/* �׸��� �׸񿡼� ���õ� �� */

	CMyGrpBox			m_grp_ctl[1];
	CMacButton			m_btn_ctl[2];	/* Normal */

	CGridCtrl			*m_pGrid;


/* ���� �Լ� */
protected:

	BOOL				InitCtrl();
	BOOL				InitGrid();

	VOID				LoadFrameRate();


/* ���� �Լ� */
public:

	INT_PTR				MyDoModal();

	DOUBLE				GetFrameRate()	{	return m_dbFrameRate;	}


/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnGridFrameRate(NMHDR *nm_hdr, LRESULT *result);
};
