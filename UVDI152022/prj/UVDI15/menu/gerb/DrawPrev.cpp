
/*
 desc : Gerber Preview 이미지 파일 출력
 */

#include "pch.h"
#include "../../MainApp.h"
#include "DrawPrev.h"
#include "../../mesg/DlgMesg.h"

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : h_draw	- [in]  이미지가 출력되는 윈도 핸들
 retn : None
*/
CDrawPrev::CDrawPrev(HWND h_draw)
{
	m_hDraw		= h_draw;
	m_hMemDC	= NULL;
	m_hMemBmp	= NULL;

	/* 그리고자 하는 영역 얻기 */
	::GetClientRect(m_hDraw, &m_rDraw);
	/* h_draw 윈도 영역의 출력 위치 설정 */
	m_rcDrawSize = m_rDraw;
//	m_rcDrawSize.OffsetRect(17, 5);

	m_rDraw = m_rcDrawSize;

	m_dGerberSizeX = 1;
	m_dGerberSizeY = 1;
	m_dScaleX = 1.0;
	m_dScaleY = 1.0;

	m_nSelectGlobalMark = -1;
	m_nSelectLocalMark = -1;

}

CDrawPrev::CDrawPrev(HWND h_draw, CRect rect)
{
	m_hDraw = h_draw;
	m_hMemDC = NULL;
	m_hMemBmp = NULL;

	/* 그리고자 하는 영역 얻기 */
	::GetClientRect(m_hDraw, &m_rDraw);

	m_rcDrawSize = rect;

	m_dGerberSizeX = 1;
	m_dGerberSizeY = 1;
	m_dScaleX = 1.0;
	m_dScaleY = 1.0;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDrawPrev::~CDrawPrev()
{
	m_vGlobalMark.clear();
	m_vLocalMark.clear();
	/* Memory DC 객체 해제 */
	if (m_hMemBmp)	::DeleteObject(m_hMemBmp);
	if (m_hMemDC)	::DeleteDC(m_hMemDC);
}

/*
 desc : 현재 각 장비의 통신 상태 출력
 parm : recipe	- [in]  Recipe Structure
 retn : None
*/
#if 0
VOID CDrawPrev::Draw(LPG_RBGF recipe)
{
	CHAR szFile[MAX_PATH_LEN]	= {NULL};
	TCHAR tzName[64]= { NULL };
	HDC hDC			= NULL;
	RECT rDraw		= {NULL};
	CPicture picImg;
	CUniToChar csCnv;

	if (!recipe)	return;

	/* 이미지 파일 설정 */
	sprintf_s(szFile, MAX_PATH_LEN, "%s\\%s\\PreviewDownscale.bmp",
			  recipe->gerber_path, recipe->gerber_name);
	/* 이미지 적재 */
	if (!picImg.Load(csCnv.Ansi2Uni(szFile)))	return;

	/* 그리고자 하는 영역 얻기 */
	::GetClientRect(m_hDraw, &rDraw);
 	rDraw.left	+= 15;
 	rDraw.top	+= 25;
 	rDraw.right	-= 15;/*rDraw.left+ picImg.GetWidth();*/
 	rDraw.bottom-= 15;/*rDraw.top + picImg.GetHeight();*/

	/* 그리고자 하는 윈도 핸들 영역의 DC 얻고, 이미지 출력 */
	hDC	= ::GetDC(m_hDraw);
	if (hDC)
	{
		picImg.ShowFit(hDC, rDraw);	/* 이미지 출력 */
		::ReleaseDC(m_hDraw, hDC);	/* DC 핸들 해제 */
	}
}
#else
VOID CDrawPrev::Draw()
{
	HDC hDC		= NULL;

	/* 그리고자 하는 윈도 핸들 영역의 DC 얻고, 이미지 출력 */
	hDC	= ::GetDC(m_hDraw);
	if (hDC && m_hMemDC)
	{
		StretchBlt(hDC, m_rDraw.left, m_rDraw.top, m_rDraw.right, m_rDraw.bottom, m_hMemDC, xFlip ? m_rDraw.right - 1 : 0,
																							yFlip ? m_rDraw.bottom -1 : 0 , 
																							xFlip ? -m_rDraw.right : m_rDraw.right,
																							yFlip ? -m_rDraw.bottom : m_rDraw.bottom, SRCCOPY);

		
		::ReleaseDC(m_hDraw, hDC);	/* DC 핸들 해제 */
	}
}
#endif

BOOL CDrawPrev::GetGlobalMark(std::vector<STG_XMXY>& vMark)
{
	vMark.clear();
	for (auto& mark : m_vGlobalMark)
	{
		vMark.push_back(mark.stMark);
	}

	return 0 < vMark.size();
}

BOOL CDrawPrev::GetLocalMark(std::vector<STG_XMXY>& vMark)
{
	vMark.clear();
	for (auto& mark : m_vLocalMark)
	{
		vMark.push_back(mark.stMark);
	}

	return 0 < vMark.size();
}

void CDrawPrev::SetGlobalMarkResult(int nID, STG_MARK_INFO& stInfo)
{
	for (auto& mark : m_vGlobalMark)
	{
		if (mark.stMark.tgt_id == nID)
		{
			mark.stInfo = stInfo;
			return;
		}
	}
}

void CDrawPrev::SetLocalMarkResult(int nID, STG_MARK_INFO& stInfo)
{
	for (auto& mark : m_vLocalMark)
	{
		if (mark.stMark.tgt_id == nID)
		{
			mark.stInfo = stInfo;
			return;
		}
	}
}

VOID CDrawPrev::LoadMark(LPG_RJAF recipe)
{
	CUniToChar csCnv;
	//////////////////////////////////////////////////////////////////////////
	/* 현재 선택된 거버 파일 (전체 경로) 얻기 */
	CString strGerbPath = csCnv.Ansi2Uni(recipe->gerber_path);
	CString strGerbName = strGerbPath + _T("\\") + csCnv.Ansi2Uni(recipe->gerber_name);
	LPG_RAAF pstAlign = uvEng_Mark_GetSelectAlignRecipe();

	if (!pstAlign)
	{
		return;
	}

	if (!uvEng_Luria_LoadSelectJobXMLEx(csCnv.Uni2Ansi(strGerbName.GetBuffer()), ENG_ATGL(pstAlign->align_type)))
	{
		strGerbName.ReleaseBuffer();
		return;
	}
	strGerbName.ReleaseBuffer();

	uvEng_Luria_GetGerberSize(m_dGerberSizeX, m_dGerberSizeY);

	m_vGlobalMark.clear();
	std::vector < STG_XMXY > vMark;
	/* 거버에 대한 마크 정보 얻기 */
	if (0x00 != uvEng_Luria_GetGlobalMarkJobNameVector(strGerbName.GetBuffer(), vMark, ENG_ATGL(pstAlign->align_type)))
	{
		strGerbName.ReleaseBuffer();
		return;
	}
	strGerbName.ReleaseBuffer();

	for (const auto& mark : vMark)
	{
		STG_MARK stMark;
		stMark.stMark = mark;
		m_vGlobalMark.push_back(stMark);
	}

	m_vLocalMark.clear();
	vMark.clear();
	if (0x00 != uvEng_Luria_GetLocalMarkJobNameVector(strGerbName.GetBuffer(), vMark, ENG_ATGL(pstAlign->align_type)))
	{
		strGerbName.ReleaseBuffer();
		return;
	}
	strGerbName.ReleaseBuffer();

	for (const auto& mark : vMark)
	{
		STG_MARK stMark;
		stMark.stMark = mark;
		m_vLocalMark.push_back(stMark);
	}
	//////////////////////////////////////////////////////////////////////////
}

/*
 desc : 레시피의 Preview 이미지를 메모리 DC 영역에 출력
 parm : recipe	- [in]  Recipe Structure
 retn : None
*/
VOID CDrawPrev::DrawMem(LPG_RJAF recipe)
{
	CHAR szFile[MAX_PATH_LEN]	= {NULL};
	TCHAR tzName[64]= { NULL };
	HDC hDC			= NULL;
	HBITMAP hOldBmp	= NULL;
	HPEN hPen		= NULL, hOldPen;

	HBRUSH hBrush	= NULL, hOldBrush;
	CRect rDraw		= {NULL};
	CPicture picImg;
	CUniToChar csCnv;

	if (!recipe)
	{
		if (m_hMemBmp)	::DeleteObject(m_hMemBmp);
		if (m_hMemDC)	::DeleteDC(m_hMemDC);
		m_hMemBmp	= NULL;
		m_hMemDC	= NULL;

		return;
	}

	/* 이미지 파일 설정 */
	sprintf_s(szFile, MAX_PATH_LEN, "%s\\%s\\PreviewDownscale.bmp",
			  recipe->gerber_path, recipe->gerber_name);

	BOOL bIsExistImage = TRUE;
	/* 파일이 있는지 확인 */
	if (!uvCmn_FindFile(csCnv.Ansi2Uni(szFile)))
	{
		bIsExistImage = FALSE;
	}

	if (bIsExistImage)
	{
		/* 이미지 적재 */
		if (!picImg.Load(csCnv.Ansi2Uni(szFile)))	return;
	}

	/* 메모리 영역에 그려야 하기 때문에 값을 재배치 */
	rDraw.right	= m_rDraw.right - m_rDraw.left;
	rDraw.bottom= m_rDraw.bottom - m_rDraw.top;
	rDraw.left	= 0;
	rDraw.top	= 0;

	/* 그리고자 하는 윈도 핸들 영역의 DC 얻고, 이미지 출력 */
	hDC	= ::GetDC(m_hDraw);
	if (!hDC)	return;

	/* Memory DC 생성 */
	m_hMemDC	= ::CreateCompatibleDC(hDC);
	m_hMemBmp	= ::CreateCompatibleBitmap(hDC, rDraw.right, rDraw.bottom);
	hOldBmp		= (HBITMAP)::SelectObject(m_hMemDC, m_hMemBmp);
	hPen		= ::CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	hOldPen		= (HPEN)::SelectObject(m_hMemDC, hPen);
	hBrush		= ::CreateSolidBrush(RGB(255, 255, 255));
	hOldBrush	= (HBRUSH)::SelectObject(m_hMemDC, hBrush);
	if (!m_hMemBmp || !hOldBmp || !m_hMemDC || !hBrush)
	{
		if (m_hMemDC)	::DeleteDC(m_hMemDC);
		if (m_hMemBmp)	::DeleteObject(m_hMemBmp);
		if (hBrush)		::DeleteObject(hBrush);
		if (hPen)		::DeleteObject(hPen);
		::ReleaseDC(m_hDraw, hDC);	/* DC 핸들 해제 */
		return;
	}

	/* Memory DC 영역을 White 색상으로 도배 */
	::Rectangle(m_hMemDC, 0, 0, rDraw.right, rDraw.bottom);

	CRect rtImage;
	/* Memory DC 영역에 이미지 출력 */
	if (bIsExistImage)
	{
		picImg.ShowFit(m_hMemDC, rDraw, rtImage);	/* 이미지 출력 */
	}
	else
	{
		rDraw = m_rDraw;
	}

	::SelectObject(m_hMemDC, ::GetStockObject(NULL_PEN)); // 펜은 사용하지 않음
	::SelectObject(m_hMemDC, ::GetStockObject(DC_BRUSH)); // 브러시 선택
	
	double	dImageWidth = rDraw.right - rDraw.left;
	double	dImageHeight = rDraw.bottom - rDraw.top;
	double	dScaleX = dImageWidth / m_dGerberSizeX;
	double	dScaleY = dImageHeight / m_dGerberSizeY;
	int		nGlobalMarkSize = 10;
	int		nLocalMarkSize = 5;

	int nIndex = 0;

	LPG_RJAF pstJobRecipe = uvEng_JobRecipe_GetSelectRecipe();
	LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstJobRecipe->expo_recipe));
	for (auto& mark : m_vGlobalMark)
	{
		mark.rtArea.left	= (LONG)(rDraw.CenterPoint().x + ((mark.stMark.mark_x - (m_dGerberSizeX / 2)) * dScaleX) - nGlobalMarkSize);
		mark.rtArea.top		= (LONG)(rDraw.CenterPoint().y + (( (m_dGerberSizeY / 2) - mark.stMark.mark_y) * dScaleY) - nGlobalMarkSize);
		mark.rtArea.right	= (LONG)(rDraw.CenterPoint().x + ((mark.stMark.mark_x - (m_dGerberSizeX / 2)) * dScaleX) + nGlobalMarkSize);
		mark.rtArea.bottom	= (LONG)(rDraw.CenterPoint().y + (((m_dGerberSizeY / 2) - mark.stMark.mark_y) * dScaleY) + nGlobalMarkSize);

		if (nIndex == m_nSelectGlobalMark)
		{
			::SetDCBrushColor(m_hMemDC, MEDIUM_BLUE); // 빨간색 브러시로 설정
		}
		else
		{
			if (mark.stInfo.dScore == -1)
				::SetDCBrushColor(m_hMemDC, LIGHT_GRAY);
			else
				::SetDCBrushColor(m_hMemDC, mark.stInfo.nResult == 1 ? SEA_GREEN : TOMATO);
		}

		::Rectangle(m_hMemDC, mark.rtArea.left, mark.rtArea.top, mark.rtArea.right, mark.rtArea.bottom);
		nIndex++;
	}
	
	nIndex = 0;
	for (auto& mark : m_vLocalMark)
	{

		mark.rtArea.left = (LONG)(rDraw.CenterPoint().x  + ((mark.stMark.mark_x - (m_dGerberSizeX / 2)) * dScaleX) - nLocalMarkSize);
		mark.rtArea.top = (LONG)(rDraw.CenterPoint().y   + (((m_dGerberSizeY / 2) - mark.stMark.mark_y) * dScaleY) - nLocalMarkSize);
		mark.rtArea.right = (LONG)(rDraw.CenterPoint().x  + ((mark.stMark.mark_x - (m_dGerberSizeX / 2)) * dScaleX) + nLocalMarkSize);
		mark.rtArea.bottom = (LONG)(rDraw.CenterPoint().y + (((m_dGerberSizeY / 2) - mark.stMark.mark_y) * dScaleY) + nLocalMarkSize);

		if (nIndex == m_nSelectLocalMark)
		{
			::SetDCBrushColor(m_hMemDC, MEDIUM_BLUE); // 빨간색 브러시로 설정
		}
		else
		{
			if (mark.stInfo.dScore == -1)
				::SetDCBrushColor(m_hMemDC, LIGHT_GRAY);
			else
				::SetDCBrushColor(m_hMemDC, mark.stInfo.nResult == 1 ? SEA_GREEN : TOMATO);
		}
		
		::Rectangle(m_hMemDC, mark.rtArea.left, mark.rtArea.top, mark.rtArea.right, mark.rtArea.bottom);
		nIndex++;
	}

