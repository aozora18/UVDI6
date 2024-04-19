
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
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

/* Grabbed Image Size */
UINT32						g_u32GrabWidth		= 0;
UINT32						g_u32GrabHeight		= 0;

/* 멀티 마크 즉, Grabbed Image 내에서 여러 개의 마크를 검색하는 경우, 마크가 표현된 가로/세로 영역 (단위: um) */
DOUBLE						g_dbMuliAreaWidth	= 0.0f;	/* pixel */
DOUBLE						g_dbMuliAreaHeight	= 0.0f;	/* pixel */

PUINT8						g_pGrabBuff			= NULL;		/* The buffer of image for temporary storage */

LPBITMAPINFO				g_pBmpInfo			= NULL;

//CMilMain					*g_pMilMain			= NULL;

AlignMotion* alignMotionPtr = nullptr;

/* ----------------------------------------------------------------------------------------- */
/*                                 lk91 VISION 추가 함수                                     */
/* ----------------------------------------------------------------------------------------- */
/*
 desc : Drawing - Align(검색된 Mark) Image (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		grab	- [in]  Grabbed Image 정보가 저장된 구조체 포인터
		find	- [in]  마크 검색 성공 여부 (검색됐더라도, 검색 결과 값에 따라 달라짐)
						0x00 - 검색 결과 실패, 0x01 - 검색 결과 성공
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

	///* 수집된 데이터가 없는 경우인지 */
	//if (grab->grab_width < 1 || grab->grab_height < 1 || !grab->grab_data)	return;
	///* 가로와 세로의 이미지 크기 비교 후, 많이 차이나는 부분을 기준으로 설정 */
	//if (!ResizeDrawBitmap(rDraw, szSrc, szTgt, dbRate))	return;
	//if (FLT_MAX == dbRate)	return;
	///* 이미지 출력 시작 위치 계산 */
	//ptMove.x = (LONG)ROUNDED(DOUBLE((rDraw.right - rDraw.left) - szTgt.cx) / 2.0f, 0) + rDraw.left;
	//ptMove.y = (LONG)ROUNDED(DOUBLE((rDraw.bottom - rDraw.top) - szTgt.cy) / 2.0f, 0) + rDraw.top;

	/* Grabbed Image 출력 */
	/* Memory DC 영역에 Live Image 출력 그리기 */
	CDPoint dRateP;
#ifndef _NOT_USE_MIL_
	dRateP = theApp.clMilDisp.GetRateDispToBuf(DISP_TYPE_MMPM);
	MbufClear(theApp.clMilMain.m_mImgDisp_MMPM, 0L);
	MimResize(theApp.clMilMain.m_mImg_Grab[Grab_No], theApp.clMilMain.m_mImgDisp_MMPM, dRateP.x, dRateP.y, M_DEFAULT);

	theApp.clMilDisp.DrawOverlayDC(false, DISP_TYPE_MMPM, 0);
	theApp.clMilDisp.DrawBase(DISP_TYPE_MMPM, 0);
	//theApp.clMilDisp.DrawOverlayDC(true, 4, 0);

	//DrawGrabImage(hdc, grab, draw, &szTgt, &ptMove); 
	/* Grabbed Image의 중심에 기호 (전체 라인) ('+') 출력 */
	// //DrawGrabImageCent(hdc, grab, dbRate, &ptMove);
	//DrawGrabImageCent(hdc, draw); 
	if (ENG_MMSM::en_cent_side == theApp.clMilMain.GetMarkMethod(grab->cam_id))
	{
		//DrawGrabMarkDiffText(hdc, grab, &ptMove);
		CString sTmp;
		sTmp.Format(_T("(x:%+.4f y:%+.4f um)"), grab->move_mm_x, grab->move_mm_y);
		theApp.clMilDisp.AddTextList(4, 0, 50, 50, sTmp, eM_COLOR_RED, 10, 25, VISION_FONT_TEXT, true);
	}

	/* 총 검색된 Mark에 대한 정보 표현 (출력) */
	u8Find = theApp.clMilMain.GetGrabbedMarkCount(grab->cam_id, grab->img_id);
	pstFind = theApp.clMilMain.GetGrabbedMarkAll(grab->cam_id, grab->img_id);
	for (i = 0; i < u8Find; i++)
	{
		/* Grabbed Image에서 Mark의 중심 위치에 기호 ('+') 출력 */
		//DrawGrabMarkArea(hdc, grab, &pstFind[i], dbRate, &ptMove, find);
		theApp.clMilDisp.AddCrossList(DISP_TYPE_MMPM, 0, (int)pstFind[i].cent_x, (int)pstFind[i].cent_y, 20, 20, eM_COLOR_GREEN);
	}

	theApp.clMilDisp.DrawOverlayDC(true, DISP_TYPE_MMPM, 0);
#endif
}


/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Grabbed Image 변경에 따른, 출력 관련 메모리 및 비트맵 설정 변경
 parm : grap_width	- [in]  Grabbed Image Size (Width)
		grap_height	- [in]  Grabbed Image Size (Height)
		bit_count	- [in]  bit per counts
 retn : TRUE or FALSE
