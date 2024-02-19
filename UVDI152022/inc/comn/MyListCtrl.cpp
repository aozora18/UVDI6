
#include "pch.h"
#include "MyListCtrl.h"

/*
 note : 리스트 컨트롤 속성 설정하기
		No Labe Wrap	: TRUE
		No Column Header: TRUE
*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : None
 retn : None
*/
CMyListCtrl::CMyListCtrl()
{
	m_bEnableNMClick= FALSE;	/* 기본적으로 NM Click Event 부모에게 알림 */
	m_bAscending	= FALSE;	/* 기본 정렬 방식 지정 */

	wmemset(m_tzSelText, 0x00, 1024);

	/* 선택된 기본 문자 및 배경색 */
	m_clrText	= RGB(255, 255, 255);
	m_clrBack	= RGB(0, 0, 0);
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMyListCtrl::~CMyListCtrl()
{
	if (m_fText.GetSafeHandle())	m_fText.DeleteObject();
}

BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK,	OnLvnColumnclick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW,	OnCustomDraw)
	ON_NOTIFY_REFLECT_EX(NM_CLICK,		OnNMClick)
END_MESSAGE_MAP()

/*
 desc : 폰트 설정
 parm : lf	- [in]  로그 폰트
 retn : None
*/
VOID CMyListCtrl::SetLogFont(LOGFONT *lf)
{
	if (m_fText.GetSafeHandle())	m_fText.DeleteObject();
	if (m_fText.CreateFontIndirect(lf))
	{
		SetFont(&m_fText);
	}
}

/*
 desc : 새로운 폰트 이름 적용
 parm : name	- [in]  Font Name
		size	- [in]  Font Size
 retn : TRUE or FALSE
*/
BOOL CMyListCtrl::SetFontName(TCHAR *name, UINT32 size)
{
	if (m_fText.GetSafeHandle())	m_fText.DeleteObject();
	if (!m_fText.CreateFont(size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET,
							 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
							 DEFAULT_PITCH | FF_SWISS, name))	return FALSE;
	SetFont(&m_fText);
	return TRUE;
}

/*
 desc : 텍스트와 선택된 배경 색상을 설정
 parm : text	- [in]  선택된 문자색
		back	- [in]  선택된 배경색
 retn : None
*/
VOID CMyListCtrl::SetDataColor(COLORREF text, COLORREF back)
{
	m_clrText	= text;
	m_clrBack	= back;
}

/*
 desc : NM Click Event를 자신의 부모에게 알리는 기능 On/Off
 parm : flag	- [in]  TRUE : 자신의 부모에게 NM Click Event 알리지 않는다.
						FALSE: 자신의 부모에게 NM Click Event 알려준다.
 retn : None
*/
VOID CMyListCtrl::SetEnableNMClick(BOOL flag)
{
	m_bEnableNMClick	= flag;
}

/*
 desc : 스크롤을 맨 아래로 이동 시킴
 parm : None
 retn : None
*/
VOID CMyListCtrl::SetScrollEnd()
{
	CRect r;
	GetItemRect(0, r, LVIR_BOUNDS);
	Scroll(CSize(0, (r.bottom - r.top) * GetItemCount()));
}

/*
 desc : 스크롤 아래로 1개 이동
 parm : None
 retn : None
*/
VOID CMyListCtrl::SetScrollDown()
{
	INT32 iScrollPos	= GetScrollPos(SB_VERT);
	CRect r;
	GetItemRect(0, r, LVIR_BOUNDS);
	CSize szHeight(0, r.Height() * iScrollPos);

	Scroll(szHeight);
	SetScrollPos(SB_VERT, iScrollPos, FALSE);
}

/*
 desc : 현재 선택되어 있는 위치 찾기
 parm : None
 retn : 선택되어 있는 위치 값 반환 (음수 값은 실패)
 note : 여러개 선택되어 있더라도, 맨 처음 찾은것만 리턴
*/
INT32 CMyListCtrl::GetCurSel()
{
#if 0
	INT32 i, i32Count	= GetItemCount();

	for (i=0; i<i32Count; i++)
	{
		// 현재 선택되어 있는 항목인지 확인
		if (GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			return i;
		}
	}
	return -1;
#else
	return m_i32SelRow;
#endif
}

