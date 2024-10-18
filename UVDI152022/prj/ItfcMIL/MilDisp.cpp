#include "pch.h"
#include "MilDisp.h"
#include "MainApp.h"
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CMilDisp, CWnd)

/*
 desc : 생성자
 parm : 
 retn : None
*/
CMilDisp::CMilDisp()
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMilDisp::~CMilDisp()
{
	if (terminated == true) return;

	CloseMilDisp();
}

/* desc : Display 관련 변수 초기화 */
VOID CMilDisp::InitMilDisp()
{
	clLineList = new CDrawDcLine*[PART_CNT];// (CDrawDcLine**)malloc(sizeof(CDrawDcLine*) * PART_CNT);
	for (int i = 0; i < PART_CNT; i++) {
		clLineList[i] = new CDrawDcLine[theApp.clMilMain.GRABMARK_CNT];// (CDrawDcLine*)malloc(sizeof(CDrawDcLine) * theApp.clMilMain.GRABMARK_CNT);
	}

	clPixelList = new CDrawDcPixel[PART_CNT]; //(CDrawDcPixel**)malloc(sizeof(CDrawDcPixel*) * PART_CNT);
	for (int i = 0; i < PART_CNT; i++) {
		clPixelList[i] = new CDrawDcPixel[theApp.clMilMain.GRABMARK_CNT];
	}

	clBoxList = new  CDrawDcBox*[PART_CNT];// (CDrawDcBox**)malloc(sizeof(CDrawDcBox*) * PART_CNT);
	for (int i = 0; i < PART_CNT; i++) {
		clBoxList[i] = new CDrawDcBox[theApp.clMilMain.GRABMARK_CNT];// (CDrawDcBox*)malloc(sizeof(CDrawDcBox) * theApp.clMilMain.GRABMARK_CNT);
	}

	clCircleList = new CDrawDcCircle* [PART_CNT];// (CDrawDcCircle**)malloc(sizeof(CDrawDcCircle*) * PART_CNT);
	for (int i = 0; i < PART_CNT; i++) {
		clCircleList[i] = new CDrawDcCircle[theApp.clMilMain.GRABMARK_CNT];// (CDrawDcCircle*)malloc(sizeof(CDrawDcCircle) * theApp.clMilMain.GRABMARK_CNT);
	}

	clCrossList = new CDrawDcCross*[PART_CNT];// (CDrawDcCross**)malloc(sizeof(CDrawDcCross*) * PART_CNT);
	for (int i = 0; i < PART_CNT; i++) {
		clCrossList[i] = new CDrawDcCross[theApp.clMilMain.GRABMARK_CNT];//  (CDrawDcCross*)malloc(sizeof(CDrawDcCross) * theApp.clMilMain.GRABMARK_CNT);
	}

	clTextList = new CDrawDcText*[PART_CNT];// (CDrawDcText**)malloc(sizeof(CDrawDcText*) * PART_CNT);
	for (int i = 0; i < PART_CNT; i++) {
		clTextList[i] = new CDrawDcText[theApp.clMilMain.GRABMARK_CNT];// (CDrawDcText*)malloc(sizeof(CDrawDcText) * theApp.clMilMain.GRABMARK_CNT);
	}

	m_fZoomX = new float[theApp.clMilMain.CAM_CNT];
	m_fZoomY = new float[theApp.clMilMain.CAM_CNT];
	m_fPanX = new double[theApp.clMilMain.CAM_CNT];
	m_fPanY = new double[theApp.clMilMain.CAM_CNT];

	m_fFixZoomX = 1;
	m_fFixZoomY = 1;

	for (int i = 0; i < theApp.clMilMain.CAM_CNT; i++)
	{
		m_fZoomX[i] = 1;
		m_fZoomY[i] = 1;

		m_fPanX[i] = 0.0;
		m_fPanY[i] = 0.0;
	}
	dZoomX = 1;
	dZoomY = 1;
}
/* desc : Display 관련 변수 초기화 */
VOID CMilDisp::CloseMilDisp()
{
	if (terminated)
		return;

	if (m_fZoomX)		delete m_fZoomX;
	if (m_fZoomY)		delete m_fZoomY;
	if (m_fPanX)		delete m_fPanX;
	if (m_fPanY)		delete m_fPanY;

	for (int i = 0; i < PART_CNT; i++) 
	{
		delete clLineList[i];
		delete clPixelList[i];
		delete clBoxList[i];
		delete clCircleList[i];
		delete clCrossList[i];
		delete clTextList[i];
	}
	delete[] clLineList;
	delete[] clPixelList;
	delete[] clBoxList;
	delete[] clCircleList;
	delete[] clCrossList;
	delete[] clTextList;
	terminated = true;
}

/* desc : DispType 별로 Disp-Size/Org-Size 비율 반환 */
CDPoint CMilDisp::GetRateDispToBuf(int fi_DispType)
{
	CDPoint dRtnP;

	if (fi_DispType == DISP_TYPE_EXPO) // expo, 미사용...
	{
		//dRtnP.x = (double)theApp.clMilMain.EXPO_DISP_SIZE_X / (double)ACA1920_SIZE_X;
		//dRtnP.y = (double)theApp.clMilMain.EXPO_DISP_SIZE_Y / (double)ACA1920_SIZE_Y;
		dRtnP.x = 1.0;
		dRtnP.y = 1.0;
	}
	else if (fi_DispType == DISP_TYPE_MARK) // mark
	{
		dRtnP.x = theApp.clMilMain.MARK_DISP_RATE;
		dRtnP.y = theApp.clMilMain.MARK_DISP_RATE;
	}
	else if (fi_DispType == DISP_TYPE_MARK_LIVE) { // live
		dRtnP.x = (double)theApp.clMilMain.LIVE_DISP_SIZE_X / (double)ACA1920_SIZE_X;
		dRtnP.y = (double)theApp.clMilMain.LIVE_DISP_SIZE_Y / (double)ACA1920_SIZE_Y;
	}
	else if (fi_DispType == DISP_TYPE_MARKSET) { // mark set
		dRtnP.x = 1.0;
		dRtnP.y = 1.0;
	}
	else if (fi_DispType == DISP_TYPE_MMPM) { // MMPM
		dRtnP.x = (double)theApp.clMilMain.MMPM_DISP_SIZE_X / (double)ACA1920_SIZE_X;
		dRtnP.y = (double)theApp.clMilMain.MMPM_DISP_SIZE_Y / (double)ACA1920_SIZE_Y;
	}
	else if (fi_DispType == DISP_TYPE_CALB_CAMSPEC) { 
		dRtnP.x = (double)theApp.clMilMain.CALIB_CAMSPEC_DISP_SIZE_X / 
			(double)(theApp.clMilMain.rectSearhROI_Calb_CamSpec.right- theApp.clMilMain.rectSearhROI_Calb_CamSpec.left);
		dRtnP.y = (double)theApp.clMilMain.CALIB_CAMSPEC_DISP_SIZE_Y / 
			(double)(theApp.clMilMain.rectSearhROI_Calb_CamSpec.bottom - theApp.clMilMain.rectSearhROI_Calb_CamSpec.top);
	}
	else if (fi_DispType == DISP_TYPE_CALB_EXPO) { 
		dRtnP.x = (double)theApp.clMilMain.CALIB_EXPO_DISP_SIZE_X / 
			(double)(theApp.clMilMain.rectSearhROI_Calb_Step.right - theApp.clMilMain.rectSearhROI_Calb_Step.left);
		dRtnP.y = (double)theApp.clMilMain.CALIB_EXPO_DISP_SIZE_Y / 
			(double)(theApp.clMilMain.rectSearhROI_Calb_Step.bottom - theApp.clMilMain.rectSearhROI_Calb_Step.top);
	}
	else if (fi_DispType == DISP_TYPE_CALB_ACCR) {
		dRtnP.x = (double)theApp.clMilMain.ACCR_DISP_SIZE_X /
			(double)(theApp.clMilMain.rectSearhROI_Calb_Accr.right - theApp.clMilMain.rectSearhROI_Calb_Accr.left);
		dRtnP.y = (double)theApp.clMilMain.ACCR_DISP_SIZE_Y /
			(double)(theApp.clMilMain.rectSearhROI_Calb_Accr.bottom - theApp.clMilMain.rectSearhROI_Calb_Accr.top);
	}
	else if (fi_DispType == DISP_TYPE_MMPM_AUTOCENTER) {
		dRtnP.x = (double)theApp.clMilMain.MMPM_AUTOCENTER_DISP_SIZE_X /
			(double)(theApp.clMilMain.rectSearhROI_MPMM_AutoCenter.right - theApp.clMilMain.rectSearhROI_MPMM_AutoCenter.left);
		dRtnP.y = (double)theApp.clMilMain.MMPM_AUTOCENTER_DISP_SIZE_Y /
			(double)(theApp.clMilMain.rectSearhROI_MPMM_AutoCenter.bottom - theApp.clMilMain.rectSearhROI_MPMM_AutoCenter.top);
	}
	else {
		dRtnP.x = 1.0;
		dRtnP.y = 1.0;
	}
	return dRtnP;
}

