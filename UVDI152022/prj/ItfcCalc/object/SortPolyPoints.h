#pragma once

#pragma pack (push, 8)

	typedef struct __st_point_xy__
	{
		DOUBLE			x,y;

	}	STM_PTXY, *LPM_PTXY;

#pragma pack (pop)

class CSortPolyPoints
{
/* ������ & �ı��� */
public:
	CSortPolyPoints(DOUBLE *x, DOUBLE *y, UINT32 n);
	~CSortPolyPoints();

/* ����ü */
protected:


/* ���� ���� */
protected:

	UINT32				m_u32Num;
	DOUBLE				*m_pX, *m_pY;
	LPM_PTXY			m_pstXY;


/* ���� �Լ� */
protected:



/* ���� �Լ� */
public:

	DOUBLE				GetX(UINT32 index)	{	return m_pstXY[index].x;	}
	DOUBLE				GetY(UINT32 index)	{	return m_pstXY[index].y;	}
	BOOL				Sort();
};