/*
 desc : Custom Draw
 parm : nmhdr	- [in]  ???
		result	- [in]  result
 retn : None
*/
VOID CMyListCtrl::OnCustomDraw(NMHDR *nmhdr, LRESULT *result) 
{
    NMLVCUSTOMDRAW *pCD = (NMLVCUSTOMDRAW*)nmhdr;

	*result	= CDRF_DODEFAULT;     

	// obtain row and column of item
	INT32 i32Row = (INT32)pCD->nmcd.dwItemSpec;
	/*INT32 i32Col = (INT32)pCD->iSubItem;*/
	// Remove standard highlighting of selected (sub)item.
	pCD->nmcd.uItemState = CDIS_DEFAULT;
			
	switch (pCD->nmcd.dwDrawStage)
	{
		// First stage (for the whole control)
    	case CDDS_PREPAINT		:	*result	= CDRF_NOTIFYITEMDRAW;		break;
		
		case CDDS_ITEMPREPAINT	:	*result	= CDRF_NOTIFYSUBITEMDRAW;	break;
		
		// Stage three 
		case CDDS_ITEMPREPAINT|CDDS_SUBITEM	:
		// if (sub)item was the one clicked, set custom text/background color
		// Un-comment the code segment to allow single cell highlight
		if ((m_i32SelRow == i32Row /*&& m_i32SelCol == i32Col */))
		{
			pCD->clrText	= m_clrText;			//selected text color
			pCD->clrTextBk	= m_clrBack;			//selected basckground color
		}
		else
		{
			pCD->clrText	= RGB(0,0,0);			// default text color
			pCD->clrTextBk	= RGB(255,255,255);		// default background color
		}
		*result =  CDRF_NOTIFYPOSTPAINT;
		break;

		// Stage four (called for each subitem of the focused item)
		case CDDS_ITEMPOSTPAINT|CDDS_SUBITEM	:	break;
		// it wasn't a notification that was interesting to us.	
		default	:	*result = CDRF_DODEFAULT;	break;
	}
}

/*
 desc : 버튼이 눌린 경우
 parm : char	- [in]  ???
		rep_cnt	- [in]  ???
		flags	- [in]  ???
 retn : None
*/
VOID CMyListCtrl::OnKeyDown(UINT32 key, UINT32 rep_cnt, UINT32 flags) 
{
	CHeaderCtrl* pHeaderCtrl= GetHeaderCtrl();
	INT32 i32ColumnCount	= pHeaderCtrl->GetItemCount();

	if (key == VK_LEFT && m_i32SelCol > 0)						m_i32SelCol -= 1;
	if (key == VK_UP && m_i32SelRow > 0)						m_i32SelRow -= 1;
	if (key == VK_RIGHT && m_i32SelCol < (i32ColumnCount-1))	m_i32SelCol += 1;
	if (key == VK_DOWN &&  m_i32SelRow < (GetItemCount()-1))	m_i32SelRow += 1;

	Invalidate();
	
	CListCtrl::OnKeyDown(key, rep_cnt, flags);
}

/*
 desc : 마우스 클릭된 경우
 parm : nmhdr	- [in]  ???
		result	- [in]  ???
 retn : TRUE (부모에게 통지 안함) / FALSE (부모에게 통지함
*/
BOOL CMyListCtrl::OnNMClick(NMHDR *nmhdr, LRESULT *result)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)nmhdr;

    int i32Row = pNMListView->iItem;
    int i32Col = pNMListView->iSubItem;
	if (i32Row != -1 && i32Col != -1)
	{
	   // set current item
	   m_i32SelRow = i32Row;
	   m_i32SelCol = i32Col;
       
	   Invalidate();
	}

	return m_bEnableNMClick;
}

/*
 desc : 현재 선택된 항목의 문자열 반환
 parm : column	- [in]  행의 컬럼 위치
 retn : 선택된 문자열 (최대 1024)
*/
TCHAR *CMyListCtrl::GetSelText(UINT32 column)
{
	wmemset(m_tzSelText, 0x00, 1024);
	GetItemText(m_i32SelRow, column, m_tzSelText, 1024);
	return m_tzSelText;
}

/*
 desc : 2 Bytes의 Unsigned 숫자 값을 반환
 parm : row	- [in]  행
		col	- [in]  열
 retn : 값 반환
*/
UINT16 CMyListCtrl::GetTextToNumU16(UINT32 row, UINT32 col)
{
	TCHAR tzValue[64] = {NULL};
	GetItemText(row, col, tzValue, 64);
	return (UINT16)_wtoi64(tzValue);
}

/*
 desc : 2 Bytes의 Signed 숫자 값을 반환
 parm : row	- [in]  행
		col	- [in]  열
 retn : 값 반환
*/
UINT16 CMyListCtrl::GetTextToNumI16(UINT32 row, UINT32 col)
{
	TCHAR tzValue[64] = {NULL};
	GetItemText(row, col, tzValue, 64);
	return (INT16)_wtoi64(tzValue);
}

/*
 desc : 2 Bytes의 Unsigned 숫자 값을 해당 위치에 출력
 parm : row	- [in]  행
		col	- [in]  열
		val	- [in]  값
 retn : None
*/
VOID CMyListCtrl::SetTextToNumU16(UINT32 row, UINT32 col, UINT16 val)
{
	TCHAR tzValue[64] = {NULL};

	if (val != GetTextToNumU16(row, col))
	{
		swprintf_s(tzValue, 64, L"%u", val);
		SetItemText(row, col, tzValue);
	}
}

