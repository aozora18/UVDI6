/*
 desc : Least Square Method (https://www.investopedia.com/terms/l/least-squares-method.asp)
		y = A + Bx (least squares method)
*/

#include "pch.h"
#include "LeastSquare.h"

#ifdef _DEBUG
#define new	DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : Constructor
 parm : None
 retn : None
*/
CLeastSquare::CLeastSquare()
{
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CLeastSquare::~CLeastSquare()
{
}

/*
 desc : Sum (n) : Sum from 1 to n
 parm : num	- [in]  Number (Last)
 retn : Sum (Total sum?)
*/
UINT64 CLeastSquare::GetSumOfNum(UINT32 num)
{
	return UINT64((1 + num) * num / 2);
}

/*
 desc : Sum (x) : Sum of data (x) from 1 to n
 parm : in	- [in]  data (x)
 retn : Sum (Total sum?)
*/
DOUBLE CLeastSquare::GetSumOfData(vector<DOUBLE> &in)
{
	DOUBLE dbSum= 0.0f;
	for (INT32 i=0; i<in.size(); i++)	dbSum	+= in[i];
	return dbSum;
}

/*
 desc : Sum (n * n) : Sum of squares (n^2) from 1 to n
 parm : num	- [in] Number (Last)
 retn : Sum (Total sum?)
*/
UINT64 CLeastSquare::GetSumSquareOfNum(UINT32 num)
{
	UINT64 u64Sum	= 0;
	for (UINT32 i=1; i<=num; i++)
	{
#if 0	/* It's slow when it's an integer type */
		u64Sum	+= (UINT64)pow(i, 2);
#else
		u64Sum	+= i * i;
#endif
	}
	return u64Sum;
}

/*
 desc : Sum (n * x) : Sum of multiple of data and number from 1 to n
 parm : in	- [in]  data (x)
 retn : Sum (Total sum?)
 note : Calculates the sum of the values multiplied by the index of each element
*/
DOUBLE CLeastSquare::GetSumMulOfDataNum(vector<DOUBLE> &in)
{
	DOUBLE dbSum	= 0.0f;
	for (INT32 i=0; i<in.size(); i++)
	{
		dbSum	+= in[i] * DOUBLE(i+1);
	}
	return dbSum;
}

/*
 desc : (yi - A - B * xi) ^ 2
 parm : in	- [in]  data (x)
		ict	- [in]  Intercept
		incl- [in]  Inclination
 retn : Sum (Total sum?)
*/
DOUBLE CLeastSquare::GetSumYiABxi(vector<DOUBLE> &in, DOUBLE ict, DOUBLE incl)
{
	DOUBLE dbSum	= 0.0f;
	for (INT32 i=0; i<in.size(); i++)
	{
		dbSum	+= pow(in[i] - ict - incl * (i + 1), 2);
	}
	return dbSum;
}

/*
 desc : y_fit = A + B * xi (or yi)
 parm : in	- [in]  data (x)
		ict	- [in]  Intercept
		incl- [in]  Inclination
		out	- [out] Result
 retn : None
*/
VOID CLeastSquare::CalcFitY(vector<DOUBLE> &in, DOUBLE ict, DOUBLE incl, vector<DOUBLE> &out)
{
	DOUBLE dbVal	= 0.0f;

	for (INT32 i=0; i<in.size(); i++)
	{
		dbVal	= ict + incl * (i + 1);
		out.push_back(dbVal);
	}
}

/*
 desc : Straightness (Tren Lines)
 parm : fit	- [in]  y_fit
		in	- [in]  data
		out	- [out] Result
 retn : None
*/
VOID CLeastSquare::CalcResult(vector<DOUBLE> &fit, vector<DOUBLE> &in, vector<DOUBLE> &out)
{
	DOUBLE dbVal	= 0.0f;

	for (INT32 i=0; i<in.size(); i++)
	{
		dbVal	= fit[i] - in[i];
		out.push_back(dbVal);
	}
}

/*
 desc : It takes raw data as input and returns the final straightness (trend line) value.
 parm : None
 retn : TRUE or FALSE
 note : Please refer to the calculation formula organized in the Excel file
*/
BOOL CLeastSquare::GetCalc(vector<DOUBLE> &in, vector<DOUBLE> &out)
{
	vector <DOUBLE> vFit;
	DOUBLE dbSumNum			= (DOUBLE)GetSumOfNum(UINT32(in.size()));
	DOUBLE dbSumData		= GetSumOfData(in);
	DOUBLE dbSumSquareNum	= (DOUBLE)GetSumSquareOfNum(UINT32(in.size()));
	DOUBLE dbSumMulDataNum	= GetSumMulOfDataNum(in);
	DOUBLE dbD				= DOUBLE(in.size()) * dbSumSquareNum - dbSumNum * dbSumNum;
	DOUBLE dbYictA			= (dbSumSquareNum * dbSumData - dbSumNum * dbSumMulDataNum) / dbD;
	DOUBLE dbInclB			= (DOUBLE(in.size()) * dbSumMulDataNum - dbSumNum * dbSumData) / dbD;
	DOUBLE dbSumIctInc		= GetSumYiABxi(in, dbYictA, dbInclB);
	DOUBLE dbSqrt_y			= sqrt(dbSumIctInc/(in.size()-2));
	DOUBLE dbSqrt_A			= dbSqrt_y * sqrt(dbSumSquareNum/dbD);
	DOUBLE dbSqrt_B			= dbSqrt_y * sqrt(in.size()/dbD);

	/* y_fit = A+B*xi (or yi) */
	CalcFitY(in, dbYictA, dbInclB, vFit);
	/* Get the last result data */
	CalcResult(vFit, in, out);
	/* Remove the vector data */
	vFit.clear();

	return TRUE;
}

