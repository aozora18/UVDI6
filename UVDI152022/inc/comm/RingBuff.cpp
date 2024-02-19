
#include "pch.h"
#include "RingBuff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
 desc : ������
 parm : None
 retn : None
*/
CRingBuff::CRingBuff()
{
	m_pRingBuff		= NULL;
	m_u32BuffSize	= 0;
	m_u32ReadPtr	= 0;
	m_u32WritePtr	= 0;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CRingBuff::~CRingBuff()
{
	Destroy();
}

/*
 desc : Ring Buffer �޸� �Ҵ�
 parm : buff_size	- [in]  ���� ũ�� (����: ����Ʈ)
 retn : 0x00 - �Ҵ� ����, 0x01 - �Ҵ� ����
*/
BOOL CRingBuff::Create(UINT32 buff_size)
{
	// �ϴ� �Ҵ�� ���� ũ�� ����
	m_u32BuffSize	= buff_size;
	// �� ���� �޸� �Ҵ�
	m_pRingBuff		= new UINT8 [buff_size+1];
	memset(m_pRingBuff, 0x00, buff_size+1);

	return m_pRingBuff ? TRUE : FALSE;
}
/*
 desc : �� ���� �޸� ����
 parm : None
 retn : None
*/
VOID CRingBuff::Destroy()
{
	if (m_pRingBuff)
	{
		delete [] m_pRingBuff;
		m_pRingBuff	= NULL;
	}
}

/*
 desc : ���� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CRingBuff::ResetBuff()
{
	if (m_pRingBuff)	memset(m_pRingBuff, 0x00, m_u32BuffSize);

	m_u32ReadPtr	= 0;
	m_u32WritePtr	= 0;
}

/*
 desc : ���� ���۷κ��� �б� ������ �������� ��(ũ��; Bytes)�� ��ȯ �Ѵ�
		��, ���� ���ۿ� ����(�����ִ�)�Ǿ� �ִ� �������� ũ�⸦ ��ȯ �Ѵ�
 parm : None
 retn : �б� ������ �������� ��(ũ��; Bytes) ��ȯ
*/
UINT32 CRingBuff::GetReadSize()
{
	if (!m_pRingBuff)	return 0;

	if (m_u32ReadPtr == m_u32WritePtr)	return 0;
	if (m_u32ReadPtr < m_u32WritePtr)	return m_u32WritePtr - m_u32ReadPtr;
	if (m_u32ReadPtr > m_u32WritePtr)	return (m_u32BuffSize - m_u32ReadPtr) + m_u32WritePtr;
	return 0;
}

/*
 desc : ���� ���� ���带 ���� ���� �ִ� ������ ��(ũ��; Bytes)�� ��ȯ �Ѵ�.
 parm : None
 retn : �����ϱ� ���� ���� �ִ� ���� ũ�� ��ȯ
*/
UINT32 CRingBuff::GetWriteSize()
{
	if (!m_pRingBuff)	return 0;

	if (m_u32ReadPtr == m_u32WritePtr)	return m_u32BuffSize - 1;
	if (m_u32ReadPtr < m_u32WritePtr)	return (m_u32BuffSize - m_u32WritePtr) + m_u32ReadPtr - 1;
	if (m_u32ReadPtr > m_u32WritePtr)	return m_u32ReadPtr - m_u32WritePtr - 1;

	return 0;
}

/*
 desc : ���� Ring Buffer�� �б� ���� ��ġ�� ������ ���
 parm : None
 retn : �б� ���� ������ ���� ������ ��ȯ
*/
UINT8 *CRingBuff::GetReadBuffPtr()
{
	return &m_pRingBuff[m_u32ReadPtr];
}

/*
 desc : ���� Ring Buffer�� ���� ���� ��ġ�� ������ ���
 parm : None
 retn : �����ϱ� ���� ������ ���� ������ ��ȯ
*/
UINT8 *CRingBuff::GetWriteBuffPtr()
{
	return &m_pRingBuff[m_u32WritePtr];
}

/*
 desc : ���� ���ۿ� ����� ������ �б� (������ ����)
 desc : data	- [in]  �о���� �����Ͱ� ����� ���� ������
		size	- [in]  �о���̰��� �ϴ� �������� ũ��
						(���� ���� ũ�⺸�� 1����Ʈ ���� ���̾�� �Ѵ�.)
 retn : 0x00 - �б� ����, 0x01 - �б� ����
*/
BOOL CRingBuff::ReadBinary(UINT8 *data, UINT32 size)
{
	if (size < 1 || size > GetReadSize())	return FALSE;

	// easy case, no wrapping
	if (m_u32ReadPtr+size <= m_u32BuffSize)
	{
		memcpy(data, &m_pRingBuff[m_u32ReadPtr], size);
		m_u32ReadPtr += size;
	}
	else // harder case, buffer wraps
	{
		UINT32 u32FirstChunkSize	= m_u32BuffSize - m_u32ReadPtr;
		UINT32 u32SecondChunkSize	= size - u32FirstChunkSize;

		memcpy(data, &m_pRingBuff[m_u32ReadPtr], u32FirstChunkSize);
		memcpy(&data[u32FirstChunkSize], &m_pRingBuff[0], u32SecondChunkSize);
		m_u32ReadPtr = u32SecondChunkSize;
	}
	return TRUE;
}

/*
 desc : ���� ���ۿ� ����� �����͸� �о �ٷ� ���� (��ȯ ����)
 desc : size	- [in]  �о ������ ������ ũ�� (���� 0 ���̸�, ��� ����)
 retn : 0x00 - �а� ������ ����, 0x01 - �о ������ ����
*/
BOOL CRingBuff::PopBinary(UINT32 size)
{
	if (size > GetReadSize())	return FALSE;

	// easy case, no wrapping
	if (m_u32ReadPtr+size <= m_u32BuffSize)
	{
		m_u32ReadPtr += size;
	}
	else // harder case, buffer wraps
	{
		UINT32 u32FirstChunkSize	= m_u32BuffSize - m_u32ReadPtr;
		UINT32 u32SecondChunkSize	= size - u32FirstChunkSize;
		m_u32ReadPtr = u32SecondChunkSize;
	}
	return TRUE;
}

/*
 desc : ���� ���ۿ� ����� �������� �д� ��ġ�� �̵� �Ѵ�. (������ ����)
 desc : size	- [in]  �о ���������ϴ� �������� ũ��
						(���� ���� ũ�⺸�� 1����Ʈ ���� ���̾�� �Ѵ�.)
 retn : 0x00 - �б� ����, 0x01 - �б� ����
*/
BOOL CRingBuff::SkipBinary(UINT32 size)
{
// 	if (size > GetMaxReadSize())	return 0x00;

	// easy case, no wrapping
	if (m_u32ReadPtr+size <= m_u32BuffSize)
	{
		m_u32ReadPtr += size;
	}
	else // harder case, buffer wraps
	{
		m_u32ReadPtr = size - (m_u32BuffSize - m_u32ReadPtr);
	}
	return 0x01;
}

/*
 desc : ���� ���ۿ� ������ ����
 parm : data	- [in]  �����ϰ��� �ϴ� �����Ͱ� ����� ������ ����
		size	- [in]  �����ϰ��� �ϴ� �������� ũ�� ��, 'data' ���ۿ� ����� �������� ũ��
 retn : 0x00 - ���� ����, 0x01 - ���� ����
*/
BOOL CRingBuff::WriteBinary(UINT8 *data, UINT32 size)
{
	if (size < 1 || size > GetWriteSize())	return FALSE;

	/* �� �̻� ������ ������ ���ٸ�, �������� ���� */
	if (GetWriteSize() <= size)	return FALSE;

	/* easy case, no wrapping */
	if (m_u32WritePtr+size <= m_u32BuffSize)
	{
		memcpy(&m_pRingBuff[m_u32WritePtr], data, size);
		m_u32WritePtr += size;
	}
	else /* harder case we need to wrap */
	{
		UINT32 u32FirstChunkSize	= m_u32BuffSize - m_u32WritePtr;
		UINT32 u32SecondChunkSize	= size - u32FirstChunkSize;

		memcpy(&m_pRingBuff[m_u32WritePtr], data, u32FirstChunkSize);
		memcpy(&m_pRingBuff[0], &data[u32FirstChunkSize], u32SecondChunkSize);
		m_u32WritePtr = u32SecondChunkSize;
	}

	return TRUE;
}

/*
 desc : ���� ���ۿ� ����� ������ ���� (������ ���� - ������ �̵� ����)
 desc : data	- [in]  �о���� �����Ͱ� ����� ���� ������
		size	- [in]  �о���̰��� �ϴ� �������� ũ��
						(���� ���� ũ�⺸�� 1����Ʈ ���� ���̾�� �Ѵ�.)
 retn : 0x00 - �б� ����, 0x01 - �б� ����
*/
BOOL CRingBuff::CopyBinary(UINT8 *data, UINT32 size)
{
//	if (size > GetMaxReadSize())	return FALSE;
	/* easy case, no wrapping */
	if (m_u32ReadPtr+size <= m_u32BuffSize)
	{
		memcpy(data, &m_pRingBuff[m_u32ReadPtr], size);
	}
	else /* harder case, buffer wraps */
	{
		UINT32 u32FirstChunkSize	= m_u32BuffSize - m_u32ReadPtr;
		UINT32 u32SecondChunkSize	= size - u32FirstChunkSize;
		memcpy(data, &m_pRingBuff[m_u32ReadPtr], u32FirstChunkSize);
		memcpy(&data[u32FirstChunkSize], &m_pRingBuff[0], u32SecondChunkSize);
	}
	return TRUE;
}

/*
 desc : ���۷κ��� �־��� ��ġ�� �����ϴ� ����(������)�� �����´�.
		 (�� ���� �����ؼ� ���������� �ʴ´�.)
 parm : pos		- [in]  ���۷κ��� ���������� �ϴ� ��ġ (zero-based)
		data	- [in]  �ش� ��ġ�� ���� ����� ����
 retn : 0x00 - �������� ����, 0x01 - �������� ����
*/
BOOL CRingBuff::PeekChar(UINT32 pos, UINT8 &data)
{
	if (pos >= GetReadSize())	return 0x00;

	if (m_u32WritePtr > m_u32ReadPtr)
	{
		// easy case, buffer doesn't wrap
		data = m_pRingBuff[m_u32ReadPtr+pos];
	}
	else if (m_u32WritePtr == m_u32ReadPtr)
	{
		// nothing in the buffer
		return 0x00;
	}
	else if (m_u32WritePtr < m_u32ReadPtr)
	{
		// harder case, buffer wraps
		if (m_u32ReadPtr+pos < m_u32BuffSize)
		{
			// pos was in first chunk
			data = m_pRingBuff[m_u32ReadPtr + pos];
		}
		else
		{
			// pos is in second chunk
			data = m_pRingBuff[pos - (m_u32BuffSize-m_u32ReadPtr)];
		}
	}

	return 0x01;
}

/*
 desc : �˻� ����� ���� 1���� ���� ���� �ִ���, �ִٸ� �ش� ��ġ�� ��ȯ (zero-based)
 parm : find	- [in]  �˻��ϰ��� �ϴ� ����
		pos		- [out] �˻�(ã��)�� ��ġ (zero-based)
 retn : 0x00 - �˻� ����, 0x01 - �˻� ����
*/
BOOL CRingBuff::FindChar(UINT8 find, UINT32 &pos)
{
	UINT8 u8GetCh;
	UINT32 i = 0;

	for (; i<GetReadSize(); i++)
	{
		if (PeekChar(i, u8GetCh))
		{
			if (u8GetCh == find)
			{
				pos = i;
				return TRUE;
			}
		}
	}

	return FALSE;
}

/*
 desc : �˻� ����� ���� 2 ���� ���� ���� �ִ���, �ִٸ� �ش� ��ġ�� ��ȯ (zero-based)
 parm : find1	- [in]  ù ��° �˻��ϰ��� �ϴ� ����
		find2	- [in]  �� ��° �˻��ϰ��� �ϴ� ����
		pos		- [out] �˻�(ã��)�� ��ġ (zero-based) (find2�� ��ġ ��ȯ)
 retn : 0x00 - �˻� ����, 0x01 - �˻� ����
*/
BOOL CRingBuff::FindChar(UINT8 find1, UINT8 find2, UINT32 &pos)
{
	UINT8 u8GetCh, u8Find[2] = { find1, find2 };
	UINT32 i = 0, j = 0;

	for (; i<GetReadSize(); i++)
	{
		if (PeekChar(i, u8GetCh))
		{
			if (u8GetCh == u8Find[j])
			{
				if (++j == 0x02)
				{
					pos = i;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}