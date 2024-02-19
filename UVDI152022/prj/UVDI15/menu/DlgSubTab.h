
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

	/* 생성사 / 파괴자 */
	CDlgSubTab(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgSubTab();
	/* 모달리스 다이얼로그 생성 */
	BOOL				Create(HWND h_out);


/* 가상함수 선언 */
protected:
	virtual VOID		UpdateControl(UINT64 tick, BOOL is_busy)= 0;
public:
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy)	= 0;

/* 열거형 선언 */
protected:


/* 로컬 변수 */
protected:

	ENG_CMDI_TAB		m_enDlgID;	/* Dialog ID */

	UINT64				m_u64ReqCmdTime;

	CDlgMain			*m_pDlgMain;


/* 로컬 함수 */
protected:



/* 공용 함수 */
public:

	VOID				DrawOutLine();
	ENG_CMDI_TAB		GetDlgID()		{	return m_enDlgID;	}


/* 메시지 맵 */
protected:

	DECLARE_MESSAGE_MAP()
};
