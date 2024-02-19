#pragma once
//#include "MainApp.h"
#include "stdafx.h"

#define MAX_CNT_PIXEL	20000
#define MAX_CNT_LINE	30000
#define MAX_CNT_BOX		100
#define MAX_CNT_CROSS	1000
#define MAX_CNT_CIRCLE	100
#define MAX_CNT_TEXT	100


//----------Point----------//
class CDrawDcPixel {
public:
	int			cnt;
	COLORREF	c[MAX_CNT_PIXEL];
	POINT		p[MAX_CNT_PIXEL];
};

//----------Line----------//
class CDrawDcLine {
public:
	int			cnt;
	COLORREF	c[MAX_CNT_LINE];
	POINT		p[MAX_CNT_LINE][2];
	int			style[MAX_CNT_LINE];
};

//----------Box----------//
class CDrawDcBox {
public:
	int			cnt;
	CRect		r[MAX_CNT_BOX];
	COLORREF	c[MAX_CNT_BOX];
	int			style[MAX_CNT_BOX];
};

//----------Circle----------//
class CDrawDcCircle {
public:
	int			cnt;
	CRect		r[MAX_CNT_CIRCLE];
	COLORREF	c[MAX_CNT_CIRCLE];
};

//----------Cross----------//
class CDrawDcCross {
public:
	int			cnt;
	CPoint		p[MAX_CNT_CROSS];
	COLORREF	c[MAX_CNT_CROSS];
	int			w1[MAX_CNT_CROSS];
	int			w2[MAX_CNT_CROSS];
};

//----------Text----------//
class CDrawDcText {
public:
	int			cnt;							//개수
	CPoint		p[MAX_CNT_TEXT];				//좌표
	COLORREF	c[MAX_CNT_TEXT];				//색깔
	char		str[MAX_CNT_TEXT][1000];				//내용
	CPoint		size[MAX_CNT_TEXT];				//폰트 크기	
	char		fontname[MAX_CNT_TEXT][100];	//폰트명
	bool		edgeFlag[MAX_CNT_TEXT];			//외각선
};

class CMilDisp : public CWnd
{
	DECLARE_DYNAMIC(CMilDisp, CWnd)
private:
	bool terminated = false;

	/* 생성자 & 파괴자 */
public:
	CMilDisp(void);
	~CMilDisp(void);

	void DrawBase(int fi_iDispType, int fi_iNo);
	void DrawOverlayDC(bool fi_bDrawFlag, int fi_iDispType, int fi_iNo);

	void OverlayDC_Text(MIL_ID fi_MilBuf, int fi_iDispType, int fi_iNo, double fi_dRateX, double fi_dRateY);
	void OverlayDC_Pixel(MIL_ID fi_MilBuf, int fi_iDispType, int fi_iNo, double fi_dRateX, double fi_dRateY);
	void OverlayDC_Line(MIL_ID fi_MilBuf, int fi_iDispType, int fi_iNo, double fi_dRateX, double fi_dRateY);
	void OverlayDC_Circle(MIL_ID fi_MilBuf, int fi_iDispType, int fi_iNo, double fi_dRateX, double fi_dRateY);
	void OverlayDC_Cross(MIL_ID fi_MilBuf, int fi_iDispType, int fi_iNo, double fi_dRateX, double fi_dRateY);
	void OverlayDC_Box(MIL_ID fi_MilBuf, int fi_iDispType, int fi_iNo, double fi_dRateX, double fi_dRateY);

	void AddCircleList(int fi_iDispType, int fi_iNo, int fi_iLeft, int fi_iTop, int fi_iRight, int fi_iBottom, int fi_color);
	void AddBoxList(int fi_iDispType, int fi_iNo, int fi_iLeft, int fi_iTop, int fi_iRight, int fi_iBottom, int fi_iStyle, int fi_color);
	void AddPixelList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, int fi_color);
	void AddLineList(int fi_iDispType, int fi_iNo, int fi_iSx, int fi_iSy, int fi_iEx, int fi_iEy, int fi_iStyle, int fi_color);
	void AddCrossList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, int fi_iWdt1, int fi_iWdt2, int fi_color);
	void AddTextList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, CString fi_sText, int fi_color, int fi_iSizeX, int fi_iSizeY, CString fi_sFont, bool fi_bEgdeFlag);

	CDrawDcLine**	clLineList;
	CDrawDcPixel**	clPixelList;
	CDrawDcBox**	clBoxList;
	CDrawDcCircle**	clCircleList;
	CDrawDcCross**	clCrossList;
	CDrawDcText**	clTextList;

	CPoint GetOrgImgSize(int fi_iCamNo);
	CDPoint GetRateDispToBuf(int fi_iCamNo, int fi_DispType);
	VOID InitMilDisp();
	VOID CloseMilDisp();

	void MilZoomIn(int fi_iDispType, int cam_id, CRect rc);
	BOOL MilZoomOut(int fi_iDispType, int cam_id, CRect rc);
	void MilAutoScale(int fi_iDispType, int cam_id);
	BOOL MilSetZoomFactor(int fi_iDispType, int cam_id, float fZoomX, float fZoomY, CRect rc);
	void SetZoomDownP(int fi_iDispType, int cam_id, CPoint fi_point);
	void MoveZoomDisp(int fi_iDispType, int cam_id, CPoint fi_point, CRect fi_rect);
	CDPoint trZoomPoint(int fi_iDispType, int cam_id, CPoint fi_point);

	float* m_fZoomX;
	float* m_fZoomY;
	float m_fFixZoomX;
	float m_fFixZoomY;
	double* m_fPanX;
	double* m_fPanY;
	float dZoomX;
	float dZoomY;
	CPoint ZoomDownP;

};

