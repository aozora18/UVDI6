#pragma once
#include "../../DlgSubTab.h"
#include "../DlgCalbAccuracy.h"
#include "..\..\..\..\..\inc\conf\vision_uvdi15.h"


#define DEF_DEFAULT_INTERLOCK_TIMEOUT	60 * 1000



enum EnCalbAccuracyMeasureBtn
{
	eCALB_ACCURACY_MEASURE_BTN_OPEN_CTRL_PANEL,
	eCALB_ACCURACY_MEASURE_BTN_STOP,
	eCALB_ACCURACY_MEASURE_BTN_START,
	eCALB_ACCURACY_MEASURE_BTN_LIVE,
	eCALB_ACCURACY_MEASURE_BTN_LOAD,
	eCALB_ACCURACY_MEASURE_BTN_NEW,
	eCALB_ACCURACY_MEASURE_BTN_REMEASURE,
	eCALB_ACCURACY_MEASURE_BTN_MOVE,
	eCALB_ACCURACY_MEASURE_BTN_GRAB,
	eCALB_ACCURACY_MEASURE_BTN_MAKE_FILE,
	eCALB_ACCURACY_MEASURE_BTN_EXCEL_EXPORT,
	eCALB_ACCURACY_MEASURE_BTN_MAX,
};

enum EnCalbAccuracyMeasureStt
{
	eCALB_ACCURACY_MEASURE_STT_POINT,
	eCALB_ACCURACY_MEASURE_STT_CAMERA_VIEW,
	eCALB_ACCURACY_MEASURE_STT_OPTION,
	eCALB_ACCURACY_MEASURE_STT_POSITION,
	eCALB_ACCURACY_MEASURE_STT_MAX,
};

enum EnCalbAccuracyMeasurePic
{
	eCALB_ACCURACY_MEASURE_PIC_ROI,
	eCALB_ACCURACY_MEASURE_PIC_MAX,
};

enum EnCalbAccuracyMeasureGrd
{
	eCALB_ACCURACY_MEASURE_GRD_SELECT_CAM,
	eCALB_ACCURACY_MEASURE_GRD_OPTION,
	eCALB_ACCURACY_MEASURE_GRD_DATA,
	eCALB_ACCURACY_MEASURE_GRD_MAX,
};

enum EnCalbAccuracyMeasureTimer
{
	eCALB_ACCURACY_MEASURE_TIMER_WORK,
	eCALB_ACCURACY_MEASURE_TIMER_MOVE_START,
	eCALB_ACCURACY_MEASURE_TIMER_MAX
};



// CDlgCalbAccuracyMeasure 대화 상자

class CDlgCalbAccuracyMeasure : public CDlgSubTab
{
public:


	/* 생성자 / 파괴자 */
	CDlgCalbAccuracyMeasure(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgCalbAccuracyMeasure();
	const int CAMER_ACOUNT = uvEng_GetConfig()->set_cams.acam_count;



	enum EN_GRID_OPTION
	{
		eOPTION_COL_NAME = 0,
		eOPTION_COL_VALUE,
		eOPTION_COL_MAX,

		eOPTION_ROW_X_DRV = 0,
		eOPTION_ROW_USE_CAL,
		eOPTION_ROW_CAL_FILE_PATH,
		
		eOPTION_DOUBLE_MARK,
		eOPTION_EXPO_AREA_MEASURE,
		eOPTION_ROW_MAX,
	};

	enum EN_GRID_CAMERA_OPTION
	{
		eCAM_OPTION_COL_NAME = 0,
		eCAM_OPTION_COL_VALUE,
		eCAM_OPTION_COL_SET,
		eCAM_OPTION_COL_MAX,

		eCAM_OPTION_ROW_GAIN1 = 0,
		eCAM_OPTION_ROW_GAIN2,
		eCAM_OPTION_ROW_MAX,
	};

	enum EN_GRID_POINT
	{
		eRESULT_COL_INDEX = 0,
		eRESULT_COL_POINT_X,
		eRESULT_COL_POINT_Y,
		eRESULT_COL_VALUE_X,
		eRESULT_COL_VALUE_Y,
		eRESULT_COL_MAX,
	};

	enum EN_ACCURACY_MEASURE_DEFINES
	{
		//DEF_CAMERA_COUNT = 3,
		DEF_DEFAULT_GRID_ROW_SIZE = 35,
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

	CMacButton			m_btn_ctl[eCALB_ACCURACY_MEASURE_BTN_MAX];
	CMyStatic			m_stt_ctl[eCALB_ACCURACY_MEASURE_STT_MAX];
	CMyStatic			m_pic_ctl[eCALB_ACCURACY_MEASURE_PIC_MAX];
	CGridCtrl			m_grd_ctl[eCALB_ACCURACY_MEASURE_GRD_MAX];

	/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	VOID				InitGridSelectCam();
	VOID				InitGridOption();
	VOID				InitGridData(BOOL bIsReload = FALSE);
	VOID				InitDispMark();

	VOID				DispResize(CWnd* pWnd);
	VOID				InvalidateView();
	VOID				SetLiveView(BOOL bLive);
	VOID				UpdateLiveView();
	VOID				ResetDataViewPoint(UINT32 u32Row);
	bool				IsSearchMarkTypeChanged();
	BOOL				SaveClacFile();

	VOID				LoadMeasureField();
	VOID				LoadCalibrationFile();
	VOID				MakeField();
	VOID				SaveAsExcelFile();
	VOID				MeasureStart();
	BOOL				MarkGrab(double* pdErrX = NULL, double* pdErrY = NULL);
	VOID				HoldControl(BOOL bSet);

	BOOL				MotionMove(ENG_MMDI eMotor, double dPos);
	VOID				PointMoveStart();
	VOID				Remeasurement();

	VOID				DoEvents();
	VOID				Wait_(DWORD dwTime);

	/* 공용 함수 */
public:
	UINT8				GetCheckACam() { return m_u8SelHead; }
	/* 내부 변수 */
private:

	BOOL				m_bCamLiveState;
	UINT8				m_u8SelHead;
	UINT32				m_u32IndexNum;
	
	unsigned long long	m_ullTimeMove;

	BOOL				m_bHoldCtrl;


protected:
	LRESULT				RefreshGrabView(WPARAM wParam, LPARAM lParam);
	LRESULT				ReciveReportMsg(WPARAM wParam, LPARAM lParam);

	/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClick(UINT32 id);
	afx_msg	void		OnClickGridSelectCam(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridOption(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridData(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void		OnTimer(UINT_PTR nIDEvent);
};


