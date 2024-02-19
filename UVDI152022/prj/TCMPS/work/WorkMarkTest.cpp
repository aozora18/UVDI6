
/*
 desc : Align Mark �˻� (��ũ ������ŭ ���������� �ν��ϴ��� �˻�)
*/

#include "pch.h"
#include "../MainApp.h"
#include "WorkMarkTest.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : ������
 parm : None
 retn : None
*/
CWorkMarkTest::CWorkMarkTest()
	: CWork()
{
	m_enWorkJobID	= ENG_BWOK::en_mark_test;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWorkMarkTest::~CWorkMarkTest()
{
}


/*
 desc : �ʱ� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkMarkTest::InitWork()
{
	CWork::InitWork();

	/* ��ü �۾� �ܰ� */
	m_u8WorkTotal	= 0x11;

	return TRUE;
}

/*
 desc : �ֱ������� �۾� ����
 parm : None
 retn : None
*/
VOID CWorkMarkTest::RunWork()
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
	case 0x0e : m_enWorkState = SetTrigDisabled();				break;
	case 0x0f : m_enWorkState = IsTrigDisabled();				break;
	/* ------------------------------------------------------------ */
	/*                   Unload ��ġ�� ���� ��Ŵ                    */
	/* ------------------------------------------------------------ */
	/* Unload ��ġ�� ���� ��Ŵ */
	case 0x10 : m_enWorkState = SetMovingUnloader();			break;
	case 0x11 : m_enWorkState = IsMovedUnloader();				break;
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
VOID CWorkMarkTest::SetWorkNext()
{
	/* �۾��� �����̰ų� ����Ǿ��� �� */
	if (ENG_JWNS::en_error == m_enWorkState || ENG_JWNS::en_comp == m_enWorkState)	return;
	if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* ��� �۾��� ����Ǿ��ٸ� ... */
		if (m_u8WorkStep == m_u8WorkTotal)
		{
			m_enWorkState	= ENG_JWNS::en_comp;
		}

		/* ������ ���� Step���� �̵� */
		m_u8WorkStep++;
		/* ���� �ֱٿ� Waiting �� �ð� ���� */
		m_u64DelayTime	= GetTickCount64();
	}
}
