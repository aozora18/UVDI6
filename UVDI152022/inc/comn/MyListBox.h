
#pragma once


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CMyListBox : public CListBox
{
/* ������ & �ı��� */
public:
	CMyListBox();
	virtual ~CMyListBox();

/* ���� �Լ� */
protected:

	virtual void		PreSubclassWindow();


/* ���� ���� */
protected:

	INT32				m_i32MaxStr;		/* �׸� �ִ� ���� �� */
	UINT32				m_u32Height;	/* �׸� ���� �� */
	CFont				m_ftText;

/* ���� �Լ� */
protected:

	VOID				UpdateHorizScrollBar(LPCTSTR str);


/* ���� �Լ� */
public:

	INT32				AddString(LPTSTR str);
	INT32				InsertString(INT32 i, LPTSTR str);
	INT32				DeleteString(INT32 index);
	VOID				ResetContent();
	VOID				SetSel(INT32 index);
	VOID				DeleteSelString();

	INT64				GetSelNum();
	INT64				GetTextToNum(UINT32 index);

	VOID				SetLogFont(LPLOGFONT lf);
	VOID				AddNumber(UINT32 data);
	VOID				AddNumber(UINT64 data);
	VOID				AddNumber(INT32 data);
	VOID				AddNumber(INT64 data);
	VOID				AddNumber(DOUBLE data, INT32 point);
	VOID				SortingInt(UINT8 flag=0x00);

	BOOL				SelectDelete();

	VOID				UpdateScrollBar();


/* �޽��� �� */
protected:

	DECLARE_MESSAGE_MAP()
};
