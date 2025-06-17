
#pragma once

#include "../WorkStep.h"
#include <algorithm>
#include <math.h>
using namespace std;

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif


class CWorkIdle : public CWorkStep
{
/* 생성자 & 파괴자 */
public:

	CWorkIdle();
	virtual ~CWorkIdle();

/* 가상함수 재정의 */
protected:
public:
	
	virtual	BOOL		InitWork();
	virtual VOID		DoWork();

	
	std::pair<double, double> Next(double dx, double dy);

	void SetDirection(double newDx, double newDy)
	{
		
		double mag = std::sqrt(newDx * newDx + newDy * newDy);
		if (mag > 0.0) 
		{
			dx = newDx / mag * 10.0;
			dy = newDy / mag * 10.0;
		}

	}

	std::pair<double, double> NextPos()
	{
		double nextX = x + dx;
		double nextY = y + dy;

		// 반사
		if (nextX < xMin || nextX > xMax) {
			dx *= -1;
			nextX = x + dx;
		}
		if (nextY < yMin || nextY > yMax) {
			dy *= -1;
			nextY = y + dy;
		}

		x = Clamp(nextX, xMin, xMax);
		y = Clamp(nextY, yMin, yMax);

		return { x, y };
	}


	void Reset(double startX, double startY)
	{
		x = startX;
		y = startY;
	}

	std::pair<double, double> GetCurrent() const
	{
		return { x, y };

	}

	double Clamp(double val, double min, double max)
	{
		return std::max(min, std::min(val, max));
	}
/* 로컬 함수 */
protected:

	double x = 490.0;
	double y = 1490.0;
	double dx = -7.07;
	double dy = -7.07;
	const double xMin = 10.0, xMax = 490.0;
	const double yMin = 10.0, yMax = 1490.0;
	
};
