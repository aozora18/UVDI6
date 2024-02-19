
/*
 desc : Align Mark �� ������ �� ������ �̵�
*/

#include "pch.h"
#include "../MainApp.h"
#include "WorkMarkMove.h"


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
CWorkMarkMove::CWorkMarkMove()
	: CWork()
{
	m_enWorkJobID	= ENG_BWOK::en_mark_move;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWorkMarkMove::~CWorkMarkMove()
{
}


/*
 desc : �ʱ� �۾� ����
 parm : mark_no	- [in]  �˻� ����� Mark Number (0x1 or Later)
 retn : TRUE or FALSE
*/
BOOL CWorkMarkMove::InitWork(UINT16 mark_no)
{
	CWork::InitWork();

	m_u16MarkNo		= mark_no;
	/* ��ü �۾� �ܰ� */
	m_u8WorkTotal	= 0x04;

	return TRUE;
}

/*
 desc : �ֱ������� �۾� ����
 parm : None
 retn : None
*/
VOID CWorkMarkMove::RunWork()
{
	switch (m_u8WorkStep)
	{
	case 0x01 : m_enWorkState = IsRegistGerberCheck();		break;
	case 0x02 : m_enWorkState = IsMotorDriveStopAll();		break;
	case 0x03 : m_enWorkState = SetMovingAlignMark();		break;
	case 0x04 : m_enWorkState = IsMovedAlignMark();			break;
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
VOID CWorkMarkMove::SetWorkNext()
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

