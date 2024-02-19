#pragma once

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Imm.h>

// 라이브러리 호출
#pragma comment(lib, "imm32.lib")

#define	TRANS_BACK		-1

typedef enum class __en_data_input_text_mode__ : UINT8
{
	en_none		= 0x00, /* unknown */

	en_int8		= 0x01,
	en_uint8	= 0x02,
	en_int16	= 0x03,
	en_uint16	= 0x04,
	en_int32	= 0x05,
	en_uint32	= 0x06,
	en_int64	= 0x07,
	en_uint64	= 0x08,
	en_float	= 0x09,
	en_double	= 0x0a,

	en_string	= 0x11,	/* String input */
	en_hex2		= 0x12,	/* Binary input */
	en_hex8		= 0x13,	/* octal input */
	en_hex10	= 0x14,	/* decimal input */
	en_hex16	= 0x15,	/* hexa-decimal input */

}	ENM_DITM;

typedef enum class __en_data_input_language_mode__ : UINT8
{
	en_none		= 0x00,	// 모름
	en_mixed	= 0x01,	// 한글과 영어 혼합 입력 가능
	en_korean	= 0x02,	// 한글만 입력 가능
	en_english	= 0x03,	// 영어만 입력 가능

}	ENM_DILM;

/* 멤버 변수의 변수 종류 */
typedef enum class __en_member_numberic_use_sign__ : UINT8
{
	en_unsign	= 0x00,	/* +/- 부호 없음 */
	en_sign		= 0x01,	/* +/- 부호 있음 */

}	ENM_MNUS;

class CMyEdit : public CEdit
{
public:
	CMyEdit();
	virtual ~CMyEdit();

// 가상함수
protected:

	virtual VOID		LButtonUp();
	virtual VOID		LButtonDown();
	virtual VOID		LButtonDblDown();
	virtual VOID		RButtonUp();
	virtual VOID		RButtonDown();
	virtual VOID		RButtonDblDown();

public:

	// 가상함수 재정의
	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual void		PreSubclassWindow();

private:

	// 변수
	INT32				m_i32PreferredHeight;
	INT32				m_i32FontSize;

	HIMC				m_hIMC;

	CFont				m_fText;

	// 함수
	INT32				CalcPreferredHeight();

	VOID				SetHanMode();
	VOID				SetEngMode();

protected:

	ENM_DITM			m_enType;		/* 에디트 컨트롤 내에 2, 8, 10, 16 진수 입력 모드인지 문자열 모드 아닌지 */
	ENM_DILM			m_enLang;		/* 에디트 박스 내에 한영 전환 혹은 한글만 혹은 영문만 입력 가능하도록 설정 */
	ENM_MNUS			m_enSign;		/* Sing 사용 여부 (0 : Disable, 1 : Enable) */

	BOOL				m_bIsBtnEvent;	/* 버튼 이벤트 발생 여부 */
	BOOL				m_bIsEnterKey;
	BOOL				m_bIsReadOnly;
	BOOL				m_bIsMouseClick;
	BOOL				m_bIsMouseDblClick;
	BOOL				m_bUseMinMax;	// Min / Max 값 사용 여부
	BOOL				m_bCentType;	/* 0x00 : Normal, 0x01 : Horz Cent, 0x02 : Vert Cent, 0x03 : Both Cent */

	UINT8				m_u8Point;	/* Floating anf Double Points Count */

	INT8				m_i8Value;
	UINT8				m_u8Value;
	INT16				m_i16Value;
	UINT16				m_u16Value;
	INT32				m_i32Value;
	UINT32				m_u32Value;
	INT64				m_i64Value;
	UINT64				m_u64Value;
	FLOAT				m_fValue;
	DOUBLE				m_dbValue;

	INT8				m_i8Min, m_i8Max;
	UINT8				m_u8Min, m_u8Max;
	INT16				m_i16Min, m_i16Max;
	UINT16				m_u16Min, m_u16Max;
	INT32				m_i32Min, m_i32Max;
	UINT32				m_u32Min, m_u32Max;
	INT64				m_i64Min, m_i64Max;
	UINT64				m_u64Min, m_u64Max;
	DOUBLE				m_dbMin, m_dbMax;

	COLORREF			m_clrText;
	COLORREF			m_clrBack;
	COLORREF			m_clrBorder;

	CBrush				m_brushBg;

	CRect				m_rNCBottom;
    CRect				m_rNCTop;

protected:

	VOID				UpdateData();

	VOID				SetTextToVal(INT64 data, BOOL redraw);
	VOID				SetTextToVal(UINT64 data, BOOL redraw);


public:

