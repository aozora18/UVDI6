
/*
 desc : Support the Matrox Image Library
*/

#include "pch.h"
#include "MainApp.h"
#include "../../inc/comn/Picture.h"

//#include "MilMain.h"

class AlignMotion;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                           ���� ����                                           */
/* --------------------------------------------------------------------------------------------- */

/* Grabbed Image Size */
UINT32						g_u32GrabWidth		= 0;
UINT32						g_u32GrabHeight		= 0;

/* ��Ƽ ��ũ ��, Grabbed Image ������ ���� ���� ��ũ�� �˻��ϴ� ���, ��ũ�� ǥ���� ����/���� ���� (����: um) */
DOUBLE						g_dbMuliAreaWidth	= 0.0f;	/* pixel */
DOUBLE						g_dbMuliAreaHeight	= 0.0f;	/* pixel */

PUINT8						g_pGrabBuff			= NULL;		/* The buffer of image for temporary storage */

LPBITMAPINFO				g_pBmpInfo			= NULL;

//CMilMain					*g_pMilMain			= NULL;

AlignMotion* alignMotionPtr = nullptr;

/* ----------------------------------------------------------------------------------------- */
/*                                 lk91 VISION �߰� �Լ�                                     */
/* ----------------------------------------------------------------------------------------- */
/*
 desc : Drawing - Align(�˻��� Mark) Image (Bitmap�� �̿��Ͽ� ���)
 parm : hdc		- [in]  �̹����� ��� ��� context
		draw	- [in]  �̹����� ��µ� ���� (�θ� ���� ���� ��� ��ǥ)
		grab	- [in]  Grabbed Image ������ ����� ����ü ������
		find	- [in]  ��ũ �˻� ���� ���� (�˻��ƴ���, �˻� ��� ���� ���� �޶���)
						0x00 - �˻� ��� ����, 0x01 - �˻� ��� ����
 retn : None
*/
//VOID DrawGrabMarkBitmap_MMPM(HDC hdc, RECT draw, LPG_ACGR grab, UINT8 find)
VOID DrawGrabMarkBitmap_MMPM(HDC hdc, RECT draw, LPG_ACGR grab, INT32 Grab_No, UINT8 find)
{
	UINT8 i, u8Find;
	//DOUBLE dbRate;
	HDC hDC = NULL;
	//POINT ptMove;
	LPG_GMSR pstFind;
	RECT rDraw(draw);
	//CSize szSrc(grab->grab_width, grab->grab_height), szTgt;

	///* ������ �����Ͱ� ���� ������� */
	//if (grab->grab_width < 1 || grab->grab_height < 1 || !grab->grab_data)	return;
	///* ���ο� ������ �̹��� ũ�� �� ��, ���� ���̳��� �κ��� �������� ���� */
	//if (!ResizeDrawBitmap(rDraw, szSrc, szTgt, dbRate))	return;
	//if (FLT_MAX == dbRate)	return;
	///* �̹��� ��� ���� ��ġ ��� */
	//ptMove.x = (LONG)ROUNDED(DOUBLE((rDraw.right - rDraw.left) - szTgt.cx) / 2.0f, 0) + rDraw.left;
	//ptMove.y = (LONG)ROUNDED(DOUBLE((rDraw.bottom - rDraw.top) - szTgt.cy) / 2.0f, 0) + rDraw.top;

	/* Grabbed Image ��� */
	/* Memory DC ������ Live Image ��� �׸��� */
	CDPoint dRateP;
#ifndef _NOT_USE_MIL_
	dRateP = theApp.clMilDisp.GetRateDispToBuf(DISP_TYPE_MMPM);
	MbufClear(theApp.clMilMain.m_mImgDisp_MMPM, 0L);
	MimResize(theApp.clMilMain.m_mImg_Grab[Grab_No], theApp.clMilMain.m_mImgDisp_MMPM, dRateP.x, dRateP.y, M_DEFAULT);

	theApp.clMilDisp.DrawOverlayDC(false, DISP_TYPE_MMPM, 0);
	theApp.clMilDisp.DrawBase(DISP_TYPE_MMPM, 0);
	//theApp.clMilDisp.DrawOverlayDC(true, 4, 0);

	//DrawGrabImage(hdc, grab, draw, &szTgt, &ptMove); 
	/* Grabbed Image�� �߽ɿ� ��ȣ (��ü ����) ('+') ��� */
	// //DrawGrabImageCent(hdc, grab, dbRate, &ptMove);
	//DrawGrabImageCent(hdc, draw); 
	if (ENG_MMSM::en_cent_side == theApp.clMilMain.GetMarkMethod(grab->cam_id))
	{
		//DrawGrabMarkDiffText(hdc, grab, &ptMove);
		CString sTmp;
		sTmp.Format(_T("(x:%+.4f y:%+.4f um)"), grab->move_mm_x, grab->move_mm_y);
		theApp.clMilDisp.AddTextList(4, 0, 50, 50, sTmp, eM_COLOR_RED, 10, 25, VISION_FONT_TEXT, true);
	}

	/* �� �˻��� Mark�� ���� ���� ǥ�� (���) */
	u8Find = theApp.clMilMain.GetGrabbedMarkCount(grab->cam_id, grab->img_id);
	pstFind = theApp.clMilMain.GetGrabbedMarkAll(grab->cam_id, grab->img_id);
	for (i = 0; i < u8Find; i++)
	{
		/* Grabbed Image���� Mark�� �߽� ��ġ�� ��ȣ ('+') ��� */
		//DrawGrabMarkArea(hdc, grab, &pstFind[i], dbRate, &ptMove, find);
		theApp.clMilDisp.AddCrossList(DISP_TYPE_MMPM, 0, (int)pstFind[i].cent_x, (int)pstFind[i].cent_y, 20, 20, eM_COLOR_GREEN);
	}

	theApp.clMilDisp.DrawOverlayDC(true, DISP_TYPE_MMPM, 0);
#endif
}


/* --------------------------------------------------------------------------------------------- */
/*                                           ���� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Grabbed Image ���濡 ����, ��� ���� �޸� �� ��Ʈ�� ���� ����
 parm : grap_width	- [in]  Grabbed Image Size (Width)
		grap_height	- [in]  Grabbed Image Size (Height)
		bit_count	- [in]  bit per counts
 retn : TRUE or FALSE
*/
BOOL ReAllocGrabImage(UINT32 width, UINT32 height, UINT8 bit_count)
{
	UINT32 i, u32ColorSize	= 0;

	if (g_pBmpInfo)	delete [] g_pBmpInfo;

	/* �޸� �Ҵ� */
	if (bit_count <= 8)	u32ColorSize	= UINT32(1 << bit_count) * (UINT32)sizeof(RGBQUAD);
	g_pBmpInfo	= (LPBITMAPINFO)new UINT8 [sizeof(BITMAPINFOHEADER) + u32ColorSize];

	/* Bitmap Header ���� */
	g_pBmpInfo->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	g_pBmpInfo->bmiHeader.biWidth			= width; //�̹��� ���� ũ�⸦ ����.  
	g_pBmpInfo->bmiHeader.biHeight			= -1 /* image invert */ * height;
	g_pBmpInfo->bmiHeader.biPlanes			= 1;
	g_pBmpInfo->bmiHeader.biBitCount		= bit_count;
	g_pBmpInfo->bmiHeader.biCompression		= BI_RGB;
	g_pBmpInfo->bmiHeader.biXPelsPerMeter	= 0;
	g_pBmpInfo->bmiHeader.biYPelsPerMeter	= 0;
	g_pBmpInfo->bmiHeader.biClrUsed			= (bit_count <= 8) ? (1 << bit_count): 0;
	g_pBmpInfo->bmiHeader.biClrImportant	= (bit_count <= 8) ? (1 << bit_count) : 0;
	g_pBmpInfo->bmiHeader.biSizeImage		= height * WIDTHBYTES(bit_count, width);

	/*     Palette ����		*/
	/* 1 Bit = 2^0 =   1 ��	*/
	/* 2 Bit = 2^2 =   4 ��	*/
	/* 4 Bit = 2^4 =  16 ��	*/
	/* 8 Bit = 2^8 = 256 ��	*/
	for (i=0; i<g_pBmpInfo->bmiHeader.biClrUsed; i++)
	{
		g_pBmpInfo->bmiColors[i].rgbBlue	= UINT8(i);
		g_pBmpInfo->bmiColors[i].rgbGreen	= UINT8(i);
		g_pBmpInfo->bmiColors[i].rgbRed		= UINT8(i);
		g_pBmpInfo->bmiColors[i].rgbReserved= 0;
	}

	return TRUE;
}

/*
 desc : �̹����� ���� ũ�⸦ ��� ����� ���� ������ �°� �׸��� ���� �̹��� ũ�� �� ���� �� ��ȯ
 parm : draw_area	- [in]  �̹����� ��µǴ� ������ ���� (��ǥ)
		src_size	- [in]  ���� �̹����� ũ�� (����: Pixel)
		tgt_size	- [out] ���� ������ ��µǴ� �̹����� ũ�� (����: pxiel)
		tgt_rate	- [out] ���� �̹��� ��� ��� ������ ���� ũ�� ���� ��
							���� �̹��� ũ�⸦ 1 ������ ����, ��ȯ�� ���� 1.0 ���� ũ�� Ȯ��, ������ ���
 retn : TRUE or FALSE
*/
BOOL ResizeDrawBitmap(RECT draw_area, CSize src_size, CSize &tgt_size, DOUBLE &tgt_rate)
{
	CSize szDraw(draw_area.right-draw_area.left, draw_area.bottom-draw_area.top);

	/* �ݵ�� 1.0f ������ �ʱ�ȭ */
	tgt_rate	= 1.0f;

	/* ���� ũ�� ������ ���� ũ�� �������� ū ��� */
	if ((DOUBLE(szDraw.cx) / DOUBLE(src_size.cx)) > (DOUBLE(szDraw.cy) / DOUBLE(src_size.cy)))
	{
		/* <����> ���� �������� ���� Ȯ�� or ��� ���� */
		tgt_rate	= DOUBLE(szDraw.cy) / DOUBLE(src_size.cy);
	}
	/* ���� ũ�� ������ ���� ũ�� �������� ū ��� */
	else
	{
		/* <����> ���� �������� ���� Ȯ�� or ��� ���� */
		tgt_rate	= DOUBLE(szDraw.cx) / DOUBLE(src_size.cx);
	}
	/* ���� �̹��� ������ �°� ���� �̹��� ũ�� ���� */
	tgt_size.cx	= (LONG)ROUNDED(tgt_rate * src_size.cx, 0);
	tgt_size.cy	= (LONG)ROUNDED(tgt_rate * src_size.cy, 0);

	/* ���� Grabbed Image�� ũ�Ⱑ �ٸ� ���, �޸� �ٽ� �Ҵ� */
	if (g_u32GrabWidth != src_size.cx || g_u32GrabHeight != src_size.cy)
	{
		if (!ReAllocGrabImage(src_size.cx, src_size.cy, 8))	return	FALSE;

		/* ������ ������ �̹����� ũ�� ���� */
		g_u32GrabWidth	= src_size.cx;
		g_u32GrabHeight	= src_size.cy;
	}

	return TRUE;
}

/*
 desc : �˻��� Mark�� ������ ���� �̹��� ũ�� ������
 parm : in_size		- [in]  �˻��� ��ũ �̹��� ũ�� (Pixel)
		in_cent		- [in]  �˻��� ��ũ �߽� ��ġ (Pixel)
		resize_rate	- [in]  ��� Ȥ�� Ȯ�� ����
		out_cent	- [out] ���� �׷��� Mark �߽� ��ǥ
		out_line	- [out] ���� �׷��� Mark 4 ���� ��ǥ
 retn : None
*/
VOID GetDrawMarkArea(LPG_DBWH in_size, LPG_DBXY in_cent, DOUBLE resize_rate,
					 POINT &out_cent, PPOINT out_line)
{
	STG_DBXY stCent;
	STG_DBWH stSize;
	DOUBLE dbLineX[4], dbLineY[4];

	/* �̹����� ��µ� ������ ũ�� (����)�� ���� Mark ������ �׷��� ũ�� ���� */
	stSize.cx	= in_size->cx * resize_rate;
	stSize.cy	= in_size->cy * resize_rate;
	/* �˻��� ��ũ�� �߽� ��ġ ���� */
	stCent.x	= in_cent->x * resize_rate;
	stCent.y	= in_cent->y * resize_rate;
	/* ��ũ�� ��� or Ȯ�뿡 ���� ��ġ ���� */
	dbLineX[0]	= stCent.x - stSize.cx / 2.0f;
	dbLineY[0]	= stCent.y - stSize.cy / 2.0f;
	dbLineX[1]	= dbLineX[0]+ stSize.cx;
	dbLineY[1]	= dbLineY[0];
	dbLineX[2]	= dbLineX[0]+ stSize.cx;
	dbLineY[2]	= dbLineY[0]+ stSize.cy;
	dbLineX[3]	= dbLineX[0];
	dbLineY[3]	= dbLineY[0]+ stSize.cy;

	/* �� ��ȯ */
	out_cent.x		= (INT32)ROUNDED(stCent.x,	0);
	out_cent.y		= (INT32)ROUNDED(stCent.y,	0);
	out_line[0].x	= (INT32)ROUNDED(dbLineX[0],0);
	out_line[0].y	= (INT32)ROUNDED(dbLineY[0],0);
	out_line[1].x	= (INT32)ROUNDED(dbLineX[1],0);
	out_line[1].y	= (INT32)ROUNDED(dbLineY[1],0);
	out_line[2].x	= (INT32)ROUNDED(dbLineX[2],0);
	out_line[2].y	= (INT32)ROUNDED(dbLineY[2],0);
	out_line[3].x	= (INT32)ROUNDED(dbLineX[3],0);
	out_line[3].y	= (INT32)ROUNDED(dbLineY[3],0);
}

/*
 desc : �ܼ� Grabbed Image ���
 parm : hdc		- [in]  DC ��ü �ڵ�
		grab	- [in]  Grabbed Image ������ ����� ����ü ������
		area	- [in]  Memory DC ������ ����Ϸ��� ũ��
		resize	- [in]  ���� ��µǴ� �̹��� ũ�� ����
		move	- [in]  ���� ��µǴ� �̹��� ���� ��ġ
 retn : None
*/
VOID DrawGrabImage(HDC hdc, LPG_ACGR grab, RECT area, CSize *resize, POINT *move, UINT8 flipFlag=0xff)
{
	DOUBLE dbAngle	= 0.0f;
	/* Memory DC ������ Grabbed Image ��� �� �˻��� ��� �� �׸��� */
	HDC hMemDC;
	HBITMAP hMemBmp, hMemBmpOld;

	/* �޸� DC�� ��Ʈ�� �����ϰ� ���� ���� �ֱ� */
	hMemDC	= ::CreateCompatibleDC(hdc);
	hMemBmp	= ::CreateCompatibleBitmap(hdc, grab->grab_width, grab->grab_height);
	if (!hMemBmp)
	{
		if (hMemDC)	::DeleteDC(hMemDC);
		return;
	}

	/* Memory DC�� Bitmap ���� ���� */
	hMemBmpOld	= (HBITMAP)::SelectObject(hMemDC, hMemBmp);
	if (hMemBmpOld)
	{
		/* Bitmap Memory ������ Grabbed Image ��� (ȸ�� ���ο� ����) */
		if (0 == g_pstConfig->set_cams.acam_inst_angle/* 0 degree */)
		{
			::StretchDIBits(hMemDC, 0, 0, grab->grab_width, grab->grab_height,
							0, 0, grab->grab_width, grab->grab_height, 
							grab->grab_data, g_pBmpInfo, DIB_RGB_COLORS, SRCCOPY);
			/* ȭ�� DC�� ��� */
			::SetStretchBltMode(hdc, COLORONCOLOR);	/* �̹��� ����� ��, �ּ�ȭ�ϱ� ���� ��� ���� */
			::StretchBlt(hdc, move->x, move->y, resize->cx, resize->cy,
						 hMemDC, 0, 0, grab->grab_width, grab->grab_height, SRCCOPY);
		}
		else	/* 180 degree ȸ���� �����Ƿ� */
		{
			/* ���� ���� �޸� �ʱ�ȭ */
			g_pGrabBuff[grab->grab_size]	= 0x00;	/* ����Ƿ�, �� �� �κи� ^^ */
			/* �̹��� ȸ�� ���� */
			//if (g_pMilMain)
			//{
			if(flipFlag != 0xFF)
				theApp.clMilMain.ImageFlip(grab->grab_data, g_pGrabBuff, grab->grab_width, grab->grab_height, flipFlag);
			else
				theApp.clMilMain.ImageRotate(grab->grab_data, g_pGrabBuff,
					grab->grab_width, grab->grab_height,
					180.0f/*g_pstConfig->set_cams.acam_inst_angle*/,
					grab->grab_width / 2, grab->grab_height / 2);

			//}
			/* Memory DC ������ DIB�� ��� */
			::StretchDIBits(hMemDC, 0, 0, grab->grab_width, grab->grab_height,
							0, 0, grab->grab_width, grab->grab_height, 
							g_pGrabBuff, g_pBmpInfo, DIB_RGB_COLORS, SRCCOPY);
			/* ȭ�� DC�� ��� */
			::SetStretchBltMode(hdc, COLORONCOLOR);	/* �̹��� ����� ��, �ּ�ȭ�ϱ� ���� ��� ���� */
			::StretchBlt(hdc, move->x, move->y, resize->cx, resize->cy,
						 hMemDC, 0, 0, grab->grab_width, grab->grab_height, SRCCOPY);
		}
		/* Bitmap Memory ���� */
		if (hMemBmpOld)	::SelectObject(hMemDC, hMemBmpOld);
	}

	/* Memory DC & Bitmap ���� */
	if (hMemBmp)	::DeleteObject(hMemBmp);
	if (hMemDC)		::DeleteDC(hMemDC);
}

