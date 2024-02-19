
#pragma once

#include "DlgMenu.h"

class CGridRecipe;	/* Geber Recipe */
class CGridPowerI;	/* LED Item */
class CGridPowerV;	/* LED Value */
class CGridThick;	/* Material Thick Calibration for Align Camera */
class CDrawType;	/* Align Mark Array Type */
class CDrawPrev;	/* Gerber Preivew File */

// by sys&j : 가독성을 위해 enum 추가
enum EnGerbGrp
{
	eGERB_GRP_RECIPE_LIST		,
	eGERB_GRP_LED_LIST			,
	eGERB_GRP_SEARCH_RANGE		,
	eGERB_GRP_MARK_RUN_METHOD	,
	eGERB_GRP_GERBER_INFO		,
	eGERB_GRP_GERBER_PREVIEW	,
	eGERB_GRP_EXPO_PARAM		,
	eGERB_GRP_MARK_SEARCH_METHOD,
	eGERB_GRP_CALI_THICK		,
	eGERB_GRP_WORK_OPERATION	,
	eGERB_GRP_GERB_DECODE		,
	eGERB_GRP_MARK_DIST			,
	eGERB_GRP_DECODE_FLIP		,
	eGERB_GRP_ALIGN_MARK_TYPE	,
	eGERB_GRP_ACAM_GAIN			,
	eGERB_GRP_MAX				,
};

enum EnGerbTxt
{
	eGERB_TXT_SEARCH_SCORE	,
	eGERB_TXT_SEARCH_SCALE	,
	eGERB_TXT_RECIP_NAME		,
	eGERB_TXT_LOT_DATE		,
	eGERB_TXT_GERBER_FILE	,
	eGERB_TXT_EXPO_DUTY		,
	eGERB_TXT_EXPO_SPEED		,
	eGERB_TXT_EXPO_STEP		,
	eGERB_TXT_EXPO_THICK		,
	eGERB_TXT_MARK_COUNT		,
	eGERB_TXT_AREA_WIDTH		,
	eGERB_TXT_DECODE_SERIAL	,
	eGERB_TXT_DECODE_SCALE	,
	eGERB_TXT_DECODE_TEXT	,
	eGERB_TXT_AREA_HEIGHT	,
	eGERB_TXT_MARK_TOP_HORZ	,
	eGERB_TXT_MARK_BTM_HORZ	,
	eGERB_TXT_MARK_LEFT_VERT	,
	eGERB_TXT_MARK_RIGHT_VERT,
	eGERB_TXT_MARK_LEFT_DIAG	,
	eGERB_TXT_MARK_RIGHT_DIAG,
	eGERB_TXT_FLIP_SERIAL	,
	eGERB_TXT_FLIP_SCALE		,
	eGERB_TXT_FLIP_TEXT		,
	eGERB_TXT_TEXT_STRING	,
	eGERB_TXT_ACAM_GAIN_1	,
	eGERB_TXT_ACAM_GAIN_2	,
	eGERB_TXT_MAX,
};

enum EnGerbEdtCtl
{
	eGERB_EDT_CTL_GERBER_FILE,
	eGERB_EDT_CTL_MAX
};

enum EnGerbEdtSpd
{
	eGERB_EDT_SPD_EXPO_SPEED,
	eGERB_EDT_SPD_MAX
};

enum EnGerbEdtTxt
{
	eGERB_EDT_TXT_RECIPE_NAME	,
	eGERB_EDT_TXT_LOT_DATE		,
	eGERB_EDT_TXT_TEXT_STRING	,
	eGERB_EDT_TXT_MAX
};

enum EnGerbEdtInt
{
	eGERB_EDT_INT_EXPO_STEP			,
	eGERB_EDT_INT_EXPO_THICK		,
	eGERB_EDT_INT_AREA_WIDTH		,
	eGERB_EDT_INT_AREA_HEIGHT		,
	eGERB_EDT_INT_MARK_COUNT		,
	eGERB_EDT_INT_DECODE_SERIAL		,
	eGERB_EDT_INT_DECODE_SCALE		,
	eGERB_EDT_INT_DECODE_TEXT		,
	eGERB_EDT_INT_MARK_TOP_HORZ		,
	eGERB_EDT_INT_MARK_BTM_HORZ		,
	eGERB_EDT_INT_MARK_LEFT_VERT	,
	eGERB_EDT_INT_MARK_RIGHT_VERT	,
	eGERB_EDT_INT_MARK_LEFT_DIAG	,
	eGERB_EDT_INT_MARK_RIGHT_DIAG	,
	eGERB_EDT_INT_EXPO_DUTY			,
	eGERB_EDT_INT_ACAM_GAIN_1		,
	eGERB_EDT_INT_ACAM_GAIN_2		,
	eGERB_EDT_INT_MAX
};

