#pragma once

#include "../../../../inc/comn/Picture.h"

class CDrawType
{
/* 생성자 / 파괴자 */
public:
	CDrawType(HWND h_draw);
	virtual ~CDrawType();


/* 열거형 변수 */
protected:


/* 로컬 변수 */
protected:

	ENG_ATGL			m_enType;	/* Align Mark Arrangement Type */

	HWND				m_hDraw;


/* 로컬 함수 */
protected:

	VOID				Draw();


/* 공용 함수 */
public:

	VOID				DrawType(ENG_ATGL type=ENG_ATGL::en_global_0_local_0x0_n_point);
	ENG_ATGL			GetNextType();
	ENG_ATGL			GetType()	{	return m_enType;	}
};
