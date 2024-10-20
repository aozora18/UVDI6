
#pragma once

#include "DlgMenu.h"

#include "../../../prj/UVDI15/param/LogManager.h"
#include "../../../inc/grid/GridCellDateTime.h"


#define _CAL_PERCENTAGE_(value, total) (int)(((double)value / (double)total) * 100.0)

class CGridFile;	/* Log Files */

enum EnLogGrp
{
	eLOGS_GRP_LIST		,
	eLOGS_GRP_CONTENT	,
	eLOGS_GRP_FILTER	,
	eLOGS_GRP_OPTION	,
	eLOGS_GRP_MAX,
};

enum EnLogBtn
{
	eLOGS_BTN_TODAY		,
	eLOGS_BTN_THIS_WEEK	,
	eLOGS_BTN_SEARCH	,
	eLOGS_BTN_STOP		,
	eLOGS_BTN_MAX		,
};

enum EnLogType
{
	eLOGS_TYPE_SERVICE = 0,
	eLOGS_TYPE_CONTROL,
	eLOGS_TYPE_MOTION,

	eLOGS_TYPE_LIBRARY,
	eLOGS_TYPE_VISION,
	eLOGS_TYPE_PHILHMI,

	eLOGS_TYPE_PGFS,
	eLOGS_TYPE_CALIBRATION,
	eLOGS_TYPE_ETC,

	eLOGS_TYPE_MAX
};

enum EnLogViewOption
{
	eLOGS_VOP_SYNC_SCROLL = 0,

	eLOGS_VOP_MAX,
};

enum EnLogTitle
{
	eLOGS_TTL_CONTENT_1,
	eLOGS_TTL_CONTENT_2,
	eLOGS_TTL_CONTENT_3,
	eLOGS_TTL_CONTENT_4,
	eLOGS_TTL_SEARCH,
	eLOGS_TTL_MAX,
};

enum EnLogRch
{
	eLOGS_RCH_CONTENT_1,
	eLOGS_RCH_CONTENT_2,
	eLOGS_RCH_CONTENT_3,
	eLOGS_RCH_CONTENT_4,
	eLOGS_RCH_MAX,
};

enum EnLogGrd
{
	eLOGS_GRD_SEARCH,

	eLOGS_GRD_MAX,
};

enum EnLogProgressBar
{
	eLOGS_PGR_LOADING,

	eLOGS_PGR_MAX,
};

typedef enum EnLogStep
{
	eLOGS_STEP_INIT,
	eLOGS_STEP_FILE_LOAD,
	eLOGS_STEP_VIEW_UPDATE,
} EN_LOG_STEP;



class CDlgLogs : public CDlgMenu
{
	struct ST_GRID_LOGS_SEARCH
	{
		CString strTitle;
		UINT	nFormat;
		int		nWidth;
	};

	TCHAR m_tzType[MAX_PATH_COUNT][12] = { L"Service", L"Control", L"Motion", L"Library", L"Vision", L"PhilHmi", L"PGFS", L"Calibration", L"Etc" };

public:

	/* 생성자 / 파괴자 */
	CDlgLogs(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgLogs();


/* 가상함수 선언 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		UpdateControl(UINT64 tick, BOOL is_busy);


public:

	virtual BOOL		OnInitDlg();
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel() {}
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy);


/* 로컬 변수 */
protected:

	CMyGrpBox			m_grp_ctl[eLOGS_GRP_MAX];
	CMacButton			m_btn_ctl[eLOGS_BTN_MAX];
	CMacCheckBox		m_chk_flt[eLOGS_TYPE_MAX];
	CMacCheckBox		m_chk_vop[eLOGS_VOP_MAX];
	CMyStatic			m_ttl_ctl[eLOGS_TTL_MAX];
	CMyStatic			m_pgr_bar[eLOGS_PGR_MAX];
	CMyRich				m_rch_ctl[eLOGS_RCH_MAX];
	CGridCtrl			m_grd_ctl[eLOGS_GRD_MAX];
	CProgressCtrlX*		m_pPgr;

	CRect				m_rtOriginTTL[eLOGS_TTL_MAX];
	CRect				m_rtOriginRCH[eLOGS_RCH_MAX];

	vST_LOG_MSG			m_vLogs[eLOGS_TYPE_MAX];
	CCriticalSection	m_csLogs;


	thread			m_pLogThread;
	atomic<bool>	m_bStopThread = false;

	UINT8				m_nChecked;

	EN_LOG_STEP			m_eSTEP;
	BOOL				m_bStop;

/* 로컬 함수 */
protected:

	VOID				InitCtrl();

	BOOL				InitObject();
	VOID				CloseObject();
	VOID				PartitionObject(int nChecked);

	BOOL				InitGrid();
	VOID				CloseGrid();

	VOID				GetLogInfo(vST_LOG_MSG* vLogs);
	VOID				UpdateList();
	VOID				DeleteFile();

	VOID				SetGridTextToday();
	VOID				SetGridTextThisWeek();
	VOID				GetGridTextData(COleDateTime& tStart, COleDateTime& tEnd);

	VOID				SetStop();

	VOID				InitTitles();
	VOID				InitContents();
	VOID				UpdateLogTitles();
	VOID				UpdateLogContents();

	VOID				InitListFirstLine();

	UINT			LogThreadProc();

/* 공용 함수 */
public:
	void				StartLogThread();
	void				StopLogThread();
	EN_LOG_STEP			GetLogStep();

	

	afx_msg LRESULT		OnSyncScroll(WPARAM wParam, LPARAM lParam);

/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
 	afx_msg VOID		OnBtnClick(UINT32 id);
 	afx_msg VOID		OnChkClick(UINT32 id);
	afx_msg VOID		OnOptClick(UINT32 id);

public:
};