enum EnGerbTxtFloat
{
	eGERB_EDT_FLOAT_SEARCH_SCALE		,
	eGERB_EDT_FLOAT_SEARCH_SCORE		,
	eGERB_EDT_FLOAT_MARK_DIST_OFFSET	,
	eGERB_EDT_FLOAT_MAX
};

enum EnGerbChk
{
	eGERB_CHK_MARK_ON_THE_FLY	,	
	eGERB_CHK_MARK_STEP_BY_STEP	,
	eGERB_CHK_MARK_TYPE_NORMAL	,
	eGERB_CHK_MARK_TYPE_MULTI	,
	eGERB_CHK_FLIP_SERIAL_HORZ	,
	eGERB_CHK_FLIP_SERIAL_VERT	,
	eGERB_CHK_FLIP_SCALE_HORZ	,
	eGERB_CHK_FLIP_SCALE_VERT	,
	eGERB_CHK_FLIP_TEXT_HORZ	,
	eGERB_CHK_FLIP_TEXT_VERT	,
	eGERB_CHK_MAX
};

enum EnGerbBtn
{
	eGERB_BTN_FILE_SELECT		,
	eGERB_BTN_MARK_ARRAY		,
	eGERB_BTN_RECIPE_SELECT		,
	eGERB_BTN_RECIPE_APPEND		,
	eGERB_BTN_RECIPE_DELETE		,
	eGERB_BTN_RECIPE_RESET		,
	eGERB_BTN_RECIPE_MODIFY		,
	eGERB_BTN_CALC_ENERGY		,
	eGERB_BTN_DIST_OFFSET		,
	eGERB_BTN_MAX,
};
// by sys&j : 가독성을 위해 enum 추가

class CDlgGerb : public CDlgMenu
{
public:

	/* 생성자 / 파괴자 */
	CDlgGerb(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgGerb();


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
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy);


/* 로컬 변수 */
protected:

	CMyGrpBox			m_grp_ctl[eGERB_GRP_MAX];
	CMacButton			m_btn_ctl[eGERB_BTN_MAX];
	CMacCheckBox		m_chk_ctl[eGERB_CHK_MAX];
	CMyStatic			m_txt_ctl[eGERB_TXT_MAX];
	CMyEdit				m_edt_ctl[eGERB_EDT_CTL_MAX];	/* Text (String) */
	CEditTxt			m_edt_txt[eGERB_EDT_TXT_MAX];	/* Text (String) */
	CEditNum			m_edt_int[eGERB_EDT_INT_MAX];	/* Number - Integer */
	CEditNum			m_edt_flt[eGERB_EDT_FLOAT_MAX];	/* Number - Float */
	CMyEdit				m_edt_spd[eGERB_EDT_SPD_MAX];	/* Frame Rate */

	CGridRecipe			*m_pGridRecipe;	/* Grid Control */
	CGridPowerI			*m_pGridPowerI;	/* Grid Control */
	CGridPowerV			*m_pGridPowerV;	/* Grid Control */
	CGridThick			*m_pGridThick;	/* Grid Control */
	CDrawType			*m_pDrawType;	/* Align Mark Type */
	CDrawPrev			*m_pDrawPrev;	/* Gerber Preivew File */


/* 로컬 함수 */
protected:

	VOID				InitCtrl();

	BOOL				InitObject();
	VOID				CloseObject();

	BOOL				InitGrid();
	VOID				CloseGrid();

	VOID				UpdateRecipe(LPG_RJAF recipe);
	VOID				UpdateMarkArray();

	VOID				SelectGerber();
	VOID				RecipeAppend(UINT8 mode);
	VOID				RecipeSelect();
	VOID				RecipeReset();
	VOID				SetBaseData();
	VOID				CalcEnergy();
	VOID				CalcMarkDist();
	VOID				ResetEnergySpeed();
	VOID				ResetMarkInfo();
	VOID				UpdateDistCalc();


/* 공용 함수 */
public:

	

/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
 	afx_msg VOID		OnBtnClick(UINT32 id);
	afx_msg VOID		OnEdtExpoSpeed();
 	afx_msg VOID		OnChkClick(UINT32 id);
	afx_msg VOID		OnGridGerbList(NMHDR *nm_hdr, LRESULT *result);
	afx_msg VOID		OnGridLedList(NMHDR *nm_hdr, LRESULT *result);
};
