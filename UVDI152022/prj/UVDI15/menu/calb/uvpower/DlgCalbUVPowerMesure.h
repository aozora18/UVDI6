#pragma once
#include "../../DlgSubTab.h"
#include "../DlgCalbUVPower.h"

#define DEF_DEFAULT_GRID_ROW_SIZE		40
#define DEF_DEFAULT_AUTO_READ_TIME		60 * 1000
#define DEF_DEFAULT_INTERLOCK_TIMEOUT	60 * 1000

enum EnCalbUVPowerMesureBtn
{
	eCALB_UVPOWER_MESURE_BTN_SAVE				,
	eCALB_UVPOWER_MESURE_BTN_START				,
	eCALB_UVPOWER_MESURE_BTN_STOP				,
	eCALB_UVPOWER_MESURE_BTN_OPEN_CTRL_PANEL	,
	eCALB_UVPOWER_MESURE_BTN_MANUAL_ON			,
	eCALB_UVPOWER_MESURE_BTN_MANUAL_OFF			,
	eCALB_UVPOWER_MESURE_BTN_VIEW_OPTION,
	eCALB_UVPOWER_MESURE_BTN_VIEW_SAVE_RESULT,
	eCALB_UVPOWER_MESURE_BTN_MAX,				
};

enum EnCalbUVPowerMesureStt
{
	eCALB_UVPOWER_MESURE_STT_DATA			 ,
	eCALB_UVPOWER_MESURE_STT_OPERATION		 ,
	eCALB_UVPOWER_MESURE_STT_SETTING		 ,
	eCALB_UVPOWER_MESURE_STT_SEL_HEAD		 ,
	eCALB_UVPOWER_MESURE_STT_POSITION		 ,
	eCALB_UVPOWER_MESURE_STT_MANUAL_MASURE	 ,
	eCALB_UVPOWER_MESURE_STT_MAX,
};

enum EnCalbUVPowerMesureTxt
{
	eCALB_UVPOWER_MESURE_TXT_STATE,
	eCALB_UVPOWER_MESURE_TXT_MAX,
};

enum EnCalbUVPowerMesurePrg
{
	eCALB_UVPOWER_MESURE_PGR_RATE,
	eCALB_UVPOWER_MESURE_PGR_MAX,
};

enum EnCalbUVPowerMesureGrd
{
	eCALB_UVPOWER_MESURE_GRD_DATA				,
	eCALB_UVPOWER_MESURE_GRD_SETTING			,
	eCALB_UVPOWER_MESURE_GRD_SEL_HEAD			,
	eCALB_UVPOWER_MESURE_GRD_POSITION			,
	eCALB_UVPOWER_MESURE_GRD_MANUAL				,
	eCALB_UVPOWER_MESURE_GRD_MANUAL_AUTOREAD	,
	eCALB_UVPOWER_MESURE_GRD_MAX				,
};

enum EnCalbUVPowerMesureCmb
{
	eCALB_UVPOWER_MESURE_CMB_WAVELENGTH,
	eCALB_UVPOWER_MESURE_CMB_MAX,
};

enum EnCalbUVPowerMesureTimer
{
	eCALB_UVPOWER_MESURE_TIMER_START = 1,
	eCALB_UVPOWER_MESURE_TIMER_AUTOREAD,
	eCALB_UVPOWER_MESURE_TIMER_UV_ON,
	eCALB_UVPOWER_MESURE_TIMER_MOVING,
	eCALB_UVPOWER_MESURE_TIMER_MAX = eCALB_UVPOWER_MESURE_TIMER_MOVING,
};

// CDlgCalbUVPowerMesure 대화 상자

class CDlgCalbUVPowerMesure : public CDlgSubTab
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

	/* 생성자 / 파괴자 */
	CDlgCalbUVPowerMesure(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgCalbUVPowerMesure();


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
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy);


	/* 로컬 변수 */
protected:

	CMacButton			m_btn_ctl[eCALB_UVPOWER_MESURE_BTN_MAX];
	CMyStatic			m_stt_ctl[eCALB_UVPOWER_MESURE_STT_MAX];
	CMyStatic			m_txt_ctl[eCALB_UVPOWER_MESURE_TXT_MAX];
	CProgressCtrlX		m_pgr_ctl[eCALB_UVPOWER_MESURE_PGR_MAX];
	CGridCtrl			m_grd_ctl[eCALB_UVPOWER_MESURE_GRD_MAX];
	CMyCombo			m_cmb_ctl[eCALB_UVPOWER_MESURE_CMB_MAX];

	/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	VOID				InitGridData(VCT_LED_TABLE& pVctPowerIndex, BOOL bIsReload = FALSE);
	VOID				InitGridSetting();
	VOID				InitGridSelectHead(int nHeadCount = eHEAD_MAX);
	VOID				InitGridPosition(int nHeadCount = eHEAD_MAX);
	VOID				UpdateGridPosition();
	VOID				InitGridMesureManualSet();
	VOID				UpdateGridMesureManualSet();
	VOID				InitGridMesureManualAutoRead();

	VOID				AttachButton();

	BOOL				InitObject();
	VOID				CloseObject();

	VOID				ViewControls(BOOL bView = TRUE);

	BOOL				PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts = 0);

	/* 공용 함수 */
public:
	void				MesureStart();
	void				MesureStop();
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

	BOOL				m_bBlink[eCALB_UVPOWER_MESURE_TIMER_MAX];

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
