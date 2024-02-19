
/*
 desc : �뱤 �۾� ���� - Direct Expose
*/

#include "pch.h"
#include "../MainApp.h"
#include "WorkExpoOnly.h"


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
CWorkExpoOnly::CWorkExpoOnly()
	: CWork()
{
	m_enWorkJobID	= ENG_BWOK::en_expo_only;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWorkExpoOnly::~CWorkExpoOnly()
{
}


/*
 desc : �ʱ� �۾� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CWorkExpoOnly::InitWork()
{
	CWork::InitWork();

	/* ��ü �۾� �ܰ� */
	m_u8WorkTotal	= 0x0d;

	return TRUE;
}

/*
 desc : �ֱ������� �۾� ����
 parm : None
 retn : None
*/
VOID CWorkExpoOnly::RunWork()
{
	switch (m_u8WorkStep)
	{
	case 0x01 : m_enWorkState = SetExpoReady();					break;
	case 0x02 : m_enWorkState = IsTrigDisabled();				break;
	/* ------------------------------------------------------------ */
	/*        Luria�� Align Mark ��� (Trigger Disabled)            */
	/* ------------------------------------------------------------ */
	case 0x03 : m_enWorkState = SetAlignMarkRegist();			break;
	case 0x04 : m_enWorkState = SetTrigDisabled();				break;
	case 0x05 : m_enWorkState = IsTrigDisabled();				break;
	case 0x06 : m_enWorkState = IsAlignMarkRegisted();			break;
	/* ------------------------------------------------------------ */
	/*                       Luria Printing                         */
	/* ------------------------------------------------------------ */
	case 0x07 : m_enWorkState = SetPrePrinting();				break;
	case 0x08 : m_enWorkState = IsPrePrinted();					break;
	case 0x09 : m_enWorkState = SetPrinting();					break;
	case 0x0a : m_enWorkState = IsPrinted();					break;
	/* Printing �����ٰ� �ؼ�, �ٷ� �������� �̵��ϸ� ���� �߻� */
	case 0x0b : m_enWorkState = IsWorkWaitTime();				break;
	/* ------------------------------------------------------------ */
	/*                   Unload ��ġ�� ���� ��Ŵ                    */
	/* ------------------------------------------------------------ */
	/* Unload ��ġ�� ���� ��Ŵ */
	case 0x0c : m_enWorkState = SetMovingUnloader();			break;
	case 0x0d : m_enWorkState = IsMovedUnloader();				break;
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
VOID CWorkExpoOnly::SetWorkNext()
{
	/* �۾��� �����̰ų� ����Ǿ��� �� */
	if (ENG_JWNS::en_error == m_enWorkState || ENG_JWNS::en_comp == m_enWorkState)
	{
		m_u8WorkStep	= 0x00;
		return;
	}

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

