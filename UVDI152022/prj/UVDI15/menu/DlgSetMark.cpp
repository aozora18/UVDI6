// .\prj\UVDI15\menu\DlgSetMark.cpp: 구현 파일
//

#include "pch.h"
#include "DlgSetMark.h"
#include "afxdialogex.h"
#include "../MainApp.h"
#include "../mesg/DlgMesg.h"

#include <memory.h>
#include <wchar.h>

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//--- Jog Control
//#define JOG_MODE_MOVE	0	
//#define JOG_MODE_SIZE	1		

#define JOG_DIR_LEFT	0
#define JOG_DIR_RIGHT	1
#define JOG_DIR_UP		2
#define JOG_DIR_DOWN	3

// CDlgSetMark 대화 상자

IMPLEMENT_DYNAMIC(CDlgSetMark, CDialogEx)

CDlgSetMark::CDlgSetMark(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SETMARK, pParent)
	, m_edit_length_exclude(0)
{
	m_iMaskType = 0;
	m_iJogStep = 0;
	m_iJogType = 0;
	m_iBrushSize = 2;
	m_edit_center_smooth = 40;
	m_edit_length_exclude = 500;
}

CDlgSetMark::~CDlgSetMark()
{
}

void CDlgSetMark::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_DRAW_MASK, m_bDrawMask);
	DDX_Text(pDX, IDC_EDT_BRUSH_SIZE, m_iBrushSize);
	DDX_Control(pDX, IDC_SLIDER_BRUSH_SIZE, m_sliderBrushSize);
	DDX_Control(pDX, IDC_BTN_SET_JOG_UP, m_btnMoveUp);
	DDX_Control(pDX, IDC_BTN_SET_JOG_DOWN, m_btnMoveDown);
	DDX_Control(pDX, IDC_BTN_SET_JOG_LEFT, m_btnMoveLeft);
	DDX_Control(pDX, IDC_BTN_SET_JOG_RIGHT, m_btnMoveRight);
	DDX_Control(pDX, IDC_LIST_CENTERFIND, m_list_centerfind);
	DDX_Control(pDX, IDC_SLIDER_CENTERFIND_SMOOTH, m_slide_center_smooth);
	DDX_Text(pDX, IDC_EDIT_CENTER_EDIT, m_edit_center_smooth);
	DDX_Radio(pDX, IDC_RAD_MASK_TYPE_BRUSH, m_iMaskType);
	DDX_Radio(pDX, IDC_RAD_SET_JOG_STEP_1, m_iJogStep);
	DDX_Radio(pDX, IDC_RAD_SET_JOG_TYPE_MOVE, m_iJogType);
	DDX_Text(pDX, IDC_EDT_LENGTH_EX, m_edit_length_exclude);
	DDV_MinMaxInt(pDX, m_edit_length_exclude, 0, 1000);
}

// CDlgSetMark 메시지 처리기
BEGIN_MESSAGE_MAP(CDlgSetMark, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgSetMark::OnBnClickedOk)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDCANCEL, &CDlgSetMark::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_MARK_SAVE, &CDlgSetMark::OnBnClickedBtnMarkSave)
	ON_BN_CLICKED(IDC_BTN_MARK_EXIT, &CDlgSetMark::OnBnClickedBtnMarkExit)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_MASK_CLEAR, &CDlgSetMark::OnBnClickedBtnMaskClear)
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED(IDC_BTN_SET_JOG_LEFT, &CDlgSetMark::OnBnClickedBtnSetJogLeft)
	ON_BN_CLICKED(IDC_BTN_SET_JOG_RIGHT, &CDlgSetMark::OnBnClickedBtnSetJogRight)
	ON_BN_CLICKED(IDC_BTN_SET_JOG_UP, &CDlgSetMark::OnBnClickedBtnSetJogUp)
	ON_BN_CLICKED(IDC_BTN_SET_JOG_DOWN, &CDlgSetMark::OnBnClickedBtnSetJogDown)
	ON_BN_CLICKED(IDC_BTN_CENTERFIND, &CDlgSetMark::OnBnClickedBtnCenterfind)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CENTERFIND, &CDlgSetMark::OnNMClickListCenterfind)
	ON_WM_MEASUREITEM()
	ON_BN_CLICKED(IDC_BTN_ZOOM_OUT_SET, &CDlgSetMark::OnBnClickedBtnZoomOutSet)
	ON_BN_CLICKED(IDC_BTN_ZOOM_FIT_SET, &CDlgSetMark::OnBnClickedBtnZoomFitSet)
	ON_BN_CLICKED(IDC_BTN_ZOOM_IN_SET, &CDlgSetMark::OnBnClickedBtnZoomInSet)
END_MESSAGE_MAP()


