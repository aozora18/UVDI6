#pragma once

class CCrypt
{
/* Constructor and Destructor */
public:

	CCrypt(PUINT8 k_data, UINT32 k_size);
	virtual ~CCrypt();

/* Virtual Function */
protected:

public:


/* Member Parameter : Local */
protected:

	UINT32				m_u32KeySize;		/* ��/��ȣȭ �ϴµ� ���Ǵ� Key Data ũ�� (Bytes) */
	DWORD				m_dwOutSize;		/* ������ ��/��ȣȭ�� �������� ũ�� */

	PUINT8				m_pKeyData;			/* ��/��ȣȭ �ϴµ� ���Ǵ� Key Data (Buffer) */
	PUINT8				m_pOutData;			/* ��/��ȣȭ �����Ͱ� ����� ���� (����� ũ�� ����) */


/* Member Function : Local */
protected:


/* Member Function : Public */
public:

	UINT32				Decrypt(PUINT8 data, UINT32 size);
	UINT32				Encrypt(PUINT8 data, UINT32 size);
	UINT32				GetResultSize()		{	return UINT32(m_dwOutSize);	}	/* ��ȣȭ�� �������� ũ�� */
	PUINT8				GetResultData()		{	return m_pOutData;	}			/* ��ȣȭ�� �������� ���� */
};
