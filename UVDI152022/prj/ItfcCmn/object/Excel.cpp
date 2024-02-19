
/*
 desc : Excel Utility ���
*/

#include "pch.h"
#include "Excel.h"


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
CExcel::CExcel()
{
}

/*
 desc : Destructor
 parm : None
 retn : None
*/
CExcel::~CExcel()
{
}

/*
 desc : �������� ��(Cell)�� ��ġ�� ��(Row)�� ��(Col) ���ڸ� Cell �̸����� ��ȯ�ϱ�
		(�ִ� 'AAAA'�� ���� 4���� �÷������ ��ȯ �����ϴ�)
 parm : col	- [in]  Cell�� ��ġ�� Column Index (1 based)
		row	- [in]  Cell�� ��ġ�� Row Index (1 based)
		name- [out] Cell�� ��ġ�� ���� ������ Cell Name���� ��ȯ
		size- [in]  'name' ���� ũ��
 retn : TRUE or FALSE
*/
BOOL CExcel::ExcelCellName(UINT32 col, UINT32 row, PTCHAR name, UINT32 size)
{
	CUniToChar csCnv;

	if (!name)	return FALSE;
	wmemset(name, 0x00, size);

	UINT32 u32ZColumn	= 26;
	UINT32 u32ZZColumn	= 702;		// u32ZColumn + (26 * 26);
	UINT32 u32ZZZColumn	= 18278;	// u32ZZColumn + (26 * 26 * 26);

	UINT32 u32AColumn	= 1;
	UINT32 u32AAColumn	= 27;		// u32ZColumn + 1
	UINT32 u32AAAColumn	= 703;		// u32ZZColumn + 1
	UINT32 u32AAAAColumn= 18279;	// u32ZZZColumn + 1

	TCHAR tFours	= (CHAR)((col-u32AAAAColumn) / 17576) % 26 + L'A';
	TCHAR tThrees	= (CHAR)((col-u32AAAColumn) / 676) % 26 + L'A';
	TCHAR tTwos		= (CHAR)((col-u32AAColumn) / 26) % 26 + L'A';
	TCHAR tOnes		= (CHAR)((col-u32AColumn) % 26) + L'A';

	if (col > u32ZZZColumn)		// AAAA1 - ZZZZ1
		swprintf_s(name, size, _T("%c%c%c%c%d"), tFours, tThrees, tTwos, tOnes, row);
	else if (col > u32ZZColumn)	// AAA1 - ZZZ1
		swprintf_s(name, size, _T("%c%c%c%d"), tThrees, tTwos, tOnes, row);
	else if(col > u32ZColumn )	// AA1 - ZZ1
		swprintf_s(name, size, _T("%c%c%d"), tTwos, tOnes, row);
	else						// A1 - Z1          
		swprintf_s(name, size, _T("%c%d"), tOnes, row);

	return TRUE;
}
