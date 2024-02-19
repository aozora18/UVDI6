// CDlgMMPMAutoCenter.cpp: 구현 파일
//

#include "pch.h"
#include "DlgMMPMAutoCenter.h"
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

#define JOG_DIR_LEFT	0
#define JOG_DIR_RIGHT	1
#define JOG_DIR_UP		2
#define JOG_DIR_DOWN	3

// CDlgMMPMAutoCenter 대화 상자

IMPLEMENT_DYNAMIC(CDlgMMPMAutoCenter, CDialogEx)

CDlgMMPMAutoCenter::CDlgMMPMAutoCenter(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MMPM_AUTOCENTER, pParent)
	, m_edit_length_exclude(0)
{
#ifndef _WIN32_WCE
	EnableActiveAccessibility();
#endif

	m_iJogStep = 0;
	m_iJogType = 0;
	m_edit_center_smooth = 40;
	m_edit_length_exclude = 500;
}

CDlgMMPMAutoCenter::~CDlgMMPMAutoCenter()
{
}

void CDlgMMPMAutoCenter::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_SET_JOG_UP, m_btnMoveUp);
	DDX_Control(pDX, IDC_BTN_SET_JOG_DOWN, m_btnMoveDown);
	DDX_Control(pDX, IDC_BTN_SET_JOG_LEFT, m_btnMoveLeft);
	DDX_Control(pDX, IDC_BTN_SET_JOG_RIGHT, m_btnMoveRight);
	DDX_Control(pDX, IDC_LIST_CENTERFIND, m_list_centerfind);
	DDX_Control(pDX, IDC_SLIDER_CENTERFIND_SMOOTH, m_slide_center_smooth);
	DDX_Text(pDX, IDC_EDIT_CENTER_EDIT, m_edit_center_smooth);
	DDX_Radio(pDX, IDC_RAD_SET_JOG_STEP_1, m_iJogStep);
	DDX_Radio(pDX, IDC_RAD_SET_JOG_TYPE_MOVE, m_iJogType);
	DDX_Text(pDX, IDC_EDT_LENGTH_EX, m_edit_length_exclude);
	DDV_MinMaxInt(pDX, m_edit_length_exclude, 0, 1000);
}


BEGIN_MESSAGE_MAP(CDlgMMPMAutoCenter, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgMMPMAutoCenter::OnBnClickedOk)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDCANCEL, &CDlgMMPMAutoCenter::OnBnClickedCancel)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_HSCROLL()
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED(IDC_BTN_SET_JOG_LEFT, &CDlgMMPMAutoCenter::OnBnClickedBtnSetJogLeft)
	ON_BN_CLICKED(IDC_BTN_SET_JOG_RIGHT, &CDlgMMPMAutoCenter::OnBnClickedBtnSetJogRight)
	ON_BN_CLICKED(IDC_BTN_SET_JOG_UP, &CDlgMMPMAutoCenter::OnBnClickedBtnSetJogUp)
	ON_BN_CLICKED(IDC_BTN_SET_JOG_DOWN, &CDlgMMPMAutoCenter::OnBnClickedBtnSetJogDown)
	ON_BN_CLICKED(IDC_BTN_CENTERFIND, &CDlgMMPMAutoCenter::OnBnClickedBtnCenterfind)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CENTERFIND, &CDlgMMPMAutoCenter::OnNMClickListCenterfind)
	ON_WM_MEASUREITEM()
	//ON_BN_CLICKED(IDC_BTN_ZOOM_OUT_SET, &CDlgMMPMAutoCenter::OnBnClickedBtnZoomOutSet)
	//ON_BN_CLICKED(IDC_BTN_ZOOM_FIT_SET, &CDlgMMPMAutoCenter::OnBnClickedBtnZoomFitSet)
	//ON_BN_CLICKED(IDC_BTN_ZOOM_IN_SET, &CDlgMMPMAutoCenter::OnBnClickedBtnZoomInSet)
	ON_BN_CLICKED(IDC_BTN_CENTER_SET, &CDlgMMPMAutoCenter::OnBnClickedBtnCenterSet)
