
#pragma once

class CMyListCtrl : public CListCtrl
{

// ������ & �ı���
public:
	CMyListCtrl();
	virtual ~CMyListCtrl();

	  
// ���� �Լ�	
protected:

  
public:

/* ����ü or ������ */
protected:

#pragma pack (push, 8)
	typedef struct __st_lvn_item_sort_param__
	{
		INT32			column;			/* ���� ���� �÷� */
		INT32			ascend;			/* �ø����� (1:TRUE) / �������� (0:FALSE) */
		HWND			hwnd;			/* ����Ʈ ��Ʈ�� �ڵ� */

	}	STM_LISP,	*LPM_LISP;

#pragma pack (pop)

// ���� ����
protected:
	
	TCHAR				m_tzSelText[1024];

	BOOL				m_bAscending;
	BOOL				m_bEnableNMClick;

	INT32				m_i32SortColumn;
	INT32				m_i32SelRow;
    INT32				m_i32SelCol;

	COLORREF			m_clrText;
	COLORREF			m_clrBack;

	CFont				m_fText;	// Text Font


// ���� �Լ�
protected:

	static INT32 CALLBACK SortList(LPARAM lparam1, LPARAM lparam2, LPARAM sort);


// ���� �Լ�
public:

	VOID				SetLogFont(LOGFONT *lf);
	BOOL				SetFontName(TCHAR *name, UINT32 size);
	VOID				SetScrollEnd();
	VOID				SetScrollDown();
	
	INT32				GetCurSel();
	TCHAR				*GetSelText(UINT32 column);

	VOID				SetEnableNMClick(BOOL flag);

	VOID				SetDataColor(COLORREF text, COLORREF back);
	UINT16				GetTextToNumU16(UINT32 row, UINT32 col);
	UINT16				GetTextToNumI16(UINT32 row, UINT32 col);
	VOID				SetTextToNumU16(UINT32 row, UINT32 col, UINT16 val);
	UINT32				GetTextToNumI32(UINT32 row, UINT32 col);
	UINT32				GetTextToNumU32(UINT32 row, UINT32 col);
	VOID				SetTextToNumU32(UINT32 row, UINT32 col, UINT32 val);
	DOUBLE				GetTextToDouble(UINT32 row, UINT32 col);
	VOID				SetTextToDouble(UINT32 row, UINT32 col, DOUBLE val, UINT8 point, BOOL space=FALSE);

	VOID				SetCurSel(INT32 sel)	{	m_i32SelRow = sel;		};

// �޽��� ��
protected:

	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnKeyDown(UINT32 key, UINT32 rep_cnt, UINT32 flags);
	afx_msg VOID		OnLvnColumnclick(NMHDR *nmhdr, LRESULT *result);
	afx_msg VOID		OnCustomDraw(NMHDR* nmhdr, LRESULT *result);
	afx_msg BOOL		OnNMClick(NMHDR *nmhdr, LRESULT *result);
};
