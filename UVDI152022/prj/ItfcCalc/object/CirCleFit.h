#pragma once

#include <iostream>
#include <cmath>
#include <limits>
#include <iomanip>
#include <cstdlib>

using namespace std;

/*
 Class for Circle
 A circle has 7 fields:
	a, b, r (of type reals), the circle parameters
	s (of type reals), the estimate of sigma (standard deviation)
	g (of type reals), the norm of the gradient of the objective function
	i and j (of type int), the iteration counters (outer and inner, respectively)
*/

class CCircle
{
public:

	// The fields of a Circle
	double a, b, r, s, g, Gx, Gy;
	int i, j;

	// constructors
	CCircle();
	CCircle(double aa, double bb, double rr);

	// no destructor we didn't allocate memory by hand.
};

class CCircleData
{
	public:

	int n;
	double *X;		//space is allocated in the constructors
	double *Y;		//space is allocated in the constructors
	double meanX, meanY;

	// constructors
	CCircleData();
	CCircleData(int N);
	CCircleData(int N, double X[], double Y[]);
	// destructors
	~CCircleData();

	// routines
	void means(void);
	void center(void);
	void scale(void);
};

class CCircleFit
{
public:

	CCircleFit();
	~CCircleFit();

protected:


protected:

	double				Sigma(CCircleData* data, CCircle& circle);

public:

	CCircle				CircleFitByHyper(CCircleData *data);
	CCircle				CircleFitByKasa(CCircleData *data);
	CCircle				CircleFitByPratt(CCircleData *data);
	CCircle				CircleFitByTaubin(CCircleData *data);
	int					CircleFitByLevenbergMarquardtFull(CCircleData *data, CCircle &circleIni, double LambdaIni, CCircle &circle);

	BOOL				GetFit(UINT32 count, DOUBLE *coord_x, DOUBLE *coord_y, UINT8 method,
							   DOUBLE &cent_x, DOUBLE &cent_y);
};