// 	for (auto& mark : m_vGlobalMark)
// 	{
// 		mark.rtArea.left	= rDraw.left + (mark.stMark.mark_x * dScaleX) - nGlobalMarkSize;
// 		mark.rtArea.top		= rDraw.top + (mark.stMark.mark_y * dScaleY) - nGlobalMarkSize;
// 		mark.rtArea.right	= rDraw.left + (mark.stMark.mark_x * dScaleX) + nGlobalMarkSize;
// 		mark.rtArea.bottom	= rDraw.top + (mark.stMark.mark_y * dScaleY) + nGlobalMarkSize;
// 		::Rectangle(m_hMemDC, mark.rtArea.left, mark.rtArea.top, mark.rtArea.right, mark.rtArea.bottom);
// 	}
// 
// 	for (auto& mark : m_vLocalMark)
// 	{
// 		mark.rtArea.left	= rDraw.left + (mark.stMark.mark_x * dScaleX) - nLocalMarkSize;
// 		mark.rtArea.top		= rDraw.top + (mark.stMark.mark_y * dScaleY) - nLocalMarkSize;
// 		mark.rtArea.right	= rDraw.left + (mark.stMark.mark_x * dScaleX) + nLocalMarkSize;
// 		mark.rtArea.bottom	= rDraw.top + (mark.stMark.mark_y * dScaleY) + nLocalMarkSize;
// 		::Rectangle(m_hMemDC, mark.rtArea.left, mark.rtArea.top, mark.rtArea.right, mark.rtArea.bottom);
// 	}