BOOL CDlgSetMark::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	um_iSizeP = uvCmn_Camera_GetMarkSize(m_icamNum, TMP_MARK); 

	InitDisp();
	UpdateInfo();


	CFont font;
	font.CreatePointFont(8, _T("Arial"));
	m_list_centerfind.SetFont(&font);
	m_list_centerfind.ModifyStyle(LVS_OWNERDRAWFIXED, 0, 0);
	m_list_centerfind.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list_centerfind.InsertColumn(1, _T("X, Y"), LVCFMT_CENTER, 170);
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

INT_PTR CDlgSetMark::MyDoModal(int fi_camid, CString fi_filename)
{
	m_icamNum = fi_camid;
	m_Filename = fi_filename;

	return DoModal();
}

void CDlgSetMark::InitDisp()
{
	CWnd* pWnd;
	pWnd = GetDlgItem(IDC_SETMARK_DISP);
	uvEng_Camera_SetDispMarkSet(pWnd);
}

INT CDlgSetMark::DlgPutSetMarkDisp(DOUBLE fi_Rate) 
{
	int iRtnVal = 0;
	GetDlgItem(IDC_SETMARK_DISP)->ShowWindow(SW_SHOW);
	CWnd* pImageWnd = GetDlgItem(IDC_SETMARK_DISP);
	CRect rDraw;
	pImageWnd->GetClientRect(rDraw);

	TCHAR* tcharFileName = _T("");
	CFileFind	finder;
	CString		strDir;
	//CUniToChar csCnv1, csCnv2;
	BOOL IsFind;
	int tmpfindType;
	strDir.Format(_T("%s\\%s\\mmf\\%s.mmf"), g_tzWorkDir, CUSTOM_DATA_CONFIG, m_Filename);
	IsFind = finder.FindFile(strDir);
	if (IsFind) {
		// CString -> TCHAR * 
		tcharFileName = (TCHAR*)(LPCTSTR)strDir;
		tmpfindType = 0;
	}
	else {
		strDir.Format(_T("%s\\%s\\pat\\%s.pat"), g_tzWorkDir, CUSTOM_DATA_CONFIG, m_Filename);
		IsFind = finder.FindFile(strDir);
		if (IsFind) {
			// CString -> TCHAR * 
			tcharFileName = (TCHAR*)(LPCTSTR)strDir;
			tmpfindType = 1;
		}
	}
	if (IsFind) {
		if (uvCmn_Camera_PutSetMarkDisp(pImageWnd->m_hWnd, rDraw, m_icamNum, tcharFileName, tmpfindType, fi_Rate))
			iRtnVal = 0; // ok
		else
			iRtnVal = 1; 
	}
	else
		iRtnVal = 99;

	double tmpDispRate = uvCmn_Camera_GetMarkSetDispRate();
	CPoint tmpOffset = uvCmn_Camera_GetMarkOffset(m_icamNum, false, TMP_MARK);
	CPoint tmpNewOffset;
	tmpNewOffset.x = (int)(tmpOffset.x * tmpDispRate + 0.5);
	tmpNewOffset.y = (int)(tmpOffset.y * tmpDispRate + 0.5);
	uvCmn_Camera_SetMarkOffset(m_icamNum, tmpNewOffset, 1, TMP_MARK);
	um_rectTmp.right = (int)(um_iSizeP.x * tmpDispRate + 0.5);
	um_rectTmp.bottom = (int)(um_iSizeP.y * tmpDispRate + 0.5);
	DrawMarkInfo(tmpNewOffset, um_rectTmp, false);

	return iRtnVal;
}

void CDlgSetMark::UpdateInfo()
{
	CPoint	tmpOffSet;

	tmpOffSet = uvCmn_Camera_GetMarkOffset(m_icamNum, FALSE, TMP_MARK);

	//tmpOffSet.x = MARK_BUF_SIZE_X / 2;
	//tmpOffSet.y = MARK_BUF_SIZE_Y / 2;
	//uvCmn_Camera_SetMarkOffset(m_icamNum, tmpOffSet, 0, TMP_MARK);
	//uvCmn_Camera_SetMarkOffset(m_icamNum, tmpOffSet, 1, TMP_MARK);

	uvCmn_Camera_InitMask(m_icamNum);
	DlgPutSetMarkDisp(1.0);
	uvCmn_Camera_MaskBufGet();

	m_sliderBrushSize.SetRangeMin(2, FALSE);
	m_sliderBrushSize.SetRangeMax(10, FALSE);
	m_sliderBrushSize.SetTicFreq(1);

	m_slide_center_smooth.SetRangeMin(40, FALSE);
	m_slide_center_smooth.SetRangeMax(100, FALSE);
	m_slide_center_smooth.SetTicFreq(10);
	m_slide_center_smooth.SetPos(40);
}

