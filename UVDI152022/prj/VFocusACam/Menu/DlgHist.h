
#pragma once

#include "DlgMenu.h"

class CDlgHist : public CDlgMenu
{
public:

	CDlgHist(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgHist();


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
	virtual VOID		ResetData();


/* ���� ���� */
protected:

	CHAR				m_szFileDOF[MAX_PATH_LEN];		/* DOF ���� ������ ������ ���� ���� �� (��� ����) */

	DOUBLE				m_dbMinMax[2][COLLECT_DATA_COUNT];

	/* Double Buffering ��� */
	CDC					m_dcMemHist;
	CBitmap				m_bmMemHist, *m_bmHistOld;
	CRect				m_rMemHist;

	CMyStatic			m_pic_ctl[1];


/* ���� �Լ� */
protected:

	VOID				InitCtrl();

	BOOL				InitMemDC();
	VOID				CloseMemDC();
	VOID				DrawHist();


/* ���� �Լ� */
public:



/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()

};
