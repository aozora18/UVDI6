
/*
 desc : �뱤�� �ʿ��� ��ü ������ ����
*/

#include "pch.h"
#include "../../MainApp.h"
#include "WorkOnlyFEM.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : ������
 parm : offset	- [in]  ���� �뱤�� ��, ������ ����� ���� �� (Pixel)
						�ٸ�, 0xff�̸�, ���� ȯ�� ���Ͽ� ������ ���� �� �״�� �뱤
						Hysterisys�� Negative Pixel ��
 retn : None
*/
CWorkOnlyFEM::CWorkOnlyFEM()
	: CWorkStep()
{
	m_enWorkJobID	= ENG_BWOK::en_gerb_onlyfem;
	m_lastUniqueID	= 0;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWorkOnlyFEM::~CWorkOnlyFEM()
{
}

/*
 desc : �ʱ� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkOnlyFEM::InitWork()
{
	/* ���� ��� ���� �� �ʱ�ȭ */
	if (!CWork::InitWork())	return FALSE;

	/* ��ü �۾� �ܰ� */
	m_u8StepTotal = 0x09;

	return TRUE;
}

/*
 desc : �ֱ������� �۾� ����
 parm : None
 retn : None
*/
VOID CWorkOnlyFEM::DoWork()
{
	/* �۾� �ܰ� ���� ���� ó�� */
	switch (m_u8StepIt)
	{
	case 0x01: m_enWorkState = CheckValidRecipe();				break;
		/* Exposure Start XY */
	case 0x02: m_enWorkState = SetExposeStartXY();				break;
	case 0x03: m_enWorkState = IsExposeStartXY();				break;
		/* Photohead Z Axis Up & Down */
	case 0x04: m_enWorkState = SetPhZAxisMovingAll();			break;
	case 0x05: m_enWorkState = IsPhZAxisMovedAll();				break;
		/* Align Camera Z Axis Up & Down */
	case 0x06: m_enWorkState = SetACamZAxisMovingAll(m_lastUniqueID);			break;
	case 0x07: m_enWorkState = IsACamZAxisMovedAll(m_lastUniqueID);			break;
		/* Led Duty Cycle & Frame Rate */
	case 0x08: m_enWorkState = SetStepDutyFrame();				break;
	case 0x09: m_enWorkState = IsStepDutyFrame();				break;
// #if 1
// 	/* �۾� ���� ���� ȭ�� */
// 	case 0x0e : m_enWorkState = IsGerberJobLoaded();			break;
// #endif
	}

	/* ���� �۾� ���� ���� �Ǵ� */
	CWorkOnlyFEM::SetWorkNext();
	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	CWork::CheckWorkTimeout();
}

/*
 desc : ���� �ܰ�� �̵��ϱ� ���� ó��
 parm : None
 retn : None
*/
VOID CWorkOnlyFEM::SetWorkNext()
{
	UINT64 u64JobTime	= GetTickCount64() - m_u64StartTime;

	if (GetAbort())
	{
		CWork::EndWork();
		return;
	}

	/* �� �۾� �������� �ð� �� ���� ó�� */
	uvEng_UpdateJobWorkTime(u64JobTime);

	/* ��ġ�� ���а� ���� */
	UINT8 u8PhCount		= uvEng_GetConfig()->luria_svc.ph_count;
	UINT8 u8ACamCount	= uvEng_GetConfig()->set_cams.acam_count;

	if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* �۾��� ��� �� �ӽ� ���� */
		CWorkOnlyFEM::CalcStepRate();

		if (m_u8StepIt == m_u8StepTotal)
		{
			m_enWorkState = ENG_JWNS::en_comp;
			/* �ʱ� Luria Error �� �ʱ�ȭ */
			uvCmn_Luria_ResetLastErrorStatus();

			/* �׻� ȣ��*/
			CWork::EndWork();
		}
		else
		{
		}
		/* ���� �۾��� ó���ϱ� ���� */
		m_u8StepIt++;
		m_u64DelayTime	= GetTickCount64();
	}

	else if (ENG_JWNS::en_error == m_enWorkState)
	{
		m_enWorkState = ENG_JWNS::en_comp;
	}


}

/*
 desc : �۾� ����� ���� (percent; %)
 parm : None
 retn : None
*/
VOID CWorkOnlyFEM::CalcStepRate()
{
	UINT8 u8PhCount	= uvEng_GetConfig()->luria_svc.ph_count;
	//DOUBLE dbRate	= DOUBLE(m_u8StepIt + m_u8SetPhNo) / DOUBLE(m_u8StepTotal + u8PhCount) * 100.0f;
	DOUBLE dbRate = DOUBLE(m_u8StepIt) / DOUBLE(m_u8StepTotal) * 100.0f;
	if (dbRate > 100.0f)	dbRate = 100.0f;
	/* ���� �޸𸮿� ���� */
	uvEng_SetWorkStepRate(dbRate);
}