void CDlgSetMark::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}

void CDlgSetMark::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CDlgSetMark::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}

void CDlgSetMark::OnBnClickedBtnMarkSave()
{
	uvCmn_Camera_SaveMask_MOD(m_icamNum, TMP_MARK); 
	uvCmn_Camera_SaveMask_PAT(m_icamNum, TMP_MARK); 

	CPoint tmpPoint;
	tmpPoint = uvCmn_Camera_GetMarkOffset(m_icamNum, true, TMP_MARK);
	uvCmn_Camera_SetMarkOffset(m_icamNum, tmpPoint, 2, TMP_MARK);

	TCHAR SaveFileName[1000];
	_stprintf_s(SaveFileName, 1000, _T("%s\\%s\\mmf\\%s.mmf"), g_tzWorkDir, CUSTOM_DATA_CONFIG, m_Filename);
	uvCmn_Camera_ModMarkSave(m_icamNum, SaveFileName, TMP_MARK);
	_stprintf_s(SaveFileName, 1000, _T("%s\\%s\\pat\\%s.pat"), g_tzWorkDir, CUSTOM_DATA_CONFIG, m_Filename);
	uvCmn_Camera_PatMarkSave(m_icamNum, SaveFileName, TMP_MARK);

	DrawMarkInfo(tmpPoint, um_rectTmp, false);
	um_rectOld = um_rectTmp;
}

void CDlgSetMark::OnBnClickedBtnMarkExit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	uvCmn_Camera_CloseSetMark();

	CDialogEx::OnOK();
}

void CDlgSetMark::DrawMarkInfo(CPoint fi_iOffsetP, CRect fi_rectTmp, bool fi_bRectCen) 
{
	CRect	rectTmp;
	GetDlgItem(IDC_SETMARK_DISP)->GetWindowRect(rectTmp);

	// 센터 & 외각 오버레이 그리고...
	uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MARKSET, 0);
	
	uvEng_Camera_OverlayAddLineList(DISP_TYPE_MARKSET, 0, fi_iOffsetP.x, 0, fi_iOffsetP.x, rectTmp.bottom, PS_SOLID, eM_COLOR_GREEN);
	uvEng_Camera_OverlayAddLineList(DISP_TYPE_MARKSET, 0, 0, fi_iOffsetP.y, rectTmp.right, fi_iOffsetP.y, PS_SOLID, eM_COLOR_GREEN);
	uvEng_Camera_OverlayAddLineList(DISP_TYPE_MARKSET, 0, fi_rectTmp.left, 0, fi_rectTmp.left, rectTmp.bottom, PS_SOLID, eM_COLOR_BLUE);
	uvEng_Camera_OverlayAddLineList(DISP_TYPE_MARKSET, 0, 0, fi_rectTmp.top, rectTmp.right, fi_rectTmp.top, PS_SOLID, eM_COLOR_BLUE);
	uvEng_Camera_OverlayAddLineList(DISP_TYPE_MARKSET, 0, fi_rectTmp.right, 0, fi_rectTmp.right, rectTmp.bottom, PS_SOLID, eM_COLOR_BLUE);
	uvEng_Camera_OverlayAddLineList(DISP_TYPE_MARKSET, 0, 0, fi_rectTmp.bottom, rectTmp.right, fi_rectTmp.bottom, PS_SOLID, eM_COLOR_BLUE);

	if (fi_bRectCen)
	{//--- 센터 세팅할 때 변경예정되는 포인트 그리고..
		CPoint tmpNewOffset;
		tmpNewOffset.x = (fi_rectTmp.left + fi_rectTmp.right) >> 1;
		tmpNewOffset.y = (fi_rectTmp.top + fi_rectTmp.bottom) >> 1;
		uvCmn_Camera_SetMarkOffset(m_icamNum, tmpNewOffset, 1, TMP_MARK);

		uvEng_Camera_OverlayAddLineList(DISP_TYPE_MARKSET, 0, tmpNewOffset.x, 0, tmpNewOffset.x, rectTmp.bottom, PS_DOT, eM_COLOR_GREEN);
		uvEng_Camera_OverlayAddLineList(DISP_TYPE_MARKSET, 0, 0, tmpNewOffset.y, rectTmp.right, tmpNewOffset.y, PS_DOT, eM_COLOR_GREEN);
	}
	uvEng_Camera_DrawOverlayDC(true, DISP_TYPE_MARKSET, 0);

	uvEng_Camera_DrawMarkInfo_UseMIL(m_icamNum, m_edit_center_smooth, TMP_MARK); 
}

