#pragma once
#include "../../DlgSubTab.h"
#include "../DlgCalbExposure.h"

enum EnCalbExposureFEMBtn
{
	eCALB_EXPOSURE_FEM_BTN_START,
	eCALB_EXPOSURE_FEM_BTN_STOP,
	eCALB_EXPOSURE_FEM_BTN_MOVE_UNLOAD,
	eCALB_EXPOSURE_FEM_BTN_MOVE_FOCUS,
	eCALB_EXPOSURE_FEM_BTN_RELOAD_LURIA_DATA,
	eCALB_EXPOSURE_FEM_BTN_JOB_LOAD,
	eCALB_EXPOSURE_FEM_BTN_JOB_REFRESH,
	eCALB_EXPOSURE_FEM_BTN_SAVE,
	eCALB_EXPOSURE_FEM_BTN_JOB_ADD,
	eCALB_EXPOSURE_FEM_BTN_JOB_DELETE,
	eCALB_EXPOSURE_FEM_BTN_OPEN_CTRL_PANEL,
	eCALB_EXPOSURE_FEM_BTN_MAX,
};

enum EnCalbExposureFEMStt
{
	eCALB_EXPOSURE_FEM_STT_OPERATION,
	eCALB_EXPOSURE_FEM_STT_PRINT_OPTION,
	eCALB_EXPOSURE_FEM_STT_POSITION,
	eCALB_EXPOSURE_FEM_STT_EXPOSURE_OPTION,
	eCALB_EXPOSURE_FEM_STT_JOBS,
	eCALB_EXPOSURE_FEM_STT_HEAD_FOCUS,
	eCALB_EXPOSURE_FEM_STT_MAX,
};

enum EnCalbExposureFEMTxt
{
	eCALB_EXPOSURE_FEM_TXT_STATE,
	eCALB_EXPOSURE_FEM_TXT_MAX,
};

enum EnCalbExposureFEMPrg
{
	eCALB_EXPOSURE_FEM_PGR_RATE,
	eCALB_EXPOSURE_FEM_PGR_MAX,
};

enum EnCalbExposureFEMGrd
{
	eCALB_EXPOSURE_FEM_GRD_RESULT,
	eCALB_EXPOSURE_FEM_GRD_OPTIC_STATE,
	eCALB_EXPOSURE_FEM_GRD_PRINT_OPTION,
	eCALB_EXPOSURE_FEM_GRD_POSITION,
	eCALB_EXPOSURE_FEM_GRD_EXPOSURE_OPTION,
	eCALB_EXPOSURE_FEM_GRD_JOBS,
	eCALB_EXPOSURE_FEM_GRD_JOB_STATE,
	eCALB_EXPOSURE_FEM_GRD_HEAD_FOCUS,
	eCALB_EXPOSURE_FEM_GRD_PRINT_TYPE,
	eCALB_EXPOSURE_FEM_GRD_HEAD_FOCUS_CTRL,
	eCALB_EXPOSURE_FEM_GRD_MAX,
};

enum EnCalbExposureFEMTimer
{
	eCALB_EXPOSURE_FEM_TIMER_UPDATE_VALUE,
	eCALB_EXPOSURE_FEM_TIMER_REQ_LURIA_LOAD_STATE,
	eCALB_EXPOSURE_FEM_TIMER_PRINT_PROGRESS,
	eCALB_EXPOSURE_FEM_TIMER_MAX
};

// CDlgCalbExposureFEM 대화 상자

class CDlgCalbExposureFEM : public CDlgSubTab
{
public:

