
/*
 desc : ��� ������ ������ ����ó��
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

	// X ��� �ݻ�
	if (nextX < xMin || nextX > xMax) {
		dx *= -1;
		nextX = x + dx;
	}

	// Y ��� �ݻ�
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
	/* ���� ��� ���� �� �ʱ�ȭ */
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