#if 0	/* 여기서 사용하면 안됨) */
	/* 이미지 DC 복원 */
 	if (hOldBmp)	::SelectObject(m_hMemDC, hOldBmp);
 	if (hOldBrush)	::SelectObject(m_hMemDC, hOldBrush);
#endif

	/* 객체 해제 */
	::DeleteObject(hBrush);
	::DeleteObject(hPen);
	::ReleaseDC(m_hDraw, hDC);
}

int CDrawPrev::OnMouseClick(int x, int y, CRect winRect)
{
	POINT point;
	double clickThreshold = 800;
	if (GetTickCount() - lastClick < clickThreshold)
	{
		bool dblClk = true;
		
		//dlg로 띄워주면된다. 다른 엑션 못하게. 어으 구찮어증말.

		return;
	}

	lastClick = GetTickCount();



	if (yFlip) //지랄도 이런 지랄이 없네.
	{
		int minY = std::min_element(m_vGlobalMark.begin(), m_vGlobalMark.end(), [](const STG_MARK& a, const STG_MARK& b)
			{
				return a.rtArea.top < b.rtArea.top;
			})->rtArea.top;

		INT maxY = std::max_element(m_vGlobalMark.begin(), m_vGlobalMark.end(), [](const STG_MARK& a, const STG_MARK& b)
			{
				return a.rtArea.bottom < b.rtArea.bottom;
			})->rtArea.bottom;

		y = maxY - (y - minY);
	}

	point.x = x;
	point.y = y;
	BOOL bFind = FALSE;
	int nIndex = 0;
	for (const auto& mark : m_vGlobalMark)
	{
		if (mark.rtArea.PtInRect(point))
		{
			// 예시로 클릭한 위치의 좌표를 출력합니다.
// 			CString str;
// 			str.Format(_T("Clicked Mark: Global %d (%d, %d)"), nIndex, point.x, point.y);
// 			AfxMessageBox(str);
			m_nSelectGlobalMark = mark.stMark.tgt_id;
			m_nSelectLocalMark = -1;
			return m_nSelectGlobalMark;
		}

	}

	
	for (const auto& mark : m_vLocalMark)
	{
		if (mark.rtArea.PtInRect(point))
		{
			// 예시로 클릭한 위치의 좌표를 출력합니다.
// 			CString str;
// 			str.Format(_T("Clicked Mark: Local %d (%d, %d)"), nIndex, point.x, point.y);
// 			AfxMessageBox(str);
			m_nSelectLocalMark = mark.stMark.tgt_id;;
			m_nSelectGlobalMark = -1;
			return m_nSelectLocalMark;
		}

	
	}

	m_nSelectGlobalMark = -1;
	m_nSelectLocalMark = -1;

	return -1;
}

void CDrawPrev::ReleaseMark()
{
	m_nSelectGlobalMark = -1;
	m_nSelectLocalMark = -1;
	
}

