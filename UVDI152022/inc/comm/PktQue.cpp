
/*
 desc : ��Ŷ ���� ��� ��� ���� ȯ�� ť ��ü
*/

#include "pch.h"
#include "PktQue.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


#define QUE_BUFF_SIZE		1024 * 1024 * 8	// About 4 MBytes �޸� �Ҵ�


/*
 desc : ������
 parm : count	- [in]  ť ����
 retn : None
*/
CPktQue::CPktQue(UINT32 count)
{
	UINT32 i	= 0;
	// ��� ���� �ʱ�ȭ
	m_u32Front	= 0;
	m_u32Rear	= 0;
	// ȯ�� ť ���� ���� ����
	m_u32QueCount	= count;

	/* -------------------------------------- */
	/* ������ŭ ȯ�� ť ���� ũ�� �޸� �Ҵ� */
	/* -------------------------------------- */
	m_pstQue = new STG_PSRB[count];
	ASSERT(m_pstQue);
	memset(m_pstQue, 0x00, sizeof(STG_PSRB)*count);
	for (; i<count; i++)
	{
		m_pstQue[i].data	= new UINT8 [QUE_BUFF_SIZE];
		memset(m_pstQue[i].data, 0x00, QUE_BUFF_SIZE);
	}
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CPktQue::~CPktQue()
{
	UINT32 i	= 0;

	// ȯ�� ť ���� �޸� ����
	for (; i<m_u32QueCount; i++)	delete [] m_pstQue[i].data;
	delete [] m_pstQue;
}

/*
 desc : ȯ�� ť ���� FULL���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CPktQue::IsQueFull()
{
	return (((m_u32Rear + 1) % m_u32QueCount) == m_u32Front) ? TRUE : FALSE;
}

/*
 desc : ȯ�� ť ���� Empty���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CPktQue::IsQueEmpty()
{
	return (m_u32Front == m_u32Rear) ? TRUE : FALSE;
}

/*
 desc : ť�� ���ο� �����͸� �߰��ϰ��� �ϴ� ���� (�ε���) ��ȯ
		�߿�!!! - �� �Լ� ȣ��Ǹ�, Rear ���� ������
 parm : None
 retn : ���� �߰� (����)�ϰ��� �ϴ� �迭�� �ε��� ��
*/
UINT32 CPktQue::PushRearIndex()
{
	return (m_u32Rear = (m_u32Rear + 1) % m_u32QueCount);
}

/*
 desc : ť�� ������ �����͸� ���������� �ϴ� ���� (�ε���) ��ȯ
		�߿�!!! - �� �Լ� ȣ��Ǹ�, Front ���� ������
 parm : None
 retn : ���� ����(����)�ϰ��� �ϴ� �迭�� �ε��� ��
*/
UINT32 CPktQue::PopFrontIndex()
{
	return (m_u32Front = (m_u32Front + 1) % m_u32QueCount);
}

/*
 desc : ȯ�� ť�� ���ŵ� ��Ŷ ���� ���
 parm : cmd	- [in]  ���ŵ� ��Ŷ ��ɾ�
		len	- [in]  ���ŵ� ��Ŷ ũ��
		data- [in]  ���ŵ� ��Ŷ ������
 retn : TRUE or FALSE
*/
BOOL CPktQue::PushPktData(UINT16 cmd, UINT32 len, PUINT8 data)
{
	UINT32 u32RearIdx	= 0;
	LPG_PSRB pstData	= NULL;

	/* ���ŵ� ��Ŷ �����Ͱ� �ʹ� ũ�� ������� ���� */
	if (len > QUE_BUFF_SIZE-1)
	{
		return FALSE;
	}

	/* ���� ����ϰ��� �ϴ� ť ������ ������ Ȯ�� */
	if (IsQueFull())
	{
		return FALSE;
	}
	/* ������ ����� Rear Index �� ���, ť ������ ���� */
	u32RearIdx	= PushRearIndex();
	pstData		= &m_pstQue[u32RearIdx];
	/* ��Ŷ �����͸� ť�� ��� */
	pstData->cmd= cmd;
	pstData->len= len;
	if (len > 0)	memcpy(pstData->data, data, len);

	return TRUE;
}

/*
 desc : ȯ�� ť�κ��� ���ŵ� ��Ŷ ���� ��ȯ
 parm : None
 retn : NULL - ť�� ���ŵ� ��Ŷ�� ���� ���
*/
LPG_PSRB CPktQue::PopPktData()
{
	UINT32 u32FrontIdx	= 0;

	// ���� ť�� ��� �ִ��� Ȯ��
	if (IsQueEmpty())	return NULL;
	// ������ ����� Rear Index �� ���, ť ������ ����
	u32FrontIdx	= PopFrontIndex();
	return &m_pstQue[u32FrontIdx];
}
