
/*
 desc : �̹��� ��� ��ü
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
/*                                        �⺻ �̹��� ��ü                                       */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ������
 parm : hwnd	- [in]  �̹����� ��µǴ� ���� �ڵ�
 retn : None
*/
CImages::CImages(HWND hwnd)
{
	/* �Ű� ���� ���� */
	m_hPicWnd	= hwnd;
	/* �̹��� ��� ���� ��ġ ���� */
	m_rDraw.SetRectEmpty();
	if (hwnd)
	{
		CWnd *pWndPic	= CWnd::FromHandle(hwnd);
		pWndPic->GetWindowRect(m_rDraw);
		pWndPic->ScreenToClient(m_rDraw);
	}
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CImages::~CImages()
{
	/* DC �ڵ� �ݱ� */
	if (m_hPicDC)	::ReleaseDC(m_hPicWnd, m_hPicDC);
}

/*
 desc : �̹��� ���
 parm : None
 retn : None
*/
VOID CImages::DrawImage()
{
	if (m_hPicDC)	m_picLogo.ShowFit(m_hPicDC, m_rDraw);
}

/*
 desc : �̹��� ���� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CImages::LoadFile()
{
	if (!m_hPicWnd)	return FALSE;

	/* �ڵ��� �̿��Ͽ�, DC ��� */
	m_hPicDC	= ::GetDC(m_hPicWnd);
	if (!m_hPicDC)	return FALSE;

	/* �ΰ� ���� ���� */
	return m_picLogo.Load(m_tzFile);
}

/* --------------------------------------------------------------------------------------------- */
/*                                 �ΰ� �̹��� ��ü (PHILOPTICS)                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ������
 parm : hwnd	- [in]  �̹����� ��µǴ� ���� �ڵ�
 retn : None
*/
CImgPhil::CImgPhil(HWND hwnd)
	: CImages(hwnd)
{
	wmemset(m_tzFile, MAX_PATH_LEN, 0x00);
	swprintf_s(m_tzFile, MAX_PATH_LEN, L"%s\\ui_img\\logo\\phil.jpg", g_tzWorkDir);
}

/* --------------------------------------------------------------------------------------------- */
/*                                    �ΰ� �̹��� ��ü (����)                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ������
 parm : hwnd	- [in]  �̹����� ��µǴ� ���� �ڵ�
 retn : None
*/
CImgCustomer::CImgCustomer(HWND hwnd)
	: CImages(hwnd)
{
	wmemset(m_tzFile, MAX_PATH_LEN, 0x00);
	swprintf_s(m_tzFile, MAX_PATH_LEN, L"%s\\ui_img\\logo\\absolics.jpg", g_tzWorkDir);
}

