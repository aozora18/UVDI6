
#pragma once

class CMyListCtrl : public CListCtrl
{

// 생성자 & 파괴자
public:
	CMyListCtrl();
	virtual ~CMyListCtrl();

	  
// 가상 함수	
protected:

  
public:

/* 구조체 or 열거형 */
protected:

#pragma pack (push, 8)
	typedef struct __st_lvn_item_sort_param__
	{
		INT32			column;			/* 정렬 기준 컬럼 */
		INT32			ascend;			/* 올림차순 (1:TRUE) / 내림차순 (0:FALSE) */
		HWND			hwnd;			/* 리스트 컨트롤 핸들 */

	}	STM_LISP,	*LPM_LISP;

#pragma pack (pop)

// 로컬 변수
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


// 로컬 함수
protected:

	static INT32 CALLBACK SortList(LPARAM lparam1, LPARAM lparam2, LPARAM sort);


// 공용 함수
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

// 메시지 맵
protected:

	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnKeyDown(UINT32 key, UINT32 rep_cnt, UINT32 flags);
	afx_msg VOID		OnLvnColumnclick(NMHDR *nmhdr, LRESULT *result);
	afx_msg VOID		OnCustomDraw(NMHDR* nmhdr, LRESULT *result);
	afx_msg BOOL		OnNMClick(NMHDR *nmhdr, LRESULT *result);
};
