#pragma once
#include "../../DlgSubTab.h"
#include "../DlgCalbUVPower.h"

#define DEF_DEFAULT_GRID_ROW_SIZE		40
#define DEF_DEFAULT_AUTO_READ_TIME		60 * 1000
#define DEF_DEFAULT_INTERLOCK_TIMEOUT	60 * 1000

enum EnCalbUVPowerCheckBtn
{
	eCALB_UVPOWER_CHECK_BTN_SAVE,
	eCALB_UVPOWER_CHECK_BTN_START,
	eCALB_UVPOWER_CHECK_BTN_STOP,
	eCALB_UVPOWER_CHECK_BTN_OPEN_CTRL_PANEL,
	eCALB_UVPOWER_CHECK_BTN_SELECT_RECIPE,
	eCALB_UVPOWER_CHECK_BTN_MAKE_RECIPE,
	eCALB_UVPOWER_CHECK_BTN_ALL_HEAD_OFF,
	eCALB_UVPOWER_CHECK_BTN_MAKE_MAX_RECIPE,
	eCALB_UVPOWER_CHECK_BTN_MAX,
};

enum EnCalbUVPowerCheckStt
{
	eCALB_UVPOWER_CHECK_STT_OPERATION,
	eCALB_UVPOWER_CHECK_STT_RESULT,
	eCALB_UVPOWER_CHECK_STT_POSITION,
	eCALB_UVPOWER_CHECK_STT_MAX,
};

enum EnCalbUVPowerCheckTxt
{
	eCALB_UVPOWER_CHECK_TXT_RECIPE_NAME,
	eCALB_UVPOWER_CHECK_TXT_MAX,
};

enum EnCalbUVPowerCheckGrd
{
	eCALB_UVPOWER_CHECK_GRD_POSITION,
	eCALB_UVPOWER_CHECK_GRD_RESULT,
	eCALB_UVPOWER_CHECK_GRD_MAX,
};

enum EnCalbUVPowerCheckTimer
{
	eCALB_UVPOWER_CHECK_TIMER_START = 1,
	eCALB_UVPOWER_CHECK_TIMER_MOVING,
	eCALB_UVPOWER_CHECK_TIMER_MAX = eCALB_UVPOWER_CHECK_TIMER_MOVING,
};

// CDlgCalbUVPowerCheck 대화 상자

class CDlgCalbUVPowerCheck : public CDlgSubTab
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

	enum EN_MSG_BOX_TYPE
	{
		eINFO,
		eSTOP,
		eWARN,
		eQUEST,
	};

	enum CALIMODE
	{
		MATCH_PH, //포토헤드간 가장 낮은 출력 램프기준
		MATCH_LAMP, //포토헤드의 각 램프간 출력기준.

	};

	CALIMODE			caliMode = CALIMODE::MATCH_PH; //캘리브레이션 모드

	/* 생성자 / 파괴자 */
	CDlgCalbUVPowerCheck(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgCalbUVPowerCheck();


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

	CMacButton			m_btn_ctl[eCALB_UVPOWER_CHECK_BTN_MAX];
	CMyStatic			m_stt_ctl[eCALB_UVPOWER_CHECK_STT_MAX];
	CMyStatic			m_txt_ctl[eCALB_UVPOWER_CHECK_TXT_MAX];
	CGridCtrl			m_grd_ctl[eCALB_UVPOWER_CHECK_GRD_MAX];

	/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	VOID				InitGridPosition(int nHeadCount = eHEAD_MAX);
	VOID				UpdateGridPosition();
	VOID				InitGridResult();

	VOID				AttachButton();

	BOOL				InitObject();
	VOID				CloseObject();

	VOID				SelectRecipe();
	VOID				MakeRecipe();
	VOID				MakeMaxRecipe();

	VOID				UpdateRecipeData();
	VOID				CalculatePowerEachPH(STG_PLPI& stData, double dRate); //포토헤드간 출력으로 조정 
	VOID				CalculatePowerEachLED(STG_PLPI& stData, double dRate); //포토헤드 led간 출력으로 조정

	COLORREF			CompareParam(double dParam1, double dParam2);

	BOOL				PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts = 0);
	int					ShowMultiSelectMsg(EN_MSG_BOX_TYPE mType, CString strTitle, CStringArray& strArrCommand);


	// 20230823 mhbaek Add : File Mgr
	BOOL				CheckFileExists(LPCTSTR szFileName);
	VOID				CreateDir(LPCTSTR Path);
	VOID				WriteFile(CString strName);


	/* 공용 함수 */
public:
	void				CheckStart();
	void				CheckStop();
	void				ShowCtrlPanel();
	void				SaveParam();

private:
	unsigned long long	m_ullTimeHeadMove;

	int					m_nSelectedMove;
	int					m_nSelectedRecipe;

	int					m_nMeasureHead;
	int					m_nMeasureLed;
	BOOL				m_bMeasure;

	BOOL				m_bManualPosMoving;
	BOOL				m_bBlink[eCALB_UVPOWER_CHECK_TIMER_MAX];

protected:
	LRESULT InputSetPoint(WPARAM wParam, LPARAM lParam);
	LRESULT InputMeasureCompl(WPARAM wParam, LPARAM lParam);
	LRESULT ReportResult(WPARAM wParam, LPARAM lParam);

	/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClick(UINT32 id);
	afx_msg	void		OnClickGridPosition(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridResult(NMHDR* pNotifyStruct, LRESULT* pResult);

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
