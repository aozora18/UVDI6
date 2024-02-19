
#include "pch.h"
#include "MyListBox.h"

#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* standard library 사용 */
using namespace std;

/*
 desc : 생성자
 parm : None
 retn : None
*/
CMyListBox::CMyListBox()
{
	m_i32MaxStr	= 0;
}

/*
 desc : 파괴자
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
 desc : 폰트 설정
 parm : lf	- [in]  폰트 정보
 retn : None
*/
VOID CMyListBox::SetLogFont(LPLOGFONT lf)
{
	if (m_ftText.GetSafeHandle())			m_ftText.DeleteObject();
	if (m_ftText.CreateFontIndirect(lf))	SetFont(&m_ftText);

	Invalidate(TRUE);
}

/*
 desc : 숫자 등록
 parm : data	- [in]  등록하고자 하는 숫자
		point	- [in]  실수형일 경우, 소숫점 자리 수
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
 desc : 현재 위치의 숫자 값 반환
 parm : index	- [in]  0 or Later
 retn : 숫자 값 반환
*/
INT64 CMyListBox::GetTextToNum(UINT32 index)
{
	TCHAR tzValue[32]	= {NULL};

	if (GetCount() <= INT32(index))	return -1;
	if (GetText(index, tzValue) == LB_ERR)	return -1;

	return _wtoi64(tzValue);
}

/*
 desc : 현재 선택된 항목 제거
 parm : None
 retn : TRUE or FALSE
*/
BOOL CMyListBox::SelectDelete()
{
	INT32 i32Index		= 0;
	if (GetCount() < 1)	return FALSE;
	i32Index	= CListBox::GetCurSel();
	if (i32Index < 0)	return FALSE;

	/* 현재 선택된 위치 */
	DeleteString(i32Index);

	return TRUE;
}

/*
 desc : 현재 선택된 항목의 숫자 값 반환
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
 desc : 숫자 (정수) 정렬 (내림 차순 or 오름 차준)
 parm : flag	- [in]  0x00 : 오름차순, 0x01 : 내름차순
 retn : None
*/
VOID CMyListBox::SortingInt(UINT8 flag)
{
	TCHAR tzValue[32]	= {NULL};
	INT32 i, i32Count	= 0;
	vector <INT32> vNum;
	CUniToChar csCnv;

	/* 현재 등록된 리스트 개수 얻어서 벡터 영역 할당 */
	i32Count	= GetCount();
	if (i32Count <= 0)	return;
	vNum.reserve(i32Count);

	/* 현재 값 읽어와서 벡터에 등록 */
	for (i=0; i<i32Count; i++)
	{
		wmemset(tzValue, 0x00, 32);
		if (GetText(i, tzValue) > 0)
		{
			vNum.push_back(atoi(csCnv.Uni2Ansi(tzValue)));
		}
	}

	/* Vector에 등록된 값 기준으로 오름차순 / 내림차순 정렬 */
	if (0x00 == flag)	sort(vNum.begin(), vNum.end());
	else				sort(vNum.begin(), vNum.end(), greater<INT32>());

	/* 기존 등록된 값 지우기 */
	SetRedraw(FALSE);
	ResetContent();

	/* 새로 등록 */
	for (i=0; i<vNum.size(); i++)
	{
		AddNumber((INT32)vNum[i]);
	}
	SetRedraw(TRUE);

	/* 벡터 메모리 해제 */
	vNum.clear();
}

/*
 desc : Scroll Bar 갱신 (가로 출력)
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
 desc : 가로 스크롤바 나타내기 위함 (리스트 박스에서는 가로 스크롤바가 정상적으로 호출되지 않음)
 parm : str	- [in]  입력되는 문자열
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
 desc : 문자열 맨 마지막에 추가
 parm : str	- [in]  추가 대상 문자열이 저장된 버퍼
 retn : 등록된 위치 (Zero or Later)
*/
INT32 CMyListBox::AddString(LPTSTR str)
{
	INT32 i32Index = CListBox::AddString(str);
	if (i32Index < 0)	return i32Index;
	/* 수평 스크롤 바 갱신 */
	UpdateHorizScrollBar(str);

	return i32Index;
}

/*
 desc : 문자열 맨 마지막에 추가
 parm : index	- [in]  추가하려는 리스트 박스의 위치
		str		- [in]  추가 대상 문자열이 저장된 버퍼
 retn : 등록된 위치 (Zero or Later)
*/
INT32 CMyListBox::InsertString(INT32 index, LPTSTR str)
{
	INT32 i32Index = CListBox::InsertString(index, str);
	if (i32Index < 0)	return i32Index;
	/* 수평 스크롤 바 갱신 */
	UpdateHorizScrollBar(str);

	return i32Index;
}

/*
 desc : 리스트 박스의 내용 모두 비우기
 parm : None
 retn : None
*/
VOID CMyListBox::ResetContent()
{
	CListBox::ResetContent();
	m_i32MaxStr = 0;
}

/*
 desc : 리스트 박스에 등록된 임의 항목 선택
 parm : index	- [in]  선택 위치 (Zero-based)
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
 desc : 리스트 박스에 등록된 임의 항목 삭제
 parm : index	- [in]  삭제 대상 위치 (Zero-based)
 retn : 삭제 위치 반환
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

	/* 기존 값 초기화 */
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
 desc : 리스트 박스에 등록된 임의 항목 삭제
 parm : index	- [in]  삭제 대상 위치 (Zero-based)
 retn : 삭제 위치 반환
*/
VOID CMyListBox::DeleteSelString()
{
	INT32 i32Index	= CListBox::GetCurSel();

	if (i32Index == -1)	return;
	DeleteString(i32Index);
}