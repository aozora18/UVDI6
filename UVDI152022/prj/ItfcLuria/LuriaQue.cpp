/*
 desc : ��Ŷ ���� ��� ��� ���� ȯ�� ť ��ü
*/

#include "pch.h"
#include "LuriaQue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

/*
 desc : ������
 parm : count	- [in]  ť ����
 retn : None
*/
CLuriaQue::CLuriaQue(UINT32 count)
{
	/* ��� ���� �ʱ�ȭ */
	m_u32Front		= 0;
	m_u32Rear		= 0;
	/* ȯ�� ť ���� ���� ���� */
	m_u32QueCount	= count;

	/* ������ŭ ȯ�� ť ���� ũ�� �޸� �Ҵ� */
	m_pstQue = new STG_PCLR[count];
	ASSERT(m_pstQue);
	memset(m_pstQue, 0x00, sizeof(STG_PCLR)*count);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CLuriaQue::~CLuriaQue()
{
	/* ȯ�� ť ���� �޸� ���� */
	if (m_pstQue)	delete[] m_pstQue;
}

/*
 desc : ȯ�� ť ���� FULL���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaQue::IsQueFull()
{
	return (((m_u32Rear + 1) % m_u32QueCount) == m_u32Front) ? TRUE : FALSE;
}

/*
 desc : ȯ�� ť ���� Empty���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CLuriaQue::IsQueEmpty()
{
	return (m_u32Front == m_u32Rear) ? TRUE : FALSE;
}

/*
 desc : ť�� ���ο� �����͸� �߰��ϰ��� �ϴ� ���� (�ε���) ��ȯ
		�߿�!!! - �� �Լ� ȣ��Ǹ�, Rear ���� ������
 parm : None
 retn : ���� �߰� (����)�ϰ��� �ϴ� �迭�� �ε��� ��
*/
UINT32 CLuriaQue::PushRearIndex()
{
	return (m_u32Rear = (m_u32Rear + 1) % m_u32QueCount);
}

/*
 desc : ť�� ������ �����͸� ���������� �ϴ� ���� (�ε���) ��ȯ
		�߿�!!! - �� �Լ� ȣ��Ǹ�, Front ���� ������
 parm : None
 retn : ���� ����(����)�ϰ��� �ϴ� �迭�� �ε��� ��
*/
UINT32 CLuriaQue::PopFrontIndex()
{
	return (m_u32Front = (m_u32Front + 1) % m_u32QueCount);
}

/*
 desc : ȯ�� ť�� ���ŵ� ��Ŷ ���� ���
 parm : data- [in]  ���ŵ� ��Ŷ ������
 retn : TRUE or FALSE
*/
BOOL CLuriaQue::PushPktData(LPG_PCLR data)
{
	UINT32 u32RearIdx = 0;
	LPG_PCLR pstData = NULL;

	/* ���� ����ϰ��� �ϴ� ť ������ ������ Ȯ�� */
	if (IsQueFull())
	{
		LOG_ERROR(ENG_EDIC::en_luria, L"There is not enough space in the queue. It is full.");
		return FALSE;
	}
	/* ������ ����� Rear Index �� ���, ť ������ ���� */
	u32RearIdx = PushRearIndex();
	pstData = &m_pstQue[u32RearIdx];
	/* ��Ŷ �����͸� ť�� ��� */
	memcpy(pstData, data, sizeof(STG_PCLR));

	return TRUE;
}

/*
 desc : ȯ�� ť�κ��� ���ŵ� ��Ŷ ���� ��ȯ
 parm : None
 retn : NULL - ť�� ���ŵ� ��Ŷ�� ���� ���
*/
LPG_PCLR CLuriaQue::PopPktData()
{
	UINT32 u32FrontIdx = 0;

	/* ���� ť�� ��� �ִ��� Ȯ�� */
	if (IsQueEmpty())	return NULL;
	/* ������ ����� Rear Index �� ���, ť ������ ���� */
	u32FrontIdx = PopFrontIndex();
	return &m_pstQue[u32FrontIdx];
}