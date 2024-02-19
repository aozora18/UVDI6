#pragma once
#include "../../DlgSubTab.h"
#include "../DlgCalbUVPower.h"

#define DEF_DEFAULT_GRID_ROW_SIZE		40
#define DEF_DEFAULT_AUTO_READ_TIME		60 * 1000
#define DEF_DEFAULT_INTERLOCK_TIMEOUT	60 * 1000

enum EnCalbUVPowerMeasureBtn
{
	eCALB_UVPOWER_MEASURE_BTN_SAVE				,
	eCALB_UVPOWER_MEASURE_BTN_START				,
	eCALB_UVPOWER_MEASURE_BTN_STOP				,
	eCALB_UVPOWER_MEASURE_BTN_OPEN_CTRL_PANEL	,
	eCALB_UVPOWER_MEASURE_BTN_MANUAL_ON			,
	eCALB_UVPOWER_MEASURE_BTN_MANUAL_OFF			,
	eCALB_UVPOWER_MEASURE_BTN_VIEW_OPTION,
	eCALB_UVPOWER_MEASURE_BTN_VIEW_SAVE_RESULT,
	eCALB_UVPOWER_MEASURE_BTN_MAX,				
};

enum EnCalbUVPowerMeasureStt
{
	eCALB_UVPOWER_MEASURE_STT_DATA			 ,
	eCALB_UVPOWER_MEASURE_STT_OPERATION		 ,
	eCALB_UVPOWER_MEASURE_STT_SETTING		 ,
	eCALB_UVPOWER_MEASURE_STT_SEL_HEAD		 ,
	eCALB_UVPOWER_MEASURE_STT_POSITION		 ,
	eCALB_UVPOWER_MEASURE_STT_MANUAL_MEASURE	 ,
	eCALB_UVPOWER_MEASURE_STT_MAX,
};

enum EnCalbUVPowerMeasureTxt
{
	eCALB_UVPOWER_MEASURE_TXT_STATE,
	eCALB_UVPOWER_MEASURE_TXT_MAX,
};

enum EnCalbUVPowerMeasurePrg
{
	eCALB_UVPOWER_MEASURE_PGR_RATE,
	eCALB_UVPOWER_MEASURE_PGR_MAX,
};

enum EnCalbUVPowerMeasureGrd
{
	eCALB_UVPOWER_MEASURE_GRD_DATA				,
	eCALB_UVPOWER_MEASURE_GRD_SETTING			,
	eCALB_UVPOWER_MEASURE_GRD_SEL_HEAD			,
	eCALB_UVPOWER_MEASURE_GRD_POSITION			,
	eCALB_UVPOWER_MEASURE_GRD_MANUAL				,
	eCALB_UVPOWER_MEASURE_GRD_MANUAL_AUTOREAD	,
	eCALB_UVPOWER_MEASURE_GRD_MAX				,
};

enum EnCalbUVPowerMeasureCmb
{
	eCALB_UVPOWER_MEASURE_CMB_WAVELENGTH,
	eCALB_UVPOWER_MEASURE_CMB_MAX,
};

enum EnCalbUVPowerMeasureTimer
{
	eCALB_UVPOWER_MEASURE_TIMER_START = 1,
	eCALB_UVPOWER_MEASURE_TIMER_AUTOREAD,
	eCALB_UVPOWER_MEASURE_TIMER_UV_ON,
	eCALB_UVPOWER_MEASURE_TIMER_MOVING,
	eCALB_UVPOWER_MEASURE_TIMER_MAX = eCALB_UVPOWER_MEASURE_TIMER_MOVING,
};

// CDlgCalbUVPowerMeasure 대화 상자

class CDlgCalbUVPowerMeasure : public CDlgSubTab
{
public:
	enum GRD_POSITION
	{
		eGRD_ROW_POSITION_TITLE = 0,

