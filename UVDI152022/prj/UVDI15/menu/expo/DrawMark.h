#pragma once

#include "../../../../inc/comn/Picture.h"

class CDrawMark
{
/* ������ / �ı��� */
public:
	CDrawMark(HWND *h_draw, HWND *h_text, LPRECT r_draw);
	virtual ~CDrawMark();

/* ����ü ���� */
protected:

#pragma pack (push, 8)
	typedef struct __st_mark_score_scale_move__
	{
		DOUBLE			scale, score;
		DOUBLE			move_x,move_y;

		/*
		 desc : �� �ʱ�ȭ
		 parm : None
		 retn : None
		*/
		VOID Reset()
		{
			scale	= score	= DBL_MAX;
			move_x	= move_y= DBL_MAX;
		}

		/*
		 desc : �� �ʱ�ȭ
		 parm : None
		 retn : None
		*/
		VOID SetZero()
		{
			scale	= score	= 0.0f;
			move_x	= move_y= 0.0f;
		}

		/*
		 desc : �Է� ���� �� �� �������� ����
		 parm : s_scale	- [in]  �˻��� ��ũ �̹��� ũ�� ���� (unit: %)
				s_score	- [in]  �˻��� ��ũ �̹��� ��ġ ���� (unit: %)
				s_move_x- [in]  �˻��� ��ũ �̹��� ���� X �� (unit: mm)
				s_move_y- [in]  �˻��� ��ũ �̹��� ���� Y �� (unit: mm)
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

/* ���� ���� */
protected:

	/* ���� �����ְ� �ִ� ������ ��ȣ �ӽ� ���� */
	UINT8				m_u8PageNo;

	/* ��ũ �˻� ����� �ؽ�Ʈ �� �̹����� ��� */
	HWND				m_hDraw[4];
	HWND				m_hText[4];
	RECT				m_rDraw[4];

	ST_MSSM				m_stMark[4];	/* ���� ȭ�鿡 ��µ� ��ũ �˻� ��� �� */


/* ���� �Լ� */
protected:

	VOID				DrawInit(UINT8 page, UINT8 index);
	VOID				DrawMark(UINT8 page);


/* ���� �Լ� */
public:

	VOID				DrawPage(UINT8 page_no);
};
