
#pragma once

#include "DlgMenu.h"
#include "../param/enum.h"
#include "./gerb/DrawPrev.h"
#include "../pops/DlgMarkShow.h"

#define DEF_UI_MANUAL_STT		 IDC_MANUAL_STT_PREVIEW
#define DEF_UI_MANUAL_GRD		(IDC_MANUAL_STT_PREVIEW + 10)
#define DEF_UI_MANUAL_BTN		(IDC_MANUAL_STT_PREVIEW + 30)
#define DEF_UI_MANUAL_PRG		(IDC_MANUAL_STT_PREVIEW + 40)
#define DEF_UI_OFFSET			5
#define DEF_UI_GRD_ROW_OFFSET	40

class CDrawPrev;	/* Gerber Preivew File */


JHMI_ENUM(EN_MANUAL_BTN, int
	, DIRECT_EXPOSE
	, MARK
	, ALIGN_TEST
	, ALIGN_EXPOSE
	, RECIPE_LOAD
	, RECIPE_UNLOAD
	, STOP
	, CALIBRATION_MARK
	, MARK_ZERO
	, ENERGE
	, LOAD
	
)

JHMI_ENUM(EN_MANUAL_STT, int
	, PREVIEW
	, PROCESS
	, INFORMATION
	, PARAMETER
)

JHMI_ENUM(EN_GRD_INFORMATION_ROW, int
	, MARK_DIST_ERR
	, TACT_TIME
	, GERBER_NAME
	, REAL_SCALE
	, REAL_ROTATION
)

JHMI_ENUM(EN_GRD_PARAMETER_ROW, int
	, DUTY
	, SPEED 
	, STEP
	, THICKNESS
)

class CDlgManual : public CDlgMenu
{
	enum EN_MANUAL_GRD
	{
		eGRD_INFORMATION,
		eGRD_PARAMETER,
		eGRD_MAX
	};

	enum EN_MANUAL_PRG
	{
		ePRG_PROCESS_STEP,
		ePRG_MAX
	};

public:

	/* 생성자 / 파괴자 */
	CDlgManual(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgManual();


/* 가상함수 선언 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		UpdateControl(UINT64 tick, BOOL is_busy);
	VOID				UpdateBtn(UINT64 tick, BOOL is_busy);


public:

	virtual BOOL		OnInitDlg();
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel() {}
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy);


/* 로컬 변수 */
protected:

	UINT64				m_u64TickCount;
	UINT64				m_u64ReqTime;	/* 통신 명령어 요청 시간 임시 저장 */

	CGridCtrl			*m_pGrd[eGRD_MAX];
	CMacButton			*m_pBtn[EN_MANUAL_BTN::_size()];
	CMyStatic			*m_pStt[EN_MANUAL_STT::_size()];
	CProgressCtrlX		*m_pPgr;

	CDrawPrev*			m_pDrawPrev;	/* Gerber Preivew File */
	CDlgMarkShow*		m_pdlgMarkShow;
	BOOL				m_bBlink;

	STG_RJAF			m_stJob;

/* 로컬 함수 */
protected:

	VOID				InitCtrl();

	BOOL				InitObject();
	VOID				CloseObject();

	void				InitGridInformation();
	void				UpdateGridInformation();

	void				InitGridParameter();
	void				UpdateGridParameter();
	void				OnGrdParameter(NMHDR* pNotifyStruct);
	VOID				CalcEnergy();

	BOOL				IsCheckTransRecipe();

	VOID				RedrawMarkPage(UINT8 mode);
	VOID				MoveCheckMarkNo();

	VOID				RecipeLoad();
	VOID				RecipeUnload();
	VOID				ExposeDirect();
	VOID				ExposeAlign();
	VOID				TestAlign();
	VOID				OnlyFEMLoad();
	VOID				ResetData();
	BOOL				MarkZero();

/* 공용 함수 */
public:

	VOID				DrawMarkData();	/* 마크 측정 결과를 화면에 출력 */

/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
 	afx_msg VOID		OnBtnClick(UINT32 id);
	afx_msg void		OnGrdClick(UINT ID, NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg VOID		OnLButtonDown(UINT32 nFlags, CPoint point);
};