/*
 desc : Grabbed Image���� Mark�� ���� ���� ���ڿ��� ���
 parm : hdc		- [in]  DC ��ü �ڵ�
		grab	- [in]  Grabbed Image ������ ����� ����ü ������
		move	- [in]  ���ڿ� (����)�� ��µǴ� ��ġ (����: Pixel)
 retn : None
*/
VOID DrawGrabMarkDiffText(HDC hdc, LPG_ACGR grab, PPOINT move)
{
	TCHAR tzDiff[128]	= {NULL};
	BOOL bFind			= FALSE, bMultiMark = FALSE;
	INT32 i32BkMode		= 0;
	COLORREF clrText[2]	= {RGB(255, 0, 0), RGB(0, 255, 0) }, clrPrev;
	LOGFONT lfText		= {NULL};
	HFONT ftText, ftFont= NULL;

	/* ��� �������� ���� ���� */
	bMultiMark	= (ENG_MMSM)grab->mark_method != ENG_MMSM::en_single;
	bFind = grab->IsMarkValid(bMultiMark);
	/* ��Ʈ ���� ��� */
	if (!uvCmn_GetLogFont(lfText, L"D2Coding", 13, DEFAULT_CHARSET))	return;
	ftText	= ::CreateFontIndirect(&lfText);
	if (!ftText)	return;
	/* ��� �����ϰ� */
	i32BkMode	= ::SetBkMode(hdc, TRANSPARENT);
	/* ����� ��Ʈ ����*/
	ftFont	= (HFONT)::SelectObject(hdc, ftText);
	if (ftFont)
	{
		/* ����� �ؽ�Ʈ ���� ���� */
		clrPrev	= ::SetTextColor(hdc, clrText[bFind]);
		/* �ؽ�Ʈ ��� */
		swprintf_s(tzDiff, 128, L"(x:%+.4f y:%+.4f um)", grab->move_mm_x, grab->move_mm_y);
		::TextOut(hdc, 15, move->y+1, tzDiff, (INT32)_tcslen(tzDiff));
		/* �ؽ�Ʈ ���� */
		::SetTextColor(hdc, clrPrev);
		/* ��Ʈ ����*/
		::SelectObject(hdc, ftFont);
	}
	/* ��� ��� ���� */
	::SetBkMode(hdc, i32BkMode);
	/* ��Ʈ ��ü ���� */
	::DeleteObject(ftText);
}

/*
 desc : Grabbed Image���� Mark ���� ǥ�� (���)
 parm : hdc			- [in]  DC ��ü �ڵ�
		s_grab		- [in]  Grabbed Image ������ ����� ����ü ������ (����)
		m_grab		- [in]  Grabbed Image ������ ����� ����ü ������ (�˻��� ��ũ�� ���� ������ ����)
		resize_rate	- [in]  ��� Ȥ�� Ȯ�� ����
		move		- [in]  ���� ��µǴ� �̹��� ���� ��ġ
		find		- [in]  ��ũ �˻� ���� ���� (�˻��ƴ���, �˻� ��� ���� ���� �޶���)
							0x00 - �˻� ��� ����, 0x01 - �˻� ��� ����
 retn : None
*/
VOID DrawGrabMarkArea(HDC hdc, LPG_ACGR s_grab, LPG_GMSR m_grab,
					  DOUBLE resize_rate, POINT *move, UINT8 find)
{
	DOUBLE dbAngle		= g_pstConfig->set_cams.acam_inst_angle == 0 ? 0.0f : 180.0f;
	HPEN hPen, hOldPen;
	POINT ptLine[4], ptCent;
	COLORREF crLine[3]	= {RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255) };
	STG_DBXY stInCent	= {NULL};
	STG_DBWH stInSize	= {NULL};
	CRect rArea;

	/* ��� �����Ͱ� �ִ��� ���� */
	if (m_grab->IsEmptyData())	return;
	/* Grabbed Image 180�� ȸ�� �����Ƿ� */
	if (g_pstConfig->set_cams.acam_inst_angle)
	{
		/* Grabbed Image�� �߽ɿ��� ȸ������ ���, ���� ��ũ�� �߽� ��ǥ �� ��ȯ */
		uvCmn_RotateCoord(DOUBLE(s_grab->grab_width/2.0f), DOUBLE(s_grab->grab_height/2.0f),
						  s_grab->mark_cent_px_x, s_grab->mark_cent_px_y,
						  DOUBLE(-dbAngle/*g_pstConfig->set_cams.acam_inst_angle*/),	/* �׻� ȸ�� ������ - (����) ���� �ٿ���� �� (�ֳ��ϸ�, Grabbed Image ��, MIL�� ImgRotate �Լ� ȸ���� �ð� �ݴ� �������� �ϱ� ������ */
						  m_grab->cent_x, m_grab->cent_y);
	}

	/* ��µǴ� ���� ������ ����, Grabbed Image�� ��� or Ȯ�뿡 ���� ã�� Mark ���� ũ�� ��� or Ȯ�� */
	stInSize.cx	= m_grab->mark_width;
	stInSize.cy	= m_grab->mark_height;
	stInCent.x	= m_grab->cent_x;
	stInCent.y	= m_grab->cent_y;
	GetDrawMarkArea(&stInSize, &stInCent, resize_rate, ptCent, ptLine);

	/* --------------------------------------------- */
	/* Grabbed Image���� �˻��� ������ �簢�� �׸��� */
	/* --------------------------------------------- */
	hPen	= ::CreatePen(PS_SOLID, 1, crLine[find]);
	if (hPen)
	{
		hOldPen	= (HPEN)::SelectObject(hdc, hPen);
		if (hOldPen)
		{
			/* Mark Lines (Rectangle) ��� */
			::MoveToEx(hdc,	ptLine[0].x+move->x,	ptLine[0].y+move->y, NULL);
			::LineTo(hdc,	ptLine[1].x+move->x,	ptLine[1].y+move->y);
			::LineTo(hdc,	ptLine[2].x+move->x,	ptLine[2].y+move->y);
			::LineTo(hdc,	ptLine[3].x+move->x,	ptLine[3].y+move->y);
			::LineTo(hdc,	ptLine[0].x+move->x,	ptLine[0].y+move->y);

			/* Mark Center (+) ��� */
			::MoveToEx(hdc,	ptCent.x-5+move->x,		ptCent.y+move->y, NULL);
			::LineTo(hdc,	ptCent.x+5+move->x,		ptCent.y+move->y);
			::MoveToEx(hdc,	ptCent.x+move->x,		ptCent.y-5+move->y, NULL);
			::LineTo(hdc,	ptCent.x+move->x,		ptCent.y+5+move->y);

			::SelectObject(hdc, hOldPen);
		}
		::DeleteObject(hPen);
	}
#if 1
	INT32 i32MWidth, i32MHeight, i32MStartX, i32MStartY;
	/* ���� Multi-Mark�� ���, ��Ƽ ��ũ�� �����ϰ� �ִ� �簢�� ���� �׸��� */
	if (g_dbMuliAreaWidth > 0.0f && g_dbMuliAreaHeight > 0.0f &&
		ENG_MMSM::en_multi_only == theApp.clMilMain.GetMarkMethod(s_grab->cam_id))
	{
		i32MWidth	= (INT32)ROUNDED(g_dbMuliAreaWidth * resize_rate, 0);
		i32MHeight	= (INT32)ROUNDED(g_dbMuliAreaHeight * resize_rate, 0);
		i32MStartX	= ptCent.x - (INT32)ROUNDED(i32MWidth / 2, 0);
		i32MStartY	= ptCent.y - (INT32)ROUNDED(i32MHeight / 2, 0);

		hPen	= ::CreatePen(PS_SOLID, 1, crLine[2]);
		if (hPen)
		{
			hOldPen	= (HPEN)::SelectObject(hdc, hPen);
			if (hOldPen)
			{
				::MoveToEx(hdc,	i32MStartX+move->x,				i32MStartY+move->y, NULL);
				::LineTo(hdc,	i32MStartX+i32MWidth+move->x,	i32MStartY+move->y);
				::LineTo(hdc,	i32MStartX+i32MWidth+move->x,	i32MStartY+i32MHeight+move->y);
				::LineTo(hdc,	i32MStartX+move->x,				i32MStartY+i32MHeight+move->y);
				::LineTo(hdc,	i32MStartX+move->x,				i32MStartY+move->y);

				::SelectObject(hdc, hOldPen);
			}
			::DeleteObject(hPen);
		}
	}
#endif
}

/*
 desc : Grabbed Image�� �߽ɿ� '+' ǥ�� (���)
 parm : hdc			- [in]  DC ��ü �ڵ�
		grab		- [in]  Grabbed Image ������ ����� ����ü ������
		resize_rate	- [in]  ��� Ȥ�� Ȯ�� ����
		move		- [in]  ���� ��µǴ� �̹��� ���� ��ġ
 retn : None
*/
#if 0
VOID DrawGrabImageCent(HDC hdc, LPG_ACGR grab, DOUBLE resize_rate, POINT *move)
{
	HPEN hPen, hOldPen;
	POINT ptCent;

	/* ----------------------------------------------- */
	/* Grabbed Image���� �߽��� �Ǵ� Center (+) �׸��� */
	/* ----------------------------------------------- */
	hPen	= ::CreatePen(PS_SOLID, 1, RGB(255, 127, 39));
	if (hPen)
	{
		hOldPen	= (HPEN)::SelectObject(hdc, hPen);
		if (hOldPen)
		{
			ptCent.x = (UINT32)ROUNDED((grab->grab_width / 2.0f) * resize_rate, 0);
			ptCent.y = (UINT32)ROUNDED((grab->grab_height / 2.0f) * resize_rate, 0);
			/* Mark Center (+) ��� */
#if 0
			::MoveToEx(hdc,	ptCent.x-5+move->x,	ptCent.y+move->y, NULL);
			::LineTo(hdc,	ptCent.x+5+move->x,	ptCent.y+move->y);
			::MoveToEx(hdc,	ptCent.x+move->x,	ptCent.y-5+move->y, NULL);
			::LineTo(hdc,	ptCent.x+move->x,	ptCent.y+5+move->y);
#else
			::MoveToEx(hdc,	move->x,					ptCent.y+move->y, NULL);
			::LineTo(hdc,	move->x+grab->grab_width,	ptCent.y+move->y);
			::MoveToEx(hdc,	ptCent.x+move->x,			move->y, NULL);
			::LineTo(hdc,	ptCent.x+move->x,			move->y+grab->grab_height);
#endif
			::SelectObject(hdc, hOldPen);
		}
		::DeleteObject(hPen);
	}
}
#else
VOID DrawGrabImageCent(HDC hdc, RECT draw)
{
	HPEN hPen, hOldPen;

	/* ----------------------------------------------- */
	/* Grabbed Image���� �߽��� �Ǵ� Center (+) �׸��� */
	/* ----------------------------------------------- */
	hPen	= ::CreatePen(PS_DOT, 1, RGB(255, 127, 39));
	if (hPen)
	{
		hOldPen	= (HPEN)::SelectObject(hdc, hPen);
		if (hOldPen)
		{
			/* Mark Center (+) ��� */
			::MoveToEx(hdc,	draw.left,							draw.top+(draw.bottom-draw.top)/2, NULL);
			::LineTo(hdc,	draw.right,							draw.top+(draw.bottom-draw.top)/2);
			::MoveToEx(hdc,	draw.left+(draw.right-draw.left)/2,	draw.top, NULL);
			::LineTo(hdc,	draw.left+(draw.right-draw.left)/2,	draw.bottom);
			::SelectObject(hdc, hOldPen);
		}
		::DeleteObject(hPen);
	}
}
#endif
/*
 desc : Grabbed Image���� Mark �߽� ��ġ�� '+' ǥ�� (���)
 parm : hdc			- [in]  DC ��ü �ڵ�
		grab		- [in]  Grabbed Image ������ ����� ����ü ������
		resize_rate	- [in]  ��� Ȥ�� Ȯ�� ����
		move		- [in]  ���� ��µǴ� �̹��� ���� ��ġ
 retn : None
*/
VOID DrawGrabMarkCent(HDC hdc, LPG_ACGR grab, DOUBLE resize_rate, POINT *move)
{
	HPEN hPen, hOldPen;
	POINT ptDiff;

	/* ----------------------------------------------- */
	/* Grabbed Image���� �߽��� �Ǵ� Center (+) �׸��� */
	/* ----------------------------------------------- */
	hPen	= ::CreatePen(PS_SOLID, 1, RGB(255, 127, 39));
	if (hPen)
	{
		hOldPen	= (HPEN)::SelectObject(hdc, hPen);
		if (hOldPen)
		{
			ptDiff.x = (UINT32)ROUNDED((grab->mark_cent_px_x) * resize_rate, 0);
			ptDiff.y = (UINT32)ROUNDED((grab->mark_cent_px_y) * resize_rate, 0);
			/* Mark Center (+) ��� */
			::MoveToEx(hdc,	ptDiff.x-5+move->x,	ptDiff.y+move->y, NULL);
			::LineTo(hdc,	ptDiff.x+5+move->x,	ptDiff.y+move->y);
			::MoveToEx(hdc,	ptDiff.x+move->x,	ptDiff.y-5+move->y, NULL);
			::LineTo(hdc,	ptDiff.x+move->x,	ptDiff.y+5+move->y);
			::SelectObject(hdc, hOldPen);
		}
		::DeleteObject(hPen);
	}
}

/*
 desc : Drawing - Align(�˻��� Mark) Image (Bitmap�� �̿��Ͽ� ���)
 parm : hdc		- [in]  �̹����� ��� ��� context
		draw	- [in]  �̹����� ��µ� ���� (�θ� ���� ���� ��� ��ǥ)
		grab	- [in]  Grabbed Image ������ ����� ����ü ������
		find	- [in]  ��ũ �˻� ���� ���� (�˻��ƴ���, �˻� ��� ���� ���� �޶���)
						0x00 - �˻� ��� ����, 0x01 - �˻� ��� ����
 retn : None
*/
VOID DrawGrabMarkBitmap(HDC hdc, RECT draw, LPG_ACGR grab, UINT8 find,UINT8 flipFlag=0xff)
{
	UINT8 i, u8Find;
	DOUBLE dbRate;
	HDC hDC	= NULL;
	POINT ptMove;
	LPG_GMSR pstFind;
	RECT rDraw (draw);
	CSize szSrc (grab->grab_width, grab->grab_height), szTgt;
#if 0
	/* ��� ���� ���� */
	draw.left++;
	draw.top++;
	draw.right--;
	draw.bottom--;
#endif
	/* ������ �����Ͱ� ���� ������� */
	if (grab->grab_width < 1 || grab->grab_height < 1 || !grab->grab_data)	return;
	/* ���ο� ������ �̹��� ũ�� �� ��, ���� ���̳��� �κ��� �������� ���� */
	if (!ResizeDrawBitmap(rDraw, szSrc, szTgt, dbRate))	return;
	if (FLT_MAX == dbRate)	return;
	/* �̹��� ��� ���� ��ġ ��� */
	ptMove.x= (LONG)ROUNDED(DOUBLE((rDraw.right - rDraw.left) - szTgt.cx) / 2.0f, 0) + rDraw.left;
	ptMove.y= (LONG)ROUNDED(DOUBLE((rDraw.bottom- rDraw.top) - szTgt.cy) / 2.0f, 0) + rDraw.top;

	/* Grabbed Image ��� */
	DrawGrabImage(hdc, grab, draw, &szTgt, &ptMove, flipFlag);
	/* Grabbed Image�� �߽ɿ� ��ȣ (��ü ����) ('+') ��� */
//	DrawGrabImageCent(hdc, grab, dbRate, &ptMove);
	DrawGrabImageCent(hdc, draw); 
	if (ENG_MMSM::en_cent_side == theApp.clMilMain.GetMarkMethod(grab->cam_id))
	{
		DrawGrabMarkDiffText(hdc, grab, &ptMove);
	}

	/* �� �˻��� Mark�� ���� ���� ǥ�� (���) */
	u8Find = theApp.clMilMain.GetGrabbedMarkCount(grab->cam_id, grab->img_id);
	pstFind= theApp.clMilMain.GetGrabbedMarkAll(grab->cam_id, grab->img_id);
	for (i=0; i<u8Find; i++)
	{
		/* Grabbed Image���� Mark�� �߽� ��ġ�� ��ȣ ('+') ��� */
		DrawGrabMarkArea(hdc, grab, &pstFind[i], dbRate, &ptMove, find);
	}
}