/* desc : Cam번호로 원본Img 버퍼 사이즈 반환 */
CPoint CMilDisp::GetOrgImgSize() // lk91 미사용!
{
	CPoint iRtnP;

	//if (fi_iCamNo == _CAM_SCARA_1 || fi_iCamNo == _CAM_ALIGN_1 || fi_iCamNo == _CAM_ALIGN_2)
	//{
		iRtnP.x = ACA1920_SIZE_X;
		iRtnP.y = ACA1920_SIZE_Y;
	//}
	//else if (fi_iCamNo == _CAM_INSP1_1 || fi_iCamNo == _CAM_INSP1_2 || fi_iCamNo == _CAM_INSP2_1 || fi_iCamNo == _CAM_INSP2_2 || fi_iCamNo == CAM_CNT + VIEWER_MINI_CNT + VIEWER_FULL_CNT)
	//{
	//	iRtnP.x = ACA2440_SIZE_X;
	//	iRtnP.y = ACA2440_SIZE_Y;
	//}

	return iRtnP;
}

/* desc : Overlay 관련 함수 - Text List 추가 */
void CMilDisp::AddTextList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, CString fi_sText, int fi_color, int fi_iSizeX, int fi_iSizeY, CString fi_sFont, bool fi_bEgdeFlag)
{
	if (clTextList[fi_iDispType][fi_iNo].cnt < MAX_CNT_TEXT)
	{// Camera Disp
		clTextList[fi_iDispType][fi_iNo].p[clTextList[fi_iDispType][fi_iNo].cnt].x = fi_iX;
		clTextList[fi_iDispType][fi_iNo].p[clTextList[fi_iDispType][fi_iNo].cnt].y = fi_iY;
		sprintf_s(clTextList[fi_iDispType][fi_iNo].str[clTextList[fi_iDispType][fi_iNo].cnt], 1000, CT2A(fi_sText));
		fi_sText.ReleaseBuffer();

		clTextList[fi_iDispType][fi_iNo].c[clTextList[fi_iDispType][fi_iNo].cnt] = theApp.clMilMain.MilColorConvert(fi_color);
		clTextList[fi_iDispType][fi_iNo].size[clTextList[fi_iDispType][fi_iNo].cnt].x = fi_iSizeX;
		clTextList[fi_iDispType][fi_iNo].size[clTextList[fi_iDispType][fi_iNo].cnt].y = fi_iSizeY;
		sprintf_s(clTextList[fi_iDispType][fi_iNo].fontname[clTextList[fi_iDispType][fi_iNo].cnt], 100, CT2A(fi_sFont));
		fi_sFont.ReleaseBuffer();

		clTextList[fi_iDispType][fi_iNo].edgeFlag[clTextList[fi_iDispType][fi_iNo].cnt] = fi_bEgdeFlag;
		clTextList[fi_iDispType][fi_iNo].cnt++;
	}
}

/* desc : Overlay 관련 함수 - Cross List 추가 */
void CMilDisp::AddCrossList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, int fi_iWdt1, int fi_iWdt2, int fi_color)
{
	if (clCrossList[fi_iDispType][fi_iNo].cnt < MAX_CNT_CROSS)
	{// Camera Disp
		clCrossList[fi_iDispType][fi_iNo].c[clCrossList[fi_iDispType][fi_iNo].cnt] = theApp.clMilMain.MilColorConvert(fi_color);
		clCrossList[fi_iDispType][fi_iNo].p[clCrossList[fi_iDispType][fi_iNo].cnt].x = fi_iX;
		clCrossList[fi_iDispType][fi_iNo].p[clCrossList[fi_iDispType][fi_iNo].cnt].y = fi_iY;
		clCrossList[fi_iDispType][fi_iNo].w1[clCrossList[fi_iDispType][fi_iNo].cnt] = fi_iWdt1;
		clCrossList[fi_iDispType][fi_iNo].w2[clCrossList[fi_iDispType][fi_iNo].cnt] = fi_iWdt2;
		clCrossList[fi_iDispType][fi_iNo].cnt++;
	}
}

/* desc : Overlay 관련 함수 - Line List 추가 */
void CMilDisp::AddLineList(int fi_iDispType, int fi_iNo, int fi_iSx, int fi_iSy, int fi_iEx, int fi_iEy, int fi_iStyle, int fi_color)
{
	if (clLineList[fi_iDispType][fi_iNo].cnt < MAX_CNT_LINE)
	{// Camera Disp
		clLineList[fi_iDispType][fi_iNo].c[clLineList[fi_iDispType][fi_iNo].cnt] = theApp.clMilMain.MilColorConvert(fi_color);
		clLineList[fi_iDispType][fi_iNo].p[clLineList[fi_iDispType][fi_iNo].cnt][0].x = fi_iSx;
		clLineList[fi_iDispType][fi_iNo].p[clLineList[fi_iDispType][fi_iNo].cnt][0].y = fi_iSy;
		clLineList[fi_iDispType][fi_iNo].p[clLineList[fi_iDispType][fi_iNo].cnt][1].x = fi_iEx;
		clLineList[fi_iDispType][fi_iNo].p[clLineList[fi_iDispType][fi_iNo].cnt][1].y = fi_iEy;
		clLineList[fi_iDispType][fi_iNo].style[clLineList[fi_iDispType][fi_iNo].cnt] = fi_iStyle;
		clLineList[fi_iDispType][fi_iNo].cnt++;
	}
}

/* desc : Overlay 관련 함수 - Pixel List 추가 */
void CMilDisp::AddPixelList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, int fi_color)
{
	if (clPixelList[fi_iDispType][fi_iNo].cnt < MAX_CNT_PIXEL)
	{// Camera Disp
		clPixelList[fi_iDispType][fi_iNo].c[clPixelList[fi_iDispType][fi_iNo].cnt] = theApp.clMilMain.MilColorConvert(fi_color);
		clPixelList[fi_iDispType][fi_iNo].p[clPixelList[fi_iDispType][fi_iNo].cnt].x = fi_iX;
		clPixelList[fi_iDispType][fi_iNo].p[clPixelList[fi_iDispType][fi_iNo].cnt].y = fi_iY;
		clPixelList[fi_iDispType][fi_iNo].cnt++;
	}
}

/* desc : Overlay 관련 함수 - Box List 추가 */
void CMilDisp::AddBoxList(int fi_iDispType, int fi_iNo, int fi_iLeft, int fi_iTop, int fi_iRight, int fi_iBottom, int fi_iStyle, int fi_color)
{
	if (clBoxList[fi_iDispType][fi_iNo].cnt < MAX_CNT_BOX)
	{// Camera Disp
		clBoxList[fi_iDispType][fi_iNo].c[clBoxList[fi_iDispType][fi_iNo].cnt] = theApp.clMilMain.MilColorConvert(fi_color);
		clBoxList[fi_iDispType][fi_iNo].r[clBoxList[fi_iDispType][fi_iNo].cnt].left = fi_iLeft;
		clBoxList[fi_iDispType][fi_iNo].r[clBoxList[fi_iDispType][fi_iNo].cnt].right = fi_iRight;
		clBoxList[fi_iDispType][fi_iNo].r[clBoxList[fi_iDispType][fi_iNo].cnt].top = fi_iTop;
		clBoxList[fi_iDispType][fi_iNo].r[clBoxList[fi_iDispType][fi_iNo].cnt].bottom = fi_iBottom;
		clBoxList[fi_iDispType][fi_iNo].style[clBoxList[fi_iDispType][fi_iNo].cnt] = fi_iStyle;
		clBoxList[fi_iDispType][fi_iNo].cnt++;
	}
}

