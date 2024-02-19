
/*
 desc : ��ũ �� �׸���
 */

#include "pch.h"
#include "../../MainApp.h"
#include "DrawModel.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : r_draw	- [in]  Model Image�� ��µ� ���� ���� (�θ� ������ ���� ����)
 retn : None
*/
CDrawModel::CDrawModel(RECT r_draw)
{
	m_rDraw	= r_draw;
	ResetModel();
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDrawModel::~CDrawModel()
{
}

/*
 desc : �� ���� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDrawModel::ResetModel()
{
	UINT8 i	= 0x00;
	m_u8ACamID		= 0x00;
	m_enModelType	= ENG_MMDT::en_none;
	m_enFindType	= 0;
	m_dbMarkW		= 0.0f;
	m_dbMarkH		= 0.0f;
	wmemset(m_tzModelName, 0x00, MARK_MODEL_NAME_LENGTH);
	wmemset(m_tzMMFFile, 0x00, MARK_MODEL_NAME_LENGTH);
	wmemset(m_tzPATFile, 0x00, MARK_MODEL_NAME_LENGTH);
	
	// lk91 VISION �߰�
	iBaseP.x = 1920 >> 1;
	iBaseP.y = 1200 >> 1;
	iSizeP.x = iSizeP.y = 0;
	iOffsetP.x = iOffsetP.y = 0;
	rectSearchArea.left = 50;
	rectSearchArea.top = 50;
	rectSearchArea.right = 1920 - 50;
	rectSearchArea.bottom = 1200 - 50;
	dScore = 80.0;


	for (i=0; i<5; i++)	m_dbParam[i]	= 0.0f;
}

/*
 desc : �׸����� �ϴ� �� ����
 parm : m_name	- [in]  Model Name
		m_file	- [in]  Model File (for mmf file)
		type	- [in]  Model Type (ENG_MMDT)
		cam_id	- [in]  Camera Number (1 or 2)
		param	- [in]  param 1 ~ 5
 retn : None
*/
VOID CDrawModel::SetModel(PTCHAR m_name, ENG_MMDT type, UINT8 cam_id, DOUBLE *param)
{
	UINT32 u32Size	= 0;

	/* �ֿ� ��� ���� ���� */
	swprintf_s(m_tzModelName, MARK_MODEL_NAME_LENGTH, L"%s", m_name);
	m_enModelType	= type;
	m_u8ACamID		= cam_id;
	m_enFindType	= 0;
	/* �Ķ���� �� ���� */
	switch (type)
	{
	case ENG_MMDT::en_circle	:
	case ENG_MMDT::en_square	:	u32Size = sizeof(DOUBLE) * 2;	break;
	case ENG_MMDT::en_ring		:
	case ENG_MMDT::en_ellipse	:
	case ENG_MMDT::en_rectangle	:
	case ENG_MMDT::en_triangle	:
	case ENG_MMDT::en_diamond	:	u32Size = sizeof(DOUBLE) * 3;	break;
	case ENG_MMDT::en_cross		:	u32Size = sizeof(DOUBLE) * 5;	break;
	}
	/* Color + Size (1 ~ 4) */
	if (u32Size)	memcpy(m_dbParam, param, u32Size);
}

VOID CDrawModel::SetModel(PTCHAR m_name, ENG_MMDT type, DOUBLE* param)
{
	UINT32 u32Size = 0;

	/* �ֿ� ��� ���� ���� */
	swprintf_s(m_tzModelName, MARK_MODEL_NAME_LENGTH, L"%s", m_name);
	m_enModelType = type;
	m_enFindType = 0;
	/* �Ķ���� �� ���� */
	switch (type)
	{
	case ENG_MMDT::en_circle:
	case ENG_MMDT::en_square:	u32Size = sizeof(DOUBLE) * 2;	break;
	case ENG_MMDT::en_ring:
	case ENG_MMDT::en_ellipse:
	case ENG_MMDT::en_rectangle:
	case ENG_MMDT::en_triangle:
	case ENG_MMDT::en_diamond:	u32Size = sizeof(DOUBLE) * 3;	break;
	case ENG_MMDT::en_cross:	u32Size = sizeof(DOUBLE) * 5;	break;
	}
	/* Color + Size (1 ~ 4) */
	if (u32Size)	memcpy(m_dbParam, param, u32Size);
}

VOID CDrawModel::SetModel(PTCHAR m_name, UINT8 cam_id, PTCHAR m_file, CPoint m_SizeP, CPoint m_CenterP) 
{
	UINT32 u32Size	= 0;

	/* �ֿ� ��� ���� ���� */
	swprintf_s(m_tzModelName, MARK_MODEL_NAME_LENGTH, L"%s", m_name);
	swprintf_s(m_tzMMFFile, MARK_MODEL_NAME_LENGTH, L"%s", m_file);
	swprintf_s(m_tzPATFile, MARK_MODEL_NAME_LENGTH, L"%s", m_file);
	//m_enFindType = find_type;
	//if(find_type == 0)
		m_enModelType	= ENG_MMDT::en_image;
	//else
	//	m_enModelType = ENG_MMDT::en_none;
	m_u8ACamID		= cam_id;
	//if(m_SizeP.x != 0 && m_SizeP.y != 0)
		iSizeP = m_SizeP;
	//if(m_CenterP.x != 0 && m_CenterP.y != 0)
		iOffsetP = m_CenterP;
}	

/*
 desc : �̹����� ũ�⸦ ��� ����� ���� ũ�⿡ �°� ������ ���� ���� ��
 parm : None
 retn : ���� �� ��ȯ
*/
DOUBLE CDrawModel::GetImageRate()
{
	LPG_CASI pstSpec	= NULL;
	CRect rDraw (&m_rDraw);

	/* Align Camera Spec. ��� */
	pstSpec	= &uvEng_GetConfig()->acam_spec;

	/* �׸����� �ϴ� ��ũ�� ũ�� �� ��� (ī�޶� ���� 1 �ȼ��� ũ�Ⱑ �޶���) (����: Pixel) */
	m_dbMarkW	= m_dbParam[1] / pstSpec->spec_pixel_um[m_u8ACamID-1];
	m_dbMarkH	= m_dbParam[2] / pstSpec->spec_pixel_um[m_u8ACamID-1];
	/* �� ���Ŀ� ���� Width / Height �޶����Ƿ� */
	switch (m_enModelType)
	{
	/* Enable : Param 2 */
	case ENG_MMDT::en_circle	:
		m_dbMarkW	= m_dbParam[1] / pstSpec->spec_pixel_um[m_u8ACamID-1];
		m_dbMarkH	= m_dbParam[1] / pstSpec->spec_pixel_um[m_u8ACamID-1];		break;
	case ENG_MMDT::en_square	:
		m_dbMarkW	= (m_dbParam[1]) / pstSpec->spec_pixel_um[m_u8ACamID-1];
		m_dbMarkH	= (m_dbParam[1]) / pstSpec->spec_pixel_um[m_u8ACamID-1];	break;
	case ENG_MMDT::en_ring		:
		m_dbMarkW	= m_dbParam[2] / pstSpec->spec_pixel_um[m_u8ACamID-1];
		m_dbMarkH	= m_dbParam[2] / pstSpec->spec_pixel_um[m_u8ACamID-1];		break;
	case ENG_MMDT::en_image		:
		m_dbMarkW	= (m_rDraw.right - m_rDraw.left) - 2;
		m_dbMarkH	= (m_rDraw.bottom - m_rDraw.top) - 2;	break;
#if 0	/* �Ʒ� Height ���� ������ �̹� ���� */
	case en_mmdt_cross		:
	case en_mmdt_ellipse	:
	case en_mmdt_rectangle	:	
	case en_mmdt_triangle	:
	case en_mmdt_diamond	:
		break;
#endif
	}

	/* ���� �ڸ� �ױ⸮ ���� */
 	rDraw.DeflateRect(1, 1);
	/* ���� ũ�� ������ ���� ũ�� �������� ū ��� */
	if (DOUBLE(rDraw.Width()) / m_dbMarkW > DOUBLE(rDraw.Height()) / m_dbMarkH)
	{
		return DOUBLE(rDraw.Height()) / m_dbMarkH;	/* <����> ���� �������� ���� Ȯ�� or ��� ���� */
	}
	/* ���� ũ�� ������ ���� ũ�� �������� ū ��� */
	else
	{
		return DOUBLE(rDraw.Width()) / m_dbMarkW;		/* <����> ���� �������� ���� Ȯ�� or ��� ���� */
	}
}

/*
 desc : �� �׸���
 parm : dc	- [in]  �θ� DC ?
 retn : None
*/
BOOL CDrawModel::DrawModel(HDC dc) 
{
	UINT32 u32DstW, u32DstH, u32MoveX, u32MoveY;
	HDC hMemDC;
	HBITMAP hBmp, hOldBmp;
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;
	CRect rDraw (&m_rDraw);
#if 1

	/* ���� �ڵ� ������ ũ�⿡ �°� ����ϱ� ���� ���� �� */
	DOUBLE dbRate	= GetImageRate();

 	/* ���� �ڸ� �ױ�� ���� */
 	rDraw.DeflateRect(0, 0);
	if (m_enModelType != ENG_MMDT::en_none)
	{
		/* ���� �̹��� ������ �°� ���� �̹��� ũ�� ���� */
		u32DstW	= (UINT32)ROUNDED(dbRate * m_dbMarkW, 0);
		u32DstH	= (UINT32)ROUNDED(dbRate * m_dbMarkH, 0);
		/* Memory DC�� ��µ� ���� Offset ���� */
		u32MoveX= u32MoveY = 0;
		if (u32DstW < (UINT32)rDraw.Width())	u32MoveX = (rDraw.Width() - u32DstW) / 2;
		if (u32DstH < (UINT32)rDraw.Height())	u32MoveY = (rDraw.Height() - u32DstH) / 2;

		/* Mmemory DC & Bitmap ���� �� ���� */
		hMemDC		= ::CreateCompatibleDC(dc);
		hBmp		= ::CreateCompatibleBitmap(dc, u32DstW, u32DstH);
		hOldBmp		= (HBITMAP)::SelectObject(hMemDC, hBmp);
		hPen		= ::CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
		hOldPen		= (HPEN)::SelectObject(hMemDC, hPen);
		hBrush		= ::CreateSolidBrush(RGB(255, 255, 255));
		hOldBrush	= (HBRUSH)::SelectObject(hMemDC, hBrush);

		/* Memory DC ������ �ϴ� ������� ĥ�ϱ� */
		::Rectangle(hMemDC, 0, 0, u32DstW, u32DstH);

		/* �� ���Ŀ� ���� ���� �׸��� */
		switch (m_enModelType)
		{
		case ENG_MMDT::en_circle	:	DrawModelCircle(hMemDC, dbRate);	break;
		case ENG_MMDT::en_square	:	DrawModelSquare(hMemDC, dbRate);	break;
		case ENG_MMDT::en_ring		:	DrawModelRing(hMemDC, dbRate);		break;
		case ENG_MMDT::en_cross		:	DrawModelCross(hMemDC, dbRate);		break;
		case ENG_MMDT::en_ellipse	:	DrawModelEllipse(hMemDC, dbRate);	break;
		case ENG_MMDT::en_rectangle	:	DrawModelRectangle(hMemDC, dbRate);	break;
		case ENG_MMDT::en_triangle	:	DrawModelTriangle(hMemDC, dbRate);	break;
		case ENG_MMDT::en_diamond	:	DrawModelDiamond(hMemDC, dbRate);	break;
		case ENG_MMDT::en_image		:	DrawModelMMF(hMemDC, dbRate);		break;
		}

		/* DC ������ ����ϱ� */
		::BitBlt(dc, rDraw.left+u32MoveX, rDraw.top+u32MoveY, u32DstW, u32DstH, hMemDC, 0, 0, SRCCOPY);

		/* Memory DC ���� ��ü ���� */
		::SelectObject(hMemDC, hOldPen);
		::SelectObject(hMemDC, hOldBrush);
		::SelectObject(hMemDC, hOldBmp);
		/* Memory DC ���� ��ü ���� */
		::DeleteObject(hPen);
		::DeleteObject(hBrush);
		::DeleteObject(hBmp);
		::DeleteDC(hMemDC);
	}
	if (m_enFindType == 1) {
		return FALSE;
	}
#endif
#if 1
	/* ���������� �ܰ� �׵θ� �׸��� */
	hPen		= ::CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
	hOldPen		= (HPEN)::SelectObject(dc, hPen);
	/* DC�� �����ڸ� (������) ������ ������ �� �׸��� */
#if 0
	::MoveToEx(dc, rDraw.left-1, rDraw.top-1, NULL);
	::LineTo(dc, rDraw.right+1, rDraw.top-1);
	::LineTo(dc, rDraw.right+1, rDraw.bottom+1);
	::LineTo(dc, rDraw.left-1, rDraw.bottom+1);
	::LineTo(dc, rDraw.left-1, rDraw.top-1);
#else
	::MoveToEx(dc, rDraw.left, rDraw.top, NULL);
	::LineTo(dc, rDraw.right, rDraw.top);
	::LineTo(dc, rDraw.right, rDraw.bottom);
	::LineTo(dc, rDraw.left, rDraw.bottom);
	::LineTo(dc, rDraw.left, rDraw.top);
#endif
	/* DC ���� ��ü ���� */
	::SelectObject(dc, hOldPen);
	/* DC ���� ��ü ���� */
	::DeleteObject(hPen);
#endif
	return TRUE;
}

/*
 desc : �� �׸��� - Circle
 parm : dc	- [in]  Memory DC
		rate- [in]  ��� ���� ��� �̹��� ũ�� ���� ��
 retn : None
*/
VOID CDrawModel::DrawModelCircle(HDC dc, DOUBLE rate)
{
	UINT32 u32Width, u32Height;
	LPG_CASI pstSpec = &uvEng_GetConfig()->acam_spec;
	HBRUSH hBrushFg, hBrushBg, hOldBrush;
	RECT rDraw;

	/* ���� ���� ���� �����ϱ� */
	if (m_dbParam[0] == 1) // black
	{
		hBrushFg = ::CreateSolidBrush(RGB(50, 50, 50));
		hBrushBg = ::CreateSolidBrush(RGB(255, 255, 255));
	}
	else if(m_dbParam[0] == 2)
	{
		hBrushFg = ::CreateSolidBrush(RGB(255, 255, 255));
		hBrushBg = ::CreateSolidBrush(RGB(128, 128, 128));
	}
	else {
		hBrushFg = ::CreateSolidBrush(RGB(128, 128, 128));
		hBrushBg = ::CreateSolidBrush(RGB(255, 255, 255));
	}
	//if (m_dbParam[0] != 256.0f)
	//{
	//	hBrushFg = ::CreateSolidBrush(RGB(255, 255, 255));
	//	hBrushBg = ::CreateSolidBrush(RGB(128, 128, 128));
	//}
	//else
	//{
	//	hBrushFg = ::CreateSolidBrush(RGB(128, 128, 128));
	//	hBrushBg = ::CreateSolidBrush(RGB(255, 255, 255));
	//}

	/* ���� ���õ� ī�޶� ��ȣ�� ���� 1 �ȼ��� ũ�Ⱑ �޶����Ƿ� (���� : Pixel) */
	u32Width	= (UINT32)ROUNDED(m_dbParam[1] / pstSpec->spec_pixel_um[m_u8ACamID-1] * rate, 0);
	u32Height	= (UINT32)ROUNDED(m_dbParam[1] / pstSpec->spec_pixel_um[m_u8ACamID-1] * rate, 0);
	/* �׸��� ���� ���� */
	rDraw.left	= 0;
	rDraw.top	= 0;
	rDraw.right	= u32Width;
	rDraw.bottom= u32Height;
	/* Brush ���� �� ���� ä��� */
	hOldBrush	= (HBRUSH)::SelectObject(dc, hBrushBg);
	::Rectangle(dc, rDraw.left, rDraw.top, rDraw.right, rDraw.bottom);
	::SelectObject(dc, hOldBrush);
	/* Brush ���� �� �� �׸��� */
	hOldBrush	= (HBRUSH)::SelectObject(dc, hBrushFg);
	::Ellipse(dc, rDraw.left, rDraw.top, rDraw.right, rDraw.bottom);
	::SelectObject(dc, hOldBrush);

	/* Brush ��ü ���� */
	::DeleteObject(hBrushBg);
	::DeleteObject(hBrushFg);
}

/*
 desc : �� �׸��� - Ellipse
 parm : dc	- [in]  Memory DC
		rate- [in]  ��� ���� ��� �̹��� ũ�� ���� ��
 retn : None
*/
VOID CDrawModel::DrawModelEllipse(HDC dc, DOUBLE rate)
{
	UINT32 u32Width, u32Height;
	LPG_CASI pstSpec = &uvEng_GetConfig()->acam_spec;
	HBRUSH hBrushFg, hBrushBg, hOldBrush;
	RECT rDraw;

	/* ���� ���� ���� �����ϱ� */
	//if (m_dbParam[0] != 256.0f)
	//{
	//	hBrushFg = ::CreateSolidBrush(RGB(255, 255, 255));
	//	hBrushBg = ::CreateSolidBrush(RGB(128, 128, 128));
	//}
	//else
	//{
	//	hBrushFg = ::CreateSolidBrush(RGB(128, 128, 128));
	//	hBrushBg = ::CreateSolidBrush(RGB(255, 255, 255));
	//}
	if (m_dbParam[0] == 1) // black
	{
		hBrushFg = ::CreateSolidBrush(RGB(50, 50, 50));
		hBrushBg = ::CreateSolidBrush(RGB(255, 255, 255));
	}
	else if (m_dbParam[0] == 2)
	{
		hBrushFg = ::CreateSolidBrush(RGB(255, 255, 255));
		hBrushBg = ::CreateSolidBrush(RGB(128, 128, 128));
	}
	else {
		hBrushFg = ::CreateSolidBrush(RGB(128, 128, 128));
		hBrushBg = ::CreateSolidBrush(RGB(255, 255, 255));
	}

	/* ���� ���õ� ī�޶� ��ȣ�� ���� 1 �ȼ��� ũ�Ⱑ �޶����Ƿ� (���� : Pixel) */
	u32Width	= (UINT32)ROUNDED(m_dbParam[1] / pstSpec->spec_pixel_um[m_u8ACamID-1] * rate, 0);
	u32Height	= (UINT32)ROUNDED(m_dbParam[2] / pstSpec->spec_pixel_um[m_u8ACamID-1] * rate, 0);
	/* �׸��� ���� ���� */
	rDraw.left	= 0;
	rDraw.top	= 0;
	rDraw.right	= u32Width;
	rDraw.bottom= u32Height;
	/* Brush ���� �� ���� ä��� */
	hOldBrush	= (HBRUSH)::SelectObject(dc, hBrushBg);
	::Rectangle(dc, rDraw.left, rDraw.top, rDraw.right, rDraw.bottom);
	::SelectObject(dc, hOldBrush);
	/* Brush ���� �� �� �׸��� */
	hOldBrush	= (HBRUSH)::SelectObject(dc, hBrushFg);
	::Ellipse(dc, rDraw.left, rDraw.top, rDraw.right, rDraw.bottom);
	::SelectObject(dc, hOldBrush);

	/* Brush ��ü ���� */
	::DeleteObject(hBrushBg);
	::DeleteObject(hBrushFg);
}

/*
 desc : �� �׸��� - Square
 parm : dc	- [in]  Memory DC
		rate- [in]  ��� ���� ��� �̹��� ũ�� ���� ��
 retn : None
*/
VOID CDrawModel::DrawModelSquare(HDC dc, DOUBLE rate)
{
	UINT32 u32Width, u32Height;
	LPG_CASI pstSpec = &uvEng_GetConfig()->acam_spec;
	HBRUSH hBrushFg, hOldBrush;
	RECT rDraw;

	/* ���� ���� ���� �����ϱ� */ 
	if (m_dbParam[0] == 1) { // black
		hBrushFg = ::CreateSolidBrush(RGB(255, 255, 255)); //white
	}
	else if (m_dbParam[0] == 2) {
		hBrushFg = ::CreateSolidBrush(RGB(128, 128, 128)); // gray
	}
	else  {
		hBrushFg = ::CreateSolidBrush(RGB(128, 128, 128));
	}

	/* ���� ���õ� ī�޶� ��ȣ�� ���� 1 �ȼ��� ũ�Ⱑ �޶����Ƿ� (���� : Pixel) */
	u32Width	= (UINT32)ROUNDED(m_dbParam[1] / pstSpec->spec_pixel_um[m_u8ACamID-1] * rate, 0);
	u32Height	= (UINT32)ROUNDED(m_dbParam[1] / pstSpec->spec_pixel_um[m_u8ACamID-1] * rate, 0);
	/* �׸��� ���� ���� */
	rDraw.left	= 0;
	rDraw.top	= 0;
	rDraw.right	= u32Width;
	rDraw.bottom= u32Height;
	/* Brush ���� �� ���� ä��� */
	hOldBrush	= (HBRUSH)::SelectObject(dc, hBrushFg);
	::Rectangle(dc, rDraw.left, rDraw.top, rDraw.right, rDraw.bottom);
	::SelectObject(dc, hOldBrush);

	/* Brush ��ü ���� */
	::DeleteObject(hBrushFg);
}

/*
 desc : �� �׸��� - Ring
 parm : dc	- [in]  Memory DC
		rate- [in]  ��� ���� ��� �̹��� ũ�� ���� ��
 retn : None
*/
VOID CDrawModel::DrawModelRing(HDC dc, DOUBLE rate)
{
	UINT32 u32Inner, u32Outer;
	LPG_CASI pstSpec = &uvEng_GetConfig()->acam_spec;
	HBRUSH hBrushFg, hBrushBg, hOldBrush;
	RECT rDrawIn, rDrawOut;

	/* ���� ���� ���� �����ϱ� */
	//if (m_dbParam[0] != 256.0f)
	//{
	//	hBrushFg = ::CreateSolidBrush(RGB(255, 255, 255));
	//	hBrushBg = ::CreateSolidBrush(RGB(128, 128, 128));
	//}
	//else
	//{
	//	hBrushFg = ::CreateSolidBrush(RGB(128, 128, 128));
	//	hBrushBg = ::CreateSolidBrush(RGB(255, 255, 255));
	//}

	if (m_dbParam[0] == 1) // black
	{
		hBrushFg = ::CreateSolidBrush(RGB(50, 50, 50));
		hBrushBg = ::CreateSolidBrush(RGB(255, 255, 255));
	}
	else if (m_dbParam[0] == 2)
	{
		hBrushFg = ::CreateSolidBrush(RGB(255, 255, 255));
		hBrushBg = ::CreateSolidBrush(RGB(128, 128, 128));
	}
	else {
		hBrushFg = ::CreateSolidBrush(RGB(128, 128, 128));
		hBrushBg = ::CreateSolidBrush(RGB(255, 255, 255));
	}

	/* ���� ���õ� ī�޶� ��ȣ�� ���� 1 �ȼ��� ũ�Ⱑ �޶����Ƿ� (���� : Pixel) */
	u32Inner	= (UINT32)ROUNDED(m_dbParam[1] / pstSpec->spec_pixel_um[m_u8ACamID-1] * rate, 0);
	u32Outer	= (UINT32)ROUNDED(m_dbParam[2] / pstSpec->spec_pixel_um[m_u8ACamID-1] * rate, 0);
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
	hOldBrush	= (HBRUSH)::SelectObject(dc, hBrushBg);
	::Rectangle(dc, rDrawOut.left, rDrawOut.top, rDrawOut.right, rDrawOut.bottom);
	::SelectObject(dc, hOldBrush);
	/* <��>�� �� ä��� */
	hOldBrush	= (HBRUSH)::SelectObject(dc, hBrushFg);
	::Ellipse(dc, rDrawOut.left, rDrawOut.top, rDrawOut.right, rDrawOut.bottom);
	::SelectObject(dc, hOldBrush);
	/* <��>�� �� ä��� */
	hOldBrush	= (HBRUSH)::SelectObject(dc, hBrushBg);
	::Ellipse(dc, rDrawIn.left, rDrawIn.top, rDrawIn.right, rDrawIn.bottom);
	::SelectObject(dc, hOldBrush);

	/* Brush ��ü ���� */
	::DeleteObject(hBrushBg);
	::DeleteObject(hBrushFg);
}

/*
 desc : �� �׸��� - Rectangle
 parm : dc	- [in]  Memory DC
		rate- [in]  ��� ���� ��� �̹��� ũ�� ���� ��
 retn : None
*/
VOID CDrawModel::DrawModelRectangle(HDC dc, DOUBLE rate)
{
	UINT32 u32Width, u32Height;
	LPG_CASI pstSpec = &uvEng_GetConfig()->acam_spec;
	HBRUSH hBrushFg, hOldBrush;
	RECT rDraw;

	/* ���� ���� ���� �����ϱ� */
	/*if (m_dbParam[0] < 256.0f)
	{
		hBrushFg = ::CreateSolidBrush(RGB(255, 255, 255));
	}
	else
	{
		hBrushFg = ::CreateSolidBrush(RGB(128, 128, 128));
	}*/
	if (m_dbParam[0] == 1) // black
	{
		hBrushFg = ::CreateSolidBrush(RGB(50, 50, 50));
	}
	else if (m_dbParam[0] == 2)
	{
		hBrushFg = ::CreateSolidBrush(RGB(255, 255, 255));
	}
	else {
		hBrushFg = ::CreateSolidBrush(RGB(128, 128, 128));
	}


	/* ���� ���õ� ī�޶� ��ȣ�� ���� 1 �ȼ��� ũ�Ⱑ �޶����Ƿ� (���� : Pixel) */
	u32Width	= (UINT32)ROUNDED(m_dbParam[1] / pstSpec->spec_pixel_um[m_u8ACamID-1] * rate, 0);
	u32Height	= (UINT32)ROUNDED(m_dbParam[2] / pstSpec->spec_pixel_um[m_u8ACamID-1] * rate, 0);
	/* �׸��� ���� ���� */
	rDraw.left	= 0;
	rDraw.top	= 0;
	rDraw.right	= u32Width;
	rDraw.bottom= u32Height;
	/* Brush ���� �� �� �׸��� */
	hOldBrush	= (HBRUSH)::SelectObject(dc, hBrushFg);
	::Rectangle(dc, rDraw.left, rDraw.top, rDraw.right, rDraw.bottom);
	::SelectObject(dc, hOldBrush);

	/* Brush ��ü ���� */
	::DeleteObject(hBrushFg);
}

/*
 desc : �� �׸��� - Triangle
 parm : dc	- [in]  Memory DC
		rate- [in]  ��� ���� ��� �̹��� ũ�� ���� ��
 retn : None
*/
VOID CDrawModel::DrawModelTriangle(HDC dc, DOUBLE rate)
{
	UINT32 u32Width, u32Height;
	LPG_CASI pstSpec = &uvEng_GetConfig()->acam_spec;
	HBRUSH hBrushFg, hBrushBg, hOldBrush;
	POINT ptVertex[3];
	RECT rDraw;

	/* ���� ���� ���� �����ϱ� */
	//if (m_dbParam[0] != 256.0f)
	//{
	//	hBrushFg = ::CreateSolidBrush(RGB(255, 255, 255));
	//	hBrushBg = ::CreateSolidBrush(RGB(128, 128, 128));
	//}
	//else
	//{
	//	hBrushFg = ::CreateSolidBrush(RGB(128, 128, 128));
	//	hBrushBg = ::CreateSolidBrush(RGB(255, 255, 255));
	//}
	if (m_dbParam[0] == 1) // black
	{
		hBrushFg = ::CreateSolidBrush(RGB(50, 50, 50));
		hBrushBg = ::CreateSolidBrush(RGB(255, 255, 255));
	}
	else if (m_dbParam[0] == 2)
	{
		hBrushFg = ::CreateSolidBrush(RGB(255, 255, 255));
		hBrushBg = ::CreateSolidBrush(RGB(128, 128, 128));
	}
	else {
		hBrushFg = ::CreateSolidBrush(RGB(128, 128, 128));
		hBrushBg = ::CreateSolidBrush(RGB(255, 255, 255));
	}

	/* ���� ���õ� ī�޶� ��ȣ�� ���� 1 �ȼ��� ũ�Ⱑ �޶����Ƿ� (���� : Pixel) */
	u32Width	= (UINT32)ROUNDED(m_dbParam[1] / pstSpec->spec_pixel_um[m_u8ACamID-1] * rate, 0);
	u32Height	= (UINT32)ROUNDED(m_dbParam[2] / pstSpec->spec_pixel_um[m_u8ACamID-1] * rate, 0);
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

	/* Brush ���� �� ���� ä��� */
	hOldBrush	= (HBRUSH)::SelectObject(dc, hBrushBg);
	::Rectangle(dc, rDraw.left, rDraw.top, rDraw.right, rDraw.bottom);
	::SelectObject(dc, hOldBrush);
	/* <��>�� �ﰢ�� ä��� */
	hOldBrush	= (HBRUSH)::SelectObject(dc, hBrushFg);
	Polygon(dc, ptVertex, 3);
	::SelectObject(dc, hOldBrush);

	/* Brush ��ü ���� */
	::DeleteObject(hBrushBg);
	::DeleteObject(hBrushFg);
}

/*
 desc : �� �׸��� - Diamond
 parm : dc	- [in]  Memory DC
		rate- [in]  ��� ���� ��� �̹��� ũ�� ���� ��
 retn : None
*/
VOID CDrawModel::DrawModelDiamond(HDC dc, DOUBLE rate)
{
	UINT32 u32Width, u32Height;
	LPG_CASI pstSpec = &uvEng_GetConfig()->acam_spec;
	HBRUSH hBrushFg, hBrushBg, hOldBrush;
	POINT ptVertex[4];
	RECT rDraw;

	/* ���� ���� ���� �����ϱ� */
	//if (m_dbParam[0] != 256.0f)
	//{
	//	hBrushFg = ::CreateSolidBrush(RGB(255, 255, 255));
	//	hBrushBg = ::CreateSolidBrush(RGB(128, 128, 128));
	//}
	//else
	//{
	//	hBrushFg = ::CreateSolidBrush(RGB(128, 128, 128));
	//	hBrushBg = ::CreateSolidBrush(RGB(255, 255, 255));
	//}
	if (m_dbParam[0] == 1) // black
	{
		hBrushFg = ::CreateSolidBrush(RGB(50, 50, 50));
		hBrushBg = ::CreateSolidBrush(RGB(255, 255, 255));
	}
	else if (m_dbParam[0] == 2)
	{
		hBrushFg = ::CreateSolidBrush(RGB(255, 255, 255));
		hBrushBg = ::CreateSolidBrush(RGB(128, 128, 128));
	}
	else {
		hBrushFg = ::CreateSolidBrush(RGB(128, 128, 128));
		hBrushBg = ::CreateSolidBrush(RGB(255, 255, 255));
	}

	/* ���� ���õ� ī�޶� ��ȣ�� ���� 1 �ȼ��� ũ�Ⱑ �޶����Ƿ� (���� : Pixel) */
	u32Width	= (UINT32)ROUNDED(m_dbParam[1] / pstSpec->spec_pixel_um[m_u8ACamID-1] * rate, 0);
	u32Height	= (UINT32)ROUNDED(m_dbParam[2] / pstSpec->spec_pixel_um[m_u8ACamID-1] * rate, 0);
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

	/* Brush ���� �� ���� ä��� */
	hOldBrush	= (HBRUSH)::SelectObject(dc, hBrushBg);
	::Rectangle(dc, rDraw.left, rDraw.top, rDraw.right, rDraw.bottom);
	::SelectObject(dc, hOldBrush);
	/* <��>�� �� ä��� */
	hOldBrush	= (HBRUSH)::SelectObject(dc, hBrushFg);
	::Polygon(dc, ptVertex, 4);
	::SelectObject(dc, hOldBrush);

	/* Brush ��ü ���� */
	::DeleteObject(hBrushBg);
	::DeleteObject(hBrushFg);
}

/*
 desc : �� �׸��� - Cross
 parm : dc	- [in]  Memory DC
		rate- [in]  ��� ���� ��� �̹��� ũ�� ���� ��
 retn : None
*/
VOID CDrawModel::DrawModelCross(HDC dc, DOUBLE rate)
{
	UINT32 u32WidthOut, u32HeightOut, u32WidthIn, u32HeightIn;
	LPG_CASI pstSpec = &uvEng_GetConfig()->acam_spec;
	HBRUSH hBrushFg, hBrushBg, hOldBrush;
	POINT ptVertex[12];
	RECT rDraw;

	/* ���� ���� ���� �����ϱ� */
	/*if (m_dbParam[0] != 256.0f)
	{
		hBrushFg = ::CreateSolidBrush(RGB(255, 255, 255));
		hBrushBg = ::CreateSolidBrush(RGB(128, 128, 128));
	}
	else
	{
		hBrushFg = ::CreateSolidBrush(RGB(128, 128, 128));
		hBrushBg = ::CreateSolidBrush(RGB(255, 255, 255));
	}*/
	if (m_dbParam[0] == 1) // black
	{
		hBrushFg = ::CreateSolidBrush(RGB(50, 50, 50));
		hBrushBg = ::CreateSolidBrush(RGB(255, 255, 255));
	}
	else if (m_dbParam[0] == 2)
	{
		hBrushFg = ::CreateSolidBrush(RGB(255, 255, 255));
		hBrushBg = ::CreateSolidBrush(RGB(128, 128, 128));
	}
	else {
		hBrushFg = ::CreateSolidBrush(RGB(128, 128, 128));
		hBrushBg = ::CreateSolidBrush(RGB(255, 255, 255));
	}

	/* ���� ���õ� ī�޶� ��ȣ�� ���� 1 �ȼ��� ũ�Ⱑ �޶����Ƿ� (���� : Pixel) */
	u32WidthOut	= (UINT32)ROUNDED(m_dbParam[1] / pstSpec->spec_pixel_um[m_u8ACamID-1] * rate, 0);
	u32HeightOut= (UINT32)ROUNDED(m_dbParam[2] / pstSpec->spec_pixel_um[m_u8ACamID-1] * rate, 0);
	u32WidthIn	= (UINT32)ROUNDED(m_dbParam[3] / pstSpec->spec_pixel_um[m_u8ACamID-1] * rate, 0);
	u32HeightIn	= (UINT32)ROUNDED(m_dbParam[4] / pstSpec->spec_pixel_um[m_u8ACamID-1] * rate, 0);
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

	/* Brush ���� �� ���� ä��� */
	hOldBrush	= (HBRUSH)::SelectObject(dc, hBrushBg);
	::Rectangle(dc, rDraw.left, rDraw.top, rDraw.right, rDraw.bottom);
	::SelectObject(dc, hOldBrush);
	/* <����/����> �簢�� ä��� */
	hOldBrush	= (HBRUSH)::SelectObject(dc, hBrushFg);
	::Polygon(dc, ptVertex, 12);
	::SelectObject(dc, hOldBrush);

	/* Brush ��ü ���� */
	::DeleteObject(hBrushBg);
	::DeleteObject(hBrushFg);
}

/*
 desc : �� �׸��� - MMF Image File Name
 parm : dc	- [in]  Memory DC
		rate- [in]  ��� ���� ��� �̹��� ũ�� ���� ��
 retn : None
*/
VOID CDrawModel::DrawModelMMF(HDC dc, DOUBLE rate)
{
	INT32 i32BkMode	= 0;
	HFONT hFont, hOldFont;
	HBRUSH hBrushBg, hOldBrush;
	POINT ptText	= {NULL};
	RECT rDraw;

	/* ���� ���� ���� �����ϱ� */
	hBrushBg = ::CreateSolidBrush(RGB(128, 128, 128));

	/* �׸��� ���� ���� */
	rDraw.left	= 1;
	rDraw.top	= 1;
	rDraw.right	= m_rDraw.right - m_rDraw.left - 1;
	rDraw.bottom= m_rDraw.bottom - m_rDraw.top - 1;
	/* �ؽ�Ʈ ��� ��ġ */
	ptText.x	= (rDraw.right - rDraw.left) / 2;
	ptText.y	= (rDraw.bottom - rDraw.top) / 2;
	/* Brush ���� �� �� �׸��� */
	hOldBrush	= (HBRUSH)::SelectObject(dc, hBrushBg);
	::Rectangle(dc, rDraw.left, rDraw.top, rDraw.right, rDraw.bottom);
	::SelectObject(dc, hOldBrush);
	/* Brush ��ü ���� */
	::DeleteObject(hBrushBg);

	/* ��Ʈ ���� */
	hFont	= ::CreateFont(13,								/* ���� �� */
						   0,								/* ���� ���� */
						   0,								/* ���� ���� */
						   0,								/* ���� ���� */
						   FW_NORMAL,						/* ���� ���� */
						   FALSE,							/* ���� */
						   FALSE,							/* ���� */
						   0,								/* ��Ҽ� */
						   ANSI_CHARSET/*HANGEUL_CHARSET*/,	/* Character Set */
						   OUT_DEFAULT_PRECIS,				/* ��� ��Ȯ�� */
						   CLIP_DEFAULT_PRECIS,				/* Ŭ���� ��Ȯ�� */
						   DEFAULT_QUALITY,					/* ����� �� */
						   DEFAULT_PITCH|FF_SWISS,			/* ���� ���� (�ڰ�) */
						   L"D2Coding");					/* ��Ʈ �̸� */
	hOldFont	= (HFONT)::SelectObject(dc, hFont);
	i32BkMode	= ::SetBkMode(dc, TRANSPARENT);
	/* �ؽ�Ʈ ��� */
	::SetTextAlign(dc, TA_BOTTOM|TA_CENTER);
	::TextOut(dc, ptText.x, ptText.y, m_tzModelName, (INT32)wcslen(m_tzModelName));
	::SelectObject(dc, hOldBrush);
	/* �ؽ�Ʈ ���� ���� */
	::SetBkMode(dc, i32BkMode);
	/* Font ��ü ���� */
	::DeleteObject(hOldFont);
}

/*
 desc : �� ũ�Ⱑ �������� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDrawModel::IsModelSize(DOUBLE size)
{
	if (size != 0.0f)	return (size == m_dbParam[1]);
	return FALSE;
}
