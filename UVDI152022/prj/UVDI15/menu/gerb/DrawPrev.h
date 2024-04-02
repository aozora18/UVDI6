#pragma once

#include "../../../../inc/comn/Picture.h"

class CDrawPrev
{
/* ������ / �ı��� */
public:
	CDrawPrev(HWND h_draw);
	CDrawPrev(HWND h_draw, CRect rect);
	virtual ~CDrawPrev();


/* ������ ���� */
protected:


/* ���� ���� */
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

/* ���� �Լ� */
protected:


/* ���� �Լ� */
public:
	BOOL				GetGlobalMark(std::vector<STG_XMXY> &vMark);
	BOOL				GetLocalMark(std::vector<STG_XMXY>& vMark);
	void				SetGlobalMarkResult(int nID, STG_MARK_INFO& stInfo);
	void				SetLocalMarkResult(int nID, STG_MARK_INFO& stInfo);
	VOID				LoadMark(LPG_RJAF recipe);

	VOID				DrawMem(LPG_RJAF recipe);	/* Memory DC ������ �׷� ���� */
#if 0
	VOID				Draw(LPG_RBGF recipe);
#else
	VOID				Draw();
#endif

	int					OnMouseClick(int x, int y);
	int					GetSelectGlobalMark() 
	{ 
		return m_nSelectGlobalMark; 
	}

	int					GetSelectLocalMark() 
	{ 
		return m_nSelectLocalMark; 
	}
	void				ReleaseMark();
};
