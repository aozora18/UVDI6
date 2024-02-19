
#pragma once

#include "../../../../inc/grid/GridCtrl.h"

class CGridPLC
{
/* 생성자 & 파괴자 */
public:

	CGridPLC(HWND parent, HWND grid);
	virtual ~CGridPLC();


/* 로컬 변수 */
protected:

	UINT8				m_u8PageNo;		/* 0x00 - Input Bit - Alarm  1	(Digital)			*/
										/* 0x01 - Input Bit - Alarm  2	(Digital)			*/
										/* 0x02 - Input Bit - Alarm  3	(Digital)			*/
										/* 0x03 - Input Bit - Normal 1	(Digital & Analog)	*/
										/* 0x04 - Input Bit - Camera	(Digital & Analog)	*/
	UINT8				m_u8LastPage;

	CWnd				*m_pParent;
	CWnd				*m_pCtrl;
	CGridCtrl			*m_pGrid;

/* 로컬 함수 */
protected:

	VOID				UpdatePage0(LPG_PMRW data);
	VOID				UpdatePage1(LPG_PMRW data);
	VOID				UpdatePage2(LPG_PMRW data);
	VOID				UpdatePage3(LPG_PMRW data);

/* 공용 함수 */
public:

	BOOL				InitGrid();
	VOID				SetPageNext();
	VOID				SetPagePrev();
	BOOL				IsPageLast()	{	return m_u8PageNo == 3;	};
	BOOL				IsPageFirst()	{	return m_u8PageNo == 0;	};
	VOID				UpdatePeriod();
	VOID				UpdatePeriod(std::vector <std::vector <bool>> &vIOGroup);
};
