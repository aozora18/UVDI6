#pragma once

class CCamInst : public Camera_t
{
// ������ �� �ı���
public:

	CCamInst(UINT8 cam_id);
	virtual ~CCamInst();

// �����Լ� ������
protected:

public:


// ���� ����
protected:

	UINT8				m_u8CamIndex;	/* Align Camera Index (1 or Later) */

// ���� �Լ�
protected:


// ���� �Լ�
public:

	UINT8				GetCamIndex()	{	return m_u8CamIndex;	}
};
