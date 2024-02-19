#pragma once


// CDlgMMPMAutoCenter 대화 상자
#include "DlgMenu.h"  
#include <queue>

#define MARK_BUF_SIZE_X		500
#define MARK_BUF_SIZE_Y		500

class CDlgMMPMAutoCenter : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMMPMAutoCenter)

public:
	CDlgMMPMAutoCenter(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgMMPMAutoCenter();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MMPM_AUTOCENTER};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()


public:
	virtual BOOL OnInitDialog();
	//INT_PTR	MyDoModal(int cam_id, CString fi_filename);

	void UpdateInfo();
	afx_msg void OnBnClickedOk();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedCancel();
	VOID InitDisp();

	VOID DrawMarkInfo(CPoint fi_iOffsetP, CRect fi_rectTmp, bool fi_bRectCen);
	void ControlJog(int fi_iJogType, int fi_iJogDirection);
	INT DlgPutMMPMAutoCenterDisp();

	queue<int> QueEdge;

	CPoint	um_iSizeP;	// Mark Size

	int m_icamNum;
	CString m_Filename;

	bool	um_bDragFlag;
	bool	um_bClickFlag;
	bool	um_bRectClickFlag[4];

	CRect	um_rectTmp;
	CRect	um_rectOld;
	CRect   um_rectArea;

	int m_iJogStep;
	int m_iJogType;

	int m_Cetner_Smoothness;

	double um_iEdgeCenterX[100];
	double um_iEdgeCenterY[100];
	int EdgeIndex;
	bool EdgeExist;
	double tmpEdgeNumber[100];
	//----- Mask Click Point
	//CPoint	um_iStartP;
	//CPoint	um_iEndP;

	double dEdgeCenterX;
	double dEdgeCenterY;

	CListCtrl m_list_centerfind;

	CSliderCtrl m_slide_center_smooth;
	int m_edit_center_smooth;


	CBitmapButton  m_btnMoveLeft, m_btnMoveRight, m_btnMoveUp, m_btnMoveDown;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnSetJogLeft();
	afx_msg void OnBnClickedBtnSetJogRight();
	afx_msg void OnBnClickedBtnSetJogUp();
	afx_msg void OnBnClickedBtnSetJogDown();
	afx_msg void OnBnClickedBtnCenterfind();
	afx_msg void OnNMClickListCenterfind(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	int m_edit_length_exclude;
	//afx_msg void OnBnClickedBtnZoomOutSet();
	//afx_msg void OnBnClickedBtnZoomFitSet();
	//afx_msg void OnBnClickedBtnZoomInSet();
	afx_msg void OnBnClickedBtnCenterSet();
};
