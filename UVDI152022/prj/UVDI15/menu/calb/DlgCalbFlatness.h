#pragma once
#pragma once
#include "../DlgSubMenu.h"
#include "../DlgSubTab.h"

#define DEF_FLATNESS_CONFIG		_T("PHILHMI_FLATNESS.csv")
#define DEF_DEVICE_NUM			0

enum EnCalbFlatnessTimer
{
	eCALB_FLATNESS_TIMER_START = 1,
};

enum EnFlatnessProcessStep
{
	eFLAT_PROC_FAIL = -1,
	eFLAT_PROC_INIT = 0,
	eFLAT_PROC_MOVE_WAIT_POS,
	eFLAT_PROC_MOVE_MEASURE_POS,
	eFLAT_PROC_MEASUREMENT,
	eFLAT_PROC_END,
};

typedef struct stDoublePoint
{
	double	x;		// X축 점
	double	y;		// Y축 점
	
	stDoublePoint()
	{
		x = 0.;
		y = 0.;
	}

} DPOINT;

// CDlgCalbFlatness 대화 상자
class CDlgCalbFlatness : public CDlgSubMenu
{
public:
	enum EnSTTCalbFlatness
	{
		eSTT_FLATNESS_TITLE_1,
		eSTT_FLATNESS_TITLE_2,
		eSTT_FLATNESS_TITLE_3,
		eSTT_FLATNESS_TITLE_4,
		eSTT_FLATNESS_TITLE_MAX,
	};

	enum EnBtnCalbFlatness
	{
		eBTN_FLATNESS_SAVE_DATA,
		eBTN_FLATNESS_REMEASUREMENT,
		eBTN_FLATNESS_POINT_MOVE,
		eBTN_FLATNESS_START,
		eBTN_FLATNESS_STOP,
		eBTN_FLATNESS_MANUAL_MEASURE,
		eBTN_FLATNESS_EDIT,
		eBTN_FLATNESS_OPEN_CTRL_PANEL,
		eBTN_FLATNESS_MAX,
	};

	enum EnGrdCalbFlatness
	{
		eGRD_FLATNESS_RESULT,
		eGRD_FLATNESS_MANUAL_MEASURE,
		eGRD_FLATNESS_SETTING,
		eGRD_FLATNESS_MAX,
	};

	enum EnGrdSettingItem
	{
		eFLAT_SETTING_COL_LEFT_NAME = 0,
		eFLAT_SETTING_COL_LEFT_POS,
		eFLAT_SETTING_COL_LEFT_MOVE = eFLAT_SETTING_COL_LEFT_POS,
		eFLAT_SETTING_COL_LEFT_PARAM,
		eFLAT_SETTING_COL_LEFT_UNIT,

		eFLAT_SETTING_COL_RIGHT_NAME,
		eFLAT_SETTING_COL_RIGHT_POS,
		eFLAT_SETTING_COL_RIGHT_MOVE = eFLAT_SETTING_COL_RIGHT_POS,
		eFLAT_SETTING_COL_RIGHT_PARAM,
		eFLAT_SETTING_COL_RIGHT_UNIT,

		eFLAT_SETTING_COL_MAX,

		eFLAT_SETTING_ROW_POS_X = 0,
		eFLAT_SETTING_ROW_POS_Y,
		eFLAT_SETTING_ROW_POS_T,
		eFLAT_SETTING_ROW_POS_Z,
		eFLAT_SETTING_ROW_MOVE,

		eFLAT_SETTING_ROW_PARAM_X,
		eFLAT_SETTING_ROW_PARAM_Y,
		eFLAT_SETTING_ROW_PARAM_T,
		eFLAT_SETTING_ROW_PARAM_Z,

		eFLAT_SETTING_ROW_MEASURE,

		eFLAT_SETTING_ROW_MAX,
	};

	enum EnGrdManualMeasureItem
	{
		//eFLAT_MANUAL_ROW_DEVICE,
		eFLAT_MANUAL_ROW_RESULT = 0,
		eFLAT_MANUAL_ROW_BASE,
		eFLAT_MANUAL_ROW_MAX,