void CDlgSetMark::ControlJog(int fi_iJogType, int fi_iJogDirection)
{
	UpdateData(TRUE);

	int iJogStep;
	if (m_iJogStep == 0)		iJogStep = 1;
	else if (m_iJogStep == 1)	iJogStep = 5;
	else if (m_iJogStep == 2)	iJogStep = 10;
	else if (m_iJogStep == 3)	iJogStep = 20;
	
	CPoint tmpNewOff;
	tmpNewOff = uvCmn_Camera_GetMarkOffset(m_icamNum, true, TMP_MARK);
	//if (fi_iJogType == JOG_MODE_MOVE)
	//{//center move
		if (fi_iJogDirection == JOG_DIR_LEFT)
		{
			tmpNewOff.x = tmpNewOff.x - iJogStep;
			uvCmn_Camera_SetMarkOffset(m_icamNum, tmpNewOff, 1, TMP_MARK);
		}
		else if (fi_iJogDirection == JOG_DIR_RIGHT)
		{
			tmpNewOff.x = tmpNewOff.x + iJogStep;
			uvCmn_Camera_SetMarkOffset(m_icamNum, tmpNewOff, 1, TMP_MARK);
		}
		else if (fi_iJogDirection == JOG_DIR_UP)
		{
			tmpNewOff.y = tmpNewOff.y - iJogStep;
			uvCmn_Camera_SetMarkOffset(m_icamNum, tmpNewOff, 1, TMP_MARK);
		}
		else if (fi_iJogDirection == JOG_DIR_DOWN)
		{
			tmpNewOff.y = tmpNewOff.y + iJogStep;
			uvCmn_Camera_SetMarkOffset(m_icamNum, tmpNewOff, 1, TMP_MARK);
		}
	//}
	//else if (fi_iJogType == JOG_MODE_SIZE)
	//{//Re size
	//	if (fi_iJogDirection == JOG_DIR_LEFT)
	//	{
	//		um_rectTmp.left = um_rectTmp.left + iJogStep;
	//		um_rectTmp.right = um_rectTmp.right - iJogStep;

	//		if (um_rectTmp.left < 0)					um_rectTmp.left = 0;
	//		if (um_rectTmp.left > um_rectTmp.right - 15)	um_rectTmp.left = um_rectTmp.right - 15;
	//	}
	//	else if (fi_iJogDirection == JOG_DIR_RIGHT)
	//	{
	//		um_rectTmp.left = um_rectTmp.left - iJogStep;
	//		um_rectTmp.right = um_rectTmp.right + iJogStep;

	//		if (um_rectTmp.right < um_rectTmp.left + 10)	um_rectTmp.right = um_rectTmp.left + 10;
	//		if (um_rectTmp.right > um_iSizeP.x)			um_rectTmp.right = um_iSizeP.x;
	//	}
	//	else if (fi_iJogDirection == JOG_DIR_UP)
	//	{
	//		um_rectTmp.top = um_rectTmp.top + iJogStep;
	//		um_rectTmp.bottom = um_rectTmp.bottom - iJogStep;

	//		if (um_rectTmp.top < 0)						um_rectTmp.top = 0;
	//		if (um_rectTmp.top > um_rectTmp.bottom - 15)	um_rectTmp.top = um_rectTmp.bottom - 15;
	//	}
	//	else if (fi_iJogDirection == JOG_DIR_DOWN)
	//	{
	//		um_rectTmp.top = um_rectTmp.top - iJogStep;
	//		um_rectTmp.bottom = um_rectTmp.bottom + iJogStep;

	//		if (um_rectTmp.bottom < um_rectTmp.top + 10)	um_rectTmp.bottom = um_rectTmp.top + 10;
	//		if (um_rectTmp.bottom > um_iSizeP.y)			um_rectTmp.bottom = um_iSizeP.y;
	//	}
	//}
	DrawMarkInfo(tmpNewOff, um_rectTmp, false);
}

HBRUSH CDlgSetMark::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	UINT nID = pWnd->GetDlgCtrlID();

	switch (nID)
	{
	case IDC_CHK_DRAW_MASK:
		pDC->SetBkMode(TRANSPARENT);
		hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
		break;
	}
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}


