
#pragma once

#include "../../../inc/comn/MyDialog.h"
#include "DlgMenu.h"

// by sys&j : 가독성을 위해 enum 추가
enum EnMmpmGrp
{
	eMMPM_GRP_STEP	,
	eMMPM_GRP_SEARCH,
	eMMPM_GRP_MAX
};

enum EnMmpmPic
{
	eMMPM_PIC_VIEW,
	eMMPM_PIC_MAX
};

enum EnMmpmChk
{
	eMMPM_CHK_STEP_01 ,
	eMMPM_CHK_STEP_02 ,
	eMMPM_CHK_STEP_05 ,
	eMMPM_CHK_STEP_10 ,
	eMMPM_CHK_MAX
};

enum EnMmpmBtn
{
	eMMPM_BTN_HORZ_LEFT		,
	eMMPM_BTN_HORZ_RIGHT		,
	eMMPM_BTN_VERT_UP		,
	eMMPM_BTN_VERT_DOWN		,
	eMMPM_BTN_UNDO			,
	eMMPM_BTN_PREV			,
	eMMPM_BTN_NEXT			,
	eMMPM_BTN_CHECK			,
	eMMPM_BTN_APPLY			,
	eMMPM_BTN_CANCEL			,
	eMMPM_BTN_MAX
};

//class CDPoint
//{
//public:
//	double x, y;
//	CDPoint()
//	{
//		x = y = 0.0;
//	}
//};


class CDlgMmpm : public CMyDialog
{
public:

	CDlgMmpm(CWnd* parent = NULL);
	enum { IDD = IDD_MMPM };

/* 가상 함수 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel() {}

public:

	virtual VOID		UpdatePeriod()	{};

/* 로컬 변수 */
protected:

	UINT8				m_u8Index;			/* 현재 Grab.Failed 위치 (Zero based) */
	UINT8				m_u8LeftClick;		/* 마우스 좌측 버튼 클릭 Down 했는지 여부 */

	HCURSOR				m_hPrvCursor, m_hNowCursor;	/* 마우스 커서 핸들 */

	DOUBLE				m_dbStep;			/* 키보드나 버튼에 이동할 때, 이동 간격 (Pixel) */
	HDC					m_hDCDraw;			/* 현재 Align Mark 출력을 위한 영역 DC 핸들 */
	RECT				m_rAreaDraw;		/* 현재 Align Mark 출력을 위한 윈도 영역 */

	CPoint				m_ptClickDown;		/* 마우스 왼쪽 버튼 클릭한 위치 저장 */
	CRect				um_rectArea;

	CMyGrpBox			m_grp_ctl[eMMPM_GRP_MAX];
	CMacButton			m_btn_ctl[eMMPM_BTN_MAX];		/* Normal */
	CMacCheckBox		m_chk_ctl[eMMPM_CHK_MAX];
	CMyStatic			m_pic_ctl[eMMPM_PIC_MAX];		/* Mark Drawing Area */

	/* 현재 선택된 정보 저장 */
	LPG_ACGR			m_pstGrab;

	CAtlList <LPG_ACGR>	m_lstFind;			/* Find.Failed한 정보만 저장 */
	CAtlList <LPG_ACGR>	m_lstGrab;			/* Grab.Failed한 정보만 저장 */

	CDPoint				m_pt3Point_Img[3];		/* 3점 영역 설정, 이미지 좌표 */
	CDPoint				m_ptCenter; // 좌상단(0,0)기준으로 좌표 설정
/* 로컬 함수 */
protected:

	BOOL				InitCtrl();
	BOOL				GetMarkData(UINT8 type);

	VOID				FindData(UINT8 direct);

	VOID				UndoCenter();
	VOID				MoveCenter(UINT8 type);

	VOID				WorkApply();
	VOID				WorkCancel();
	VOID				KeyUpDown(UINT16 key);

	VOID				Restore();

	VOID				InitDispMark();
	VOID				DispResize(CWnd* pWnd);
	BOOL				ZoomFlag;
	BOOL				OldZoomFlag;
	BOOL				um_bMoveFlag;
	void				MenuZoomIn();
	void				MenuZoomOut();
	void				MenuZoomFit();
	int					menuPart; // menu 선택, 1:zoom, 2,3,4:3point set, 5:measure, 99 : 아무것도 선택안된 상태
	double				tgt_rate; // 비율

/* 공용 함수 */
public:

	INT_PTR				MyDoModal();

/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
	afx_msg VOID		OnChkClicked(UINT32 id);
	afx_msg VOID		OnLButtonDown(UINT flags, CPoint point);
	afx_msg VOID		OnLButtonUp(UINT flags, CPoint point);
	afx_msg VOID		OnMouseMove(UINT flags, CPoint point);
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};
