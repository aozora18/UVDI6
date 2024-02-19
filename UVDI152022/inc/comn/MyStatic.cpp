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
	SetTextFont(12, _T("����"), FW_NORMAL);
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
 desc : ȭ�� ����
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

	// ��輱�� �׸����
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

	// ���� �ȿ� �� ����
	GetWindowText(szText);
	// ��Ÿ�� ����
	dwStyle = GetStyle(), dwText = 0;

	// Map "Static Styles" to "Text Styles"
#define MAP_STYLE(src, dest)	if(dwStyle & (src)) dwText |= (dest)
#define NMAP_STYLE(src, dest)	if(!(dwStyle & (src))) dwText |= (dest)

	// ��Ʈ ó��
	if(m_ftText.GetSafeHandle())
	{
		hFont = (HFONT)SelectObject(dc.m_hDC, (HFONT)m_ftText.m_hObject);
	}

	// ���� ó��
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
			// by sys&j : �̻��� �ʾ� �ּ�ó��
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

	// �ؽ�Ʈ ó��
	if (szText.GetLength() > 0)
	{
		i32PrvMode = dc.SetBkMode(TRANSPARENT);			// ���� ��� �����ϰ�
		dc.SetTextColor(m_clrText);						// ���ڻ� ����
		dc.DrawText(szText, rClient, dwText);			// ���� �Ѹ���
		dc.SetBkMode(i32PrvMode);
	}

	if (hFont && m_ftText.GetSafeHandle())	::SelectObject(dc.m_hDC, hFont);

	// �����Լ� ȣ��
	DoPaint();
}

/*
 desc : ��Ʈ�� �Ӽ� �⺻ ����
 parm : lf	- ��Ʈ ����
 retn : None
*/
VOID CMyStatic::SetTextFont(LOGFONT *lf)
{
	if(m_ftText.GetSafeHandle())
	{
		m_ftText.DeleteObject();
	}
	/* Font ���� �� ���� */
	m_ftText.CreateFontIndirect(lf);
	SetFont(&m_ftText);
}
VOID CMyStatic::SetTextFont(INT32 size, TCHAR *name, INT32 bold)
{
	if(m_ftText.GetSafeHandle())	m_ftText.DeleteObject();

	/* Font ���� �� ���� */
    m_ftText.CreateFont(size,						// ���ڿ� ����
						0,							// �ʺ�
						0,							// ���ڿ� ����
						0,							// ���� ���� ����
						bold,						// ����
						0x00,						// ���Ÿ�
						0x00,						// ����
						0,							// ��Ҽ�
						HANGUL_CHARSET,				// nCharSet
						OUT_STROKE_PRECIS,			// ��������Ʈ
						CLIP_CHARACTER_PRECIS,		// ��������Ʈ 
						DRAFT_QUALITY,				// nQuality
						DEFAULT_PITCH|FF_SWISS,		// �۲� �̸�
						name);						// ��Ʈ �̸�
	SetFont(&m_ftText);
}

/*
 desc : ��Ʈ�� �Ӽ� �⺻ ����
 parm : lf			- ��Ʈ ����
		border		- ��輱 �׸��� ����
		bg			- ���� ä���� ����
		grad		- �׶���Ʈ �������� ����
		clr_border, bg, grad, text	- ������ ����
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
 desc : ���� �Էµ� ���ڿ��� �� ���� �� �������� ����
 parm : text	- [in]  ���ϱ� ���� ���ڿ�
 retn : TRUE - �����ϴ�, FALSE - �������� �ʴ�
*/
BOOL CMyStatic::IsCompareText(PTCHAR text)
{
	TCHAR tzText[1024]	= {NULL};
	
	GetWindowText(tzText, 1024);
	return 0 == wcscmp(tzText, text) ? TRUE : FALSE;
}

/*
 desc : ���� �Էµ� �ؽ�Ʈ�� �ִ��� ����
 parm : None
 retn : TRUE - ����. FALSE - �ִ�.
*/
BOOL CMyStatic::IsEmpty()
{
	TCHAR tzText[32]	= {NULL};
	GetWindowText(tzText, 32);
	return _tcslen(tzText) > 0 ? FALSE : TRUE;
}

/*
 desc : ���� �Էµ� ���ڿ��� unsigned integer32 ������ ��ȯ
 parm : None
 retn : unsigned integer32 �� ��ȯ
*/
UINT32 CMyStatic::GetTextToUint32()
{
	TCHAR tzText[32]	= {NULL};
	GetWindowText(tzText, 32);

	return (UINT32)_wtoi(tzText);
}

/*
 desc : ���� �Էµ� ���ڿ��� floating ������ ��ȯ
 parm : None
 retn : floating �� ��ȯ
*/
FLOAT CMyStatic::GetTextToFloat()
{
	TCHAR tzText[32]	= {NULL};
	GetWindowText(tzText, 32);

	return (FLOAT)_wtof(tzText);
}

/*
 desc : ���ڷ� �Էµ� �����͸� ��Ʈ�ѿ� ���
 parm : data	- [in]  ���� ������ ��
		point	- [in]  �Ҽ��� ���� �ڸ��� ����
		redraw	- [in]  ���� ��Ȱ��ȭ -> �� ���� -> ���� Ȱ��ȭ
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
 desc : ���ڿ��� �Էµ� �����͸� ��Ʈ�ѿ� ���
 parm : data	- [in]  ���ڿ� ������ ��
		redraw	- [in]  ���� ��Ȱ��ȭ -> �� ���� -> ���� Ȱ��ȭ
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
	// ���� ��Ÿ�� ��������
	DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);

	// SS_NOTIFY ��Ÿ�� ����
	dwStyle |= SS_NOTIFY;

	// ����� ��Ÿ�� ����
	SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);

	// ��Ʈ�� �ٽ� �׸��� (������)
	Invalidate();
}
