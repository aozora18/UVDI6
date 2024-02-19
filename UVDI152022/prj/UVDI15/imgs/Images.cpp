
/*
 desc : 이미지 출력 객체
*/

#include "pch.h"
#include "../MainApp.h"
#include "Images.h"

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                        기본 이미지 객체                                       */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : hwnd	- [in]  이미지가 출력되는 윈도 핸들
 retn : None
*/
CImages::CImages(HWND hwnd)
{
	/* 매개 변수 저장 */
	m_hPicWnd	= hwnd;
	/* 이미지 출력 영역 위치 저장 */
	m_rDraw.SetRectEmpty();
	if (hwnd)
	{
		CWnd *pWndPic	= CWnd::FromHandle(hwnd);
		pWndPic->GetWindowRect(m_rDraw);
		pWndPic->ScreenToClient(m_rDraw);
	}
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CImages::~CImages()
{
	/* DC 핸들 닫기 */
	if (m_hPicDC)	::ReleaseDC(m_hPicWnd, m_hPicDC);
}

/*
 desc : 이미지 출력
 parm : None
 retn : None
*/
VOID CImages::DrawImage()
{
	if (m_hPicDC)	m_picLogo.ShowFit(m_hPicDC, m_rDraw);
}

/*
 desc : 이미지 파일 적재
 parm : None
 retn : TRUE or FALSE
*/
BOOL CImages::LoadFile()
{
	if (!m_hPicWnd)	return FALSE;

	/* 핸들을 이용하여, DC 얻기 */
	m_hPicDC	= ::GetDC(m_hPicWnd);
	if (!m_hPicDC)	return FALSE;

	/* 로고 파일 적재 */
	return m_picLogo.Load(m_tzFile);
}

/* --------------------------------------------------------------------------------------------- */
/*                                 로고 이미지 객체 (PHILOPTICS)                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : hwnd	- [in]  이미지가 출력되는 윈도 핸들
 retn : None
*/
CImgPhil::CImgPhil(HWND hwnd)
	: CImages(hwnd)
{
	wmemset(m_tzFile, MAX_PATH_LEN, 0x00);
	swprintf_s(m_tzFile, MAX_PATH_LEN, L"%s\\ui_img\\logo\\phil.jpg", g_tzWorkDir);
}

/* --------------------------------------------------------------------------------------------- */
/*                                    로고 이미지 객체 (고객사)                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 생성자
 parm : hwnd	- [in]  이미지가 출력되는 윈도 핸들
 retn : None
*/
CImgCustomer::CImgCustomer(HWND hwnd)
	: CImages(hwnd)
{
	wmemset(m_tzFile, MAX_PATH_LEN, 0x00);
	swprintf_s(m_tzFile, MAX_PATH_LEN, L"%s\\ui_img\\logo\\absolics.jpg", g_tzWorkDir);
}

