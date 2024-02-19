/*
 desc : Utility ���
*/

#include "pch.h"
#include "Utility.h"

#include <comdef.h>

#ifdef _DEBUG
#define new	DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif


/*
 desc : Constructor
 parm : None
 retn : None
*/
CUtility::CUtility()
{
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CUtility::~CUtility()
{
}

/*
 desc : micro �������� wait ��Ŵ
 parm : parm : usec	- [in]  ���(Sleep) �ϰ��� �ϴ� �ð� (����: micro-second = 1 / 1000000 ��)
 retn : None
*/
VOID CUtility::uSleep(UINT64 usec)
{
	// Microsecond (1 / 1000000��)���� �����ϱ� ���� ���� Type
	LARGE_INTEGER perf, start, now;

	/* ----------------------------------------------------------------------------- */
	/* GetTickCount�Լ��� 10 msec �̻� ���� �߻�, �� ���� �� ��Ȯ�� �Լ��� �Ʒ� �Լ� */
	/* ----------------------------------------------------------------------------- */
	if (!QueryPerformanceFrequency(&perf))	return Sleep((UINT32)usec);
	if (!QueryPerformanceCounter(&start))	return Sleep((UINT32)usec);

	do {

		QueryPerformanceCounter((LARGE_INTEGER*)&now);

	  /* 1 ����ũ�� �� (1 / 1000000) ���� ���Ͽ�, �ʰ��Ǿ����� ���� ���� ���� */
	} while ((now.QuadPart - start.QuadPart) / (perf.QuadPart / 1000000.0f) <= usec);
}

/*
 desc : LOGFONT�� CHARFORMAT���� �����ؼ� ��ȯ
 parm : lf	- [in]  LOGFONT ������ ����� ����ü ����
		cf	- [out] CHARFORMAT���� ����� ����ü ���� ����
 retn : None
*/
VOID CUtility::Logfont2Charformat(LOGFONT lf, CHARFORMAT2 &cf)
{
	HDC hDC	= NULL;
	LONG32 l32PixPerInch;

	cf.dwEffects	= CFM_EFFECTS | CFE_AUTOBACKCOLOR;
	cf.dwEffects	&= ~(CFE_PROTECTED | CFE_LINK| CFM_COLOR);

	// Set CHARFORMAT structure
	cf.cbSize	= sizeof(CHARFORMAT2);
	hDC			= ::GetDC(::GetDesktopWindow());
	if (hDC)
	{
		l32PixPerInch	= ::GetDeviceCaps(hDC, LOGPIXELSY);
		cf.yHeight		= abs(lf.lfHeight * 72 * 20 / l32PixPerInch);
		::ReleaseDC(::GetDesktopWindow(), hDC);
	}

	cf.yOffset		= 0;
//	cf.crTextColor	= 0;

	if (lf.lfWeight < FW_BOLD)	cf.dwEffects &= ~CFE_BOLD;
	if (!lf.lfItalic)			cf.dwEffects &= ~CFE_ITALIC;
	if (!lf.lfUnderline)		cf.dwEffects &= ~CFE_UNDERLINE;
	if (!lf.lfStrikeOut)		cf.dwEffects &= ~CFE_STRIKEOUT;

//	cf.dwMask			= CFM_ALL | CFM_BACKCOLOR | CFM_OFFSET;
	cf.dwMask			= CFM_ALL | CFM_BACKCOLOR;
	cf.bCharSet			= lf.lfCharSet;
	cf.bPitchAndFamily	= lf.lfPitchAndFamily;

#ifndef UNICODE
	strcpy_s(cf.szFaceName, LF_FACESIZE, lf.lfFaceName);
#else
	wcscpy_s(cf.szFaceName, LF_FACESIZE, lf.lfFaceName);
#endif
}

/*
 desc : Converting CComBSTR to TCHAR
 parm : source	- [in]  CComBSTR String
		target	- [out] TCHAR String ('source' ���ۺ��� ������ ū ������ ������ �־�� ��)
		t_size	- [in]  'target' buffer size
 retn : TRUE or FALSE (Target Buffer ũ�Ⱑ �ʹ� ���� ���)
*/
BOOL CUtility::CComBSTR2TCHAR(BSTR source, PTCHAR target, UINT32 size)
{
	_bstr_t bstrSource;
	if (!source || !target || size < 1)	return FALSE;

	/* _bstr_t Ŭ������ ��ȯ */
	bstrSource	= _bstr_t(source);
	wcscpy_s(target, size, bstrSource);
	
	return TRUE;
}

/*
 desc : Ŭ�����忡 ����� ������ �ӽ� ������ �д�
 parm : data	- [in]  Ŭ�����忡 �����Ϸ��� �����Ͱ� ����� ������
		size	- [in]  'data' ���ۿ� ����� ũ��
 retn : TRUE or FALSE
*/
BOOL CUtility::CopyToClipboard(PTCHAR data, UINT32 size)
{
	BOOL bSucc	= FALSE;
	PTCHAR pData	= NULL;
	HGLOBAL hMem	= NULL;

	// Clipboard ����
	if (!OpenClipboard(NULL))	return FALSE;
	// ���� Ŭ������ �ʱ�ȭ
	if (EmptyClipboard())
	{
		// Ŭ�����忡 �ӽ÷� ������ �������� �޸� ���� Ȯ��
		hMem = GlobalAlloc(GMEM_DDESHARE, size+1);	// �������� ũ�⺸�� �׻� 1 ũ��
		if (hMem)
		{
			// Ŭ�������� �޸� ���� Lock !!!
			pData = (TCHAR*)GlobalLock(hMem);
			if (pData)
			{
				// ���� �޸𸮿� �����ϰ��� �ϴ� ������ ����
				wcscpy_s(pData, size+1, data);
				// Ŭ�� ���忡 ���� �޸��� �����͸� ����
				SetClipboardData(CF_OEMTEXT, hMem);

				// Ŭ�������� �޸� ���� Unlock !!!
				GlobalUnlock(hMem);
			}
		}
		// ���� �޸� ��ȯ
		GlobalFree(hMem);
	}
	// Clipboard �ݱ�
	CloseClipboard();

	return bSucc;
}

/*
 desc : 10 ������ 2 ������ ��ȯ �� ���ڿ� (����)�� ����
 parm : dec_num	- [in]  2 ������ ��ȯ�ϰ��� �ϴ� 10 ���� ��
		bin_num - [out] 2 ������ ��ȯ�� �����Ͱ� ����� 0 or 1�� ���ڸ� �����ϱ� ���� ����
						(����� Ŀ�� ��. �ּ� 32 ~ 255)
		bin_size- [in]  'bin_str' ������ ũ��
 retn : bin_size ���� 32 ~ 256 ���� ���� �ƴϸ� FALSE ��ȯ
*/
BOOL CUtility::DecimalNumToBinaryStr(UINT64 dec_num, PUINT8 bin_num, UINT8 bin_size)
{
	if (dec_num < 1 || bin_size < 32 || bin_size > 256)	return FALSE;

	/* ���� ���� �ʱ�ȭ */
	memset(bin_num, 0x00, bin_size);
#if 0
	UINT8 i	= 0x00, u8Temp = 0x00, u8Count = 0x00, u8Reverse;
	/* �������� ����� */
	for (i=0; dec_num>0; i++)
	{
		bin_num[i]	= (dec_num % 2);
		dec_num		/= 2;
		u8Count++;
	}
	/* ���� ���� */
	for (i=0; i<u8Count/2; i++)
	{
		u8Reverse			= (u8Count - 1) - i;
		u8Temp				= bin_num[i];
		bin_num[i]			= bin_num[u8Reverse];
		bin_num[u8Reverse]	= u8Temp;
	}
#else
	UINT8 i	= 0x00;
	/* �������� ����� */
	for (i=0; dec_num>0; i++)
	{
		bin_num[i]	= (dec_num % 2);
		dec_num		/= 2;
	}
#endif
	return TRUE;
}

/*
 desc : memset ��ü �Լ� (�ӵ��� �� ������ ���� ...)
 parm : dest	- [in]  �ʱ�ȭ ��� ���� ������
		count	- [in]  �ʱ�ȭ ����
 retn : None
*/
VOID CUtility::Memset(PVOID dest, UINT64 count)
{
	UINT64 u64Index	= 0;
	UINT64 blocks	= count >> 3;
	UINT64 u64Left	= count - (blocks << 3);

	UINT64 *pPtr8 = (UINT64*)dest;
	for (; u64Index<blocks; u64Index++)	pPtr8[u64Index] = 0x0000000000000000;
	if (!u64Left)	return;

	blocks = u64Left >> 2;
	u64Left = u64Left - (blocks << 2);

	UINT32 *pPtr4 = (UINT32*)&pPtr8[u64Index];
	for (u64Index=0; u64Index<blocks; u64Index++)	pPtr4[u64Index] = 0x00000000;
	if (!u64Left)	return;

	blocks = u64Left >> 1;
	u64Left = u64Left - (blocks << 1);

	UINT16 *pPtr2 = (UINT16*)&pPtr4[u64Index];
	for (u64Index=0; u64Index<blocks; u64Index++)	pPtr2[u64Index] = 0x0000;
	if (!u64Left)	return;

	UINT8 *pPtr1 = (UINT8*)&pPtr2[u64Index];
	for (u64Index=0; u64Index<u64Left; u64Index++)	pPtr1[u64Index] = 0x00;
}

/*
 desc : memset ��ü �Լ� (8 Bytes ������ �ʱ�ȭ)
 parm : dest	- [in]  �ʱ�ȭ ��� ���� ������
		count	- [in]  �ʱ�ȭ ����
 retn : None
*/
VOID CUtility::Memset64(PVOID dest, UINT64 count)
{
	UINT64 i, *p64Dest	= (UINT64*)dest;
	for (i=0; i<count/sizeof(UINT64); i++)	p64Dest[i]	= 0;
	UINT8 *p8Dest	= (UINT8*)dest + (count - count & 7);	// (count % sizeof(UINT64)) == (count & 7))
	for (i=0; i<(count & 7); i++)	p8Dest[i]	= 0;
}

/*
 desc : memset ��ü �Լ� (SIMD_128 �̿�)
		SIMD(128)�̰�, ����(dest)�� 16 bytes�� Align�Ǿ� �־�߸� ���� �����Ѵ�
 parm : dest	- [in]  �ʱ�ȭ ��� ���� ������
		count	- [in]  dest ���� ũ�⸦ 16 bytes�� ���� ���� ���� ��
 retn : None
*/
VOID CUtility::MemsetS128(PVOID dest, UINT32 count)
{
	__declspec(align(16)) UINT32 u32Zero[4] = {0,};
	UINT64 i;

	// SIMD ������ ����
	__m128i* pDest	= (__m128i*)dest;
	__m128i mClear	= _mm_load_si128((__m128i*)u32Zero);
	// 16 ����Ʈ�� �̵��ϸ鼭 �޸� �ʱ�ȭ
	for (i=0; i<count/8; i+=8)
	{
		_mm_stream_si128(&pDest[i], mClear);
		_mm_stream_si128(&pDest[i+1], mClear);
		_mm_stream_si128(&pDest[i+2], mClear);
		_mm_stream_si128(&pDest[i+3], mClear);
		_mm_stream_si128(&pDest[i+4], mClear);
		_mm_stream_si128(&pDest[i+5], mClear);
		_mm_stream_si128(&pDest[i+6], mClear);
		_mm_stream_si128(&pDest[i+7], mClear);
	}
}

/*
 desc : ��Ʈ �̸��� ���� ����ü ���� ��ȯ
 parm :	lf		- [in]  ��ȯ�Ǿ� ����� �α� ��Ʈ ����ü
		name	- [in]  ��Ʈ �̸�
		size	- [in]  ��Ʈ ũ��
		lang	- [in]  ���� ��� �ڵ�
 retn : TRUE or FALSE
*/
BOOL CUtility::GetLogFont(LOGFONT &lf, PTCHAR name, UINT32 size, UINT8 lang)
{
	if (_tcslen(name) >= LF_FACESIZE)	return FALSE;

	/* �⺻ ��Ʈ ���� ��� */
	if (!::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lf, 0))
		return FALSE;
	lf.lfCharSet	= lang;			/* ���� �ڵ� */
	lf.lfHeight		= LONG(size);	/* ��Ʈ ũ�� */
	/* ��Ʈ �� ���� */
	swprintf_s(lf.lfFaceName, LF_FACESIZE, name);

	return TRUE;
}

/*
 desc : �迭 (����Ʈ) �������� ��ȯ�Ͽ� ����
 parm : data	- [out] �迭 ������ (�Է� : ����, ��� : ���� �迭)
		size	- [in]  �迭 ����
 retn : None
*/
VOID CUtility::GetReverseArrayUint8(PUINT8 data, UINT32 size)
{
	UINT8 u8Temp= 0x00;
	UINT32 i	= 0;

	if (size < 1)	return;

	for (; i<size/2; i++)
	{
		u8Temp	= data[i];
		data[i]	= data[(size - 1) - i];
		data[(size - 1) - i]	= u8Temp;
	}
}