*/
BOOL ReAllocGrabImage(UINT32 width, UINT32 height, UINT8 bit_count)
{
	UINT32 i, u32ColorSize	= 0;

	if (g_pBmpInfo)	delete [] g_pBmpInfo;

	/* 메모리 할당 */
	if (bit_count <= 8)	u32ColorSize	= UINT32(1 << bit_count) * (UINT32)sizeof(RGBQUAD);
	g_pBmpInfo	= (LPBITMAPINFO)new UINT8 [sizeof(BITMAPINFOHEADER) + u32ColorSize];

	/* Bitmap Header 설정 */
	g_pBmpInfo->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	g_pBmpInfo->bmiHeader.biWidth			= width; //이미지 가로 크기를 적음.  
	g_pBmpInfo->bmiHeader.biHeight			= -1 /* image invert */ * height;
	g_pBmpInfo->bmiHeader.biPlanes			= 1;
	g_pBmpInfo->bmiHeader.biBitCount		= bit_count;
	g_pBmpInfo->bmiHeader.biCompression		= BI_RGB;
	g_pBmpInfo->bmiHeader.biXPelsPerMeter	= 0;
	g_pBmpInfo->bmiHeader.biYPelsPerMeter	= 0;
	g_pBmpInfo->bmiHeader.biClrUsed			= (bit_count <= 8) ? (1 << bit_count): 0;
	g_pBmpInfo->bmiHeader.biClrImportant	= (bit_count <= 8) ? (1 << bit_count) : 0;
	g_pBmpInfo->bmiHeader.biSizeImage		= height * WIDTHBYTES(bit_count, width);

	/*     Palette 설정		*/
	/* 1 Bit = 2^0 =   1 색	*/
	/* 2 Bit = 2^2 =   4 색	*/
	/* 4 Bit = 2^4 =  16 색	*/
	/* 8 Bit = 2^8 = 256 색	*/
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
 desc : 이미지의 실제 크기를 출력 대상의 윈도 영역에 맞게 그리기 위한 이미지 크기 및 비율 값 반환
 parm : draw_area	- [in]  이미지가 출력되는 윈도의 영역 (좌표)
		src_size	- [in]  원본 이미지의 크기 (단위: Pixel)
		tgt_size	- [out] 윈도 영역에 출력되는 이미지의 크기 (단위: pxiel)
		tgt_rate	- [out] 원본 이미지 대비 출력 윈도의 변경 크기 비율 값
							원본 이미지 크기를 1 값으로 볼때, 반환된 값이 1.0 보다 크면 확대, 작으면 축소
 retn : TRUE or FALSE
*/
BOOL ResizeDrawBitmap(RECT draw_area, CSize src_size, CSize &tgt_size, DOUBLE &tgt_rate)
{
	CSize szDraw(draw_area.right-draw_area.left, draw_area.bottom-draw_area.top);

	/* 반드시 1.0f 값으로 초기화 */
	tgt_rate	= 1.0f;

	/* 가로 크기 비율이 세로 크기 비율보다 큰 경우 */
	if ((DOUBLE(szDraw.cx) / DOUBLE(src_size.cx)) > (DOUBLE(szDraw.cy) / DOUBLE(src_size.cy)))
	{
		/* <세로> 비율 기준으로 영상 확대 or 축소 진행 */
		tgt_rate	= DOUBLE(szDraw.cy) / DOUBLE(src_size.cy);
	}
	/* 세로 크기 비율이 가로 크기 비율보다 큰 경우 */
	else
	{
		/* <가로> 비율 기준으로 영상 확대 or 축소 진행 */
		tgt_rate	= DOUBLE(szDraw.cx) / DOUBLE(src_size.cx);
	}
	/* 윈도 이미지 영역에 맞게 영상 이미지 크기 조정 */
	tgt_size.cx	= (LONG)ROUNDED(tgt_rate * src_size.cx, 0);
	tgt_size.cy	= (LONG)ROUNDED(tgt_rate * src_size.cy, 0);

	/* 기존 Grabbed Image의 크기가 다른 경우, 메모리 다시 할당 */
	if (g_u32GrabWidth != src_size.cx || g_u32GrabHeight != src_size.cy)
	{
		if (!ReAllocGrabImage(src_size.cx, src_size.cy, 8))	return	FALSE;

		/* 이전에 생성된 이미지의 크기 저장 */
		g_u32GrabWidth	= src_size.cx;
		g_u32GrabHeight	= src_size.cy;
	}

	return TRUE;
}

/*
 desc : 검색된 Mark의 영역에 대한 이미지 크기 재조정
 parm : in_size		- [in]  검색된 마크 이미지 크기 (Pixel)
		in_cent		- [in]  검색된 마크 중심 위치 (Pixel)
		resize_rate	- [in]  축소 혹은 확대 비율
		out_cent	- [out] 최종 그려질 Mark 중심 좌표
		out_line	- [out] 최종 그려질 Mark 4 군데 좌표
 retn : None
*/
VOID GetDrawMarkArea(LPG_DBWH in_size, LPG_DBXY in_cent, DOUBLE resize_rate,
					 POINT &out_cent, PPOINT out_line)
{
	STG_DBXY stCent;
	STG_DBWH stSize;
	DOUBLE dbLineX[4], dbLineY[4];

	/* 이미지가 출력될 영역의 크기 (비율)에 따라 Mark 영역이 그렬질 크기 결정 */
	stSize.cx	= in_size->cx * resize_rate;
	stSize.cy	= in_size->cy * resize_rate;
	/* 검색된 마크의 중심 위치 재계산 */
	stCent.x	= in_cent->x * resize_rate;
	stCent.y	= in_cent->y * resize_rate;
	/* 마크의 축소 or 확대에 따른 위치 설정 */
	dbLineX[0]	= stCent.x - stSize.cx / 2.0f;
	dbLineY[0]	= stCent.y - stSize.cy / 2.0f;
	dbLineX[1]	= dbLineX[0]+ stSize.cx;
	dbLineY[1]	= dbLineY[0];
	dbLineX[2]	= dbLineX[0]+ stSize.cx;
	dbLineY[2]	= dbLineY[0]+ stSize.cy;
	dbLineX[3]	= dbLineX[0];
	dbLineY[3]	= dbLineY[0]+ stSize.cy;

	/* 값 반환 */
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
 desc : 단순 Grabbed Image 출력
 parm : hdc		- [in]  DC 객체 핸들
		grab	- [in]  Grabbed Image 정보가 저장된 구조체 포인터
		area	- [in]  Memory DC 영역에 출력하려는 크기
		resize	- [in]  실제 출력되는 이미지 크기 영역
		move	- [in]  실제 출력되는 이미지 시작 위치
 retn : None
*/
VOID DrawGrabImage(HDC hdc, LPG_ACGR grab, RECT area, CSize *resize, POINT *move, UINT8 flipFlag=0xff)
{
	DOUBLE dbAngle	= 0.0f;
	/* Memory DC 영역에 Grabbed Image 출력 및 검색된 결과 값 그리기 */
	HDC hMemDC;
	HBITMAP hMemBmp, hMemBmpOld;

	/* 메모리 DC와 비트맵 생성하고 연결 시켜 주기 */
	hMemDC	= ::CreateCompatibleDC(hdc);
	hMemBmp	= ::CreateCompatibleBitmap(hdc, grab->grab_width, grab->grab_height);
	if (!hMemBmp)
	{
		if (hMemDC)	::DeleteDC(hMemDC);
		return;
	}

	/* Memory DC와 Bitmap 간의 연결 */
	hMemBmpOld	= (HBITMAP)::SelectObject(hMemDC, hMemBmp);
	if (hMemBmpOld)
	{
		/* Bitmap Memory 영역에 Grabbed Image 출력 (회전 여부에 따라) */
		if (0 == g_pstConfig->set_cams.acam_inst_angle/* 0 degree */)
		{
			::StretchDIBits(hMemDC, 0, 0, grab->grab_width, grab->grab_height,
							0, 0, grab->grab_width, grab->grab_height, 
							grab->grab_data, g_pBmpInfo, DIB_RGB_COLORS, SRCCOPY);
			/* 화면 DC에 출력 */
			::SetStretchBltMode(hdc, COLORONCOLOR);	/* 이미지 축소할 때, 최소화하기 위한 모드 선택 */
			::StretchBlt(hdc, move->x, move->y, resize->cx, resize->cy,
						 hMemDC, 0, 0, grab->grab_width, grab->grab_height, SRCCOPY);
		}
		else	/* 180 degree 회전을 했으므로 */
		{
			/* 기존 버퍼 메모리 초기화 */
			g_pGrabBuff[grab->grab_size]	= 0x00;	/* 덮어쓰므로, 맨 끝 부분만 ^^ */
			/* 이미지 회전 진행 */
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
			/* Memory DC 영역에 DIB로 출력 */
			::StretchDIBits(hMemDC, 0, 0, grab->grab_width, grab->grab_height,
							0, 0, grab->grab_width, grab->grab_height, 
							g_pGrabBuff, g_pBmpInfo, DIB_RGB_COLORS, SRCCOPY);
			/* 화면 DC에 출력 */
			::SetStretchBltMode(hdc, COLORONCOLOR);	/* 이미지 축소할 때, 최소화하기 위한 모드 선택 */
			::StretchBlt(hdc, move->x, move->y, resize->cx, resize->cy,
						 hMemDC, 0, 0, grab->grab_width, grab->grab_height, SRCCOPY);
		}
		/* Bitmap Memory 복원 */
		if (hMemBmpOld)	::SelectObject(hMemDC, hMemBmpOld);
	}

	/* Memory DC & Bitmap 해제 */
	if (hMemBmp)	::DeleteObject(hMemBmp);
	if (hMemDC)		::DeleteDC(hMemDC);
}

/*
 desc : Grabbed Image에서 Mark의 오차 값을 문자열로 출력
 parm : hdc		- [in]  DC 객체 핸들
		grab	- [in]  Grabbed Image 정보가 저장된 구조체 포인터
		move	- [in]  문자열 (오차)이 출력되는 위치 (단위: Pixel)
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

	/* 결과 데이터의 성공 여부 */
	bMultiMark	= (ENG_MMSM)grab->mark_method != ENG_MMSM::en_single;
	bFind = grab->IsMarkValid(bMultiMark);
	/* 폰트 정보 얻기 */
	if (!uvCmn_GetLogFont(lfText, L"D2Coding", 13, DEFAULT_CHARSET))	return;
	ftText	= ::CreateFontIndirect(&lfText);
	if (!ftText)	return;
	/* 배경 투명하게 */
	i32BkMode	= ::SetBkMode(hdc, TRANSPARENT);
	/* 사용할 폰트 적용*/
	ftFont	= (HFONT)::SelectObject(hdc, ftText);
	if (ftFont)
	{
		/* 사용할 텍스트 색상 적용 */
		clrPrev	= ::SetTextColor(hdc, clrText[bFind]);
		/* 텍스트 출력 */
		swprintf_s(tzDiff, 128, L"(x:%+.4f y:%+.4f um)", grab->move_mm_x, grab->move_mm_y);
		::TextOut(hdc, 15, move->y+1, tzDiff, (INT32)_tcslen(tzDiff));
		/* 텍스트 복구 */
		::SetTextColor(hdc, clrPrev);
		/* 폰트 복구*/
		::SelectObject(hdc, ftFont);
	}
	/* 배경 모드 복구 */
	::SetBkMode(hdc, i32BkMode);
	/* 폰트 객체 제거 */
	::DeleteObject(ftText);
}

/*
 desc : Grabbed Image에서 Mark 영역 표시 (출력)
 parm : hdc			- [in]  DC 객체 핸들
		s_grab		- [in]  Grabbed Image 정보가 저장된 구조체 포인터 (원본)
		m_grab		- [in]  Grabbed Image 정보가 저장된 구조체 포인터 (검색된 마크의 세부 정보가 저장)
		resize_rate	- [in]  축소 혹은 확대 비율
		move		- [in]  실제 출력되는 이미지 시작 위치
		find		- [in]  마크 검색 성공 여부 (검색됐더라도, 검색 결과 값에 따라 달라짐)
							0x00 - 검색 결과 실패, 0x01 - 검색 결과 성공
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

	/* 결과 데이터가 있는지 여부 */
	if (m_grab->IsEmptyData())	return;
	/* Grabbed Image 180도 회전 했으므로 */
	if (g_pstConfig->set_cams.acam_inst_angle)
	{
		/* Grabbed Image의 중심에서 회전했을 경우, 현재 마크의 중심 좌표 값 반환 */
		uvCmn_RotateCoord(DOUBLE(s_grab->grab_width/2.0f), DOUBLE(s_grab->grab_height/2.0f),
						  s_grab->mark_cent_px_x, s_grab->mark_cent_px_y,
						  DOUBLE(-dbAngle/*g_pstConfig->set_cams.acam_inst_angle*/),	/* 항상 회전 각도의 - (음수) 값을 붙여줘야 됨 (왜냐하면, Grabbed Image 즉, MIL의 ImgRotate 함수 회전이 시계 반대 방향으로 하기 때문임 */
						  m_grab->cent_x, m_grab->cent_y);
	}

	/* 출력되는 윈도 영역에 따라, Grabbed Image의 축소 or 확대에 따른 찾은 Mark 도형 크기 축소 or 확대 */
	stInSize.cx	= m_grab->mark_width;
	stInSize.cy	= m_grab->mark_height;
	stInCent.x	= m_grab->cent_x;
	stInCent.y	= m_grab->cent_y;
	GetDrawMarkArea(&stInSize, &stInCent, resize_rate, ptCent, ptLine);

	/* --------------------------------------------- */
	/* Grabbed Image에서 검색된 영역의 사각형 그리기 */
	/* --------------------------------------------- */
	hPen	= ::CreatePen(PS_SOLID, 1, crLine[find]);
	if (hPen)
	{
		hOldPen	= (HPEN)::SelectObject(hdc, hPen);
		if (hOldPen)
		{
			/* Mark Lines (Rectangle) 출력 */
			::MoveToEx(hdc,	ptLine[0].x+move->x,	ptLine[0].y+move->y, NULL);
			::LineTo(hdc,	ptLine[1].x+move->x,	ptLine[1].y+move->y);
			::LineTo(hdc,	ptLine[2].x+move->x,	ptLine[2].y+move->y);
			::LineTo(hdc,	ptLine[3].x+move->x,	ptLine[3].y+move->y);
			::LineTo(hdc,	ptLine[0].x+move->x,	ptLine[0].y+move->y);

			/* Mark Center (+) 출력 */
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
	/* 만약 Multi-Mark인 경우, 멀티 마크를 포함하고 있는 사각형 영역 그리기 */
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
 desc : Grabbed Image의 중심에 '+' 표시 (출력)
 parm : hdc			- [in]  DC 객체 핸들
		grab		- [in]  Grabbed Image 정보가 저장된 구조체 포인터
		resize_rate	- [in]  축소 혹은 확대 비율
		move		- [in]  실제 출력되는 이미지 시작 위치
 retn : None
*/
#if 0
VOID DrawGrabImageCent(HDC hdc, LPG_ACGR grab, DOUBLE resize_rate, POINT *move)
{
	HPEN hPen, hOldPen;
	POINT ptCent;

	/* ----------------------------------------------- */
	/* Grabbed Image에서 중심이 되는 Center (+) 그리기 */
	/* ----------------------------------------------- */
	hPen	= ::CreatePen(PS_SOLID, 1, RGB(255, 127, 39));
	if (hPen)
	{
		hOldPen	= (HPEN)::SelectObject(hdc, hPen);
		if (hOldPen)
		{
			ptCent.x = (UINT32)ROUNDED((grab->grab_width / 2.0f) * resize_rate, 0);
			ptCent.y = (UINT32)ROUNDED((grab->grab_height / 2.0f) * resize_rate, 0);
			/* Mark Center (+) 출력 */
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
	/* Grabbed Image에서 중심이 되는 Center (+) 그리기 */
	/* ----------------------------------------------- */
	hPen	= ::CreatePen(PS_DOT, 1, RGB(255, 127, 39));
	if (hPen)
	{
		hOldPen	= (HPEN)::SelectObject(hdc, hPen);
		if (hOldPen)
		{
			/* Mark Center (+) 출력 */
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
 desc : Grabbed Image에서 Mark 중심 위치에 '+' 표시 (출력)
 parm : hdc			- [in]  DC 객체 핸들
		grab		- [in]  Grabbed Image 정보가 저장된 구조체 포인터
		resize_rate	- [in]  축소 혹은 확대 비율
		move		- [in]  실제 출력되는 이미지 시작 위치
 retn : None
*/
VOID DrawGrabMarkCent(HDC hdc, LPG_ACGR grab, DOUBLE resize_rate, POINT *move)
{
	HPEN hPen, hOldPen;
	POINT ptDiff;

	/* ----------------------------------------------- */
	/* Grabbed Image에서 중심이 되는 Center (+) 그리기 */
	/* ----------------------------------------------- */
	hPen	= ::CreatePen(PS_SOLID, 1, RGB(255, 127, 39));
	if (hPen)
	{
		hOldPen	= (HPEN)::SelectObject(hdc, hPen);
		if (hOldPen)
		{
			ptDiff.x = (UINT32)ROUNDED((grab->mark_cent_px_x) * resize_rate, 0);
			ptDiff.y = (UINT32)ROUNDED((grab->mark_cent_px_y) * resize_rate, 0);
			/* Mark Center (+) 출력 */
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
 desc : Drawing - Align(검색된 Mark) Image (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		grab	- [in]  Grabbed Image 정보가 저장된 구조체 포인터
		find	- [in]  마크 검색 성공 여부 (검색됐더라도, 검색 결과 값에 따라 달라짐)
						0x00 - 검색 결과 실패, 0x01 - 검색 결과 성공
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
	/* 출력 영역 조절 */
	draw.left++;
	draw.top++;
	draw.right--;
	draw.bottom--;
#endif
	/* 수집된 데이터가 없는 경우인지 */
	if (grab->grab_width < 1 || grab->grab_height < 1 || !grab->grab_data)	return;
	/* 가로와 세로의 이미지 크기 비교 후, 많이 차이나는 부분을 기준으로 설정 */
	if (!ResizeDrawBitmap(rDraw, szSrc, szTgt, dbRate))	return;
	if (FLT_MAX == dbRate)	return;
	/* 이미지 출력 시작 위치 계산 */
	ptMove.x= (LONG)ROUNDED(DOUBLE((rDraw.right - rDraw.left) - szTgt.cx) / 2.0f, 0) + rDraw.left;
	ptMove.y= (LONG)ROUNDED(DOUBLE((rDraw.bottom- rDraw.top) - szTgt.cy) / 2.0f, 0) + rDraw.top;

	/* Grabbed Image 출력 */
	DrawGrabImage(hdc, grab, draw, &szTgt, &ptMove, flipFlag);
	/* Grabbed Image의 중심에 기호 (전체 라인) ('+') 출력 */
//	DrawGrabImageCent(hdc, grab, dbRate, &ptMove);
	DrawGrabImageCent(hdc, draw); 
	if (ENG_MMSM::en_cent_side == theApp.clMilMain.GetMarkMethod(grab->cam_id))
	{
		DrawGrabMarkDiffText(hdc, grab, &ptMove);
	}

	/* 총 검색된 Mark에 대한 정보 표현 (출력) */
	u8Find = theApp.clMilMain.GetGrabbedMarkCount(grab->cam_id, grab->img_id);
	pstFind= theApp.clMilMain.GetGrabbedMarkAll(grab->cam_id, grab->img_id);
	for (i=0; i<u8Find; i++)
	{
		/* Grabbed Image에서 Mark의 중심 위치에 기호 ('+') 출력 */
		DrawGrabMarkArea(hdc, grab, &pstFind[i], dbRate, &ptMove, find);
	}
}

/* --------------------------------------------------------------------------------------------- */
/*                                           외부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*
 desc : BIPS Server와 통신을 담당하는 Client 스레드 생성 및 실행
 parm : config		- [in]  환경 정보
		shmem		- [in]  Shared Memory
		work_dir	- [in]  작업 경로 (실행 파일 경로)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_Init(LPG_CIEA config, LPG_VDSM shmem, PTCHAR work_dir)
{
	g_pstConfig	= config;

	/* MIL Main Object Memory 생성 */
	//g_pMilMain	= new CMilMain(config, shmem);
	//ASSERT(g_pMilMain);
	theApp.clMilMain.InitMilMain(config, shmem);

	/* Calb ROI 설정 */
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


	/* 작업 경로 저장 */
	swprintf_s(g_tzWorkDir, MAX_PATH_LEN, L"%s", work_dir);
	/* 카메라 최대 해상도 크기만큼 임시 버퍼 할당 (이미지 회전 임시 버퍼) */
	UINT32 u32Size	= config->set_cams.GetCamSizeBytes();
	/* Live 출력할 때, 회전 후 저장을 위한 임시 버퍼 할당 */
	g_pGrabBuff = new UINT8[u32Size + 1];//  (PUINT8)::Alloc(u32Size + 1);
	g_pGrabBuff[u32Size]	= 0x00;

	return TRUE;
}

/*
 desc : 라이브러리 해제
 parm : None
 retn : None
*/
API_EXPORT VOID uvMIL_Close()
{
	/* MIL Main Object Memory 해제 */
	//if (g_pMilMain)	delete g_pMilMain;
	//g_pMilMain	= NULL;

	/* Grabbed Image 출력 관련 메모리 해제 */
	if (g_pBmpInfo)	delete [] g_pBmpInfo;
	g_pBmpInfo	= NULL;
	/* 메모리 해제와 함께 크기 값 초기화 필요 */
	g_u32GrabWidth	= 0;
	g_u32GrabHeight	= 0;
	/* 임시 회전 후 저장용 버퍼 메모리 해제 */
	if (g_pGrabBuff)	
		delete[] g_pGrabBuff;

	g_pGrabBuff	= NULL;
	/* 반드시 해줘야 됨 */
	g_pstConfig	= NULL;


	// lk91 여기서 해제 해보기
	theApp.clMilMain.CloseMilAlloc();
}

/*
 desc : 가장 최근의 Grabbed Image 결과 초기화
		새로운 Align 작업을 수행한다면, Align 수행하기 전에 무조건 한번은 호출되어야 함
 parm : None
 retn : None
*/
API_EXPORT VOID uvMIL_ResetGrabAll()
{
	//if (g_pMilMain)	
	theApp.clMilMain.ResetGrabAll();
}

/*
 desc : Mark 정보 설정
 parm : cam_id		- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		detail_level- [in]  등록된 모델 이미지와 Grabbed Image이미지의 Edge 영역에 대한 비교 정도 값
							값이 클수록 상세히 비교하지만, 속도는 많이 느려지짐
							LOW보다 HIGH로 갈수록 더 많은 Edge를 가지고 비교하게 됨
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - 기본 값, 1.0f ~ 100.0f
							기본 값은 50이지만, 100으로 갈수록 엣지의 강도가 센것만 나타납니다.
		model		- [in]  Model Type, Param 1 ~ 5개 정보까지 모두 포함된 구조체 포인터
		count		- [in]  등록하고자 하는 모델의 개수
		아래 2개의 값이 0 값이면 파라미터 적용되지 않음
		scale_min	- [in]  검색 대상의 크기 범위 값 설정 (최소한 이 값 비율보다 커야 됨. 범위: 0.5 ~ 1.0)
		scale_max	- [in]  검색 대상의 크기 범위 값 설정 (최대한 이 값 비율보다 작아야 됨. 범위: 1.0 ~ 1.0)
		score_min	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0)
		score_tgt	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0))
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
 desc : 로컬 시스템 (저장소)에 저장되어 있는 패턴 매칭 원본 이미지 적재
 parm : cam_id		- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
		speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		detail_level- [in]  등록된 모델 이미지와 Grabbed Image이미지의 Edge 영역에 대한 비교 정도 값
							값이 클수록 상세히 비교하지만, 속도는 많이 느려지짐
							LOW보다 HIGH로 갈수록 더 많은 Edge를 가지고 비교하게 됨
							0 (M_VERY_LOW), 1 (M_LOW), 2 (M_MEDIUM), 3 (M_HIGH), 4 (M_VERY_HIGH)
		smooth		- [in]  0.0f - 기본 값, 1.0f ~ 100.0f
							기본 값은 50이지만, 100으로 갈수록 엣지의 강도가 센것만 나타납니다.
		model		- [in]  Model Type 즉, circle, square, rectangle, cross, diamond, triangle
		param		- [in]  총 5개의 Parameter Values (unit : um)
		mark_no		- [in]  등록하고자 하는 모델 번호
		아래 2개의 값이 0 값이면 파라미터 적용되지 않음
		scale_min	- [in]  검색 대상의 크기 범위 값 설정 (최소한 이 값 비율보다 커야 됨. 범위: 0.5 ~ 1.0)
		scale_max	- [in]  검색 대상의 크기 범위 값 설정 (최대한 이 값 비율보다 작아야 됨. 범위: 1.0 ~ 1.0)
		score_tgt	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0))
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
 desc : Mark 정보 설정 - 이미지 데이터
 parm : cam_id		- [in]  Align Camera Index (1 or 2)
		speed		- [in]  0 - Very Low, 1 - Low, 2 - Medium, 3 - High, 4 - Very High
		level		- [in]  등록된 모델 이미지와 Grabbed Image이미지의 Edge 영역에 대한 비교 정도 값
							값이 클수록 상세히 비교하지만, 속도는 많이 느려지짐
							LOW보다 HIGH로 갈수록 더 많은 Edge를 가지고 비교하게 됨
							0 (M_MEDIUM), 1 (M_HIGH), 2 (M_VERY_HIGH)
		smooth		- [in]  0.0f - 기본 값, 1.0f ~ 100.0f
		scale_min	- [in]  검색 대상의 크기 범위 값 설정 (최소한 이 값 비율보다 커야 됨. 범위: 0.5 ~ 1.0)
		scale_max	- [in]  검색 대상의 크기 범위 값 설정 (최대한 이 값 비율보다 작아야 됨. 범위: 1.0 ~ 1.0)
		score_min	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0)
		score_tgt	- [in]  이 값 이하로는 검색할 수 없다 (입력되는 값은 percentage (0.0 ~ 100.0))
		name		- [in]  Model Name
		file		- [in]  모델 이미지가 저장된 파일 이름 (전체 경로 포함. Image File)
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
 desc : 현재 등록된 Mark Model 정보 반환
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : 등록된 구조체 포인터 반환
*/
API_EXPORT LPG_CMPV uvMIL_GetModelDefine(UINT8 cam_id)
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.GetModelDefine(cam_id);
}

/*
 desc : Camera로부터 수집된 Grabbed Image (Binary Image Data)의 Edge Detection
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
		grab	- [in]  Grabbed Image 정보가 저장된 구조체 포인터
		saved	- [in]  분석된 edge image 저장 여부
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
 desc : 현재 검색을 위해 등록된 모델의 개수 반환
 parm : cam_id	- [in]  Camera Index (0x01 ~ MAX_INSTALL_CAMERA_COUNT)
 retn : 등록된 개수
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
 desc : 사용자에 의해 수동으로 검색된 경우, 결과 값만 처리
 parm : grab	- [in]  사용자에 의해 수동으로 입력된 grabbed image 결과 정보가 저장된 구조체 포인터
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
 desc : Geometric Model Find (Step; Vision 단차 확인용)
 parm : cam_id	- [in]  Camera Index (1 or 2)
		width	- [in]  Grabbed Image - Width (Pixel)
		height	- [in]  Grabbed Image - Width (Pixel)
		count	- [in]  검색하고자 하는 모델 개수
		angle	- [in]  각도 적용 여부 (TRUE : 각도 측정함, FALSE : 각도 측정하지 않음)
						TRUE : 현재 카메라의 회전된 각도 구하기 위함, FALSE : 기존 각도 적용하여 회전된 각도 구함
		results	- [out] 검색된 결과 값 반환 구조체 포인터
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_RunModelStep(UINT8 cam_id, PUINT8 image, UINT32 width, UINT32 height, UINT16 count, BOOL angle, LPG_ACGR results, UINT8 img_id, UINT8 dlg_id, UINT8 mark_no, UINT8 img_proc)
{
	// if (!g_pMilMain)	return FALSE;
	return  theApp.clMilMain.RunModelStep(cam_id, image, width, height, count, angle, results, img_id, dlg_id, mark_no, img_proc);
}

/*
 desc : Geometric Model Find (Examination; Vision Align 노광 결과 확인용)
 parm : width		- [in]  Grabbed Image - Width (Pixel)
		height		- [in]  Grabbed Image - Width (Pixel)
		score		- [in]  크기 비율 값 (이 값보다 크거나 같으면 Okay)
		scale		- [in]  매칭 비율 값 (1.0 기준으로 +/- 이 값 범위 안에 있으면 Okay)
		results		- [out] 검색된 결과 값 반환 구조체 포인터
							만약 NULL인 경우, 검색 되었는지 여부만 묻기 위함
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_RunModelExam(PUINT8 image, UINT32 width, UINT32 height,DOUBLE score, DOUBLE scale, LPG_ACGR results, UINT8 img_id, UINT8 dlg_id, UINT8 mark_no) // 미사용
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
 desc : 가장 최근 Grabbed Image의 매칭 검색 결과 구조체 포인터 반환
 parm : None
 retn : 객체 포인터 (매칭이 실패한 경우 NULL)
*/
API_EXPORT LPG_ACGR uvMIL_GetLastGrabbedMark()
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.GetLastGrabbedMark();
}

/*
 desc : 검색된 마크 이미지가 포함된 구조체 포인터 반환
 parm : cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
 retn : 구조체 포인터 반환
*/
API_EXPORT LPG_ACGR uvMIL_GetGrabbedMark(UINT8 cam_id, UINT8 img_id)
{
	// if (!g_pMilMain)	return NULL;
	return theApp.clMilMain.GetGrabbedMark(cam_id, img_id);
}

/*
 desc : 가장 최근에 Edge Detection된 이미지에서 특정 영역만 추출해서 Bitmap Image로 등록
		즉, Mark Template 이미지로 등록
 parm : cam_id	- [in]  Align Camera ID (1 or Later)
		area	- [in]  추출하고자 하는 영역 정보가 저장된 픽셀 위치 값
		type	- [in]  0x00 : Edge, 0x02 : Line, 0x02 : Match, 0x03 : Grab
		file	- [in]  Mark Template를 저장하려는 파일 이름 (전체 경로 포함)
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_SaveGrabbedMarkToFile(UINT8 cam_id, LPRECT area, UINT8 type, TCHAR *file)
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.SaveGrabbedMarkToFile(cam_id, area, type, file);
}

/*
 desc : 기존에 적재 (등록)되어 있던 모든 Mark 해제 수행
 parm : None
 retn : None
*/
API_EXPORT VOID uvMIL_ResetMarkModel()
{
	//if (g_pMilMain)	
	theApp.clMilMain.ResetMarkModel();
}

/*
 desc : Mark Pattern 등록 여부
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
 desc : Mark Pattern 등록 여부
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

	/* 새로운 파일 생성 하기 */
	mlBufID = theApp.clMilMain.GetBufID(cam_id, type);
	if (!mlBufID)	return;

	/* 원본 이미지 크기 얻기  */
	szSrc.cx = (UINT32)MbufInquire(mlBufID, M_SIZE_X, NULL);
	szSrc.cy = (UINT32)MbufInquire(mlBufID, M_SIZE_Y, NULL);

	/* 원본 이미지 회전 후 크기 얻기 */
	szRot = szSrc;
	uvCmn_RotateRectSize(dbAngle, szRot);
	/* 출력 윈도 크기에 맞게 이미지 축소 혹은 확대된 크기 얻기 */
	dbRate = ResizeDrawBitmap(rDraw, szRot, szTgt, dbRate);
	/* 회전된 후 크기가 커지므로, 크기 재조정 */
	if (szTgt.cx > rDraw.right - rDraw.left)	szTgt.cx = rDraw.right - rDraw.left;
	if (szTgt.cy > rDraw.bottom - rDraw.top)		szTgt.cy = rDraw.bottom - rDraw.top;
	/* 크기 다시 설정된 이미지 버퍼 생성 */
	if (0x00 == type)
	{
		/* Binary Buffer 할당 */
		mlBufResize = MbufAlloc2d(theApp.clMilMain.GetMilSysID(), szTgt.cx, szTgt.cy,
			8 + M_UNSIGNED, bufType, M_NULL);
	}
	else
	{
		/* Color Buffer 할당 */
		mlBufResize = MbufAllocColor(theApp.clMilMain.GetMilSysID(), 3,	/* 1: Monochrome, 3: RGB */
			szTgt.cx, szTgt.cy,
			8 + M_UNSIGNED, bufType, M_NULL);
	}
	if (mlBufResize)
	{
		/* 회전 버퍼 메모리 할당 */
		mlBufRotate = MbufAllocColor(theApp.clMilMain.GetMilSysID(), 3, szRot.cx, szRot.cy,
			8 + M_UNSIGNED, bufType, M_NULL);
		/* 회전 각도에 따라 원본 이미지 회전 후 회전 버퍼에 복사 */
		if (mlBufRotate)
		{
			MimRotate(mlBufID, mlBufRotate, dbAngle,
				M_DEFAULT, M_DEFAULT, /*(MIL_DOUBLE)szResize.cx/2.0f, (MIL_DOUBLE)szResize.cy/2.0f, */
				M_DEFAULT, M_DEFAULT, /*(MIL_DOUBLE)szResize.cy/2.0f, (MIL_DOUBLE)szResize.cx/2.0f, */
				M_NEAREST_NEIGHBOR + M_OVERSCAN_CLEAR);
			//			MbufExport(L"g:\\download\\qcells\\export_rotate.bmp", M_BMP, mlBufRotate);
						/* 이미지 크기 재조정 후 임시 버퍼에 할당 */
			MimResize(mlBufRotate, mlBufResize, M_FILL_DESTINATION, M_FILL_DESTINATION, M_INTERPOLATE);
			//			MbufExport(L"g:\\download\\qcells\\export_resize.bmp", M_BMP, mlBufResize);
						/* 이미지 버퍼의 내용을 특정 윈도 영역에 출력 */
			MdispSelectWindow(theApp.clMilMain.GetDispID(DISP_TYPE_EXPO), mlBufResize, hwnd); // lk91 미사용...?
			/* 회전 버퍼 메모리 해제 */
			MbufFree(mlBufRotate);
		}
		/* 이미지 제거 */
		MbufFree(mlBufResize);
	}

	// lk91 수정 버전 (삭제 예정, expo에서만 사용하는 함수)
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
//	/* 새로운 파일 생성 하기 */
//	mlBufID	= theApp.clMilMain.GetBufID(cam_id, type);
//	if (!mlBufID)	return;
//
//	/* 원본 이미지 크기 얻기  */
//	szSrc.cx	= (UINT32)MbufInquire(mlBufID, M_SIZE_X, NULL);
//	szSrc.cy	= (UINT32)MbufInquire(mlBufID, M_SIZE_Y, NULL);
//		
//	/* 원본 이미지 회전 후 크기 얻기 */
//	szRot		= szSrc;
//	uvCmn_RotateRectSize(dbAngle, szRot);
//	/* 출력 윈도 크기에 맞게 이미지 축소 혹은 확대된 크기 얻기 */
//	dbRate		= ResizeDrawBitmap(rDraw, szRot, szTgt, dbRate);
//	/* 회전된 후 크기가 커지므로, 크기 재조정 */
//	if (szTgt.cx > rDraw.right - rDraw.left)	szTgt.cx = rDraw.right - rDraw.left;
//	if (szTgt.cy > rDraw.bottom- rDraw.top)		szTgt.cy = rDraw.bottom- rDraw.top;
//	/* 크기 다시 설정된 이미지 버퍼 생성 */
//	if (0x00 == type)
//	{
//		/* Binary Buffer 할당 */
//		mlBufResize	= MbufAlloc2d(theApp.clMilMain.GetMilSysID(), szTgt.cx, szTgt.cy,
//								  8+M_UNSIGNED, bufType, M_NULL);
//	}
//	else
//	{
//		/* Color Buffer 할당 */
//		mlBufResize	= MbufAllocColor(theApp.clMilMain.GetMilSysID(), 3,	/* 1: Monochrome, 3: RGB */
//									 szTgt.cx, szTgt.cy,
//									 8+M_UNSIGNED, bufType, M_NULL);
//	}
//	if (mlBufResize)
//	{
//		/* 회전 버퍼 메모리 할당 */
//		mlBufRotate	= MbufAllocColor(theApp.clMilMain.GetMilSysID(), 3, szRot.cx, szRot.cy,
//									 8+M_UNSIGNED, bufType, M_NULL);
//		/* 회전 각도에 따라 원본 이미지 회전 후 회전 버퍼에 복사 */
//		if (mlBufRotate)
//		{
//			MimRotate(mlBufID, mlBufRotate, dbAngle,
//					  M_DEFAULT, M_DEFAULT, /*(MIL_DOUBLE)szResize.cx/2.0f, (MIL_DOUBLE)szResize.cy/2.0f, */
//					  M_DEFAULT, M_DEFAULT, /*(MIL_DOUBLE)szResize.cy/2.0f, (MIL_DOUBLE)szResize.cx/2.0f, */
//					  M_NEAREST_NEIGHBOR+M_OVERSCAN_CLEAR);
////			MbufExport(L"g:\\download\\qcells\\export_rotate.bmp", M_BMP, mlBufRotate);
//			/* 이미지 크기 재조정 후 임시 버퍼에 할당 */
//			MimResize(mlBufRotate, mlBufResize, M_FILL_DESTINATION, M_FILL_DESTINATION, M_INTERPOLATE);
////			MbufExport(L"g:\\download\\qcells\\export_resize.bmp", M_BMP, mlBufResize);
//			/* 이미지 버퍼의 내용을 특정 윈도 영역에 출력 */
//			//MdispSelectWindow(theApp.clMilMain.GetDispID(cam_id), mlBufResize, hwnd);
//			MdispSelectWindow(theApp.clMilMain.GetDispID(DISP_TYPE_EXPO), mlBufResize, hwnd);
//
//			
//			/* 회전 버퍼 메모리 해제 */
//			MbufFree(mlBufRotate);
//		}
//		/* 이미지 제거 */
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
	/* Memory DC 영역에 Live Image 출력 그리기 */
	HDC hMemDC;
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;
	HBITMAP hMemBmp, hMemBmpOld;

	/* 출력 윈도 영역 조정 */
	rDraw.left++;
	rDraw.top++;
	rDraw.right--;
	rDraw.bottom--;

	/* 가로와 세로의 이미지 크기 비교 후, 많이 차이나는 부분을 기준으로 설정 */
	if (!ResizeDrawBitmap(rDraw, szSrc, szTgt, dbRate))	return;

	/* 기존 DC 영역 초기화 */
	hBrush		= ::CreateSolidBrush(RGB(32, 32, 32));
	hOldBrush	= (HBRUSH)SelectObject(hdc, hBrush);
	if (!hOldBrush)
	{
		SelectObject(hdc, hOldBrush);
		::DeleteObject(hBrush);
		return;
	}

	/* 메모리 DC와 비트맵 생성하고 연결 시켜 주기 */
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
	/* Memory DC와 Bitmap 간의 연결 */
	hMemBmpOld	= (HBITMAP)::SelectObject(hMemDC, hMemBmp);

	/* 이미지 출력 시작 위치 계산 */
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
 desc : Calibration 이미지 (검색된 결과)를 윈도 영역에 출력 수행 (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
 retn : None
*/
API_EXPORT VOID uvMIL_DrawCaliMarkBitmap(HDC hdc, RECT draw)
{
	LPG_ACGR pstGrab	= NULL;

	// if (!g_pMilMain)	return;

	/* Mark 찾은 결과 값 가져오기 */
	pstGrab	= theApp.clMilMain.GetLastGrabbedMark();
	/* 256 Gray Color 이미지 영역에 출력 */
	if (pstGrab && pstGrab->score_rate > 0.0f)
	{
		DrawGrabMarkBitmap(hdc, draw, pstGrab, 0x01);
	}
}

/*
 desc : 검색된 Mark 이미지 윈도 영역에 출력 수행 (DC를 이용하여 bitmap 이미지로 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		cam_id	- [in]  Camera Index (1 or 2)
		img_id	- [in]  Camera Grabbed Image Index (0 or Later) (if img_id == 0xff then Calibration Method)
		find	- [in]  마크 검색 성공 여부 (검색됐더라도, 검색 결과 값에 따라 달라짐)
						0x00 - 검색 결과 실패, 0x01 - 검색 결과 성공
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_DrawMarkBitmap(HDC hdc, RECT draw, UINT8 cam_id, UINT8 img_id, UINT8 find)
{
	LPG_ACGR pstGrab	= NULL;

	// if (!g_pMilMain)	return FALSE;

	/* Mark 찾은 결과 값 가져오기 */
	pstGrab	= theApp.clMilMain.GetGrabbedMark(cam_id, img_id);
	if (!pstGrab)				return FALSE;
	if (!pstGrab->grab_size)	return FALSE;
	/* 256 Gray Color 이미지 영역에 출력 (검색 안되더라도 무조건 이미지 출력) */
	//DrawGrabMarkBitmap(hdc, draw, pstGrab, find);
	//BOOL CMilMain::RegistMILGrabImg(INT32 fi_No, INT32 width, INT32 height, PUINT8 image)
	if (pstGrab->grab_width < 1 || pstGrab->grab_height < 1 || !pstGrab->grab_data)	return TRUE;

	INT32 Grab_No = (cam_id - 1) * 2 + img_id;
	//INT32 Grab_No = (cam_id - 1) * 2 + (img_id - 1); // lk91 OnImageGrabbed() 함수 참고... uvMIL_RegistMILGrabImg((m_u8CamID - 1) * 2 + (m_u8GrabIndex - 1), pstMark->grab_width, pstMark->grab_height, pstMark->grab_data);
	theApp.clMilMain.RegistMILGrabImg(Grab_No, pstGrab->grab_width, pstGrab->grab_height, pstGrab->grab_data); 
	DrawGrabMarkBitmap_MMPM(hdc, draw, pstGrab, Grab_No, find); 
	return TRUE;
}

/*
 desc : 최종 검색된 이미지 (Model find)를 윈도 영역에 출력 수행 (MIL Buffer에 저장된 것을 출력)
 parm : hwnd	- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
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
	/* Mark 찾은 결과 값 가져오기 */
	pstGrab	= theApp.clMilMain.GetGrabbedMark(cam_id, img_id);
	if (!mlBufID || !pstGrab)	return;
#endif
	/* Get the size of original image (pixel) */
	szSrc.cx	= (UINT32)MbufInquire(mlBufID, M_SIZE_X, NULL);
	szSrc.cy	= (UINT32)MbufInquire(mlBufID, M_SIZE_Y, NULL);
		
	/* 원본 이미지 회전 후 크기 얻기 */
	szRot		= szSrc;
	uvCmn_RotateRectSize(dbAngle, szRot);
	/* 출력 윈도 크기에 맞게 이미지 축소 혹은 확대된 크기 얻기 */
	if (!ResizeDrawBitmap(rDraw, szRot, szTgt, dbRate))	return;
	/* 이미지 출력 시작 위치 계산 */
	ptMove.x	= (LONG)ROUNDED(DOUBLE((rDraw.right - rDraw.left) - szTgt.cx) / 2.0f, 0) + rDraw.left;
	ptMove.y	= (LONG)ROUNDED(DOUBLE((rDraw.bottom- rDraw.top) - szTgt.cy) / 2.0f, 0) + rDraw.top;
	/* 회전된 후 크기가 커지므로, 크기 재조정 */
	if (szTgt.cx > rDraw.right - rDraw.left)	szTgt.cx	= rDraw.right - rDraw.left;
	if (szTgt.cy > rDraw.bottom- rDraw.top)		szTgt.cy	= rDraw.bottom- rDraw.top;

	/* Binary Buffer 할당 */
#if 1
	mlBufResize	= MbufAlloc2d(theApp.clMilMain.GetMilSysID(), szTgt.cx, szTgt.cy,
							  8+M_UNSIGNED, bufType, M_NULL);
#else	/* Grabbed Image가 화면에 출력되는 영역보다 작은 경우, 이 루틴을 적용해 보는 것도 ... */
	CSize szResize (szTgt.cx, szTgt.cy);
	if (szResize.cx < (rDraw.right-rDraw.left))	szResize.cx = rDraw.right-rDraw.left;
	if (szResize.cy < (rDraw.bottom-rDraw.top))	szResize.cy = rDraw.bottom-rDraw.top;
	mlBufResize	= MbufAlloc2d(theApp.clMilMain.GetMilSysID(), szResize.cx, szResize.cy,
							  8+M_UNSIGNED, bufType, M_NULL);
#endif
	if (mlBufResize)
	{
		/* 회전 버퍼 메모리 할당 */
		mlBufRotate	= MbufAllocColor(theApp.clMilMain.GetMilSysID(), 3, szRot.cx, szRot.cy,
									 8+M_UNSIGNED, bufType, M_NULL);
		/* 회전 각도에 따라 원본 이미지 회전 후 회전 버퍼에 복사 */
		if (mlBufRotate)
		{
			MimRotate(mlBufID, mlBufRotate, dbAngle,
					  M_DEFAULT, M_DEFAULT, /*(MIL_DOUBLE)szResize.cx/2.0f, (MIL_DOUBLE)szResize.cy/2.0f, */
					  M_DEFAULT, M_DEFAULT, /*(MIL_DOUBLE)szResize.cy/2.0f, (MIL_DOUBLE)szResize.cx/2.0f, */
					  M_NEAREST_NEIGHBOR+M_OVERSCAN_CLEAR);
			//			MbufExport(L"g:\\download\\qcells\\export_rotate.bmp", M_BMP, mlBufRotate);
			/* 이미지 크기 재조정 후 임시 버퍼에 할당 */
			MimResize(mlBufRotate, mlBufResize, M_FILL_DESTINATION, M_FILL_DESTINATION, M_INTERPOLATE);

			MimFlip(mlBufResize, mlBufResize, M_FLIP_HORIZONTAL, M_DEFAULT);
			// lk91 test ////////////////////////
			//CDPoint dRtnP;
			//dRtnP.x = (double)szTgt.cx / (double)ACA1920_SIZE_X;
			//dRtnP.y = (double)szTgt.cy / (double)ACA1920_SIZE_Y;
			//MimResize(mlBufRotate, mlBufResize, dRtnP.x, dRtnP.y, M_DEFAULT);
			// lk91 test /////////////////////
			//MimResize(mlBufRotate, mlBufResize, M_FILL_DESTINATION, M_FILL_DESTINATION, M_INTERPOLATE);
			/* 이미지 버퍼의 내용을 특정 윈도 영역에 출력 */
			//MdispSelectWindow(theApp.clMilMain.GetDispID(DISP_TYPE_EXPO), mlBufResize, hwnd); // lk91 미사용..?
			MdispSelectWindow(theApp.clMilMain.m_mDisID_EXPO[hwndIdx], mlBufResize, hwnd);

			/* 회전 버퍼 메모리 해제 */
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
					/* 총 검색된 Mark에 대한 정보 표현 (출력) */
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
											  DOUBLE(-dbAngle/*g_pstConfig->set_cams.acam_inst_angle*/),	/* 항상 회전 각도의 - (음수) 값을 붙여줘야 됨 (왜냐하면, Grabbed Image 즉, MIL의 ImgRotate 함수 회전이 시계 반대 방향으로 하기 때문임 */
											  dbRotateCentX, dbRotateCentY);
						}
						/* 출력되는 윈도 영역에 따라, Grabbed Image의 축소 or 확대에 따른 찾은 Mark 도형 크기 축소 or 확대 */
						stInSize.cx	= pstFind[i].mark_width;
						stInSize.cy	= pstFind[i].mark_height;
						stInCent.x	= dbRotateCentX/*pstFind[i].cent_x*/;
						stInCent.y	= dbRotateCentY/*pstFind[i].cent_y*/;
						/* Get the center of mark in the grabbed images */
						GetDrawMarkArea(&stInSize, &stInCent, dbRate, ptCent, ptLine);





						// lk91!!! Mark 격자 그리는 부분
						/* Mark Lines (Rectangle) 출력 */
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


						/* Grabbed Image에서 Mark의 중심 위치에 기호 ('+') 출력 */
						::MoveToEx(hDC, ptCent.x - 20, ptCent.y, NULL);
						::LineTo(hDC, ptCent.x + 20, ptCent.y);
						::MoveToEx(hDC, ptCent.x, ptCent.y - 20, NULL);
						::LineTo(hDC, ptCent.x, ptCent.y + 20);

						//::MoveToEx(hDC,	ptCent.x-5+ptMove.x,	ptCent.y+ptMove.y, NULL);
						//::LineTo(hDC,	ptCent.x+5+ptMove.x,	ptCent.y+ptMove.y);
						//::MoveToEx(hDC,	ptCent.x+ptMove.x,		ptCent.y-5+ptMove.y, NULL);
						//::LineTo(hDC,	ptCent.x+ptMove.x,		ptCent.y+5+ptMove.y);
						///* Mark Lines (Rectangle) 출력 */
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
												  DOUBLE(-dbAngle/*g_pstConfig->set_cams.acam_inst_angle*/),	/* 항상 회전 각도의 - (음수) 값을 붙여줘야 됨 (왜냐하면, Grabbed Image 즉, MIL의 ImgRotate 함수 회전이 시계 반대 방향으로 하기 때문임 */
												  dbRotateCentX, dbRotateCentY);
							}
							/* 출력되는 윈도 영역에 따라, Grabbed Image의 축소 or 확대에 따른 찾은 Mark 도형 크기 축소 or 확대 */
							stInSize.cx	= pstResult->mark_width_px;
							stInSize.cy	= pstResult->mark_height_px;
							stInCent.x	= dbRotateCentX/*pstFind[i].cent_x*/;
							stInCent.y	= dbRotateCentY/*pstFind[i].cent_y*/;
							/* Get the center of mark in the grabbed images */
							GetDrawMarkArea(&stInSize, &stInCent, dbRate, ptCent, ptLine);
							/* Grabbed Image에서 Mark의 중심 위치에 기호 ('+') 출력 */
							::MoveToEx(hDC,	ptCent.x-5+ptMove.x,	ptCent.y+ptMove.y, NULL);
							::LineTo(hDC,	ptCent.x+5+ptMove.x,	ptCent.y+ptMove.y);
							::MoveToEx(hDC,	ptCent.x+ptMove.x,		ptCent.y-5+ptMove.y, NULL);
							::LineTo(hDC,	ptCent.x+ptMove.x,		ptCent.y+5+ptMove.y);
							/* Mark Lines (Rectangle) 출력 */
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

		/* 이미지 제거 */
		MbufFree(mlBufResize);
	}
#endif
}

/*
 desc : Calibration 이미지 윈도 영역에 출력 수행 (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		grab	- [in]  grab 이미지가 저장된 구조체 포인터 (grab_data에 이미지가 정보가 반드시 저장되어 있어야 됨)
		find	- [in]  마크 검색 성공 여부 (검색됐더라도, 검색 결과 값에 따라 달라짐)
						0x00 - 검색 결과 실패, 0x01 - 검색 결과 성공
 retn : None
*/
API_EXPORT VOID uvMIL_DrawMarkDataBitmap(HDC hdc, RECT draw, LPG_ACGR grab, UINT8 find , bool drawForce,UINT8 flipFlag)
{
	// if (!g_pMilMain)	return;

	/* 256 Gray Color 이미지 영역에 출력 */
	if (grab && grab->score_rate > 0.0f || drawForce)
	{
		DrawGrabMarkBitmap(hdc, draw, grab, find, flipFlag);
	}
}

/*
 desc : Drawing - Examination Object Image (Bitmap을 이용하여 출력)
 parm : hdc		- [in]  이미지가 출력 대상 context
		draw	- [in]  이미지가 출력될 영역 (부모 윈도 기준 상대 좌표)
		grab_out- [in]  Grabbed Image 정보가 저장된 구조체 포인터 (바깥 링)
		grab_in	- [in]  Grabbed Image 정보가 저장된 구조체 포인터 (안쪽 원)
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
	/* Memory DC 영역에 Grabbed Image 출력 및 검색된 결과 값 그리기 */
	HPEN hPen, hOldPen;

	/* 수집된 데이터가 없는 경우인지 */
	if (!(grab_out && grab_in))	return;
	if (grab_out->score_rate == 0.0f|| grab_in->score_rate == 0.0f)		return;
	if (grab_out->grab_width < 1.0f	|| grab_out->grab_height < 1.0f)	return;
	if (grab_in->grab_width < 1.0f	|| grab_in->grab_height < 1.0f)		return;

	/* 출력 윈도 영역 조정 */
	rDraw.left++;
	rDraw.top++;
	rDraw.right--;
	rDraw.bottom--;

	/* 가로와 세로의 이미지 크기 비교 후, 많이 차이나는 부분을 기준으로 설정 */
	if (!ResizeDrawBitmap(rDraw, szSrc, szTgt, dbRate))	return;
	/* 출력되는 윈도 영역에 따라, Grabbed Image의 축소 or 확대에 따른 찾은 Mark 도형 크기 축소 or 확대 */
	if (grab_out->score_rate > 0.0f)
	{
		/* Grabbed Image 180도 회전 했으므로 */
		if (g_pstConfig->set_cams.acam_inst_angle)
		{
			/* Grabbed Image의 중심에서 회전했을 경우, 현재 마크의 중심 좌표 값 반환 */
			uvCmn_RotateCoord(DOUBLE(grab_out->grab_width/2.0f), DOUBLE(grab_out->grab_height/2.0f),
							  grab_out->mark_cent_px_x, grab_out->mark_cent_px_y,
							  DOUBLE(-dbAngle/*g_pstConfig->set_cams.acam_inst_angle*/),	/* 항상 회전 각도의 - (음수) 값을 붙여줘야 됨 (왜냐하면, Grabbed Image 즉, MIL의 ImgRotate 함수 회전이 시계 반대 방향으로 하기 때문임 */
							  stCent.x, stCent.y);
		}
		/* 바깥쪽 Mark (Object) 영역 그리기 */
		stSize.cx	= grab_out->mark_width_px;
		stSize.cy	= grab_out->mark_height_px;
		GetDrawMarkArea(&stSize, &stCent, dbRate, ptCent[0], ptLine[0]);
		/* Grabbed Image 180도 회전 했으므로 */
		if (g_pstConfig->set_cams.acam_inst_angle)
		{
			/* Grabbed Image의 중심에서 회전했을 경우, 현재 마크의 중심 좌표 값 반환 */
			uvCmn_RotateCoord(DOUBLE(grab_in->grab_width/2.0f), DOUBLE(grab_in->grab_height/2.0f),
							  grab_in->mark_cent_px_x, grab_in->mark_cent_px_y,
							  DOUBLE(-dbAngle/*g_pstConfig->set_cams.acam_inst_angle*/),	/* 항상 회전 각도의 - (음수) 값을 붙여줘야 됨 (왜냐하면, Grabbed Image 즉, MIL의 ImgRotate 함수 회전이 시계 반대 방향으로 하기 때문임 */
							  stCent.x, stCent.y);
		}
		/* 내부쪽 Mark (Object) 영역 그리기 */
		stSize.cx	= grab_in->mark_width_px;
		stSize.cy	= grab_in->mark_height_px;
		GetDrawMarkArea(&stSize, &stCent, dbRate, ptCent[1], ptLine[1]);
	}

	/* 이미지 출력 시작 위치 계산 */
	ptMove.x = (UINT32)ROUNDED(DOUBLE((rDraw.right - rDraw.left) - szTgt.cx) / 2.0f, 0) + rDraw.left;
	ptMove.y = (UINT32)ROUNDED(DOUBLE((rDraw.bottom- rDraw.top) - szTgt.cy) / 2.0f, 0) + rDraw.top;

	/* Grabbed Image 출력 */
	DrawGrabImage(hdc, grab_out, rDraw, &szTgt, &ptMove);

	/* -------------------------------------- */
	/* Mark Lines (Rectangle) 출력 (Ring Out) */
	/* -------------------------------------- */
	hPen	= ::CreatePen(PS_SOLID, 1, clrFind[0]);
	hOldPen	= (HPEN)::SelectObject(hdc, hPen);
	if (hOldPen)
	{
		/* Mark 외곽 영역에 사각형 출력 */
		::MoveToEx(hdc,	ptLine[0][0].x+ptMove.x,	ptLine[0][0].y+ptMove.y, NULL);
		::LineTo(hdc,	ptLine[0][1].x+ptMove.x,	ptLine[0][1].y+ptMove.y);
		::LineTo(hdc,	ptLine[0][2].x+ptMove.x,	ptLine[0][2].y+ptMove.y);
		::LineTo(hdc,	ptLine[0][3].x+ptMove.x,	ptLine[0][3].y+ptMove.y);
		::LineTo(hdc,	ptLine[0][0].x+ptMove.x,	ptLine[0][0].y+ptMove.y);
		/* Mark 중심에서 Cross (+) 출력 */
		::MoveToEx(hdc,	ptCent[0].x-5+ptMove.x,		ptCent[0].y+ptMove.y, NULL);
		::LineTo(hdc,	ptCent[0].x+6+ptMove.x,		ptCent[0].y+ptMove.y);
		::MoveToEx(hdc,	ptCent[0].x+ptMove.x,		ptCent[0].y-5+ptMove.y, NULL);
		::LineTo(hdc,	ptCent[0].x+ptMove.x,		ptCent[0].y+6+ptMove.y);
		::SelectObject(hdc, hOldPen);
	}
	if (hPen)	::DeleteObject(hPen);

	/* -------------------------------------- */
	/* Mark Lines (Rectangle) 출력 (Ring In)  */
	/* -------------------------------------- */
	hPen	= ::CreatePen(PS_SOLID, 1, clrFind[1]);
	hOldPen	= (HPEN)::SelectObject(hdc, hPen);
	if (hOldPen)
	{
		/* Mark 외곽 영역에 사각형 출력 */
		::MoveToEx(hdc,	ptLine[1][0].x+ptMove.x,	ptLine[1][0].y+ptMove.y, NULL);
		::LineTo(hdc,	ptLine[1][1].x+ptMove.x,	ptLine[1][1].y+ptMove.y);
		::LineTo(hdc,	ptLine[1][2].x+ptMove.x,	ptLine[1][2].y+ptMove.y);
		::LineTo(hdc,	ptLine[1][3].x+ptMove.x,	ptLine[1][3].y+ptMove.y);
		::LineTo(hdc,	ptLine[1][0].x+ptMove.x,	ptLine[1][0].y+ptMove.y);
		/* Mark 중심에서 Cross (+) 출력 */
		::MoveToEx(hdc,	ptCent[1].x-5+ptMove.x,		ptCent[1].y+ptMove.y, NULL);
		::LineTo(hdc,	ptCent[1].x+6+ptMove.x,		ptCent[1].y+ptMove.y);
		::MoveToEx(hdc,	ptCent[1].x+ptMove.x,		ptCent[1].y-5+ptMove.y, NULL);
		::LineTo(hdc,	ptCent[1].x+ptMove.x,		ptCent[1].y+6+ptMove.y);

		::SelectObject(hdc, hOldPen);
	}
	if (hPen)	::DeleteObject(hPen);
}

/*
 desc : Edge Detection 검색된 결과 반환
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 retn : 결과 값이 저장된 포인터 반환
*/
API_EXPORT LPG_EDFR uvMIL_GetEdgeDetectResults(UINT8 cam_id)
{
	// if (!g_pMilMain)	return NULL;
	return theApp.clMilMain.GetEdgeDetectResults(cam_id);
}

/*
 desc : Edge Detection 검색된 개수 반환
 parm : cam_id	- [in]  Align Camera ID (1 or 2)
 변환 : 개수 반환. 실패할 경우, 0 이하 값
*/
API_EXPORT INT32 uvMIL_GetEdgeDetectCount(UINT8 cam_id)
{
	// if (!g_pMilMain)	return 0;
	return theApp.clMilMain.GetEdgeDetectCount(cam_id);
}

/*
 desc : 현재 등록된 Mark Model Type 반환
 parm : cam_id	- [in]  Align Camera Index (0x01 ~ MAX_INSTALL_CAMERA)
		mark_id	- [in]  Align Mark Index (0x00 ~ MAX_REGIST_MODEL-1)
 retn : Model Type 값
*/
API_EXPORT UINT32 uvMIL_GetMarkModelType(UINT8 cam_id, UINT8 index)
{
	// if (!g_pMilMain)	return 0;
	return theApp.clMilMain.GetMarkModelType(cam_id, index);
}

/*
 desc : 현재 등록된 Mark Model 크기 반환
 parm : cam_id	- [in]  Grabbed Image 정보가 발생된 Align Camera Index (1 or 2)
		index	- [in]  요청하고자 하는 모델의 위치 (Zero-based)
		flag	- [in]  0x00 : 가로  크기, 0x01 : 세로 크기
		unit	- [in]  0x00 : um, 0x01 : pixel
 retn : 크기 반환 (단위: um)
*/
API_EXPORT DOUBLE uvMIL_GetMarkModelSize(UINT8 cam_id, UINT8 index, UINT8 flag, UINT8 unit)
{
	// if (!g_pMilMain)	return 0.0f;
	return theApp.clMilMain.GetMarkModelSize(cam_id, index, flag, unit);
}

/*
 desc : Align Mark 검색 방식 설정
 parm : method	- [in]  0x00 : 최적의 1개 반환, 0x01 : 다점 마크 기준으로 가중치를 부여하여 결과 반환
		count	- [in]  'method' 값이 2 or 3인 경우, 최종 검색될 Mark 개수 값 2 이상 값이어야 됨
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
 desc : 이미지의 가로 혹은 세로의 경계선 중심 위치 얻기
 parm : cam_id		- [in]  Align Camera ID (1 or 2)
		grab_data	- [in]  Grabbed Image 정보가 저장된 구조체 포인터
		mil_result	- [in]  MIL 내부 검색 결과 대화 상자 출력 여부
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_RunLineCentXY(UINT8 cam_id, LPG_ACGR grab_data, BOOL mil_result)
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.RunLineCentXY(cam_id, grab_data, mil_result);
}

/*
 desc : Iamge 회전 (이미지의 중심을 기준으로 회전)
 parm : img_src		- [in]  원본 이미지 버퍼
		img_dst		- [out] 회전된 이미지 버퍼
		width		- [in]	원본 이미지의 넓이
		height		- [in]	원본 이미지의 높이
		angle		- [in]	회전 하고자 하는 각도
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
 desc : 현재 MIL 라이선스가 유효한지 여부 확인
		Pattern, Model Finder, Edge Finder, Read, OCR, Image Processing 등 유효한지 확인
 parm : None
 retn : TRUE or FALS
*/
API_EXPORT BOOL uvMIL_IsLicenseValid()
{
	return theApp.clMilMain.IsLicenseValid();
}

/*
 desc : 노광 모드 설정 즉, 직접 노광, 얼라인 노광, 보정 후 얼라인 노광
 parm : mode	- [in]  직접 노광 (0x00), 얼라인 노광 (0x01), 얼라인 카메라 보정 값 적용 후 얼라인 노광 (0x02)
 retn : None
*/
API_EXPORT VOID uvMIL_SetAlignMode(ENG_AOEM mode)
{
	// if (!g_pMilMain)	return;
	theApp.clMilMain.SetAlignMode(mode);
}

/*
 desc : 카메라로부터 수집된 이미지 버퍼의 내용을 파일로 저장
 parm : file	- [in]  저장하려는 파일 (전체 경로 포함)
		width	- [in]  이미지의 크기 (넓이: pixel)
		height	- [in]  이미지의 크기 (높이: pixel)
		image	- [in]  이미지가 저장된 버퍼
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvMIL_SaveGrabbedToFile(PTCHAR file, INT32 width, INT32 height, PUINT8 image)
{
	// if (!g_pMilMain)	return FALSE;
	return theApp.clMilMain.SaveGrabbedToFile(file, width, height, image);
}

/*
 desc : 현재 적재된 레시피의 마크 검색 조건 값 설정
 parm : score	- [in]  Score Accpetance (대상 이미지에서 검색된 마크의 Score 값이 이 값보다 작으면 버린다)
		scale	- [in]  Scale Range (대상 이미지에서 등록된 마크를 검색 결과들 중에서 이 비율 범위 내에 )
									(포함된 결과들만 유효하고, 그 중에서 Score 값이 가장 높은 것만 추출)
									(가령. 이 값이 50.0000 이면, 실제 검색에 사용되는 마크의 크기 범위)
									(검색범위 = 1.0f - (100.0 - 50.0) / 100 임 즉, 0.5 ~ 1.5 임)
 retn : None
*/
API_EXPORT VOID uvMIL_SetRecipeMarkRate(DOUBLE score, DOUBLE scale)
{
	// if (!g_pMilMain)	return;
	theApp.clMilMain.SetRecipeMarkRate(score, scale);
}

/*
 desc : 스테이지의 이동 방향 설정 (정방향 이동인지 / 역방향 이동인지 여부)
 parm : direct	- [in]  TRUE (정방향 이동 : 앞에서 뒤로 이동), FALSE (역방향 이동)
 retn : None
*/
API_EXPORT VOID uvMIL_SetMoveStateDirect(BOOL direct)
{
	// if (!g_pMilMain)	return;
	theApp.clMilMain.SetMoveStateDirect(direct);
}

/*
 desc : Grabbed Image 마다 표현된 마크의 개수가 여러 개인 경우, 마크 들이 분포된 영역의 크기 값 설정
 parm : width	- [in]  사각형 영역의 넓이 값 (단위: um)
		height	- [in]  사각형 영역의 높이 값 (단위: um)
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

	/* MMF 일 경우, 검색된 마크의 경계선 영역을 사각형으로 그리기 위함 (Mark 가 MMF 파일인 경우, 마크 크기 값) */
	theApp.clMilMain.SetMultiMarkArea(width, height);
}

/* ----------------------------------------------------------------------------------------- */
/*                                 lk91 VISION 추가 함수                                     */
/* ----------------------------------------------------------------------------------------- */
/* desc : MarkROI 초기화 */
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

/* desc : 최종 검색된 Live 이미지 윈도 영역에 출력 수행 (Bitmap을 이용하여 출력) */
API_EXPORT VOID uvMIL_DrawLiveBitmap(HDC hdc, RECT draw, LPG_ACGR grab, DOUBLE angle)
{
	UINT32 u32MoveX, u32MoveY;
	DOUBLE dbRate;
	PUINT8 pImgBuff = NULL;
	RECT rDraw(draw);
	CSize szSrc(grab->grab_width, grab->grab_height), szTgt;
	/* Memory DC 영역에 Live Image 출력 그리기 */
	HDC hMemDC;
	HBRUSH hBrush, hOldBrush;
	HBITMAP hMemBmp, hMemBmpOld;

	/* 출력 윈도 영역 조정 */
	rDraw.left++;
	rDraw.top++;
	rDraw.right--;
	rDraw.bottom--;

	/* 가로와 세로의 이미지 크기 비교 후, 많이 차이나는 부분을 기준으로 설정 */
	if (!ResizeDrawBitmap(rDraw, szSrc, szTgt, dbRate))	return;

	/* 기존 DC 영역 초기화 */
	hBrush = ::CreateSolidBrush(RGB(32, 32, 32));
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	if (!hOldBrush)
	{
		SelectObject(hdc, hOldBrush);
		::DeleteObject(hBrush);
		return;
	}

	/* 메모리 DC와 비트맵 생성하고 연결 시켜 주기 */
	hMemDC = ::CreateCompatibleDC(hdc);
	hMemBmp = ::CreateCompatibleBitmap(hdc, rDraw.right - rDraw.left, rDraw.bottom - rDraw.top);
	if (!hMemBmp)
	{
		SelectObject(hdc, hOldBrush);
		::DeleteObject(hBrush);
		if (hMemDC)	::DeleteDC(hMemDC);
		return;
	}
	/* Memory DC와 Bitmap 간의 연결 */
	hMemBmpOld = (HBITMAP)::SelectObject(hMemDC, hMemBmp);

	/* 이미지 출력 시작 위치 계산 */
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

	/* 화면 DC에 출력 */
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
 

/* desc : MIL ID 로 저장된 이미지 화면 출력 */
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


/* desc : MIL ID 로 저장된 이미지 화면 출력 */
API_EXPORT VOID uvMIL_DrawImageBitmap(int dispType, int Num, LPG_ACGR grab, DOUBLE angle, int cam_id)
{
	//UINT32 u32MoveX, u32MoveY;
	//DOUBLE dbRate;
	PUINT8 pImgBuff = NULL;
	CSize szSrc(grab->grab_width, grab->grab_height), szTgt;
	//HDC hdc = ::GetDC(hWnd);
	/* Memory DC 영역에 Live Image 출력 그리기 */
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

/* desc : Mark 정보 설정 - MMF (MIL Model Find File) */
API_EXPORT BOOL uvMIL_SetModelDefineMMF(UINT8 cam_id, PTCHAR name, PTCHAR mmf, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no)
{
	return theApp.clMilMain.SetModelDefineMMF(cam_id, name, mmf, m_MarkSizeP, m_MarkCenterP, mark_no);
}

/* desc : Mark 정보 설정 - PAT (MIL Pattern Matching File) */
API_EXPORT BOOL uvMIL_SetModelDefinePAT(UINT8 cam_id, PTCHAR name, PTCHAR pat, CPoint m_MarkSizeP, CPoint m_MarkCenterP, UINT8 mark_no)
{
	return theApp.clMilMain.SetModelDefinePAT(cam_id, name, pat, m_MarkSizeP, m_MarkCenterP, mark_no);
}

/* desc : Set Mark ROI */
API_EXPORT VOID uvMIL_MilSetMarkROI(UINT8 cam_id, CRect fi_rectSearhROI)
{
	theApp.clMilMain.MilSetMarkROI(cam_id, fi_rectSearhROI);
}

/* desc: pat Mark 등록 */
API_EXPORT BOOL uvMIL_RegistPat(UINT8 cam_id, LPG_ACGR grab, CRect fi_rectArea, CString fi_filename, UINT8 mark_no)
{
	return theApp.clMilMain.RegistPat(cam_id, grab->grab_data, fi_rectArea, fi_filename, mark_no);
}

/* desc: mmf Mark 등록 */
API_EXPORT BOOL uvMIL_RegistMod(UINT8 cam_id, LPG_ACGR grab, CRect fi_rectArea, CString fi_filename, UINT8 mark_no)
{
	return theApp.clMilMain.RegistMod(cam_id, grab->grab_data, fi_rectArea, fi_filename, mark_no);
}

/* desc: MMPM Auto Center 이미지 등록 */
API_EXPORT BOOL uvMIL_RegistMMPM_AutoCenter(CRect fi_rectArea, UINT8 cam_id, UINT8 img_id)
{
	LPG_ACGR pstGrab = NULL;

	// if (!g_pMilMain)	return FALSE;

	/* Mark 찾은 결과 값 가져오기 */
	pstGrab = theApp.clMilMain.GetGrabbedMark(cam_id, img_id);
	if (!pstGrab)				return FALSE;
	if (!pstGrab->grab_size)	return FALSE;
	/* 256 Gray Color 이미지 영역에 출력 (검색 안되더라도 무조건 이미지 출력) */
	if (pstGrab->grab_width < 1 || pstGrab->grab_height < 1 || !pstGrab->grab_data)	return TRUE;

	INT32 Grab_No = (cam_id - 1) * 2 + img_id;
	//INT32 Grab_No = (cam_id - 1) * 2 + (img_id - 1); // lk91 OnImageGrabbed() 함수 참고... uvMIL_RegistMILGrabImg((m_u8CamID - 1) * 2 + (m_u8GrabIndex - 1), pstMark->grab_width, pstMark->grab_height, pstMark->grab_data);

	// lk91 여기서 이미지 잘라서 등록하기...
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

/* desc: Mark Size, Offset 초기화 */
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

/* desc: MMF 파일에 MASK 정보 저장, Mark Set에서만 사용 */
API_EXPORT VOID uvMIL_SaveMask_MOD(UINT8 cam_id, UINT8 mark_no)
{
	theApp.clMilMain.SaveMask_MOD(cam_id, mark_no);
}

/* desc: PAT 파일에 MASK 정보 저장, Mark Set에서만 사용 */
API_EXPORT VOID uvMIL_SaveMask_PAT(UINT8 cam_id, UINT8 mark_no)
{
	theApp.clMilMain.SaveMask_PAT(cam_id, mark_no);
}

/* desc: PAT MARK 저장 */
API_EXPORT VOID uvMIL_PatMarkSave(UINT8 cam_id, CString fi_strFileName, UINT8 mark_no)
{
	theApp.clMilMain.PatMarkSave(cam_id, fi_strFileName, mark_no);
}

/* desc: MMF MARK 저장 */
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

/* desc: Find Center (Mark Set에서만 사용) */
API_EXPORT VOID uvMIL_CenterFind(int cam_id, int fi_length, int fi_curSmoothness, double* fi_NumEdgeMIN_X, double* fi_NumEdgeMAX_X, double* fi_NumEdgeMIN_Y, double* fi_NumEdgeMAX_Y, int* fi_NumEdgeFound, int fi_Mode)
{
	theApp.clMilMain.CenterFind(cam_id, fi_length, fi_curSmoothness, fi_NumEdgeMIN_X, fi_NumEdgeMAX_X, fi_NumEdgeMIN_Y, fi_NumEdgeMAX_Y, fi_NumEdgeFound, fi_Mode);
}

/* desc: Mil Main 할당 변수 해제 */
API_EXPORT VOID uvMIL_CloseSetMark()
{
	theApp.clMilMain.CloseSetMark();
}

/* desc: Mask 초기화 */
API_EXPORT VOID uvMIL_InitMask(UINT8 cam_id)
{
	theApp.clMilMain.InitMask(cam_id);
}

/* desc: MARK DISP ID 할당 */
API_EXPORT VOID uvMIL_SetDispMark(CWnd *pWnd)
{
	theApp.clMilMain.SetDispMark(pWnd);
}

/* desc: MARK DISP ID 할당 */
API_EXPORT VOID uvMIL_SetDispRecipeMark(CWnd* pWnd[2])
{
	theApp.clMilMain.SetDispRecipeMark(pWnd);
}

API_EXPORT VOID uvMIL_SetDispExpo(CWnd* pWnd[4])
{
	theApp.clMilMain.SetDispExpo(pWnd);
}
/* desc: Mark 정보 그릴 때 사용하는 MIL 함수 */
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
	desc : Overlay 관련 함수 - 전체 Overlay Clear or DC 출력
	fi_bDrawFlag : FALSE - CLEAR
	fi_iDispType : 0:Expo, 1:mark, 2 : Live, 3 : mark set
	fi_iNo : Cam Num 혹은 Grab Mark Num (각자 Disp Type 에 맞춰서 사용)
*/
API_EXPORT VOID uvMIL_DrawOverlayDC(bool fi_bDrawFlag, int fi_iDispType, int fi_iNo)
{
	theApp.clMilDisp.DrawOverlayDC(fi_bDrawFlag, fi_iDispType, fi_iNo);
}


API_EXPORT VOID uvMIL_Camera_ClearShapes(int fi_iDispType)
{
	theApp.clMilDisp.ClearShapes(fi_iDispType);
}



/* desc : Overlay 관련 함수 - Box List 추가 */
API_EXPORT VOID uvMIL_OverlayAddBoxList(int fi_iDispType, int fi_iNo, int fi_iLeft, int fi_iTop, int fi_iRight, int fi_iBottom, int fi_iStyle, int fi_color)
{
	theApp.clMilDisp.AddBoxList(fi_iDispType, fi_iNo, fi_iLeft, fi_iTop, fi_iRight, fi_iBottom, fi_iStyle, fi_color);
}

/* desc : Overlay 관련 함수 - Cross List 추가 */
API_EXPORT VOID uvMIL_OverlayAddCrossList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, int fi_iWdt1, int fi_iWdt2, int fi_color)
{
	theApp.clMilDisp.AddCrossList(fi_iDispType, fi_iNo, fi_iX, fi_iY, fi_iWdt1, fi_iWdt2, fi_color);
}

/* desc : Overlay 관련 함수 - Text List 추가 */
API_EXPORT VOID uvMIL_OverlayAddTextList(int fi_iDispType, int fi_iNo, int fi_iX, int fi_iY, CString fi_sText, int fi_color, int fi_iSizeX, int fi_iSizeY, CString fi_sFont, bool fi_bEgdeFlag)
{
	theApp.clMilDisp.AddTextList(fi_iDispType, fi_iNo, fi_iX, fi_iY, fi_sText, fi_color, fi_iSizeX, fi_iSizeY, fi_sFont, fi_bEgdeFlag);
}

/* desc : Overlay 관련 함수 - Line List 추가 */
API_EXPORT VOID uvMIL_OverlayAddLineList(int fi_iDispType, int fi_iNo, int fi_iSx, int fi_iSy, int fi_iEx, int fi_iEy, int fi_iStyle, int fi_color)
{
	theApp.clMilDisp.AddLineList(fi_iDispType, fi_iNo, fi_iSx, fi_iSy, fi_iEx, fi_iEy, fi_iStyle, fi_color);
}

/* desc: MARK SET DISP ID 할당 */
API_EXPORT VOID uvMIL_SetDispMarkSet(CWnd* pWnd)
{
	theApp.clMilMain.SetDispMarkSet(pWnd);
}

/* desc: MMPM AutoCenter DISP ID 할당 */
API_EXPORT VOID uvMIL_SetDispMMPM_AutoCenter(CWnd* pWnd)
{
	theApp.clMilMain.SetDispMMPM_AutoCenter(pWnd);
}


/* desc: LIVE DISP ID 할당 */
API_EXPORT VOID uvMIL_SetDisp(CWnd** pWnd, UINT8 fi_Mode)
{
	theApp.clMilMain.SetDisp(pWnd, fi_Mode);
}

/* desc: MMPM DISP ID 할당 */
API_EXPORT VOID uvMIL_SetDispMMPM(CWnd* pWnd)
{
	theApp.clMilMain.SetDispMMPM(pWnd);
}


/* desc: MIL Img ID 등록 */
API_EXPORT BOOL uvMIL_RegistMILImg(INT32 cam_id, INT32 width, INT32 height, PUINT8 image) // grab된 이미지를 mil 에 저장 uvMIL_BufPutMILImg 로 이름 변경할 것
{
	return theApp.clMilMain.RegistMILImg(cam_id, width, height, image);
}

/* desc: MIL Img ID 등록 */
API_EXPORT BOOL uvMIL_RegistMILGrabImg(INT32 fi_No, INT32 width, INT32 height, PUINT8 image) // grab된 이미지를 mil 에 저장 uvMIL_BufPutMILImg 로 이름 변경할 것
{
	return theApp.clMilMain.RegistMILGrabImg(fi_No, width, height, image);
}


/* desc : Zoom 관련 함수 - Zoom In */
API_EXPORT VOID uvMIL_MilZoomIn(int fi_iDispType, int cam_id, CRect rc)
{
	theApp.clMilDisp.MilZoomIn(fi_iDispType, cam_id - 1, rc);
}

/* desc : Zoom 관련 함수 - Zoom Out */
API_EXPORT BOOL uvMIL_MilZoomOut(int fi_iDispType, int cam_id, CRect rc)
{
	return theApp.clMilDisp.MilZoomOut(fi_iDispType, cam_id - 1, rc);
}

/* desc : Zoom 관련 함수 - Zoom Fit, 이미지 비율 1.0 */
API_EXPORT VOID uvMIL_MilAutoScale(int fi_iDispType, int cam_id)
{
	theApp.clMilDisp.MilAutoScale(fi_iDispType, cam_id - 1);
}

/* desc : Zoom 관련 함수 - Mouse Point 저장, Zoom으로 확대되었을 때 이미지 이동하기 위함 */
API_EXPORT VOID uvMIL_SetZoomDownP(int fi_iDispType, int cam_id, CPoint fi_point)
{
	theApp.clMilDisp.SetZoomDownP(fi_iDispType, cam_id - 1, fi_point);
}

/* desc : Zoom 관련 함수 - 이미지 이동 */
API_EXPORT VOID uvMIL_MoveZoomDisp(int fi_iDispType, int cam_id, CPoint fi_point, CRect fi_rect)
{
	theApp.clMilDisp.MoveZoomDisp(fi_iDispType, cam_id - 1, fi_point, fi_rect);
}

/* desc : Zoom 관련 함수 - Zoom 된 영상에서 클릭된 좌표를 이미지 좌표로 변환시켜주는 작업 */
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