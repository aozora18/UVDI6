
#ifndef	__MAC_BUTTON_H__
#define	__MAC_BUTTON_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define CHECKBOX_HEIGHT		12
#define TYPE_STANDARD		0
#define TYPE_CHECKBOX		1
#define TYPE_RADIO			2
#define TYPE_MORE			3

#define IMAGE_EFFECT_NONE	0
#define IMAGE_EFFECT_RAISED	1
#define IMAGE_EFFECT_SUNKEN	2

#define CHECK_STYLE_CHECK	0
#define CHECK_STYLE_CROSS	1

#define ALIGN_TEXT_LEFT		0
#define ALIGN_TEXT_RIGHT	1

class CMacButton : public CButton
{

public:
	CMacButton();
	virtual ~CMacButton();

/* ���� �Լ� */
protected:
	virtual VOID		DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual VOID		PreSubclassWindow();

private:
	VOID				DrawStandardButton(CDC *pDC, const CRect &rect, UINT32 nStyle, UINT32 nState);

/* ���� ���� */
protected:

//	TCHAR				m_szFontName[LF_FACESIZE];

	INT32				m_nMultiLines;	/* If type is BS_MULTILINE, �� �� */ /* Added by JsKang 2018.06.27 */

	INT32				m_nCheckStyle;
	INT32				m_nImageEffect;
	INT32				m_nType;
	INT32				m_nCheck;
	INT32				m_iFontSize;

	BOOL				m_bBold;
	BOOL				m_bMouseDown;

	HICON				m_hIconEnable;
	HICON				m_hIconDisable;
	HBITMAP				m_hBitmap;

	CFont				m_csFont;
	CPen				m_penHilight;
	CPen				m_penFace;
	CPen				m_penLiteFace;
	CPen				m_penLiteShadow;
	CPen				m_penShadow;
	CPen				m_penDkShadow;
	CPen				m_penDarkDkShadow;

	COLORREF			m_crHilight;
	COLORREF			m_crLiteFace;
	COLORREF			m_crFace;
	COLORREF			m_crLiteShadow;
	COLORREF			m_crShadow;
	COLORREF			m_crDkShadow;
	COLORREF			m_crDarkDkShadow;
	COLORREF			m_crText;

	CSize				m_sizeImage;

	HBITMAP				GetDitherBitmap(CDC *pDC, COLORREF crColor1, COLORREF crColo2);

	COLORREF			LightenColor(COLORREF crColor, BYTE byIncreaseVal);
	COLORREF			DarkenColor(COLORREF crColor, BYTE byReduceVal);

	CRect				GetCheckRect(const CRect &rect, UINT32 nStyle);

	VOID				DrawButton(LPDRAWITEMSTRUCT lpDIS);
	VOID				DrawCheckBox(CDC *pDC, CRect rect, UINT32 nStyle, UINT32 nState);
	VOID				DrawRadioButton(CDC *pDC, CRect rect, UINT32 nStyle, UINT32 nState);
	VOID				DrawPushLikeButton(CDC *pDC, const CRect &rect, UINT32 nStyle, UINT32 nState);
	VOID				DrawPressedPushButton(CDC *pDC, const CRect &rect);
	VOID				DrawUnpressedPushButton(CDC *pDC, const CRect &rect);
	VOID				DrawCornerShadows(CDC *pDC, const CRect &rect);
	VOID				DrawImage(CDC *pDC, CRect &rect, UINT32 nStyle, UINT32 nState);
	VOID				DrawButtonText(CDC *pDC, CRect &rect, const CString &sText, UINT32 nStyle, UINT32 nState);
	VOID				GetColors();
	
	VOID				CreatePens();
	VOID				DeletePens();

/* ���� �Լ� */
public:
	VOID				SetLogFont(LOGFONT lf);
	VOID				SetBgColor(COLORREF bg)		{	m_crFace = bg;		}
	BOOL				IsBgColor(COLORREF bg)		{	return m_crFace == bg;	}
	COLORREF			GetBgColor()				{	return m_crFace;	}
	VOID				SetCheck(INT32 nCheck);							// Sets the button's checked state
	INT32				GetCheck() const;								// Gets the button's checked state
	VOID				SetBold(BOOL bBold=TRUE);						// Sets the button's bold flag
	BOOL				GetBold() const;								// Gets the button's bold flag
	VOID				SetImageEffect(INT32 nEffect);					// Sets the image effect
	VOID				SetIconID(UINT32 enable, UINT32 disable);
	HBITMAP				SetBitmap(HBITMAP hBitmap);						// Sets the button's bitmap
	VOID				SetMultiLines(INT32 lines)	{	m_nMultiLines = lines;	};
	VOID				SetTextColor(COLORREF bg) { m_crText = bg; }

