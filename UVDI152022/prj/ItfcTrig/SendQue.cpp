
/*
 desc : ��Ŷ �۽� ��� ��� ���� ȯ�� ť ��ü
*/

#include "pch.h"
#include "MainApp.h"
#include "SendQue.h"


#define QUE_BUFF_SIZE		8192

/*
 desc : ������
 parm : count	- [in]  ť ����
 retn : None
*/
CSendQue::CSendQue(UINT32 count)
{
	/* ��� ���� �ʱ�ȭ */
	m_u32Front		= 0;
	m_u32Rear		= 0;

	/* ȯ�� ť ���� ���� ���� */
	m_u32QueCount	= count;

	/* -------------------------------------- */
	/* ������ŭ ȯ�� ť ���� ũ�� �޸� �Ҵ� */
	/* -------------------------------------- */
	m_pstQue = new STG_TPQR[count];
	ASSERT(m_pstQue);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CSendQue::~CSendQue()
{
	// ȯ�� ť ���� �޸� ����
	delete [] m_pstQue;
}

/*
 desc : ȯ�� ť ���� FULL���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSendQue::IsQueFull()
{
	return (((m_u32Rear + 1) % m_u32QueCount) == m_u32Front) ? TRUE : FALSE;
}

/*
 desc : ȯ�� ť ���� Empty���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CSendQue::IsQueEmpty()
{
	return (m_u32Front == m_u32Rear) ? TRUE : FALSE;
}

/*
 desc : ��� ť ����
 parm : None
 retn : None
*/
VOID CSendQue::EmptyQue()
{
	m_u32Rear = m_u32Front;
}

/*
 desc : ť�� ���ο� �����͸� �߰��ϰ��� �ϴ� ���� (�ε���) ��ȯ
		�߿�!!! - �� �Լ� ȣ��Ǹ�, Rear ���� ������
 parm : None
 retn : ���� �߰� (����)�ϰ��� �ϴ� �迭�� �ε��� ��
*/
UINT32 CSendQue::PushRearIndex()
{
	return (m_u32Rear = (m_u32Rear + 1) % m_u32QueCount);
}

/*
 desc : ť�� ������ �����͸� ���������� �ϴ� ���� (�ε���) ��ȯ
		�߿�!!! - �� �Լ� ȣ��Ǹ�, Front ���� ������
 parm : None
 retn : ���� ��ϵ� ��ġ�� �ε��� �� ��ȯ
*/
UINT32 CSendQue::PopFrontIndex()
{
	return (m_u32Front = (m_u32Front + 1) % m_u32QueCount);
}

/*
 desc : ȯ�� ť�κ��� �۽ŵ� ��Ŷ ���� ��ȯ
 parm : None
 retn : NULL - ť�� �۽ŵ� ��Ŷ�� ���� ���
*/
LPG_TPQR CSendQue::PopData()
{
	/* ���� ť�� ��� �ִ��� Ȯ�� */
	if (IsQueEmpty())	return NULL;
	/* ������ ����� Front Index ������ �̵��ϰ�, �ش� ��ġ�� ť ������ ���� */
	return &m_pstQue[PopFrontIndex()];
}

/*
 desc : �۽ŵ� ���ۿ� ��Ŷ ����
 parm : data	- [in]  �����ϰ��� �ϴ� ������ ��
 retn : TRUE or FALSE
*/
BOOL CSendQue::PushData(LPG_TPQR data)
{
	UINT32 u32RearIdx	= 0;
	LPG_TPQR pstQue		= NULL;

	/* ���� ����ϰ��� �ϴ� ť ������ ������ Ȯ�� */
	if (IsQueFull())
	{
		uvLogs_SaveLogs(ENG_EDIC::en_trig, ENG_LNWE::en_warning, L"The send packet buffer is full",
						WFILE, WFUNC, WLINE);
		return FALSE;
	}
	/* ������ ����� Rear Index �� ���, ť ������ ���� */
	u32RearIdx	= PushRearIndex();
	pstQue		= &m_pstQue[u32RearIdx];
	/* �۽� ���� �޸� ���� */
	memcpy(pstQue, data, sizeof(STG_TPQR));
	
	return TRUE;
}

/*
 desc : ���� Queue�� ��ϵ� ���� ��ȯ
 parm : None
 retn : ���� ��ȯ
*/
UINT32 CSendQue::GetQueCount()
{
	if (m_u32Front > m_u32Rear)
	{
		return (m_u32QueCount - m_u32Front + m_u32Rear);
	}
	return m_u32Rear - m_u32Front;
}