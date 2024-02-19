
#include "pch.h"
#include "MyListBox.h"

#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* standard library ��� */
using namespace std;

/*
 desc : ������
 parm : None
 retn : None
*/
CMyListBox::CMyListBox()
{
	m_i32MaxStr	= 0;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CMyListBox::~CMyListBox()
{
	if (m_ftText.GetSafeHandle())	m_ftText.DeleteObject();
}


BEGIN_MESSAGE_MAP(CMyListBox, CListBox)
END_MESSAGE_MAP()

VOID CMyListBox::PreSubclassWindow()
{

	CListBox::PreSubclassWindow();
}


/*
 desc : ��Ʈ ����
 parm : lf	- [in]  ��Ʈ ����
 retn : None
*/
VOID CMyListBox::SetLogFont(LPLOGFONT lf)
{
	if (m_ftText.GetSafeHandle())			m_ftText.DeleteObject();
	if (m_ftText.CreateFontIndirect(lf))	SetFont(&m_ftText);

	Invalidate(TRUE);
}

/*
 desc : ���� ���
 parm : data	- [in]  ����ϰ��� �ϴ� ����
		point	- [in]  �Ǽ����� ���, �Ҽ��� �ڸ� ��
 retn : None
*/
VOID CMyListBox::AddNumber(UINT32 data)
{
#ifdef	_UNICODE
	TCHAR szData[1024]	= {NULL};
	swprintf_s(szData, 1024, L"%I32u", data);
#else
	char szData[1024]	= {NULL};
	sprintf_s(szData, 1024, "%I32u", data);
#endif
	CMyListBox::AddString(szData);
}
VOID CMyListBox::AddNumber(INT32 data)
{
#ifdef	_UNICODE
	TCHAR szData[1024]	= {NULL};
	swprintf_s(szData, 1024, L"%I32d", data);
#else
	char szData[1024]	= {NULL};
	sprintf_s(szData, 1024, "%I32d", data);
#endif
	CMyListBox::AddString(szData);
}
VOID CMyListBox::AddNumber(UINT64 data)
{
#ifdef	_UNICODE
	TCHAR szData[1024]	= {NULL};
	swprintf_s(szData, 1024, L"%I64u", data);
#else
	char szData[1024]	= {NULL};
	sprintf_s(szData, 1024, "%I64u", data);
#endif
	CMyListBox::AddString(szData);
}
VOID CMyListBox::AddNumber(INT64 data)
{
#ifdef	_UNICODE
	TCHAR szData[1024]	= {NULL};
	swprintf_s(szData, 1024, L"%I64d", data);
#else
	char szData[1024]	= {NULL};
	sprintf_s(szData, 1024, "%I64d", data);
#endif
	CMyListBox::AddString(szData);
}
VOID CMyListBox::AddNumber(DOUBLE data, INT32 point)
{
#ifdef	_UNICODE
	TCHAR szData[1024]	= {NULL};
	TCHAR szPoint[32]	= {NULL};
	swprintf_s(szPoint, _T("%%.%df"), point);
	swprintf_s(szData, 1024, szPoint, data);
#else
	char szData[1024]	= {NULL};
	char szPoint[32]	= {NULL};
	swprintf_s(szPoint, "%%.%df", point);
	sprintf_s(szData, 1024, "%d", data);
#endif
	CMyListBox::AddString(szData);
}

/*
 desc : ���� ��ġ�� ���� �� ��ȯ
 parm : index	- [in]  0 or Later
 retn : ���� �� ��ȯ
*/
INT64 CMyListBox::GetTextToNum(UINT32 index)
{
	TCHAR tzValue[32]	= {NULL};

	if (GetCount() <= INT32(index))	return -1;
	if (GetText(index, tzValue) == LB_ERR)	return -1;

	return _wtoi64(tzValue);
}

/*
 desc : ���� ���õ� �׸� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMyListBox::SelectDelete()
{
	INT32 i32Index		= 0;
	if (GetCount() < 1)	return FALSE;
	i32Index	= CListBox::GetCurSel();
	if (i32Index < 0)	return FALSE;

	/* ���� ���õ� ��ġ */
	DeleteString(i32Index);

	return TRUE;
}

/*
 desc : ���� ���õ� �׸��� ���� �� ��ȯ
 parm : None
 retn : -1 or 0 or Later
*/
INT64 CMyListBox::GetSelNum()
{
	TCHAR tzValue[32]	= {NULL};
	INT32 i32Index		= 0;

	if (GetCount() < 1)		return -1;
	i32Index	= CListBox::GetCurSel();
	if (i32Index < 0)	return -1;
	if (GetText(i32Index, tzValue) == LB_ERR)	return -1;

	return _wtoi64(tzValue);
}

/*
 desc : ���� (����) ���� (���� ���� or ���� ����)
 parm : flag	- [in]  0x00 : ��������, 0x01 : ��������
 retn : None
*/
VOID CMyListBox::SortingInt(UINT8 flag)
{
	TCHAR tzValue[32]	= {NULL};
	INT32 i, i32Count	= 0;
	vector <INT32> vNum;
	CUniToChar csCnv;

	/* ���� ��ϵ� ����Ʈ ���� �� ���� ���� �Ҵ� */
	i32Count	= GetCount();
	if (i32Count <= 0)	return;
	vNum.reserve(i32Count);

	/* ���� �� �о�ͼ� ���Ϳ� ��� */
	for (i=0; i<i32Count; i++)
	{
		wmemset(tzValue, 0x00, 32);
		if (GetText(i, tzValue) > 0)
		{
			vNum.push_back(atoi(csCnv.Uni2Ansi(tzValue)));
		}
	}

	/* Vector�� ��ϵ� �� �������� �������� / �������� ���� */
	if (0x00 == flag)	sort(vNum.begin(), vNum.end());
	else				sort(vNum.begin(), vNum.end(), greater<INT32>());

	/* ���� ��ϵ� �� ����� */
	SetRedraw(FALSE);
	ResetContent();

	/* ���� ��� */
	for (i=0; i<vNum.size(); i++)
	{
		AddNumber((INT32)vNum[i]);
	}
	SetRedraw(TRUE);

	/* ���� �޸� ���� */
	vNum.clear();
}

