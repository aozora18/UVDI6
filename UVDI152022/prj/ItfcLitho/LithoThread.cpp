
/*
 desc : PIO ���� �� ����͸� ������
*/

#include "pch.h"
#include "LithoThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : shmem	- [in]  ���� �޸�
 retn : None
*/
CLithoThread::CLithoThread(LPG_PMDV shmem)
{
	m_pstShMem	= shmem;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CLithoThread::~CLithoThread()
{
	ResetHistPIO();
}

/*
 desc : ������ ������ ��, ���� �ѹ� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLithoThread::StartWork()
{

	return TRUE;
}

/*
 desc : ������ ���� ��, ���������� ȣ���
 parm : None
 retn : None
*/
VOID CLithoThread::RunWork()
{
	/* PLC�� ����� ��츸 ���� */
	if (!uvMCQDrv_IsConnected())	return;

	/* ���� ���� �ٷ� ���� ���� ���Ͽ�, �ٸ��� ��� */
	UpdatePIO();
}

/*
 desc : ������ ����(����)�� ��, ���������� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CLithoThread::EndWork()
{
}

/*
 desc : PIO History �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CLithoThread::ResetHistPIO()
{
	/* ���� ���� */
	if (m_syncPIO.Enter())
	{
		m_lstPIO.RemoveAll();
		/* ���� ���� */
		m_syncPIO.Leave();
	}
}

/*
 desc : PIO �̺�Ʈ ���� (���� ���� ���� ���� ���Ͽ� �ٸ� ��츸 ��� ����)
 parm : None
 retn : None
*/
VOID CLithoThread::UpdatePIO()
{
	BOOL bAdded	= TRUE;

	/* ���� ���� */
	if (m_syncPIO.Enter())
	{
		STG_STSE stData	= {NULL};

		/* ���� ����Ʈ ������ ������ ���ٸ�, ���� �� �ٷ� ��� */
		if (m_lstPIO.GetCount() > 0)
		{
			/* ���� ��ϵ� ���۰� 64�� �̻�Ǹ�, �� ������ ���ۺ��� ���� */
			if (m_lstPIO.GetCount() > MAX_PIO_COUNT)	m_lstPIO.RemoveTail();
			/* ���� PIO ���� ���� ��� ���� (���� �ֱ�)�� ��ϵ� �� �� ��, �ٸ��� ��� */
			stData	= m_lstPIO.GetHead();
			bAdded	= m_pstShMem->IsEqualPIO(stData.in_track_expo.w_scan,
											 stData.out_expo_track.w_track);
		}

		/*���� �� �ٷ� ��� */
		if (bAdded)
		{
			stData.in_track_expo.w_scan		= m_pstShMem->in_track_expo.w_scan;
			stData.out_expo_track.w_track	= m_pstShMem->out_expo_track.w_track;
			/* ��� (!!! ���� : ���� �ֱ� ���� �� �տ� ��� !!!) */
			m_lstPIO.AddHead(stData);

			/* ���� ����� ���� ���� */
			if (GetConfig()->set_comn.comm_log_litho)
			{
				TCHAR tzEvent[128]	= {NULL};

				swprintf_s(tzEvent, 128, L"PODIS vs. Litho [Event] [called_func=UpdatePIO]"
										 L"[in_track_expo.w_scan=0x%04x][stData.out_expo_track.w_track=0x%04x]",
						   stData.in_track_expo.w_scan, stData.out_expo_track.w_track);
				LOG_MESG(ENG_EDIC::en_litho, tzEvent);
			}
		}

		/* ���� ���� */
		m_syncPIO.Leave();
	}
}

/*
 desc : Ư�� ��ġ�� PIO ���� �� ��ȯ (���� �ֱٺ��� ������ �߻��� ���� �� ��������)
 parm : count	- [in]  �ּ� 1 �� (���� �ֱٿ� �߻��� ������ �� �������� ���� ���� ��)
		state	- [out] PIO ���� �� ���� ('count' ������ŭ ����� ����ü ���� ������)
 retn : TRUE or FALSE (���۰� ��� �ְų�, �ش� ��ġ (Index)�� ���� ���ų� ...)
*/
BOOL CLithoThread::GetHistPIO(UINT8 count, LPG_STSE state)
{
	UINT8 i, u8Count= 0;
	BOOL bIsState	= FALSE;

	/* ���� ���� */
	if (m_syncPIO.Enter())
	{
		/* ���� ��ϵ� ���� */
		u8Count	= (UINT8)m_lstPIO.GetCount();
		if (count > MAX_PIO_COUNT)	count = MAX_PIO_COUNT;
		if (u8Count >= count && count > 0)
		{
			/* ���� Index�� �ش�Ǵ� ��ġ�� Scanner �� Track�� PIO ���� �� ��� */
			for (i=0; i<count; i++)
			{
				state[i] = m_lstPIO.GetAt(m_lstPIO.FindIndex(i));
			}
			/* ��ȯ ���� */
			bIsState = TRUE;
		}
		/* ���� ���� */
		m_syncPIO.Leave();
	}

	return bIsState;
}