
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
API_IMPORT BOOL uvEng_Calc_GetStraightness(vector<DOUBLE> &in, vector<DOUBLE> &out);


#ifdef __cplusplus
}
#endif