/* desc : Overlay 관련 함수 - Circle List 추가 */
void CMilDisp::AddCircleList(int fi_iDispType, int fi_iNo, int fi_iLeft, int fi_iTop, int fi_iRight, int fi_iBottom, int fi_color)
{
	if (clCircleList[fi_iDispType][fi_iNo].cnt < MAX_CNT_CIRCLE)
	{// Camera Disp
		clCircleList[fi_iDispType][fi_iNo].c[clCircleList[fi_iDispType][fi_iNo].cnt] = theApp.clMilMain.MilColorConvert(fi_color);
		clCircleList[fi_iDispType][fi_iNo].r[clCircleList[fi_iDispType][fi_iNo].cnt].left = fi_iLeft;
		clCircleList[fi_iDispType][fi_iNo].r[clCircleList[fi_iDispType][fi_iNo].cnt].top = fi_iTop;
		clCircleList[fi_iDispType][fi_iNo].r[clCircleList[fi_iDispType][fi_iNo].cnt].right = fi_iRight;
		clCircleList[fi_iDispType][fi_iNo].r[clCircleList[fi_iDispType][fi_iNo].cnt].bottom = fi_iBottom;
		//clCircleList[fi_iDispType][fi_iNo].c[clCircleList[fi_iDispType][fi_iNo].cnt] = fi_color;
		clCircleList[fi_iDispType][fi_iNo].cnt++;
	}
}

//이것저것 다 그려진 결과를 뒤집든 아님 그리기전에 뒤집든 할때 필요 
void CMilDisp::OverlayDC_Flip(MIL_ID fi_MilBuf, FlipDir dir)
{
#ifndef _NOT_USE_MIL_
	MbufControl(fi_MilBuf, M_DC_ALLOC, M_DEFAULT);
	HDC	OverlayDC = (HDC)MbufInquire(fi_MilBuf, M_DC_HANDLE, M_NULL);

	int iCnt;
	

	if (OverlayDC != M_NULL)
	{
		CDC NewDC;

		NewDC.Attach(OverlayDC);
		NewDC.SetBkMode(TRANSPARENT);
		NewDC.SelectStockObject(NULL_BRUSH);

		CRect rect;
		NewDC.GetClipBox(&rect); //이미지 크기 얻고 .

		CBitmap flippedBitmap;
		auto width = rect.right;
		auto height = rect.bottom;

		flippedBitmap.CreateCompatibleBitmap(&NewDC, rect.right, rect.bottom); // 이미지 생성하고 
		
		CDC flippedDC;
		flippedDC.CreateCompatibleDC(&NewDC);
		flippedDC.SelectObject(&flippedBitmap); //복사할 영역 만든다음에 
		flippedDC.SetStretchBltMode(HALFTONE); // 이미지의 품질을 높이기 위한 설정


		int srcX = dir == FlipDir::X ? width - 1 : 0;
		int srcY = dir == FlipDir::Y ? height - 1 : 0;
		int srcWidth = dir == FlipDir::X ? -width : width;
		int srcHeight = dir == FlipDir::Y ? -height : height;

		flippedDC.StretchBlt(0, 0, width, height, &NewDC, srcX, srcY, srcWidth, srcHeight, SRCCOPY);

		NewDC.BitBlt(rect.left, rect.top, width, height, &flippedDC, 0, 0, SRCCOPY);

		
		flippedDC.SelectObject(GetStockObject(NULL_BRUSH)); 
		flippedBitmap.DeleteObject(); // 비트맵 객체 해제
		flippedDC.DeleteDC(); // DC 객체 해제

		NewDC.DeleteDC();
		NewDC.Detach();
	}
	MbufControl(fi_MilBuf, M_DC_FREE, M_DEFAULT);
#endif
}

/* desc : Overlay 관련 함수 - Box List DC 출력 */
void CMilDisp::OverlayDC_Box(MIL_ID fi_MilBuf, int fi_iDispType, int fi_iNo, double fi_dRateX, double fi_dRateY)
{
#ifndef _NOT_USE_MIL_
	MbufControl(fi_MilBuf, M_DC_ALLOC, M_DEFAULT);
	HDC	OverlayDC = (HDC)MbufInquire(fi_MilBuf, M_DC_HANDLE, M_NULL);

	int iCnt;
	int x1, x2, y1, y2;
	unsigned long oc_red = 0L, oc_green = 150L, oc_blue = 0L;

	if (OverlayDC != M_NULL)
	{
		CDC NewDC;

		NewDC.Attach(OverlayDC);
		NewDC.SetBkMode(TRANSPARENT);
		NewDC.SelectStockObject(NULL_BRUSH);

		CPen     m_pen;
		CPen* m_penOld;

		for (iCnt = 0; iCnt < clBoxList[fi_iDispType][fi_iNo].cnt; iCnt++)
		{
			oc_red = (unsigned long)GetRValue(clBoxList[fi_iDispType][fi_iNo].c[iCnt]);
			oc_green = (unsigned long)GetGValue(clBoxList[fi_iDispType][fi_iNo].c[iCnt]);
			oc_blue = (unsigned long)GetBValue(clBoxList[fi_iDispType][fi_iNo].c[iCnt]);

			m_pen.CreatePen(clBoxList[fi_iDispType][fi_iNo].style[iCnt], 1, RGB(oc_red, oc_green, oc_blue));
			m_penOld = NewDC.SelectObject(&m_pen);

			x1 = (long)(clBoxList[fi_iDispType][fi_iNo].r[iCnt].left * fi_dRateX + 0.5);
			x2 = (long)(clBoxList[fi_iDispType][fi_iNo].r[iCnt].right * fi_dRateX + 0.5);
			y1 = (long)(clBoxList[fi_iDispType][fi_iNo].r[iCnt].top * fi_dRateY + 0.5);
			y2 = (long)(clBoxList[fi_iDispType][fi_iNo].r[iCnt].bottom * fi_dRateY + 0.5);

			NewDC.Rectangle(x1, y1, x2, y2);

			NewDC.SelectObject(m_penOld);
			m_pen.DeleteObject();
		}

		// Detach the device context.
		NewDC.Detach();
	}
	MbufControl(fi_MilBuf, M_DC_FREE, M_DEFAULT);
#endif
}

/* desc : Overlay 관련 함수 - Cross List DC 출력 */
void CMilDisp::OverlayDC_Cross(MIL_ID fi_MilBuf, int fi_iDispType, int fi_iNo, double fi_dRateX, double fi_dRateY)
{
#ifndef _NOT_USE_MIL_
	MbufControl(fi_MilBuf, M_DC_ALLOC, M_DEFAULT);
	HDC	OverlayDC = (HDC)MbufInquire(fi_MilBuf, M_DC_HANDLE, M_NULL);

	int iCnt;
	int x, y, w1, w2;
	unsigned long oc_red = 0L, oc_green = 150L, oc_blue = 0L;

	if (OverlayDC != M_NULL)
	{
		CDC NewDC;

		NewDC.Attach(OverlayDC);
		NewDC.SetBkMode(TRANSPARENT);
		NewDC.SelectStockObject(NULL_BRUSH);

		CPen     m_pen;
		CPen* m_penOld;

		for (iCnt = 0; iCnt < clCrossList[fi_iDispType][fi_iNo].cnt; iCnt++)
		{
			oc_red = (unsigned long)GetRValue(clCrossList[fi_iDispType][fi_iNo].c[iCnt]);
			oc_green = (unsigned long)GetGValue(clCrossList[fi_iDispType][fi_iNo].c[iCnt]);
			oc_blue = (unsigned long)GetBValue(clCrossList[fi_iDispType][fi_iNo].c[iCnt]);

			m_pen.CreatePen(PS_SOLID, 1, RGB(oc_red, oc_green, oc_blue));
			m_penOld = NewDC.SelectObject(&m_pen);

			w1 = (long)(clCrossList[fi_iDispType][fi_iNo].w1[iCnt] * fi_dRateX + 0.5);
			w2 = (long)(clCrossList[fi_iDispType][fi_iNo].w2[iCnt] * fi_dRateY + 0.5);
			x = (long)(clCrossList[fi_iDispType][fi_iNo].p[iCnt].x * fi_dRateX + 0.5);
			y = (long)(clCrossList[fi_iDispType][fi_iNo].p[iCnt].y * fi_dRateY + 0.5);

			NewDC.MoveTo(x - w1, y);
			NewDC.LineTo(x + w1, y);
			NewDC.MoveTo(x, y - w2);
			NewDC.LineTo(x, y + w2);

			NewDC.SelectObject(m_penOld);
			m_pen.DeleteObject();
		}

		// Detach the device context.
		NewDC.Detach();
	}
	MbufControl(fi_MilBuf, M_DC_FREE, M_DEFAULT);
#endif
}

