
/*
 desc : 마크 모델 그리기
 */

#include "stdafx.h"
#include "ModelDraw.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : s_area	- [in]  Stage X/Y Min & Max 영역 (실제 Drive 위치 정보. 음수도 해당됨)
		d_area	- [in]  화면에 출력되는 위치 (Stage 위치 정보가 화면에 출력되는 크기. Client Position)
 retn : None
*/
CModelDraw::CModelDraw()
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CModelDraw::~CModelDraw()
{
}


/*
 desc : 모델 그리기 - Circle
 parm : cam_id	- [in]  Camera Number (1 or 2)
		type	- [in]  Model Type
		cam_no	- [in]  Align Camera Number 1 or 2
		dc		- [in]  디바이스 컨텍스트
		rate	- [in]  um 크기 단위를 pixel로 변경하기 위함
		param	- [in]  param 1 ~ 5
		cam_id	- [in]  Camera Number (1 or 2)
 retn : None
*/
VOID CModelDraw::DrawModel(UINT8 cam_id, UINT32 type, CDC *dc, DOUBLE rate, DOUBLE *param)
{
	/* 모델 형식에 따라 도형 그리기 */
	switch (type)
	{
	case en_mmdt_circle		:	DrawModelCircle(cam_id, dc, rate, param);		break;
	case en_mmdt_square		:	DrawModelSquare(cam_id, dc, rate, param);		break;
	case en_mmdt_ring		:	DrawModelRing(cam_id, dc, rate, param);			break;
	case en_mmdt_cross		:	DrawModelCross(cam_id, dc, rate, param);		break;
	case en_mmdt_ellipse	:	DrawModelEllipse(cam_id, dc, rate, param);		break;
	case en_mmdt_rectangle	:	DrawModelRectangle(cam_id, dc, rate, param);	break;
	case en_mmdt_triangle	:	DrawModelTriangle(cam_id, dc, rate, param);		break;
	case en_mmdt_diamond	:	DrawModelDiamond(cam_id, dc, rate, param);		break;
	}
}

/*
 desc : 픽셀 크기 반환
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
 retn : um
*/
DOUBLE CModelDraw::GetPixelSizeToUm(UINT8 cam_id)
{
	/* 현재 선택된 카메라 번호에 따라 1 픽셀의 크기가 달라지므로 (단위 : Pixel) */
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	return uvData_Info_GetData()->acam_data->pixel_to_um[cam_id-1];
#elif (CUSTOM_CODE_HANWHA_QCELL == DELIVERY_PRODUCT_ID)
	return uvData_Info_GetData()->acam_data->pixel_to_um;
#endif
}

/*
 desc : 모델 그리기 - Circle
 parm : dc		- [in]  디바이스 컨텍스트
		cam_no	- [in]  Align Camera Number 1 or 2
		rate	- [in]  um 크기 단위를 pixel로 변경하기 위함
		param	- [in]  param 1 ~ 5
 retn : None
*/
VOID CModelDraw::DrawModelCircle(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param)
{
	UINT32 u32Width, u32Height;
	CBrush csBrushFg, csBrushBg, *pOldBrush;
	CRect rDraw;

	/* 배경과 전경 색상 설정하기 */
	if (param[0] != 256.0f)
	{
		csBrushFg.CreateSolidBrush(RGB(255, 255, 255));
		csBrushBg.CreateSolidBrush(RGB(0, 0, 0));
	}
	else
	{
		csBrushFg.CreateSolidBrush(RGB(0, 0, 0));
		csBrushBg.CreateSolidBrush(RGB(255, 255, 255));
	}
	/* 현재 선택된 카메라 번호에 따라 1 픽셀의 크기가 달라지므로 (단위 : Pixel) */
	u32Width	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	u32Height	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	/* 그리기 영역 설정 */
	rDraw.left	= 0;
	rDraw.top	= 0;
	rDraw.right	= u32Width - 1;
	rDraw.bottom= u32Height - 1;
	/* Brush 선택 및 배경색 채우기 */
	pOldBrush	= dc->SelectObject(&csBrushBg);
	dc->Rectangle(rDraw);
	dc->SelectObject(pOldBrush);
	/* Brush 선택 및 모델 그리기 */
	pOldBrush	= dc->SelectObject(&csBrushFg);
	dc->Ellipse(rDraw);
	dc->SelectObject(pOldBrush);

	/* Brush 객체 제거 */
	csBrushFg.DeleteObject();
	csBrushBg.DeleteObject();
}

