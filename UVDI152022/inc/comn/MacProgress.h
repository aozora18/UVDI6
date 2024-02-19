
#ifndef __MAC_PROGRESS_CTRL_H__
#define __MAC_PROGRESS_CTRL_H__

#if _MSC_VER > 1000
#pragma once
#endif

class CMacProgress : public CProgressCtrl
{

public:
	CMacProgress();
	virtual ~CMacProgress();

// �����Լ� ������
protected:

// ���� ���� �� �Լ�
protected:
	int					m_nIndOffset;
	BOOL				m_bIndeterminate;
	CPen				m_penColor;
	CPen				m_penColorLight;
	CPen				m_penColorLighter;
	CPen				m_penColorDark;
	CPen				m_penColorDarker;
	CPen				m_penDkShadow;
	CPen				m_penShadow;
	CPen				m_penLiteShadow;
	COLORREF			m_crColor;
	COLORREF			m_crColorLight;
	COLORREF			m_crColorLighter;
	COLORREF			m_crColorLightest;
	COLORREF			m_crColorDark;
	COLORREF			m_crColorDarker;
	COLORREF			m_crDkShadow;
	COLORREF			m_crShadow;
	COLORREF			m_crLiteShadow;

	void				DrawVerticalBar(CDC *dc, const CRect rect);
	void				DrawHorizontalBar(CDC *dc, const CRect rect);
	void				DeletePens();
	void				CreatePens();
	void				GetColors();

// ���� �Լ�
public:

	BOOL				GetIndeterminate();
	COLORREF			GetColor();
	void				SetColor(COLORREF color);
	void				SetIndeterminate(BOOL indeterminate=TRUE);


// �޽����� �Լ�
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void		OnPaint();
	afx_msg void		OnTimer(UINT_PTR nIDEvent);
};

#endif