/* desc : Overlay 관련 함수 - Circle List DC 출력 */
void CMilDisp::OverlayDC_Circle(MIL_ID fi_MilBuf, int fi_iDispType, int fi_iNo, double fi_dRateX, double fi_dRateY)
{
#ifndef _NOT_USE_MIL_
	MbufControl(fi_MilBuf, M_DC_ALLOC, M_DEFAULT);
	HDC OverlayDC = (HDC)MbufInquire(fi_MilBuf, M_DC_HANDLE, M_NULL);

	int iCnt;
	long x1, y1, x2, y2;
	unsigned long oc_red = 0L, oc_green = 150L, oc_blue = 0L;

	if (OverlayDC != M_NULL)
	{
		CDC NewDC;

		NewDC.Attach(OverlayDC);
		NewDC.SetBkMode(TRANSPARENT);
		NewDC.SelectStockObject(NULL_BRUSH);

		CPen     m_pen;
		CPen* m_penOld;

		for (iCnt = 0; iCnt < clCircleList[fi_iDispType][fi_iNo].cnt; iCnt++)
		{
			oc_red = (unsigned long)GetRValue(clCircleList[fi_iDispType][fi_iNo].c[iCnt]);
			oc_green = (unsigned long)GetGValue(clCircleList[fi_iDispType][fi_iNo].c[iCnt]);
			oc_blue = (unsigned long)GetBValue(clCircleList[fi_iDispType][fi_iNo].c[iCnt]);

			m_pen.CreatePen(PS_SOLID, 1, RGB(oc_red, oc_green, oc_blue));
			m_penOld = NewDC.SelectObject(&m_pen);

			x1 = (long)(clCircleList[fi_iDispType][fi_iNo].r[iCnt].left * fi_dRateX + 0.5);
			y1 = (long)(clCircleList[fi_iDispType][fi_iNo].r[iCnt].top * fi_dRateY + 0.5);
			x2 = (long)(clCircleList[fi_iDispType][fi_iNo].r[iCnt].right * fi_dRateX + 0.5);
			y2 = (long)(clCircleList[fi_iDispType][fi_iNo].r[iCnt].bottom * fi_dRateY + 0.5);

			NewDC.Ellipse(x1, y1, x2, y2);

			NewDC.SelectObject(m_penOld);
			m_pen.DeleteObject();
		}


		// Detach the device context.
		NewDC.Detach();
	}
	MbufControl(fi_MilBuf, M_DC_FREE, M_DEFAULT);
#endif
}

/* desc : Overlay 관련 함수 - Line List DC 출력 */
void CMilDisp::OverlayDC_Line(MIL_ID fi_MilBuf, int fi_iDispType, int fi_iNo, double fi_dRateX, double fi_dRateY)
{
#ifndef _NOT_USE_MIL_
	MbufControl(fi_MilBuf, M_DC_ALLOC, M_DEFAULT);
	HDC OverlayDC = (HDC)MbufInquire(fi_MilBuf, M_DC_HANDLE, M_NULL);

	int iCnt;
	int x1, x2, y1, y2;
	unsigned long oc_red = 0L, oc_green = 150L, oc_blue = 0L;

	if (OverlayDC != M_NULL)
	{
		CDC NewDC;

		NewDC.Attach(OverlayDC);
		NewDC.SetBkMode(TRANSPARENT);
		NewDC.SelectStockObject(NULL_BRUSH);

		CPen     m_pen;
		CPen* m_penOld;

		for (iCnt = 0; iCnt < clLineList[fi_iDispType][fi_iNo].cnt; iCnt++)
		{
			oc_red = (unsigned long)GetRValue(clLineList[fi_iDispType][fi_iNo].c[iCnt]);
			oc_green = (unsigned long)GetGValue(clLineList[fi_iDispType][fi_iNo].c[iCnt]);
			oc_blue = (unsigned long)GetBValue(clLineList[fi_iDispType][fi_iNo].c[iCnt]);

			m_pen.CreatePen(clLineList[fi_iDispType][fi_iNo].style[iCnt], 1, RGB(oc_red, oc_green, oc_blue));
			m_penOld = NewDC.SelectObject(&m_pen);

			x1 = (long)(clLineList[fi_iDispType][fi_iNo].p[iCnt][0].x * fi_dRateX);
			y1 = (long)(clLineList[fi_iDispType][fi_iNo].p[iCnt][0].y * fi_dRateY);
			x2 = (long)(clLineList[fi_iDispType][fi_iNo].p[iCnt][1].x * fi_dRateX);
			y2 = (long)(clLineList[fi_iDispType][fi_iNo].p[iCnt][1].y * fi_dRateY);

			NewDC.MoveTo(x1, y1);
			NewDC.LineTo(x2, y2);

			NewDC.SelectObject(m_penOld);
			m_pen.DeleteObject();
		}


		// Detach the device context.
		NewDC.Detach();
	}

	MbufControl(fi_MilBuf, M_DC_FREE, M_DEFAULT);
#endif
}

/* desc : Overlay 관련 함수 - Pixel List DC 출력 */
void CMilDisp::OverlayDC_Pixel(MIL_ID fi_MilBuf, int fi_iDispType, int fi_iNo, double fi_dRateX, double fi_dRateY)
{
#ifndef _NOT_USE_MIL_
	MbufControl(fi_MilBuf, M_DC_ALLOC, M_DEFAULT);
	HDC OverlayDC = (HDC)MbufInquire(fi_MilBuf, M_DC_HANDLE, M_NULL);

	int iCnt;
	int x, y;
	unsigned long oc_red = 0L, oc_green = 150L, oc_blue = 0L;

	if (OverlayDC != M_NULL)
	{
		CDC NewDC;

		NewDC.Attach(OverlayDC);
		NewDC.SetBkMode(TRANSPARENT);
		CBrush* pOldBrush = (CBrush*)NewDC.SelectStockObject(NULL_BRUSH);

		CPen     m_pen;
		CPen* m_penOld; 	                     //CPEN variables

		for (iCnt = 0; iCnt < clPixelList[fi_iDispType][fi_iNo].cnt; iCnt++)
		{
			oc_red = (unsigned long)GetRValue(clPixelList[fi_iDispType][fi_iNo].c[iCnt]);
			oc_green = (unsigned long)GetGValue(clPixelList[fi_iDispType][fi_iNo].c[iCnt]);
			oc_blue = (unsigned long)GetBValue(clPixelList[fi_iDispType][fi_iNo].c[iCnt]);

			m_pen.CreatePen(PS_SOLID, 1, RGB(oc_red, oc_green, oc_blue));
			m_penOld = NewDC.SelectObject(&m_pen);

			x = (long)(clPixelList[fi_iDispType][fi_iNo].p[iCnt].x * fi_dRateX + 0.5);
			y = (long)(clPixelList[fi_iDispType][fi_iNo].p[iCnt].y * fi_dRateY + 0.5);

			NewDC.SetPixel(x, y, RGB(oc_red, oc_green, oc_blue));

			NewDC.SelectObject(m_penOld);
			NewDC.SelectObject(pOldBrush);
			m_pen.DeleteObject();
		}


		// Detach the device context.
		NewDC.Detach();
	}
	MbufControl(fi_MilBuf, M_DC_FREE, M_DEFAULT);
#endif
}