void CDlgSetMark::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	UpdateData(TRUE);
	um_bClickFlag = true;

	WINDOWPLACEMENT wndpl;
	int idNo = IDC_SETMARK_DISP;
	GetDlgItem(idNo)->GetWindowPlacement(&wndpl);

	CRect rectTmp;
	rectTmp = wndpl.rcNormalPosition;
	point.x = point.x - rectTmp.left;
	point.y = point.y - rectTmp.top;

	if (m_bDrawMask)
	{// 마스크 그릴 때...
		if (m_iMaskType == 0)
		{//--- Brush
			// COLORREF fi_color
			double tmpDispRate = uvCmn_Camera_GetMarkSetDispRate();
			int sx, ex, sy, ey;
			sx = point.x - m_iBrushSize;
			ex = point.x + m_iBrushSize;
			sy = point.y - m_iBrushSize;
			ey = point.y + m_iBrushSize;

			CPoint	iTmpSizeP;
			iTmpSizeP.x = (int)(um_iSizeP.x * tmpDispRate + 0.5); //um_dRate : zoom에 사용.. 1.0으로 계산, um_iSizeP : Mark 사이즈
			iTmpSizeP.y = (int)(um_iSizeP.y * tmpDispRate + 0.5);

			if (sx < 0)				sx = 0;
			if (sy < 0)				sy = 0;
			if (ex > iTmpSizeP.x - 1)		ex = iTmpSizeP.x - 1;
			if (ey > iTmpSizeP.y - 1)		ey = iTmpSizeP.y - 1;

			if ((sx > iTmpSizeP.x - 1) || (sy > iTmpSizeP.y - 1)) return;

			CRect rectTmp;
			rectTmp.left = (int)(sx / tmpDispRate + 0.5);
			rectTmp.right = (int)(ex / tmpDispRate + 0.5);
			rectTmp.top = (int)(sy / tmpDispRate + 0.5);
			rectTmp.bottom = (int)(ey / tmpDispRate + 0.5);


			CRect rectFill;
			rectFill.left = sx;
			rectFill.top = sy;
			rectFill.right = ex;
			rectFill.bottom = ey;

			uvEng_Camera_Mask_MarkSet(m_icamNum, rectTmp, um_iSizeP, rectFill, eM_COLOR_BLUE, TRUE);
		}
		else
		{//--- Rect
			um_iStartP = point;
		}
	}
	else
	{// 센터변경할 때...
		if (point.x > um_rectTmp.left - 5 && point.x < um_rectTmp.left + 5)			um_bRectClickFlag[0] = true;
		else if (point.x > um_rectTmp.right - 5 && point.x < um_rectTmp.right + 5)	um_bRectClickFlag[1] = true;
		else if (point.y > um_rectTmp.top - 5 && point.y < um_rectTmp.top + 5)		um_bRectClickFlag[2] = true;
		else if (point.y > um_rectTmp.bottom - 5 && point.y < um_rectTmp.bottom + 5)	um_bRectClickFlag[3] = true;
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CDlgSetMark::OnLButtonUp(UINT nFlags, CPoint point)
{
	um_bClickFlag = false;
	um_bRectClickFlag[0] = false;
	um_bRectClickFlag[1] = false;
	um_bRectClickFlag[2] = false;
	um_bRectClickFlag[3] = false;

	WINDOWPLACEMENT wndpl;
	int idNo = IDC_SETMARK_DISP;
	GetDlgItem(idNo)->GetWindowPlacement(&wndpl);

	CRect rectTmp;
	rectTmp = wndpl.rcNormalPosition;
	point.x = point.x - rectTmp.left;
	point.y = point.y - rectTmp.top;

	if (m_bDrawMask && um_bDragFlag)
	{
		um_bDragFlag = false;
		double tmpDispRate = uvCmn_Camera_GetMarkSetDispRate();

		CPoint	iTmpSizeP;
		iTmpSizeP.x = (int)(um_iSizeP.x * tmpDispRate + 0.5);
		iTmpSizeP.y = (int)(um_iSizeP.y * tmpDispRate + 0.5);

		if (point.x < 0)				point.x = 0;
		if (point.x > iTmpSizeP.x)	point.x = iTmpSizeP.x;
		if (point.y < 0)				point.y = 0;
		if (point.y > iTmpSizeP.y)	point.y = iTmpSizeP.y;

		um_iEndP.x = point.x;
		um_iEndP.y = point.y;

		// Filtering...
		int iTemp;
		if (um_iStartP.x > um_iEndP.x)
		{
			iTemp = um_iStartP.x;
			um_iStartP.x = um_iEndP.x;
			um_iEndP.x = iTemp;
		}
		if (um_iStartP.y > um_iEndP.y)
		{
			iTemp = um_iStartP.y;
			um_iStartP.y = um_iEndP.y;
			um_iEndP.y = iTemp;
		}
		if (um_iEndP.x > iTmpSizeP.x)	um_iEndP.x = iTmpSizeP.x;
		if (um_iEndP.y > iTmpSizeP.y)	um_iEndP.y = iTmpSizeP.y;


		CRect rectTmp;
		rectTmp.left = (int)(um_iStartP.x / tmpDispRate + 0.5);
		rectTmp.right = (int)(um_iEndP.x / tmpDispRate + 0.5);
		rectTmp.top = (int)(um_iStartP.y / tmpDispRate + 0.5);
		rectTmp.bottom = (int)(um_iEndP.y / tmpDispRate + 0.5);

		if (m_iMaskType == 1)
		{// 마스크>사각박스(내부) 타입일 때...
			CRect rectFill;
			rectFill.left = um_iStartP.x;
			rectFill.top = um_iStartP.y;
			rectFill.right = um_iEndP.x;
			rectFill.bottom = um_iEndP.y;

			uvEng_Camera_Mask_MarkSet(m_icamNum, rectTmp, um_iSizeP, rectFill, eM_COLOR_GREEN, TRUE);
		}
	}
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CDlgSetMark::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	WINDOWPLACEMENT wndpl;
	int idNo = IDC_SETMARK_DISP;
	GetDlgItem(idNo)->GetWindowPlacement(&wndpl);

	CRect rectTmp;
	rectTmp = wndpl.rcNormalPosition;
	point.x = point.x - rectTmp.left;
	point.y = point.y - rectTmp.top;
	double tmpDispRate = uvCmn_Camera_GetMarkSetDispRate();
	CPoint tmpNewOffset = uvCmn_Camera_GetMarkOffset(m_icamNum, true, TMP_MARK);
	CPoint tmpOffset = uvCmn_Camera_GetMarkOffset(m_icamNum, false, TMP_MARK);

	if (m_bDrawMask && um_bClickFlag)
	{// 마스크 그릴 때...
		um_bDragFlag = true;
		if (m_iMaskType == 0)
		{//--- Brush
			int sx, ex, sy, ey;
			sx = point.x - m_iBrushSize;
			ex = point.x + m_iBrushSize;
			sy = point.y - m_iBrushSize;
			ey = point.y + m_iBrushSize;

			CPoint	iTmpSizeP;
			iTmpSizeP.x = (int)(um_iSizeP.x * tmpDispRate + 0.5);
			iTmpSizeP.y = (int)(um_iSizeP.y * tmpDispRate + 0.5);

			if (sx < 0)				sx = 0;
			if (sy < 0)				sy = 0;
			if (ex > iTmpSizeP.x - 1)		ex = iTmpSizeP.x - 1;
			if (ey > iTmpSizeP.y - 1)		ey = iTmpSizeP.y - 1;

			if ((sx > iTmpSizeP.x - 1) || (sy > iTmpSizeP.y - 1)) return;

			CRect rectTmp;
			rectTmp.left = (int)(sx / tmpDispRate + 0.5);
			rectTmp.right = (int)(ex / tmpDispRate + 0.5);
			rectTmp.top = (int)(sy / tmpDispRate + 0.5);
			rectTmp.bottom = (int)(ey / tmpDispRate + 0.5);


			CRect rectFill;
			rectFill.left = sx;
			rectFill.top = sy;
			rectFill.right = ex;
			rectFill.bottom = ey;

			uvEng_Camera_Mask_MarkSet(m_icamNum, rectTmp, um_iSizeP, rectFill, eM_COLOR_BLUE, TRUE);
		}
		else
		{//--- 사각박스 타입...
			CPoint iCurPos;
			iCurPos.x = point.x;
			iCurPos.y = point.y;

			DrawMarkInfo(tmpNewOffset, um_rectTmp, false); // lk91 여기 주석 처리함... 

			
			CRect rectFill;
			//rectFill.left = um_iStartP.x;
			//rectFill.top = um_iStartP.y;
			//rectFill.right = iCurPos.x;
			//rectFill.bottom = iCurPos.y;
			rectFill.left = (int)(um_iStartP.x / tmpDispRate + 0.5);
			rectFill.top = (int)(um_iStartP.y / tmpDispRate + 0.5);
			rectFill.right = (int)(iCurPos.x / tmpDispRate + 0.5);
			rectFill.bottom = (int)(iCurPos.y / tmpDispRate + 0.5);
			


			CRect zeroRect;
			zeroRect.left = 0;
			zeroRect.top = 0;
			zeroRect.right = 0;
			zeroRect.bottom = 0;
			CPoint zeroPoint;
			zeroPoint.x = 0;
			zeroPoint.y = 0;

			//uvEng_Camera_Mask_MarkSet(m_icamNum, zeroRect, zeroPoint, rectFill, eM_COLOR_GREEN);
			uvEng_Camera_Mask_MarkSet(m_icamNum, rectFill, um_iSizeP, zeroRect, eM_COLOR_GREEN, FALSE);
			//uvEng_Camera_Mask_MarkSet(m_icamNum, rectTmp, um_iSizeP, rectFill, eM_COLOR_GREEN);
		}
	}
	else
	{
		if (!um_bClickFlag)
		{
			// SetCapture, ReleaseCapture 영역 밖에서 마우스 move 이벤트 받고 싶을 때
			if ((point.x > um_rectTmp.left - 5 && point.x < um_rectTmp.left + 5) || (point.x > um_rectTmp.right - 5 && point.x < um_rectTmp.right + 5))
			{
				SetCapture();
			}
			else if ((point.y > um_rectTmp.top - 5 && point.y < um_rectTmp.top + 5) || (point.y > um_rectTmp.bottom - 5 && point.y < um_rectTmp.bottom + 5))
			{
				SetCapture();
			}
			else	ReleaseCapture();
		}
		else
		{
			if (um_bRectClickFlag[0])
			{
				um_rectTmp.left = point.x;

				if (um_rectTmp.left < 0)					um_rectTmp.left = 0;
				if (um_rectTmp.left > um_rectTmp.right - 15)	um_rectTmp.left = um_rectTmp.right - 15;

				DrawMarkInfo(tmpOffset, um_rectTmp, true);
			}
			else if (um_bRectClickFlag[1])
			{
				um_rectTmp.right = point.x;

				if (um_rectTmp.right < um_rectTmp.left + 10)	um_rectTmp.right = um_rectTmp.left + 10;
				if (um_rectTmp.right > um_iSizeP.x)		um_rectTmp.right = um_iSizeP.x;

				DrawMarkInfo(tmpOffset, um_rectTmp, true);
			}
			else if (um_bRectClickFlag[2])
			{
				um_rectTmp.top = point.y;

				if (um_rectTmp.top < 0)					um_rectTmp.top = 0;
				if (um_rectTmp.top > um_rectTmp.bottom - 15)	um_rectTmp.top = um_rectTmp.bottom - 15;

				DrawMarkInfo(tmpOffset, um_rectTmp, true);
			}
			else if (um_bRectClickFlag[3])
			{
				um_rectTmp.bottom = point.y;

				if (um_rectTmp.bottom < um_rectTmp.top + 10)	um_rectTmp.bottom = um_rectTmp.top + 10;
				if (um_rectTmp.bottom > um_iSizeP.y)		um_rectTmp.bottom = um_iSizeP.y;

				DrawMarkInfo(tmpOffset, um_rectTmp, true);
			}
		}
	}
	CDialogEx::OnMouseMove(nFlags, point);
}

void CDlgSetMark::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CSliderCtrl* Slider = (CSliderCtrl*)pScrollBar;
	int Pos = Slider->GetPos();

	if ((UINT)pScrollBar == (UINT)&m_sliderBrushSize)
	{
		if (Pos >= 2 && Pos <= 10)
		{
			m_iBrushSize = Pos;
		}
	}

	else if ((UINT)pScrollBar == (UINT)&m_slide_center_smooth)
	{
		if (Pos >= 40 && Pos <= 100)
		{
			m_edit_center_smooth = Pos;
			CPoint tmpNewOffset = uvCmn_Camera_GetMarkOffset(m_icamNum, true, TMP_MARK);
			DrawMarkInfo(tmpNewOffset, um_rectTmp, false);
		}
	}
	UpdateData(FALSE);

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CDlgSetMark::OnBnClickedBtnMaskClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	uvCmn_Camera_MaskClear_MOD(m_icamNum, um_iSizeP, TMP_MARK);
	uvCmn_Camera_MaskClear_PAT(m_icamNum, um_iSizeP, TMP_MARK);
	CPoint tmpNewOffset = uvCmn_Camera_GetMarkOffset(m_icamNum, true, TMP_MARK);
	DrawMarkInfo(tmpNewOffset, um_rectTmp, false);
}