/*
 desc : Scroll Bar ���� (���� ���)
 parm : None
 retn : None
*/
VOID CMyListBox::UpdateScrollBar()
{
	INT32 i32Width	= 0;
	CString strMesg;
	CSize szSize;
	CDC *pDC = GetDC();
	if (!pDC)	return;

	for(int i = 0; i < GetCount(); i++)
	{
		GetText(i, strMesg);
		szSize	= pDC->GetTextExtent(strMesg);
		if (szSize.cx > i32Width)	i32Width = szSize.cx;
	}
	
	if (GetHorizontalExtent() < i32Width)
	{
		SetHorizontalExtent(i32Width);
		ASSERT(GetHorizontalExtent() == i32Width);
	}

	ReleaseDC(pDC);
}

/*
 desc : ���� ��ũ�ѹ� ��Ÿ���� ���� (����Ʈ �ڽ������� ���� ��ũ�ѹٰ� ���������� ȣ����� ����)
 parm : str	- [in]  �ԷµǴ� ���ڿ�
 retn : None
*/
VOID CMyListBox::UpdateHorizScrollBar(LPCTSTR str)
{
	CClientDC dc(this);
	CSize szText;

	CFont *ftText = CListBox::GetFont();
	dc.SelectObject(ftText);
	szText		= dc.GetTextExtent(str, (INT32)_tcslen(str));
	szText.cx	+= 3 * ::GetSystemMetrics(SM_CXBORDER);
	if (szText.cx > m_i32MaxStr)
	{
		m_i32MaxStr	= szText.cx;

		CListBox::SetHorizontalExtent(m_i32MaxStr);
	}
}

/*
 desc : ���ڿ� �� �������� �߰�
 parm : str	- [in]  �߰� ��� ���ڿ��� ����� ����
 retn : ��ϵ� ��ġ (Zero or Later)
*/
INT32 CMyListBox::AddString(LPTSTR str)
{
	INT32 i32Index = CListBox::AddString(str);
	if (i32Index < 0)	return i32Index;
	/* ���� ��ũ�� �� ���� */
	UpdateHorizScrollBar(str);

	return i32Index;
}

/*
 desc : ���ڿ� �� �������� �߰�
 parm : index	- [in]  �߰��Ϸ��� ����Ʈ �ڽ��� ��ġ
		str		- [in]  �߰� ��� ���ڿ��� ����� ����
 retn : ��ϵ� ��ġ (Zero or Later)
*/
INT32 CMyListBox::InsertString(INT32 index, LPTSTR str)
{
	INT32 i32Index = CListBox::InsertString(index, str);
	if (i32Index < 0)	return i32Index;
	/* ���� ��ũ�� �� ���� */
	UpdateHorizScrollBar(str);

	return i32Index;
}

/*
 desc : ����Ʈ �ڽ��� ���� ��� ����
 parm : None
 retn : None
*/
VOID CMyListBox::ResetContent()
{
	CListBox::ResetContent();
	m_i32MaxStr = 0;
}

/*
 desc : ����Ʈ �ڽ��� ��ϵ� ���� �׸� ����
 parm : index	- [in]  ���� ��ġ (Zero-based)
 retn : None
*/
VOID CMyListBox::SetSel(INT32 index)
{
	INT32 i32Count	= GetCount();
	if (i32Count == 0)	return;
	else if (i32Count <= index)	CListBox::SetSel(i32Count-1);
	else						CListBox::SetSel(index);
}
 
/*
 desc : ����Ʈ �ڽ��� ��ϵ� ���� �׸� ����
 parm : index	- [in]  ���� ��� ��ġ (Zero-based)
 retn : ���� ��ġ ��ȯ
*/
INT32 CMyListBox::DeleteString(INT32 index)
{
	INT32 i, i32Result	= CListBox::DeleteString(index);
	CString strText;
	CSize szText;

	if (i32Result < 0)	return i32Result;

	CClientDC dc(this);

	CFont *ftText = CListBox::GetFont();
	dc.SelectObject(ftText);

	/* ���� �� �ʱ�ȭ */
	m_i32MaxStr = 0;

	/* scan strings */
	for (i=0; i<CListBox::GetCount(); i++)
	{ 
		CListBox::GetText(i, strText);
		szText		= dc.GetTextExtent(strText);
		szText.cx	+= 3 * ::GetSystemMetrics(SM_CXBORDER);
		if(szText.cx > m_i32MaxStr)	m_i32MaxStr = szText.cx;
	}
	CListBox::SetHorizontalExtent(m_i32MaxStr);

	return i32Result;
}

/*
 desc : ����Ʈ �ڽ��� ��ϵ� ���� �׸� ����
 parm : index	- [in]  ���� ��� ��ġ (Zero-based)
 retn : ���� ��ġ ��ȯ
*/
VOID CMyListBox::DeleteSelString()
{
	INT32 i32Index	= CListBox::GetCurSel();

	if (i32Index == -1)	return;
	DeleteString(i32Index);
}