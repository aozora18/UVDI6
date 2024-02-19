
/*
 desc : Least Square Method Library
*/

#pragma once


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
API_IMPORT BOOL uvCalc_GetStraightness(vector<DOUBLE> &in, vector<DOUBLE> &out);
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
API_IMPORT BOOL uvCalc_GetCoordsToCicleCent(UINT32 count, DOUBLE *coord_x, DOUBLE *coord_y,
											UINT8 method, DOUBLE &cent_x, DOUBLE &cent_y);
/*
 desc : To find simple closed path for n points for explanation of orientation()
 parm : x	- [out] array coord.
		y	- [out] array coord.
		n	- [in]  array count
 retn : TRUE or FALSE
*/
API_IMPORT BOOL uvCalc_GetSortCoordPoints(DOUBLE *x, DOUBLE *y, UINT32 n);

#ifdef __cplusplus
}
#endif