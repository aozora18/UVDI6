/* --------------------------------------------------------- */
/* RingBuffer.h: Interface and implementation of CRingBuffer */
/* reference info : http://larry.antram.co/fast-ring-buffer/ */
/* --------------------------------------------------------- */

#pragma once

class CRingBuff
{
public:
	CRingBuff();
	virtual ~CRingBuff();

// ���� ����
protected:

	UINT8				*m_pRingBuff;
	UINT32				m_u32BuffSize;	// the size of the ring buffer
	UINT32				m_u32ReadPtr;	// the read pointer
	UINT32				m_u32WritePtr;	// the write pointer

// ���� �Լ�
protected:

// ���� �Լ�
public:

	BOOL				Create(UINT32 buff_size);
	VOID				Destroy();

	UINT32				GetReadSize();
	UINT32				GetWriteSize();
	BOOL				WriteBinary(UINT8 *data, UINT32 size);	/* �����͸� �߰��� (��������) */
	BOOL				CopyBinary(UINT8 *data, UINT32 size);	/* �����͸� ������, �״�� ���� */
	BOOL				SkipBinary(UINT32 size);				/* ������ �д� ��ġ�� ����(�̵� ��Ŵ, �о������ ȿ�� �߻�) */
	BOOL				ReadBinary(UINT8 *data, UINT32 size);
	BOOL				PopBinary(UINT32 size);
	BOOL				PeekChar(UINT32 pos, UINT8 &data);
	BOOL				FindChar(UINT8 find, UINT32 &pos);
	BOOL				FindChar(UINT8 find1, UINT8 find2, UINT32 &pos);
	VOID				ResetBuff();
	UINT32				GetRingBuffSize()	{	return m_u32BuffSize;	}

	/* �б�/���� ���� ������ ��ȯ */
	UINT8				*GetReadBuffPtr();
	UINT8				*GetWriteBuffPtr();
	/* �б�/���� ���� �ε��� ��ȯ */
	UINT32				GetReadBuffIndex()	{	return m_u32ReadPtr;	}
	UINT32				GetWriteBuffIndex()	{	return m_u32WritePtr;	}
};
