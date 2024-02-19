#pragma once

#include "../../../../inc/comn/Picture.h"

class CDrawDev
{
/* 생성자 / 파괴자 */
public:
	CDrawDev(HWND *h_draw);
	virtual ~CDrawDev();


/* 열거형 변수 */
protected:


/* 로컬 변수 */
protected:

	UINT8				m_u8DevStat[9];	/* 가장 최근의 장비 상태 (0:Failed, 1:Warning, 2: Failed*/

	HWND				m_hDraw[9];


/* 로컬 함수 */
protected:

	VOID				DrawDev(UINT8 index, UINT8 succ);


/* 공용 함수 */
public:

	VOID				DrawDev();
	VOID				DrawDevAll();
};
