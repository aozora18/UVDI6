/*
 desc : Utility 모듈
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
 desc : micro 단위까지 wait 시킴
 parm : parm : usec	- [in]  대기(Sleep) 하고자 하는 시간 (단위: micro-second = 1 / 1000000 초)
 retn : None
*/
VOID CUtility::uSleep(UINT64 usec)
{
	// Microsecond (1 / 1000000초)까지 측정하기 위한 변수 Type
	LARGE_INTEGER perf, start, now;

	/* ----------------------------------------------------------------------------- */
	/* GetTickCount함수는 10 msec 이상 오차 발생, 이 보다 더 정확한 함수는 아래 함수 */
	/* ----------------------------------------------------------------------------- */
	if (!QueryPerformanceFrequency(&perf))	return Sleep((UINT32)usec);
	if (!QueryPerformanceCounter(&start))	return Sleep((UINT32)usec);

	do {

		QueryPerformanceCounter((LARGE_INTEGER*)&now);

	  /* 1 마이크로 초 (1 / 1000000) 동안 비교하여, 초과되었으면 루프 빠져 나감 */
	} while ((now.QuadPart - start.QuadPart) / (perf.QuadPart / 1000000.0f) <= usec);
}

/*
 desc : LOGFONT를 CHARFORMAT으로 변경해서 반환
 parm : lf	- [in]  LOGFONT 정보가 저장된 구조체 변수
		cf	- [out] CHARFORMAT으로 저장될 구조체 참조 변수
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
		target	- [out] TCHAR String ('source' 버퍼보다 무조건 큰 공간을 가지고 있어야 됨)
		t_size	- [in]  'target' buffer size
 retn : TRUE or FALSE (Target Buffer 크기가 너무 작은 경우)
*/
BOOL CUtility::CComBSTR2TCHAR(BSTR source, PTCHAR target, UINT32 size)
{
	_bstr_t bstrSource;
	if (!source || !target || size < 1)	return FALSE;

	/* _bstr_t 클래스로 변환 */
	bstrSource	= _bstr_t(source);
	wcscpy_s(target, size, bstrSource);
	
	return TRUE;
}

/*
 desc : 클립보드에 복사된 정보를 임시 저장해 둔다
 parm : data	- [in]  클립보드에 복사하려는 데이터가 저장된 포인터
		size	- [in]  'data' 버퍼에 저장된 크기
 retn : TRUE or FALSE
*/
BOOL CUtility::CopyToClipboard(PTCHAR data, UINT32 size)
{
	BOOL bSucc	= FALSE;
	PTCHAR pData	= NULL;
	HGLOBAL hMem	= NULL;

	// Clipboard 열기
	if (!OpenClipboard(NULL))	return FALSE;
	// 기존 클립보드 초기화
	if (EmptyClipboard())
	{
		// 클립보드에 임시로 저장할 데이터의 메모리 공간 확보
		hMem = GlobalAlloc(GMEM_DDESHARE, size+1);	// 데이터의 크기보다 항상 1 크게
		if (hMem)
		{
			// 클립보드의 메모리 영역 Lock !!!
			pData = (TCHAR*)GlobalLock(hMem);
			if (pData)
			{
				// 전역 메모리에 복사하고자 하는 데이터 저장
				wcscpy_s(pData, size+1, data);
				// 클립 보드에 전역 메모리의 데이터를 복사
				SetClipboardData(CF_OEMTEXT, hMem);

				// 클래보드의 메모리 영역 Unlock !!!
				GlobalUnlock(hMem);
			}
		}
		// 전역 메모리 반환
		GlobalFree(hMem);
	}
	// Clipboard 닫기
	CloseClipboard();

	return bSucc;
}

