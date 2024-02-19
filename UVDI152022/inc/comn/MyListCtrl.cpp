
#include "pch.h"
#include "MyListCtrl.h"

/*
 note : ����Ʈ ��Ʈ�� �Ӽ� �����ϱ�
		No Labe Wrap	: TRUE
		No Column Header: TRUE
*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : None
 retn : None
*/
CMyListCtrl::CMyListCtrl()
{
	m_bEnableNMClick= FALSE;	/* �⺻������ NM Click Event �θ𿡰� �˸� */
	m_bAscending	= FALSE;	/* �⺻ ���� ��� ���� */

	wmemset(m_tzSelText, 0x00, 1024);

	/* ���õ� �⺻ ���� �� ���� */
	m_clrText	= RGB(255, 255, 255);
	m_clrBack	= RGB(0, 0, 0);
}

/*
 desc : �ı���
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
 desc : ��Ʈ ����
 parm : lf	- [in]  �α� ��Ʈ
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
 desc : ���ο� ��Ʈ �̸� ����
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
 desc : �ؽ�Ʈ�� ���õ� ��� ������ ����
 parm : text	- [in]  ���õ� ���ڻ�
		back	- [in]  ���õ� ����
 retn : None
*/
VOID CMyListCtrl::SetDataColor(COLORREF text, COLORREF back)
{
	m_clrText	= text;
	m_clrBack	= back;
}

/*
 desc : NM Click Event�� �ڽ��� �θ𿡰� �˸��� ��� On/Off
 parm : flag	- [in]  TRUE : �ڽ��� �θ𿡰� NM Click Event �˸��� �ʴ´�.
						FALSE: �ڽ��� �θ𿡰� NM Click Event �˷��ش�.
 retn : None
*/
VOID CMyListCtrl::SetEnableNMClick(BOOL flag)
{
	m_bEnableNMClick	= flag;
}