/*
 desc : 4 Bytes의 Unsigned 숫자 값을 반환
 parm : row	- [in]  행
		col	- [in]  열
 retn : 값 반환
*/
UINT32 CMyListCtrl::GetTextToNumU32(UINT32 row, UINT32 col)
{
	TCHAR tzValue[64] = {NULL};
	GetItemText(row, col, tzValue, 64);
	return (UINT32)_wtoi64(tzValue);
}

/*
 desc : 4 Bytes의 Signed 숫자 값을 반환
 parm : row	- [in]  행
		col	- [in]  열
 retn : 값 반환
*/
UINT32 CMyListCtrl::GetTextToNumI32(UINT32 row, UINT32 col)
{
	TCHAR tzValue[64] = {NULL};
	GetItemText(row, col, tzValue, 64);
	return (INT32)_wtoi64(tzValue);
}

/*
 desc : 4 Bytes의 Unsigned 숫자 값을 해당 위치에 출력
 parm : row	- [in]  행
		col	- [in]  열
		val	- [in]  값
 retn : None
*/
VOID CMyListCtrl::SetTextToNumU32(UINT32 row, UINT32 col, UINT32 val)
{
	TCHAR tzValue[64] = {NULL};

	swprintf_s(tzValue, 64, L"%u", val);
	SetItemText(row, col, tzValue);
}

/*
 desc : 2 Bytes의 Unsigned 숫자 값을 반환
 parm : row	- [in]  행
		col	- [in]  열
 retn : 값 반환
*/
DOUBLE CMyListCtrl::GetTextToDouble(UINT32 row, UINT32 col)
{
	TCHAR tzValue[64] = {NULL};
	GetItemText(row, col, tzValue, 64);
	return (DOUBLE)_wtof(tzValue);
}

/*
 desc : 4 Bytes의 Double 숫자 값을 해당 위치에 출력
 parm : row	- [in]  행
		col	- [in]  열
 retn : None
*/
VOID CMyListCtrl::SetTextToDouble(UINT32 row, UINT32 col, DOUBLE val, UINT8 point, BOOL space)
{
	TCHAR tzText[1024]		= {NULL};
	TCHAR tzPoint[32]	= {NULL};
	if (!space)	swprintf_s(tzPoint, _T("%%.%df"), point);
	else		swprintf_s(tzPoint, _T("%%.%df "), point);
	swprintf_s(tzText, 1024, tzPoint, val);
	SetItemText(row, col, tzText);
}

/*
 desc : 헤더 영역을 클릭한 경우, Sort 수행
 parm : nmhdr	- [in]  
		result	- [in]  
 retn : None
*/
VOID CMyListCtrl::OnLvnColumnclick(NMHDR *nmhdr, LRESULT *result)
{
	INT32 i	= 0;
	LPNMLISTVIEW pstList= reinterpret_cast<LPNMLISTVIEW>(nmhdr);
	STM_LISP stSort		= {NULL};

	/* 현재 리스트 컨트롤에 있는 데이터 총 자료 개수만큼 저장 */
	for (; i<GetItemCount(); i++)	SetItemData(i, i);

	/* 정렬 방식 지정 (Ascending, Descending) */
	m_bAscending	= !m_bAscending;

	/* 정렬 관련된 구조체 변수 생성 및 데이터 초기화 */
	stSort.hwnd		= GetSafeHwnd();
	stSort.column	= pstList->iSubItem;
	stSort.ascend	= (INT32)m_bAscending;

	/* 정렬 콜백 함수 호출 */
	SortItems(SortList, (LPARAM)&stSort);

	*result	= 0;
}

/*
 desc : 정렬 콜백 함수
 parm : lparam1	- [in]  
		lparam2	- [in]  
		sort	- [in]  
 retn : 실패 (음수), 성공 (0 or 양수)
*/
INT32 CMyListCtrl::SortList(LPARAM lparam1, LPARAM lparam2, LPARAM sort)
{
	LPM_LISP pstSort= (LPM_LISP)sort;
	CString strItem[2];
	CMyListCtrl *pListCtrl;

	/* 콜백 함수를 통해 상위 개념의 자신의 객체 얻기 */
	pListCtrl	= (CMyListCtrl*)CWnd::FromHandle(pstSort->hwnd);

	/* Sort 비교 대상 문자열 얻기 */
	strItem[0]	= pListCtrl->GetItemText((INT32)lparam1, pstSort->column);
	strItem[1]	= pListCtrl->GetItemText((INT32)lparam2, pstSort->column);
	strItem[0].MakeLower();
	strItem[1].MakeLower();

	if (pstSort->ascend)	return strItem[0].Compare(strItem[1]);
	else					return strItem[1].Compare(strItem[0]);
}