
#pragma once

#include "../../inc/conf/efem.h"

class CSendQue
{
// ������ & �ı���
public:

	CSendQue(UINT32 count);
	~CSendQue();


// ���� ����
protected:

	UINT32				m_u32QueCount;	// ȯ�� ť ���� ����

	// ȯ�� ť ���� �ε���
	UINT32				m_u32Rear;		// �� �κ�
	UINT32				m_u32Front;		// �� �κ�

	LPG_PESR			m_pstQue;		// ȯ�� ť ����

// ���� �Լ�
protected:


	UINT32				PushRearIndex();
	UINT32				PopFrontIndex();	// Front Index �� ���� ��


// ���� �Լ�
public:

	BOOL				PushData(UINT16 cmd, PUINT8 data, UINT16 size);
	LPG_PESR			PopData();	// Front Index �� ���� ��

	/* �۽��� �����Ͱ� �ִ��� ���� */
	BOOL				IsQueFull();
	BOOL				IsQueEmpty();
	// Queue ��� ����
	VOID				EmptyQue();
};