/*
 desc : ��ũ���� �� �Ʒ��� �̵� ��Ŵ
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
 desc : ��ũ�� �Ʒ��� 1�� �̵�
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
 desc : ���� ���õǾ� �ִ� ��ġ ã��
 parm : None
 retn : ���õǾ� �ִ� ��ġ �� ��ȯ (���� ���� ����)
 note : ������ ���õǾ� �ִ���, �� ó�� ã���͸� ����
*/
INT32 CMyListCtrl::GetCurSel()
{
#if 0
	INT32 i, i32Count	= GetItemCount();

	for (i=0; i<i32Count; i++)
	{
		// ���� ���õǾ� �ִ� �׸����� Ȯ��
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
 desc : ��ư�� ���� ���
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
 desc : ���콺 Ŭ���� ���
 parm : nmhdr	- [in]  ???
		result	- [in]  ???
 retn : TRUE (�θ𿡰� ���� ����) / FALSE (�θ𿡰� ������
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
 desc : ���� ���õ� �׸��� ���ڿ� ��ȯ
 parm : column	- [in]  ���� �÷� ��ġ
 retn : ���õ� ���ڿ� (�ִ� 1024)
*/
TCHAR *CMyListCtrl::GetSelText(UINT32 column)
{
	wmemset(m_tzSelText, 0x00, 1024);
	GetItemText(m_i32SelRow, column, m_tzSelText, 1024);
	return m_tzSelText;
}

/*
 desc : 2 Bytes�� Unsigned ���� ���� ��ȯ
 parm : row	- [in]  ��
		col	- [in]  ��
 retn : �� ��ȯ
*/
UINT16 CMyListCtrl::GetTextToNumU16(UINT32 row, UINT32 col)
{
	TCHAR tzValue[64] = {NULL};
	GetItemText(row, col, tzValue, 64);
	return (UINT16)_wtoi64(tzValue);
}

/*
 desc : 2 Bytes�� Signed ���� ���� ��ȯ
 parm : row	- [in]  ��
		col	- [in]  ��
 retn : �� ��ȯ
*/
UINT16 CMyListCtrl::GetTextToNumI16(UINT32 row, UINT32 col)
{
	TCHAR tzValue[64] = {NULL};
	GetItemText(row, col, tzValue, 64);
	return (INT16)_wtoi64(tzValue);
}

/*
 desc : 2 Bytes�� Unsigned ���� ���� �ش� ��ġ�� ���
 parm : row	- [in]  ��
		col	- [in]  ��
		val	- [in]  ��
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
 desc : 4 Bytes�� Unsigned ���� ���� ��ȯ
 parm : row	- [in]  ��
		col	- [in]  ��
 retn : �� ��ȯ
*/
UINT32 CMyListCtrl::GetTextToNumU32(UINT32 row, UINT32 col)
{
	TCHAR tzValue[64] = {NULL};
	GetItemText(row, col, tzValue, 64);
	return (UINT32)_wtoi64(tzValue);
}

/*
 desc : 4 Bytes�� Signed ���� ���� ��ȯ
 parm : row	- [in]  ��
		col	- [in]  ��
 retn : �� ��ȯ
*/
UINT32 CMyListCtrl::GetTextToNumI32(UINT32 row, UINT32 col)
{
	TCHAR tzValue[64] = {NULL};
	GetItemText(row, col, tzValue, 64);
	return (INT32)_wtoi64(tzValue);
}

/*
 desc : 4 Bytes�� Unsigned ���� ���� �ش� ��ġ�� ���
 parm : row	- [in]  ��
		col	- [in]  ��
		val	- [in]  ��
 retn : None
*/
VOID CMyListCtrl::SetTextToNumU32(UINT32 row, UINT32 col, UINT32 val)
{
	TCHAR tzValue[64] = {NULL};

	swprintf_s(tzValue, 64, L"%u", val);
	SetItemText(row, col, tzValue);
}

/*
 desc : 2 Bytes�� Unsigned ���� ���� ��ȯ
 parm : row	- [in]  ��
		col	- [in]  ��
 retn : �� ��ȯ
*/
DOUBLE CMyListCtrl::GetTextToDouble(UINT32 row, UINT32 col)
{
	TCHAR tzValue[64] = {NULL};
	GetItemText(row, col, tzValue, 64);
	return (DOUBLE)_wtof(tzValue);
}

/*
 desc : 4 Bytes�� Double ���� ���� �ش� ��ġ�� ���
 parm : row	- [in]  ��
		col	- [in]  ��
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
 desc : ��� ������ Ŭ���� ���, Sort ����
 parm : nmhdr	- [in]  
		result	- [in]  
 retn : None
*/
VOID CMyListCtrl::OnLvnColumnclick(NMHDR *nmhdr, LRESULT *result)
{
	INT32 i	= 0;
	LPNMLISTVIEW pstList= reinterpret_cast<LPNMLISTVIEW>(nmhdr);
	STM_LISP stSort		= {NULL};

	/* ���� ����Ʈ ��Ʈ�ѿ� �ִ� ������ �� �ڷ� ������ŭ ���� */
	for (; i<GetItemCount(); i++)	SetItemData(i, i);

	/* ���� ��� ���� (Ascending, Descending) */
	m_bAscending	= !m_bAscending;

	/* ���� ���õ� ����ü ���� ���� �� ������ �ʱ�ȭ */
	stSort.hwnd		= GetSafeHwnd();
	stSort.column	= pstList->iSubItem;
	stSort.ascend	= (INT32)m_bAscending;

	/* ���� �ݹ� �Լ� ȣ�� */
	SortItems(SortList, (LPARAM)&stSort);

	*result	= 0;
}

/*
 desc : ���� �ݹ� �Լ�
 parm : lparam1	- [in]  
		lparam2	- [in]  
		sort	- [in]  
 retn : ���� (����), ���� (0 or ���)
*/
INT32 CMyListCtrl::SortList(LPARAM lparam1, LPARAM lparam2, LPARAM sort)
{
	LPM_LISP pstSort= (LPM_LISP)sort;
	CString strItem[2];
	CMyListCtrl *pListCtrl;

	/* �ݹ� �Լ��� ���� ���� ������ �ڽ��� ��ü ��� */
	pListCtrl	= (CMyListCtrl*)CWnd::FromHandle(pstSort->hwnd);

	/* Sort �� ��� ���ڿ� ��� */
	strItem[0]	= pListCtrl->GetItemText((INT32)lparam1, pstSort->column);
	strItem[1]	= pListCtrl->GetItemText((INT32)lparam2, pstSort->column);
	strItem[0].MakeLower();
	strItem[1].MakeLower();

	if (pstSort->ascend)	return strItem[0].Compare(strItem[1]);
	else					return strItem[1].Compare(strItem[0]);
}