void CDlgSetMark::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	WINDOWPLACEMENT wndpl;
	int idNo = IDC_SETMARK_DISP;
	GetDlgItem(idNo)->GetWindowPlacement(&wndpl);

	CRect rectTmp;
	rectTmp = wndpl.rcNormalPosition;
	point.x = point.x - rectTmp.left;
	point.y = point.y - rectTmp.top;

	uvCmn_Camera_SetMarkOffset(m_icamNum, point, 1, TMP_MARK);

	DrawMarkInfo(point, um_rectTmp, false);

	CDialogEx::OnRButtonDown(nFlags, point);
}

void CDlgSetMark::OnBnClickedBtnSetJogLeft()
{
	UpdateData(TRUE);
	ControlJog(m_iJogType, JOG_DIR_LEFT);
}

void CDlgSetMark::OnBnClickedBtnSetJogRight()
{
	UpdateData(TRUE);
	ControlJog(m_iJogType, JOG_DIR_RIGHT);
}

void CDlgSetMark::OnBnClickedBtnSetJogUp()
{
	UpdateData(TRUE);
	ControlJog(m_iJogType, JOG_DIR_UP);
}

void CDlgSetMark::OnBnClickedBtnSetJogDown()
{
	UpdateData(TRUE);
	ControlJog(m_iJogType, JOG_DIR_DOWN);
}

