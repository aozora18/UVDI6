#pragma once
#include "../../DlgSubTab.h"
#include "../DlgCalbExposure.h"

#include "../../../GlobalVariables.h"

#define  SIMUL_CAMERA 1

enum EnCalbExposureFlushBtn
{
	eCALB_EXPOSURE_FLUSH_BTN_PRINT,
	eCALB_EXPOSURE_FLUSH_BTN_MATCH_ONCE,
	eCALB_EXPOSURE_FLUSH_BTN_MEASURE,
	eCALB_EXPOSURE_FLUSH_BTN_VIEW_OPTION,
	eCALB_EXPOSURE_FLUSH_BTN_OPEN_CTRL_PANEL,
	eCALB_EXPOSURE_FLUSH_BTN_LIVE,
	eCALB_EXPOSURE_FLUSH_BTN_SAVE,
	eCALB_EXPOSURE_FLUSH_BTN_MOVE_MEASURE,
	eCALB_EXPOSURE_FLUSH_BTN_MOVE_UNLOAD,
	eCALB_EXPOSURE_FLUSH_BTN_SAVE_OPTION,
 	eCALB_EXPOSURE_FLUSH_BTN_MAX,				
};

enum EnCalbExposureFlushStt
{
	eCALB_EXPOSURE_FLUSH_STT_MARK1,
	eCALB_EXPOSURE_FLUSH_STT_MARK2,
	eCALB_EXPOSURE_FLUSH_STT_PH_STEP_X,
	eCALB_EXPOSURE_FLUSH_STT_PH_STEP_Y,
	eCALB_EXPOSURE_FLUSH_STT_RESULT,
	eCALB_EXPOSURE_FLUSH_STT_OPERATION,
	eCALB_EXPOSURE_FLUSH_STT_OPTION,
	eCALB_EXPOSURE_FLUSH_STT_MAX,
};

enum EnCalbExposureFlushTxt
{
	eCALB_EXPOSURE_FLUSH_TXT_STATE,
	eCALB_EXPOSURE_FLUSH_TXT_MAX,
};

enum EnCalbExposureFlushPic
{
	eCALB_EXPOSURE_FLUSH_PIC_MARK1,
	eCALB_EXPOSURE_FLUSH_PIC_MARK2,
	eCALB_EXPOSURE_FLUSH_PIC_MAX,
};

enum EnCalbExposureFlushSttMark
{
	eCALB_EXPOSURE_FLUSH_STT_MARK1_RESULT,
	eCALB_EXPOSURE_FLUSH_STT_MARK2_RESULT,
	eCALB_EXPOSURE_FLUSH_STT_MARK_MAX,
};

enum EnCalbExposureFlushPrg
{
	eCALB_EXPOSURE_FLUSH_PGR_RATE,
	eCALB_EXPOSURE_FLUSH_PGR_MAX,
};

enum EnCalbExposureFlushGrd
{
	eCALB_EXPOSURE_FLUSH_GRD_PH_STEP_X,
	eCALB_EXPOSURE_FLUSH_GRD_PH_STEP_Y,
	eCALB_EXPOSURE_FLUSH_GRD_RESULT,
	eCALB_EXPOSURE_FLUSH_GRD_RESULT_BTN,
	eCALB_EXPOSURE_FLUSH_GRD_OPTION,
	eCALB_EXPOSURE_FLUSH_GRD_MAX,
};

enum EnCalbExposureFlushTimer
{
	eCALB_EXPOSURE_FLUSH_TIMER_UPDATE,
 	eCALB_EXPOSURE_FLUSH_TIMER_MAX
};

// CDlgCalbExposureFlush 대화 상자

class CDlgCalbExposureFlush : public CDlgSubTab
{
public:

