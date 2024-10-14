#pragma once

#include "../../../../inc/comn/Picture.h"

class CDrawPrev
{
/* 생성자 / 파괴자 */
public:
	CDrawPrev(HWND h_draw);
	CDrawPrev(HWND h_draw, CRect rect);
	virtual ~CDrawPrev();


/* 열거형 변수 */
protected:
	bool yFlip = true;
	bool xFlip = false;

/* 로컬 변수 */
protected:

	HWND				m_hDraw;

	HDC					m_hMemDC;
	HBITMAP				m_hMemBmp;

	CRect				m_rcDrawSize;
	RECT				m_rDraw;
	double				m_dGerberSizeX;
	double				m_dGerberSizeY;
	double				m_dScaleX;
	double				m_dScaleY;

	int					m_nSelectGlobalMark;
	int					m_nSelectLocalMark;

	std::vector < STG_MARK > m_vGlobalMark;
	std::vector < STG_MARK > m_vLocalMark;




/* 로컬 함수 */
protected:


/* 공용 함수 */
public:
	BOOL				GetGlobalMark(std::vector<STG_XMXY> &vMark);
	BOOL				GetLocalMark(std::vector<STG_XMXY>& vMark);
	void				SetGlobalMarkResult(int nID, STG_MARK_INFO& stInfo);
	void				SetLocalMarkResult(int nID, STG_MARK_INFO& stInfo);
	VOID				LoadMark(LPG_RJAF recipe);

	VOID				DrawMem(LPG_RJAF recipe);	/* Memory DC 영역에 그려 놓기 */
#if 0
	VOID				Draw(LPG_RBGF recipe);
#else
	VOID				Draw();
#endif
	DWORD lastClick = GetTickCount();
	int					OnMouseClick(int x, int y,CRect winRect);
	void OnMouseDblClick();
	int					GetSelectGlobalMark() 
	{ 
		return m_nSelectGlobalMark; 
	}

	int					GetSelectLocalMark() 
	{ 
		return m_nSelectLocalMark; 
	}
	void				ReleaseMark();
	void ResetMarkResult();
};
