
#pragma once

#include "../../inc/base/DlgChild.h"

#include "../../inc/comn/CpuUsage.h"

class CDlgMoni : public CDlgChild
{
public:

	CDlgMoni(UINT32 id, CWnd *parent=NULL);
	virtual ~CDlgMoni();


/* �����Լ� */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();

public:

	virtual VOID		UpdateCtrl();


/* ���� ���� */
protected:

	UINT64				m_u64TickHDD;		/* HDD �뷮 ���� ����͸� �˻� �ֱ� �ð� */
	UINT64				m_u64TickMEM;		/* �޸� �� CPU �˻� �ֱ� �ð� */
	UINT64				m_u64TickCPU;		/* �޸� �� CPU �˻� �ֱ� �ð� */

	CCpuUsage			m_csCPU;

	CMyGrpBox			m_grp_ctl[5];
	CMyStatic			m_txt_ctl[13];
	CMyEdit				m_edt_ctl[13];
	CMacButton			m_btn_ctl[1];

/* ���� �Լ� */
protected:

	VOID				InitCtrl();
	VOID				UpdateMoni();

	VOID				ResetDeviceInfo();


/* ���� �Լ� */
public:



/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
};
