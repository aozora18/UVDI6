
/*
 desc : ��ũ �� �׸���
 */

#include "stdafx.h"
#include "ModelDraw.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : s_area	- [in]  Stage X/Y Min & Max ���� (���� Drive ��ġ ����. ������ �ش��)
		d_area	- [in]  ȭ�鿡 ��µǴ� ��ġ (Stage ��ġ ������ ȭ�鿡 ��µǴ� ũ��. Client Position)
 retn : None
*/
CModelDraw::CModelDraw()
{
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CModelDraw::~CModelDraw()
{
}


/*
 desc : �� �׸��� - Circle
 parm : cam_id	- [in]  Camera Number (1 or 2)
		type	- [in]  Model Type
		cam_no	- [in]  Align Camera Number 1 or 2
		dc		- [in]  ����̽� ���ؽ�Ʈ
		rate	- [in]  um ũ�� ������ pixel�� �����ϱ� ����
		param	- [in]  param 1 ~ 5
		cam_id	- [in]  Camera Number (1 or 2)
 retn : None
*/
VOID CModelDraw::DrawModel(UINT8 cam_id, UINT32 type, CDC *dc, DOUBLE rate, DOUBLE *param)
{
	/* �� ���Ŀ� ���� ���� �׸��� */
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
 desc : �ȼ� ũ�� ��ȯ
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
 retn : um
*/
DOUBLE CModelDraw::GetPixelSizeToUm(UINT8 cam_id)
{
	/* ���� ���õ� ī�޶� ��ȣ�� ���� 1 �ȼ��� ũ�Ⱑ �޶����Ƿ� (���� : Pixel) */
#if (CUSTOM_CODE_PODIS_LLS10 == DELIVERY_PRODUCT_ID || \
	 CUSTOM_CODE_PODIS_LLS06 == DELIVERY_PRODUCT_ID)
	return uvData_Info_GetData()->acam_data->pixel_to_um[cam_id-1];
#elif (CUSTOM_CODE_HANWHA_QCELL == DELIVERY_PRODUCT_ID)
	return uvData_Info_GetData()->acam_data->pixel_to_um;
#endif
}

/*
 desc : �� �׸��� - Circle
 parm : dc		- [in]  ����̽� ���ؽ�Ʈ
		cam_no	- [in]  Align Camera Number 1 or 2
		rate	- [in]  um ũ�� ������ pixel�� �����ϱ� ����
		param	- [in]  param 1 ~ 5
 retn : None
*/
VOID CModelDraw::DrawModelCircle(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param)
{
	UINT32 u32Width, u32Height;
	CBrush csBrushFg, csBrushBg, *pOldBrush;
	CRect rDraw;

	/* ���� ���� ���� �����ϱ� */
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
	/* ���� ���õ� ī�޶� ��ȣ�� ���� 1 �ȼ��� ũ�Ⱑ �޶����Ƿ� (���� : Pixel) */
	u32Width	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	u32Height	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	/* �׸��� ���� ���� */
	rDraw.left	= 0;
	rDraw.top	= 0;
	rDraw.right	= u32Width - 1;
	rDraw.bottom= u32Height - 1;
	/* Brush ���� �� ���� ä��� */
	pOldBrush	= dc->SelectObject(&csBrushBg);
	dc->Rectangle(rDraw);
	dc->SelectObject(pOldBrush);
	/* Brush ���� �� �� �׸��� */
	pOldBrush	= dc->SelectObject(&csBrushFg);
	dc->Ellipse(rDraw);
	dc->SelectObject(pOldBrush);

	/* Brush ��ü ���� */
	csBrushFg.DeleteObject();
	csBrushBg.DeleteObject();
}

/*
 desc : �� �׸��� - Ellipse
 parm : dc		- [in]  ����̽� ���ؽ�Ʈ
		cam_no	- [in]  Align Camera Number 1 or 2
		rate	- [in]  um ũ�� ������ pixel�� �����ϱ� ����
		param	- [in]  param 1 ~ 5
 retn : None
*/
VOID CModelDraw::DrawModelEllipse(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param)
{
	UINT32 u32Width, u32Height;
	CBrush csBrushFg, csBrushBg, *pOldBrush;
	CRect rDraw;

	/* ���� ���� ���� �����ϱ� */
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
	/* ���� ���õ� ī�޶� ��ȣ�� ���� 1 �ȼ��� ũ�Ⱑ �޶����Ƿ� (���� : Pixel) */
	u32Width	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	u32Height	= (UINT32)ROUNDED((param[2] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	/* �׸��� ���� ���� */
	rDraw.left	= 0;
	rDraw.top	= 0;
	rDraw.right	= u32Width - 1;
	rDraw.bottom= u32Height - 1;
	/* Brush ���� �� ���� ä��� */
	pOldBrush	= dc->SelectObject(&csBrushBg);
	dc->Rectangle(rDraw);
	dc->SelectObject(pOldBrush);
	/* Brush ���� �� �� �׸��� */
	pOldBrush	= dc->SelectObject(&csBrushFg);
	dc->Ellipse(rDraw);
	dc->SelectObject(pOldBrush);

	/* Brush ��ü ���� */
	csBrushFg.DeleteObject();
	csBrushBg.DeleteObject();
}

/*
 desc : �� �׸��� - Square
 parm : dc		- [in]  ����̽� ���ؽ�Ʈ
		cam_no	- [in]  Align Camera Number 1 or 2
		rate	- [in]  um ũ�� ������ pixel�� �����ϱ� ����
		param	- [in]  param 1 ~ 5
 retn : None
*/
VOID CModelDraw::DrawModelSquare(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param)
{
	UINT32 u32Width, u32Height;
	CBrush csBrushFg, *pOldBrush;
	CRect rDraw;

	/* ���� ���� ���� �����ϱ� */
	if (param[0] < 256.0f)
	{
		csBrushFg.CreateSolidBrush(RGB(255, 255, 255));
	}
	else
	{
		csBrushFg.CreateSolidBrush(RGB(0, 0, 0));
	}
	/* ���� ���õ� ī�޶� ��ȣ�� ���� 1 �ȼ��� ũ�Ⱑ �޶����Ƿ� (���� : Pixel) */
	u32Width	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	u32Height	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	/* �׸��� ���� ���� */
	rDraw.left	= 0;
	rDraw.top	= 0;
	rDraw.right	= u32Width - 1;
	rDraw.bottom= u32Height - 1;
	/* Brush ���� �� �� �׸��� */
	pOldBrush	= dc->SelectObject(&csBrushFg);
	dc->Rectangle(rDraw);
	dc->SelectObject(pOldBrush);

	/* Brush ��ü ���� */
	csBrushFg.DeleteObject();
}

/*
 desc : �� �׸��� - Ring
 parm : dc		- [in]  ����̽� ���ؽ�Ʈ
		cam_no	- [in]  Align Camera Number 1 or 2
		rate	- [in]  um ũ�� ������ pixel�� �����ϱ� ����
		param	- [in]  param 1 ~ 5
 retn : None
*/
VOID CModelDraw::DrawModelRing(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param)
{
	UINT32 u32Inner, u32Outer;
	CBrush csBrushFg, csBrushBg, *pOldBrush;
	CRect rDrawIn, rDrawOut;

	/* ���� ���� ���� �����ϱ� */
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
	/* ���� ���õ� ī�޶� ��ȣ�� ���� 1 �ȼ��� ũ�Ⱑ �޶����Ƿ� (���� : Pixel) */
	u32Inner	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	u32Outer	= (UINT32)ROUNDED((param[2] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	/* <��>�� �� �׸��� ���� ���� */
	rDrawIn.left	= u32Outer / 2 - u32Inner / 2;
	rDrawIn.top		= u32Outer / 2 - u32Inner / 2;
	rDrawIn.right	= rDrawIn.left + u32Inner;
	rDrawIn.bottom	= rDrawIn.top + u32Inner;
	/* <��>�� �� �׸��� ���� ���� */
	rDrawOut.left	= 0;
	rDrawOut.top	= 0;
	rDrawOut.right	= u32Outer;
	rDrawOut.bottom	= u32Outer;

	/* ���� ä��� */
	pOldBrush	= dc->SelectObject(&csBrushBg);
	dc->Rectangle(rDrawOut);
	dc->SelectObject(pOldBrush);
	/* <��>�� �� ä��� */
	pOldBrush	= dc->SelectObject(&csBrushFg);
	dc->Ellipse(rDrawOut);
	dc->SelectObject(pOldBrush);
	/* <��>�� �� ä��� */
	pOldBrush	= dc->SelectObject(&csBrushBg);
	dc->Ellipse(rDrawIn);
	dc->SelectObject(pOldBrush);

	/* Brush ��ü ���� */
	csBrushFg.DeleteObject();
	csBrushBg.DeleteObject();
}

/*
 desc : �� �׸��� - Rectangle
 parm : dc		- [in]  ����̽� ���ؽ�Ʈ
		cam_no	- [in]  Align Camera Number 1 or 2
		rate	- [in]  um ũ�� ������ pixel�� �����ϱ� ����
		param	- [in]  param 1 ~ 5
 retn : None
*/
VOID CModelDraw::DrawModelRectangle(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param)
{
	UINT32 u32Width, u32Height;
	CBrush csBrushFg, *pOldBrush;
	CRect rDraw;

	/* ���� ���� ���� �����ϱ� */
	if (param[0] < 256.0f)
	{
		csBrushFg.CreateSolidBrush(RGB(255, 255, 255));
	}
	else
	{
		csBrushFg.CreateSolidBrush(RGB(0, 0, 0));
	}
	/* ���� ���õ� ī�޶� ��ȣ�� ���� 1 �ȼ��� ũ�Ⱑ �޶����Ƿ� (���� : Pixel) */
	u32Width	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	u32Height	= (UINT32)ROUNDED((param[2] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	/* �׸��� ���� ���� */
	rDraw.left	= 0;
	rDraw.top	= 0;
	rDraw.right	= u32Width - 1;
	rDraw.bottom= u32Height - 1;
	/* Brush ���� �� �� �׸��� */
	pOldBrush	= dc->SelectObject(&csBrushFg);
	dc->Rectangle(rDraw);
	dc->SelectObject(pOldBrush);

	/* Brush ��ü ���� */
	csBrushFg.DeleteObject();
}

/*
 desc : �� �׸��� - Triangle
 parm : dc		- [in]  ����̽� ���ؽ�Ʈ
		cam_no	- [in]  Align Camera Number 1 or 2
		rate	- [in]  um ũ�� ������ pixel�� �����ϱ� ����
		param	- [in]  param 1 ~ 5
 retn : None
*/
VOID CModelDraw::DrawModelTriangle(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param)
{
	UINT32 u32Width, u32Height;
	CBrush csBrushFg, csBrushBg, *pOldBrush;
	POINT ptVertex[3];
	CRect rDraw;

	/* ���� ���� ���� �����ϱ� */
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
	/* ���� ���õ� ī�޶� ��ȣ�� ���� 1 �ȼ��� ũ�Ⱑ �޶����Ƿ� (���� : Pixel) */
	u32Width	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	u32Height	= (UINT32)ROUNDED((param[2] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	/* �ﰢ���� 3 ������ ���ϱ� */
	ptVertex[0].x	= (u32Width - 2) / 2;
	ptVertex[0].y	= 0;
	ptVertex[1].x	= (u32Width - 2);
	ptVertex[1].y	= (u32Height- 2);
	ptVertex[2].x	= 0;
	ptVertex[2].y	= (u32Height- 2);
	/* ��� ���� */
	rDraw.left		= 0;
	rDraw.top		= 0;
	rDraw.right		= (u32Width - 1);
	rDraw.bottom	= (u32Height- 1);

	/* ���� ä��� */
	pOldBrush	= dc->SelectObject(&csBrushBg);
	dc->Rectangle(rDraw);
	dc->SelectObject(pOldBrush);
	/* <��>�� �� ä��� */
	pOldBrush	= dc->SelectObject(&csBrushFg);
	dc->Polygon(ptVertex, 3);
	dc->SelectObject(pOldBrush);

	/* Brush ��ü ���� */
	csBrushFg.DeleteObject();
	csBrushBg.DeleteObject();
}

/*
 desc : �� �׸��� - Diamond
 parm : dc		- [in]  ����̽� ���ؽ�Ʈ
		cam_no	- [in]  Align Camera Number 1 or 2
		rate	- [in]  um ũ�� ������ pixel�� �����ϱ� ����
		param	- [in]  param 1 ~ 5
 retn : None
*/
VOID CModelDraw::DrawModelDiamond(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param)
{
	UINT32 u32Width, u32Height;
	CBrush csBrushFg, csBrushBg, *pOldBrush;
	POINT ptVertex[4];
	CRect rDraw;

	/* ���� ���� ���� �����ϱ� */
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
	/* ���� ���õ� ī�޶� ��ȣ�� ���� 1 �ȼ��� ũ�Ⱑ �޶����Ƿ� (���� : Pixel) */
	u32Width	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	u32Height	= (UINT32)ROUNDED((param[2] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	/* �ﰢ���� 3 ������ ���ϱ� */
	ptVertex[0].x	= (u32Width - 2) / 2;
	ptVertex[0].y	= 0;
	ptVertex[1].x	= (u32Width - 2);
	ptVertex[1].y	= (u32Height- 2) / 2;
	ptVertex[2].x	= (u32Width - 2) / 2;
	ptVertex[2].y	= (u32Height- 2);
	ptVertex[3].x	= 0;
	ptVertex[3].y	= (u32Height- 2) / 2;
	/* ��� ���� */
	rDraw.left		= 0;
	rDraw.top		= 0;
	rDraw.right		= (u32Width - 1);
	rDraw.bottom	= (u32Height- 1);

	/* ���� ä��� */
	pOldBrush	= dc->SelectObject(&csBrushBg);
	dc->Rectangle(rDraw);
	dc->SelectObject(pOldBrush);
	/* <��>�� �� ä��� */
	pOldBrush	= dc->SelectObject(&csBrushFg);
	dc->Polygon(ptVertex, 4);
	dc->SelectObject(pOldBrush);

	/* Brush ��ü ���� */
	csBrushFg.DeleteObject();
	csBrushBg.DeleteObject();
}

/*
 desc : �� �׸��� - Cross
 parm : dc		- [in]  ����̽� ���ؽ�Ʈ
		cam_no	- [in]  Align Camera Number 1 or 2
		rate	- [in]  um ũ�� ������ pixel�� �����ϱ� ����
		param	- [in]  param 1 ~ 5
 retn : None
*/
VOID CModelDraw::DrawModelCross(UINT8 cam_id, CDC *dc, DOUBLE rate, DOUBLE *param)
{
	UINT32 u32WidthOut, u32HeightOut, u32WidthIn, u32HeightIn;
	CBrush csBrushFg, csBrushBg, *pOldBrush;
	POINT ptVertex[12];
	CRect rDraw;

	/* ���� ���� ���� �����ϱ� */
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

	/* ���� ���õ� ī�޶� ��ȣ�� ���� 1 �ȼ��� ũ�Ⱑ �޶����Ƿ� (���� : Pixel) */
	u32WidthOut	= (UINT32)ROUNDED((param[1] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	u32HeightOut= (UINT32)ROUNDED((param[2] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	/* ���� ���õ� ī�޶� ��ȣ�� ���� 1 �ȼ��� ũ�Ⱑ �޶����Ƿ� (���� : Pixel) */
	u32WidthIn	= (UINT32)ROUNDED((param[3] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	u32HeightIn	= (UINT32)ROUNDED((param[4] * 2.0f) / GetPixelSizeToUm(cam_id) * rate, 0);
	/* Cross ���ϱ� */
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

	/* ��� ���� */
	rDraw.left		= 0;
	rDraw.top		= 0;
	rDraw.right		= (u32WidthOut - 1);
	rDraw.bottom	= (u32HeightOut- 1);

	/* ���� ä��� */
	pOldBrush	= dc->SelectObject(&csBrushBg);
	dc->Rectangle(rDraw);
	dc->SelectObject(pOldBrush);
	/* <����/����> �簢�� ä��� */
	pOldBrush	= dc->SelectObject(&csBrushFg);
	dc->Polygon(ptVertex, 12);
	dc->SelectObject(pOldBrush);

	/* Brush ��ü ���� */
	csBrushFg.DeleteObject();
	csBrushBg.DeleteObject();
}