/* desc : Overlay 관련 함수 - Text List DC 출력 */
void CMilDisp::OverlayDC_Text(MIL_ID fi_MilBuf, int fi_iDispType, int fi_iNo, double fi_dRateX, double fi_dRateY)
{
#ifndef _NOT_USE_MIL_
	MbufControl(fi_MilBuf, M_DC_ALLOC, M_DEFAULT);
	HDC OverlayDC = (HDC)MbufInquire(fi_MilBuf, M_DC_HANDLE, M_NULL);

	int iCnt;
	int x, y;
	unsigned long oc_red = 0L, oc_green = 150L, oc_blue = 0L;
	TCHAR tmpChar[256];
	memset(tmpChar, 0, sizeof(char) * 256);

	int len = 1000;

	if (OverlayDC != M_NULL)
	{
		CDC NewDC;

		NewDC.Attach(OverlayDC);
		NewDC.SetBkMode(TRANSPARENT);
		CBrush* pOldBrush = (CBrush*)NewDC.SelectStockObject(NULL_BRUSH);

		CFont font, * oldfont;

		for (iCnt = 0; iCnt < clTextList[fi_iDispType][fi_iNo].cnt; iCnt++)
		{
			font.CreateFont(clTextList[fi_iDispType][fi_iNo].size[iCnt].y, clTextList[fi_iDispType][fi_iNo].size[iCnt].x, 0, 0,
				FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, CA2T(clTextList[fi_iDispType][fi_iNo].fontname[iCnt]));

			oldfont = NewDC.SelectObject(&font);

			oc_red = (unsigned long)GetRValue(clTextList[fi_iDispType][fi_iNo].c[iCnt]);
			oc_green = (unsigned long)GetGValue(clTextList[fi_iDispType][fi_iNo].c[iCnt]);
			oc_blue = (unsigned long)GetBValue(clTextList[fi_iDispType][fi_iNo].c[iCnt]);

			x = (long)(clTextList[fi_iDispType][fi_iNo].p[iCnt].x * fi_dRateX + 0.5);
			y = (long)(clTextList[fi_iDispType][fi_iNo].p[iCnt].y * fi_dRateY + 0.5);

			memset(tmpChar, 0, sizeof(char) * 256);
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, clTextList[fi_iDispType][fi_iNo].str[iCnt], strlen(clTextList[fi_iDispType][fi_iNo].str[iCnt]), tmpChar, 256);

			if (clTextList[fi_iDispType][fi_iNo].edgeFlag[iCnt])
			{
				for (int a = -1; a <= 1; a++)
				{
					for (int b = -1; b <= 1; b++)
					{
						NewDC.SetTextColor(RGB(0, 0, 0));
						NewDC.TextOutW(x + a, y + b, tmpChar, (MIL_INT32)strlen(clTextList[fi_iDispType][fi_iNo].str[iCnt]));
						NewDC.TextOutW(x + a, y + b, tmpChar);
					}
				}
			}

			NewDC.SetTextColor(RGB(oc_red, oc_green, oc_blue));
			NewDC.TextOut(x, y, tmpChar, (MIL_INT32)MosStrlen(tmpChar));

			NewDC.SelectObject(oldfont);
			font.DeleteObject();
		}
		// Detach the device context.
		NewDC.Detach();
	}
	MbufControl(fi_MilBuf, M_DC_FREE, M_DEFAULT);
#endif
}


/*
	desc : Overlay 관련 함수 - 전체 Overlay Clear or DC 출력
	fi_bDrawFlag : FALSE - CLEAR
	fi_iDispType : 0:Expo, 1:mark, 2 : Live, 3 : mark set
	fi_iNo : Cam Num 혹은 Grab Mark Num (각자 Disp Type 에 맞춰서 사용)
*/


void CMilDisp::ClearShapes(int fi_iDispType)
{
	
	for(int i=0;i< PART_CNT;i++)
	clBoxList[fi_iDispType][i].cnt = clCrossList[fi_iDispType][i].cnt = clPixelList[fi_iDispType][i].cnt
	= clCircleList[fi_iDispType][i].cnt = clLineList[fi_iDispType][i].cnt = clTextList[fi_iDispType][i].cnt = 0;
}

void CMilDisp::DrawOverlayDC(bool fi_bDrawFlag, int fi_iDispType, int fi_iNo)
{
	MIL_ID	MilBufTmp[2];
	CDPoint	rateP;
	int repeat = 1;

	if (fi_iDispType == DISP_TYPE_CALB_EXPO) {
		repeat = 2; // lk91 UI 만들어지면 CAM 3수량으로 변경 필요
		fi_iNo = 0;
	}

	if (!fi_bDrawFlag)
	{
		for (int i = 0; i < repeat; i++) {
			clBoxList[fi_iDispType][fi_iNo + i].cnt = clCrossList[fi_iDispType][fi_iNo + i].cnt = clPixelList[fi_iDispType][fi_iNo + i].cnt 
				= clCircleList[fi_iDispType][fi_iNo + i].cnt = clLineList[fi_iDispType][fi_iNo + i].cnt = clTextList[fi_iDispType][fi_iNo + i].cnt = 0;
		}
		if(fi_iDispType != 1)
			DrawBase(fi_iDispType, fi_iNo);
	}
	rateP = GetRateDispToBuf( fi_iDispType);	
	
#ifndef _NOT_USE_MIL_
	if (fi_iDispType == DISP_TYPE_EXPO) { // expo
		MilBufTmp[0] = theApp.clMilMain.m_mOverlay_EXPO[fi_iNo];
		MdispControl(theApp.clMilMain.m_mDisID_EXPO[fi_iNo], M_OVERLAY_CLEAR, M_DEFAULT);
	}
	else if (fi_iDispType == DISP_TYPE_MARK) { // mark
		MilBufTmp[0] = theApp.clMilMain.m_mOverlay_Mark[fi_iNo];
		MdispControl(theApp.clMilMain.m_mDisID_Mark[fi_iNo], M_OVERLAY_CLEAR, M_DEFAULT);
	}
	else if (fi_iDispType == DISP_TYPE_MARK_LIVE) { // live
		MilBufTmp[0] = theApp.clMilMain.m_mOverlay_Mark_Live;
		MdispControl(theApp.clMilMain.m_mDisID_Mark_Live, M_OVERLAY_CLEAR, M_DEFAULT);
	}
	else if (fi_iDispType == DISP_TYPE_MARKSET) { // mark set
		MilBufTmp[0] = theApp.clMilMain.m_mOverlay_MarkSet;
		MdispControl(theApp.clMilMain.m_mDisID_MarkSet, M_OVERLAY_CLEAR, M_DEFAULT);
	}
	else if (fi_iDispType == DISP_TYPE_MMPM) { // MMPM
		MilBufTmp[0] = theApp.clMilMain.m_mOverlay_MMPM;
		MdispControl(theApp.clMilMain.m_mDisID_MMPM, M_OVERLAY_CLEAR, M_DEFAULT);
	}
	else if (fi_iDispType == DISP_TYPE_CALB_CAMSPEC) {
		MilBufTmp[0] = theApp.clMilMain.m_mOverlay_CALI_CAMSPEC;
		MdispControl(theApp.clMilMain.m_mDisID_CALI_CAMSPEC, M_OVERLAY_CLEAR, M_DEFAULT);
	}
	else if (fi_iDispType == DISP_TYPE_CALB_EXPO) { // lk91 UI 변경되면 CAM 수량 3개로 변경 (for 문 적용)
		MilBufTmp[0] = theApp.clMilMain.m_mOverlay_CALI_STEP[0];
		MilBufTmp[1] = theApp.clMilMain.m_mOverlay_CALI_STEP[1];
		MdispControl(theApp.clMilMain.m_mDisID_CALI_STEP[0], M_OVERLAY_CLEAR, M_DEFAULT);
		MdispControl(theApp.clMilMain.m_mDisID_CALI_STEP[1], M_OVERLAY_CLEAR, M_DEFAULT);
	}
	else if (fi_iDispType == DISP_TYPE_CALB_ACCR) {
		MilBufTmp[0] = theApp.clMilMain.m_mOverlay_ACCR;
		MdispControl(theApp.clMilMain.m_mDisID_ACCR, M_OVERLAY_CLEAR, M_DEFAULT);

	}
	else if (fi_iDispType == DISP_TYPE_MMPM_AUTOCENTER) { 
		MilBufTmp[0] = theApp.clMilMain.m_mOverlay_MPMM_AutoCenter;
		MdispControl(theApp.clMilMain.m_mDisID_MPMM_AutoCenter, M_OVERLAY_CLEAR, M_DEFAULT);
	}
	else
		return;

	for (int i = 0; i < repeat; i++) {
		OverlayDC_Box(MilBufTmp[i], fi_iDispType, fi_iNo + i, rateP.x, rateP.y);
		OverlayDC_Cross(MilBufTmp[i], fi_iDispType, fi_iNo + i, rateP.x, rateP.y);
		OverlayDC_Circle(MilBufTmp[i], fi_iDispType, fi_iNo + i, rateP.x, rateP.y);
		OverlayDC_Line(MilBufTmp[i], fi_iDispType, fi_iNo + i, rateP.x, rateP.y);
		OverlayDC_Pixel(MilBufTmp[i], fi_iDispType, fi_iNo + i, rateP.x, rateP.y);
		MbufControl(MilBufTmp[i], M_MODIFIED, M_DEFAULT);
		
	}

	if (fi_iDispType == DISP_TYPE_MARK_LIVE)
	{
		OverlayDC_Flip(theApp.clMilMain.m_mOverlay_Mark_Live, FlipDir::X);
		MbufControl(theApp.clMilMain.m_mOverlay_Mark_Live, M_MODIFIED, M_DEFAULT);
	}

	for (int i = 0; i < repeat; i++) {
		
		OverlayDC_Text(MilBufTmp[i], fi_iDispType, fi_iNo + i, rateP.x, rateP.y);
		MbufControl(MilBufTmp[i], M_MODIFIED, M_DEFAULT);
	}
	
	

#endif
	return;
}