/* --------------------------------------------------------------------------------------------- */
/*                                           �ܺ� �Լ�                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*
 desc : BIPS Server�� ����� ����ϴ� Client ������ ���� �� ����
 parm : config		- [in]  ȯ�� ����
		shmem		- [in]  Shared Memory
		work_dir	- [in]  �۾� ��� (���� ���� ���)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_Init(LPG_CIEA config, LPG_VDSM shmem, PTCHAR work_dir)
{
	g_pstConfig	= config;

	/* MIL Main Object Memory ���� */
	//g_pMilMain	= new CMilMain(config, shmem);
	//ASSERT(g_pMilMain);
	theApp.clMilMain.InitMilMain(config, shmem);

	/* Calb ROI ���� */
	theApp.clMilMain.rectSearhROI_Calb_CamSpec.left = (config->set_cams.acam_size[0] - config->set_cams.spc_size[0]) / 2;
	theApp.clMilMain.rectSearhROI_Calb_CamSpec.right = theApp.clMilMain.rectSearhROI_Calb_CamSpec.left + config->set_cams.spc_size[0];
	theApp.clMilMain.rectSearhROI_Calb_CamSpec.top = (config->set_cams.acam_size[1] - config->set_cams.spc_size[1]) / 2;
	theApp.clMilMain.rectSearhROI_Calb_CamSpec.bottom = theApp.clMilMain.rectSearhROI_Calb_CamSpec.top + config->set_cams.spc_size[1];
	
	theApp.clMilMain.rectSearhROI_Calb_Step.left = (config->set_cams.acam_size[0] - config->set_cams.soi_size[0]) / 2;
	theApp.clMilMain.rectSearhROI_Calb_Step.right = theApp.clMilMain.rectSearhROI_Calb_Step.left + config->set_cams.soi_size[0];
	theApp.clMilMain.rectSearhROI_Calb_Step.top = (config->set_cams.acam_size[1] - config->set_cams.soi_size[1]) / 2;
	theApp.clMilMain.rectSearhROI_Calb_Step.bottom = theApp.clMilMain.rectSearhROI_Calb_Step.top + config->set_cams.soi_size[1];

	theApp.clMilMain.rectSearhROI_Calb_Accr.left = (config->set_cams.acam_size[0] - config->set_cams.mes_size[0]) / 2;
	theApp.clMilMain.rectSearhROI_Calb_Accr.right = theApp.clMilMain.rectSearhROI_Calb_Accr.left + config->set_cams.mes_size[0];
	theApp.clMilMain.rectSearhROI_Calb_Accr.top = (config->set_cams.acam_size[1] - config->set_cams.mes_size[1]) / 2;
	theApp.clMilMain.rectSearhROI_Calb_Accr.bottom = theApp.clMilMain.rectSearhROI_Calb_Accr.top + config->set_cams.mes_size[1];

	//if (!theApp.clMilMain.InitMilAlloc(ENG_ERVM(config->set_comn.engine_mode))/*, L"D2Coding", 20*/)
	if (!theApp.clMilMain.InitMilAlloc(ENG_ERVM(config->set_comn.engine_mode))/*, L"D2Coding", 20*/)
	{
		//delete theApp.clMilMain;
		//theApp.clMilMain = NULL;
		return FALSE;
	}


	/* �۾� ��� ���� */
	swprintf_s(g_tzWorkDir, MAX_PATH_LEN, L"%s", work_dir);
	/* ī�޶� �ִ� �ػ� ũ�⸸ŭ �ӽ� ���� �Ҵ� (�̹��� ȸ�� �ӽ� ����) */
	UINT32 u32Size	= config->set_cams.GetCamSizeBytes();
	/* Live ����� ��, ȸ�� �� ������ ���� �ӽ� ���� �Ҵ� */
	g_pGrabBuff = new UINT8[u32Size + 1];//  (PUINT8)::Alloc(u32Size + 1);
	g_pGrabBuff[u32Size]	= 0x00;

	return TRUE;
}

/*
 desc : ���̺귯�� ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvMIL_Close()
{
	/* MIL Main Object Memory ���� */
	//if (g_pMilMain)	delete g_pMilMain;
	//g_pMilMain	= NULL;

	/* Grabbed Image ��� ���� �޸� ���� */
	if (g_pBmpInfo)	delete [] g_pBmpInfo;
	g_pBmpInfo	= NULL;
	/* �޸� ������ �Բ� ũ�� �� �ʱ�ȭ �ʿ� */
	g_u32GrabWidth	= 0;
	g_u32GrabHeight	= 0;
	/* �ӽ� ȸ�� �� ����� ���� �޸� ���� */
	if (g_pGrabBuff)	
		delete[] g_pGrabBuff;

	g_pGrabBuff	= NULL;
	/* �ݵ�� ����� �� */
	g_pstConfig	= NULL;


	// lk91 ���⼭ ���� �غ���
	theApp.clMilMain.CloseMilAlloc();
}

/*
 desc : ���� �ֱ��� Grabbed Image ��� �ʱ�ȭ
		���ο� Align �۾��� �����Ѵٸ�, Align �����ϱ� ���� ������ �ѹ��� ȣ��Ǿ�� ��
 parm : None
 retn : None
*/
API_EXPORT VOID uvMIL_ResetGrabAll()
{
	//if (g_pMilMain)	
	theApp.clMilMain.ResetGrabAll();
}

/*
 desc : Mark ���� ����
 parm : cam_id		- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		detail_level- [in]  ��ϵ� �� �̹����� Grabbed Image�̹����� Edge ������ ���� �� ���� ��
							���� Ŭ���� ���� ��������, �ӵ��� ���� ��������
							LOW���� HIGH�� ������ �� ���� Edge�� ������ ���ϰ� ��
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - �⺻ ��, 1.0f ~ 100.0f
							�⺻ ���� 50������, 100���� ������ ������ ������ ���͸� ��Ÿ���ϴ�.
		model		- [in]  Model Type, Param 1 ~ 5�� �������� ��� ���Ե� ����ü ������
		count		- [in]  ����ϰ��� �ϴ� ���� ����
		�Ʒ� 2���� ���� 0 ���̸� �Ķ���� ������� ����
		scale_min	- [in]  �˻� ����� ũ�� ���� �� ���� (�ּ��� �� �� �������� Ŀ�� ��. ����: 0.5 ~ 1.0)
		scale_max	- [in]  �˻� ����� ũ�� ���� �� ���� (�ִ��� �� �� �������� �۾ƾ� ��. ����: 1.0 ~ 1.0)
		score_min	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0)
		score_tgt	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0))
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_SetModelDefine(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
									 LPG_CMPV model, UINT8 mark_no,
									 DOUBLE scale_min, DOUBLE scale_max,
									 DOUBLE score_min, DOUBLE score_tgt)
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.SetModelDefine(cam_id, speed, level, count, smooth, model, mark_no,
									  scale_min, scale_max, score_min, score_tgt);
}

/*
 desc : ���� �ý��� (�����)�� ����Ǿ� �ִ� ���� ��Ī ���� �̹��� ����
 parm : cam_id		- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		detail_level- [in]  ��ϵ� �� �̹����� Grabbed Image�̹����� Edge ������ ���� �� ���� ��
							���� Ŭ���� ���� ��������, �ӵ��� ���� ��������
							LOW���� HIGH�� ������ �� ���� Edge�� ������ ���ϰ� ��
							0 (M_VERY_LOW), 1 (M_LOW), 2 (M_MEDIUM), 3 (M_HIGH), 4 (M_VERY_HIGH)
		smooth		- [in]  0.0f - �⺻ ��, 1.0f ~ 100.0f
							�⺻ ���� 50������, 100���� ������ ������ ������ ���͸� ��Ÿ���ϴ�.
		model		- [in]  Model Type ��, circle, square, rectangle, cross, diamond, triangle
		param		- [in]  �� 5���� Parameter Values (unit : um)
		mark_no		- [in]  ����ϰ��� �ϴ� �� ��ȣ
		�Ʒ� 2���� ���� 0 ���̸� �Ķ���� ������� ����
		scale_min	- [in]  �˻� ����� ũ�� ���� �� ���� (�ּ��� �� �� �������� Ŀ�� ��. ����: 0.5 ~ 1.0)
		scale_max	- [in]  �˻� ����� ũ�� ���� �� ���� (�ִ��� �� �� �������� �۾ƾ� ��. ����: 1.0 ~ 1.0)
		score_tgt	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0))
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_SetModelDefineEx(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
	PUINT32 model, DOUBLE* param1, DOUBLE* param2, DOUBLE* param3,
	DOUBLE* param4, DOUBLE* param5, UINT8 mark_no,
	DOUBLE scale_min, DOUBLE scale_max,
	DOUBLE score_min, DOUBLE score_tgt, bool sameMark)
{
	// if (!g_pMilMain)	return FALSE;
	//return theApp.clMilMain.SetModelDefine(cam_id, speed, level, smooth, model, param1,
	//								  param2, param3, param4, param5, count,
	//								  scale_min, scale_max, score_min, score_tgt);
	theApp.clMilMain.SetModelDefine(cam_id, speed, level, count, smooth, model, param1,
		param2, param3, param4, param5, mark_no,
		scale_min, scale_max, score_min, score_tgt, sameMark);

	return TRUE;
}


/*
 desc : Mark ���� ���� - �̹��� ������
 parm : cam_id		- [in]  Align Camera Index (1 or 2)
		speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		level		- [in]  ��ϵ� �� �̹����� Grabbed Image�̹����� Edge ������ ���� �� ���� ��
							���� Ŭ���� ���� ��������, �ӵ��� ���� ��������
							LOW���� HIGH�� ������ �� ���� Edge�� ������ ���ϰ� ��
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - �⺻ ��, 1.0f ~ 100.0f
		scale_min	- [in]  �˻� ����� ũ�� ���� �� ���� (�ּ��� �� �� �������� Ŀ�� ��. ����: 0.5 ~ 1.0)
		scale_max	- [in]  �˻� ����� ũ�� ���� �� ���� (�ִ��� �� �� �������� �۾ƾ� ��. ����: 1.0 ~ 1.0)
		score_min	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0)
		score_tgt	- [in]  �� �� ���Ϸδ� �˻��� �� ���� (�ԷµǴ� ���� percentage (0.0 ~ 100.0))
		name		- [in]  Model Name
		file		- [in]  �� �̹����� ����� ���� �̸� (��ü ��� ����. Image File)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_SetModelDefineLoad(UINT8 cam_id, UINT8 speed, UINT8 level, DOUBLE smooth,
										 DOUBLE scale_min, DOUBLE scale_max,
										 DOUBLE score_min, DOUBLE score_tgt,
										 PTCHAR name, CStringArray &file)
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.SetModelDefineLoad(cam_id, speed, level, smooth,
										  scale_min, scale_max, score_min, score_tgt, name, file);
}

/*
 desc : ���� ��ϵ� Mark Model ���� ��ȯ
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : ��ϵ� ����ü ������ ��ȯ
*/
API_EXPORT LPG_CMPV uvMIL_GetModelDefine(UINT8 cam_id)
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.GetModelDefine(cam_id);
}

/*
 desc : Camera�κ��� ������ Grabbed Image (Binary Image Data)�� Edge Detection
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
		grab	- [in]  Grabbed Image ������ ����� ����ü ������
		saved	- [in]  �м��� edge image ���� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_RunEdgeDetect(UINT8 cam_id, LPG_ACGR grab, UINT8 saved)
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.RunEdgeDetect(cam_id, grab, saved);
}

/*
 desc : Set the parameters of the strip maker
 parm : cam_id	- [in]  Align Camera ID (1 or 2 or later)
		image	- [in]  The buffer of Grabbed Image
		param	- [in]  Structures pointer with information to set up is stored
		result	- [out] Buffer in which the resulting value will be returned and stored.
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_RunMarkerStrip(UINT8 cam_id, PUINT8 image, LPG_MSMP param, STG_MSMR &results)
{
	PUINT8 pImgBuff	= NULL;

	if (!GetConfig()->IsRunDemo())	pImgBuff	= image;
	else
	{
		UINT32 u32Width	= GetConfig()->set_cams.hol_size[0];
		UINT32 u32Height= GetConfig()->set_cams.hol_size[1];
		UINT32 u32Size	= u32Width * u32Height;
		/* Initialize existing temporary memory */
		g_pGrabBuff[u32Size]	= 0x00;
		pImgBuff	= g_pGrabBuff;
		theApp.clMilMain.ImageFlip(image, g_pGrabBuff, u32Width, u32Height, 0x01);
	}

	return theApp.clMilMain.RunMarkerStrip(cam_id, pImgBuff, param, results);
}

API_EXPORT VOID uvMIL_ImageFlip(PUINT8 img_src, PUINT8 img_dst, UINT32 width, UINT32 height)
{
	theApp.clMilMain.ImageFlip(img_src, img_dst, width, height, 0x01);
}

/*
 desc : ���� �˻��� ���� ��ϵ� ���� ���� ��ȯ
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : ��ϵ� ����
*/
API_EXPORT UINT32 uvMIL_GetModelRegistCount(UINT8 cam_id)
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.GetModelRegistCount(cam_id);
}

/*
 desc : Geometric Model Find
 parm : cam_id	- [in]  Camera Index (1 or 2)
 		img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
		image	- [in]  Grabbed Image Data
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_RunModelFind(UINT8 cam_id, UINT8 mode, UINT8 img_id, PUINT8 image, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp, UINT8 img_proc)
{
	BOOL bSucc	= FALSE;

	// if (!g_pMilMain)	return FALSE;
	if (cam_id < 1 || cam_id > MAX_ALIGN_CAMERA)	return FALSE;
	if (!image)	return FALSE;
	if (0xff == mode)			bSucc	= theApp.clMilMain.RunModelFind(cam_id, image, FALSE, img_id, dlg_id, mark_no, useMilDisp, img_proc);	/* Calibration Mode */
	else if (0xfe == mode)	bSucc	= theApp.clMilMain.RunModelFind(cam_id, image, TRUE, img_id, dlg_id, mark_no, useMilDisp, img_proc);	/* Angle Measuring Mode */
	else						bSucc	= theApp.clMilMain.RunModelFind(cam_id, img_id, image, dlg_id, mark_no, useMilDisp, img_proc);	/* Normal Grabbed Mode */

	return bSucc;
}

/*
 desc : ����ڿ� ���� �������� �˻��� ���, ��� ���� ó��
 parm : grab	- [in]  ����ڿ� ���� �������� �Էµ� grabbed image ��� ������ ����� ����ü ������
		gmfr	- [in]  GMFR Data
		gmsr	- [in]  GMSR Data
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_SetGrabbedMark(LPG_ACGR grab, LPG_GMFR gmfr, LPG_GMSR gmsr)
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.SetGrabbedMark(grab, gmfr, gmsr);
}

/*
 desc : Geometric Model Find (Step; Vision ���� Ȯ�ο�)
 parm : cam_id	- [in]  Camera Index (1 or 2)
		width	- [in]  Grabbed Image - Width (Pixel)
		height	- [in]  Grabbed Image - Width (Pixel)
		count	- [in]  �˻��ϰ��� �ϴ� �� ����
		angle	- [in]  ���� ���� ���� (TRUE : ���� ������, FALSE : ���� �������� ����)
						TRUE : ���� ī�޶��� ȸ���� ���� ���ϱ� ����, FALSE : ���� ���� �����Ͽ� ȸ���� ���� ����
		results	- [out] �˻��� ��� �� ��ȯ ����ü ������
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_RunModelStep(UINT8 cam_id, PUINT8 image, UINT32 width, UINT32 height, UINT16 count, BOOL angle, LPG_ACGR results, UINT8 img_id, UINT8 dlg_id, UINT8 mark_no, UINT8 img_proc)
{
	// if (!g_pMilMain)	return FALSE;
	return  theApp.clMilMain.RunModelStep(cam_id, image, width, height, count, angle, results, img_id, dlg_id, mark_no, img_proc);
}

/*
 desc : Geometric Model Find (Examination; Vision Align �뱤 ��� Ȯ�ο�)
 parm : width		- [in]  Grabbed Image - Width (Pixel)
		height		- [in]  Grabbed Image - Width (Pixel)
		score		- [in]  ũ�� ���� �� (�� ������ ũ�ų� ������ Okay)
		scale		- [in]  ��Ī ���� �� (1.0 �������� +/- �� �� ���� �ȿ� ������ Okay)
		results		- [out] �˻��� ��� �� ��ȯ ����ü ������
							���� NULL�� ���, �˻� �Ǿ����� ���θ� ���� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_RunModelExam(PUINT8 image, UINT32 width, UINT32 height,DOUBLE score, DOUBLE scale, LPG_ACGR results, UINT8 img_id, UINT8 dlg_id, UINT8 mark_no) // �̻��
{
	// if (!g_pMilMain)	return FALSE;
	return  theApp.clMilMain.RunModelExam(image, width, height, score, scale, results, img_id, dlg_id, mark_no);
}

API_EXPORT BOOL uvMIL_RunModelExam2(PUINT8 image, UINT32 width, UINT32 height, DOUBLE score, DOUBLE scale, LPG_ACGR results)
{
	//if (!g_pMilMain)	return FALSE;
	return  theApp.clMilMain.RunModelExam2(image, width, height, score, scale, results);
}


/*
 desc : ���� �ֱ� Grabbed Image�� ��Ī �˻� ��� ����ü ������ ��ȯ
 parm : None
 retn : ��ü ������ (��Ī�� ������ ��� NULL)
*/
API_EXPORT LPG_ACGR uvMIL_GetLastGrabbedMark()
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.GetLastGrabbedMark();
}

/*
 desc : �˻��� ��ũ �̹����� ���Ե� ����ü ������ ��ȯ
 parm : cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
 retn : ����ü ������ ��ȯ
*/
API_EXPORT LPG_ACGR uvMIL_GetGrabbedMark(UINT8 cam_id, UINT8 img_id)
{
	// if (!g_pMilMain)	return NULL;
	return theApp.clMilMain.GetGrabbedMark(cam_id, img_id);
}

/*
 desc : ���� �ֱٿ� Edge Detection�� �̹������� Ư�� ������ �����ؼ� Bitmap Image�� ���
		��, Mark Template �̹����� ���
 parm : cam_id	- [in]  Align Camera ID (1 or Later)
		area	- [in]  �����ϰ��� �ϴ� ���� ������ ����� �ȼ� ��ġ ��
		type	- [in]  0x00 : Edge, 0x02 : Line, 0x02 : Match, 0x03 : Grab
		file	- [in]  Mark Template�� �����Ϸ��� ���� �̸� (��ü ��� ����)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_SaveGrabbedMarkToFile(UINT8 cam_id, LPRECT area, UINT8 type, TCHAR *file)
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.SaveGrabbedMarkToFile(cam_id, area, type, file);
}

/*
 desc : ������ ���� (���)�Ǿ� �ִ� ��� Mark ���� ����
 parm : None
 retn : None
*/
API_EXPORT VOID uvMIL_ResetMarkModel()
{
	//if (g_pMilMain)	
	theApp.clMilMain.ResetMarkModel();
}

