#pragma once
#include "../../DlgSubTab.h"
#include "../DlgCalbCamera.h"

enum EnCalbCameraSpecBtn
{
	eCALB_CAMERA_SPEC_BTN_TEST_MATCH,
	eCALB_CAMERA_SPEC_BTN_START,
	eCALB_CAMERA_SPEC_BTN_STOP,
	eCALB_CAMERA_SPEC_BTN_SAVE_RESULT,
	eCALB_CAMERA_SPEC_BTN_VIEW_OPTION,
	eCALB_CAMERA_SPEC_BTN_OPEN_CTRL_PANEL,
	eCALB_CAMERA_SPEC_BTN_SAVE,
	eCALB_CAMERA_SPEC_BTN_LIVE,
	eCALB_CAMERA_SPEC_BTN_MAX,
};

enum EnCalbCameraSpecStt
{
	eCALB_CAMERA_SPEC_STT_CAMERA_VIEW,
	eCALB_CAMERA_SPEC_STT_OPERATION_MODE,
	eCALB_CAMERA_SPEC_STT_OPERATION,
	eCALB_CAMERA_SPEC_STT_RESULT,
	eCALB_CAMERA_SPEC_STT_TRIGGER_OPTION,
	eCALB_CAMERA_SPEC_STT_OPTION,
	eCALB_CAMERA_SPEC_STT_CAMERA_OPTION,
	eCALB_CAMERA_SPEC_STT_POSITION,
	eCALB_CAMERA_SPEC_STT_MAX,
};

enum EnCalbCameraSpecTxt
{
	eCALB_CAMERA_SPEC_TXT_STATE,
	eCALB_CAMERA_SPEC_TXT_MAX,
};

enum EnCalbCameraSpecPrg
{
	eCALB_CAMERA_SPEC_PGR_RATE,
	eCALB_CAMERA_SPEC_PGR_MAX,
};

enum EnCalbCameraSpecPic
{
	eCALB_CAMERA_SPEC_PIC_ROI,
	eCALB_CAMERA_SPEC_PIC_MAX,
};

enum EnCalbCameraSpecGrd
{
	eCALB_CAMERA_SPEC_GRD_SELECT_CAM,
	eCALB_CAMERA_SPEC_GRD_RESULT,
	eCALB_CAMERA_SPEC_GRD_SELECT_MODE,
	eCALB_CAMERA_SPEC_GRD_REPEAT_COUNT,
	eCALB_CAMERA_SPEC_GRD_TRIGGER_OPTION,
	eCALB_CAMERA_SPEC_GRD_OPTION,
	eCALB_CAMERA_SPEC_GRD_CAMERA_OPTION,
	eCALB_CAMERA_SPEC_GRD_POSITION,
	eCALB_CAMERA_SPEC_GRD_MAX,
};

enum EnCalbCameraSpecTimer
{
	eCALB_CAMERA_SPEC_TIMER_LIVE,
	eCALB_CAMERA_SPEC_TIMER_MAX
};

// CDlgCalbCameraSpec 대화 상자

