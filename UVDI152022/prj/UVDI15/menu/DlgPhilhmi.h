
#pragma once

#include "DlgMenu.h"
#include "../param/enum.h"
#include "../param/RecipeManager.h"
#include "./gerb/DrawPrev.h"

#define DEF_UI_PHILHMI_BTN IDC_PHILHMI_BTN_TEST
#define DEF_DEFAULT_GRID_ROW_SIZE		30

// by sys&j : 가독성을 위해 enum 추가
enum EnPhilhmiGrp
{
	ePHILHMI_GRP_PHIL_LOG,
	ePHILHMI_GRP_MAX,
};
enum EnPhilhmiBox
{
	ePHILHMI_BOX_PHIL_LOG,
	ePHILHMI_BOX_MAX,
};
enum EnPhilhmiGrd
{
	ePHILHMI_GRD_RECIPE_LIST,
	ePHILHMI_GRD_JOB_PARAMETER,
	ePHILHMI_GRD_PHIL_PARAMETER,
	ePHILHMI_GRD_MAX,
};
enum EnPhilhmiTxt
{
	ePHILHMI_TXT_RECIPE_LIST,
	ePHILHMI_TXT_JOB_PARAMETER,
	ePHILHMI_TXT_PHIL_PARAMETER,
	ePHILHMI_TXT_MAX,
};
enum EnPhilhmiBtn
{
	ePHILHMI_BTN_RECIPE_ADD,
	ePHILHMI_BTN_RECIPE_DELETE,
	ePHILHMI_BTN_RECIPE_SELECT,
	ePHILHMI_BTN_RECIPE_SAVE,
	ePHILHMI_BTN_IO_STATUS,
	ePHILHMI_BTN_STATUS_SEND,
	ePHILHMI_BTN_INTERR_STOP,
	ePHILHMI_BTN_LOG_CLEAR,
	ePHILHMI_BTN_MAX,
};
enum EnPhilhmiGrdColList
{
	ePHILHMI_GRD_COL_RECIPE_LIST_NO,
	ePHILHMI_GRD_COL_RECIPE_LIST_NAME,
	ePHILHMI_GRD_COL_RECIPE_LIST_MAX
};
enum EnPhilhmiGrdColParam
{
	ePHILHMI_GRD_COL_PARAMETER_ID,
	ePHILHMI_GRD_COL_PARAMETER_NAME,
	ePHILHMI_GRD_COL_PARAMETER_VALUE,
	ePHILHMI_GRD_COL_PARAMETER_UNIT,
	ePHILHMI_GRD_COL_PARAMETER_MAX
};

class CDlgPhilhmi : public CDlgMenu
{
	enum EN_PHILHMI_BTN
	{
		eBTN_TEST,
		eBTN_MAX,
	};
	enum EN_MSG_BOX_TYPE
	{
		eINFO,
		eSTOP,
		eWARN,
		eQUEST,
	};

	enum EN_PHILHMI_SET
	{
		eGRD_COL_PHIL_TITLE = 0,

		eGRD_ROW_OUTPUT_NUM = 0,
		eGRD_ROW_OUTPUT_ONOFF,
		eGRD_ROW_PROCESS_STEP,
		eGRD_ROW_PROCESS_COMP,
		eGRD_ROW_SUB_PROCESS_COMP,
		eGRD_ROW_EC_MODIFY,
		eGRD_ROW_INITIAL_COMP,
		eGRD_ROW_ALARM_OCCUR,
		eGRD_ROW_EVENT_NOTIFY,
		eGRD_ROW_INTERRUPT_STOP,
		eGRD_ROW_ABS_MOVE,
		eGRD_ROW_ABS_MOVE_COMP,
		eGRD_ROW_REL_MOVE,
		eGRD_ROW_REL_MOVE_COMP,
		eGRD_ROW_CHAR_MOVE,
		eGRD_ROW_CHAR_MOVE_COMP,

		eGRD_ROW_MAX
	};
public:

	/* 생성자 / 파괴자 */
	CDlgPhilhmi(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgPhilhmi();


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
	CMacButton			m_btn[eBTN_MAX];

	CMyGrpBox			m_grp_ctl[ePHILHMI_GRP_MAX];
	CMyListBox			m_box_ctl[ePHILHMI_BOX_MAX];
	CGridCtrl			m_grd_ctl[ePHILHMI_GRD_MAX];
	CMyStatic			m_txt_ctl[ePHILHMI_TXT_MAX];
	CMacButton			m_btn_ctl[ePHILHMI_BTN_MAX];

	int					m_nSelectRecipe[eRECIPE_MODE_MAX];
	int					m_nSelectRecipeOld[eRECIPE_MODE_MAX];
	BOOL				m_bEnableEdit;
	CArrGrdPage			m_arrGrdPage[EN_RECIPE_TAB::_size()];

/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	BOOL				InitGrid();
	void				DeleteGridParam(int nRecipeTab);

	BOOL				InitObject();
	VOID				CloseObject();
	BOOL				InitGridRecipeList(BOOL bUpdate = TRUE);
	void				InitGridParam(int nRecipeTab);
	void				InitGridMeasureManualSet();
	void				UpdateGridParam(int nRecipeTab);
	VOID				UpdateRecipe(int nSelectRecipe);

	PTCHAR				SelectGerber();
	void				SelectRecipe(int nSelect, EN_RECIPE_MODE eRecipeMode = eRECIPE_MODE_SEL);
	VOID				RecipeSelect();
	VOID				RecipeControl(UINT8 mode);
	VOID				IOStatus();
	VOID				InterruptStop();
	VOID				SendStatus();
	VOID				ClearListBox();

	

	// 공통으로 호출할 수 있는 클래스에 이동 필요
	int					ShowMsg(EN_MSG_BOX_TYPE mType, LPCTSTR lpszMsg, ...);
	int					ShowYesNoMsg(EN_MSG_BOX_TYPE mType, LPCTSTR lpszMsg, ...);
	int					ShowMultiSelectMsg(EN_MSG_BOX_TYPE mType, CString strTitle, CStringArray& strArrCommand);
	BOOL				PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts = 0);
	// 공통으로 호출할 수 있는 클래스에 이동 필요
/* 공용 함수 */
public:
	VOID				AddListBox(TCHAR* mesg);

/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	//afx_msg LRESULT		OnMsgMainPHILHMI(WPARAM wparam, LPARAM lparam);
	afx_msg	void		OnClickGridRecipeList(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridJobParameter(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridPhilParameter(NMHDR* pNotifyStruct, LRESULT* pResult);
 	afx_msg VOID		OnBtnClick(UINT32 id);
};