/*
 desc : Mark Pattern ��� ����
 parm : mode	- [in]  0x00 : Expose, 0x01 : Vstep
		cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_IsSetMarkModel(UINT8 mode, UINT8 cam_id, UINT8 fi_No)
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.IsSetMarkModel(mode, cam_id, fi_No);
}

/*
 desc : Mark Pattern ��� ����
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_IsSetMarkModelACam(UINT8 cam_id, UINT8 mark_no)
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.IsSetMarkModelACam(cam_id, mark_no);
}

/*
 desc : Outputs the most recently generated Buffer ID value to the window
 parm : hwnd	- [in]  Handle of window where the image is output
		draw	- [in]  The area of window where the image is output
		cam_id	- [in]  Align Camera ID (1 or 2)
		type	- [in]  0x00: Edge Detection, 0x01: Line Detection, 0x02: Strip Detection (Measurement)
 retn : None
*/
API_EXPORT VOID uvMIL_DrawMBufID(HWND hwnd, RECT draw, UINT8 cam_id, UINT8 type)
{
#ifndef _NOT_USE_MIL_
	DOUBLE dbAngle = g_pstConfig->set_cams.acam_inst_angle == 0 ? 0.0f : 180.0f;
	MIL_INT64 bufType = M_IMAGE + M_PROC + M_DISP;
	MIL_ID mlBufID = M_NULL, mlBufResize, mlBufRotate = M_NULL;
	DOUBLE dbRate = 1.0f;
	RECT rDraw(draw);
	CSize szSrc, szTgt, szRot;

	// if (!g_pMilMain)	return;

	/* ���ο� ���� ���� �ϱ� */
	mlBufID = theApp.clMilMain.GetBufID(cam_id, type);
	if (!mlBufID)	return;

	/* ���� �̹��� ũ�� ���  */
	szSrc.cx = (UINT32)MbufInquire(mlBufID, M_SIZE_X, NULL);
	szSrc.cy = (UINT32)MbufInquire(mlBufID, M_SIZE_Y, NULL);

	/* ���� �̹��� ȸ�� �� ũ�� ��� */
	szRot = szSrc;
	uvCmn_RotateRectSize(dbAngle, szRot);
	/* ��� ���� ũ�⿡ �°� �̹��� ��� Ȥ�� Ȯ��� ũ�� ��� */
	dbRate = ResizeDrawBitmap(rDraw, szRot, szTgt, dbRate);
	/* ȸ���� �� ũ�Ⱑ Ŀ���Ƿ�, ũ�� ������ */
	if (szTgt.cx > rDraw.right - rDraw.left)	szTgt.cx = rDraw.right - rDraw.left;
	if (szTgt.cy > rDraw.bottom - rDraw.top)		szTgt.cy = rDraw.bottom - rDraw.top;
	/* ũ�� �ٽ� ������ �̹��� ���� ���� */
	if (0x00 == type)
	{
		/* Binary Buffer �Ҵ� */
		mlBufResize = MbufAlloc2d(theApp.clMilMain.GetMilSysID(), szTgt.cx, szTgt.cy,
			8 + M_UNSIGNED, bufType, M_NULL);
	}
	else
	{
		/* Color Buffer �Ҵ� */
		mlBufResize = MbufAllocColor(theApp.clMilMain.GetMilSysID(), 3,	/* 1: Monochrome, 3: RGB */
			szTgt.cx, szTgt.cy,
			8 + M_UNSIGNED, bufType, M_NULL);
	}
	if (mlBufResize)
	{
		/* ȸ�� ���� �޸� �Ҵ� */
		mlBufRotate = MbufAllocColor(theApp.clMilMain.GetMilSysID(), 3, szRot.cx, szRot.cy,
			8 + M_UNSIGNED, bufType, M_NULL);
		/* ȸ�� ������ ���� ���� �̹��� ȸ�� �� ȸ�� ���ۿ� ���� */
		if (mlBufRotate)
		{
			MimRotate(mlBufID, mlBufRotate, dbAngle,
				M_DEFAULT, M_DEFAULT, /*(MIL_DOUBLE)szResize.cx/2.0f, (MIL_DOUBLE)szResize.cy/2.0f, */
				M_DEFAULT, M_DEFAULT, /*(MIL_DOUBLE)szResize.cy/2.0f, (MIL_DOUBLE)szResize.cx/2.0f, */
				M_NEAREST_NEIGHBOR + M_OVERSCAN_CLEAR);
			//			MbufExport(L"g:\\download\\qcells\\export_rotate.bmp", M_BMP, mlBufRotate);
						/* �̹��� ũ�� ������ �� �ӽ� ���ۿ� �Ҵ� */
			MimResize(mlBufRotate, mlBufResize, M_FILL_DESTINATION, M_FILL_DESTINATION, M_INTERPOLATE);
			//			MbufExport(L"g:\\download\\qcells\\export_resize.bmp", M_BMP, mlBufResize);
						/* �̹��� ������ ������ Ư�� ���� ������ ��� */
			MdispSelectWindow(theApp.clMilMain.GetDispID(DISP_TYPE_EXPO), mlBufResize, hwnd); // lk91 �̻��...?
			/* ȸ�� ���� �޸� ���� */
			MbufFree(mlBufRotate);
		}
		/* �̹��� ���� */
		MbufFree(mlBufResize);
	}

	// lk91 ���� ���� (���� ����, expo������ ����ϴ� �Լ�)
//	DOUBLE dbAngle		= g_pstConfig->set_cams.acam_inst_angle == 0 ? 0.0f : 180.0f;
//	//BUFATTRTYPE bufType	= M_IMAGE+M_PROC+M_DISP;
//	MIL_INT64 bufType = M_IMAGE + M_PROC + M_DISP;
//	MIL_ID mlBufID		= M_NULL, mlBufResize, mlBufRotate = M_NULL;
//	DOUBLE dbRate		= 1.0f;
//	RECT rDraw (draw);
//	CSize szSrc, szTgt, szRot;
//
//	// if (!g_pMilMain)	return;
//
//	/* ���ο� ���� ���� �ϱ� */
//	mlBufID	= theApp.clMilMain.GetBufID(cam_id, type);
//	if (!mlBufID)	return;
//
//	/* ���� �̹��� ũ�� ���  */
//	szSrc.cx	= (UINT32)MbufInquire(mlBufID, M_SIZE_X, NULL);
//	szSrc.cy	= (UINT32)MbufInquire(mlBufID, M_SIZE_Y, NULL);
//		
//	/* ���� �̹��� ȸ�� �� ũ�� ��� */
//	szRot		= szSrc;
//	uvCmn_RotateRectSize(dbAngle, szRot);
//	/* ��� ���� ũ�⿡ �°� �̹��� ��� Ȥ�� Ȯ��� ũ�� ��� */
//	dbRate		= ResizeDrawBitmap(rDraw, szRot, szTgt, dbRate);
//	/* ȸ���� �� ũ�Ⱑ Ŀ���Ƿ�, ũ�� ������ */
//	if (szTgt.cx > rDraw.right - rDraw.left)	szTgt.cx = rDraw.right - rDraw.left;
//	if (szTgt.cy > rDraw.bottom- rDraw.top)		szTgt.cy = rDraw.bottom- rDraw.top;
//	/* ũ�� �ٽ� ������ �̹��� ���� ���� */
//	if (0x00 == type)
//	{
//		/* Binary Buffer �Ҵ� */
//		mlBufResize	= MbufAlloc2d(theApp.clMilMain.GetMilSysID(), szTgt.cx, szTgt.cy,
//								  8+M_UNSIGNED, bufType, M_NULL);
//	}
//	else
//	{
//		/* Color Buffer �Ҵ� */
//		mlBufResize	= MbufAllocColor(theApp.clMilMain.GetMilSysID(), 3,	/* 1: Monochrome, 3: RGB */
//									 szTgt.cx, szTgt.cy,
//									 8+M_UNSIGNED, bufType, M_NULL);
//	}
//	if (mlBufResize)
//	{
//		/* ȸ�� ���� �޸� �Ҵ� */
//		mlBufRotate	= MbufAllocColor(theApp.clMilMain.GetMilSysID(), 3, szRot.cx, szRot.cy,
//									 8+M_UNSIGNED, bufType, M_NULL);
//		/* ȸ�� ������ ���� ���� �̹��� ȸ�� �� ȸ�� ���ۿ� ���� */
//		if (mlBufRotate)
//		{
//			MimRotate(mlBufID, mlBufRotate, dbAngle,
//					  M_DEFAULT, M_DEFAULT, /*(MIL_DOUBLE)szResize.cx/2.0f, (MIL_DOUBLE)szResize.cy/2.0f, */
//					  M_DEFAULT, M_DEFAULT, /*(MIL_DOUBLE)szResize.cy/2.0f, (MIL_DOUBLE)szResize.cx/2.0f, */
//					  M_NEAREST_NEIGHBOR+M_OVERSCAN_CLEAR);
////			MbufExport(L"g:\\download\\qcells\\export_rotate.bmp", M_BMP, mlBufRotate);
//			/* �̹��� ũ�� ������ �� �ӽ� ���ۿ� �Ҵ� */
//			MimResize(mlBufRotate, mlBufResize, M_FILL_DESTINATION, M_FILL_DESTINATION, M_INTERPOLATE);
////			MbufExport(L"g:\\download\\qcells\\export_resize.bmp", M_BMP, mlBufResize);
//			/* �̹��� ������ ������ Ư�� ���� ������ ��� */
//			//MdispSelectWindow(theApp.clMilMain.GetDispID(cam_id), mlBufResize, hwnd);
//			MdispSelectWindow(theApp.clMilMain.GetDispID(DISP_TYPE_EXPO), mlBufResize, hwnd);
//
//			
//			/* ȸ�� ���� �޸� ���� */
//			MbufFree(mlBufRotate);
//		}
//		/* �̹��� ���� */
//		MbufFree(mlBufResize);
//	}
#endif
}

/*
 desc : Draw the strip information in the current grabbed image
 parm : hdc		- [in]  The handle of context
		draw	- [in]  The area in which the image is output
		cam_id	- [in]  Camera Index (1 or 2)
		angle	- [in]  The rotated angle value of the camera (unit: degree)
		param	- [in]  Strip output information
 retn : None
*/
API_EXPORT VOID uvMIL_DrawStripBitmap(HDC hdc, RECT draw, LPG_ACGR grab,
									  DOUBLE angle, LPG_MSMP param)
{
	UINT32 u32MoveX, u32MoveY;
	DOUBLE dbRate;
	PUINT8 pImgBuff	= NULL;
	RECT rDraw (draw);
	CSize szSrc (grab->grab_width, grab->grab_height), szTgt;
	/* Memory DC ������ Live Image ��� �׸��� */
	HDC hMemDC;
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;
	HBITMAP hMemBmp, hMemBmpOld;

	/* ��� ���� ���� ���� */
	rDraw.left++;
	rDraw.top++;
	rDraw.right--;
	rDraw.bottom--;

	/* ���ο� ������ �̹��� ũ�� �� ��, ���� ���̳��� �κ��� �������� ���� */
	if (!ResizeDrawBitmap(rDraw, szSrc, szTgt, dbRate))	return;

	/* ���� DC ���� �ʱ�ȭ */
	hBrush		= ::CreateSolidBrush(RGB(32, 32, 32));
	hOldBrush	= (HBRUSH)SelectObject(hdc, hBrush);
	if (!hOldBrush)
	{
		SelectObject(hdc, hOldBrush);
		::DeleteObject(hBrush);
		return;
	}

	/* �޸� DC�� ��Ʈ�� �����ϰ� ���� ���� �ֱ� */
	hMemDC	= ::CreateCompatibleDC(hdc);
#if 0
	hMemBmp	= ::CreateCompatibleBitmap(hdc, rDraw.right-rDraw.left, rDraw.bottom-rDraw.top);
#else
	hMemBmp	= ::CreateCompatibleBitmap(hdc, szSrc.cx, szSrc.cy);
#endif
	if (!hMemBmp)
	{
		SelectObject(hdc, hOldBrush);
		::DeleteObject(hBrush);
		if (hMemDC)	::DeleteDC(hMemDC);
		return;
	}
	/* Memory DC�� Bitmap ���� ���� */
	hMemBmpOld	= (HBITMAP)::SelectObject(hMemDC, hMemBmp);

	/* �̹��� ��� ���� ��ġ ��� */
	u32MoveX	= (UINT32)ROUNDED(DOUBLE((rDraw.right-rDraw.left) - szTgt.cx) / 2.0f, 0) + rDraw.left;
	u32MoveY	= (UINT32)ROUNDED(DOUBLE((rDraw.bottom-rDraw.top) - szTgt.cy) / 2.0f, 0) + rDraw.top;

	/* Check if the input image is rotated */
	pImgBuff	= grab->grab_data;
	if (!GetConfig()->IsRunDemo())
	{
		if (0.0f != angle)
		{
			/* Initialize existing temporary memory */
			g_pGrabBuff[grab->grab_size]	= 0x00;
			pImgBuff= g_pGrabBuff;
			theApp.clMilMain.ImageRotate(grab->grab_data, g_pGrabBuff,
									grab->grab_width, grab->grab_height,
									angle/*g_pstConfig->set_cams.acam_inst_angle*/,
									grab->grab_width/2, grab->grab_height/2);
		}
	}
	else
	{
		/* Initialize existing temporary memory */
		g_pGrabBuff[grab->grab_size]	= 0x00;
		pImgBuff= g_pGrabBuff;
		theApp.clMilMain.ImageFlip(grab->grab_data, g_pGrabBuff,
							  grab->grab_width, grab->grab_height, 0x01);
	}

	/* Outputs to the memory DC */
	::SetStretchBltMode(hdc, COLORONCOLOR);
#if 1
	::StretchDIBits(hMemDC, 0, 0, szTgt.cx, szTgt.cy,
					0, 0, grab->grab_width, grab->grab_height, 
					pImgBuff, g_pBmpInfo, DIB_RGB_COLORS, SRCCOPY);
	/* Draw the search area */
	hPen	= ::CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	if (hPen)
	{
		INT32 i32X1	= (INT32)ROUNDED(param->box_origin.x * dbRate, 0);
		INT32 i32Y1	= (INT32)ROUNDED(param->box_origin.y * dbRate, 0);
		INT32 i32X2	= (INT32)ROUNDED((param->box_origin.x+param->box_size.x) * dbRate, 0);
		INT32 i32Y2	= (INT32)ROUNDED((param->box_origin.y+param->box_size.y) * dbRate, 0);
		hOldPen	= (HPEN)::SelectObject(hMemDC, hPen);
		if (hOldPen)
		{
			::MoveToEx(hMemDC,	i32X1,	i32Y1, NULL);
			::LineTo(hMemDC,	i32X2,	i32Y1);
			::LineTo(hMemDC,	i32X2,	i32Y2);
			::LineTo(hMemDC,	i32X1,	i32Y2);
			::LineTo(hMemDC,	i32X1,	i32Y1);
			::SelectObject(hMemDC, hOldPen);
		}
		::DeleteObject(hPen);
	}
	/* Outputs to the screen DC */
	::StretchBlt(hdc, u32MoveX, u32MoveY, szTgt.cx, szTgt.cy,
				 hMemDC, 0, 0, szTgt.cx, szTgt.cy, SRCCOPY);
#else
	::StretchDIBits(hMemDC, 0, 0, szSrc.cx, szSrc.cy,
					0, 0, grab->grab_width, grab->grab_height, 
					pImgBuff, g_pBmpInfo, DIB_RGB_COLORS, SRCCOPY);
	/* Draw the search area */
	hPen	= ::CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	if (hPen)
	{
		hOldPen	= (HPEN)::SelectObject(hMemDC, hPen);
		if (hOldPen)
		{
			::MoveToEx(hMemDC,	param->box_origin.x,					param->box_origin.y, NULL);
			::LineTo(hMemDC,	param->box_origin.x+param->box_size.x,	param->box_origin.y);
			::LineTo(hMemDC,	param->box_origin.x+param->box_size.x,	param->box_origin.y+param->box_size.y);
			::LineTo(hMemDC,	param->box_origin.x,					param->box_origin.y+param->box_size.y);
			::LineTo(hMemDC,	param->box_origin.x,					param->box_origin.y);
			::SelectObject(hMemDC, hOldPen);
		}
		::DeleteObject(hPen);
	}
	/* Outputs to the screen DC */
	::StretchBlt(hdc, u32MoveX, u32MoveY, rDraw.right-rDraw.left, rDraw.bottom-rDraw.top,
				 hMemDC, 0, 0, grab->grab_width, grab->grab_height, SRCCOPY);
#endif

	if (hBrush)		::DeleteObject(hBrush);
	if (hMemBmp)	::DeleteObject(hMemBmp);
	if (hMemDC)		::DeleteDC(hMemDC);
}

/*
 desc : Calibration �̹��� (�˻��� ���)�� ���� ������ ��� ���� (Bitmap�� �̿��Ͽ� ���)
 parm : hdc		- [in]  �̹����� ��� ��� context
		draw	- [in]  �̹����� ��µ� ���� (�θ� ���� ���� ��� ��ǥ)
 retn : None
*/
API_EXPORT VOID uvMIL_DrawCaliMarkBitmap(HDC hdc, RECT draw)
{
	LPG_ACGR pstGrab	= NULL;

	// if (!g_pMilMain)	return;

	/* Mark ã�� ��� �� �������� */
	pstGrab	= theApp.clMilMain.GetLastGrabbedMark();
	/* 256 Gray Color �̹��� ������ ��� */
	if (pstGrab && pstGrab->score_rate > 0.0f)
	{
		DrawGrabMarkBitmap(hdc, draw, pstGrab, 0x01);
	}
}

