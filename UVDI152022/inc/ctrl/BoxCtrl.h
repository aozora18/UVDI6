
#pragma once

class CBoxCtrl : public CListBox
{
	DECLARE_DYNAMIC (CBoxCtrl)

/* ������ & �ı��� */
public:

	CBoxCtrl();
	virtual ~CBoxCtrl();

/* ���� ���� */
protected:

	CFont				m_csFont;

/* ���� �Լ� */
protected:

/* ���� �Լ� */
public:

	VOID				SetBoxFont(LOGFONT *lf);

protected:
	
	DECLARE_MESSAGE_MAP()

};