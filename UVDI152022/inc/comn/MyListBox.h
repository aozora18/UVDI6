
#pragma once


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CMyListBox : public CListBox
{
/* 생성자 & 파괴자 */
public:
	CMyListBox();
	virtual ~CMyListBox();

/* 가상 함수 */
protected:

	virtual void		PreSubclassWindow();


/* 로컬 변수 */
protected:

	INT32				m_i32MaxStr;		/* 항목 최대 길이 값 */
	UINT32				m_u32Height;	/* 항목 높이 값 */
	CFont				m_ftText;

/* 로컬 함수 */
protected:

	VOID				UpdateHorizScrollBar(LPCTSTR str);


/* 공용 함수 */
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


/* 메시지 맵 */
protected:

	DECLARE_MESSAGE_MAP()
};