/*
 desc : �˻��� Mark �̹��� ���� ������ ��� ���� (DC�� �̿��Ͽ� bitmap �̹����� ���)
 parm : hdc		- [in]  �̹����� ��� ��� context
		draw	- [in]  �̹����� ��µ� ���� (�θ� ���� ���� ��� ��ǥ)
		cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
		find	- [in]  ��ũ �˻� ���� ���� (�˻��ƴ���, �˻� ��� ���� ���� �޶���)
						0x00 - �˻� ��� ����, 0x01 - �˻� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_DrawMarkBitmap(HDC hdc, RECT draw, UINT8 cam_id, UINT8 img_id, UINT8 find)
{
	LPG_ACGR pstGrab	= NULL;

	// if (!g_pMilMain)	return FALSE;

	/* Mark ã�� ��� �� �������� */
	pstGrab	= theApp.clMilMain.GetGrabbedMark(cam_id, img_id);
	if (!pstGrab)				return FALSE;
	if (!pstGrab->grab_size)	return FALSE;
	/* 256 Gray Color �̹��� ������ ��� (�˻� �ȵǴ��� ������ �̹��� ���) */
	//DrawGrabMarkBitmap(hdc, draw, pstGrab, find);
	//BOOL CMilMain::RegistMILGrabImg(INT32 fi_No, INT32 width, INT32 height, PUINT8 image)
	if (pstGrab->grab_width < 1 || pstGrab->grab_height < 1 || !pstGrab->grab_data)	return TRUE;

	INT32 Grab_No = (cam_id - 1) * 2 + img_id;
	//INT32 Grab_No = (cam_id - 1) * 2 + (img_id - 1); // lk91 OnImageGrabbed() �Լ� ����... uvMIL_RegistMILGrabImg((m_u8CamID - 1) * 2 + (m_u8GrabIndex - 1), pstMark->grab_width, pstMark->grab_height, pstMark->grab_data);
	theApp.clMilMain.RegistMILGrabImg(Grab_No, pstGrab->grab_width, pstGrab->grab_height, pstGrab->grab_data); 
	DrawGrabMarkBitmap_MMPM(hdc, draw, pstGrab, Grab_No, find); 
	return TRUE;
}

/*
 desc : ���� �˻��� �̹��� (Model find)�� ���� ������ ��� ���� (MIL Buffer�� ����� ���� ���)
 parm : hwnd	- [in]  �̹����� ��� ��� context
		draw	- [in]  �̹����� ��µ� ���� (�θ� ���� ���� ��� ��ǥ)
		cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
 retn : None
*/
API_EXPORT VOID uvMIL_DrawMarkMBufID(HWND hwnd, RECT draw, UINT8 cam_id, UINT8 hwndIdx,UINT8 img_id)
{
#ifndef _NOT_USE_MIL_
	UINT8 i, u8Find		= 0x00;
	DOUBLE dbAngle = 0; //g_pstConfig->set_cams.acam_inst_angle == 0 ? 0.0f : 180.0f;
	//BUFATTRTYPE bufType	= M_IMAGE+M_PROC+M_DISP;
	MIL_INT64 bufType = M_IMAGE + M_PROC + M_DISP; 
	MIL_ID mlBufID		= M_NULL, mlBufResize, mlBufRotate = M_NULL;
	DOUBLE dbRate		= 1.0f, dbRotateCentX, dbRotateCentY;
	POINT ptLine[4]		= {NULL}, ptCent = {NULL}, ptMove = {NULL};

	RECT rDraw (draw);
	LPG_GMSR pstFind	= NULL;
#if 0
	LPG_ACGR pstGrab	= NULL;
#endif
	STG_DBXY stInCent	= {NULL};
	STG_DBWH stInSize	= {NULL};
	CSize szSrc, szTgt, szRot;

	// if (!g_pMilMain)	return;
	/* Get the buffer id of the recognized mark */
	mlBufID	= theApp.clMilMain.GetMarkBufID(cam_id, img_id);
	if (!mlBufID)	
		return;

#if 0
	/* Mark ã�� ��� �� �������� */
	pstGrab	= theApp.clMilMain.GetGrabbedMark(cam_id, img_id);
	if (!mlBufID || !pstGrab)	return;
#endif
	/* Get the size of original image (pixel) */
	szSrc.cx	= (UINT32)MbufInquire(mlBufID, M_SIZE_X, NULL);
	szSrc.cy	= (UINT32)MbufInquire(mlBufID, M_SIZE_Y, NULL);
		
	/* ���� �̹��� ȸ�� �� ũ�� ��� */
	szRot		= szSrc;
	uvCmn_RotateRectSize(dbAngle, szRot);
	/* ��� ���� ũ�⿡ �°� �̹��� ��� Ȥ�� Ȯ��� ũ�� ��� */
	if (!ResizeDrawBitmap(rDraw, szRot, szTgt, dbRate))	return;
	/* �̹��� ��� ���� ��ġ ��� */
	ptMove.x	= (LONG)ROUNDED(DOUBLE((rDraw.right - rDraw.left) - szTgt.cx) / 2.0f, 0) + rDraw.left;
	ptMove.y	= (LONG)ROUNDED(DOUBLE((rDraw.bottom- rDraw.top) - szTgt.cy) / 2.0f, 0) + rDraw.top;
	/* ȸ���� �� ũ�Ⱑ Ŀ���Ƿ�, ũ�� ������ */
	if (szTgt.cx > rDraw.right - rDraw.left)	szTgt.cx	= rDraw.right - rDraw.left;
	if (szTgt.cy > rDraw.bottom- rDraw.top)		szTgt.cy	= rDraw.bottom- rDraw.top;

	/* Binary Buffer �Ҵ� */
#if 1
	mlBufResize	= MbufAlloc2d(theApp.clMilMain.GetMilSysID(), szTgt.cx, szTgt.cy,
							  8+M_UNSIGNED, bufType, M_NULL);
#else	/* Grabbed Image�� ȭ�鿡 ��µǴ� �������� ���� ���, �� ��ƾ�� ������ ���� �͵� ... */
	CSize szResize (szTgt.cx, szTgt.cy);
	if (szResize.cx < (rDraw.right-rDraw.left))	szResize.cx = rDraw.right-rDraw.left;
	if (szResize.cy < (rDraw.bottom-rDraw.top))	szResize.cy = rDraw.bottom-rDraw.top;
	mlBufResize	= MbufAlloc2d(theApp.clMilMain.GetMilSysID(), szResize.cx, szResize.cy,
							  8+M_UNSIGNED, bufType, M_NULL);
#endif
	if (mlBufResize)
	{
		/* ȸ�� ���� �޸� �Ҵ� */
		mlBufRotate	= MbufAllocColor(theApp.clMilMain.GetMilSysID(), 3, szRot.cx, szRot.cy,
									 8+M_UNSIGNED, bufType, M_NULL);
		/* ȸ�� ������ ���� ���� �̹��� ȸ�� �� ȸ�� ���ۿ� ���� */
		if (mlBufRotate)
		{
			MimRotate(mlBufID, mlBufRotate, dbAngle,
					  M_DEFAULT, M_DEFAULT, /*(MIL_DOUBLE)szResize.cx/2.0f, (MIL_DOUBLE)szResize.cy/2.0f, */
					  M_DEFAULT, M_DEFAULT, /*(MIL_DOUBLE)szResize.cy/2.0f, (MIL_DOUBLE)szResize.cx/2.0f, */
					  M_NEAREST_NEIGHBOR+M_OVERSCAN_CLEAR);
			//			MbufExport(L"g:\\download\\qcells\\export_rotate.bmp", M_BMP, mlBufRotate);
			/* �̹��� ũ�� ������ �� �ӽ� ���ۿ� �Ҵ� */
			MimResize(mlBufRotate, mlBufResize, M_FILL_DESTINATION, M_FILL_DESTINATION, M_INTERPOLATE);

			MimFlip(mlBufResize, mlBufResize, M_FLIP_HORIZONTAL, M_DEFAULT);
			// lk91 test ////////////////////////
			//CDPoint dRtnP;
			//dRtnP.x = (double)szTgt.cx / (double)ACA1920_SIZE_X;
			//dRtnP.y = (double)szTgt.cy / (double)ACA1920_SIZE_Y;
			//MimResize(mlBufRotate, mlBufResize, dRtnP.x, dRtnP.y, M_DEFAULT);
			// lk91 test /////////////////////
			//MimResize(mlBufRotate, mlBufResize, M_FILL_DESTINATION, M_FILL_DESTINATION, M_INTERPOLATE);
			/* �̹��� ������ ������ Ư�� ���� ������ ��� */
			//MdispSelectWindow(theApp.clMilMain.GetDispID(DISP_TYPE_EXPO), mlBufResize, hwnd); // lk91 �̻��..?
			MdispSelectWindow(theApp.clMilMain.m_mDisID_EXPO[hwndIdx], mlBufResize, hwnd);

			/* ȸ�� ���� �޸� ���� */
			MbufFree(mlBufRotate);
		}

		/* Get the DC for hwnd */
		HDC hDC = ::GetDC(hwnd);
		if (hDC)
		{
			/*  Draw a big cross */
			HPEN hOldPen, hPen = ::CreatePen(PS_DOT, 1, RGB(255, 0, 0));
			if (hPen)
			{
				hOldPen	= (HPEN)::SelectObject(hDC, hPen);
				if (hOldPen)
				{
					::MoveToEx(hDC, 0, (szTgt.cy) / 2, NULL);
					::LineTo(hDC, szTgt.cx, (szTgt.cy) / 2);
					::MoveToEx(hDC, (szTgt.cx) / 2, 0, NULL);
					::LineTo(hDC, (szTgt.cx) / 2, szTgt.cy);

					//::MoveToEx(hDC, draw.left, (draw.bottom-draw.top)/2, NULL);
					//::LineTo(hDC, draw.right, (draw.bottom-draw.top)/2);
					//::MoveToEx(hDC, (draw.right-draw.left)/2, draw.top, NULL);
					//::LineTo(hDC, (draw.right-draw.left)/2, draw.bottom);
					::SelectObject(hDC, hOldPen);
				}
				::DeleteObject(hPen);
			}
#if 1
			/*  Draw a small cross in the center of the mark */
			hPen = ::CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
			if (hPen)
			{
				hOldPen	= (HPEN)::SelectObject(hDC, hPen);
				if (hOldPen)
				{
					/* �� �˻��� Mark�� ���� ���� ǥ�� (���) */
					u8Find = theApp.clMilMain.GetGrabbedMarkCount(cam_id, img_id);
					pstFind= theApp.clMilMain.GetGrabbedMarkAll(cam_id, img_id);
					for (i=0x00; i<u8Find; i++)
					{
						dbRotateCentX	= pstFind[i].cent_x;
						dbRotateCentY	= pstFind[i].cent_y;
						
						POINT half;
						half.x = szSrc.cx / 2.0f;
						half.y = szSrc.cy / 2.0f;
						auto withGab =  half.x - dbRotateCentX;
						dbRotateCentX += dbRotateCentX < half.x ? abs(withGab) * 2  : abs(withGab) * -2;
						if (dbAngle != 0.0f && pstFind[i].manual_set == 0x00)
						{
							/* Returns the center coordinate value of the current mark, */
							/* if rotated relative to the center of the Grabbed Image */
							uvCmn_RotateCoord(DOUBLE(szSrc.cx/2.0f), DOUBLE(szSrc.cy/2.0f),
											  pstFind[i].cent_x, pstFind[i].cent_y,
											  DOUBLE(-dbAngle/*g_pstConfig->set_cams.acam_inst_angle*/),	/* �׻� ȸ�� ������ - (����) ���� �ٿ���� �� (�ֳ��ϸ�, Grabbed Image ��, MIL�� ImgRotate �Լ� ȸ���� �ð� �ݴ� �������� �ϱ� ������ */
											  dbRotateCentX, dbRotateCentY);
						}
						/* ��µǴ� ���� ������ ����, Grabbed Image�� ��� or Ȯ�뿡 ���� ã�� Mark ���� ũ�� ��� or Ȯ�� */
						stInSize.cx	= pstFind[i].mark_width;
						stInSize.cy	= pstFind[i].mark_height;
						stInCent.x	= dbRotateCentX/*pstFind[i].cent_x*/;
						stInCent.y	= dbRotateCentY/*pstFind[i].cent_y*/;
						/* Get the center of mark in the grabbed images */
						GetDrawMarkArea(&stInSize, &stInCent, dbRate, ptCent, ptLine);





						// lk91!!! Mark ���� �׸��� �κ�
						/* Mark Lines (Rectangle) ��� */
						::MoveToEx(hDC, ptLine[0].x, ptLine[0].y, NULL);
						::LineTo(hDC, ptLine[1].x, ptLine[1].y);
						::LineTo(hDC, ptLine[2].x, ptLine[2].y);
						::LineTo(hDC, ptLine[3].x, ptLine[3].y);
						::LineTo(hDC, ptLine[0].x, ptLine[0].y);

						//int div = 10;
						//int divWidth = abs(ptLine[0].x - ptLine[2].x);
						//int divHeight = abs(ptLine[0].y - ptLine[2].y);
						//int wCnt = divWidth / div;
						//int hCnt = divHeight / div;
						//if ((divWidth % div) == 0)
						//	wCnt -= 1;
						//if ((divHeight % div) == 0)
						//	hCnt -= 1;

						//divWidth /= div;
						//divHeight /= div;
						//
// 						for (int i = 0; i < wCnt; i++) {
// 							::MoveToEx(hDC, ptLine[0].x + div * (i+1), ptLine[0].y, NULL);
// 							::LineTo(hDC, ptLine[0].x + div * (i+1), ptLine[2].y);
// 						}
// 						for (int i = 0; i < hCnt; i++) {
// 							::MoveToEx(hDC, ptLine[0].x, ptLine[0].y + div * (i + 1), NULL);
// 							::LineTo(hDC, ptLine[2].x, ptLine[0].y + div * (i + 1));
// 						}
// 
// 						::SelectObject(hDC, hOldPen);
// 						::DeleteObject(hPen);

						hPen = ::CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
						hOldPen = (HPEN)::SelectObject(hDC, hPen);


						/* Grabbed Image���� Mark�� �߽� ��ġ�� ��ȣ ('+') ��� */
						::MoveToEx(hDC, ptCent.x - 20, ptCent.y, NULL);
						::LineTo(hDC, ptCent.x + 20, ptCent.y);
						::MoveToEx(hDC, ptCent.x, ptCent.y - 20, NULL);
						::LineTo(hDC, ptCent.x, ptCent.y + 20);

						//::MoveToEx(hDC,	ptCent.x-5+ptMove.x,	ptCent.y+ptMove.y, NULL);
						//::LineTo(hDC,	ptCent.x+5+ptMove.x,	ptCent.y+ptMove.y);
						//::MoveToEx(hDC,	ptCent.x+ptMove.x,		ptCent.y-5+ptMove.y, NULL);
						//::LineTo(hDC,	ptCent.x+ptMove.x,		ptCent.y+5+ptMove.y);
						///* Mark Lines (Rectangle) ��� */
						//::MoveToEx(hDC,	ptLine[0].x+ptMove.x,	ptLine[0].y+ptMove.y, NULL);
						//::LineTo(hDC,	ptLine[1].x+ptMove.x,	ptLine[1].y+ptMove.y);
						//::LineTo(hDC,	ptLine[2].x+ptMove.x,	ptLine[2].y+ptMove.y);
						//::LineTo(hDC,	ptLine[3].x+ptMove.x,	ptLine[3].y+ptMove.y);
						//::LineTo(hDC,	ptLine[0].x+ptMove.x,	ptLine[0].y+ptMove.y);
					}
					if (u8Find > 4)
					{
						LPG_ACGR pstResult = theApp.clMilMain.GetGrabbedMark(cam_id, img_id);
						if (pstResult)
						{
							dbRotateCentX	= pstResult->mark_cent_px_x;
							dbRotateCentY	= pstResult->mark_cent_px_y;
							if (dbAngle != 0.0f && pstResult->manual_set == 0x00)
							{
								/* Returns the center coordinate value of the current mark, */
								/* if rotated relative to the center of the Grabbed Image */
								uvCmn_RotateCoord(DOUBLE(szSrc.cx/2.0f), DOUBLE(szSrc.cy/2.0f),
												  pstResult->mark_cent_px_x, pstResult->mark_cent_px_y,
												  DOUBLE(-dbAngle/*g_pstConfig->set_cams.acam_inst_angle*/),	/* �׻� ȸ�� ������ - (����) ���� �ٿ���� �� (�ֳ��ϸ�, Grabbed Image ��, MIL�� ImgRotate �Լ� ȸ���� �ð� �ݴ� �������� �ϱ� ������ */
												  dbRotateCentX, dbRotateCentY);
							}
							/* ��µǴ� ���� ������ ����, Grabbed Image�� ��� or Ȯ�뿡 ���� ã�� Mark ���� ũ�� ��� or Ȯ�� */
							stInSize.cx	= pstResult->mark_width_px;
							stInSize.cy	= pstResult->mark_height_px;
							stInCent.x	= dbRotateCentX/*pstFind[i].cent_x*/;
							stInCent.y	= dbRotateCentY/*pstFind[i].cent_y*/;
							/* Get the center of mark in the grabbed images */
							GetDrawMarkArea(&stInSize, &stInCent, dbRate, ptCent, ptLine);
							/* Grabbed Image���� Mark�� �߽� ��ġ�� ��ȣ ('+') ��� */
							::MoveToEx(hDC,	ptCent.x-5+ptMove.x,	ptCent.y+ptMove.y, NULL);
							::LineTo(hDC,	ptCent.x+5+ptMove.x,	ptCent.y+ptMove.y);
							::MoveToEx(hDC,	ptCent.x+ptMove.x,		ptCent.y-5+ptMove.y, NULL);
							::LineTo(hDC,	ptCent.x+ptMove.x,		ptCent.y+5+ptMove.y);
							/* Mark Lines (Rectangle) ��� */
							::MoveToEx(hDC,	ptLine[0].x+ptMove.x,	ptLine[0].y+ptMove.y, NULL);
							::LineTo(hDC,	ptLine[1].x+ptMove.x,	ptLine[1].y+ptMove.y);
							::LineTo(hDC,	ptLine[2].x+ptMove.x,	ptLine[2].y+ptMove.y);
							::LineTo(hDC,	ptLine[3].x+ptMove.x,	ptLine[3].y+ptMove.y);
							::LineTo(hDC,	ptLine[0].x+ptMove.x,	ptLine[0].y+ptMove.y);

							

						}
					}

					::SelectObject(hDC, hOldPen);

				}
				::DeleteObject(hPen);
			}
#endif
			/* Release the DC */
			::ReleaseDC(hwnd, hDC);
		}

		/* �̹��� ���� */
		MbufFree(mlBufResize);
	}
#endif
}

