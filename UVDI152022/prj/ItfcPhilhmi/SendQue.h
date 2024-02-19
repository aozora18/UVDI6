
#pragma once

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

	LPG_PP_PHILHMI		m_pstQue;		// ȯ�� ť ����

// ���� �Լ�
protected:


	UINT32				PushRearIndex();
	UINT32				PopFrontIndex();	// Front Index �� ���� ��


// ���� �Լ�
public:

	BOOL				PushData(LPG_PP_PHILHMI data);
	LPG_PP_PHILHMI		PopData();	// Front Index �� ���� ��

	/* �۽��� �����Ͱ� �ִ��� ���� */
	BOOL				IsQueFull();
	BOOL				IsQueEmpty();
	/* Queue ��� ���� */
	VOID				EmptyQue();

	UINT32				GetQueCount();
};