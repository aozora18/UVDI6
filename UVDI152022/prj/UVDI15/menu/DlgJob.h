
#pragma once

#include "DlgMenu.h"
#include "../param/RecipeManager.h"
#define DEF_DEFAULT_GRID_ROW_SIZE		30

class CDrawPrev;	/* Gerber Preivew File */
class CDrawModel;

// by sys&j : 가독성을 위해 enum 추가
enum EnJobGrp
{
	eJOB_GRP_JOB_PREVIEW		,
	eJOB_GRP_MAX				,
};

enum EnJobGrd
{
	eJOB_GRD_RECIPE_LIST		,
	eJOB_GRD_JOB_PARAMETER		,
	eJOB_GRD_EXPOSE_PARAMETER	,
	eJOB_GRD_ALIGN_PARAMETER,
	eJOB_GRD_MAX				,
};

enum EnJobTxt
{
	eJOB_TXT_RECIPE_LIST		,
	eJOB_TXT_JOB_PARAMETER		,
	eJOB_TXT_EXPOSE_PARAMETER	,
	eJOB_TXT_ALIGN_PARAMETER	,
	eJOB_TXT_ALIGN_MARK_GLOBAL	,
	eJOB_TXT_ALIGN_MARK_LOCAL	,
	eJOB_TXT_GERBER_PREVIEW		,
	eJOB_TXT_MAX				,
};

enum EnJobBtn
{
	eJOB_BTN_RECIPE_ADD			,
	eJOB_BTN_RECIPE_DELETE		,
	eJOB_BTN_RECIPE_SELECT		,
	eJOB_BTN_RECIPE_SAVE		,
	eJOB_BTN_MAX				,
};

enum EnJobPic
{
	eJOB_PIC_MARK_GLOBAL,
	eJOB_PIC_MARK_LOCAL,
	eJOB_PIC_MAX,
};

enum EnJobGrdColList
{
	eJOB_GRD_COL_RECIPE_LIST_NO,
	eJOB_GRD_COL_RECIPE_LIST_NAME,
	eJOB_GRD_COL_RECIPE_LIST_MAX
};

enum EnJobGrdColParam
{
	eJOB_GRD_COL_PARAMETER_ID,
	eJOB_GRD_COL_PARAMETER_NAME,
	eJOB_GRD_COL_PARAMETER_VALUE,
	eJOB_GRD_COL_PARAMETER_UNIT,
	eJOB_GRD_COL_PARAMETER_MAX
};

class CDlgJob : public CDlgMenu
{
public:

	/* 생성자 / 파괴자 */
	CDlgJob(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgJob();


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

	CMyStatic			m_grp_ctl[eJOB_GRP_MAX];
	CMacButton			m_btn_ctl[eJOB_BTN_MAX];
	CMyStatic			m_txt_ctl[eJOB_TXT_MAX];
	CGridCtrl			m_grd_ctl[eJOB_GRD_MAX];
	//CMyStatic			m_pic_img[eJOB_PIC_MAX];

	CDrawPrev			*m_pDrawPrev;	/* Gerber Preivew File */

	BOOL				m_bEnableEdit;		
	BOOL				m_bIsEditing;		
	CArrGrdPage			m_arrGrdPage[EN_RECIPE_TAB::_size()];
	INT32				m_i32DistS[6];

	int					m_nSelectRecipe[eRECIPE_MODE_MAX];
	int					m_nSelectRecipeOld[eRECIPE_MODE_MAX];

	CDrawModel**		m_pDrawModel;			/* Mark Model Object */
/* 로컬 함수 */
protected:

	VOID				InitCtrl();

	BOOL				InitObject();
	VOID				CloseObject();

	BOOL				InitGrid();
	void				DeleteGridParam(int nRecipeTab);
	void				InitGridParam(int nRecipeTab);
	void				UpdateGridParam(int nRecipeTab);
	BOOL				InitGridRecipeList(BOOL bUpdate = TRUE);

	VOID				CloseGrid();

	VOID				UpdateRecipe(int nSelectRecipe);

	PTCHAR				SelectGerber();
	VOID				RecipeControl(UINT8 mode);
	VOID				RecipeSelect();
	VOID				SetBaseData();
	VOID				ResetMarkInfo();
	VOID				CalcMarkDist();
	VOID				MarkRateMarkDist();

	void				SelectRecipe(int nSelect, EN_RECIPE_MODE eRecipeMode = eRECIPE_MODE_SEL);
	void				SetEditing(BOOL bEditing);
	BOOL				CheckEditing();
	BOOL				EnableEdit();

	VOID				SetMark(UINT8 index);
	VOID				InitDispMark();

	BOOL				CheckModifyedParam(ST_RECIPE_PARAM stParam, CString strValueOld);
	void				ChangedParamColorModify(int nRecipeTab, std::vector <int> vChangeList);
	std::vector <int> m_vChangeList[EN_RECIPE_TAB::ALIGN + 1];
	/* 공용 함수 */
public:

	

/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
 	afx_msg VOID		OnBtnClick(UINT32 id);
	afx_msg	void		OnClickGridRecipeList(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridJobParameter(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridExposeParameter(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridAlignParameter(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridJobParameterChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridExposeParameterChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnClickGridAlignParameterChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
};
