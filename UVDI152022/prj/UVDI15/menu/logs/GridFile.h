#pragma once

#include "../../../../inc/grid/GridCtrl.h"


class CGridFile
{
/* 생성자 & 파괴자 */
public:

	CGridFile(HWND parent, HWND grid)
	{
		m_pGrid			= NULL;
		m_pParent		= CWnd::FromHandle(parent);
		m_pCtrl			= CWnd::FromHandle(grid);

		wmemset(m_tzFile, 0x00, MAX_FILE_LEN);

		/* 그리드 컨트롤 기본 공통 속성 */
		m_u32Style		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
		m_stGV.mask		= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
		m_stGV.nFormat	= DT_LEFT | DT_VCENTER | DT_SINGLELINE;
		m_stGV.crFgClr	= RGB(32, 32, 32);
		m_stGV.crBkClr	= RGB(255, 255, 255);
	}
	virtual ~CGridFile();


/* 로컬 변수 */
protected:

	TCHAR				m_tzFile[MAX_FILE_LEN];

	UINT32				m_u32Style;

	CRect				m_rGrid;

	GV_ITEM				m_stGV;

	CWnd				*m_pParent;
	CWnd				*m_pCtrl;
	CGridCtrl			*m_pGrid;


/* 로컬 함수 */
protected:


/* 공용 함수 */
public:

	BOOL				Init();
	VOID				LoadFiles();
	VOID				DeleteFiles();

	BOOL				IsSelected();
	PTCHAR				GetSelect();
};
