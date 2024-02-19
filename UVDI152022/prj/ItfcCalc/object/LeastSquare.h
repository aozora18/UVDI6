#pragma once

class CLeastSquare
{
/* Constructor and Destructor */
public:

	CLeastSquare();
	virtual ~CLeastSquare();

/* Virtual Function */
protected:

public:


/* Member Parameter : Local */
protected:


/* Member Function : Local */
protected:

	UINT64				GetSumOfNum(UINT32 num);
	DOUBLE				GetSumOfData(vector<DOUBLE> &in);
	UINT64				GetSumSquareOfNum(UINT32 num);
	DOUBLE				GetSumMulOfDataNum(vector<DOUBLE> &in);
	DOUBLE				GetSumYiABxi(vector<DOUBLE> &in, DOUBLE ict, DOUBLE incl);
	VOID				CalcFitY(vector<DOUBLE> &in, DOUBLE ict, DOUBLE incl, vector<DOUBLE> &out);
	VOID				CalcResult(vector<DOUBLE> &fit, vector<DOUBLE> &in, vector<DOUBLE> &out);

/* Member Function : Public */
public:

	BOOL				GetCalc(vector<DOUBLE> &in, vector<DOUBLE> &out);
};
