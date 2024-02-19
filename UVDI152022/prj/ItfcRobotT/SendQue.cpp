
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
	// ��� ���� �ʱ�ȭ
	m_u32Front		= 0;
	m_u32Rear		= 0;

	// ȯ�� ť ���� ���� ����
	m_u32QueCount	= count;

	/* ������ŭ ȯ�� ť ���� ũ�� �޸� �Ҵ� */
	m_pstQue = new STG_PESR[count];
	ASSERT(m_pstQue);
	memset(m_pstQue, 0x00, sizeof(STG_PESR)*count);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CSendQue::~CSendQue()
{
	/* ȯ�� ť ���� �޸� ���� */
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
LPG_PESR CSendQue::PopData()
{
	/* ���� ť�� ��� �ִ��� Ȯ�� */
	if (IsQueEmpty())	return NULL;
	/* ������ ����� Front Index ������ �̵��ϰ�, �ش� ��ġ�� ť ������ ���� */
	return &m_pstQue[PopFrontIndex()];
}

/*
 desc : ��Ŷ �۽� ���� ����
 parm : cmd		- [in]  �۽� ��ɾ�
		data	- [in]  �����͸� �����ϰ� �ִ� �޸� ������
		size	- [in]  'data' ���ۿ� ����� �������� ũ��
 retn : TRUE or FALSE
*/
BOOL CSendQue::PushData(UINT16 cmd, PUINT8 data, UINT16 size)
{
	UINT16 u16PktSize	= 0;
	UINT32 u32RearIdx	= 0;
	STG_UTPH stHead		= {NULL};
	STG_UTPT stTail		= {NULL};
	PUINT8 pPktData		= NULL;
	LPG_PESR pstQue		= NULL;

	/* ���� ����ϰ��� �ϴ� ť ������ ������ Ȯ�� */
	if (IsQueFull())
	{
		LOG_ERROR(L"There is not enough space in the queue. It is full.");
		return FALSE;
	}

	/* Header */
	stHead.head		= 0xaa99;	/* TES vs. CMPS */
	stHead.cmd		= cmd;
	stHead.length	= u16PktSize = sizeof(STG_UTPH) + size + sizeof(STG_UTPT);
	/* Tailer */
	stTail.crc		= 0x00;
	stTail.tail		= 0xffaa;	/* TES vs. CMPS */
	/* Big Endian */
	stHead.SwapHostToNetwork();
	stTail.SwapHostToNetwork();

	// ������ ����� Rear Index �� ���, ť ������ ����
	u32RearIdx	= PushRearIndex();
	pstQue		= &m_pstQue[u32RearIdx];
	pstQue->size= u16PktSize;

	/* ��Ŷ ��� ���� */
	pPktData	= pstQue->data;
	memcpy(pPktData, &stHead, sizeof(STG_UTPH));
	pPktData += sizeof(STG_UTPH);
	/* ��Ŷ ���� ���� */
	if (data)
	{
		memcpy(pPktData, data, size);
		pPktData += size;
	}
	/* ��Ŷ ���� ���� */
	memcpy(pPktData, &stTail, sizeof(STG_UTPT));
	
	return TRUE;
}