	/* 생성자 / 파괴자 */
	CDlgCalbExposureFEM(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgCalbExposureFEM();


	enum EN_EXPOSURE_FEM_DEFINES
	{
		DEF_DEFAULT_GRID_ROW_SIZE = 40,
	};

	enum EN_GRID_OPTIC_STATE
	{
		eGRD_OPTIC_STATE_ROW = 0,

		eGRD_OPTIC_STATE_COL_TITLE = 0,
		eGRD_OPTIC_STATE_COL_VALUE,
		eGRD_OPTIC_STATE_COL_INIT,
		eGRD_OPTIC_STATE_COL_RESET,
		eGRD_OPTIC_STATE_COL_MAX,
	};

	enum EN_GRID_PRINT_OPTION
	{
		eGRD_PRINT_COL_TITLE = 0,
		eGRD_PRINT_COL_VALUE,
		eGRD_PRINT_COL_UNIT,
		eGRD_PRINT_COL_MAX,

		eGRD_PRINT_ROW_COUNT_X = 0,
		eGRD_PRINT_ROW_COUNT_Y,
		eGRD_PRINT_ROW_PERIOD_X,
		eGRD_PRINT_ROW_PERIOD_Y,
		eGRD_PRINT_ROW_MAX,
	};

	enum EN_GRID_POSITION
	{
		eGRD_POS_COL_TITLE = 0,
		eGRD_POS_COL_X,
		eGRD_POS_COL_Y,
		eGRD_POS_COL_MAX,

		eGRD_POS_ROW_TITLE = 0,
		eGRD_POS_ROW_EXPOSURE,
		eGRD_POS_ROW_UNLOAD,
		eGRD_POS_ROW_MAX,


		eGRD_FOCUS_COL_TITLE = 0,
		eGRD_FOCUS_COL_START,
		eGRD_FOCUS_COL_END,
		eGRD_FOCUS_COL_MAX,
	};

	enum EN_GRID_EXPO_OPTION
	{
		eGRD_EXPO_COL_TITLE = 0,
		eGRD_EXPO_COL_VALUE,
		eGRD_EXPO_COL_UNIT,
		eGRD_EXPO_COL_MAX,

		eGRD_EXPO_ROW_STEP_FOCUS = 0,
		eGRD_EXPO_ROW_START_ENERGY,
		eGRD_EXPO_ROW_STEP_ENERGY,
		eGRD_EXPO_ROW_MAX_ENERGY,
		eGRD_EXPO_ROW_MAX,
	};

	enum EN_GRID_JOB_STATE
	{
		eGRD_JOB_STATE_ROW = 0,

		eGRD_JOB_STATE_COL_TITLE = 0,
		eGRD_JOB_STATE_COL_STATE,
		eGRD_JOB_STATE_COL_MAX,
	};

	enum EN_GRID_PRINT_TYPE
	{
		eGRD_TYPE_COL_XF_YE = 0,
		eGRD_TYPE_COL_XE_YF,
		eGRD_TYPE_COL_MAX,

		eGRD_TYPE_ROW_PARAM = 0,
		eGRD_TYPE_ROW_MAX,
	};

	enum EN_GRID_CTRL_FOCUS
	{
		eGRD_CTRL_COL_LOAD = 0,
		eGRD_CTRL_COL_VALUE,
		eGRD_CTRL_COL_PLUS,
		eGRD_CTRL_COL_MINUS,
		eGRD_CTRL_COL_MAX,

		eGRD_CTRL_ROW_PARAM = 0,
		eGRD_CTRL_ROW_MAX,
	};

	/* 가상함수 선언 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		UpdateControl(UINT64 tick, BOOL is_busy);


public:

	virtual BOOL		OnInitDlg();
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC* dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel() {}
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy);


	/* 로컬 변수 */
protected:

	CMacButton			m_btn_ctl[eCALB_EXPOSURE_FEM_BTN_MAX];
	CMyStatic			m_stt_ctl[eCALB_EXPOSURE_FEM_STT_MAX];
	CMyStatic			m_txt_ctl[eCALB_EXPOSURE_FEM_TXT_MAX];
	CProgressCtrlX		m_pgr_ctl[eCALB_EXPOSURE_FEM_PGR_MAX];
	CGridCtrl			m_grd_ctl[eCALB_EXPOSURE_FEM_GRD_MAX];

	/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	VOID				InitGridResult(int nRowCount = 1, int nColCount = 1, BOOL bInit = FALSE);
	VOID				InitGridOpticState();
	VOID				InitGridPrintOption();
	VOID				InitGridPosition();
	VOID				InitGridExpoOption();
	VOID				InitGridJobs();
	VOID				InitGridJobState();
	VOID				InitGridHeadFocus();
	VOID				InitGridPrintType();
	VOID				InitGridHeadFocusCtrl();

	VOID				LoadDataConfig();
	VOID				SaveDataConfig();

	VOID				LoadFocusData();
	VOID				MoveFocusData(double dValue);

	VOID				UnloadPosition();
	VOID				PHMovingFocus();

	VOID				RefreshJobList();
	VOID				LoadJob();
	VOID				GetExpoParam();

	VOID				AddJob();
	VOID				DeleteJob();

	VOID				UpdateJobList();
	VOID				UpdateJobState();
	VOID				UpdateWorkProgress();
	VOID				UpdateLuriaError();
	VOID				ReqLuriaState();
	VOID				RefreshResultCell();
	VOID				SelectPrintType(UINT8 u8Type);

	VOID				CalcMaxEnergy();
	VOID				CalcStepEnergy();
	VOID				CalcMaxFocus();

	VOID				PHHWInit();

	VOID				StartPrint();
	VOID				StopPrint();

	double				GetSpeedToEnergy(double dFrameRate);

	BOOL				PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts = 0);

	/* 공용 함수 */
public:

	/* 내부 변수 */
private:

	BOOL				m_bBlink[eCALB_EXPOSURE_FEM_TIMER_MAX];

	BOOL				m_bViewCtrls;
	INT8				m_n8SelJob;
	ENG_LSLS			m_enLastJobLoadStatus;	/* 가장 최근의 Job Load 상태 값 임시 저장 */


	int					m_nPrintRow;
	int					m_nPrintCol;
	BOOL				m_bPrinting;

protected:
	LRESULT InputSetPoint(WPARAM wParam, LPARAM lParam);
	LRESULT InputPrintCompl(WPARAM wParam, LPARAM lParam);
	LRESULT ReportResult(WPARAM wParam, LPARAM lParam);

	/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClick(UINT32 id);

	afx_msg	void		OnClickGridOpticResult(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridOpticState(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridPrintOption(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridPosition(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridExposureOption(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridJobs(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridJobState(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridHeadFocus(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridPrintType(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridHeadFocusCtrl(NMHDR* pNotifyStruct, LRESULT* pResult);

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
