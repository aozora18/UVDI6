
#pragma once

#include "../../inc/base/DlgChild.h"

#include "../../inc/comn/CpuUsage.h"

class CDlgMoni : public CDlgChild
{
public:

	CDlgMoni(UINT32 id, CWnd *parent=NULL);
	virtual ~CDlgMoni();


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

	UINT64				m_u64TickHDD;		/* HDD 용량 상태 모니터링 검사 주기 시간 */
	UINT64				m_u64TickMEM;		/* 메모리 및 CPU 검사 주기 시간 */
	UINT64				m_u64TickCPU;		/* 메모리 및 CPU 검사 주기 시간 */

	CCpuUsage			m_csCPU;

	CMyGrpBox			m_grp_ctl[5];
	CMyStatic			m_txt_ctl[13];
	CMyEdit				m_edt_ctl[13];
	CMacButton			m_btn_ctl[1];

/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	VOID				UpdateMoni();

	VOID				ResetDeviceInfo();


/* 공용 함수 */
public:



/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
};