/*
 desc : 10 진수를 2 진수로 변환 후 문자열 (버퍼)로 저장
 parm : dec_num	- [in]  2 진수로 변환하고자 하는 10 진수 값
		bin_num - [out] 2 진수로 변환된 데이터가 저장될 0 or 1의 숫자만 저장하기 위한 버퍼
						(충분히 커야 됨. 최소 32 ~ 255)
		bin_size- [in]  'bin_str' 버퍼의 크기
 retn : bin_size 값이 32 ~ 256 사이 값이 아니면 FALSE 반환
*/
BOOL CUtility::DecimalNumToBinaryStr(UINT64 dec_num, PUINT8 bin_num, UINT8 bin_size)
{
	if (dec_num < 1 || bin_size < 32 || bin_size > 256)	return FALSE;

	/* 기존 버퍼 초기화 */
	memset(bin_num, 0x00, bin_size);
#if 0
	UINT8 i	= 0x00, u8Temp = 0x00, u8Count = 0x00, u8Reverse;
	/* 역순으로 저장됨 */
	for (i=0; dec_num>0; i++)
	{
		bin_num[i]	= (dec_num % 2);
		dec_num		/= 2;
		u8Count++;
	}
	/* 역순 정렬 */
	for (i=0; i<u8Count/2; i++)
	{
		u8Reverse			= (u8Count - 1) - i;
		u8Temp				= bin_num[i];
		bin_num[i]			= bin_num[u8Reverse];
		bin_num[u8Reverse]	= u8Temp;
	}
#else
	UINT8 i	= 0x00;
	/* 역순으로 저장됨 */
	for (i=0; dec_num>0; i++)
	{
		bin_num[i]	= (dec_num % 2);
		dec_num		/= 2;
	}
#endif
	return TRUE;
}

/*
 desc : memset 대체 함수 (속도가 더 빠를지 몰라서 ...)
 parm : dest	- [in]  초기화 대상 버퍼 포인터
		count	- [in]  초기화 개수
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
 desc : memset 대체 함수 (8 Bytes 단위로 초기화)
 parm : dest	- [in]  초기화 대상 버퍼 포인터
		count	- [in]  초기화 개수
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
 desc : memset 대체 함수 (SIMD_128 이용)
		SIMD(128)이고, 버퍼(dest)가 16 bytes로 Align되어 있어야만 정상 동작한다
 parm : dest	- [in]  초기화 대상 버퍼 포인터
		count	- [in]  dest 버퍼 크기를 16 bytes로 나눌 때의 개수 값
 retn : None
*/
VOID CUtility::MemsetS128(PVOID dest, UINT32 count)
{
	__declspec(align(16)) UINT32 u32Zero[4] = {0,};
	UINT64 i;

	// SIMD 변수에 연결
	__m128i* pDest	= (__m128i*)dest;
	__m128i mClear	= _mm_load_si128((__m128i*)u32Zero);
	// 16 바이트씩 이동하면서 메모리 초기화
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
 desc : 폰트 이름에 대한 구조체 정보 반환
 parm :	lf		- [in]  반환되어 저장될 로그 폰트 구조체
		name	- [in]  폰트 이름
		size	- [in]  폰트 크기
		lang	- [in]  국가 언어 코드
 retn : TRUE or FALSE
*/
BOOL CUtility::GetLogFont(LOGFONT &lf, PTCHAR name, UINT32 size, UINT8 lang)
{
	if (_tcslen(name) >= LF_FACESIZE)	return FALSE;

	/* 기본 폰트 정보 얻기 */
	if (!::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lf, 0))
		return FALSE;
	lf.lfCharSet	= lang;			/* 국가 코드 */
	lf.lfHeight		= LONG(size);	/* 폰트 크기 */
	/* 폰트 명 설정 */
	swprintf_s(lf.lfFaceName, LF_FACESIZE, name);

	return TRUE;
}

/*
 desc : 배열 (바이트) 역순으로 변환하여 저장
 parm : data	- [out] 배열 포인터 (입력 : 원본, 출력 : 역순 배열)
		size	- [in]  배열 개수
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
