
#pragma once

#include "DlgMenu.h"

class CDrawModel;
class CGridMark;
class CGridModel;
class CGridModelInMark;
class CGridMiniMotor;


//class CDPoint
//{
//public:
//	double x, y;
//	CDPoint()
//	{
//		x = y = 0.0;
//	}
//};
// by sys&j : 가독성을 위해 enum 추가
enum EnMarkGrp
{
	eMARK_GRP_MODEL_INFO,
	eMARK_GRP_MODEL_LIST,
	eMARK_GRP_RECIPE_MODELS,
	eMARK_GRP_MODEL_1,
	eMARK_GRP_MODEL_TYPE,
	eMARK_GRP_MODEL_COLOR,
	eMARK_GRP_MODEL_SIZE,
	eMARK_GRP_SEARCH_RESULT,
	eMARK_GRP_ACAM_SELECT,
	eMARK_GRP_MARK_FIND,
	eMARK_GRP_EDGE_FIND,
	eMARK_GRP_CALIB,
	eMARK_GRP_MAX,
};
enum EnLampGrd
{
	eMARK_GRD_STROBE_VIEW,
	eMARK_GRD_MAX,
};

//enum EnMarkPic
//{
//	eMARK_PIC_MODEL_1  ,
//	eMARK_PIC_MODEL_2  ,
//	eMARK_PIC_MODEL_3  ,
//	eMARK_PIC_MODEL_4  ,
//	eMARK_PIC_FIND_GRAB,
//	eMARK_PIC_MAX,
//};

enum EnMarkTxt
{
	eMARK_TXT_SIZE_1		  ,
	eMARK_TXT_SIZE_2		  ,
	eMARK_TXT_SIZE_3		  ,
	eMARK_TXT_SIZE_4		  ,
	eMARK_TXT_CENT_ERR_X	  ,
	eMARK_TXT_CENT_ERR_Y	  ,
	eMARK_TXT_SCALE_RATE	  ,
	eMARK_TXT_SCORE_RATE	  ,
	eMARK_TXT_EDGE_WIDTH	  ,
	eMARK_TXT_EDGE_HEIGHT  ,
	eMARK_TXT_MODEL_NAME	  ,
	eMARK_TXT_GAIN_LEVEL1	,
	eMARK_TXT_GAIN_LEVEL2	,
	eMARK_TXT_GAIN_LEVEL3	,
	eMARK_TXT_CALIB_SUB,
	eMARK_TXT_MAX,
};

enum EnMarkEdtTxt
{
	eMARK_EDT_TXT_MODEL_NAME	 ,
	eMARK_EDT_TXT_MAX
};

enum EnMarkEdtCtl
{
	eMARK_EDT_CTL_MMF,
	eMARK_EDT_CTL_MAX
};

enum EnMarkTxtFloat
{
	eMARK_EDT_FLOAT_SIZE_1	,
	eMARK_EDT_FLOAT_SIZE_2	,
	eMARK_EDT_FLOAT_SIZE_3	,
	eMARK_EDT_FLOAT_SIZE_4	,
	eMARK_EDT_FLOAT_MAX
};


enum EnMarkEdtOut
{
	eMARK_EDT_OUT_CENT_ERR_X	,
	eMARK_EDT_OUT_CENT_ERR_Y	,
	eMARK_EDT_OUT_SCALE_RATE	,
	eMARK_EDT_OUT_SCORE_RATE	,
	eMARK_EDT_OUT_EDGE_WIDTH	,
	eMARK_EDT_OUT_EDGE_HEIGHT,
	eMARK_EDT_GAIN_LEVEL1,
	eMARK_EDT_GAIN_LEVEL2,
	eMARK_EDT_GAIN_LEVEL3,
	eMARK_EDT_CALIB_R,
	eMARK_EDT_CALIB_C,
	eMARK_EDT_OUT_MAX
};

enum EnMarkBtnCtl
{
	eMARK_BTN_CTL_MODEL_APPEND	,
	eMARK_BTN_CTL_MODEL_DELETE	,
	eMARK_BTN_CTL_MODEL_RESET	,
	eMARK_BTN_CTL_MARK_FIND		,
	eMARK_BTN_CTL_EDGE_FIND		,
	eMARK_BTN_CTL_TRIG_CH		,
	eMARK_BTN_CTL_FILE_LOAD		,
	eMARK_BTN_CTL_MARK_IMG		,
	eMARK_BTN_CTL_MARK_FIND_MODE	,
	eMARK_BTN_MARK_MERGE	,
	eMARK_BTN_CTL_MAX,
};

