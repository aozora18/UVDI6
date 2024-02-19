
#pragma once

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CMyCombo : public CComboBox
{
public:
	CMyCombo();
	virtual ~CMyCombo();

	// 가상함수 재정의
	virtual void		PreSubclassWindow();


protected:

	// 변수
	CFont				m_ftCombo;
	CBrush				m_csBrush;

	COLORREF			m_clrText;
	COLORREF			m_clrBack;

public:

	VOID				SetLogFont(LOGFONT *lf);
	INT64				GetLBTextToNum();
	VOID				AddNumber(INT64 num);
	VOID				AddNumber(UINT64 num);


protected:

	DECLARE_MESSAGE_MAP()
};