/*
 desc : Calibration �̹��� ���� ������ ��� ���� (Bitmap�� �̿��Ͽ� ���)
 parm : hdc		- [in]  �̹����� ��� ��� context
		draw	- [in]  �̹����� ��µ� ���� (�θ� ���� ���� ��� ��ǥ)
		grab	- [in]  grab �̹����� ����� ����ü ������ (grab_data�� �̹����� ������ �ݵ�� ����Ǿ� �־�� ��)
		find	- [in]  ��ũ �˻� ���� ���� (�˻��ƴ���, �˻� ��� ���� ���� �޶���)
						0x00 - �˻� ��� ����, 0x01 - �˻� ��� ����
 retn : None
*/
API_EXPORT VOID uvMIL_DrawMarkDataBitmap(HDC hdc, RECT draw, LPG_ACGR grab, UINT8 find , bool drawForce,UINT8 flipFlag)
{
	// if (!g_pMilMain)	return;

	/* 256 Gray Color �̹��� ������ ��� */
	if (grab && grab->score_rate > 0.0f || drawForce)
	{
		DrawGrabMarkBitmap(hdc, draw, grab, find, flipFlag);
	}
}

/*
 desc : Drawing - Examination Object Image (Bitmap�� �̿��Ͽ� ���)
 parm : hdc		- [in]  �̹����� ��� ��� context
		draw	- [in]  �̹����� ��µ� ���� (�θ� ���� ���� ��� ��ǥ)
		grab_out- [in]  Grabbed Image ������ ����� ����ü ������ (�ٱ� ��)
		grab_in	- [in]  Grabbed Image ������ ����� ����ü ������ (���� ��)
 retn : None
*/
API_EXPORT VOID uvMIL_DrawGrabExamBitmap(HDC hdc, RECT draw, LPG_ACGR grab_out, LPG_ACGR grab_in)
{
	DOUBLE dbRate		= 1.0f;
	DOUBLE dbAngle		= g_pstConfig->set_cams.acam_inst_angle == 0 ? 0.0f : 180.0f;
	POINT ptLine[2][4]	= {NULL}, ptCent[2] = {NULL}, ptDiff[2] = {NULL};
	COLORREF clrFind[2]	= { RGB(255, 0, 0), RGB(0, 0, 255) };
	POINT ptMove;
	RECT rDraw (draw);
	STG_DBXY stCent;
	STG_DBWH stSize;
	CSize szSrc (grab_out->grab_width, grab_out->grab_height), szTgt;
	/* Memory DC ������ Grabbed Image ��� �� �˻��� ��� �� �׸��� */
	HPEN hPen, hOldPen;

	/* ������ �����Ͱ� ���� ������� */
	if (!(grab_out && grab_in))	return;
	if (grab_out->score_rate == 0.0f|| grab_in->score_rate == 0.0f)		return;
	if (grab_out->grab_width < 1.0f	|| grab_out->grab_height < 1.0f)	return;
	if (grab_in->grab_width < 1.0f	|| grab_in->grab_height < 1.0f)		return;

	/* ��� ���� ���� ���� */
	rDraw.left++;
	rDraw.top++;
	rDraw.right--;
	rDraw.bottom--;

	/* ���ο� ������ �̹��� ũ�� �� ��, ���� ���̳��� �κ��� �������� ���� */
	if (!ResizeDrawBitmap(rDraw, szSrc, szTgt, dbRate))	return;
	/* ��µǴ� ���� ������ ����, Grabbed Image�� ��� or Ȯ�뿡 ���� ã�� Mark ���� ũ�� ��� or Ȯ�� */
	if (grab_out->score_rate > 0.0f)
	{
		/* Grabbed Image 180�� ȸ�� �����Ƿ� */
		if (g_pstConfig->set_cams.acam_inst_angle)
		{
			/* Grabbed Image�� �߽ɿ��� ȸ������ ���, ���� ��ũ�� �߽� ��ǥ �� ��ȯ */
			uvCmn_RotateCoord(DOUBLE(grab_out->grab_width/2.0f), DOUBLE(grab_out->grab_height/2.0f),
							  grab_out->mark_cent_px_x, grab_out->mark_cent_px_y,
							  DOUBLE(-dbAngle/*g_pstConfig->set_cams.acam_inst_angle*/),	/* �׻� ȸ�� ������ - (����) ���� �ٿ���� �� (�ֳ��ϸ�, Grabbed Image ��, MIL�� ImgRotate �Լ� ȸ���� �ð� �ݴ� �������� �ϱ� ������ */
							  stCent.x, stCent.y);
		}
		/* �ٱ��� Mark (Object) ���� �׸��� */
		stSize.cx	= grab_out->mark_width_px;
		stSize.cy	= grab_out->mark_height_px;
		GetDrawMarkArea(&stSize, &stCent, dbRate, ptCent[0], ptLine[0]);
		/* Grabbed Image 180�� ȸ�� �����Ƿ� */
		if (g_pstConfig->set_cams.acam_inst_angle)
		{
			/* Grabbed Image�� �߽ɿ��� ȸ������ ���, ���� ��ũ�� �߽� ��ǥ �� ��ȯ */
			uvCmn_RotateCoord(DOUBLE(grab_in->grab_width/2.0f), DOUBLE(grab_in->grab_height/2.0f),
							  grab_in->mark_cent_px_x, grab_in->mark_cent_px_y,
							  DOUBLE(-dbAngle/*g_pstConfig->set_cams.acam_inst_angle*/),	/* �׻� ȸ�� ������ - (����) ���� �ٿ���� �� (�ֳ��ϸ�, Grabbed Image ��, MIL�� ImgRotate �Լ� ȸ���� �ð� �ݴ� �������� �ϱ� ������ */
							  stCent.x, stCent.y);
		}
		/* ������ Mark (Object) ���� �׸��� */
		stSize.cx	= grab_in->mark_width_px;
		stSize.cy	= grab_in->mark_height_px;
		GetDrawMarkArea(&stSize, &stCent, dbRate, ptCent[1], ptLine[1]);
	}

	/* �̹��� ��� ���� ��ġ ��� */
	ptMove.x = (UINT32)ROUNDED(DOUBLE((rDraw.right - rDraw.left) - szTgt.cx) / 2.0f, 0) + rDraw.left;
	ptMove.y = (UINT32)ROUNDED(DOUBLE((rDraw.bottom- rDraw.top) - szTgt.cy) / 2.0f, 0) + rDraw.top;

	/* Grabbed Image ��� */
	DrawGrabImage(hdc, grab_out, rDraw, &szTgt, &ptMove);

	/* -------------------------------------- */
	/* Mark Lines (Rectangle) ��� (Ring Out) */
	/* -------------------------------------- */
	hPen	= ::CreatePen(PS_SOLID, 1, clrFind[0]);
	hOldPen	= (HPEN)::SelectObject(hdc, hPen);
	if (hOldPen)
	{
		/* Mark �ܰ� ������ �簢�� ��� */
		::MoveToEx(hdc,	ptLine[0][0].x+ptMove.x,	ptLine[0][0].y+ptMove.y, NULL);
		::LineTo(hdc,	ptLine[0][1].x+ptMove.x,	ptLine[0][1].y+ptMove.y);
		::LineTo(hdc,	ptLine[0][2].x+ptMove.x,	ptLine[0][2].y+ptMove.y);
		::LineTo(hdc,	ptLine[0][3].x+ptMove.x,	ptLine[0][3].y+ptMove.y);
		::LineTo(hdc,	ptLine[0][0].x+ptMove.x,	ptLine[0][0].y+ptMove.y);
		/* Mark �߽ɿ��� Cross (+) ��� */
		::MoveToEx(hdc,	ptCent[0].x-5+ptMove.x,		ptCent[0].y+ptMove.y, NULL);
		::LineTo(hdc,	ptCent[0].x+6+ptMove.x,		ptCent[0].y+ptMove.y);
		::MoveToEx(hdc,	ptCent[0].x+ptMove.x,		ptCent[0].y-5+ptMove.y, NULL);
		::LineTo(hdc,	ptCent[0].x+ptMove.x,		ptCent[0].y+6+ptMove.y);
		::SelectObject(hdc, hOldPen);
	}
	if (hPen)	::DeleteObject(hPen);

	/* -------------------------------------- */
	/* Mark Lines (Rectangle) ��� (Ring In)  */
	/* -------------------------------------- */
	hPen	= ::CreatePen(PS_SOLID, 1, clrFind[1]);
	hOldPen	= (HPEN)::SelectObject(hdc, hPen);
	if (hOldPen)
	{
		/* Mark �ܰ� ������ �簢�� ��� */
		::MoveToEx(hdc,	ptLine[1][0].x+ptMove.x,	ptLine[1][0].y+ptMove.y, NULL);
		::LineTo(hdc,	ptLine[1][1].x+ptMove.x,	ptLine[1][1].y+ptMove.y);
		::LineTo(hdc,	ptLine[1][2].x+ptMove.x,	ptLine[1][2].y+ptMove.y);
		::LineTo(hdc,	ptLine[1][3].x+ptMove.x,	ptLine[1][3].y+ptMove.y);
		::LineTo(hdc,	ptLine[1][0].x+ptMove.x,	ptLine[1][0].y+ptMove.y);
		/* Mark �߽ɿ��� Cross (+) ��� */
		::MoveToEx(hdc,	ptCent[1].x-5+ptMove.x,		ptCent[1].y+ptMove.y, NULL);
		::LineTo(hdc,	ptCent[1].x+6+ptMove.x,		ptCent[1].y+ptMove.y);
		::MoveToEx(hdc,	ptCent[1].x+ptMove.x,		ptCent[1].y-5+ptMove.y, NULL);
		::LineTo(hdc,	ptCent[1].x+ptMove.x,		ptCent[1].y+6+ptMove.y);

		::SelectObject(hdc, hOldPen);
	}
	if (hPen)	::DeleteObject(hPen);
}

/*
 desc : Edge Detection �˻��� ��� ��ȯ
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 retn : ��� ���� ����� ������ ��ȯ
*/
API_EXPORT LPG_EDFR uvMIL_GetEdgeDetectResults(UINT8 cam_id)
{
	// if (!g_pMilMain)	return NULL;
	return theApp.clMilMain.GetEdgeDetectResults(cam_id);
}

/*
 desc : Edge Detection �˻��� ���� ��ȯ
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 ��ȯ : ���� ��ȯ. ������ ���, 0 ���� ��
*/
API_EXPORT INT32 uvMIL_GetEdgeDetectCount(UINT8 cam_id)
{
	// if (!g_pMilMain)	return 0;
	return theApp.clMilMain.GetEdgeDetectCount(cam_id);
}

/*
 desc : ���� ��ϵ� Mark Model Type ��ȯ
 parm : cam_id	- [in]  Align Camera Index (0x01 ~ MAX_INSTALL_CAMERA)
		mark_id	- [in]  Align Mark Index (0x00 ~ MAX_REGIST_MODEL-1)
 retn : Model Type ��
*/
API_EXPORT UINT32 uvMIL_GetMarkModelType(UINT8 cam_id, UINT8 index)
{
	// if (!g_pMilMain)	return 0;
	return theApp.clMilMain.GetMarkModelType(cam_id, index);
}

/*
 desc : ���� ��ϵ� Mark Model ũ�� ��ȯ
 parm : cam_id	- [in]  Grabbed Image ������ �߻��� Align Camera Index (1 or 2)
		index	- [in]  ��û�ϰ��� �ϴ� ���� ��ġ (Zero-based)
		flag	- [in]  0x00 : ����  ũ��, 0x01 : ���� ũ��
		unit	- [in]  0x00 : um, 0x01 : pixel
 retn : ũ�� ��ȯ (����: um)
*/
API_EXPORT DOUBLE uvMIL_GetMarkModelSize(UINT8 cam_id, UINT8 index, UINT8 flag, UINT8 unit)
{
	// if (!g_pMilMain)	return 0.0f;
	return theApp.clMilMain.GetMarkModelSize(cam_id, index, flag, unit);
}

/*
 desc : Align Mark �˻� ��� ����
 parm : method	- [in]  0x00 : ������ 1�� ��ȯ, 0x01 : ���� ��ũ �������� ����ġ�� �ο��Ͽ� ��� ��ȯ
		count	- [in]  'method' ���� 2 or 3�� ���, ���� �˻��� Mark ���� �� 2 �̻� ���̾�� ��
 retn : None
*/
API_EXPORT VOID uvMIL_SetMarkMethod(ENG_MMSM method, UINT8 count)
{
	//if (g_pMilMain)	
		theApp.clMilMain.SetMarkMethod(method, count);
}

API_EXPORT UINT8 uvMIL_SetMarkFindSetCount(int camNum)
{
	return theApp.clMilMain.GetMarkFindSetCount(camNum);
	
}


API_EXPORT UINT8 uvMIL_SetMarkFoundCount(int camNum)
{
	return theApp.clMilMain.GetMarkFindedCount(camNum);

}



/*
 desc : �̹����� ���� Ȥ�� ������ ��輱 �߽� ��ġ ���
 parm : cam_id		- [in]  Align Camera ID (1 or 2)
		grab_data	- [in]  Grabbed Image ������ ����� ����ü ������
		mil_result	- [in]  MIL ���� �˻� ��� ��ȭ ���� ��� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_RunLineCentXY(UINT8 cam_id, LPG_ACGR grab_data, BOOL mil_result)
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.RunLineCentXY(cam_id, grab_data, mil_result);
}

/*
 desc : Iamge ȸ�� (�̹����� �߽��� �������� ȸ��)
 parm : img_src		- [in]  ���� �̹��� ����
		img_dst		- [out] ȸ���� �̹��� ����
		width		- [in]	���� �̹����� ����
		height		- [in]	���� �̹����� ����
		angle		- [in]	ȸ�� �ϰ��� �ϴ� ����
 retn : None
*/
API_EXPORT VOID uvMIL_ImageRotate(PUINT8 img_src, PUINT8 img_dst,
								  UINT32 width, UINT32 height, UINT32 angle)
{
	// if (!g_pMilMain)	return;

	UINT32 u32RotateX	= (UINT32)ROUNDED(width / 2.0f, 0);
	UINT32 u32RotateY	= (UINT32)ROUNDED(height / 2.0f, 0);
	//if (g_pMilMain)	
		theApp.clMilMain.ImageRotate(img_src, img_dst, width, height, angle, u32RotateX, u32RotateY);
}

/*
 desc : ���� MIL ���̼����� ��ȿ���� ���� Ȯ��
		Pattern, Model Finder, Edge Finder, Read, OCR, Image Processing �� ��ȿ���� Ȯ��
 parm : None
 retn : TRUE or FALS
*/
API_EXPORT BOOL uvMIL_IsLicenseValid()
{
	return theApp.clMilMain.IsLicenseValid();
}

/*
 desc : �뱤 ��� ���� ��, ���� �뱤, ����� �뱤, ���� �� ����� �뱤
 parm : mode	- [in]  ���� �뱤 (0x00), ����� �뱤 (0x01), ����� ī�޶� ���� �� ���� �� ����� �뱤 (0x02)
 retn : None
*/
API_EXPORT VOID uvMIL_SetAlignMode(ENG_AOEM mode)
{
	// if (!g_pMilMain)	return;
	theApp.clMilMain.SetAlignMode(mode);
}

/*
 desc : ī�޶�κ��� ������ �̹��� ������ ������ ���Ϸ� ����
 parm : file	- [in]  �����Ϸ��� ���� (��ü ��� ����)
		width	- [in]  �̹����� ũ�� (����: pixel)
		height	- [in]  �̹����� ũ�� (����: pixel)
		image	- [in]  �̹����� ����� ����
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_SaveGrabbedToFile(PTCHAR file, INT32 width, INT32 height, PUINT8 image)
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.SaveGrabbedToFile(file, width, height, image);
}

/*
 desc : ���� ����� �������� ��ũ �˻� ���� �� ����
 parm : score	- [in]  Score Accpetance (��� �̹������� �˻��� ��ũ�� Score ���� �� ������ ������ ������)
		scale	- [in]  Scale Range (��� �̹������� ��ϵ� ��ũ�� �˻� ����� �߿��� �� ���� ���� ���� )
									(���Ե� ����鸸 ��ȿ�ϰ�, �� �߿��� Score ���� ���� ���� �͸� ����)
									(����. �� ���� 50.0000 �̸�, ���� �˻��� ���Ǵ� ��ũ�� ũ�� ����)
									(�˻����� = 1.0f - (100.0 - 50.0) / 100 �� ��, 0.5 ~ 1.5 ��)
 retn : None
*/
API_EXPORT VOID uvMIL_SetRecipeMarkRate(DOUBLE score, DOUBLE scale)
{
	// if (!g_pMilMain)	return;
	theApp.clMilMain.SetRecipeMarkRate(score, scale);
}

/*
 desc : ���������� �̵� ���� ���� (������ �̵����� / ������ �̵����� ����)
 parm : direct	- [in]  TRUE (������ �̵� : �տ��� �ڷ� �̵�), FALSE (������ �̵�)
 retn : None
*/
API_EXPORT VOID uvMIL_SetMoveStateDirect(BOOL direct)
{
	// if (!g_pMilMain)	return;
	theApp.clMilMain.SetMoveStateDirect(direct);
}

/*
 desc : Grabbed Image ���� ǥ���� ��ũ�� ������ ���� ���� ���, ��ũ ���� ������ ������ ũ�� �� ����
 parm : width	- [in]  �簢�� ������ ���� �� (����: um)
		height	- [in]  �簢�� ������ ���� �� (����: um)
 retn : None
*/
API_EXPORT VOID uvMIL_SetMultiMarkArea(UINT32 width, UINT32 height)
{
	if (NULL == g_pstConfig)
	{
		return;
	}
	DOUBLE dbPixelToMM	= g_pstConfig->acam_spec.GetPixelToMM(0x00 /* fixed T.T */);

	// if (!g_pMilMain)	return;

	g_dbMuliAreaWidth	= width  / (dbPixelToMM * 1000.0f);
	g_dbMuliAreaHeight	= height / (dbPixelToMM * 1000.0f);

	/* MMF �� ���, �˻��� ��ũ�� ��輱 ������ �簢������ �׸��� ���� (Mark �� MMF ������ ���, ��ũ ũ�� ��) */
	theApp.clMilMain.SetMultiMarkArea(width, height);
}

