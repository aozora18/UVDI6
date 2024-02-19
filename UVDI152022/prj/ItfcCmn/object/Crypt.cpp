/*
  desc : 암호화 모듈
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
 parm : k_data	- [in]  암/복호화 하는데 사용되는 키 버퍼
		k_size	- [in]  암/복호화 하는데 사용되는 키 버퍼의 크기
 retn : None
*/
CCrypt::CCrypt(PUINT8 k_data, UINT32 k_size)
{
	/* 암/복호화 하는데 사용되는 Key 정보 설정 */
	m_u32KeySize	= k_size;
	m_pKeyData		= (PUINT8)::Alloc(k_size+1);
	memcpy(m_pKeyData, k_data, k_size);
	m_pKeyData[k_size]	= 0x00;
	/* 암/복호화된 데이터가 저장될 버퍼 생성 */
	m_dwOutSize		= 0;
	m_pOutData		= (PUINT8)::Alloc(4096);	/* 충분히 크게 잡자 */
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
	/* 암/복호화 Key Data 메모리 해제 */
	::Free(m_pKeyData);
	/* 암/복호화 데이터 메모리 해제 */
	::Free(m_pOutData);
}


/*
 desc : RC4 알고리즘을 사용한 암호화 진행
 parm : data	- [in]  암호화될 대상이 저장된 버퍼
		size	- [in]  암호화될 데이터 ('data')의 크기
 retn : 0 or Error Value, or 0xffffffff인 경우, 암호화될 데이터의 버퍼가 너무 작은 경우임
 note : 모든 버퍼는 문자열이 아닌 바이트 스트림으로 간주해야 함
*/
UINT32 CCrypt::Encrypt(PUINT8 data, UINT32 size)
{
	HCRYPTPROV hProv	= NULL;
	HCRYPTKEY hKey		= NULL;
	HCRYPTHASH hHash	= NULL;
	UINT32 u32Ret		= 0;

	/* 암호화될 데이터 (plain data)의 원문 크기 */
  	m_dwOutSize	= size;

	/* 암호화될 원문 (평문) 데이터가 너무 큰지 여부 */
	if ((size * 2) > 4095)	return 0xffffffff;
	/* 원문을 암호화를 위해 암호화 버퍼에 복사 */
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
 desc : RC4 알고리즘을 사용한 복호화 진행
 parm : data	- [in]  복호화될 대상이 저장된 버퍼
		size	- [in]  복호화될 데이터 ('data')의 크기
 retn : 0 or Error Value, or 0xffffffff인 경우, 복호화될 데이터의 버퍼가 너무 작은 경우임
 note : 모든 버퍼는 문자열이 아닌 바이트 스트림으로 간주해야 함
*/
UINT32 CCrypt::Decrypt(PUINT8 data, UINT32 size)
{
	HCRYPTPROV hProv	= NULL;
	HCRYPTKEY hKey		= NULL;
	HCRYPTHASH hHash	= NULL;
	UINT32 u32Ret		= 0;

	/* 복호화될 데이터 (plain data)의 크기 */
	m_dwOutSize	= size;

	/* 암호화될 원문 (평문) 데이터가 너무 큰지 여부 */
	if ((size * 2) > 4095)	return 0xffffffff;
	/* 원문을 암호화를 위해 암호화 버퍼에 복사 */
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