/* desc : Overlay 관련 함수 - Base Overlay */
void CMilDisp::DrawBase(int fi_iDispType, int fi_iNo)
{
	CString sTmp;

	CPoint		iOrgSizeP;
	//iOrgSizeP = GetOrgImgSize(fi_iNo);
	if (fi_iDispType == DISP_TYPE_CALB_CAMSPEC) {
		iOrgSizeP.x = theApp.clMilMain.rectSearhROI_Calb_CamSpec.right - theApp.clMilMain.rectSearhROI_Calb_CamSpec.left;
		iOrgSizeP.y = theApp.clMilMain.rectSearhROI_Calb_CamSpec.bottom - theApp.clMilMain.rectSearhROI_Calb_CamSpec.top;
	}
	else if (fi_iDispType == DISP_TYPE_CALB_EXPO) {
		iOrgSizeP.x = theApp.clMilMain.rectSearhROI_Calb_Step.right - theApp.clMilMain.rectSearhROI_Calb_Step.left;
		iOrgSizeP.y = theApp.clMilMain.rectSearhROI_Calb_Step.bottom - theApp.clMilMain.rectSearhROI_Calb_Step.top;
	}
	else if (fi_iDispType == DISP_TYPE_CALB_ACCR) {
		iOrgSizeP.x = theApp.clMilMain.rectSearhROI_Calb_Accr.right - theApp.clMilMain.rectSearhROI_Calb_Accr.left;
		iOrgSizeP.y = theApp.clMilMain.rectSearhROI_Calb_Accr.bottom - theApp.clMilMain.rectSearhROI_Calb_Accr.top;
	}
	else {
		iOrgSizeP.x = ACA1920_SIZE_X;
		iOrgSizeP.y = ACA1920_SIZE_Y;
	}
	int posX[4];
	int posY[4];
	//int SX_X, SY_X, EX_X, EY_X;
	//int SX_Y, SY_Y, EX_Y, EY_Y;
	//int nSize, nStartX, nStartY;

	posX[0] = iOrgSizeP.x - 50;
	posX[1] = iOrgSizeP.x - 80;
	posX[2] = (iOrgSizeP.x >> 1) - 50;
	posX[3] = (iOrgSizeP.x >> 1) - 80;

	posY[0] = iOrgSizeP.y - 30;
	posY[1] = iOrgSizeP.y - 50;
	posY[2] = (iOrgSizeP.y >> 1) + 30;
	posY[3] = (iOrgSizeP.y >> 1) + 50;

	//--- 영상 Center 그리기
	if (fi_iDispType == DISP_TYPE_CALB_EXPO) {
		AddLineList(fi_iDispType, 0, iOrgSizeP.x >> 1, 0, iOrgSizeP.x >> 1, iOrgSizeP.y, PS_SOLID, eM_COLOR_YELLOW);
		AddLineList(fi_iDispType, 0, 0, iOrgSizeP.y >> 1, iOrgSizeP.x, iOrgSizeP.y >> 1, PS_SOLID, eM_COLOR_YELLOW);
		AddLineList(fi_iDispType, 1, iOrgSizeP.x >> 1, 0, iOrgSizeP.x >> 1, iOrgSizeP.y, PS_SOLID, eM_COLOR_YELLOW);
		AddLineList(fi_iDispType, 1, 0, iOrgSizeP.y >> 1, iOrgSizeP.x, iOrgSizeP.y >> 1, PS_SOLID, eM_COLOR_YELLOW);
	}
	else {
		AddLineList(fi_iDispType, fi_iNo, iOrgSizeP.x >> 1, 0, iOrgSizeP.x >> 1, iOrgSizeP.y, PS_SOLID, eM_COLOR_YELLOW);
		AddLineList(fi_iDispType, fi_iNo, 0, iOrgSizeP.y >> 1, iOrgSizeP.x, iOrgSizeP.y >> 1, PS_SOLID, eM_COLOR_YELLOW);
	}

	if (fi_iDispType == DISP_TYPE_MARK_LIVE) { //LIVE
		if (fi_iNo == 0)				AddTextList(fi_iDispType, fi_iNo, 10, 5, _T("CAM 1"), eM_COLOR_WHITE, 10, 25, VISION_FONT_TEXT, true);
		else if (fi_iNo == 1)		AddTextList(fi_iDispType, fi_iNo, 10, 5, _T("CAM 2"), eM_COLOR_WHITE, 10, 25, VISION_FONT_TEXT, true);
		else if (fi_iNo == 2)		AddTextList(fi_iDispType, fi_iNo, 10, 5, _T("CAM 3"), eM_COLOR_WHITE, 10, 25, VISION_FONT_TEXT, true);

		//--- Search Area 그리기
		AddBoxList(fi_iDispType, fi_iNo, 
			theApp.clMilMain.rectSearhROI[fi_iNo].left, theApp.clMilMain.rectSearhROI[fi_iNo].top,
			theApp.clMilMain.rectSearhROI[fi_iNo].right, theApp.clMilMain.rectSearhROI[fi_iNo].bottom, PS_SOLID, eM_COLOR_BLUE);
		CString sTmp;
		sTmp.Format(_T("Search ROI #%d"), fi_iNo + 1);
		AddTextList(fi_iDispType, fi_iNo, theApp.clMilMain.rectSearhROI[fi_iNo].right - 50, 
			theApp.clMilMain.rectSearhROI[fi_iNo].bottom, sTmp, eM_COLOR_BLUE, 6, 12, VISION_FONT_TEXT, true);
	}
	else if (fi_iDispType == DISP_TYPE_MMPM) { // MMPM
		//if (fi_iNo == 0)				AddTextList(fi_iDispType, fi_iNo, 10, 5, _T("CAM 1"), eM_COLOR_WHITE, 10, 25, VISION_FONT_TEXT, true);
		//else if (fi_iNo == 1)		AddTextList(fi_iDispType, fi_iNo, 10, 5, _T("CAM 2"), eM_COLOR_WHITE, 10, 25, VISION_FONT_TEXT, true);
		//else if (fi_iNo == 2)		AddTextList(fi_iDispType, fi_iNo, 10, 5, _T("CAM 3"), eM_COLOR_WHITE, 10, 25, VISION_FONT_TEXT, true);
	}
	else if (fi_iDispType == DISP_TYPE_CALB_CAMSPEC) { 
		if (fi_iNo == 0)				AddTextList(fi_iDispType, fi_iNo, 10, 5, _T("CAM 1"), eM_COLOR_WHITE, 10, 25, VISION_FONT_TEXT, true);
		else if (fi_iNo == 1)		AddTextList(fi_iDispType, fi_iNo, 10, 5, _T("CAM 2"), eM_COLOR_WHITE, 10, 25, VISION_FONT_TEXT, true);
		//else if (fi_iNo == 2)		AddTextList(fi_iDispType, fi_iNo, 10, 5, _T("CAM 3"), eM_COLOR_WHITE, 10, 25, VISION_FONT_TEXT, true);
	}
	else if (fi_iDispType == DISP_TYPE_CALB_EXPO) { 
		AddTextList(fi_iDispType, 0, 10, 5, _T("MARK 1"), eM_COLOR_WHITE, 10, 25, VISION_FONT_TEXT, true);
		AddTextList(fi_iDispType, 1, 10, 5, _T("MARK 2"), eM_COLOR_WHITE, 10, 25, VISION_FONT_TEXT, true);
		//AddTextList(fi_iDispType, 2, 10, 5, _T("MARK 3"), eM_COLOR_WHITE, 10, 25, VISION_FONT_TEXT, true); // lk91 cam 3개 적용시 추가(for문)
	}
	else if (fi_iDispType == DISP_TYPE_CALB_ACCR) {
		if (fi_iNo == 0)				AddTextList(fi_iDispType, fi_iNo, 10, 5, _T("CAM 1"), eM_COLOR_WHITE, 10, 25, VISION_FONT_TEXT, true);
		else if (fi_iNo == 1)		AddTextList(fi_iDispType, fi_iNo, 10, 5, _T("CAM 2"), eM_COLOR_WHITE, 10, 25, VISION_FONT_TEXT, true);
		//else if (fi_iNo == 2)		AddTextList(fi_iDispType, fi_iNo, 10, 5, _T("CAM 3"), eM_COLOR_WHITE, 10, 25, VISION_FONT_TEXT, true);
	}
}

