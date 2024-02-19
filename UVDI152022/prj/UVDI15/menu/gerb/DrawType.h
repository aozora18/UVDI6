#pragma once

#include "../../../../inc/comn/Picture.h"

class CDrawType
{
/* ������ / �ı��� */
public:
	CDrawType(HWND h_draw);
	virtual ~CDrawType();


/* ������ ���� */
protected:


/* ���� ���� */
protected:

	ENG_ATGL			m_enType;	/* Align Mark Arrangement Type */

	HWND				m_hDraw;


/* ���� �Լ� */
protected:

	VOID				Draw();


/* ���� �Լ� */
public:

	VOID				DrawType(ENG_ATGL type=ENG_ATGL::en_global_0_local_0x0_n_point);
	ENG_ATGL			GetNextType();
	ENG_ATGL			GetType()	{	return m_enType;	}
};
