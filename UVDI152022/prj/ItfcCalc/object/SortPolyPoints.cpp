
/* N ���� ��ǥ���� �ð� �ݴ� �������� ����  */

#include "pch.h"
#include "SortPolyPoints.h"


#ifdef _DEBUG
#define new	DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

/* A global point needed for  sorting points with reference */
/* to the first point. Used in compare function of qsort() */
LPM_PTXY g_pstXY;

/*
 ���� : �� �� ������ �簢�� �Ÿ� �� ��ȯ
 ���� : p1	- [in]  ù ��° ��ǥ
		p2	- [in]  �� ��° ��ǥ
 ��ȯ : �Ÿ� �� ��ȯ
 ���� : A utility function to return square of distance between p1 and p2
*/
DOUBLE Dist(LPM_PTXY p1, LPM_PTXY p2)
{
	return (p1->x - p2->x) * (p1->x - p2->x) + (p1->y - p2->y) * (p1->y - p2->y);
}

/*
 ���� : ������ ��ǥ�� ���� ���� �˻�
 ���� : p,q,r	- [in]  triplet coordination
 ��ȯ : 0 - p, q and r are collinear
		1 - Clockwise
		2 - Counterclockwise
*/
UINT8 Orientation(LPM_PTXY p, LPM_PTXY q, LPM_PTXY r)
{
	DOUBLE dbVal	= (q->y - p->y) * (r->x - q->x) - (q->x - p->x) * (r->y - q->y);

	if (dbVal == 0.0f) return 0;	/* collinear */
	return (dbVal > 0.0f)? 1 : 2;	/* clockwise or counterclock wise */
}

/*
 ���� : �迭�� ����� ��ǥ ������ QSort �����ϱ� ���� �ݹ� �Լ� 
 ���� : p1, p2	- [in]  ���� ��� ����ü ������
 ��ȯ : -1 - ���� ���� ��� or >= 0
 ���� : A function used by library function qsort() to sort
		an array of points with respect to the first point
*/
INT Compare(const void *vp1, const void *vp2)
{
	LPM_PTXY pt1	= (LPM_PTXY)vp1;
	LPM_PTXY pt2	= (LPM_PTXY)vp2;
  
	/* Find orientation */
	UINT8 u8Ret	= Orientation(&g_pstXY[0], pt1, pt2);
	if (u8Ret == 0)
	{
		return (Dist(&g_pstXY[0], pt2) >= Dist(&g_pstXY[0], pt1)) ? -1 : 1;
	}

	return (u8Ret == 2) ? -1 : 1;
}

/*
 ���� : �� ���� ��ǥ�� ���� �ٲ�
 ���� : p1	- [out] p2 ��ǥ�� �ٲ�
		p2	- [out] p1 ��ǥ�� �ٲ�
 ��ȯ : None
 ���� : A utility function to swap two points
*/
VOID Swap(STM_PTXY &p1, STM_PTXY &p2)
{
	STM_PTXY stTemp	= p1;
	p1	= p2;
	p2	= stTemp;
}


/*
 ���� : ������
 ���� : x	- [in]  2D ��ǥ �� X ��ǥ�� ����� �迭 ������
		y	- [in]  2D ��ǥ �� Y ��ǥ�� ����� �迭 ������
		n	- [in]  X,Y�� �迭 ���� (��� ����)
 ��ȯ : None
*/
CSortPolyPoints::CSortPolyPoints(DOUBLE *x, DOUBLE *y, UINT32 n)
{
	UINT32 i= 0;
	m_pstXY	= NULL;
	m_u32Num= 0;

	if (n > 0 && x && y)
	{
		m_pstXY	= new STM_PTXY[n];
		ASSERT(m_pstXY);
		m_u32Num= n;
		g_pstXY	= m_pstXY;
		for (; i<n; i++)
		{
			m_pstXY[i].x = x[i];
			m_pstXY[i].y = y[i];
		}
	}
}

/*
 ���� : �ı���
 ���� : None
 ��ȯ : None
*/
CSortPolyPoints::~CSortPolyPoints()
{
	if (m_pstXY)	delete [] m_pstXY;
}

/*
 ���� : ��ǥ ���� ���� (�ٰ������� ����)
 ���� : None
 ��ȯ : TRUE or FALSE
*/
BOOL CSortPolyPoints::Sort()
{
	UINT32 i, u32Min = 0;

	/* Find the bottommost point */
	DOUBLE dbY = 0.0f, dbMinY = m_pstXY[0].y;
	
	for (i=1; i<m_u32Num; i++)
	{
		dbY		= m_pstXY[i].y;
  		/* Pick the bottom-most. In case of tie, chose the left most point */
		if ((dbY < dbMinY) || (dbMinY == dbY &&	m_pstXY[i].x < m_pstXY[u32Min].x))
		dbMinY	= m_pstXY[i].y;
		u32Min	= i;
	}
  
	/* Place the bottom-most point at first position */
	Swap(m_pstXY[0], m_pstXY[u32Min]);  
	/* Sort n-1 points with respect to the first point. */
	/* A point p1 comes before p2 in sorted output if p2 */
	/* has larger polar angle (in counterclockwise direction) than p1 */
	qsort(&m_pstXY[1], m_u32Num-1, sizeof(STM_PTXY), Compare);
  
	return TRUE;
}
