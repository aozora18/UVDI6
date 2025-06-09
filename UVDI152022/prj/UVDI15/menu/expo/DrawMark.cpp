
/*
 desc : Mark 정보 출력 (이미지 및 결과 데이터)
 */

#include "pch.h"
#include "../../MainApp.h"
#include "DrawMark.h"
#include "../../../../inc/comn/MyStatic.h"
#include "../../GlobalVariables.h"

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : h_draw	- [in]  The handle of the window from which the image is output
		h_text	- [in]  결과 값이 출력되는 윈도 핸들
		r_draw	- [in]  이미지가 출력되는 윈도 영역 (부모 기준 영역)
 retn : None
*/
CDrawMark::CDrawMark(HWND *h_draw, HWND *h_text, LPRECT r_draw) 
{
	m_u8PageNo	= UINT8_MAX;
	/* 윈도 핸들 복사 */
	memcpy(m_hDraw, h_draw, sizeof(HWND) * 4);
	memcpy(m_hText, h_text, sizeof(HWND) * 4);
	memcpy(m_rDraw, r_draw, sizeof(RECT) * 4);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDrawMark::~CDrawMark()
{
}

/*
 desc : 현재 혹은 가장 최근에 검색된 마크 결과 정보 출력 (갱신)
 parm : dc		- [in]  이미지를 출력할 Context (부모)
		page_no	- [in]  현재 출력하고자 하는 마크의 페이지 번호 (1:Global, 2~5:Local)
 retn : None
*/
VOID CDrawMark::DrawPage(UINT8 page)
{
	/* 현재 출력 페이지 번호와 최근 페이지 번호가 다르면. 각종 조건 값 초기화 */
	if (page != m_u8PageNo)
	{
		m_u8PageNo	= page;
		for (UINT8 i=0x00; i<4; i++)	m_stMark[i].Reset();
	}

	if (!uvEng_Camera_GetGrabbedCount())
	{
		DrawInit(page, 0x00);
		DrawInit(page, 0x01);
		DrawInit(page, 0x02);
		DrawInit(page, 0x03);
	}
	else
	{
		DrawMark(page);
	}
}

/*
 desc : 기본 초기화된 마크 이미지 출력
 parm : dc		- [in]  이미지를 출력할 Context (부모)
		page_no	- [in]  현재 출력하고자 하는 마크의 페이지 번호 (1:Global, 2~5:Local)
		index	- [in]  0x00:Left/Top, 0x01:Left/Bottom, 0x02:Right/Top, 0x03:Right/Bottom
 retn : None
*/
VOID CDrawMark::DrawInit(UINT8 page, UINT8 index)
{
	UINT8 i	= 0x00;
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	TCHAR tzMark[128]	= {NULL};
	CPicture picImg;
	CMyStatic *pText	= NULL;

	/* 이미지 적재 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\ui_img\\status\\mark_init.jpg", g_tzWorkDir);
	if (!picImg.Load(tzFile))	return;

	/* Page & Mark Number */
	for (; i<4; i++)
	{
		swprintf_s(tzMark, 128, L"[%u.%02u] [SCORE 000.000] [SCALE 000.000 0] [%%] [X +000.0] [Y +000.0] [mm]",
				   i/2+1, (page-1)*2+i%2+1);
		/* 마크 검색 결과 출력 */
		pText	= (CMyStatic *)CWnd::FromHandle(m_hText[i]);
		pText->SetWindowText(tzMark);
		pText->SetTextColor(RGB(128, 128, 128));
		pText->Invalidate(FALSE);
	}

	/* 이미지 출력 */
	if (m_hDraw[index])
	{
		HDC hDC = ::GetDC(m_hDraw[index]);
		if (hDC)
		{
			picImg.Show(hDC, m_rDraw[index]);
			::ReleaseDC(m_hDraw[index], hDC);
		}
	}
}

/*
 desc : 검색된 마크 이미지 출력
 parm : dc		- [in]  이미지를 출력할 Context (부모)
		page_no	- [in]  현재 출력하고자 하는 마크의 페이지 번호 (1:Global, 2~5:Local)
 retn : None
*/
VOID CDrawMark::DrawMark(UINT8 page_no)
{
	auto& alignMotion = GlobalVariables::GetInstance()->GetAlignMotion();
	CUniToChar csCnv;
	ENG_AMOS motion = alignMotion.markParams.alignMotion;
	UINT8 i, j, k		= 0x00;
	UINT8 u8ACams = motion == ENG_AMOS::en_onthefly_2cam || motion == ENG_AMOS::en_static_2cam ? 2 : alignMotion.markParams.centerCamIdx;
	UINT8 u8ImgNo		= (page_no - 1) * u8ACams, u8ImgId, u8Index, u8Mark=0x00;
	TCHAR tzMark[128]	= {NULL};
	/*BOOL bRedraw		= FALSE;*/
	LPG_ACGR pstMark	= NULL;

	bool isLocalSelRecipe = uvEng_JobRecipe_WhatLastSelectIsLocal();
	LPG_RJAF rcp = uvEng_JobRecipe_GetSelectRecipe(isLocalSelRecipe);
	LPG_REAF pstRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(rcp->expo_recipe));
	

	COLORREF clrText[2]	= { RGB(255, 0, 0), RGB(34, 177, 76) };
	CMyStatic *pText	= NULL;
	
	
	

	

	auto DrawforCentercam = [&]()
		{
			for (int i = 0; i < 4; i++)
			{
				pstMark = uvEng_Camera_GetGrabbedMark(u8ACams, ((m_u8PageNo - 1) * 4) + (i));
				if (!pstMark)
					continue;

				uvEng_Camera_DrawMarkMBufID(m_hDraw[i], m_rDraw[i], u8ACams, i, pstMark->img_id);
				
				swprintf_s(tzMark, 128, L"[%d.%02d] [SCORE %6.3f] [SCALE %6.3f %u] [%%] [X %+4.3f] [Y %+4.3f] [mm]",
					u8ACams, u8ImgId + 1, pstMark->score_rate, pstMark->scale_rate, pstMark->scale_size,
					pstMark->move_mm_x , pstMark->move_mm_y );
				
				u8Mark = pstMark->IsMarkValid() ? 0x01 : 0x00;
				if (u8Mark && pstRecipe)
				{
					u8Mark = pstRecipe->mark_score_accept <= pstMark->score_rate ? 0x01 : 0x00;
				}
				/* Output the grabbed results to the text control */
				pText = (CMyStatic*)CWnd::FromHandle(m_hText[i]);
				if (pText == nullptr) continue;
				pText->SetWindowText(tzMark);
				pText->SetTextColor(clrText[u8Mark]);
				pText->Invalidate(FALSE);

			}
		};


	auto Drawfor2SideCam = [&]()
		{
			/* k = 1 or 2 ? Align Camera 1, 2 */
			for (k = 1, j = 0; k <= u8ACams; k++, j++)
			{
				/* Align Camera 마다 이미지 2개씩 (상/하) 출력 */
				for (i = 0; i < 2; i++, u8Mark = 0x01/*,bRedraw=FALSE*/)
				{
					/* 출력된 컨트롤 Index 값 */
					u8Index = (k - 1) * u8ACams + i;
					/* Align Camera 별로 Grab 및 Mark 인식될 결과 정보 얻기 */
					/* Align Camera 1의 경우, Image Number = 0 or 1 (Global Mark),  4 ~  7 or 12 ~ 15 (Local Mark) */
					/* Align Camera 2의 경우, Image Number = 2 or 3 (Global Mark),  8 ~ 11 or 16 ~ 19 (Local Mark) */
					u8ImgId = i + u8ImgNo;
					pstMark = uvEng_Camera_GetGrabbedMark(k, u8ImgId);
					if (!pstMark)
						continue;

					/* Output the grabbed images on screen */
					uvEng_Camera_DrawMarkMBufID(m_hDraw[u8Index], m_rDraw[u8Index], k, (j * u8ACams) + i, pstMark->img_id);
					/* Update the grabbed results to the text buffer */
					swprintf_s(tzMark, 128, L"[%d.%02d] [SCORE %6.3f] [SCALE %6.3f %u] [%%] [X %+4.3f] [Y %+4.3f] [um]",
						k, u8ImgId + 1, pstMark->score_rate, pstMark->scale_rate, pstMark->scale_size,
						pstMark->move_mm_x * 1000.0f, pstMark->move_mm_y * 1000.0f);
					/* Check if it is higher than the mark valid score of the registered recipe */
					u8Mark = pstMark->IsMarkValid() ? 0x01 : 0x00;
					if (u8Mark && pstRecipe)
					{
						u8Mark = pstRecipe->mark_score_accept <= pstMark->score_rate ? 0x01 : 0x00;
					}
					/* Output the grabbed results to the text control */
					pText = (CMyStatic*)CWnd::FromHandle(m_hText[u8Index]);
					if (pText == nullptr) continue;
					pText->SetWindowText(tzMark);
					pText->SetTextColor(clrText[u8Mark]);
					pText->Invalidate(FALSE);
				}
			}
		};

	switch (motion)
	{
	case ENG_AMOS::en_onthefly_2cam:
	case ENG_AMOS::en_static_2cam:
	{
		Drawfor2SideCam();
	}
	break;

	default:
	{
		DrawforCentercam();
	}
	break;
	}
}