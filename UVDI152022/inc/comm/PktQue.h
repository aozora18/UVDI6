
#pragma once

#include "../conf/tcpip.h"

class CPktQue
{
// ������ & �ı���
public:

	CPktQue(UINT32 count);
	~CPktQue();


// ���� ����
protected:

	UINT32				m_u32QueCount;	// ȯ�� ť ���� ����

	// ȯ�� ť ���� �ε���
	UINT32				m_u32Rear;		// �� �κ�
	UINT32				m_u32Front;		// �� �κ�

	LPG_PSRB			m_pstQue;		// ȯ�� ť ����

// ���� �Լ�
protected:

	UINT32				PushRearIndex();
	UINT32				PopFrontIndex();

// ���� �Լ�
public:

	BOOL				IsQueFull();
	BOOL				IsQueEmpty();

	BOOL				PushPktData(UINT16 cmd, UINT32 len, PUINT8 data);
	LPG_PSRB			PopPktData();

	UINT32				GetQueCount()	{	return m_u32QueCount;	}
};
