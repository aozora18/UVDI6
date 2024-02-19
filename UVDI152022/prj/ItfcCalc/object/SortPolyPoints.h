#pragma once

#pragma pack (push, 8)

	typedef struct __st_point_xy__
	{
		DOUBLE			x,y;

	}	STM_PTXY, *LPM_PTXY;

#pragma pack (pop)

class CSortPolyPoints
{
/* 생성자 & 파괴자 */
public:
	CSortPolyPoints(DOUBLE *x, DOUBLE *y, UINT32 n);
	~CSortPolyPoints();

/* 구조체 */
protected:


/* 로컬 변수 */
protected:

	UINT32				m_u32Num;
	DOUBLE				*m_pX, *m_pY;
	LPM_PTXY			m_pstXY;


/* 로컬 함수 */
protected:



/* 공용 함수 */
public:

	DOUBLE				GetX(UINT32 index)	{	return m_pstXY[index].x;	}
	DOUBLE				GetY(UINT32 index)	{	return m_pstXY[index].y;	}
	BOOL				Sort();
};