END_MESSAGE_MAP()


// CDlgMMPMAutoCenter 메시지 처리기


BOOL CDlgMMPMAutoCenter::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	um_iSizeP = uvCmn_Camera_GetMarkSize(m_icamNum, TMP_MARK); // size를... 훔... 

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

void CDlgMMPMAutoCenter::InitDisp()
{
	CWnd* pWnd;
	pWnd = GetDlgItem(IDC_MMPMAUTOCENTER_DISP);
	uvEng_Camera_SetDispMMPM_AutoCenter(pWnd);
}

INT CDlgMMPMAutoCenter::DlgPutMMPMAutoCenterDisp()
{
	int iRtnVal = 0;
	GetDlgItem(IDC_MMPMAUTOCENTER_DISP)->ShowWindow(SW_SHOW);
	CWnd* pImageWnd = GetDlgItem(IDC_MMPMAUTOCENTER_DISP);
	CRect rDraw;
	pImageWnd->GetClientRect(rDraw);

	//if (uvCmn_Camera_PutSetMarkDisp(pImageWnd->m_hWnd, rDraw))

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

void CDlgMMPMAutoCenter::UpdateInfo()
{
	CPoint	tmpOffSet;

	tmpOffSet = uvCmn_Camera_GetMarkOffset(m_icamNum, FALSE, TMP_MARK);

	DlgPutMMPMAutoCenterDisp();

	m_slide_center_smooth.SetRangeMin(40, FALSE);
	m_slide_center_smooth.SetRangeMax(100, FALSE);
	m_slide_center_smooth.SetTicFreq(10);
	m_slide_center_smooth.SetPos(40);
}

void CDlgMMPMAutoCenter::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}

void CDlgMMPMAutoCenter::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CDlgMMPMAutoCenter::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}

void CDlgMMPMAutoCenter::DrawMarkInfo(CPoint fi_iOffsetP, CRect fi_rectTmp, bool fi_bRectCen)
{
	CRect	rectTmp;
	GetDlgItem(IDC_MMPMAUTOCENTER_DISP)->GetWindowRect(rectTmp);

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

void CDlgMMPMAutoCenter::ControlJog(int fi_iJogType, int fi_iJogDirection)
{
	UpdateData(TRUE);

	int iJogStep;
	if (m_iJogStep == 0)		iJogStep = 1;
	else if (m_iJogStep == 1)	iJogStep = 5;
	else if (m_iJogStep == 2)	iJogStep = 10;
	else if (m_iJogStep == 3)	iJogStep = 20;

	CPoint tmpNewOff;
	tmpNewOff = uvCmn_Camera_GetMarkOffset(m_icamNum, true, TMP_MARK);
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
	DrawMarkInfo(tmpNewOff, um_rectTmp, false);
}

HBRUSH CDlgMMPMAutoCenter::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}