		eGRD_COL_POSITION_HEAD = 0,
		eGRD_COL_POSITION_POS_X,
		eGRD_COL_POSITION_POS_Y,
		eGRD_COL_POSITION_POS_Z,
		eGRD_COL_POSITION_READ,
		eGRD_COL_POSITION_MOVE,
		eGRD_COL_POSITION_MAX
	};
	enum GRD_MANUAL
	{
		eGRD_ROW_MANUAL_BEGIN_POWER_INDEX = 0,
		eGRD_ROW_MANUAL_MEASURE_COUNT,
		eGRD_ROW_MANUAL_INTERVAL_POWER,
		eGRD_ROW_MANUAL_MAX_POINTS,
		eGRD_ROW_MANUAL_PITCH_POS,
		eGRD_ROW_MANUAL_FOCUS_SCAN_POS,
		eGRD_ROW_MANUAL_STAGE_SPEED,
		eGRD_ROW_MANUAL_HEAD_SPEED,
		eGRD_ROW_MANUAL_POWER_INDEX,
		eGRD_ROW_MANUAL_MAX,

		eGRD_COL_MANUAL_NAME = 0,
		eGRD_COL_MANUAL_PARAM,
		eGRD_COL_MANUAL_UNIT,
		eGRD_COL_MANUAL_MAX
	};
	enum EN_GRID_SETTING
	{
		eSETTING_COL_NAME = 0,
		eSETTING_COL_VALUE,
		eSETTING_COL_UNIT,
		eSETTING_COL_MAX,

		eSETTING_ROW_MAX_ILL = 0,
		eSETTING_ROW_ALL_WAVE,
		eSETTING_ROW_ALL_PH,
		eSETTING_ROW_MAX,
	};

	/* 생성자 / 파괴자 */
	CDlgCalbUVPowerMeasure(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgCalbUVPowerMeasure();


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

	CMacButton			m_btn_ctl[eCALB_UVPOWER_MEASURE_BTN_MAX];
	CMyStatic			m_stt_ctl[eCALB_UVPOWER_MEASURE_STT_MAX];
	CMyStatic			m_txt_ctl[eCALB_UVPOWER_MEASURE_TXT_MAX];
	CProgressCtrlX		m_pgr_ctl[eCALB_UVPOWER_MEASURE_PGR_MAX];
	CGridCtrl			m_grd_ctl[eCALB_UVPOWER_MEASURE_GRD_MAX];
	CMyCombo			m_cmb_ctl[eCALB_UVPOWER_MEASURE_CMB_MAX];

	/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	VOID				InitGridData(VCT_LED_TABLE& pVctPowerIndex, BOOL bIsReload = FALSE);
	VOID				InitGridSetting();
	VOID				InitGridSelectHead(int nHeadCount = eHEAD_MAX);
	VOID				InitGridPosition(int nHeadCount = eHEAD_MAX);
	VOID				UpdateGridPosition();
	VOID				InitGridMeasureManualSet();
	VOID				UpdateGridMeasureManualSet();
	VOID				InitGridMeasureManualAutoRead();

	VOID				AttachButton();

	BOOL				InitObject();
	VOID				CloseObject();

	VOID				ViewControls(BOOL bView = TRUE);

	BOOL				PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts = 0);

	/* 공용 함수 */
public:
	void				MeasureStart();
	void				MeasureStop();
	void				ShowCtrlPanel();
	void				ManualUvOn();
	void				ManualUvOff();
	void				SaveParam();
	void				SaveResult();

private:
	int					m_nSelectedHead;
	int					m_nSelectedMove;
	unsigned long long	m_ullTimeAutoRead;
	unsigned long long	m_ullTimeManualON;
	unsigned long long	m_ullTimeHeadMove;

	BOOL				m_bManualPosMoving;
	BOOL				m_bAutoRead;
	BOOL				m_bViewCtrls;

	BOOL				m_bBlink[eCALB_UVPOWER_MEASURE_TIMER_MAX];

protected:
	LRESULT InputTableValue(WPARAM wParam, LPARAM lParam);
	LRESULT InputProgress(WPARAM wParam, LPARAM lParam);
	LRESULT InputXYPosition(WPARAM wParam, LPARAM lParam);
	LRESULT InputZPosition(WPARAM wParam, LPARAM lParam);

	/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClick(UINT32 id);
	afx_msg	void		OnClickGridSelectHead(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridSetting(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridPosition(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridManualIndex(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridAutoRead(NMHDR* pNotifyStruct, LRESULT* pResult);

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
