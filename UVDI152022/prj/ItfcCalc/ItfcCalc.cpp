
/*
 desc : Calculation Algorithm Library
*/

#include "pch.h"
#include "MainApp.h"

/* User-defined class */
#include "./object/LeastSquare.h"
#include "./object/CirCleFit.h"			/* 허프 변환을 통한 직선의 교차점 계산 */
#include "./object/SortPolyPoints.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/* --------------------------------------------------------------------------------------------- */
/*                                           전역 변수                                           */
/* --------------------------------------------------------------------------------------------- */

CLeastSquare			g_csLeastSquare;

/* --------------------------------------------------------------------------------------------- */
/*                                           콜백 함수                                           */
/* --------------------------------------------------------------------------------------------- */



/* --------------------------------------------------------------------------------------------- */
/*                                           내부 함수                                           */
/* --------------------------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------------------------- */
/*                                           외부 함수                                           */
/* --------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

/*
 desc : It takes raw data as input and returns the final straightness (trend line) value.
 parm : in	- [in]  Original Data
		out	- [in]  Result Data
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCalc_GetStraightness(vector<DOUBLE> &in, vector<DOUBLE> &out)
{
	return g_csLeastSquare.GetCalc(in, out);
}

/*
 desc : After constructing a circle with 3 or more coordinates, return the center point of the circle
 parm : count	- [in]  Data Counts
		coord_x	- [in]  Data Array for coord X
		coord_y	- [in]  Data Array for coord Y
        method  - [in]  0x00 : hyperaccuracy, 0x01 : Kasa, 0x02 : Pratt, 0x03 : Taubin
        cent_x  - [out] Center value of circle
        cent_y  - [out] Center value of circle
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCalc_GetCoordsToCicleCent(UINT32 count, DOUBLE *coord_x, DOUBLE *coord_y,
											UINT8 method, DOUBLE &cent_x, DOUBLE &cent_y)
{
	/* It must consist of at least 3 coordinates */
	if (count < 3)	return FALSE;
	CCircleFit csFit;
	return csFit.GetFit(count, coord_x, coord_y, method, cent_x, cent_y);
}

/*
 desc : To find simple closed path for n points for explanation of orientation()
 parm : x	- [out] array coord.
		y	- [out] array coord.
		n	- [in]  array count
 retn : TRUE or FALSE
*/
API_EXPORT BOOL uvCalc_GetSortCoordPoints(DOUBLE *x, DOUBLE *y, UINT32 n)
{
	CSortPolyPoints csSort(x, y, n);
	if (csSort.Sort())
	{
		UINT32 i;
		for (i=0; i<n; i++)
		{
			x[i] = csSort.GetX(i);
			y[i] = csSort.GetY(i);
		}
	}
	return FALSE;
}

#ifdef __cplusplus
}
#endif