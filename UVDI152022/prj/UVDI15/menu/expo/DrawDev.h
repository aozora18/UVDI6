#pragma once

#include "../../../../inc/comn/Picture.h"

class CDrawDev
{
/* ������ / �ı��� */
public:
	CDrawDev(HWND *h_draw);
	virtual ~CDrawDev();


/* ������ ���� */
protected:


/* ���� ���� */
protected:

	UINT8				m_u8DevStat[9];	/* ���� �ֱ��� ��� ���� (0:Failed, 1:Warning, 2: Failed*/

	HWND				m_hDraw[9];


/* ���� �Լ� */
protected:

	VOID				DrawDev(UINT8 index, UINT8 succ);


/* ���� �Լ� */
public:

	VOID				DrawDev();
	VOID				DrawDevAll();
};
