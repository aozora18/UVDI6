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

	UINT32				m_u32KeySize;		/* 암/복호화 하는데 사용되는 Key Data 크기 (Bytes) */
	DWORD				m_dwOutSize;		/* 생성된 암/복호화된 데이터의 크기 */

	PUINT8				m_pKeyData;			/* 암/복호화 하는데 사용되는 Key Data (Buffer) */
	PUINT8				m_pOutData;			/* 암/복호화 데이터가 저장될 버퍼 (충분히 크게 설정) */


/* Member Function : Local */
protected:


/* Member Function : Public */
public:

	UINT32				Decrypt(PUINT8 data, UINT32 size);
	UINT32				Encrypt(PUINT8 data, UINT32 size);
	UINT32				GetResultSize()		{	return UINT32(m_dwOutSize);	}	/* 암호화된 데이터의 크기 */
	PUINT8				GetResultData()		{	return m_pOutData;	}			/* 암호화된 데이터의 버퍼 */
};
