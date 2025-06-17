
/*
 desc : 모든 동작을 강제로 중지처리
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkIdle.h"

#include "../../GlobalVariables.h"

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

CWorkIdle::CWorkIdle()
	: CWorkStep()
{
	m_enWorkJobID = ENG_BWOK::en_work_roaming;
	m_enWorkState = ENG_JWNS::en_wait;
}

CWorkIdle::~CWorkIdle()
{

}


std::pair<double, double> CWorkIdle::Next(double dx, double dy)
{
	double nextX = x + dx;
	double nextY = y + dy;

	// X 경계 반사
	if (nextX < xMin || nextX > xMax) {
		dx *= -1;
		nextX = x + dx;
	}

	// Y 경계 반사
	if (nextY < yMin || nextY > yMax) {
		dy *= -1;
		nextY = y + dy;
	}

	x = nextX;
	y = nextY;

	return { x, y };
}


BOOL CWorkIdle::InitWork()
{
	/* 내부 멤버 변수 값 초기화 */
	if (!CWork::InitWork())	
		return FALSE;
	SetDirection(-5, -8.66);

	auto currStagePos = CommonMotionStuffs::GetInstance().GetCurrStagePos();

	double x = std::get<0>(currStagePos); double y = std::get<1>(currStagePos);

	Reset(x,y);

	return TRUE;
}

VOID CWorkIdle::DoWork()
{
	Roaming(this);
}