/* desc : Zoom 관련 함수 - Zoom In */
void CMilDisp::MilZoomIn(int fi_iDispType, int cam_id, CRect rc)
{
	//if (fi_iDispType == 2) // live
	//{
		float fZoomX = m_fZoomX[cam_id];
		float fZoomY = m_fZoomY[cam_id];

		fZoomX = (float)(fZoomX + 0.5);
		fZoomY = (float)(fZoomY + 0.5);

		dZoomX = fZoomX;
		dZoomY = fZoomY;

		MilSetZoomFactor(fi_iDispType, cam_id, dZoomX, dZoomY, rc);
	//}
	//else if (fi_iDispType == 4) // MMPM
	//{
	//	float fZoomX = m_fZoomX[cam_id];
	//	float fZoomY = m_fZoomY[cam_id];

	//	fZoomX = (float)(fZoomX + 0.5);
	//	fZoomY = (float)(fZoomY + 0.5);

	//	dZoomX = fZoomX;
	//	dZoomY = fZoomY;

	//	MilSetZoomFactor(fi_iDispType, cam_id, dZoomX, dZoomY, rc);
	//}
}

/* desc : Zoom 관련 함수 - Zoom Out */
BOOL CMilDisp::MilZoomOut(int fi_iDispType, int cam_id, CRect rc)
{
	//if (fi_iDispType == 2) {
		float fZoomX = m_fZoomX[cam_id];
		float fZoomY = m_fZoomY[cam_id];

		fZoomX = (float)(fZoomX - 0.5);
		fZoomY = (float)(fZoomY - 0.5);

		dZoomX = fZoomX;
		dZoomY = fZoomY;

		return MilSetZoomFactor(fi_iDispType, cam_id, dZoomX, dZoomY, rc);
	//}
}

/* desc : Zoom 관련 함수 - Zoom Fit, 이미지 비율 1.0 */
void CMilDisp::MilAutoScale(int fi_iDispType, int cam_id)
{
#ifndef _NOT_USE_MIL_
	MIL_ID DispID;
	if (fi_iDispType == 2)
	{
		DispID = theApp.clMilMain.m_mDisID_Mark_Live;
	}
	else {
		DispID = theApp.clMilMain.m_mDisID_MMPM;
	}
#endif
	//if (fi_iDispType == 2)
	//{
		dZoomX = m_fZoomX[cam_id] = 1;
		dZoomY = m_fZoomY[cam_id] = 1;

		m_fFixZoomX = m_fZoomX[cam_id];
		m_fFixZoomY = m_fZoomY[cam_id];

		m_fPanX[cam_id] = 0;
		m_fPanY[cam_id] = 0;

#ifndef _NOT_USE_MIL_
		MdispPan(DispID, 0, 0);
		MdispZoom(DispID, m_fZoomX[cam_id], m_fZoomY[cam_id]);
#endif
	//}
}

/* desc : Zoom 관련 함수 - 계산된 비율로 Zoom 동작 실행 */
BOOL CMilDisp::MilSetZoomFactor(int fi_iDispType, int cam_id, float fZoomX, float fZoomY, CRect rc)
{
	CDPoint dRtnP;
#ifndef _NOT_USE_MIL_
	MIL_ID DispID;
	if (fi_iDispType == 2)
	{
		DispID = theApp.clMilMain.m_mDisID_Mark_Live;
		dRtnP.x = (double)theApp.clMilMain.LIVE_DISP_SIZE_X;
		dRtnP.y = (double)theApp.clMilMain.LIVE_DISP_SIZE_Y;
	}
	//else if (fi_iDispType == 4) {
	else{
		DispID = theApp.clMilMain.m_mDisID_MMPM;
		dRtnP.x = (double)theApp.clMilMain.MMPM_DISP_SIZE_X;
		dRtnP.y = (double)theApp.clMilMain.MMPM_DISP_SIZE_Y;
	}
#endif

	if (fZoomX <= m_fFixZoomX || fZoomY <= m_fFixZoomY) {
		m_fZoomX[cam_id] = m_fFixZoomX;
		m_fZoomY[cam_id] = m_fFixZoomY;
		m_fPanX[cam_id] = 0;
		m_fPanY[cam_id] = 0;
#ifndef _NOT_USE_MIL_
		MdispZoom(DispID, m_fZoomX[cam_id], m_fZoomY[cam_id]);
		MdispPan(DispID, 0, 0);
#endif
		MilAutoScale(fi_iDispType, cam_id);
		return false;
	}

	m_fZoomX[cam_id] = fZoomX;
	m_fZoomY[cam_id] = fZoomY;

#ifndef _NOT_USE_MIL_
	MdispZoom(DispID, M_OVERLAY_CLEAR, M_DEFAULT);
	MdispZoom(DispID, m_fZoomX[cam_id], m_fZoomY[cam_id]);

	LONG lRectPixelX = (LONG)(rc.Width() / m_fZoomX[cam_id]);
	LONG lRectPixelY = (LONG)(rc.Height() / m_fZoomY[cam_id]);
	if ((dRtnP.x - m_fPanX[cam_id]) < lRectPixelX) {
		m_fPanX[cam_id] = dRtnP.x - lRectPixelX;
		MdispPan(theApp.clMilMain.m_mDisID_Mark_Live, m_fPanX[cam_id], m_fPanY[cam_id]);
	}
	if ((dRtnP.y - m_fPanY[cam_id]) < lRectPixelY) {
		m_fPanY[cam_id] = dRtnP.y - lRectPixelY;
		MdispPan(theApp.clMilMain.m_mDisID_Mark_Live, m_fPanX[cam_id], m_fPanY[cam_id]);
	}
#endif
	return true;
	

//	if (fi_iDispType == 2) 
//	{
//		dRtnP.x = (double)theApp.clMilMain.LIVE_DISP_SIZE_X;
//		dRtnP.y = (double)theApp.clMilMain.LIVE_DISP_SIZE_Y;
//
//		if (fZoomX <= m_fFixZoomX || fZoomY <= m_fFixZoomY) {
//			m_fZoomX[cam_id] = m_fFixZoomX;
//			m_fZoomY[cam_id] = m_fFixZoomY;
//			m_fPanX[cam_id] = 0;
//			m_fPanY[cam_id] = 0;
//#ifndef _NOT_USE_MIL_
//			MdispZoom(theApp.clMilMain.m_mDisID_Mark_Live, m_fZoomX[cam_id], m_fZoomY[cam_id]);
//			MdispPan(theApp.clMilMain.m_mDisID_Mark_Live, 0, 0);
//#endif
//			MilAutoScale(fi_iDispType, cam_id);
//			return false;
//		}
//
//		m_fZoomX[cam_id] = fZoomX;
//		m_fZoomY[cam_id] = fZoomY;
//
//#ifndef _NOT_USE_MIL_
//		MdispZoom(theApp.clMilMain.m_mDisID_Mark_Live, M_OVERLAY_CLEAR, M_DEFAULT);
//		MdispZoom(theApp.clMilMain.m_mDisID_Mark_Live, m_fZoomX[cam_id], m_fZoomY[cam_id]);
//
//		LONG lRectPixelX = (LONG)(rc.Width() / m_fZoomX[cam_id]);
//		LONG lRectPixelY = (LONG)(rc.Height() / m_fZoomY[cam_id]);
//		if ((dRtnP.x - m_fPanX[cam_id]) < lRectPixelX) {
//			m_fPanX[cam_id] = dRtnP.x - lRectPixelX;
//			MdispPan(theApp.clMilMain.m_mDisID_Mark_Live, m_fPanX[cam_id], m_fPanY[cam_id]);
//		}
//		if ((dRtnP.y - m_fPanY[cam_id]) < lRectPixelY) {
//			m_fPanY[cam_id] = dRtnP.y - lRectPixelY;
//			MdispPan(theApp.clMilMain.m_mDisID_Mark_Live, m_fPanX[cam_id], m_fPanY[cam_id]);
//		}
//#endif
//		return true;
//	}
}

