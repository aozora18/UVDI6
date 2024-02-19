
/*
 desc : String (���ڿ�) ���
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
 desc : ȥ�� ���ڿ� �����ϱ�
 parm : p1	- [in]  �񱳵Ǵ� �տ� ���ڿ�
		p2	- [in]  �񱳵Ǵ� �ڿ� ���ڿ�
 retn : ���� ��. 0 - �� ���ڿ� ����, -1 - p1�� p2���� ����, 1 - p1�� p2���� ŭ
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
 desc : ���ڿ� �߿� ���� ���ڰ� ���ԵǾ� �ִ��� ���� Ȯ��
 parm : str	- [in]  ���ڿ�
 retn : TRUE (�Ѱ� �̻� ���� ���ڰ� ���ԵǾ� �ִ�), FALSE (���ԵǾ� ���� �ʴ�)
 note : ���� ���� ������ space, tab(\t), return(\n), carrage (\r)
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
 desc : Ư�� �������� ��ġ ��ȯ
 parm : buff	- [in]  �˻� ����� ����� ���ڿ� ����
		size	- [in]  'buff'�� ����� ���ڿ��� ũ��
		ch		- [in]  �˻� ���� ��
 retn : ���� - �˻� ����, 0 �̻� �� - �˻� ����
*/
#ifdef _UNICODE
INT32 CStringEx::GetFindCh(PTCHAR buff, INT32 size, TCHAR ch)
#else
INT32 CStringEx::GetFindCh(CHAR *buff, INT32 size, CHAR ch)
#endif
{
	INT32 i32Find = -1, i = 0;

	// ���ڿ� ���̸�ŭ ���� ���鼭 ���� �˻�
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
 desc : ������ (���ڿ�)���� �˻� ���ڰ� �� �� �����ϴ��� ��ȯ
 parm : buff	- [in]  �˻� ����� ����� ���ڿ� ����
		size	- [in]  'buff'�� ����� ���ڿ��� ũ��
		ch		- [in]  �˻� ���� ��
 retn : 0 - �˻� ����, 0 �̻� �� - �˻� ���� (�˻��� ���� ����)
*/
#ifdef _UNICODE
UINT32 CStringEx::GetFindChCount(PTCHAR buff, UINT32 size, TCHAR ch)
#else
UINT32 CStringEx::GetFindChCount(CHAR *buff, UINT32 size, CHAR ch)
#endif
{
	UINT32 u32Find = 0, i = 0;

	// ���ڿ� ���̸�ŭ ���� ���鼭 ���� �˻�
	for (; i<size; i++)
	{
		if (buff[i] == ch)	u32Find++;
	}

	return u32Find;
}

/*
 desc : 16���� ���ڸ� 16���� ���ڷ� ��ȯ
 parm : ch	- [in]  16���� ���� ��
 retn : 16���� ���� �� ��ȯ, ������ ��� 0x00 ��ȯ
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
 desc : 16���� ���ڸ� 16���� ���ڷ� ��ȯ (��ҹ��� ���� ���ϰ�, ������ �ҹ��� ó��)
 parm : num	- [in]  16���� ���� ��
 retn : 16���� ���� �� ��ȯ. ������ ��� 0x00
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
 desc : ��/���� ��Ŷ �����͸� ���ڿ� �޽����� ��ȯ
 parm : mesg	- [out] �޽��� ���� ������ ����� ���� ������
		mlen	- [in]  'mesg' ���� ũ��
		data	- [in]  �۽ŵ� or ���ŵ� �޽����� ����� ���� ������
		dlen	- [in]  'data' ���ۿ� ����� �������� ũ�� (Bytes)
 retn : ��ü ������ �������� ���� ��ȯ
*/
UINT32 CStringEx::SetMakePktToMesg(PCHAR mesg, UINT32 mlen, PUINT8 data, UINT32 dlen)
{
	PCHAR pMesg	= mesg;
	UINT32 i	= 0;

	/* ���� �� 1 Byte ���� Hex�� ���� */
	for (; i<dlen; i++)
	{
		/* ���� ���� ������ ����� ������� Ȯ�� (�����÷� ����) */
		if ((pMesg - mesg + 4) > mlen)	break;
		/* Binary code -> Ascii code */
		sprintf_s(pMesg, 4, "%02X ", data[i]);
		pMesg	+= 3;
	}

	return UINT32(pMesg - mesg);
}
