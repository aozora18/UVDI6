
#pragma once

#include "DlgMenu.h"
#include "../param/enum.h"
#include "./gerb/DrawPrev.h"
#include "../param/IOManager.h"
#include "../pops/DlgMarkShow.h"

#define DEF_UI_AUTO_STT		 IDC_AUTO_STT_SEQ_STATUS
#define DEF_UI_AUTO_GRD		(IDC_AUTO_STT_SEQ_STATUS + 20)
#define DEF_UI_AUTO_BTN		(IDC_AUTO_STT_SEQ_STATUS + 40)
#define DEF_UI_AUTO_PRG		(IDC_AUTO_STT_SEQ_STATUS + 60)
#define DEF_UI_OFFSET			5
#define DEF_UI_GRD_COL_TITLE	160
#define DEF_UI_GRD_ROW_OFFSET	40

class CDrawPrev;	/* Gerber Preivew File */


JHMI_ENUM(EN_AUTO_STT, int
	, SEQUENCE_STATUS
	, PREVIEW
	, IO_STATUS
	, HEAD_PROCESS_INFORMATION
	, PRODUCT_INFORMATION
)

JHMI_ENUM(EN_AUTO_STT_SEQ, int
	, READY
	, LOAD
	, ALIGN
	, EXPOSE
	, UNLOAD
)

JHMI_ENUM(EN_GRD_PROCESS_COL, int
	, HEAD1
	, HEAD2
	, HEAD3
	, HEAD4
	, HEAD5
	, HEAD6
)


JHMI_ENUM(EN_GRD_PROCESS_ROW, int
	, AF_Z
	, POWER
	, TEMP_LED
	, ENERGE
	, SPEED
	, GERBER_NAME
)

JHMI_ENUM(EN_GRD_PRODUCT_ROW, int
	//, PRODUCT_ID
	, PRODUCT_COUNT
	, LAST_TACT_TIME
	, AVERAGE_TACT_TIME
	, REAL_SCALE
	, MARK_DIST_ERROR
)

class CDlgAuto : public CDlgMenu
{
	enum EN_AUTO_GRD
	{
		eGRD_PROCESS,
		eGRD_PRODUCT,
		eGRD_IO_QUICK1,
		eGRD_IO_QUICK2,
		eGRD_IO_QUICK3,
		eGRD_IO_QUICK4,
		eGRD_IO_QUICK5,
		eGRD_IO_QUICK6,
		eGRD_IO_QUICK7,
		eGRD_IO_QUICK8,
		eGRD_IO_QUICK9,
		eGRD_IO_QUICK10,
		eGRD_MAX
	};

	enum EN_AUTO_PRG
	{
		ePRG_PROCESS_STEP,
		ePRG_MAX
	};


public:

	/* 생성자 / 파괴자 */
	CDlgAuto(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgAuto();

	bool ProcessAction(UINT32 action)
	{
		switch (action)
		{
		case (UINT32)ENG_RIJA::clearMarkData:
		{
			m_pDrawPrev->ResetMarkResult();
			DrawMarkData(true);
		}
		break;

		default:break;
		}

		return true;
	}


/* 가상함수 선언 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		UpdateControl(UINT64 tick, BOOL is_busy);
	VOID				UpdateSttSeq(UINT64 tick, BOOL is_busy);


public:

	virtual BOOL		OnInitDlg();
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel() {}
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy);


/* 로컬 변수 */
protected:

	UINT64				m_u64ReqTime;	/* 통신 명령어 요청 시간 임시 저장 */
	UINT64				m_u64TickCount;
	CGridCtrl			*m_pGrd[eGRD_MAX];
	CMyStatic			*m_pStt[EN_AUTO_STT::_size()];
	CMyStatic			*m_pSttSeq[EN_AUTO_STT_SEQ::_size()];
	CProgressCtrlX		*m_pPgr;

	CDrawPrev*			m_pDrawPrev;	/* Gerber Preivew File */

	BOOL				m_bBlink;

	vector<ST_QUICK_IO_ACT>		m_vQuickIO2;
	vector<ST_IO>				m_vQuickIO;

	CDlgMarkShow				*m_pdlgMarkShow;

/* 로컬 함수 */
protected:

	VOID				InitCtrl();

	BOOL				InitObject();
	VOID				CloseObject();

	void				InitGridProcess();
	void				UpdateGridProcess();

	void				InitGridProduct();
	void				UpdateGridProduct();

	VOID				InitQuickIOGrid();
	VOID				UpdateQuickIOStatus();
	COLORREF			GetIOColorBg(CString strColor);
	COLORREF			GetIOColorFg(CString strColor);
	

/* 공용 함수 */
public:

	VOID				DrawMarkData(bool drawForce = false);	/* 마크 측정 결과를 화면에 출력 */

/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnLButtonDown(UINT32 nFlags, CPoint point);
};
