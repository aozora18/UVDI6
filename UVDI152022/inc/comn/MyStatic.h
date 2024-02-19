#pragma once


typedef UINT32 (CALLBACK* LPFNDLLFUNC1)(HDC,CONST PTRIVERTEX,UINT32,CONST PVOID,UINT32,UINT32);

class CMyStatic : public CStatic
{
	DECLARE_DYNAMIC(CMyStatic)

public:
	CMyStatic();
	virtual ~CMyStatic();

// 가상함수 정의
protected:

	virtual void		DoPaint()	{					};

// 로컬 변수 및 함수
protected:

	BOOL				m_u8IsDrawBg;		// 배경색 그릴 것인지
	BOOL				m_u8IsDrawBorder;
	BOOL				m_u8IsGradientFill;

	CBrush				m_csBrushBg;

	COLORREF			m_clrBg;			// 배경색
	COLORREF			m_clrBorder;		// 경계선 색
	COLORREF			m_clrGradient;		// 그라디언트 컬러
	COLORREF			m_clrText;			// 글자색

	HINSTANCE			m_hinstMsimg32;

	LPFNDLLFUNC1		m_DllFuncGradFill;	// 외부 함수

	CFont				m_ftText;			// 글자 폰트

// 공용 변수 및 함수
public:

	BOOL				IsDrawBorder()							{	return m_u8IsDrawBorder;			}

	COLORREF			GetBgColor()							{	return m_clrBg;						}

	VOID				SetBgColor(COLORREF bg)					{	m_clrBg				= bg;			}
	VOID				SetBorderColor(COLORREF border)			{	m_u8IsDrawBorder	= 0x01;
																	m_clrBorder			= border;		}
	VOID				SetDrawBorder(BOOL is_draw=0x01)		{	m_u8IsDrawBorder	= is_draw;		}
	VOID				SetDrawBg(BOOL is_draw=0x01)			{	m_u8IsDrawBg		= is_draw;		}
	VOID				SetGradientFill(BOOL is_draw=0x01)		{	m_u8IsGradientFill	= is_draw;		}
	VOID				SetGradientColor(COLORREF grad)			{	m_clrGradient		= grad;			}
	VOID				SetTextFont(LOGFONT *lf);
	VOID				SetTextFont(INT32 size, TCHAR *name, INT32 bold=FW_NORMAL);
	VOID				SetTextColor(COLORREF text)				{	m_clrText			= text;			}
	VOID				SetBaseProp(BOOL border=0x00, BOOL bg=0x00, BOOL grad=0x00,
									COLORREF clr_border=RGB(0, 0, 0), COLORREF clr_bg=RGB(255, 255, 255),
									COLORREF clr_grad=RGB(0, 0, 0), COLORREF clr_text=RGB(0, 0, 0));
	BOOL				IsCompareText(PTCHAR text);
	BOOL				IsEmpty();

	UINT32				GetTextToUint32();
	FLOAT				GetTextToFloat();

	VOID				SetTextToNum(DOUBLE data, INT32 point, BOOL redraw=FALSE);
	VOID				SetTextToStr(PTCHAR data, BOOL redraw=FALSE);

	VOID				SetNotify();


// 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg	VOID		OnPaint();
};