class CDlgCalbCameraSpec : public CDlgSubTab
{
public:
	/* 생성자 / 파괴자 */
	CDlgCalbCameraSpec(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgCalbCameraSpec();
	
	enum EN_GRID_SETTING
	{
		eSETTING_COL_NAME = 0,
		eSETTING_COL_VALUE,
		eSETTING_COL_MAX,

		eSETTING_COL_ANGLE = 0,
		eSETTING_COL_PIXEL_SIZE,
		eSETTING_COL_MAX_MODE,

		eSETTING_ROW_ITEM = 0,
		eSETTING_ROW_MAX,
	};

	enum EN_GRID_OPTION
	{
		eOPTION_COL_NAME = 0,
		eOPTION_COL_VALUE,
		eOPTION_COL_UNIT,
		eOPTION_COL_MAX,

		eOPTION_ROW_TRIGGER_ON_TIME = 0,
		eOPTION_ROW_STROBE_ON_TIME,
		eOPTION_ROW_DELAY_TIME,
		eOPTION_ROW_TRIGGER_MAX,

		eOPTION_ROW_STAGE_MEASURE_POS_X = 0,
		eOPTION_ROW_STAGE_MEASURE_POS_Y,
		eOPTION_ROW_CAMERA_MEASURE_POS_X,
		eOPTION_ROW_CAMERA_MEASURE_POS_Z,
		eOPTION_ROW_GRAB_WIDTH,
		eOPTION_ROW_GRAB_HEIGHT,
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

	enum EN_GRID_RESULT
	{
		eRESULT_COL_NAME = 0,
		eRESULT_COL_VALUE,
		eRESULT_COL_UNIT,
		eRESULT_COL_MAX,

		eRESULT_ROW_ANGLE = 0,
		eRESULT_ROW_PIXEL,
		eRESULT_ROW_SCORE,
		eRESULT_ROW_SCALE,
		eRESULT_PARAM_MAX,
		eRESULT_ROW_MARK_OFFSET_X = eRESULT_PARAM_MAX,
		eRESULT_ROW_MARK_OFFSET_Y,
		eRESULT_ROW_MAX,
	};

	enum EN_CAMERA_SPEC_DEFINES
	{
		DEF_CAMERA_COUNT = 2,
		DEF_DEFAULT_GRID_ROW_SIZE = 40,
	};

	/* 구조체 변수 */
protected:
#pragma pack (push, 8)
	typedef struct __st_result_angle_pixel_value__
	{
		double			dValue[eRESULT_PARAM_MAX];

	}	STM_RAPV, *LPG_RAPV;
#pragma pack (pop)

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

	CMacButton			m_btn_ctl[eCALB_CAMERA_SPEC_BTN_MAX];
	CMyStatic			m_stt_ctl[eCALB_CAMERA_SPEC_STT_MAX];
	CMyStatic			m_txt_ctl[eCALB_CAMERA_SPEC_TXT_MAX];
	CMyStatic			m_pic_ctl[eCALB_CAMERA_SPEC_PIC_MAX];
	CProgressCtrlX		m_pgr_ctl[eCALB_CAMERA_SPEC_PGR_MAX];
	CGridCtrl			m_grd_ctl[eCALB_CAMERA_SPEC_GRD_MAX];

	/* 로컬 함수 */
protected:

	VOID				InitCtrl();

	VOID				InitGridSelectCam();
	VOID				InitGridReslut();
	VOID				InitGridSelectMode();
	VOID				InitGridRepeatCount();
	VOID				InitGridTriggerOption();
	VOID				InitGridOption();
	VOID				InitGridCameraOption();
	VOID				InitGridPosition();

	VOID				InitDispMark();
	VOID				DispResize(CWnd* pWnd);

	VOID				InvalidateView();
	VOID				ResetResult();

	VOID				ViewControls(BOOL bView = TRUE);
	VOID				SetLiveView(BOOL bLive = TRUE);

	VOID				LoadDataConfig();
	VOID				SaveDataConfig();
	VOID				SaveResult();

	VOID				MovePosMeasure(UINT8 u8SelHead);
	VOID				MarkGrabbedResult();

	VOID				MeasureStart();
	VOID				MeasureStop();

	VOID				UpdateCaliResult();
	VOID				CalcNormalDist(double dRate = 2.0f);

	BOOL				PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts = 0);

	VOID				UpdateCtrlACam();
	VOID				UpdateLiveView();

	VOID				DoEvents();
	VOID				Wait_(DWORD dwTime);

	/* 공용 함수 */
public:

	UINT8				GetCheckACam() { return m_u8SelHead; }
	UINT8				GetTrigChNo();

	/* 내부 변수 */
private:

	BOOL				m_bValidResult;
	BOOL				m_bViewCtrls;
	BOOL				m_bCamLiveState;
	BOOL				m_bEditMode;
	UINT8				m_u8SelHead;


	/* 측정 결과 계산을 위한 임시 버퍼 */
	std::vector<STM_RAPV>	m_stVctResult;		/* 얼라인 카메라 설치 각도 */

protected:
	LRESULT UpdateMeasureProgress(WPARAM wParam, LPARAM lParam);
	LRESULT UpdateCaliResult(WPARAM wParam, LPARAM lParam);
	LRESULT MeasureComplete(WPARAM wParam, LPARAM lParam);
	LRESULT RefreshGrabView(WPARAM wParam, LPARAM lParam);

	/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClick(UINT32 id);
	afx_msg	void		OnClickGridSelectCam(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridRepeatCount(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridResult(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridSelectMode(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridTriggerOption(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridOption(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridCameraOption(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridPosition(NMHDR* pNotifyStruct, LRESULT* pResult);
	//afx_msg LRESULT		OnMsgThread(WPARAM wparam, LPARAM lparam);
};
