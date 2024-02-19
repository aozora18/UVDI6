/*
  desc : ��ȣȭ ���
*/

#include "pch.h"
#include "Crypt.h"


#ifdef _DEBUG
#define new	DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : Constructor
 parm : k_data	- [in]  ��/��ȣȭ �ϴµ� ���Ǵ� Ű ����
		k_size	- [in]  ��/��ȣȭ �ϴµ� ���Ǵ� Ű ������ ũ��
 retn : None
*/
CCrypt::CCrypt(PUINT8 k_data, UINT32 k_size)
{
	/* ��/��ȣȭ �ϴµ� ���Ǵ� Key ���� ���� */
	m_u32KeySize	= k_size;
	m_pKeyData		= (PUINT8)::Alloc(k_size+1);
	memcpy(m_pKeyData, k_data, k_size);
	m_pKeyData[k_size]	= 0x00;
	/* ��/��ȣȭ�� �����Ͱ� ����� ���� ���� */
	m_dwOutSize		= 0;
	m_pOutData		= (PUINT8)::Alloc(4096);	/* ����� ũ�� ���� */
	ASSERT(m_pOutData);
	memset(m_pOutData, 0x00, 4096);
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CCrypt::~CCrypt()
{
	/* ��/��ȣȭ Key Data �޸� ���� */
	::Free(m_pKeyData);
	/* ��/��ȣȭ ������ �޸� ���� */
	::Free(m_pOutData);
}


/*
 desc : RC4 �˰����� ����� ��ȣȭ ����
 parm : data	- [in]  ��ȣȭ�� ����� ����� ����
		size	- [in]  ��ȣȭ�� ������ ('data')�� ũ��
 retn : 0 or Error Value, or 0xffffffff�� ���, ��ȣȭ�� �������� ���۰� �ʹ� ���� �����
 note : ��� ���۴� ���ڿ��� �ƴ� ����Ʈ ��Ʈ������ �����ؾ� ��
*/
UINT32 CCrypt::Encrypt(PUINT8 data, UINT32 size)
{
	HCRYPTPROV hProv	= NULL;
	HCRYPTKEY hKey		= NULL;
	HCRYPTHASH hHash	= NULL;
	UINT32 u32Ret		= 0;

	/* ��ȣȭ�� ������ (plain data)�� ���� ũ�� */
  	m_dwOutSize	= size;

	/* ��ȣȭ�� ���� (��) �����Ͱ� �ʹ� ū�� ���� */
	if ((size * 2) > 4095)	return 0xffffffff;
	/* ������ ��ȣȭ�� ���� ��ȣȭ ���ۿ� ���� */
	memcpy(m_pOutData, data, size);  
  
	try  
	{  
		if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) == FALSE)  
			throw GetLastError();
  		if (CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash) == FALSE)
			throw GetLastError();  
  		if (CryptHashData(hHash, m_pKeyData, m_u32KeySize, 0) == FALSE)
			throw GetLastError();  
  		if (CryptDeriveKey(hProv, CALG_RC4, hHash, CRYPT_EXPORTABLE, &hKey) == FALSE)
			throw GetLastError();  
		if (CryptEncrypt(hKey, 0, TRUE, 0, m_pOutData, &m_dwOutSize, size) == FALSE)
			throw GetLastError();  
	}  
	catch(const DWORD error)  
	{  
		u32Ret	= error;
	}  
      
	if (hKey)	CryptDestroyKey(hKey);  
  	if (hHash)	CryptDestroyHash(hHash);  
  	if (hProv)	CryptReleaseContext(hProv, 0);  

	return u32Ret;  
}

/*
 desc : RC4 �˰����� ����� ��ȣȭ ����
 parm : data	- [in]  ��ȣȭ�� ����� ����� ����
		size	- [in]  ��ȣȭ�� ������ ('data')�� ũ��
 retn : 0 or Error Value, or 0xffffffff�� ���, ��ȣȭ�� �������� ���۰� �ʹ� ���� �����
 note : ��� ���۴� ���ڿ��� �ƴ� ����Ʈ ��Ʈ������ �����ؾ� ��
*/
UINT32 CCrypt::Decrypt(PUINT8 data, UINT32 size)
{
	HCRYPTPROV hProv	= NULL;
	HCRYPTKEY hKey		= NULL;
	HCRYPTHASH hHash	= NULL;
	UINT32 u32Ret		= 0;

	/* ��ȣȭ�� ������ (plain data)�� ũ�� */
	m_dwOutSize	= size;

	/* ��ȣȭ�� ���� (��) �����Ͱ� �ʹ� ū�� ���� */
	if ((size * 2) > 4095)	return 0xffffffff;
	/* ������ ��ȣȭ�� ���� ��ȣȭ ���ۿ� ���� */
	memcpy(m_pOutData, data, size);  
  
	try  
	{  
		if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) == FALSE)
			throw GetLastError();  
		if (CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash) == FALSE)
			throw GetLastError();  
  		if (CryptHashData(hHash, m_pKeyData, m_u32KeySize, 0) == FALSE)
			throw GetLastError();  
		if (CryptDeriveKey(hProv, CALG_RC4, hHash, CRYPT_EXPORTABLE, &hKey) == FALSE)
			throw GetLastError();  
		if (CryptDecrypt(hKey, 0, TRUE, 0, m_pOutData, &m_dwOutSize) == FALSE)
			throw GetLastError();  
	}  
	catch(const DWORD error)  
	{  
		u32Ret	= error;  
	}  
  
	if (hKey)	CryptDestroyKey(hKey);
	if (hHash)	CryptDestroyHash(hHash);
	if (hProv)	CryptReleaseContext(hProv, 0);

	return u32Ret;  
}