enum EnMarkBtnSet
{
	eMARK_BTN_SET_MARK_SET_11 ,
	eMARK_BTN_SET_MAX,
};

enum EnMarkBtnRst
{
	eMARK_BTN_RST_MARK_RESET_1 ,
	eMARK_BTN_CONTROL_PANEL,
	eMARK_BTN_GAIN_SET,
	eMARK_BTN_CALIB_SET,
	eMARK_BTN_CALIB,
	eMARK_BTN_STROBE_SET,
	eMARK_BTN_STROBE_GET,
	eMARK_BTN_RST_MAX,
};

enum EnMarkBtnMov
{
	eMARK_BTN_MOV_MAX,
};

enum EnMarkChkTyp
{
	eMARK_CHK_TYP_MODEL_CIRCLE	,
	eMARK_CHK_TYP_MODEL_RING	,
	eMARK_CHK_TYP_MODEL_CROSS	,
	eMARK_CHK_TYP_MODEL_RECTANGLE	,
	eMARK_CHK_TYP_MODEL_IMAGE	,
	eMARK_CHK_TYP_MAX
};

enum EnMarkChkProc
{
	eMARK_CHK_IMG_PROC,
	eMARK_CHK_IMG_MAX
};

enum EnMarkChkClr
{
	eMARK_CHK_CLR_BLACK  ,
	eMARK_CHK_CLR_WHITE  ,
	eMARK_CHK_CLR_ANY	   ,
	eMARK_CHK_CLR_MAX
};

enum EnMarkChkAxs
{
	eMARK_CHK_AXS_MAX
};

enum EnMarkChkMvt
{
	eMARK_CHK_MVT_MAX
};

enum EnMarkChkCam	
{
	eMARK_CHK_CAM_ACAM_1 ,
	eMARK_CHK_CAM_ACAM_2 ,
	eMARK_CHK_CAM_ACAM_3,
	eMARK_CHK_CAM_LIVE	 ,
	eMARK_CHK_CAM_MAX
};

// by sys&j : 가독성을 위해 enum 추가

class CDlgMark : public CDlgMenu
{
public:

	/* 생성자 / 파괴자 */
	CDlgMark(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgMark();


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
	virtual void		RegisterUILevel();
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy);


/* 로컬 변수 */
protected:

	UINT8				m_u8ViewMode;		/* 0x00 : Grab Mode, 0x01 : Live Mode */
	UINT8				m_u8lamp_type;		/*램프 조명 타입(0:Ring or 1 : Coaxial)*/

	CMyGrpBox			m_grp_ctl[eMARK_GRP_MAX];
	CGridCtrl			m_grd_ctl[eMARK_GRD_MAX];
	
	CMacButton			m_btn_ctl[eMARK_BTN_CTL_MAX];		/* Normal */
	CMacButton			m_btn_set[eMARK_BTN_SET_MAX];		/* Mark Set */
	CMacButton			m_btn_rst[eMARK_BTN_RST_MAX];		/* Mark Reset */
	//CMacButton			m_btn_mov[eMARK_BTN_MOV_MAX];		/* Motion Move */

	CMacCheckBox		m_chk_img[eMARK_CHK_IMG_MAX];		/* Img Processing 0:미사용, 1:사용 */
	CMacCheckBox		m_chk_typ[eMARK_CHK_TYP_MAX];		/* Model Type */
	CMacCheckBox		m_chk_clr[eMARK_CHK_CLR_MAX];		/* Model Color  0 : BLACK, 1 : WHITE, 2 : ANY*/ 
	//CMacCheckBox		m_chk_axs[eMARK_CHK_AXS_MAX];		/* Motion Stage, Align Camera */
	//CMacCheckBox		m_chk_mvt[eMARK_CHK_MVT_MAX];		/* Move Type (Absolute, Relative) */
	CMacCheckBox		m_chk_cam[eMARK_CHK_CAM_MAX];		/* Align Camera / Live View */

	CMyStatic			m_txt_ctl[eMARK_TXT_MAX];			/* Text - Normal */
//	CMyStatic			m_pic_img[eMARK_PIC_MAX];			/* Picture - Model & Mark (Grabbed) */

