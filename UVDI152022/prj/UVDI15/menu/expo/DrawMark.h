#pragma once

#include "../../../../inc/comn/Picture.h"

class CDrawMark
{
/* 생성자 / 파괴자 */
public:
	CDrawMark(HWND *h_draw, HWND *h_text, LPRECT r_draw);
	virtual ~CDrawMark();

/* 구조체 변수 */
protected:

#pragma pack (push, 8)
	typedef struct __st_mark_score_scale_move__
	{
		DOUBLE			scale, score;
		DOUBLE			move_x,move_y;

		/*
		 desc : 값 초기화
		 parm : None
		 retn : None
		*/
		VOID Reset()
		{
			scale	= score	= DBL_MAX;
			move_x	= move_y= DBL_MAX;
		}

		/*
		 desc : 값 초기화
		 parm : None
		 retn : None
		*/
		VOID SetZero()
		{
			scale	= score	= 0.0f;
			move_x	= move_y= 0.0f;
		}

		/*
		 desc : 입력 값과 비교 후 동일한지 여부
		 parm : s_scale	- [in]  검색된 마크 이미지 크기 비율 (unit: %)
				s_score	- [in]  검색된 마크 이미지 일치 비율 (unit: %)
				s_move_x- [in]  검색된 마크 이미지 오차 X 값 (unit: mm)
				s_move_y- [in]  검색된 마크 이미지 오차 Y 값 (unit: mm)
		 retn : TRUE or FALSE
		*/
		BOOL IsEqual(DOUBLE s_scale=0.0f, DOUBLE s_score=0.0f,
					 DOUBLE s_move_x=0.0f, DOUBLE s_move_y=0.0f)
		{
			if (scale != s_scale)	return FALSE;
			if (score != s_score)	return FALSE;
			if (move_x != s_move_x)	return FALSE;
			if (move_y != s_move_y)	return FALSE;

			return TRUE;
		}

	}	ST_MSSM,	*LP_MSSM;

#pragma pack (pop)

/* 로컬 변수 */
protected:

	/* 현재 보여주고 있는 페이지 번호 임시 저장 */
	UINT8				m_u8PageNo;

	/* 마크 검색 결과를 텍스트 및 이미지로 출력 */
	HWND				m_hDraw[4];
	HWND				m_hText[4];
	RECT				m_rDraw[4];

	ST_MSSM				m_stMark[4];	/* 현재 화면에 출력된 마크 검색 결과 값 */


/* 로컬 함수 */
protected:

	VOID				DrawInit(UINT8 page, UINT8 index);
	VOID				DrawMark(UINT8 page);


/* 공용 함수 */
public:

	VOID				DrawPage(UINT8 page_no);
};