/* desc : Zoom 관련 함수 - Mouse Point 저장, Zoom으로 확대되었을 때 이미지 이동하기 위함 */
void CMilDisp::SetZoomDownP(int fi_iDispType, int cam_id, CPoint fi_point)
{
	//if (fi_iDispType == 2)
	//{
		ZoomDownP = fi_point;
	//}
}

/* desc : Zoom 관련 함수 - 이미지 이동 */
void CMilDisp::MoveZoomDisp(int fi_iDispType, int cam_id, CPoint fi_point, CRect fi_rect)
{
	CDPoint dRtnP;

#ifndef _NOT_USE_MIL_
	MIL_ID DispID;
	if (fi_iDispType == 2) // LIVE
	{
		DispID = theApp.clMilMain.m_mDisID_Mark_Live;
		dRtnP.x = (double)theApp.clMilMain.LIVE_DISP_SIZE_X;
		dRtnP.y = (double)theApp.clMilMain.LIVE_DISP_SIZE_Y;
	}
	else { // MMPM
		DispID = theApp.clMilMain.m_mDisID_MMPM;
		dRtnP.x = (double)theApp.clMilMain.MMPM_DISP_SIZE_X;
		dRtnP.y = (double)theApp.clMilMain.MMPM_DISP_SIZE_Y;
	}
#endif

	int nX = 0, nY = 0;
	CPoint	iTmpP;
	iTmpP.x = fi_point.x;
	iTmpP.y = fi_point.y;

	nX = (int)(fi_point.x / dZoomX);
	nY = (int)(fi_point.y / dZoomY);

	if ((m_fZoomX[cam_id] > m_fFixZoomX && m_fZoomY[cam_id] > m_fFixZoomY))
	{
		int nMoveX = ZoomDownP.x - iTmpP.x;
		int nMoveY = ZoomDownP.y - iTmpP.y;

		int nPX = (int)(nMoveX * 2 / m_fZoomX[cam_id]);
		int nPY = (int)(nMoveY * 2 / m_fZoomY[cam_id]);
		int nViewSizeX = (int)(fi_rect.Width() / m_fZoomX[cam_id]);
		int nViewSizeY = (int)(fi_rect.Height() / m_fZoomY[cam_id]);

		if ((m_fPanX[cam_id] + nPX) < 0)
		{
			m_fPanX[cam_id] = 0;
		}
		else if ((m_fPanX[cam_id] + nPX + nViewSizeX) > (dRtnP.x))
		{
			m_fPanX[cam_id] = dRtnP.x - nViewSizeX;
		}
		else {
			m_fPanX[cam_id] = m_fPanX[cam_id] + nPX;
			ZoomDownP.x = iTmpP.x;
		}

		if ((m_fPanY[cam_id] + nPY) < 0)
		{
			m_fPanY[cam_id] = 0;
		}
		else if ((m_fPanY[cam_id] + nPY + nViewSizeY) > (dRtnP.y))
		{
			m_fPanY[cam_id] = dRtnP.y - nViewSizeY;
		}
		else {
			m_fPanY[cam_id] = m_fPanY[cam_id] + nPY;
			ZoomDownP.y = iTmpP.y;
		}

#ifndef _NOT_USE_MIL_
		MdispPan(DispID, m_fPanX[cam_id], m_fPanY[cam_id]);
#endif
	}
	

//	if (fi_iDispType == 2)
//	{
//		CDPoint dRtnP;
//
//		dRtnP.x = (double)theApp.clMilMain.LIVE_DISP_SIZE_X;
//		dRtnP.y = (double)theApp.clMilMain.LIVE_DISP_SIZE_Y;
//
//		int nX = 0, nY = 0;
//		CPoint	iTmpP;
//		iTmpP.x = fi_point.x;
//		iTmpP.y = fi_point.y;
//
//		nX = (int)(fi_point.x / dZoomX);// *dRateP.x + 0.5);
//		//nY = (int)(fi_point.x / dZoomX);// *dRateP.y + 0.5);
//		nY = (int)(fi_point.y / dZoomY);// *dRateP.y + 0.5);
//
//		//if (nX < 0 || nX > dRtnP.x - 1) return;
//		//if (nY < 0 || nY > dRtnP.y - 1) return;
//
//		//if (nX < 0 || nX > dRtnP.x - 1)
//		//	iTmpP.x = (dRtnP.x - 1)*dZoomX;
//		//if (nY < 0 || nY > dRtnP.y - 1)
//		//	iTmpP.y = (dRtnP.y - 1) * dZoomY;
//
//		if ((m_fZoomX[cam_id] > m_fFixZoomX && m_fZoomY[cam_id] > m_fFixZoomY))
//		{
//			int nMoveX = ZoomDownP.x - iTmpP.x;
//			int nMoveY = ZoomDownP.y - iTmpP.y;
//
//			int nPX = (int)(nMoveX * 2 / m_fZoomX[cam_id]);
//			int nPY = (int)(nMoveY * 2 / m_fZoomY[cam_id]);
//			int nViewSizeX = (int)(fi_rect.Width() / m_fZoomX[cam_id]);
//			int nViewSizeY = (int)(fi_rect.Height() / m_fZoomY[cam_id]);
//
//			if ((m_fPanX[cam_id] + nPX) < 0)
//			{
//				m_fPanX[cam_id] = 0;
//				//ZoomDownP.x = iTmpP.x; // 확인해보기
//			}			
//			else if ((m_fPanX[cam_id] + nPX + nViewSizeX) > (dRtnP.x))
//			{
//				m_fPanX[cam_id] = dRtnP.x - nViewSizeX;
//				//ZoomDownP.x = iTmpP.x;
//			}
//			else {
//				m_fPanX[cam_id] = m_fPanX[cam_id] + nPX;
//				ZoomDownP.x = iTmpP.x;
//			}
//
//			if ((m_fPanY[cam_id] + nPY) < 0)
//			{
//				m_fPanY[cam_id] = 0;
//				//ZoomDownP.y = iTmpP.y;// 확인해보기
//			}
//			else if ((m_fPanY[cam_id] + nPY + nViewSizeY) > (dRtnP.y))
//			{
//				m_fPanY[cam_id] = dRtnP.y - nViewSizeY;
//				//ZoomDownP.y = iTmpP.y;
//			}
//			else {
//				m_fPanY[cam_id] = m_fPanY[cam_id] + nPY;
//				ZoomDownP.y = iTmpP.y;
//			}
//
//			//if ((m_fPanX[cam_id] + nPX) >= 0 && (m_fPanX[cam_id] + nPX + nViewSizeX) < (dRtnP.x) &&
//			//	(m_fPanY[cam_id] + nPY) >= 0 && (m_fPanY[cam_id] + nPY + nViewSizeY) < (dRtnP.y))
//			//{
//#ifndef _NOT_USE_MIL_
//				MdispPan(theApp.clMilMain.m_mDisID_Mark_Live, m_fPanX[cam_id], m_fPanY[cam_id]);
//#endif
//				//m_fPanX[cam_id] = m_fPanX[cam_id] + nPX;
//				//m_fPanY[cam_id] = m_fPanY[cam_id] + nPY;
//				//ZoomDownP = iTmpP;
//			//}
//		}
//	}
}

/* desc : Zoom 관련 함수 - Zoom 된 영상에서 클릭된 좌표를 이미지 좌표로 변환시켜주는 작업 */
CDPoint CMilDisp::trZoomPoint(int fi_iDispType, int cam_id, CPoint fi_point)
{
	CDPoint iTmpPoint1;
	//if (fi_iDispType == 2)
	//{
		iTmpPoint1.x = (int)(fi_point.x / dZoomX) + (int)m_fPanX[cam_id];
		iTmpPoint1.y = (int)(fi_point.y / dZoomY) + (int)m_fPanY[cam_id];
	//}
	return iTmpPoint1;
}