/*
 desc : 모델 그리기 - Ellipse
 parm : dc		- [in]  디바이스 컨텍스트
		cam_no	- [in]  Align Camera Number 1 or 2
		rate	- [in]  um 크기 단위를 pixel로 변경하기 위함
		param	- [in]  param 1 ~ 5
 retn : None
*/
VOID CModelDraw::DrawModelEllipse(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param)
{
	UINT32 u32Width, u32Height;
	CBrush csBrushFg, csBrushBg, *pOldBrush;
	CRect rDraw;

	/* 배경과 전경 색상 설정하기 */
	if (param[0] != 256.0f)
	{
		csBrushFg.CreateSolidBrush(RGB(255, 255, 255));
		csBrushBg.CreateSolidBrush(RGB(0, 0, 0));
	}
	else
	{
		csBrushFg.CreateSolidBrush(RGB(0, 0, 0));
		csBrushBg.CreateSolidBrush(RGB(255, 255, 255));
	}
	/* 현재 선택된 카메라 번호에 따라 1 픽셀의 크기가 달라지므로 (단위 : Pixel) */
	u32Width	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	u32Height	= (UINT32)ROUNDED((param[2] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	/* 그리기 영역 설정 */
	rDraw.left	= 0;
	rDraw.top	= 0;
	rDraw.right	= u32Width - 1;
	rDraw.bottom= u32Height - 1;
	/* Brush 선택 및 배경색 채우기 */
	pOldBrush	= dc->SelectObject(&csBrushBg);
	dc->Rectangle(rDraw);
	dc->SelectObject(pOldBrush);
	/* Brush 선택 및 모델 그리기 */
	pOldBrush	= dc->SelectObject(&csBrushFg);
	dc->Ellipse(rDraw);
	dc->SelectObject(pOldBrush);

	/* Brush 객체 제거 */
	csBrushFg.DeleteObject();
	csBrushBg.DeleteObject();
}

/*
 desc : 모델 그리기 - Square
 parm : dc		- [in]  디바이스 컨텍스트
		cam_no	- [in]  Align Camera Number 1 or 2
		rate	- [in]  um 크기 단위를 pixel로 변경하기 위함
		param	- [in]  param 1 ~ 5
 retn : None
*/
VOID CModelDraw::DrawModelSquare(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param)
{
	UINT32 u32Width, u32Height;
	CBrush csBrushFg, *pOldBrush;
	CRect rDraw;

	/* 배경과 전경 색상 설정하기 */
	if (param[0] < 256.0f)
	{
		csBrushFg.CreateSolidBrush(RGB(255, 255, 255));
	}
	else
	{
		csBrushFg.CreateSolidBrush(RGB(0, 0, 0));
	}
	/* 현재 선택된 카메라 번호에 따라 1 픽셀의 크기가 달라지므로 (단위 : Pixel) */
	u32Width	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	u32Height	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	/* 그리기 영역 설정 */
	rDraw.left	= 0;
	rDraw.top	= 0;
	rDraw.right	= u32Width - 1;
	rDraw.bottom= u32Height - 1;
	/* Brush 선택 및 모델 그리기 */
	pOldBrush	= dc->SelectObject(&csBrushFg);
	dc->Rectangle(rDraw);
	dc->SelectObject(pOldBrush);

	/* Brush 객체 제거 */
	csBrushFg.DeleteObject();
}

/*
 desc : 모델 그리기 - Ring
 parm : dc		- [in]  디바이스 컨텍스트
		cam_no	- [in]  Align Camera Number 1 or 2
		rate	- [in]  um 크기 단위를 pixel로 변경하기 위함
		param	- [in]  param 1 ~ 5
 retn : None
*/
VOID CModelDraw::DrawModelRing(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param)
{
	UINT32 u32Inner, u32Outer;
	CBrush csBrushFg, csBrushBg, *pOldBrush;
	CRect rDrawIn, rDrawOut;

	/* 배경과 전경 색상 설정하기 */
	if (param[0] != 256.0f)
	{
		csBrushFg.CreateSolidBrush(RGB(255, 255, 255));
		csBrushBg.CreateSolidBrush(RGB(0, 0, 0));
	}
	else
	{
		csBrushFg.CreateSolidBrush(RGB(0, 0, 0));
		csBrushBg.CreateSolidBrush(RGB(255, 255, 255));
	}
	/* 현재 선택된 카메라 번호에 따라 1 픽셀의 크기가 달라지므로 (단위 : Pixel) */
	u32Inner	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	u32Outer	= (UINT32)ROUNDED((param[2] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	/* <내>부 원 그리기 영역 설정 */
	rDrawIn.left	= u32Outer / 2 - u32Inner / 2;
	rDrawIn.top		= u32Outer / 2 - u32Inner / 2;
	rDrawIn.right	= rDrawIn.left + u32Inner;
	rDrawIn.bottom	= rDrawIn.top + u32Inner;
	/* <외>부 원 그리기 영역 설정 */
	rDrawOut.left	= 0;
	rDrawOut.top	= 0;
	rDrawOut.right	= u32Outer;
	rDrawOut.bottom	= u32Outer;

	/* 배경색 채우기 */
	pOldBrush	= dc->SelectObject(&csBrushBg);
	dc->Rectangle(rDrawOut);
	dc->SelectObject(pOldBrush);
	/* <외>부 원 채우기 */
	pOldBrush	= dc->SelectObject(&csBrushFg);
	dc->Ellipse(rDrawOut);
	dc->SelectObject(pOldBrush);
	/* <내>부 원 채우기 */
	pOldBrush	= dc->SelectObject(&csBrushBg);
	dc->Ellipse(rDrawIn);
	dc->SelectObject(pOldBrush);

	/* Brush 객체 제거 */
	csBrushFg.DeleteObject();
	csBrushBg.DeleteObject();
}

/*
 desc : 모델 그리기 - Rectangle
 parm : dc		- [in]  디바이스 컨텍스트
		cam_no	- [in]  Align Camera Number 1 or 2
		rate	- [in]  um 크기 단위를 pixel로 변경하기 위함
		param	- [in]  param 1 ~ 5
 retn : None
*/
VOID CModelDraw::DrawModelRectangle(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param)
{
	UINT32 u32Width, u32Height;
	CBrush csBrushFg, *pOldBrush;
	CRect rDraw;

	/* 배경과 전경 색상 설정하기 */
	if (param[0] < 256.0f)
	{
		csBrushFg.CreateSolidBrush(RGB(255, 255, 255));
	}
	else
	{
		csBrushFg.CreateSolidBrush(RGB(0, 0, 0));
	}
	/* 현재 선택된 카메라 번호에 따라 1 픽셀의 크기가 달라지므로 (단위 : Pixel) */
	u32Width	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	u32Height	= (UINT32)ROUNDED((param[2] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	/* 그리기 영역 설정 */
	rDraw.left	= 0;
	rDraw.top	= 0;
	rDraw.right	= u32Width - 1;
	rDraw.bottom= u32Height - 1;
	/* Brush 선택 및 모델 그리기 */
	pOldBrush	= dc->SelectObject(&csBrushFg);
	dc->Rectangle(rDraw);
	dc->SelectObject(pOldBrush);

	/* Brush 객체 제거 */
	csBrushFg.DeleteObject();
}

/*
 desc : 모델 그리기 - Triangle
 parm : dc		- [in]  디바이스 컨텍스트
		cam_no	- [in]  Align Camera Number 1 or 2
		rate	- [in]  um 크기 단위를 pixel로 변경하기 위함
		param	- [in]  param 1 ~ 5
 retn : None
*/
VOID CModelDraw::DrawModelTriangle(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param)
{
	UINT32 u32Width, u32Height;
	CBrush csBrushFg, csBrushBg, *pOldBrush;
	POINT ptVertex[3];
	CRect rDraw;

	/* 배경과 전경 색상 설정하기 */
	if (param[0] != 256.0f)
	{
		csBrushFg.CreateSolidBrush(RGB(255, 255, 255));
		csBrushBg.CreateSolidBrush(RGB(0, 0, 0));
	}
	else
	{
		csBrushFg.CreateSolidBrush(RGB(0, 0, 0));
		csBrushBg.CreateSolidBrush(RGB(255, 255, 255));
	}
	/* 현재 선택된 카메라 번호에 따라 1 픽셀의 크기가 달라지므로 (단위 : Pixel) */
	u32Width	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	u32Height	= (UINT32)ROUNDED((param[2] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	/* 삼각형의 3 꼭지점 구하기 */
	ptVertex[0].x	= (u32Width - 2) / 2;
	ptVertex[0].y	= 0;
	ptVertex[1].x	= (u32Width - 2);
	ptVertex[1].y	= (u32Height- 2);
	ptVertex[2].x	= 0;
	ptVertex[2].y	= (u32Height- 2);
	/* 배경 영역 */
	rDraw.left		= 0;
	rDraw.top		= 0;
	rDraw.right		= (u32Width - 1);
	rDraw.bottom	= (u32Height- 1);

	/* 배경색 채우기 */
	pOldBrush	= dc->SelectObject(&csBrushBg);
	dc->Rectangle(rDraw);
	dc->SelectObject(pOldBrush);
	/* <외>부 원 채우기 */
	pOldBrush	= dc->SelectObject(&csBrushFg);
	dc->Polygon(ptVertex, 3);
	dc->SelectObject(pOldBrush);

	/* Brush 객체 제거 */
	csBrushFg.DeleteObject();
	csBrushBg.DeleteObject();
}

/*
 desc : 모델 그리기 - Diamond
 parm : dc		- [in]  디바이스 컨텍스트
		cam_no	- [in]  Align Camera Number 1 or 2
		rate	- [in]  um 크기 단위를 pixel로 변경하기 위함
		param	- [in]  param 1 ~ 5
 retn : None
*/
VOID CModelDraw::DrawModelDiamond(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param)
{
	UINT32 u32Width, u32Height;
	CBrush csBrushFg, csBrushBg, *pOldBrush;
	POINT ptVertex[4];
	CRect rDraw;

	/* 배경과 전경 색상 설정하기 */
	if (param[0] != 256.0f)
	{
		csBrushFg.CreateSolidBrush(RGB(255, 255, 255));
		csBrushBg.CreateSolidBrush(RGB(0, 0, 0));
	}
	else
	{
		csBrushFg.CreateSolidBrush(RGB(0, 0, 0));
		csBrushBg.CreateSolidBrush(RGB(255, 255, 255));
	}
	/* 현재 선택된 카메라 번호에 따라 1 픽셀의 크기가 달라지므로 (단위 : Pixel) */
	u32Width	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	u32Height	= (UINT32)ROUNDED((param[2] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	/* 삼각형의 3 꼭지점 구하기 */
	ptVertex[0].x	= (u32Width - 2) / 2;
	ptVertex[0].y	= 0;
	ptVertex[1].x	= (u32Width - 2);
	ptVertex[1].y	= (u32Height- 2) / 2;
	ptVertex[2].x	= (u32Width - 2) / 2;
	ptVertex[2].y	= (u32Height- 2);
	ptVertex[3].x	= 0;
	ptVertex[3].y	= (u32Height- 2) / 2;
	/* 배경 영역 */
	rDraw.left		= 0;
	rDraw.top		= 0;
	rDraw.right		= (u32Width - 1);
	rDraw.bottom	= (u32Height- 1);

	/* 배경색 채우기 */
	pOldBrush	= dc->SelectObject(&csBrushBg);
	dc->Rectangle(rDraw);
	dc->SelectObject(pOldBrush);
	/* <외>부 원 채우기 */
	pOldBrush	= dc->SelectObject(&csBrushFg);
	dc->Polygon(ptVertex, 4);
	dc->SelectObject(pOldBrush);

	/* Brush 객체 제거 */
	csBrushFg.DeleteObject();
	csBrushBg.DeleteObject();
}

/*
 desc : 모델 그리기 - Cross
 parm : dc		- [in]  디바이스 컨텍스트
		cam_no	- [in]  Align Camera Number 1 or 2
		rate	- [in]  um 크기 단위를 pixel로 변경하기 위함
		param	- [in]  param 1 ~ 5
 retn : None
*/
VOID CModelDraw::DrawModelCross(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param)
{
	UINT32 u32WidthOut, u32HeightOut, u32WidthIn, u32HeightIn;
	CBrush csBrushFg, csBrushBg, *pOldBrush;
	POINT ptVertex[12];
	CRect rDraw;

	/* 배경과 전경 색상 설정하기 */
	if (param[0] != 256.0f)
	{
		csBrushFg.CreateSolidBrush(RGB(255, 255, 255));
		csBrushBg.CreateSolidBrush(RGB(0, 0, 0));
	}
	else
	{
		csBrushFg.CreateSolidBrush(RGB(0, 0, 0));
		csBrushBg.CreateSolidBrush(RGB(255, 255, 255));
	}

	/* 현재 선택된 카메라 번호에 따라 1 픽셀의 크기가 달라지므로 (단위 : Pixel) */
	u32WidthOut	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	u32HeightOut= (UINT32)ROUNDED((param[2] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	/* 현재 선택된 카메라 번호에 따라 1 픽셀의 크기가 달라지므로 (단위 : Pixel) */
	u32WidthIn	= (UINT32)ROUNDED((param[3] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	u32HeightIn	= (UINT32)ROUNDED((param[4] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	/* Cross 구하기 */
	ptVertex[0].x	= 0;
	ptVertex[0].y	= (u32HeightOut- 2) / 2 - u32HeightIn / 2;
	ptVertex[1].x	= (u32WidthOut - 2) / 2 - u32WidthIn  / 2;
	ptVertex[1].y	= (ptVertex[0].y);
	ptVertex[2].x	= (ptVertex[1].x);
	ptVertex[2].y	= 0;
	ptVertex[3].x	= (ptVertex[2].x + u32WidthIn);
	ptVertex[3].y	= (ptVertex[2].y);
	ptVertex[4].x	= (ptVertex[3].x);
	ptVertex[4].y	= (ptVertex[1].y);
	ptVertex[5].x	= (u32WidthOut - 2);
	ptVertex[5].y	= (ptVertex[0].y);
	ptVertex[6].x	= (ptVertex[5].x);
	ptVertex[6].y	= (ptVertex[4].y + u32HeightIn);
	ptVertex[7].x	= (ptVertex[3].x);
	ptVertex[7].y	= (ptVertex[6].y);
	ptVertex[8].x	= (ptVertex[7].x);
	ptVertex[8].y	= (u32HeightOut- 2);
	ptVertex[9].x	= (ptVertex[2].x);
	ptVertex[9].y	= (ptVertex[8].y);
	ptVertex[10].x	= (ptVertex[9].x);
	ptVertex[10].y	= (ptVertex[7].y);
	ptVertex[11].x	= (ptVertex[0].x);
	ptVertex[11].y	= (ptVertex[10].y);

	/* 배경 영역 */
	rDraw.left		= 0;
	rDraw.top		= 0;
	rDraw.right		= (u32WidthOut - 1);
	rDraw.bottom	= (u32HeightOut- 1);

	/* 배경색 채우기 */
	pOldBrush	= dc->SelectObject(&csBrushBg);
	dc->Rectangle(rDraw);
	dc->SelectObject(pOldBrush);
	/* <가로/세로> 사각형 채우기 */
	pOldBrush	= dc->SelectObject(&csBrushFg);
	dc->Polygon(ptVertex, 12);
	dc->SelectObject(pOldBrush);

	/* Brush 객체 제거 */
	csBrushFg.DeleteObject();
	csBrushBg.DeleteObject();
}
