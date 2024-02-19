
/*
 desc : ���� ��ġ���� Align Mark �ν� ����
*/

#include "pch.h"
#include "../MainApp.h"
#include "WorkMarkGrab.h"


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
CWorkMarkGrab::CWorkMarkGrab()
	: CWork()
{
	m_enWorkJobID	= ENG_BWOK::en_mark_grab;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CWorkMarkGrab::~CWorkMarkGrab()
{
}


/*
 desc : �ʱ� �۾� ����
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
 retn : TRUE or FALSE
*/
BOOL CWorkMarkGrab::InitWork(UINT8 cam_id)
{
	CWork::InitWork();

	m_u8ACamID		= cam_id;
	/* ��ü �۾� �ܰ� */
	m_u8WorkTotal	= 0x03;

	return TRUE;
}

/*
 desc : �ֱ������� �۾� ����
 parm : None
 retn : None
*/
VOID CWorkMarkGrab::RunWork()
{
	switch (m_u8WorkStep)
	{
	case 0x01 : m_enWorkState = PutOneTrigger();	break;
	case 0x02 : m_enWorkState = IsWorkWaitTime();	break;
	case 0x03 : m_enWorkState = GetGrabResult();	break;
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
VOID CWorkMarkGrab::SetWorkNext()
{
	/* �۾��� �����̰ų� ����Ǿ��� �� */
	if (ENG_JWNS::en_error == m_enWorkState || ENG_JWNS::en_comp == m_enWorkState)
	{
		m_u8WorkStep	= 0x00;
		return;
	}

	if (ENG_JWNS::en_next == m_enWorkState)
	{
		/* ���� �ܰ谡 Ʈ���� �߻��� ���, 2�� ���� ��� �� ��� �� �ޱ� ���� */
		if (0x01  == m_u8WorkStep)	SetWorkWaitTime(2000);

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