void CDlgMMPMAutoCenter::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	UpdateData(TRUE);
	um_bClickFlag = true;

	WINDOWPLACEMENT wndpl;
	int idNo = IDC_MMPMAUTOCENTER_DISP;
	GetDlgItem(idNo)->GetWindowPlacement(&wndpl);

	CRect rectTmp;
	rectTmp = wndpl.rcNormalPosition;
	point.x = point.x - rectTmp.left;
	point.y = point.y - rectTmp.top;

	if (point.x > um_rectTmp.left - 5 && point.x < um_rectTmp.left + 5)			um_bRectClickFlag[0] = true;
	else if (point.x > um_rectTmp.right - 5 && point.x < um_rectTmp.right + 5)	um_bRectClickFlag[1] = true;
	else if (point.y > um_rectTmp.top - 5 && point.y < um_rectTmp.top + 5)		um_bRectClickFlag[2] = true;
	else if (point.y > um_rectTmp.bottom - 5 && point.y < um_rectTmp.bottom + 5)	um_bRectClickFlag[3] = true;
	
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CDlgMMPMAutoCenter::OnLButtonUp(UINT nFlags, CPoint point)
{
	um_bClickFlag = false;
	um_bRectClickFlag[0] = false;
	um_bRectClickFlag[1] = false;
	um_bRectClickFlag[2] = false;
	um_bRectClickFlag[3] = false;

	WINDOWPLACEMENT wndpl;
	int idNo = IDC_MMPMAUTOCENTER_DISP;
	GetDlgItem(idNo)->GetWindowPlacement(&wndpl);

	CRect rectTmp;
	rectTmp = wndpl.rcNormalPosition;
	point.x = point.x - rectTmp.left;
	point.y = point.y - rectTmp.top;

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CDlgMMPMAutoCenter::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	WINDOWPLACEMENT wndpl;
	int idNo = IDC_MMPMAUTOCENTER_DISP;
	GetDlgItem(idNo)->GetWindowPlacement(&wndpl);

	CRect rectTmp;
	rectTmp = wndpl.rcNormalPosition;
	point.x = point.x - rectTmp.left;
	point.y = point.y - rectTmp.top;
	double tmpDispRate = uvCmn_Camera_GetMarkSetDispRate();
	CPoint tmpNewOffset = uvCmn_Camera_GetMarkOffset(m_icamNum, true, TMP_MARK);
	CPoint tmpOffset = uvCmn_Camera_GetMarkOffset(m_icamNum, false, TMP_MARK);

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
	//}
	CDialogEx::OnMouseMove(nFlags, point);
}

void CDlgMMPMAutoCenter::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CSliderCtrl* Slider = (CSliderCtrl*)pScrollBar;
	int Pos = Slider->GetPos();

	if ((UINT)pScrollBar == (UINT)&m_slide_center_smooth)
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

void CDlgMMPMAutoCenter::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	WINDOWPLACEMENT wndpl;
	int idNo = IDC_MMPMAUTOCENTER_DISP;
	GetDlgItem(idNo)->GetWindowPlacement(&wndpl);

	CRect rectTmp;
	rectTmp = wndpl.rcNormalPosition;
	point.x = point.x - rectTmp.left;
	point.y = point.y - rectTmp.top;

	uvCmn_Camera_SetMarkOffset(m_icamNum, point, 1, TMP_MARK);

	DrawMarkInfo(point, um_rectTmp, false);

	CDialogEx::OnRButtonDown(nFlags, point);
}

void CDlgMMPMAutoCenter::OnBnClickedBtnSetJogLeft()
{
	UpdateData(TRUE);
	ControlJog(m_iJogType, JOG_DIR_LEFT);
}

void CDlgMMPMAutoCenter::OnBnClickedBtnSetJogRight()
{
	UpdateData(TRUE);
	ControlJog(m_iJogType, JOG_DIR_RIGHT);
}

void CDlgMMPMAutoCenter::OnBnClickedBtnSetJogUp()
{
	UpdateData(TRUE);
	ControlJog(m_iJogType, JOG_DIR_UP);
}

void CDlgMMPMAutoCenter::OnBnClickedBtnSetJogDown()
{
	UpdateData(TRUE);
	ControlJog(m_iJogType, JOG_DIR_DOWN);
}

void CDlgMMPMAutoCenter::OnBnClickedBtnCenterfind()
{
	UpdateData(true);

	WINDOWPLACEMENT wndpl;
	int idNo = IDC_MMPMAUTOCENTER_DISP;
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

	int CenterFindMode = 2;
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

void CDlgMMPMAutoCenter::OnNMClickListCenterfind(NMHDR* pNMHDR, LRESULT* pResult)
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

void CDlgMMPMAutoCenter::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDCtl == IDC_LIST_CENTERFIND)
	{
		lpMeasureItemStruct->itemHeight += 3;
	}
	CDialogEx::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}


void CDlgMMPMAutoCenter::OnBnClickedBtnCenterSet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
