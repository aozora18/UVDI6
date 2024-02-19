
#ifndef	__MY_GROUP_BOX_H__
#define	__MY_GROUP_BOX_H__

#define BS_3D			11
#define BS_RECT			12

struct CMyLogFont : public LOGFONT
{
public:
	CMyLogFont(	LONG fontHeight	= 90, 
				LONG fontWeight	= FW_NORMAL,
				BYTE fontItalic = FALSE,
				LPCTSTR faceName= _T("Tahoma"))
	{
		memset(this, 0, sizeof(*this));	// clear the current object

		// store the height, weight, and italics
		lfHeight		= fontHeight;
		lfWeight		= fontWeight;
		lfItalic		= fontItalic;

		// make sure that font name is not too long
		ASSERT(_tcslen(faceName) < LF_FACESIZE);
		_tcscpy_s(lfFaceName, faceName);

		// set other default parameters
		lfOutPrecision	= OUT_STROKE_PRECIS;
		lfClipPrecision	= CLIP_STROKE_PRECIS;
		lfQuality		= PROOF_QUALITY;
		lfPitchAndFamily= DEFAULT_PITCH | FF_DONTCARE;
	}

	// sets a new font name
	VOID SetFaceName(LPCTSTR faceName)
	{
		if(!faceName)	return;
		ASSERT(_tcslen(faceName) < LF_FACESIZE);
		lstrcpy(lfFaceName, faceName);
	}

	// sets a new font rotation angle
	int SetFontAngle(INT32 angle)
	{
		int oldangle	= lfEscapement;
		lfEscapement	= angle;
		return	oldangle;
	}

	// sets the underline style
	int SetUnderline(INT32 state)
	{
		int oldstate	= lfUnderline;
		lfUnderline		= BYTE(state);
		return	oldstate;
	}

	// sets the strikeout style
	int SetStrikeOut(INT32 state)
	{
		int oldstate	= lfStrikeOut;
		lfStrikeOut		= BYTE(state);
		return	oldstate;
	}
};

class CMyGrpBox : public CButton
{
public:
	CMyGrpBox();
	virtual ~CMyGrpBox();

	VOID SetBoxColors(COLORREF dark_color = NULL, COLORREF light_color = NULL, BOOL redraw = 0x00);
	VOID GetBoxColors(COLORREF& color1, COLORREF& color2);

	VOID SetText(CString textStr, BOOL redraw = 0x00);
	CString GetText()		{	return m_txtString;		};

	VOID SetTextColor(COLORREF color, BOOL redraw = 0x00);
	COLORREF GetTextColor()	{	return m_txtColor;		};

	VOID SetLogFont(const LOGFONT* lpLogFont = (LOGFONT*)NULL);
	CFont* GetFont()		{	return m_txtFont;		};

	VOID SetTextAlign(INT32 TextAlign);
	int GetTextAlign()		{	return m_i32txtAlignment;	};

	VOID SetTextOffset(INT32 Voffset, INT32 Hoffset);

	VOID SetLineStyle(INT32 StyleLine = BS_3D);
	int GetLineStyle()		{	return m_i32LineStyle;		};

	VOID SetLineThickness(INT32 thickness, BOOL redraw = 0x00)
	{
		m_i32boxThickness=thickness;
		if (redraw)	Invalidate();
	};
	INT32 GetLineThickness()	{	return m_i32boxThickness;	};

	/* Modified by JsKang 2021-07-30 */
	VOID				InvalidateArea();

protected:
	virtual VOID		PreSubclassWindow();

private:
	COLORREF			m_boxColor1;
	COLORREF			m_boxColor2;
	COLORREF			m_boxColor1_d;
	COLORREF			m_boxColor2_d;
	BOOL				m_u8IsStyleBitmap;
	INT32				m_i32boxThickness;

	COLORREF			m_txtColor;
	CFont				*m_txtFont;
	CString				m_txtString;
	INT32				m_i32txtAlignment;    // text alignment (BS_LEFT, BS_CENTER, BS_RIGHT)
	INT32				m_i32txtHOffset;      // horizontal distance from left/right side to beginning/end of text
	INT32				m_i32txtVOffset;      // vertical distance to move font up
	INT32				m_i32LineStyle;       // line style (BS_FLAT, BS_3D, BS_RECT)

	CRect				m_boxRect;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnPaint();
	afx_msg HBRUSH		CtlColor(CDC* pDC, UINT32 nCtlColor);
};

#endif