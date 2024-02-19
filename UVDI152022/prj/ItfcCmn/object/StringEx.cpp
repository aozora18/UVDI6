
/*
 desc : String (문자열) 모듈
*/

#include "pch.h"
#include "StringEx.h"


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
CStringEx::CStringEx()
{
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CStringEx::~CStringEx()
{
}

/*
 desc : 혼합 문자열 정렬하기
 parm : p1	- [in]  비교되는 앞에 문자열
		p2	- [in]  비교되는 뒤에 문자열
 retn : 정수 값. 0 - 두 문자열 같음, -1 - p1이 p2보다 작음, 1 - p1이 p2보다 큼
*/
INT32 CStringEx::SortMixedString(const VOID *p1, const VOID *p2)
{
	PTCHAR sz1 = ((CString*)(p1))->GetBuffer(0);
	PTCHAR sz2 = ((CString*)(p2))->GetBuffer(0);
	INT32 i32Pos1 = -1, i32Pos2 = -1, i32EndPos1, i32EndPos2, i32Result;

	while (1)
	{
		i32Pos1++;
		i32Pos2++;

		/* Make sure we haven't hit the end of either of the strings */
		if (sz1[i32Pos1] == 0 && sz2[i32Pos2] == 0)	return 0;
		if (sz1[i32Pos1] == 0)						return -1;
		if (sz2[i32Pos2] == 0)						return 1;
		// See if this part of both strings is a number */
		if (sz1[i32Pos1] >= _T('0') && sz1[i32Pos1] <= _T('9') &&
			sz2[i32Pos2] >= _T('0') && sz2[i32Pos2] <= _T('9'))
		{
			/* Find the end of each number */
			i32EndPos1 = i32Pos1;
			do	{

				i32EndPos1++;

			}	while (sz1[i32EndPos1] >= _T('0') && sz1[i32EndPos1] <= _T('9'));

			i32EndPos2 = i32Pos2;

			do	{

				i32EndPos2++;

			} while (sz2[i32EndPos2] >= _T('0') && sz2[i32EndPos2] <= _T('9'));

			while (1)
			{
				if (i32EndPos1-i32Pos1 == i32EndPos2-i32Pos2)
				{
					/* Both numbers are the same length, just compare them */
					i32Result = _tcsnicmp(sz1 + i32Pos1, sz2 + i32Pos2, size_t(i32EndPos1 - i32Pos1));
					if (i32Result == 0)
					{
						i32Pos1 = i32EndPos1 - 1;
						i32Pos2 = i32EndPos2 - 1;
						break;
					}
					else
					{
						return i32Result;
					}
				}
				else if (i32EndPos1-i32Pos1 > i32EndPos2-i32Pos2)
				{
					/* First number is longer, so if it's not zero padded, it's bigger */
					if (sz1[i32Pos1] == _T('0'))	i32Pos1 ++;
					else							return 1;
				}
				else
				{
					/* Second number is longer, so if it's not zero padded, it's bigger */
					if (sz2[i32Pos2] == _T('0'))	i32Pos2 ++;
					else							return -1;
				}
			}
		}
		else
		{
			/* One or both characters is not a number, so just compare them as a string */
			i32Result = _tcsnicmp(sz1+i32Pos1, sz2+i32Pos2, 1);
			if (i32Result != 0)	return i32Result;
		}
	}
}

/*
 desc : 문자열 중에 공백 문자가 포함되어 있는지 여부 확인
 parm : str	- [in]  문자열
 retn : TRUE (한개 이상 공백 문자가 포함되어 있다), FALSE (포함되어 있지 않다)
 note : 공백 문자 조건은 space, tab(\t), return(\n), carrage (\r)
 */
BOOL CStringEx::IsSpaceInString(PTCHAR str)
{
	UINT32 i =	0;

	for (; i<_tcslen(str); i++)
	{
		if (iswspace(str[i]))	return TRUE;
	}

	return FALSE;
}

/*
 desc : 특정 데이터의 위치 반환
 parm : buff	- [in]  검색 대상이 저장된 문자열 버퍼
		size	- [in]  'buff'에 저장된 문자열의 크기
		ch		- [in]  검색 문자 값
 retn : 음수 - 검색 실패, 0 이상 값 - 검색 성공
*/
#ifdef _UNICODE
INT32 CStringEx::GetFindCh(PTCHAR buff, INT32 size, TCHAR ch)
#else
INT32 CStringEx::GetFindCh(CHAR *buff, INT32 size, CHAR ch)
#endif
{
	INT32 i32Find = -1, i = 0;

	// 문자열 길이만큼 루프 돌면서 문자 검색
	for (; i<size; i++)
	{
		if (buff[i] == ch)
		{
			i32Find = i;
			break;
		}
	}

	return i32Find;
}

/*
 desc : 데이터 (문자열)에서 검색 문자가 몇 개 존재하는지 반환
 parm : buff	- [in]  검색 대상이 저장된 문자열 버퍼
		size	- [in]  'buff'에 저장된 문자열의 크기
		ch		- [in]  검색 문자 값
 retn : 0 - 검색 실패, 0 이상 값 - 검색 성공 (검색된 문자 개수)
*/
#ifdef _UNICODE
UINT32 CStringEx::GetFindChCount(PTCHAR buff, UINT32 size, TCHAR ch)
#else
UINT32 CStringEx::GetFindChCount(CHAR *buff, UINT32 size, CHAR ch)
#endif
{
	UINT32 u32Find = 0, i = 0;

	// 문자열 길이만큼 루프 돌면서 문자 검색
	for (; i<size; i++)
	{
		if (buff[i] == ch)	u32Find++;
	}

	return u32Find;
}

/*
 desc : 16진수 문자를 16진수 숫자로 변환
 parm : ch	- [in]  16진수 문자 값
 retn : 16진수 숫자 값 반환, 실패인 경우 0x00 반환
*/
UINT8 CStringEx::GetHexCharToNum(CHAR ch)
{
	/* 0x00 ~ 0x09 */
	if (ch >= 0x30 && ch <= 0x39)	/* ch >= '0' && ch <= '9' */
	{
		return (ch - 0x30);
	}

	/* 0x0a ~ 0x0f */
	if (ch >= 0x61 && ch <= 0x7A)	/* ch >= 'a' && ch <= 'z' */
	{
		return ((ch - 0x61) + 10);
	}

	/* 0x0A ~ 0x0F */
	if (ch >= 0x41 && ch <= 0x5A)	/* ch >= 'A' && ch <= 'Z' */
	{
		return ((ch - 0x41) + 10);
	}
	
	return 0x00;
}

/*
 desc : 16진수 숫자를 16진수 문자로 변환 (대소문자 구분 안하고, 무조건 소문자 처리)
 parm : num	- [in]  16진수 숫자 값
 retn : 16진수 문자 값 반환. 실패인 경우 0x00
*/
UINT8 CStringEx::GetHexNumToChar(UINT8 num)
{
	/* 0x00 ~ 0x09 */
	if (num >=0x00 && num <=0x09)
	{
		return (num + 0x30);	// '0' ~ '9'
	}

	/* 0x0a ~ 0x0f */
	if (num >=10 && num <= 16)
	{
		return (0x61 + (num - 10));	// 'a' ~ 'f'
	}

	return 0x00;
}

/*
 desc : 송/수신 패킷 데이터를 문자열 메시지로 변환
 parm : mesg	- [out] 메시지 본문 정보가 저장될 버퍼 포인터
		mlen	- [in]  'mesg' 버퍼 크기
		data	- [in]  송신될 or 수신된 메시지가 저장된 버퍼 포인터
		dlen	- [in]  'data' 버퍼에 저장된 데이터의 크기 (Bytes)
 retn : 전체 생성된 데이터의 길이 반환
*/
UINT32 CStringEx::SetMakePktToMesg(PCHAR mesg, UINT32 mlen, PUINT8 data, UINT32 dlen)
{
	PCHAR pMesg	= mesg;
	UINT32 i	= 0;

	/* 본문 각 1 Byte 값을 Hex로 변경 */
	for (; i<dlen; i++)
	{
		/* 만약 저장 공간을 벗어나는 경우인지 확인 (오버플로 방지) */
		if ((pMesg - mesg + 4) > mlen)	break;
		/* Binary code -> Ascii code */
		sprintf_s(pMesg, 4, "%02X ", data[i]);
		pMesg	+= 3;
	}

	return UINT32(pMesg - mesg);
}
