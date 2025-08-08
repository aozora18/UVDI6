#pragma once
#include "../DlgSubMenu.h"
#include "../DlgSubTab.h"
 

enum EnCalbAFBtn : int
{
	loadSetting,
	saveSetting,
	readStoredvalue,
	writeStoredvalue,
	readPlot,
	focusInit,
	setInternal,
	setExternal,
	sensorOn,
	sensorOff,
	afOn,
	afOff,
	controlPanel,
	selectPhAll,
	selectPhNone,
	readAFZWorkRange,
	writeAFZWorkRange,
	plottingOn,
	plottingOff,
	refresh,
	btnmax,
};


enum EnCalbAFGrd : int
{
	setting,
	realState,
	grdmax,
};


enum EnCalbAFLabs : int
{
	labelmax=2,
};

enum EnCalbAFGroups : int
{
	groupmax = 2,
};

enum EnCalbAFChks : int
{
	chkmax = 6,
};

// CDlgCalbFlatness 대화 상자
class CDlgCalbAutofocus : public CDlgSubMenu
{
public:
	/* 생성자 / 파괴자 */
	CDlgCalbAutofocus(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgCalbAutofocus();

	CMacButton			btns[btnmax];
	CGridCtrl			grids[grdmax];
	CMyStatic			labels[labelmax];
	CMyGrpBox			group[groupmax];

	CMacCheckBox checks[chkmax];

	/* 가상함수 선언 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		UpdateControl(UINT64 tick, BOOL is_busy);
	BOOL PopupKBDN(ENM_DITM enType, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts/* = 0*/);

public:
	void InitGrid();
	void InitGridSetting();
	void InitGridRealtimeState();
	void UpdateGridInfo();
	void MoveBtns();

	virtual BOOL		OnInitDlg();
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC* dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel() {}
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy);

	void OnClickGrid(NMHDR* pNotifyStruct, LRESULT* pResult);

	
protected:

public:
	VOID LoadDataConfig();
	VOID SaveDataConfig();
	
protected:
	VOID				InitCtrl();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClick(UINT32 id);
	
	
};
