// TransparentStatic.cpp : implementation file
//

#include "pch.h"
#include "MyStatic.h"


// CMyStatic

IMPLEMENT_DYNAMIC(CMyStatic, CStatic)
CMyStatic::CMyStatic()
{
	m_u8IsGradientFill	= 0x00;
	m_u8IsDrawBorder	= 0x00;
	m_u8IsDrawBorder	= 0x00;
	m_u8IsDrawBg		= 0x00;

	m_clrBg				= RGB(234, 228, 223);
//	m_clrBg				= GetSysColor(COLOR_ACTIVECAPTION);
	m_clrBorder			= COLOR_ACTIVEBORDER;
	m_clrGradient		= GetSysColor(COLOR_BTNFACE);
//	m_clrText			= GetSysColor(COLOR_CAPTIONTEXT);
	m_clrText			= RGB(0, 0, 0);

	m_hinstMsimg32		= LoadLibrary(_T("msimg32.dll"));
	if(!m_hinstMsimg32)
	{
		AfxMessageBox(L"Can't load MSIMG32.DLL", MB_ICONSTOP|MB_TOPMOST);
	}
	else
	{
		m_DllFuncGradFill = ((LPFNDLLFUNC1)GetProcAddress(m_hinstMsimg32, "GradientFill" ));
	}
#if 0
	SetTextFont(12, _T("돋음"), FW_NORMAL);
#endif
}

CMyStatic::~CMyStatic()
{
	if(m_ftText.GetSafeHandle())	m_ftText.DeleteObject();
	
	FreeLibrary(m_hinstMsimg32);
}