void CDlgSetMark::OnBnClickedBtnCenterfind()
{
	UpdateData(true);

	WINDOWPLACEMENT wndpl;
	int idNo = IDC_SETMARK_DISP;
	GetDlgItem(idNo)->GetWindowPlacement(&wndpl);

	CRect rectTmp;
	rectTmp = wndpl.rcNormalPosition;
	int NumEdgeFound; // Edge 찾은 갯수 찾기 

	queue<int> Empty;

	swap(QueEdge, Empty);

	int NumX = 0;
	int NumY = 0;

	EdgeIndex = 0;
	EdgeExist = false;

	m_list_centerfind.DeleteAllItems();

	for (int i = 0; i < 100; i++)
	{
		um_iEdgeCenterX[i] = 0;
		um_iEdgeCenterY[i] = 0;
		tmpEdgeNumber[i] = 0;
	}

	DOUBLE* NumEdgeMIN_X = new DOUBLE[100];
	DOUBLE* NumEdgeMAX_X = new DOUBLE[100];
	DOUBLE* NumEdgeMIN_Y = new DOUBLE[100];
	DOUBLE* NumEdgeMAX_Y = new DOUBLE[100];

	int curSmoothness = 0;

	curSmoothness = m_edit_center_smooth;
	if (curSmoothness < 5)	curSmoothness = 50;
	
	int CenterFindMode = 1;
	uvCmn_Camera_CenterFind(m_icamNum, m_edit_length_exclude, curSmoothness, NumEdgeMIN_X, NumEdgeMAX_X, NumEdgeMIN_Y, NumEdgeMAX_Y, &NumEdgeFound, CenterFindMode);

	CString Str_List[100];
	DOUBLE tmpDispRate = uvCmn_Camera_GetMarkSetDispRate();
	if (NumEdgeFound != 0)
	{
		EdgeExist = true;
		for (int i = 0; i < NumEdgeFound; i++)
		{
			tmpEdgeNumber[i] = i;
			um_iEdgeCenterX[i] = ((NumEdgeMAX_X[i] + NumEdgeMIN_X[i]) / 2) * tmpDispRate;
			um_iEdgeCenterY[i] = ((NumEdgeMAX_Y[i] + NumEdgeMIN_Y[i]) / 2) * tmpDispRate;
			Str_List[i].Format(_T("[%d] | X: %.01f , Y: %.01f"), i, um_iEdgeCenterX[i], um_iEdgeCenterY[i]);
			m_list_centerfind.InsertItem(i, Str_List[i]);
		}
	}
}