	CEditNum			m_edt_flt[eMARK_EDT_FLOAT_MAX];
	CEditNum			m_edt_out[eMARK_EDT_OUT_MAX];
	CEditTxt			m_edt_txt[eMARK_EDT_TXT_MAX];		/* Recipe Name, Model Name */
	CMyEdit				m_edt_ctl[eMARK_EDT_CTL_MAX];		/* MMF File */

	CDrawModel			**m_pDrawModel;			/* Mark Model Object */ 
	CGridMark			*m_pGridMark;			/* Grid Control */
	CGridModel			*m_pGridModel;			/* Grid Control */
	CGridModelInMark	*m_pGridModelInMark;	/* Grid Control */
	CGridMiniMotor		*m_pGridMotor;

	double tgt_rate;
	int mark_find_mode; // 0 : MOD, 1 : PAT
	bool bmmfFile;
	bool bpatFile;

/* 로컬 함수 */
protected:

	VOID				InitCtrl();
	VOID				InitDispMark();

	BOOL				InitObject();
	VOID				CloseObject();
	BOOL				InitGrid();
	VOID				CloseGrid();

	VOID				SetBaseData();

	VOID				MarkAppend(UINT8 mode);
	VOID				ModelAppend(UINT8 mode);
	VOID				RecipeReset(UINT8 mode);
	VOID				UpdateMark(LPG_RAAF recipe);
	VOID				UpdateModel(LPG_CMPV recipe);

	VOID				EnableModelSize();
	VOID				UpdateModelText();
	VOID				InitValue();

	ENG_MMDT			GetModelType();
	VOID				SetMark(UINT8 index);

	VOID				SetMotionMoving(ENG_MDMD direct);

	VOID				SelectMarkRecipe();
	VOID				SetMatchModel();
	VOID				SetEdgeDetect();
	VOID				RegistMarkImage();

	BOOL				IsSetValidMark();

	VOID				LoadTriggerCh();
	VOID				SetTriggerCh();
	VOID				SetLiveView();
	VOID				UpdateLiveView();
	VOID				InvalidateView();

	VOID				OpenSelectMARK();
	VOID				OutputFileMARK(PTCHAR file);
	void				ShowCtrlPanel();

	VOID				SetMarkFindMode(BOOL bChg);
	VOID				ProcImage();
	VOID				MarkMerge();

	CString				m_strLog;
	VOID				txtWrite(CString msg);

	VOID				setVisionCalib();
	VOID				VisionCalib();
	UINT8				CheckSelectCam();
	VOID				InitGridStrobeView();
	VOID				UpdataStrobeView();
	VOID				setStrobeValue();

/* 공용 함수 */
public:
	CPoint	ptMenu;
	CRect	um_rectArea; // Mark ROI Size
	bool	um_bMoveFlag;
	int		menuPart; // menu 선택, 0:imageload, 1:markroi, 2:searchroi, 3:measure, 4:zoom, 99 : 아무것도 선택안된 상태
	
	
	BOOL searchROI_ALL; // 전체 영역 : true, 일부 영역 : false
	int calib_row;
	int calib_col;
	int* CalibROI_left; // 동적할당
	int* CalibROI_right; // 동적할당
	int* CalibROI_top; // 동적할당
	int* CalibROI_bottom; // 동적할당

	BOOL* searchROI_CAM;
	BOOL* ZoomFlag;
/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnTypeClick(UINT32 id);
 	afx_msg VOID		OnBtnClick(UINT32 id);
 	afx_msg VOID		OnChkClick(UINT32 id);
	afx_msg VOID		OnGridMarkList(NMHDR *nm_hdr, LRESULT *result);
	afx_msg VOID		OnGridModelList(NMHDR *nm_hdr, LRESULT *result);
	afx_msg VOID		OnGridMarkModel(NMHDR *nm_hdr, LRESULT *result);
	afx_msg	void		OnClickGridInput(NMHDR* pNotifyStruct, LRESULT* pResult);
	BOOL				PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts = 0);
	
public:
	afx_msg void LoadImageFile();

	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	void DispResize(CWnd* pWnd);
	void MenuZoomIn();
	void MenuZoomOut();
	void MenuZoomFit();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	
	BOOL OldZoomFlag;
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
