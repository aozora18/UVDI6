#pragma once

class CDrawMark
{
/* 생성자 & 파괴자 */
public:

	CDrawMark();
	virtual ~CDrawMark();

// 가상 함수
protected:


// 로컬 변수
protected:

	HWND				m_hDrawWnd;	// 그려질 윈도 핸들

// 로컬 함수
protected:


// 공용 함수
public:

	VOID				SetDrawHwnd(HWND hwnd)	{	m_hDrawWnd = hwnd;	}
	VOID				Draw(LPRECT inner, LPRECT outer, UINT8 reverse, UINT8 pattern);
	BOOL				Save(TCHAR *file, LPRECT inner, LPRECT outer, UINT8 reverse, UINT8 pattern);
};