BEGIN_MESSAGE_MAP(CMyStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

/*
 desc : 화면 갱신
 parm : None
 retn : None
*/
VOID CMyStatic::OnPaint()
{
	CPaintDC dc(this);

	INT32 i32PrvMode;
	UINT32 dwStyle, dwText;
	HFONT hFont = NULL;
	CBrush csBrush, *pOldBrush = NULL;
	CRect rClient;
	CString szText;

	GetClientRect(rClient);

	// 경계선을 그릴경우
	if (m_u8IsDrawBorder)
	{
		CPen Pen(PS_SOLID, 1, m_clrBorder);
		CPen *pOldPen = dc.SelectObject(&Pen);
		dc.MoveTo(rClient.left,		rClient.top);
		dc.LineTo(rClient.right,	rClient.top);
		dc.LineTo(rClient.right,	rClient.bottom);
		dc.LineTo(rClient.left,		rClient.bottom);
		dc.LineTo(rClient.left,		rClient.top);
		dc.SelectObject(pOldPen);
	}

	// 영역 안에 들어간 문자
	GetWindowText(szText);
	// 스타일 정보
	dwStyle = GetStyle(), dwText = 0;

	// Map "Static Styles" to "Text Styles"
#define MAP_STYLE(src, dest)	if(dwStyle & (src)) dwText |= (dest)
#define NMAP_STYLE(src, dest)	if(!(dwStyle & (src))) dwText |= (dest)

	// 폰트 처리
	if(m_ftText.GetSafeHandle())
	{
		hFont = (HFONT)SelectObject(dc.m_hDC, (HFONT)m_ftText.m_hObject);
	}

	// 배경색 처리
	if (m_u8IsDrawBg)
	{
		MAP_STYLE(	SS_RIGHT,			DT_RIGHT					);
		MAP_STYLE(	SS_CENTER,			DT_CENTER					);
		MAP_STYLE(	SS_CENTERIMAGE,		DT_VCENTER | DT_SINGLELINE	);
		MAP_STYLE(	SS_NOPREFIX,		DT_NOPREFIX					);
		MAP_STYLE(	SS_WORDELLIPSIS,	DT_WORD_ELLIPSIS			);
		MAP_STYLE(	SS_ENDELLIPSIS,		DT_END_ELLIPSIS				);
		MAP_STYLE(	SS_PATHELLIPSIS,	DT_PATH_ELLIPSIS			);

		NMAP_STYLE(	SS_LEFTNOWORDWRAP | SS_CENTERIMAGE | SS_WORDELLIPSIS |
					SS_ENDELLIPSIS | SS_PATHELLIPSIS, DT_WORDBREAK);

		if(m_u8IsGradientFill)
		{
			TRIVERTEX	rcVertex[2];

			////////////////////////////////
			rClient.left++;
			rClient.top++;
			////////////////////////////////

			rcVertex[0].x		= rClient.left;
			rcVertex[0].y		= rClient.top;
			rcVertex[0].Red		= GetRValue(m_clrBg)<<8;	// color values from 0x0000 to 0xff00 !!!!
			rcVertex[0].Green	= GetGValue(m_clrBg)<<8;
			rcVertex[0].Blue	= GetBValue(m_clrBg)<<8;
			rcVertex[0].Alpha	= 0x0000;
			rcVertex[1].x		= rClient.right;
			rcVertex[1].y		= rClient.bottom;
			rcVertex[1].Red		= GetRValue(m_clrGradient)<<8;
			rcVertex[1].Green	= GetGValue(m_clrGradient)<<8;
			rcVertex[1].Blue	= GetBValue(m_clrGradient)<<8;
			rcVertex[1].Alpha	= 0;

			GRADIENT_RECT rect;
			rect.UpperLeft=0;
			rect.LowerRight=1;

			// fill the area 
			m_DllFuncGradFill(dc, rcVertex, 2, &rect, 1, GRADIENT_FILL_RECT_H);
		}
		else
		{
			// by sys&j : 이쁘지 않아 주석처리
			////////////////////////////////
			//rClient.left++;
			//rClient.top++;
			////////////////////////////////

			if (csBrush.CreateSolidBrush(m_clrBg))
			{
				pOldBrush = dc.SelectObject(&csBrush);
				dc.FillRect(rClient, &csBrush);
				dc.SelectObject(pOldBrush);
				csBrush.DeleteObject();
			}
		}
	}

	// 텍스트 처리
	if (szText.GetLength() > 0)
	{
		i32PrvMode = dc.SetBkMode(TRANSPARENT);			// 윈도 배경 투명하게
		dc.SetTextColor(m_clrText);						// 글자색 설정
		dc.DrawText(szText, rClient, dwText);			// 글자 뿌리기
		dc.SetBkMode(i32PrvMode);
	}

	if (hFont && m_ftText.GetSafeHandle())	::SelectObject(dc.m_hDC, hFont);

	// 가상함수 호출
	DoPaint();
}

/*
 desc : 컨트롤 속성 기본 설정
 parm : lf	- 폰트 정보
 retn : None
*/
VOID CMyStatic::SetTextFont(LOGFONT *lf)
{
	if(m_ftText.GetSafeHandle())
	{
		m_ftText.DeleteObject();
	}
	/* Font 생성 및 적용 */
	m_ftText.CreateFontIndirect(lf);
	SetFont(&m_ftText);
}
VOID CMyStatic::SetTextFont(INT32 size, TCHAR *name, INT32 bold)
{
	if(m_ftText.GetSafeHandle())	m_ftText.DeleteObject();

	/* Font 생성 및 적용 */
    m_ftText.CreateFont(size,						// 문자열 높이
						0,							// 너비
						0,							// 문자열 기울기
						0,							// 문자 개별 각도
						bold,						// 굵기
						0x00,						// 이탤릭
						0x00,						// 밑줄
						0,							// 취소선
						HANGUL_CHARSET,				// nCharSet
						OUT_STROKE_PRECIS,			// 가변폭폰트
						CLIP_CHARACTER_PRECIS,		// 고정폭폰트 
						DRAFT_QUALITY,				// nQuality
						DEFAULT_PITCH|FF_SWISS,		// 글꼴 이름
						name);						// 폰트 이름
	SetFont(&m_ftText);
}

/*
 desc : 컨트롤 속성 기본 설정
 parm : lf			- 폰트 정보
		border		- 경계선 그릴지 유무
		bg			- 배경색 채울지 유무
		grad		- 그라디언트 적용할지 유무
		clr_border, bg, grad, text	- 각각의 색상
 retn : None
*/
VOID CMyStatic::SetBaseProp(BOOL border, BOOL bg, BOOL grad,
							COLORREF clr_border, COLORREF clr_bg, COLORREF clr_grad, COLORREF clr_text)
{
	SetBorderColor(clr_border);
	SetBgColor(clr_bg);
	SetGradientColor(clr_grad);

	SetGradientFill(grad);
	SetDrawBorder(border);
	SetDrawBg(bg);

	SetTextColor(clr_text);
}

/*
 desc : 기존 입력된 문자열과 비교 했을 때 동일한지 여부
 parm : text	- [in]  비교하기 위한 문자열
 retn : TRUE - 동일하다, FALSE - 동일하지 않다
*/
BOOL CMyStatic::IsCompareText(PTCHAR text)
{
	TCHAR tzText[1024]	= {NULL};
	
	GetWindowText(tzText, 1024);
	return 0 == wcscmp(tzText, text) ? TRUE : FALSE;
}

/*
 desc : 현재 입력된 텍스트가 있는지 여부
 parm : None
 retn : TRUE - 없다. FALSE - 있다.
*/
BOOL CMyStatic::IsEmpty()
{
	TCHAR tzText[32]	= {NULL};
	GetWindowText(tzText, 32);
	return _tcslen(tzText) > 0 ? FALSE : TRUE;
}

/*
 desc : 현재 입력된 문자열을 unsigned integer32 값으로 반환
 parm : None
 retn : unsigned integer32 값 반환
*/
UINT32 CMyStatic::GetTextToUint32()
{
	TCHAR tzText[32]	= {NULL};
	GetWindowText(tzText, 32);

	return (UINT32)_wtoi(tzText);
}

/*
 desc : 현재 입력된 문자열을 floating 값으로 반환
 parm : None
 retn : floating 값 반환
*/
FLOAT CMyStatic::GetTextToFloat()
{
	TCHAR tzText[32]	= {NULL};
	GetWindowText(tzText, 32);

	return (FLOAT)_wtof(tzText);
}

/*
 desc : 숫자로 입력된 데이터를 컨트롤에 출력
 parm : data	- [in]  숫자 형식의 값
		point	- [in]  소숫점 이하 자릿수 개수
		redraw	- [in]  갱신 비활성화 -> 값 갱신 -> 갱신 활성화
 retn : None
*/
VOID CMyStatic::SetTextToNum(DOUBLE data, INT32 point, BOOL redraw)
{
	TCHAR szData[1024]	= {NULL};
	TCHAR szPoint[32]	= {NULL};

	if (redraw)	SetRedraw(FALSE);

	swprintf_s(szPoint, _T("%%.%df"), point);
	swprintf_s(szData, 1024, szPoint, data);
	SetWindowText(szData);

	if (redraw)
	{
		SetRedraw(TRUE);
		Invalidate(FALSE);
	}
}

/*
 desc : 문자열로 입력된 데이터를 컨트롤에 출력
 parm : data	- [in]  문자열 형식의 값
		redraw	- [in]  갱신 비활성화 -> 값 갱신 -> 갱신 활성화
 retn : None
*/
VOID CMyStatic::SetTextToStr(PTCHAR data, BOOL redraw)
{
	if (redraw)	SetRedraw(FALSE);

	SetWindowText(data);

	if (redraw)
	{
		SetRedraw(TRUE);
		Invalidate(FALSE);
	}
}

VOID CMyStatic::SetNotify()
{
	// 현재 스타일 가져오기
	DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);

	// SS_NOTIFY 스타일 설정
	dwStyle |= SS_NOTIFY;

	// 변경된 스타일 적용
	SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);

	// 컨트롤 다시 그리기 (선택적)
	Invalidate();
}