/* ----------------------------------------------------------------------------------------- */
/*                                 lk91 VISION �߰� �Լ�                                     */
/* ----------------------------------------------------------------------------------------- */
/* desc : MarkROI �ʱ�ȭ */
API_EXPORT VOID uvMIL_InitMarkROI(LPG_CRD fi_MarkROI)
{
	CRect tmpROI;
	for (int i = 0; i < g_pstConfig->set_cams.acam_count; i++) {
		tmpROI.left = fi_MarkROI->roi_Left[i];
		tmpROI.right = fi_MarkROI->roi_Right[i];
		tmpROI.top = fi_MarkROI->roi_Top[i];
		tmpROI.bottom = fi_MarkROI->roi_Bottom[i];
		theApp.clMilMain.MilSetMarkROI(i, tmpROI);
	}
}

API_EXPORT VOID uvMIL_SetAlignMotionPtr(AlignMotion& ptr)
{
	alignMotionPtr = &ptr;
}

/* desc : ���� �˻��� Live �̹��� ���� ������ ��� ���� (Bitmap�� �̿��Ͽ� ���) */
API_EXPORT VOID uvMIL_DrawLiveBitmap(HDC hdc, RECT draw, LPG_ACGR grab, DOUBLE angle)
{
	UINT32 u32MoveX, u32MoveY;
	DOUBLE dbRate;
	PUINT8 pImgBuff = NULL;
	RECT rDraw(draw);
	CSize szSrc(grab->grab_width, grab->grab_height), szTgt;
	/* Memory DC ������ Live Image ��� �׸��� */
	HDC hMemDC;
	HBRUSH hBrush, hOldBrush;
	HBITMAP hMemBmp, hMemBmpOld;

	/* ��� ���� ���� ���� */
	rDraw.left++;
	rDraw.top++;
	rDraw.right--;
	rDraw.bottom--;

	/* ���ο� ������ �̹��� ũ�� �� ��, ���� ���̳��� �κ��� �������� ���� */
	if (!ResizeDrawBitmap(rDraw, szSrc, szTgt, dbRate))	return;

	/* ���� DC ���� �ʱ�ȭ */
	hBrush = ::CreateSolidBrush(RGB(32, 32, 32));
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	if (!hOldBrush)
	{
		SelectObject(hdc, hOldBrush);
		::DeleteObject(hBrush);
		return;
	}

	/* �޸� DC�� ��Ʈ�� �����ϰ� ���� ���� �ֱ� */
	hMemDC = ::CreateCompatibleDC(hdc);
	hMemBmp = ::CreateCompatibleBitmap(hdc, rDraw.right - rDraw.left, rDraw.bottom - rDraw.top);
	if (!hMemBmp)
	{
		SelectObject(hdc, hOldBrush);
		::DeleteObject(hBrush);
		if (hMemDC)	::DeleteDC(hMemDC);
		return;
	}
	/* Memory DC�� Bitmap ���� ���� */
	hMemBmpOld = (HBITMAP)::SelectObject(hMemDC, hMemBmp);

	/* �̹��� ��� ���� ��ġ ��� */
	u32MoveX = (UINT32)ROUNDED(DOUBLE((rDraw.right - rDraw.left) - szTgt.cx) / 2.0f, 0) + rDraw.left;
	u32MoveY = (UINT32)ROUNDED(DOUBLE((rDraw.bottom - rDraw.top) - szTgt.cy) / 2.0f, 0) + rDraw.top;

	/* Check if the input image is rotated */
	pImgBuff = grab->grab_data;
	if (!GetConfig()->IsRunDemo())
	{
		if (0.0f != angle)
		{
			/* Initialize existing temporary memory */
			g_pGrabBuff[grab->grab_size] = 0x00;
			pImgBuff = g_pGrabBuff;
			theApp.clMilMain.ImageRotate(grab->grab_data, g_pGrabBuff,
				grab->grab_width, grab->grab_height,
				angle/*g_pstConfig->set_cams.acam_inst_angle*/,
				grab->grab_width / 2, grab->grab_height / 2);
		}
	}
	else
	{
		/* Initialize existing temporary memory */
		g_pGrabBuff[grab->grab_size] = 0x00;
		pImgBuff = g_pGrabBuff;
		//g_pGrabBuff = pImgBuff; 
		theApp.clMilMain.ImageFlip(grab->grab_data, g_pGrabBuff,
			grab->grab_width, grab->grab_height, 0x01);
	}

	/* ȭ�� DC�� ��� */
	::SetStretchBltMode(hdc, COLORONCOLOR);
	::StretchDIBits(hMemDC, 0, 0, szTgt.cx, szTgt.cy,
		0, 0, grab->grab_width, grab->grab_height,
		pImgBuff, g_pBmpInfo, DIB_RGB_COLORS, SRCCOPY);
	::StretchBlt(hdc, u32MoveX, u32MoveY, rDraw.right - rDraw.left, rDraw.bottom - rDraw.top,
		hMemDC, 0, 0, szTgt.cx, szTgt.cy, SRCCOPY);

	if (hBrush)		::DeleteObject(hBrush);
	if (hMemBmp)	::DeleteObject(hMemBmp);
	if (hMemDC)		::DeleteDC(hMemDC);
}

/* desc : Set Live Disp Size */
API_EXPORT VOID uvMIL_SetMarkLiveDispSize(CSize fi_size)
{
	theApp.clMilMain.LIVE_DISP_SIZE_X = fi_size.cx;
	theApp.clMilMain.LIVE_DISP_SIZE_Y = fi_size.cy;
}

/* desc : Set CalbCamSpec Disp Size */
API_EXPORT VOID uvMIL_SetCalbCamSpecDispSize(CSize fi_size)
{
	theApp.clMilMain.CALIB_CAMSPEC_DISP_SIZE_X = fi_size.cx;
	theApp.clMilMain.CALIB_CAMSPEC_DISP_SIZE_Y = fi_size.cy;
}

/* desc : Set CalbCamSpec Disp Size */
API_EXPORT VOID uvMIL_SetAccuracyMeasureDispSize(CSize fi_size)
{
	theApp.clMilMain.ACCR_DISP_SIZE_X = fi_size.cx;
	theApp.clMilMain.ACCR_DISP_SIZE_Y = fi_size.cy;
}

/* desc : Set CalbStep Disp Size */
API_EXPORT VOID uvMIL_SetCalbStepDispSize(CSize fi_size)
{
	theApp.clMilMain.CALIB_EXPO_DISP_SIZE_X = fi_size.cx;
	theApp.clMilMain.CALIB_EXPO_DISP_SIZE_Y = fi_size.cy;
}

/* desc : Set MMPM Disp Size */
API_EXPORT VOID uvMIL_SetMMPMDispSize(CSize fi_size)
{
	theApp.clMilMain.MMPM_DISP_SIZE_X = fi_size.cx;
	theApp.clMilMain.MMPM_DISP_SIZE_Y = fi_size.cy;
}
 

/* desc : MIL ID �� ����� �̹��� ȭ�� ��� */
API_EXPORT VOID uvMIL_DrawImageBitmapFlip(int dispType, int Num, LPG_ACGR grab, DOUBLE angle, int cam_id, int  flipOption)
{
#ifndef _NOT_USE_MIL_
	PUINT8 pImgBuff = NULL;
	CSize szSrc(grab->grab_width, grab->grab_height), szTgt;
	
	CDPoint dRateP;
	dRateP = theApp.clMilDisp.GetRateDispToBuf( dispType);

	MIL_ID srcID[2] = { 0L,0L };
	MIL_ID dstID[2] = { 0L,0L };

	srcID[0] = dispType == DISP_TYPE_EXPO ? theApp.clMilMain.m_mImgProc[cam_id] :
				dispType == DISP_TYPE_MARK ? theApp.clMilMain.m_mImg_Mark[Num] :
				dispType == DISP_TYPE_MARK_LIVE ? theApp.clMilMain.m_mImgProc[cam_id] :
				dispType == DISP_TYPE_MARKSET ? theApp.clMilMain.m_mImg_MarkSet :
				dispType == DISP_TYPE_MMPM ? theApp.clMilMain.m_mImg_Grab[Num] :
				dispType == DISP_TYPE_CALB_CAMSPEC ? theApp.clMilMain.m_mImg_CALI_CAMSPEC[cam_id] :
				dispType == DISP_TYPE_CALB_EXPO ? theApp.clMilMain.m_mImg_CALI_STEP[0] :
				dispType == DISP_TYPE_CALB_ACCR ? theApp.clMilMain.m_mImg_ACCR[cam_id] :
				dispType == DISP_TYPE_MMPM_AUTOCENTER ? theApp.clMilMain.m_mImg_MPMM_AutoCenter : srcID[0];

	srcID[1] = dispType == DISP_TYPE_CALB_EXPO ? theApp.clMilMain.m_mImg_CALI_STEP[1] : srcID[1];

	dstID[0] = dispType == DISP_TYPE_EXPO ? theApp.clMilMain.m_mImgDisp_EXPO[Num] :
		dispType == DISP_TYPE_MARK ? theApp.clMilMain.m_mImgDisp_Mark[Num] :
		dispType == DISP_TYPE_MARK_LIVE ? theApp.clMilMain.m_mImgDisp_Mark_Live :
		dispType == DISP_TYPE_MARKSET ? theApp.clMilMain.m_mImgDisp_MarkSet :
		dispType == DISP_TYPE_MMPM ? theApp.clMilMain.m_mImgDisp_MMPM :
		dispType == DISP_TYPE_CALB_CAMSPEC ? theApp.clMilMain.m_mImgDisp_CALI_CAMSPEC :
		dispType == DISP_TYPE_CALB_EXPO ? theApp.clMilMain.m_mImgDisp_CALI_STEP[0] :
		dispType == DISP_TYPE_CALB_ACCR ? theApp.clMilMain.m_mImgDisp_ACCR :
		dispType == DISP_TYPE_MMPM_AUTOCENTER ? theApp.clMilMain.m_mImgDisp_MPMM_AutoCenter : dstID[0];

	dstID[1] = dispType == DISP_TYPE_CALB_EXPO ? theApp.clMilMain.m_mImgDisp_CALI_STEP[1] : dstID[1];
	
	
	for (int i = 0; i < 2; i++) if (srcID[i] != 0)
	{
		MbufClear(dstID[i], 0L);
		MIL_ID milID = 0L;
		if (flipOption == -1)
		{
			MimResize(srcID[i], dstID[i], dRateP.x, dRateP.y, M_DEFAULT);
		}
		else
		{
			MIL_ID milID = MbufAlloc2d(theApp.clMilMain.GetMilSysID(), szSrc.cx, szSrc.cy, 8 + M_UNSIGNED, M_IMAGE + M_PROC + M_DISP, M_NULL);
			MimResize(srcID[i], milID, dRateP.x, dRateP.y, M_DEFAULT);
			MimFlip(milID, dstID[i], flipOption == 0 ? M_FLIP_HORIZONTAL : M_FLIP_VERTICAL, M_DEFAULT);
			MbufFree(milID);
		}
	}
	
	theApp.clMilDisp.DrawOverlayDC(true, dispType, Num);

	//theApp.clMilDisp.DrawBase(dispType, Num);
	//theApp.clMilDisp.DrawOverlayDC(true, dispType, Num);

#endif
}


/* desc : MIL ID �� ����� �̹��� ȭ�� ��� */
API_EXPORT VOID uvMIL_DrawImageBitmap(int dispType, int Num, LPG_ACGR grab, DOUBLE angle, int cam_id)
{
	//UINT32 u32MoveX, u32MoveY;
	//DOUBLE dbRate;
	PUINT8 pImgBuff = NULL;
	CSize szSrc(grab->grab_width, grab->grab_height), szTgt;
	//HDC hdc = ::GetDC(hWnd);
	/* Memory DC ������ Live Image ��� �׸��� */
	CDPoint dRateP;
	dRateP = theApp.clMilDisp.GetRateDispToBuf( dispType);
	
#ifndef _NOT_USE_MIL_
	if (dispType == DISP_TYPE_EXPO) {	// EXPO
		MbufClear(theApp.clMilMain.m_mImgDisp_EXPO[Num], 0L);
		MimResize(theApp.clMilMain.m_mImgProc[cam_id], theApp.clMilMain.m_mImgDisp_EXPO[Num], dRateP.x, dRateP.y, M_DEFAULT);
	}
	else if (dispType == DISP_TYPE_MARK) {	// MARK
		MbufClear(theApp.clMilMain.m_mImgDisp_Mark[Num], 0L);
		MimResize(theApp.clMilMain.m_mImg_Mark[Num], theApp.clMilMain.m_mImgDisp_Mark[Num], dRateP.x, dRateP.y, M_DEFAULT);
		return;
	}
	else if (dispType == DISP_TYPE_MARK_LIVE) {	// LIVE
		MbufClear(theApp.clMilMain.m_mImgDisp_Mark_Live, 0L);
		MimResize(theApp.clMilMain.m_mImgProc[cam_id], theApp.clMilMain.m_mImgDisp_Mark_Live, dRateP.x, dRateP.y, M_DEFAULT);
	}
	else if (dispType == DISP_TYPE_MARKSET) {	// MARK SET
		MbufClear(theApp.clMilMain.m_mImgDisp_MarkSet, 0L);
		MimResize(theApp.clMilMain.m_mImg_MarkSet, theApp.clMilMain.m_mImgDisp_MarkSet, dRateP.x, dRateP.y, M_DEFAULT);
	}
	else if (dispType == DISP_TYPE_MMPM) {	// MMPM
		MbufClear(theApp.clMilMain.m_mImgDisp_MMPM, 0L);
		MimResize(theApp.clMilMain.m_mImg_Grab[Num], theApp.clMilMain.m_mImgDisp_MMPM, dRateP.x, dRateP.y, M_DEFAULT);
	}
	else if (dispType == DISP_TYPE_CALB_CAMSPEC) {	// CALB CAMSPEC
		MbufClear(theApp.clMilMain.m_mImgDisp_CALI_CAMSPEC, 0L);
		MimResize(theApp.clMilMain.m_mImg_CALI_CAMSPEC[cam_id], theApp.clMilMain.m_mImgDisp_CALI_CAMSPEC, dRateP.x, dRateP.y, M_DEFAULT);
	}
	else if (dispType == DISP_TYPE_CALB_EXPO) {	// CALB EXPO
		MbufClear(theApp.clMilMain.m_mImgDisp_CALI_STEP[0], 0L);
		MimResize(theApp.clMilMain.m_mImg_CALI_STEP[0], theApp.clMilMain.m_mImgDisp_CALI_STEP[0], dRateP.x, dRateP.y, M_DEFAULT);

		MbufClear(theApp.clMilMain.m_mImgDisp_CALI_STEP[1], 0L);
		MimResize(theApp.clMilMain.m_mImg_CALI_STEP[0], theApp.clMilMain.m_mImgDisp_CALI_STEP[1], dRateP.x, dRateP.y, M_DEFAULT);
	}
	else if (dispType == DISP_TYPE_CALB_ACCR) {	// CALB EXPO
		MbufClear(theApp.clMilMain.m_mImgDisp_ACCR, 0L);
		MimResize(theApp.clMilMain.m_mImg_ACCR[cam_id], theApp.clMilMain.m_mImgDisp_ACCR, dRateP.x, dRateP.y, M_DEFAULT);
	}
	else if (dispType == DISP_TYPE_MMPM_AUTOCENTER) {	// MMPM AUTO CENTER
		MbufClear(theApp.clMilMain.m_mImgDisp_MPMM_AutoCenter, 0L);
		MimResize(theApp.clMilMain.m_mImg_MPMM_AutoCenter, theApp.clMilMain.m_mImgDisp_MPMM_AutoCenter, dRateP.x, dRateP.y, M_DEFAULT);
	}
	else {
		return;
	}
#endif

	theApp.clMilDisp.DrawOverlayDC(false, dispType, Num);
	theApp.clMilDisp.DrawBase(dispType, Num);
	theApp.clMilDisp.DrawOverlayDC(true, dispType, Num);
}

/* desc : Mark ���� ���� - MMF (MIL Model Find File) */
API_EXPORT BOOL uvMIL_SetModelDefineMMF(UINT8 cam_id, PTCHAR name, PTCHAR mmf, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no)
{
	return theApp.clMilMain.SetModelDefineMMF(cam_id, name, mmf, m_MarkSizeP, m_MarkCenterP, mark_no);
}

/* desc : Mark ���� ���� - PAT (MIL Pattern Matching File) */
API_EXPORT BOOL uvMIL_SetModelDefinePAT(UINT8 cam_id, PTCHAR name, PTCHAR pat, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no)
{
	return theApp.clMilMain.SetModelDefinePAT(cam_id, name, pat, m_MarkSizeP, m_MarkCenterP, mark_no);
}

/* desc : Set Mark ROI */
API_EXPORT VOID uvMIL_MilSetMarkROI(UINT8 cam_id, CRect fi_rectSearhROI)
{
	theApp.clMilMain.MilSetMarkROI(cam_id, fi_rectSearhROI);
}

/* desc: pat Mark ��� */
API_EXPORT BOOL uvMIL_RegistPat(UINT8 cam_id, LPG_ACGR grab, CRect fi_rectArea, CString fi_filename, UINT8 mark_no)
{
	return theApp.clMilMain.RegistPat(cam_id, grab->grab_data, fi_rectArea, fi_filename, mark_no);
}

/* desc: mmf Mark ��� */
API_EXPORT BOOL uvMIL_RegistMod(UINT8 cam_id, LPG_ACGR grab, CRect fi_rectArea, CString fi_filename, UINT8 mark_no)
{
	return theApp.clMilMain.RegistMod(cam_id, grab->grab_data, fi_rectArea, fi_filename, mark_no);
}

