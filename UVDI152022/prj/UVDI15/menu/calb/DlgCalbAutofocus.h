#pragma once
#include "../DlgSubMenu.h"
#include "../DlgSubTab.h"
 

enum EnCalbAFBtn : int
{
	btn_1,
	btnmax,
};

enum EnCalbAFStt : int
{
	stt_1,
	sttmax,
};

enum EnCalbAFGrd : int
{
	setting,
	grdmax,
};

enum EN_GRID_OPTION
{
	eOPTION_COL_NAME = 0,
	eOPTION_COL_VALUE1,
	eOPTION_COL_VALUE2,
	eOPTION_COL_UNIT,
	eOPTION_COL_MAX,

	eOPTION_ROW_STOREDVALUE = 0,
	eOPTION_ROW_ZPOS,
	eOPTION_ROW_MAX,

};

// CDlgCalbFlatness 대화 상자
class CDlgCalbAutofocus : public CDlgSubMenu
{
public:
	/* 생성자 / 파괴자 */
	CDlgCalbAutofocus(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgCalbAutofocus();

	CMacButton			btns[btnmax];
	CMyStatic			labels[sttmax];
	CGridCtrl			grids[grdmax];

	/* 가상함수 선언 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		UpdateControl(UINT64 tick, BOOL is_busy);


public:
	void InitGridOption();
	virtual BOOL		OnInitDlg();
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC* dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel() {}
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy);


	/* 로컬 변수 */
protected:

	 

public:
	VOID LoadDataConfig();
	

	VOID SaveDataConfig();
	

	/* 로컬 함수 */
protected:

	VOID				InitCtrl();

 
	/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClick(UINT32 id);
	
	
};