	/* 생성자 / 파괴자 */
	CDlgCalbExposureFlush(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgCalbExposureFlush();

	enum EN_EXPOSURE_FLUSH_DEFINES
	{
		DEF_MODEL_FIND_COUNT = 2,
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
		DEF_PH_MEASURE_COUNT = 4,		// Photohead 당 측정할 수 있는 구간 개수
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
		DEF_PH_MEASURE_COUNT = 27,		// Photohead 당 측정할 수 있는 구간 개수
#endif
		DEF_DEFAULT_GRID_ROW_SIZE = 40,	// 일반적인 grid 셀 높이
		DEF_DEFAULT_GRID_COL_SIZE = 80,	// 일반적인 grid 셀 너비
	};

	enum EN_MARK_NUM
	{
		eMARK_1,
		eMARK_2,
		eMARK_MAX,
	};

	enum EN_POS_NUM
	{
		ePOS_X,
		ePOS_Y,
		ePOS_2D_MAX,
		ePOS_Z = ePOS_2D_MAX,
		ePOS_3D_MAX,
	};

	enum EN_GRID_STEP
	{
		eSTEP_COL_TITLE = 0,
		eSTEP_ROW_TITLE = 0,
		eSTEP_ROW_PREV,
		eSTEP_ROW_THIS,
		eSTEP_ROW_SAVE,
		eSTEP_ROW_MAX,
	};

	enum EN_GRID_BTN
	{
		eGRD_BTN_ROW_FILE,
		eGRD_BTN_ROW_RESET,
		eGRD_BTN_ROW_CALC,
		eGRD_BTN_ROW_MAX,
	};

	enum EN_GRID_RESULT
	{
		eRESULT_COL_TITLE = 0,
		eRESULT_ROW_NO = 0,
		eRESULT_ROW_STEP_X,
		eRESULT_ROW_STEP_Y,
		eRESULT_ROW_LEFT_X,
		eRESULT_ROW_LEFT_Y,
		eRESULT_ROW_RIGHT_X,
		eRESULT_ROW_RIGHT_Y,
		eRESULT_ROW_MAX,
	};

	enum EN_GRID_OPTION
	{
		eOPTION_COL_NAME = 0,
		eOPTION_COL_VALUE,
		eOPTION_COL_UNIT,
		eOPTION_COL_MAX,

		eOPTION_ROW_STAGE_X = 0,
		eOPTION_ROW_STAGE_Y,
		eOPTION_ROW_CAM_X,
		eOPTION_ROW_FRAME_RATE,
		//eOPTION_ROW_SCROLL_MODE,
		//eOPTION_ROW_GRAB_WIDTH,
		//eOPTION_ROW_GRAB_HEIGHT,
		eOPTION_ROW_EXPOSE_ROUND,
		eOPTION_ROW_STAGE_Y_PLUS,
		eOPTION_ROW_MAX,
	};

	enum EN_EXPO_DIR
	{
		eDIR_FWD = 0,
		eDIR_BWD,
		eDIR_MAX,
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

	BOOL				m_bViewCtrls;
	BOOL				m_bCamLiveState;
	double				m_dLastACamPos;
	UINT8				m_u8MarkCount;

	CMacButton			m_btn_ctl[eCALB_EXPOSURE_FLUSH_BTN_MAX];
	CMyStatic			m_stt_ctl[eCALB_EXPOSURE_FLUSH_STT_MAX];
	CMyStatic			m_txt_ctl[eCALB_EXPOSURE_FLUSH_TXT_MAX];
	CMyStatic			m_pic_ctl[eCALB_EXPOSURE_FLUSH_PIC_MAX];
	CProgressCtrlX		m_pgr_ctl[eCALB_EXPOSURE_FLUSH_PGR_MAX];
	CGridCtrl			m_grd_ctl[eCALB_EXPOSURE_FLUSH_GRD_MAX];

	CMyStatic*			m_pSttResult[eCALB_EXPOSURE_FLUSH_STT_MARK_MAX];

	/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	VOID				InitGridPhStepX();
	VOID				InitGridPhStepY();
	VOID				InitGridResult();
	VOID				InitGridResultBtn();
	VOID				InitGridOption();

	VOID				InitDispMark();
	VOID				DispResize(CWnd* pWnd);

	VOID				RegStatic();
	VOID				InvalidateView();

	VOID				ResetGridData();
	VOID				SetLiveView(BOOL bLive = TRUE);

	VOID				DoPrint();
	VOID				MatchOnce();
	VOID				DoMeasure();

	VOID				UpdateMatchVal(int nStep);
	VOID				ViewControls(BOOL bView = TRUE);

	VOID				UpdateMatchGrid(int nStep, double dStepX, double dStepY, double dLeftX, double dLeftY, double dRightX, double dRightY);
	VOID				UpdateStepXY();
	VOID				UpdatePhOffset();
	VOID				UpdateLiveView();
	VOID				UpdateWorkProgress();

	VOID				LoadOption();
	VOID				SaveOption();
	VOID				SaveAsExcelFile();
	VOID				SetStepCalc();
	VOID				SavePhOffset();

	VOID				MoveStartPos();
	VOID				MoveUnloadPos();

	BOOL				OpenStepPopup(LPG_OSSD pstStep);
	VOID				SaveStepData(STG_OSSD stStep);

	BOOL				PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts = 0);

	/* 공용 함수 */
public:

	/* 내부 변수 */
private:

protected:
	LRESULT OnMsgResult(WPARAM wParam, LPARAM lParam);
	LRESULT ResultProgress(WPARAM wParam, LPARAM lParam);

	/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClick(UINT32 id);
	afx_msg	void		OnClickGridStep(UINT ID, NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridResult(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridBtn(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridOption(NMHDR* pNotifyStruct, LRESULT* pResult);

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 
// 
// 
//
// 
//
//
//								시간 없으니 합승좀 하겠습니다.
//
// 
// 
// 
//
// 
//
////////////////////////////////////////////////////////////////////////////////////////////////////////


class CDlgCalbExposureMirrorTune : public CDlgSubTab
{
public:

	/* 생성자 / 파괴자 */
	CDlgCalbExposureMirrorTune(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgCalbExposureMirrorTune();

	/* 가상함수 선언 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		UpdateControl(UINT64 tick, BOOL is_busy);
	CBrush brListBg;
	CBrush brEditBg;
	CBrush brPicBg;

	

	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	
	
public:

	virtual BOOL		OnInitDlg();
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC* dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel() {}
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy);
	BOOL PhotoLedOnOff(UINT8 head, UINT8 led, UINT16 index);
	BOOL PhotoImageLoad(UINT8 head, UINT8 imageNum);


	/* 로컬 변수 */
protected:
	std::vector<uint8_t> idsSnapshot; 
	int idsW = 0;
	int idsH = 0;


//#define IDC_BUTTON_MIRROR_LED_POWERSET      11771
//#define IDC_BUTTON_MIRROR_LOADIMAGE         11772
//#define IDC_BUTTON_MIRROR_UNLOADIMAGE       11773
//#define IDC_BUTTON_MIRROR_IDS_OPEN          11774
//#define IDC_BUTTON_MIRROR_IDS_CLOSE         11775
//#define IDC_BUTTON_MIRROR_IDS_SNAPSHOT      11776
//#define IDC_BUTTON_MIRROR_IDS_RECORD        11777
//#define IDC_BUTTON_MIRROR_OPEN_MOTION_CONTROL 11778
//#define IDC_BUTTON_MIRROR_EDIT_MOTIONLIST   11779
//#define IDC_BUTTON_MIRROR_RUN_MOTION        11780
	enum mirrorTuneBtns
	{
		LED_POWERSET,
		LOADIMAGE,
		UNLOADIMAGE,
		OPEN,
		CLOSE,
		SNAPSHOT,
		RECORD,
		OPEN_MOTION_CONTROL,
		EDIT_MOTIONLIST,
		RUN_MOTION,
		btnMax,
	};


//#define IDC_CHECK_MIRROR_LED1               11763
//#define IDC_CHECK_MIRROR_LED2               11764
//#define IDC_CHECK_MIRROR_LED3               11765
//#define IDC_CHECK_MIRROR_LED4               11766
//#define IDC_CHECK_MIRROR_KEEP_REFRESHING    11767
	enum mirrorTuneChecks
	{
		LED1,
		LED2,
		LED3,
		LED4,
		KEEP_REFRESHING,
		chkMax
	};


//#define IDC_COMBO_MIRROR_IMAGES             11768
//#define IDC_COMBO_MIRROR_PHINDEX            11769
	enum mirrorTuneCombos
	{
		IMAGES,
		PHINDEX,
		cmbMax,
	};

//#define IDC_STATIC_MIRROR_POWER_CURRENT     11762
	enum mirrorTuneStatic
	{
		POWER_CURRENT,
		stcMax
	};

//#define IDC_MIRROR_MOTION_LIST              11781
	enum mirrorTuneList
	{
		MOTION_LIST,
		lstMax,
	};

//#define IDC_MIRROR_MOTION_EDIT              11782
	enum mirrorTuneEdit
	{
		MOTION_EDIT,
		edtMax
	};

//#define IDC_IDSCAMERA_VIEW                  11783
	enum mirrorTunePic
	{
		IDSCAMERA_VIEW,
		picMax
	};

//#define IDC_GROUP_PH_SETTING                11784
//#define IDC_GROUP_LED						11785
//#define IDC_GROUP_CAMERA					11786
//#define IDC_GROUP_IMAGE						11787
//#define IDC_GROUP_MOTION					11788
	enum mirrorTurnGroups
	{
		PH_SETTING,
		LED,
		CAMERA,
		IMAGE,
		MOTION,
		grpMax
	};

//#define IDC_SLIDER_MIRROR_POWERINDEX        11770
	enum mirrorTuneSlide
	{
		POWERINDEX,
		sldMax
	};

	CEdit edits[edtMax];
	CListBox lists[lstMax];
	CSliderCtrl sliders[sldMax];
	CMacCheckBox checks[chkMax];
	CMacButton btns[btnMax];
	CComboBox combos[cmbMax];
	CMyStatic statics[stcMax];
	CMyStatic pics[picMax];
	CMyGrpBox groups[grpMax];
	/* 로컬 함수 */
	void UpdateIDSImage();

protected:
	void FixControlToPhysicalPixels(UINT ctrlId, int targetWpx, int targetHpx);
	VOID				InitCtrl();

	VOID				InvalidateView();

	/* 공용 함수 */
public:

	/* 내부 변수 */
private:

	/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnMirrorBtnClick(UINT nID);
	afx_msg void OnMirrorCheckClick(UINT nID);

	afx_msg void OnSelChangeMirrorImages();
	afx_msg void OnSelChangeMirrorPhIndex();
	afx_msg void OnSelChangeMirrorMotionList();

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	
	afx_msg void OnSliderMirrorPowerChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSliderMirrorPowerDraw(NMHDR* pNMHDR, LRESULT* pResult);

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