void CDlgSetMark::OnNMClickListCenterfind(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iRow = pNMItemActivate->iItem;
	int iColum = pNMItemActivate->iSubItem;

	if (iRow >= 0)
	{
		if (EdgeExist)
		{
			int index = tmpEdgeNumber[iRow];
			CPoint tmpNewOffSet, tmpOffSet;
			tmpNewOffSet.x = (LONG)um_iEdgeCenterX[index];
			tmpNewOffSet.y = (LONG)um_iEdgeCenterY[index];
			uvCmn_Camera_SetMarkOffset(m_icamNum, tmpNewOffSet, 1, TMP_MARK);

			tmpOffSet.x = tmpNewOffSet.x;
			tmpOffSet.y = tmpNewOffSet.y;
			uvCmn_Camera_SetMarkOffset(m_icamNum, tmpOffSet, 0, TMP_MARK);

			DrawMarkInfo(tmpOffSet, um_rectTmp, false);

		}
	}
	*pResult = 0;
}

void CDlgSetMark::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDCtl == IDC_LIST_CENTERFIND)
	{
		lpMeasureItemStruct->itemHeight += 3;
	}
	CDialogEx::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CDlgSetMark::OnBnClickedBtnZoomOutSet()
{
	double tmpDispRate = uvCmn_Camera_SetMarkSetDispRate(-0.5);

	if(DlgPutSetMarkDisp(tmpDispRate) != 0)
	{
		tmpDispRate = uvCmn_Camera_SetMarkSetDispRate(0.5);
		DlgPutSetMarkDisp(tmpDispRate);
	}
}

void CDlgSetMark::OnBnClickedBtnZoomFitSet()
{
	double tmpDispRate = uvCmn_Camera_SetMarkSetDispRate(1.0);
	DlgPutSetMarkDisp(tmpDispRate);
}

void CDlgSetMark::OnBnClickedBtnZoomInSet()
{
	double tmpDispRate = uvCmn_Camera_SetMarkSetDispRate(0.5);

	if (DlgPutSetMarkDisp(tmpDispRate) != 0)
	{
		tmpDispRate = uvCmn_Camera_SetMarkSetDispRate(-0.5);
		DlgPutSetMarkDisp(tmpDispRate);
	}
}
