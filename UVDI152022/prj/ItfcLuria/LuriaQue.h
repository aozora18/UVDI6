
#pragma once

class CLuriaQue
{
// ������ & �ı���
public:

	CLuriaQue(UINT32 count);
	~CLuriaQue();


// ���� ����
protected:

	UINT32				m_u32QueCount;	// ȯ�� ť ���� ����

	// ȯ�� ť ���� �ε���
	UINT32				m_u32Rear;		// �� �κ�
	UINT32				m_u32Front;		// �� �κ�

	LPG_PCLR			m_pstQue;		// ȯ�� ť ����

// ���� �Լ�
protected:

	UINT32				PushRearIndex();
	UINT32				PopFrontIndex();

// ���� �Լ�
public:

	BOOL				IsQueFull();
	BOOL				IsQueEmpty();

	BOOL				PushPktData(LPG_PCLR data);
	LPG_PCLR			PopPktData();
};
