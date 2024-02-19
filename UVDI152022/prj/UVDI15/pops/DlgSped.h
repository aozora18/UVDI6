
#pragma once

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/grid/gridctrl.h"

class CDlgSped : public CMyDialog
{
public:

	CDlgSped(CWnd* parent = NULL);
	enum { IDD = IDD_SPED };

/* 가상 함수 */
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


/* 로컬 변수 */
protected:

	DOUBLE				m_dbFrameRate;	/* 그리드 항목에서 선택된 값 */

	CMyGrpBox			m_grp_ctl[1];
	CMacButton			m_btn_ctl[2];	/* Normal */

	CGridCtrl			*m_pGrid;


/* 로컬 함수 */
protected:

	BOOL				InitCtrl();
	BOOL				InitGrid();

	VOID				LoadFrameRate();


/* 공용 함수 */
public:

	INT_PTR				MyDoModal();

	DOUBLE				GetFrameRate()	{	return m_dbFrameRate;	}


/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnGridFrameRate(NMHDR *nm_hdr, LRESULT *result);
};