/* desc: MMPM Auto Center �̹��� ��� */
API_EXPORT BOOL uvMIL_RegistMMPM_AutoCenter(CRect fi_rectArea, UINT8 cam_id, UINT8 img_id)
{
	LPG_ACGR pstGrab = NULL;

	// if (!g_pMilMain)	return FALSE;

	/* Mark ã�� ��� �� �������� */
	pstGrab = theApp.clMilMain.GetGrabbedMark(cam_id, img_id);
	if (!pstGrab)				return FALSE;
	if (!pstGrab->grab_size)	return FALSE;
	/* 256 Gray Color �̹��� ������ ��� (�˻� �ȵǴ��� ������ �̹��� ���) */
	if (pstGrab->grab_width < 1 || pstGrab->grab_height < 1 || !pstGrab->grab_data)	return TRUE;

	INT32 Grab_No = (cam_id - 1) * 2 + img_id;
	//INT32 Grab_No = (cam_id - 1) * 2 + (img_id - 1); // lk91 OnImageGrabbed() �Լ� ����... uvMIL_RegistMILGrabImg((m_u8CamID - 1) * 2 + (m_u8GrabIndex - 1), pstMark->grab_width, pstMark->grab_height, pstMark->grab_data);

	// lk91 ���⼭ �̹��� �߶� ����ϱ�...
#ifndef _NOT_USE_MIL_
	if (theApp.clMilMain.m_mImg_MPMM_AutoCenter_Proc)
		MbufClear(theApp.clMilMain.m_mImg_MPMM_AutoCenter_Proc, 0L);
	MbufCopy(theApp.clMilMain.m_mImg_Grab[Grab_No], theApp.clMilMain.m_mImg_MPMM_AutoCenter_Proc);

	if (!theApp.clMilMain.m_mImg_MPMM_AutoCenter)
		MbufFree(theApp.clMilMain.m_mImg_MPMM_AutoCenter);

	MbufChild2d(theApp.clMilMain.m_mImg_MPMM_AutoCenter_Proc, fi_rectArea.left, fi_rectArea.top,
		fi_rectArea.right - fi_rectArea.left, fi_rectArea.bottom- fi_rectArea.top, &theApp.clMilMain.m_mImg_MPMM_AutoCenter);
#endif

	return TRUE;
}

/* desc: Mark Size, Offset �ʱ�ȭ */
API_EXPORT VOID uvMIL_InitSetMarkSizeOffset(UINT8 cam_id, TCHAR* file, UINT8 fi_findType, UINT8 fi_No)
{
	theApp.clMilMain.InitSetMarkSizeOffset(cam_id, file, fi_findType, fi_No);
}

/* desc: Draw Mark Display */
API_EXPORT VOID uvMIL_PutMarkDisp(HWND hwnd, int fi_iNo, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType)
{
	theApp.clMilMain.PutMarkDisp(hwnd, fi_iNo, draw, cam_id, file, fi_findType);
}

/* desc: Draw Set Mark Display */
API_EXPORT BOOL uvMIL_PutSetMarkDisp(HWND hwnd, RECT draw, UINT8 cam_id, TCHAR* file, int fi_findType, DOUBLE fi_dRate)
{
	return theApp.clMilMain.PutSetMarkDisp(hwnd, draw, cam_id, file, fi_findType, fi_dRate);
}

/* desc: Set Mark Setting Disp Rate */
API_EXPORT DOUBLE uvMIL_SetMarkSetDispRate(DOUBLE fi_dRate)
{
	return theApp.clMilMain.SetMarkSetDispRate(fi_dRate);
}

/* desc: Get Mark Setting Disp Rate */
API_EXPORT DOUBLE uvMIL_GetMarkSetDispRate()
{
	return theApp.clMilMain.GetMarkSetDispRate();
}

/* desc: Get Mark Size */
API_EXPORT CPoint uvMIL_GetMarkSize(UINT8 cam_id, int fi_No)
{
	return theApp.clMilMain.GetMarkSize(cam_id, fi_No);
}

/* desc: Get Mark Offset */
API_EXPORT CPoint uvMIL_GetMarkOffset(UINT8 cam_id, BOOL bNewOffset, int fi_No)
{
	return theApp.clMilMain.GetMarkOffset(cam_id, bNewOffset, fi_No);
}

/* desc: Get Mask Buf */
API_EXPORT BOOL uvMIL_MaskBufGet()
{
	return theApp.clMilMain.MaskBufGet();
}

/* desc: Get Mark Find Mode */
API_EXPORT UINT8 uvMIL_GetMarkFindMode(UINT8 cam_id, UINT8 mark_no)
{
	return theApp.clMilMain.GetMarkFindMode(cam_id, mark_no);
}

/* desc: Set Mark Find Mode */
API_EXPORT BOOL uvMIL_SetMarkFindMode(UINT8 cam_id, UINT8 find_mode, UINT8 mark_no)
{
	return theApp.clMilMain.SetMarkFindMode(cam_id, find_mode, mark_no);
}

/* desc: Set Mark Size */
API_EXPORT VOID uvMIL_SetMarkSize(UINT8 cam_id, CPoint fi_MarkSize, int setMode, int fi_No)
{
	theApp.clMilMain.SetMarkSize(cam_id, fi_MarkSize, setMode, fi_No);
}

/* desc: Set Mark Offset */
API_EXPORT VOID uvMIL_SetMarkOffset(UINT8 cam_id, CPoint fi_MarkCenter, int setOffsetMode, int fi_No)
{
	theApp.clMilMain.SetMarkOffset(cam_id, fi_MarkCenter, setOffsetMode, fi_No);
}

/* desc: MMF ���Ͽ� MASK ���� ����, Mark Set������ ��� */
API_EXPORT VOID uvMIL_SaveMask_MOD(UINT8 cam_id, UINT8 mark_no)
{
	theApp.clMilMain.SaveMask_MOD(cam_id, mark_no);
}

/* desc: PAT ���Ͽ� MASK ���� ����, Mark Set������ ��� */
API_EXPORT VOID uvMIL_SaveMask_PAT(UINT8 cam_id, UINT8 mark_no)
{
	theApp.clMilMain.SaveMask_PAT(cam_id, mark_no);
}

/* desc: PAT MARK ���� */
API_EXPORT VOID uvMIL_PatMarkSave(UINT8 cam_id, CString fi_strFileName, UINT8 mark_no)
{
	theApp.clMilMain.PatMarkSave(cam_id, fi_strFileName, mark_no);
}

/* desc: MMF MARK ���� */
API_EXPORT VOID uvMIL_ModMarkSave(UINT8 cam_id, CString fi_strFileName, UINT8 mark_no)
{
	theApp.clMilMain.ModMarkSave(cam_id, fi_strFileName, mark_no);
}

/* desc: Clear Mask(MMF) */
API_EXPORT VOID uvMIL_MaskClear_MOD(UINT8 cam_id, CPoint fi_iSizeP, UINT8 mark_no)
{
	theApp.clMilMain.MaskClear_MOD(cam_id, fi_iSizeP, mark_no);
}

/* desc: Clear Mask(PAT) */
API_EXPORT VOID uvMIL_MaskClear_PAT(UINT8 cam_id, CPoint fi_iSizeP, UINT8 mark_no)
{
	theApp.clMilMain.MaskClear_PAT(cam_id, fi_iSizeP, mark_no);
}

/* desc: Find Center (Mark Set������ ���) */
API_EXPORT VOID uvMIL_CenterFind(int cam_id, int fi_length, int fi_curSmoothness, double* fi_NumEdgeMIN_X, double* fi_NumEdgeMAX_X, double* fi_NumEdgeMIN_Y, double* fi_NumEdgeMAX_Y, int* fi_NumEdgeFound, int fi_Mode)
{
	theApp.clMilMain.CenterFind(cam_id, fi_length, fi_curSmoothness, fi_NumEdgeMIN_X, fi_NumEdgeMAX_X, fi_NumEdgeMIN_Y, fi_NumEdgeMAX_Y, fi_NumEdgeFound, fi_Mode);
}

/* desc: Mil Main �Ҵ� ���� ���� */
API_EXPORT VOID uvMIL_CloseSetMark()
{
	theApp.clMilMain.CloseSetMark();
}

/* desc: Mask �ʱ�ȭ */
API_EXPORT VOID uvMIL_InitMask(UINT8 cam_id)
{
	theApp.clMilMain.InitMask(cam_id);
}

/* desc: MARK DISP ID �Ҵ� */
API_EXPORT VOID uvMIL_SetDispMark(CWnd *pWnd)
{
	theApp.clMilMain.SetDispMark(pWnd);
}

/* desc: MARK DISP ID �Ҵ� */
API_EXPORT VOID uvMIL_SetDispRecipeMark(CWnd* pWnd[2])
{
	theApp.clMilMain.SetDispRecipeMark(pWnd);
}

API_EXPORT VOID uvMIL_SetDispExpo(CWnd* pWnd[4])
{
	theApp.clMilMain.SetDispExpo(pWnd);
}
/* desc: Mark ���� �׸� �� ����ϴ� MIL �Լ� */
API_EXPORT VOID uvMIL_DrawMarkInfo_UseMIL(UINT8 cam_id, UINT8 fi_smooth, UINT8 mark_no)
{
	theApp.clMilMain.DrawMarkInfo_UseMIL(cam_id, fi_smooth, mark_no);
}

/* desc: Set Mask */
API_EXPORT VOID uvMIL_Mask_MarkSet(UINT8 cam_id, CRect rectTmp, CPoint iTmpSizeP, CRect rectFill, int fi_color, bool bMask)
{
	theApp.clMilMain.Mask_MarkSet(cam_id, rectTmp, iTmpSizeP, rectFill, fi_color, bMask);
}

/*
	desc : Overlay ���� �Լ� - ��ü Overlay Clear or DC ���
	fi_bDrawFlag : FALSE - CLEAR
	fi_iDispType : 0:Expo, 1:mark, 2 : Live, 3 : mark set
	fi_iNo : Cam Num Ȥ�� Grab Mark Num (���� Disp Type �� ���缭 ���)
*/
API_EXPORT VOID uvMIL_DrawOverlayDC(bool fi_bDrawFlag, int fi_iDispType, int fi_iNo)
{
	theApp.clMilDisp.DrawOverlayDC(fi_bDrawFlag, fi_iDispType, fi_iNo);
}


API_EXPORT VOID uvMIL_Camera_ClearShapes(int fi_iDispType)
{
	theApp.clMilDisp.ClearShapes(fi_iDispType);
}



/* desc : Overlay ���� �Լ� - Box List �߰� */
API_EXPORT VOID uvMIL_OverlayAddBoxList(int fi_iDispType, int fi_iNo, int fi_iLeft, int fi_iTop, int fi_iRight, int fi_iBottom, int fi_iStyle, int fi_color)
{
	theApp.clMilDisp.AddBoxList(fi_iDispType, fi_iNo, fi_iLeft, fi_iTop, fi_iRight, fi_iBottom, fi_iStyle, fi_color);
}

/* desc : Overlay ���� �Լ� - Cross List �߰� */
API_EXPORT VOID uvMIL_OverlayAddCrossList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, int fi_iWdt1, int fi_iWdt2, int fi_color)
{
	theApp.clMilDisp.AddCrossList(fi_iDispType, fi_iNo, fi_iX, fi_iY, fi_iWdt1, fi_iWdt2, fi_color);
}

/* desc : Overlay ���� �Լ� - Text List �߰� */
API_EXPORT VOID uvMIL_OverlayAddTextList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, CString fi_sText, int fi_color, int fi_iSizeX, int fi_iSizeY, CString fi_sFont, bool fi_bEgdeFlag)
{
	theApp.clMilDisp.AddTextList(fi_iDispType, fi_iNo, fi_iX, fi_iY, fi_sText, fi_color, fi_iSizeX, fi_iSizeY, fi_sFont, fi_bEgdeFlag);
}

/* desc : Overlay ���� �Լ� - Line List �߰� */
API_EXPORT VOID uvMIL_OverlayAddLineList(int fi_iDispType, int fi_iNo, int fi_iSx, int fi_iSy, int fi_iEx, int fi_iEy, int fi_iStyle, int fi_color)
{
	theApp.clMilDisp.AddLineList(fi_iDispType, fi_iNo, fi_iSx, fi_iSy, fi_iEx, fi_iEy, fi_iStyle, fi_color);
}

/* desc: MARK SET DISP ID �Ҵ� */
API_EXPORT VOID uvMIL_SetDispMarkSet(CWnd* pWnd)
{
	theApp.clMilMain.SetDispMarkSet(pWnd);
}

/* desc: MMPM AutoCenter DISP ID �Ҵ� */
API_EXPORT VOID uvMIL_SetDispMMPM_AutoCenter(CWnd* pWnd)
{
	theApp.clMilMain.SetDispMMPM_AutoCenter(pWnd);
}


/* desc: LIVE DISP ID �Ҵ� */
API_EXPORT VOID uvMIL_SetDisp(CWnd** pWnd, UINT8 fi_Mode)
{
	theApp.clMilMain.SetDisp(pWnd, fi_Mode);
}

/* desc: MMPM DISP ID �Ҵ� */
API_EXPORT VOID uvMIL_SetDispMMPM(CWnd* pWnd)
{
	theApp.clMilMain.SetDispMMPM(pWnd);
}


/* desc: MIL Img ID ��� */
API_EXPORT BOOL uvMIL_RegistMILImg(INT32 cam_id, INT32 width, INT32 height, PUINT8 image) // grab�� �̹����� mil �� ���� uvMIL_BufPutMILImg �� �̸� ������ ��
{
	return theApp.clMilMain.RegistMILImg(cam_id, width, height, image);
}

/* desc: MIL Img ID ��� */
API_EXPORT BOOL uvMIL_RegistMILGrabImg(INT32 fi_No, INT32 width, INT32 height, PUINT8 image) // grab�� �̹����� mil �� ���� uvMIL_BufPutMILImg �� �̸� ������ ��
{
	return theApp.clMilMain.RegistMILGrabImg(fi_No, width, height, image);
}


/* desc : Zoom ���� �Լ� - Zoom In */
API_EXPORT VOID uvMIL_MilZoomIn(int fi_iDispType, int cam_id, CRect rc)
{
	theApp.clMilDisp.MilZoomIn(fi_iDispType, cam_id - 1, rc);
}

/* desc : Zoom ���� �Լ� - Zoom Out */
API_EXPORT BOOL uvMIL_MilZoomOut(int fi_iDispType, int cam_id, CRect rc)
{
	return theApp.clMilDisp.MilZoomOut(fi_iDispType, cam_id - 1, rc);
}

/* desc : Zoom ���� �Լ� - Zoom Fit, �̹��� ���� 1.0 */
API_EXPORT VOID uvMIL_MilAutoScale(int fi_iDispType, int cam_id)
{
	theApp.clMilDisp.MilAutoScale(fi_iDispType, cam_id - 1);
}

/* desc : Zoom ���� �Լ� - Mouse Point ����, Zoom���� Ȯ��Ǿ��� �� �̹��� �̵��ϱ� ���� */
API_EXPORT VOID uvMIL_SetZoomDownP(int fi_iDispType, int cam_id, CPoint fi_point)
{
	theApp.clMilDisp.SetZoomDownP(fi_iDispType, cam_id - 1, fi_point);
}

/* desc : Zoom ���� �Լ� - �̹��� �̵� */
API_EXPORT VOID uvMIL_MoveZoomDisp(int fi_iDispType, int cam_id, CPoint fi_point, CRect fi_rect)
{
	theApp.clMilDisp.MoveZoomDisp(fi_iDispType, cam_id - 1, fi_point, fi_rect);
}

/* desc : Zoom ���� �Լ� - Zoom �� ���󿡼� Ŭ���� ��ǥ�� �̹��� ��ǥ�� ��ȯ�����ִ� �۾� */
API_EXPORT CDPoint uvMIL_trZoomPoint(int fi_iDispType, int cam_id, CPoint fi_point)
{
	return theApp.clMilDisp.trZoomPoint(fi_iDispType, cam_id - 1, fi_point);
}

API_EXPORT BOOL uvMIL_SetModelDefine_tot(UINT8 cam_id, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth,
	LPG_CMPV model, UINT8 fi_No, TCHAR* file,
	DOUBLE scale_min, DOUBLE scale_max,
	DOUBLE score_min, DOUBLE score_tgt)
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.SetModelDefine_tot(cam_id, speed, level, count, smooth, model, fi_No, file,
		scale_min, scale_max, score_min, score_tgt);
}

API_EXPORT BOOL uvMIL_MergeMark(UINT8 cam_id, LPG_CMPV value, UINT8 speed, UINT8 level, UINT8 count, DOUBLE smooth, UINT8 mark_no, TCHAR* file1, TCHAR* file2,
	TCHAR* RecipeName, DOUBLE scale_min, DOUBLE scale_max,
	DOUBLE score_min, DOUBLE score_tgt)
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.MergeMark(cam_id, value, speed, level, count, smooth, mark_no, file1, file2,
		RecipeName, scale_min, scale_max, score_min, score_tgt);

}

API_EXPORT BOOL uvMIL_ProcImage(UINT8 cam_id, UINT8 imgProc)
{
	return theApp.clMilMain.ProcImage(cam_id - 1, imgProc);
}

API_EXPORT BOOL uvMIL_RunModel_VisionCalib(UINT8 cam_id, UINT8 img_id, PUINT8 image, UINT8 dlg_id, UINT8 mark_no, int* roi_left, int* roi_right, int* roi_top, int* roi_bottom, int row, int col)
{
	BOOL bSucc = FALSE;

	if (cam_id < 1 || cam_id > MAX_ALIGN_CAMERA)	return FALSE;
	if (!image)	return FALSE;
	bSucc = theApp.clMilMain.RunModel_VisionCalib(cam_id, img_id, image, dlg_id, mark_no, roi_left, roi_right, roi_top, roi_bottom, row, col);
	return bSucc;
}

#ifdef __cplusplus
}
#endif