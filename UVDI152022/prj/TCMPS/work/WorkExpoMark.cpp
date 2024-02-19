
/*
 desc : �뱤 �۾� ���� - ����� ��ũ�� ����
*/

#include "pch.h"
#include "../MainApp.h"
#include "WorkExpoMark.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : ������
 parm : mode	- [in]  0x01 : Only Align, 0x02 : Both Align and Calibration
 retn : None
*/
CWorkExpoMark::CWorkExpoMark(ENG_AOEM mode)
	: CWork()
{
	if (ENG_AOEM::en_align_expose == mode)	m_enWorkJobID	= ENG_BWOK::en_expo_mark;
	else									m_enWorkJobID	= ENG_BWOK::en_expo_cali;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWorkExpoMark::~CWorkExpoMark()
{
}


/*
 desc : �ʱ� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkExpoMark::InitWork()
{
	CWork::InitWork();

	/* ��ü �۾� �ܰ� */
	m_u8WorkTotal	= 0x18;

	return TRUE;
}

/*
 desc : �ֱ������� �۾� ����
 parm : None
 retn : None
*/
VOID CWorkExpoMark::RunWork()
{
	switch (m_u8WorkStep)
	{
	case 0x01 : m_enWorkState = IsRegistGerberCheck();			break;
	case 0x02 : m_enWorkState = IsMotorDriveStopAll();			break;
	case 0x03 : m_enWorkState = SetTrigDisabled();				break;
	/* ------------------------------------------------------------ */
	/*               Global Mark  Scan ��ġ�� �̵�                  */
	/* ------------------------------------------------------------ */
	case 0x04 : m_enWorkState = SetAlignMovingInit();			break;
	case 0x05 : m_enWorkState = IsAlignMovedInit();				break;
	case 0x06 : m_enWorkState = IsTrigDisabled();				break;
	/* ------------------------------------------------------------ */
	/*           Trigger �߻� ��� (Area Mode) �� ����              */
	/* ------------------------------------------------------------ */
	case 0x07 : m_enWorkState = SetLuriaAreaMode();				break;
	case 0x08 : m_enWorkState = IsLuriaAreaMode();				break;
	/* ------------------------------------------------------------ */
	/* �Ź����� ��ü Mark �� ���� �� ���� Trigger ��ġ ��� �� ���� */
	/* ------------------------------------------------------------ */
	case 0x09 : m_enWorkState = SetTrigCalcGlobal4();			break;
	/* ------------------------------------------------------------ */
	/*                   Global Mark 4 points ����                  */
	/* ------------------------------------------------------------ */
	case 0x0a : m_enWorkState = SetTrigRegistGlobal4();			break;
	case 0x0b : m_enWorkState = IsTrigRegistedGlobal4();		break;
	case 0x0c : m_enWorkState = SetAlignMovingGlobal4();		break;
	case 0x0d : m_enWorkState = IsAlignMovedGlobal();			break;
	/* ------------------------------------------------------------ */
	/*        Luria�� Align Mark ��� (Trigger Disabled)            */
	/* ------------------------------------------------------------ */
	case 0x0e : m_enWorkState = SetAlignMarkRegist();			break;
	case 0x0f : m_enWorkState = SetTrigDisabled();				break;
	case 0x10 : m_enWorkState = IsTrigDisabled();				break;
	case 0x11 : m_enWorkState = IsAlignMarkRegisted();			break;
	/* ------------------------------------------------------------ */
	/*                       Luria Printing                         */
	/* ------------------------------------------------------------ */
	case 0x12 : m_enWorkState = SetPrePrinting();				break;
	case 0x13 : m_enWorkState = IsPrePrinted();					break;
	case 0x14 : m_enWorkState = SetPrinting();					break;
	case 0x15 : m_enWorkState = IsPrinted();					break;
	/* Printing �����ٰ� �ؼ�, �ٷ� �������� �̵��ϸ� ���� �߻� */
	case 0x16 : m_enWorkState = IsWorkWaitTime();				break;
	/* ------------------------------------------------------------ */
	/*                   Unload ��ġ�� ���� ��Ŵ                    */
	/* ------------------------------------------------------------ */
	/* Unload ��ġ�� ���� ��Ŵ */
	case 0x17 : m_enWorkState = SetMovingUnloader();			break;
	case 0x18 : m_enWorkState = IsMovedUnloader();				break;
	}

	/* ���� �۾� ���� ���� �Ǵ� */
	SetWorkNext();
	/* ��ð� ���� ���� ��ġ�� �ݺ� �����Ѵٸ� ���� ó�� */
	IsWorkTimeOut();
}

/*
 desc : ���� �ܰ�� �̵��ϱ� ���� ó��
 parm : None
 retn : None
*/
VOID CWorkExpoMark::SetWorkNext()
{
	/* �۾��� �����̰ų� ����Ǿ��� �� */
	if (ENG_JWNS::en_error == m_enWorkState || ENG_JWNS::en_comp == m_enWorkState)
	{
		m_u8WorkStep	= 0x00;
		return;
	}

	if (ENG_JWNS::en_next == m_enWorkState)
	{
#if 0
		if (m_u8WorkStep == 0x0d)
		{
			m_u8WorkStep	= 0x16;
		}
#else
		/* ��� �۾��� ����Ǿ��ٸ� ... */
		if (m_u8WorkStep == m_u8WorkTotal)
		{
			m_enWorkState	= ENG_JWNS::en_comp;
		}
#endif
		/* ������ ���� Step���� �̵� */
		m_u8WorkStep++;
		/* ���� �ֱٿ� Waiting �� �ð� ���� */
		m_u64DelayTime	= GetTickCount64();
	}
}
