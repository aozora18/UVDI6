
#pragma once

#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/comn/MyRich.h"
#include "../../../inc/ctrl/EditCtrl.h"
#include "../../../inc/grid/GridCtrl.h"

#include "../DlgMain.h"

class CGridCtrl;

enum eMilColor
{
	eM_COLOR_BLACK = 0,
	eM_COLOR_RED,
	eM_COLOR_GREEN,
	eM_COLOR_BLUE,
	eM_COLOR_YELLOW,
	eM_COLOR_MAGENTA,
	eM_COLOR_CYAN,
	eM_COLOR_WHITE,
	eM_COLOR_GRAY,
	eM_COLOR_BRIGHT_GRAY,
	eM_COLOR_LIGHT_GRAY,
	eM_COLOR_LIGHT_GREEN,
	eM_COLOR_LIGHT_BLUE,
	eM_COLOR_LIGHT_WHITE,
	eM_COLOR_DARK_RED,
	eM_COLOR_DARK_GREEN,
	eM_COLOR_DARK_BLUE,
	eM_COLOR_DARK_YELLOW,
	eM_COLOR_DARK_MAGENTA,
	eM_COLOR_DARK_CYAN,
};

enum EN_MSG_BOX_TYPE
{
	eINFO,
	eSTOP,
	eWARN,
	eQUEST,
};

class CDlgMenu : public CMyDialog
{
public:

	/* 생성사 / 파괴자 */
	CDlgMenu(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgMenu();
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

	ENG_CMDI			m_enDlgID;	/* Dialog ID */
	ENG_CMDI_SUB		m_enSubDlgID;	/* Dialog ID */
	UINT64				m_u64ReqCmdTime;

	CDlgMain			*m_pDlgMain;


/* 로컬 함수 */
protected:



/* 공용 함수 */
public:

	VOID				DrawOutLine();
	ENG_CMDI			GetDlgID()		{	return m_enDlgID;	}

	BOOL				PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts = 0);
	int					ShowMsg(EN_MSG_BOX_TYPE mType, LPCTSTR lpszMsg, ...);
	int					ShowYesNoMsg(EN_MSG_BOX_TYPE mType, LPCTSTR lpszMsg, ...);
	int					ShowMultiSelectMsg(EN_MSG_BOX_TYPE mType, CString strTitle, CStringArray& strArrCommand);
/* 메시지 맵 */
protected:

	DECLARE_MESSAGE_MAP()
};