		eFLAT_MANUAL_COL_TITLE = 0,
		eFLAT_MANUAL_COL_VALUE,
		eFLAT_MANUAL_COL_MAX,
	};

	enum EnGrdResultItem
	{
		eNONE = -1,
		eFLAT_RESULT_TITLE,
		eFLAT_RESULT_ADD_TITLE,
		eFLAT_RESULT_MAX,
	};

	struct ST_GRID_COL
	{
		CString strTitle;
		UINT nFormat;
		int nWidth;
	};

	/* 생성자 / 파괴자 */
	CDlgCalbFlatness(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgCalbFlatness();


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

	CMyStatic			m_stt_ctl[eSTT_FLATNESS_TITLE_MAX];
 	CMacButton			m_btn_ctl[eBTN_FLATNESS_MAX];
	CGridCtrl			m_grd_ctl[eGRD_FLATNESS_MAX];

	BOOL				m_bEditMode;
	BOOL				m_bBtnManualFlag;
	BOOL				m_bBtnProcFlag;
	double				m_dOldBasseValue;

	CCellID				m_cellOldPos;

	CWinThread*			m_pLDSMeasureThread;
	CWinThread*			m_pFlatProcThread;

public:

	int					m_nStep;
	int					m_nCurSeqNum;
	BOOL				m_bStop;

	BOOL				m_bLDSMeasureThread;
	BOOL				m_bIngLDSMeasureThread;

	BOOL				m_bFlatProcThread;
	BOOL				m_bIngFlatProcThread;

	std::vector<double>	m_dVctData;

	/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	VOID				InitGridResult();
	VOID				InitGridSetting();
	VOID				InitGridManualMeasure();

	VOID				RefreshResultGrid();
	VOID				SetResultGridRestartColor();

	VOID				LoadDataConfig();
	VOID				SaveDataConfig();
	BOOL				SaveResult(CString strPath);

	BOOL				BtnAuthority(int nBtn);

	// Manual Measure
	VOID				StartManualMeasure();
	VOID				StopManualMeasure();

	BOOL				StageMoveXY(double dX, double dY, double dSpeedX, double dSpeedY);
	BOOL				IsStageMovedXY(double dTargetX, double dTargetY, int nTimeOut = 60000, double dDiffDistance = 0.005);

	VOID				OnClickButtonSaveData();
	VOID				OnClickButtonRemeasurement();
	VOID				OnClickButtonPointMove();
	VOID				OnClickButtonStart();
	VOID				OnClickButtonStop();
	VOID				OnClickButtonManualMeasure();
	VOID				OnClickButtonEdit();
	VOID				OnClickButtonOpenCtrlPanel();
	VOID				OnClickMoveFlatPos(int nPos);

	/* 공용 함수 */
public:

	LOGFONT				GetUserLogFont(int nSize, BOOL bIsBold);

	void				InputManualValue(double dValue);
	VOID				InputResultGridValue(int nRow, int nCol, double dData);

	VOID				CleerResultGridColor();
	VOID				SetResultGridCellColor(int nRow, int nCol, COLORREF cColor);

	DPOINT				CalOffset(int nRow, int nCol);
	BOOL				MoveStartPos();
	BOOL				MoveEndPos();
	BOOL				MoveOffsetPos(DPOINT dOffset);

	BOOL				PopupKBDN(ENM_DITM enType, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts = 0);

	/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClick(UINT32 id);
	afx_msg VOID		OnTimer(UINT_PTR nIDEvent);
	afx_msg VOID		OnGrdResultClick(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg VOID		OnGrdManualMeasureClick(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg VOID		OnGrdSettingClick(NMHDR* pNotifyStruct, LRESULT* pResult);
};

void Wait_(int nTime_);
UINT LDSManualMeasureThread(LPVOID lpParam);
UINT FlatnessProcessThread(LPVOID lpParam);