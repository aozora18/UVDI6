#pragma once

class CDrawMark
{
/* ������ & �ı��� */
public:

	CDrawMark();
	virtual ~CDrawMark();

// ���� �Լ�
protected:


// ���� ����
protected:

	HWND				m_hDrawWnd;	// �׷��� ���� �ڵ�

// ���� �Լ�
protected:


// ���� �Լ�
public:

	VOID				SetDrawHwnd(HWND hwnd)	{	m_hDrawWnd = hwnd;	}
	VOID				Draw(LPRECT inner, LPRECT outer, UINT8 reverse, UINT8 pattern);
	BOOL				Save(TCHAR *file, LPRECT inner, LPRECT outer, UINT8 reverse, UINT8 pattern);
};