	INT32				GetPreferredHeight();
	VOID				SetTextToNum(INT8 data, BOOL redraw=FALSE);
	VOID				SetTextToNum(UINT8 data, BOOL redraw=FALSE);
	VOID				SetTextToNum(INT16 data, BOOL redraw=FALSE);
	VOID				SetTextToNum(UINT16 data, BOOL redraw=FALSE);
	VOID				SetTextToNum(INT32 data, BOOL redraw=FALSE);
	VOID				SetTextToNum(UINT32 data, BOOL redraw=FALSE);
	VOID				SetTextToNum(INT64 data, BOOL redraw=FALSE);
	VOID				SetTextToNum(UINT64 data, BOOL redraw=FALSE);
	VOID				SetTextToNum(DOUBLE data, INT32 point, BOOL redraw=FALSE);
	VOID				SetTextToHex(UINT8 data, BOOL redraw=FALSE);
	VOID				SetTextToHex(UINT16 data, BOOL redraw=FALSE);
	VOID				SetTextToHex(UINT32 data, BOOL redraw=FALSE);
	VOID				SetTextToStr(PTCHAR data, BOOL redraw=FALSE);

	VOID				SetPlusInt64();

	INT64				GetTextToNum();
	DOUBLE				GetTextToDouble();
	UINT64				GetTextToHex();

	BOOL				IsMultilined();
	BOOL				IsReadOnly()	{	return m_bIsReadOnly;	}

	VOID				SetBtnEvent(BOOL flag)			{	m_bIsBtnEvent	= flag;	}
	VOID				SetEnterKey(BOOL flag)			{	m_bIsEnterKey	= flag;	}
	VOID				SetPreferredHeight(int iHeight=0);
	VOID				SetReadOnly(BOOL flag)			{	m_bIsReadOnly	= flag;	};
	VOID				SetMouseClick(BOOL flag)		{	m_bIsMouseClick	= flag;	};
	VOID				SetEditFont(LOGFONT *lf);
	BOOL				SetFontName(TCHAR *name, UINT32 size);
	VOID				SetInputType(ENM_DITM type)		{	m_enType	= type;		}
	VOID				SetInputMode(ENM_DILM lang);
	VOID				SetInputSign(ENM_MNUS sign)		{	m_enSign	= sign;		}
	VOID				SetInputPoint(UINT8 point)		{	m_u8Point	= point;	}
	VOID				SetTextEndLine(TCHAR *text);
	VOID				ResetAllData();
	BOOL				IsEmpty();

	VOID SetTextColor(COLORREF clr)
	{
						m_clrText = clr;
	}
	VOID SetBackColor(COLORREF clr)
	{
						m_clrBack = clr;
						m_brushBg.DeleteObject();
						m_brushBg.CreateSolidBrush(clr);
	}
	VOID SetUseMinMax(BOOL flag)				{	m_bUseMinMax = flag;			}
	VOID SetMinMaxNum(INT8 min, INT8 max)		{	m_i8Max = max; m_i8Min	= min;	}
	VOID SetMinMaxNum(UINT8 min, UINT8 max)		{	m_u8Max = max; m_u8Min	= min;	}
	VOID SetMinMaxNum(INT16 min, INT16 max)		{	m_i16Max= max; m_i16Min	= min;	}
	VOID SetMinMaxNum(UINT16 min, UINT16 max)	{	m_u16Max= max; m_u16Min	= min;	}
	VOID SetMinMaxNum(INT32 min, INT32 max)		{	m_i32Max= max; m_i32Min	= min;	}
	VOID SetMinMaxNum(UINT32 min, UINT32 max)	{	m_u32Max= max; m_u32Min	= min;	}
	VOID SetMinMaxNum(INT64 min, INT64 max)		{	m_i64Max= max; m_i64Min	= min;	}
	VOID SetMinMaxNum(UINT64 min, UINT64 max)	{	m_u64Max= max; m_u64Min	= min;	}
	VOID SetMinMaxNum(DOUBLE min, DOUBLE max)	{	m_dbMax = max; m_dbMin	= min;	}

	INT32 Compare(INT64 data);
	INT32 Compare(TCHAR *data);
	
	BOOL				IsCompareText(TCHAR *str);
	VOID				SelectedAll();
	INT64				GetTextToLen();


protected:

	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH		CtlColor(CDC* dc, UINT32 ctl_color);
	afx_msg VOID		OnChar(UINT32 nchar, UINT32 rep_cnt, UINT32 flags);
	afx_msg VOID		OnSetFocus(CWnd* pOldWnd);
	afx_msg VOID		OnKillFocus(CWnd* pNewWnd);
	afx_msg VOID		OnLButtonDown(UINT32 flags, CPoint point);
	afx_msg VOID		OnLButtonDblClk(UINT32 flags, CPoint point);
	afx_msg VOID		OnLButtonUp(UINT32 flags, CPoint point);
	afx_msg VOID		OnRButtonDown(UINT32 flags, CPoint point);
	afx_msg VOID		OnRButtonDblClk(UINT32 flags, CPoint point);
	afx_msg VOID		OnRButtonUp(UINT32 flags, CPoint point);
	afx_msg VOID		OnUpdate();
	afx_msg VOID		OnPaint();
	/* -------------------------------------------------------------------------- */
	/* 에디트 박스의 내용을 수직으로 수평하도록 하기 위해 추가됨 (Verical Center) */
	/* -------------------------------------------------------------------------- */
	afx_msg	BOOL		OnEraseBkgnd(CDC* dc);
};