	VOID				RedrawCheck();
/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnSysColorChange();
	afx_msg VOID		OnLButtonDblClk(UINT32 nFlags, CPoint point);
};

class CMacCheckBox : public CMacButton
{
	
public:
	CMacCheckBox();
	virtual ~CMacCheckBox();

/* ���� �Լ� */
protected:
	virtual VOID		DrawItem(LPDRAWITEMSTRUCT lpdis);

/* ���� ���� */
protected:
	BOOL				m_bReadOnly;

/* ���� �Լ� */
public:

	INT32				GetCheckStyle() const;								// Gets the check style
	VOID				SetCheckStyle(INT32 nStyle = CHECK_STYLE_CHECK);	// Sets the check style
	VOID				SetReadOnly(BOOL flag)	{	m_bReadOnly = flag;	}

/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnLButtonDown(UINT32 nFlags, CPoint point);
	afx_msg VOID		OnLButtonUp(UINT32 nFlags, CPoint point);
	afx_msg VOID		OnKeyUp(UINT32 nChar, UINT32 nRepCnt, UINT32 nFlags);
	afx_msg VOID		OnMouseMove(UINT32 nFlags, CPoint point);
	afx_msg VOID		OnLButtonDblClk(UINT32 nFlags, CPoint point);
	afx_msg VOID		OnKeyDown(UINT32 nChar, UINT32 nRepCnt, UINT32 nFlags);
};

class CMacRadioButton : public CMacButton
{
public:
	CMacRadioButton();
	virtual ~CMacRadioButton();

private:
	VOID				UncheckOtherRadios();

/* ���� �Լ� */
protected:
	virtual BOOL		PreTranslateMessage(MSG* pMsg);
	virtual VOID		DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

/* ���� ���� */
protected:

/* ���� �Լ� */
public:


/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnLButtonUp(UINT32 nFlags, CPoint point);
	afx_msg VOID		OnLButtonDown(UINT32 nFlags, CPoint point);
	afx_msg VOID		OnMouseMove(UINT32 nFlags, CPoint point);
	afx_msg VOID		OnLButtonDblClk(UINT32 nFlags, CPoint point);
	afx_msg VOID		OnSysColorChange();
	afx_msg VOID		OnKeyDown(UINT32 nChar, UINT32 nRepCnt, UINT32 nFlags);
	afx_msg VOID		OnKeyUp(UINT32 nChar, UINT32 nRepCnt, UINT32 nFlags);
	afx_msg VOID		OnSetFocus(CWnd* pOldWnd);
};

class CMacMoreButton : public CMacButton
{
public:
	CMacMoreButton();
	virtual ~CMacMoreButton();

private:
	INT32				m_nAlignText;
	BOOL				m_bMoreShown;
	BOOL				m_bMouseDown;

/* ���� �Լ� */
protected:
	virtual VOID		PreSubclassWindow();
	virtual VOID		DrawItem(LPDRAWITEMSTRUCT lpdis);

/* ���� ���� */
protected:
	BOOL				m_bDoEffect;

/* ���� �Լ� */
public:

	BOOL				GetMoreShown() const;		// Gets the flag specifying whether "more" is shown
	VOID				SetTextAlign(INT32 nAlign);	// Sets the text alignment
	VOID				SetColor(COLORREF crColor);	// Sets the button color
	

/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnLButtonUp(UINT32 nFlags, CPoint point);
	afx_msg VOID		OnLButtonDown(UINT32 nFlags, CPoint point);
};

class CMacRoundButton : public CMacButton
{
public:
	CMacRoundButton();
	virtual ~CMacRoundButton();

/* ���� �Լ� */
protected:
	virtual VOID		PreSubclassWindow();
	virtual VOID		DrawItem(LPDRAWITEMSTRUCT lpdis);

/* ���� ���� */
protected:

	INT32				m_i32Radius;
	BOOL				m_bDrawDashedFocusCircle;
	CRgn				m_rgn;
	CPoint				m_ptCenter;

/* ���� �Լ� */
public:

/* �޽��� �� */
protected:

	DECLARE_MESSAGE_MAP()
};

#endif
