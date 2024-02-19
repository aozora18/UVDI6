// MyEdit.cpp : implementation file
//

#include "pch.h"
#include "MyEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMyEdit::CMyEdit()
	: m_rNCBottom(0, 0, 0, 0), m_rNCTop(0, 0, 0, 0)
{
	m_hIMC				= NULL;
	m_i32PreferredHeight= 0;

	m_enType			= ENM_DITM::en_string;
	m_enLang			= ENM_DILM::en_mixed;
	m_enSign			= ENM_MNUS::en_unsign;	/* !!! �ʱ� ���� �ݵ�� Unsign ���� !!! */
	m_u8Point			= 0x01;

	m_bIsBtnEvent		= FALSE;
	m_bIsEnterKey		= TRUE;
	m_bIsReadOnly		= FALSE;
	m_bIsMouseClick		= TRUE;
	m_bIsMouseDblClick	= TRUE;
	m_bUseMinMax		= FALSE;

	m_i8Value			= 0;
	m_u8Value			= 0;
	m_i16Value			= 0;
	m_u16Value			= 0;
	m_i32Value			= 0;
	m_u32Value			= 0;
	m_i64Value			= 0;
	m_u64Value			= 0;
	m_dbValue			= 0.0f;

	m_i8Min				= INT8_MIN;
	m_i8Max				= INT8_MAX;
	m_u8Min				= 0;
	m_u8Max				= UINT8_MAX;
	m_i16Min			= INT16_MIN;
	m_i16Max			= INT16_MAX;
	m_u16Min			= 0;
	m_u16Max			= UINT16_MAX;
	m_i32Min			= INT32_MIN;
	m_i32Max			= INT32_MAX;
	m_u32Min			= 0;
	m_u32Max			= UINT32_MAX;
	m_i64Min			= INT64_MIN;
	m_i64Max			= INT64_MAX;
	m_u64Min			= 0;
	m_u64Max			= UINT64_MAX;
	m_dbMin				= DBL_MIN;
	m_dbMax				= DBL_MAX;

	m_clrText			= RGB(0, 0, 0);
	m_clrBack			= RGB(255, 255, 255);

	/* Brush ���� */
	m_brushBg.CreateSolidBrush(m_clrBack);
}

CMyEdit::~CMyEdit()
{
	if (m_brushBg.GetSafeHandle())	m_brushBg.DeleteObject();
	if (m_fText.GetSafeHandle())	m_fText.DeleteObject();
}


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
 	ON_WM_CTLCOLOR_REFLECT()
 	ON_CONTROL_REFLECT(EN_UPDATE,	OnUpdate)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyEdit message handlers

VOID CMyEdit::PreSubclassWindow()
{
	SetPreferredHeight(GetPreferredHeight());

	CEdit::PreSubclassWindow();
}

BOOL CMyEdit::PreTranslateMessage(MSG* msg)
{
	if (msg->message == WM_KEYDOWN)
	{
		/* Edit���� MultiLine �Ӽ��� ��, Enter Key �԰��� �ϱ� ���� */
		if (m_bIsEnterKey && !m_bIsReadOnly &&
			((GetStyle() & ES_MULTILINE) == ES_MULTILINE) &&
			msg->wParam == VK_RETURN)
		{
			INT32 iLen = GetWindowTextLengthW();
			SetSel(iLen, iLen);
			ReplaceSel(L"\r\n");
		}
	}

	return CEdit::PreTranslateMessage(msg);
}

/*
 desc : OnPaint
 parm : None
 retn : None
*/
VOID CMyEdit::OnPaint()
{
	CWnd::OnPaint();

}

HBRUSH CMyEdit::CtlColor(CDC* dc, UINT32 nCtlColor)
{
	dc->SetBkColor(m_clrBack);
	dc->SetTextColor(m_clrText);

	return (HBRUSH)m_brushBg;
}

BOOL CMyEdit::OnEraseBkgnd(CDC* dc)
{

	return FALSE;
}

/*
 desc : Ű���� �̺�Ʈ ����ä��
 parm : nchar	- [in]  �Էµ� Ű���� ��
		repcnt	- [in]  �ݺ� �Է� ����
		flags	- [in]  �پ��� �÷��� ��
 retn : None
*/
VOID CMyEdit::OnChar(UINT32 nchar, UINT32 rep_cnt, UINT32 flags)
{
	// �б� ���� ��尡 �ƴ� ��츸 �ش�
	if (!m_bIsReadOnly)
	{
		if ((nchar != 8))	// Back Space Key �� �ƴ� ��츸
		{
			switch(m_enType)
			{
			/* Check numbers, decimal points, signs, etc */
			case ENM_DITM::en_int8	:
			case ENM_DITM::en_int16	:
			case ENM_DITM::en_int32	:
			case ENM_DITM::en_int64	:	if ((nchar < '0' || nchar > '9') && nchar != '-')	return;	break;
			case ENM_DITM::en_uint8	:
			case ENM_DITM::en_uint16:
			case ENM_DITM::en_uint32:
			case ENM_DITM::en_uint64:	if (nchar < '0' || nchar > '9')	return; break;
			case ENM_DITM::en_float	:
			case ENM_DITM::en_double:	if ((nchar < '0' || nchar > '9') && nchar != '.' && nchar != '-')	return;	break;
			/* Check binary ('0' ~ '1') */
			case ENM_DITM::en_hex2	:	if (nchar < '0' || nchar > '1')	return;	break;
			/* Check octal ('0' ~ '7') */
			case ENM_DITM::en_hex8	:	if (nchar < '0' || nchar > '7')	return;	break;
			/* Check decimal ('0' ~ '9') */
			case ENM_DITM::en_hex10	:	if (nchar < '0' || nchar > '9')	return;	break;
			/* Check hexa-decimal ('0' ~ '9' + 'A' ~ 'F') */
			case ENM_DITM::en_hex16	:
				if (nchar >= 'a' && nchar <= 'f')	nchar = toupper(nchar);
				if ((nchar < '0' || nchar > '9') && (nchar < 'A' || nchar > 'F'))	return;	break;
			default					:	break;	// EN_DITM::en_string ����� ���
			}
		}
//		CEdit::OnChar(nChar, nRepCnt, nFlags);	// CEdit::OnChar���� wParam�� lParam�� ó�� ������ ä��� CWnd::Default�� �θ��� �����Դϴ�.
		DefWindowProc(WM_CHAR, nchar, MAKEWPARAM(rep_cnt, flags));
	}
}

VOID CMyEdit::OnSetFocus(CWnd* pOldWnd)
{
	if (!m_bIsReadOnly)
	{
		switch(m_enLang)
		{
		case ENM_DILM::en_korean		:
		case ENM_DILM::en_english	:
			// �Է� ����� �ѿ� ȥ�ո�尡 �ƴ� ���, ��, ������ ��� Ư�� ���θ� �Է��� ������ ���
			if (m_hWnd)	m_hIMC	= ImmAssociateContext(m_hWnd, NULL);
			break;
		}

		CEdit::OnSetFocus(pOldWnd);
	}
}

VOID CMyEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	// ���� �Է¸�� ����
	if (m_hWnd && m_hIMC)	ImmAssociateContext(m_hWnd, m_hIMC);
}

VOID CMyEdit::LButtonDown()
{
	/* ���� �θ𿡰� ��ư �̺�Ʈ�� �˸� */
	if (m_bIsBtnEvent)
	{
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED),
								 (LPARAM)GetSafeHwnd());
	}
}
VOID CMyEdit::OnLButtonDown(UINT32 flags, CPoint point)
{
	if(m_bIsMouseClick)
	{
		LButtonDown();
		CEdit::OnLButtonDown(flags, point);
	}
}

VOID CMyEdit::LButtonDblDown()
{
}
VOID CMyEdit::OnLButtonDblClk(UINT32 flags, CPoint point)
{
	if(m_bIsMouseDblClick)
	{
		LButtonDblDown();
		CEdit::OnLButtonDblClk(flags, point);
	}
}

VOID CMyEdit::LButtonUp()
{
}
VOID CMyEdit::OnLButtonUp(UINT32 flags, CPoint point)
{
	if(m_bIsMouseClick)
	{
		LButtonUp();
		CEdit::OnLButtonUp(flags, point);
	}
}

VOID CMyEdit::RButtonDown()
{
}
VOID CMyEdit::OnRButtonDown(UINT32 flags, CPoint point)
{
	if(m_bIsMouseClick)
	{
		RButtonDown();
		CEdit::OnRButtonDown(flags, point);
	}
}

VOID CMyEdit::RButtonDblDown()
{
}
VOID CMyEdit::OnRButtonDblClk(UINT32 flags, CPoint point)
{
	if(m_bIsMouseClick)
	{
		RButtonDblDown();
		CEdit::OnRButtonDblClk(flags, point);
	}
}

VOID CMyEdit::RButtonUp()
{
}
VOID CMyEdit::OnRButtonUp(UINT32 flags, CPoint point)
{
	if(m_bIsMouseClick)
	{
		RButtonUp();
		CEdit::OnRButtonUp(flags, point);
	}
}

VOID CMyEdit::OnUpdate()
{
}

int CMyEdit::GetPreferredHeight()
{
	if (m_i32PreferredHeight == 0)
	{
		if (GetSafeHwnd())
		{
			m_i32PreferredHeight = CalcPreferredHeight();
		}
	}

	return m_i32PreferredHeight;
}

VOID CMyEdit::SetPreferredHeight(int iHeight)
{
	if (!GetSafeHwnd())	return;
	if (IsMultilined())	return;

	CRect rect;
	GetWindowRect(rect);
	rect.bottom = rect.top + iHeight;
	SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(),
				SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);

	m_i32PreferredHeight = iHeight;
}

BOOL CMyEdit::IsMultilined()
{
	if (!GetSafeHwnd())	return 0x00;

	UINT32 dwStyle = GetStyle();
	return (dwStyle & ES_MULTILINE) != 0;
}

/*
 desc : ��Ʈ ����
 parm : lf		- [in]  �����Ϸ��� ��Ʈ ������ ����� ����ü ������
		vert	- [in]  ��/�� ���� ��, ���� ���� (Center) (���� 1 �̻��̾�� ��ȿ��)
 retn : None
*/
VOID CMyEdit::SetEditFont(LOGFONT *lf)
{
	/* ���� ��Ʈ ��ü�� ���� */
	if (m_fText.GetSafeHandle())		m_fText.DeleteObject();
#if 1
	if (m_fText.CreateFontIndirect(lf))	SetFont(&m_fText);
#else
	if (m_fText.CreateFont(lf->lfHeight,
						   0,
						   lf->lfEscapement,
						   lf->lfOrientation,
						   lf->lfWeight,
						   lf->lfItalic,
						   lf->lfUnderline,
						   lf->lfStrikeOut,
						   lf->lfCharSet,
						   lf->lfOutPrecision,
						   lf->lfClipPrecision,
						   lf->lfQuality,
						   lf->lfPitchAndFamily,
						   lf->lfFaceName))	SetFont(&m_fText);
#endif
}

/*
 desc : ���ο� ��Ʈ �̸� ����
 parm : name	- [in]  Font Name
		size	- [in]  Font Size
 retn : TRUE or FALSE
*/
BOOL CMyEdit::SetFontName(TCHAR *name, UINT32 size)
{
	if (m_fText.GetSafeHandle())	m_fText.DeleteObject();
	if (!m_fText.CreateFont(size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET,
							 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
							 DEFAULT_PITCH | FF_SWISS, name))	return FALSE;
	SetFont(&m_fText);
	return TRUE;
}

INT32 CMyEdit::CalcPreferredHeight()
{
	CWnd *pParent	= NULL;

	if (!GetSafeHwnd())	return m_i32PreferredHeight;

	CFont* pFont = GetFont();
	if (!pFont)
	{
		pParent = GetParent();
		if (!pParent)	return m_i32PreferredHeight;

		pFont = pParent->GetFont();
		if (!pFont)		return m_i32PreferredHeight;
	}

	LOGFONT lf;
	VERIFY (pFont->GetLogFont(&lf));
	m_i32PreferredHeight = lf.lfHeight > 0 ? lf.lfHeight : -lf.lfHeight;

	UINT32 dwExStyle = GetExStyle();

	if ((dwExStyle & WS_BORDER) || (dwExStyle & WS_EX_CLIENTEDGE)
		|| (dwExStyle & WS_EX_STATICEDGE) || (dwExStyle & WS_EX_DLGMODALFRAME))
	{
		int iBorderHeight = ::GetSystemMetrics(SM_CYBORDER);
		m_i32PreferredHeight += iBorderHeight*4 + 3;
	}

	return m_i32PreferredHeight;
}

/*
 desc : �ѱ۸��� ��ȯ�ϴ� �Լ�
 parm : None
 retn : None
*/
VOID CMyEdit::SetHanMode()
{
	if (!m_hWnd)	return;

	HIMC hIMC = ImmGetContext(m_hWnd);
	DWORD dwConv, dwSent;
	DWORD dwTemp;

	ImmGetConversionStatus(hIMC,&dwConv,&dwSent);
	dwTemp = dwConv & ~IME_CMODE_LANGUAGE;
	dwTemp |= IME_CMODE_NATIVE;
	dwConv = dwTemp;

	ImmSetConversionStatus(hIMC,dwConv,dwSent);
	ImmReleaseContext(m_hWnd, hIMC);
}

/*
 desc : �������� ��ȯ�ϴ� �Լ�
 parm : None
 retn : None
*/
VOID CMyEdit::SetEngMode()
{
	if (!m_hWnd)	return;

	HIMC hIMC = ImmGetContext(m_hWnd);
	DWORD dwConv, dwSent;
	DWORD dwTemp;

	ImmGetConversionStatus(hIMC,&dwConv,&dwSent);
	dwTemp = dwConv & ~IME_CMODE_LANGUAGE;
	dwTemp |= IME_CMODE_ALPHANUMERIC;
	dwConv = dwTemp;

	ImmSetConversionStatus(hIMC, dwConv, dwSent);
	ImmReleaseContext(m_hWnd, hIMC);
}

/*
 desc : �Է� ��带 ������ �����ϰ�, �Է��ϴ� ���� ��� ���� ������ ����
 parm : lang	- [in]  korean or english
 retn : None
*/
VOID CMyEdit::SetInputMode(ENM_DILM lang)
{
	m_enLang	= lang;

	switch(m_enLang)
	{
	case ENM_DILM::en_korean		:	SetHanMode();	break;
	case ENM_DILM::en_english	:	SetEngMode();	break;
	}
}

/*
 desc : ���� ����Ʈ �ڽ��� �Էµ� ���ڿ��� ���ڷ� ��ȯ �� ��ȯ
 parm : None
 retn : Number
*/
INT64 CMyEdit::GetTextToNum()
{
	CString strText;

	// �Էµ� ���ڿ� ��������
	GetWindowText(strText);

#ifdef	_UNICODE
	return _wtoi64(strText);
#else
	return _atoi64(strText);
#endif
}

/*
 desc : ���� ����Ʈ �ڽ��� �Էµ� ���ڿ� ���� ��ȯ
 parm : None
 retn : ����
*/
INT64 CMyEdit::GetTextToLen()
{
	CString strText;

	// �Էµ� ���ڿ� ��������
	GetWindowText(strText);

	return strText.GetLength();
}

/*
 desc : ���� ����Ʈ �ڽ��� �Էµ� ���ڿ��� �Ҽ����� ���Ե� ���ڷ� ��ȯ �� ��ȯ
 parm : None
 retn : Number
*/
DOUBLE CMyEdit::GetTextToDouble()
{
	CString strText;

	// �Էµ� ���ڿ� ��������
	GetWindowText(strText);

#ifdef	_UNICODE
	return _wtof(strText);
#else
	return atof(strText);
#endif
}

/*
 desc : ���� ����Ʈ �ڽ��� �Էµ� ���ڿ��� Hex �� ��ȯ �� ��ȯ
 parm : None
 retn : �� ��ȯ
*/
UINT64 CMyEdit::GetTextToHex()
{
	CString strText;

	// �Էµ� ���ڿ� ��������
	GetWindowText(strText);

#ifdef	_UNICODE
	return wcstoul(strText, NULL, 16);
#else
	return strtoul(strText, NULL, 16);
#endif
}

/*
 desc : Updates control
 parm : None
 retn : None
*/
VOID CMyEdit::UpdateData()
{
	SetRedraw(TRUE);
	Invalidate(FALSE);
}

/*
 desc : ���ڷ� �Էµ� �����͸� ��Ʈ�ѿ� ���
 parm : data	- [in]  ���� ������ ��
		redraw	- [in]  ���� ��Ȱ��ȭ -> �� ���� -> ���� Ȱ��ȭ
 retn : None
*/
VOID CMyEdit::SetTextToVal(INT64 data, BOOL redraw)
{
	TCHAR tzData[128]	= {NULL};
	swprintf_s(tzData, 128, L"%I64d", data);
	if (redraw)	SetRedraw(FALSE);
	SetWindowText(tzData);
	if (redraw)	UpdateData();
}
VOID CMyEdit::SetTextToVal(UINT64 data, BOOL redraw)
{
	TCHAR tzData[128]	= {NULL};
	swprintf_s(tzData, 128, L"%I64d", data);
	if (redraw)	SetRedraw(FALSE);
	SetWindowText(tzData);
	if (redraw)	UpdateData();
}
VOID CMyEdit::SetTextToNum(INT8 data, BOOL redraw)
{
	m_i8Value	= data;
	SetTextToVal(INT64(data), redraw);
}
VOID CMyEdit::SetTextToNum(UINT8 data, BOOL redraw)
{
	m_u8Value	= data;
	SetTextToVal(UINT64(data), redraw);
}
VOID CMyEdit::SetTextToNum(INT16 data, BOOL redraw)
{
	m_i16Value	= data;
	SetTextToVal(INT64(data), redraw);
}
VOID CMyEdit::SetTextToNum(UINT16 data, BOOL redraw)
{
	m_u16Value	= data;
	SetTextToVal(UINT64(data), redraw);
}
VOID CMyEdit::SetTextToNum(INT32 data, BOOL redraw)
{
	m_i32Value	= data;
	SetTextToVal(INT64(data), redraw);
}
VOID CMyEdit::SetTextToNum(UINT32 data, BOOL redraw)
{
	m_u32Value	= data;
	SetTextToVal(INT64(data), redraw);
}
VOID CMyEdit::SetTextToNum(INT64 data, BOOL redraw)
{
	m_i64Value	= data;
	SetTextToVal(data, redraw);
}
VOID CMyEdit::SetTextToNum(UINT64 data, BOOL redraw)
{
	m_u64Value	= data;
	SetTextToVal(data, redraw);
}
VOID CMyEdit::SetTextToHex(UINT8 data, BOOL redraw)
{
	m_u8Value	= data;

	TCHAR tzData[128]	= {NULL};
	swprintf_s(tzData, 128, L"%02x", data);
	if (redraw)	SetRedraw(FALSE);
	SetWindowText(tzData);
	if (redraw)	UpdateData();
}
VOID CMyEdit::SetTextToHex(UINT16 data, BOOL redraw)
{
	m_u16Value	= data;

	TCHAR tzData[128]	= {NULL};
	swprintf_s(tzData, 128, L"%04x", data);
	if (redraw)	SetRedraw(FALSE);
	SetWindowText(tzData);
	if (redraw)	UpdateData();
}
VOID CMyEdit::SetTextToHex(UINT32 data, BOOL redraw)
{
	m_u32Value	= data;

	TCHAR tzData[128]	= {NULL};
	swprintf_s(tzData, 128, L"%08x", data);
	if (redraw)	SetRedraw(FALSE);
	SetWindowText(tzData);
	if (redraw)	UpdateData();
}

/*
 desc : ���ڷ� �Էµ� �����͸� ��Ʈ�ѿ� ���
 parm : data	- [in]  ���� ������ ��
		point	- [in]  �Ҽ��� ���� �ڸ��� ����
		redraw	- [in]  ���� ��Ȱ��ȭ -> �� ���� -> ���� Ȱ��ȭ
 retn : None
*/
VOID CMyEdit::SetTextToNum(DOUBLE data, INT32 point, BOOL redraw)
{
	TCHAR tzData[1024]	= {NULL};
	TCHAR tzPoint[32]	= {NULL};
	swprintf_s(tzPoint, _T("%%.%df"), point);
	swprintf_s(tzData, 1024, tzPoint, data);
	if (redraw)	SetRedraw(FALSE);
	SetWindowText(tzData);
	if (redraw)	UpdateData();
}

/*
 desc : ���ڿ� �Էµ� �����͸� ��Ʈ�ѿ� ���
 parm : data	- [in]  ���ڿ� ������ ��
		redraw	- [in]  ���� ��Ȱ��ȭ -> �� ���� -> ���� Ȱ��ȭ
 retn : None
*/
VOID CMyEdit::SetTextToStr(PTCHAR data, BOOL redraw)
{
	if (redraw)	SetRedraw(FALSE);
	SetWindowText(data);
	if (redraw)	UpdateData();
}

/*
 desc : ���� ����Ǿ� �ִ� ���� 1 �� ���� ��Ű��
*/
VOID CMyEdit::SetPlusInt64()
{
	SetTextToNum(GetTextToNum()+1, TRUE);
}

/*
 desc : ���ڿ� ��
 parm : data	- [in]  ���� ��
 retn : data ���� ũ�� ���, ������ ����, �����ϸ� 0
*/
INT32 CMyEdit::Compare(INT64 data)
{
	if (data > GetTextToNum())	return 1;
	else if (data < GetTextToNum())	return -1;

	return 0;
}

/*
 desc : ���ڿ� ��
 parm : data	- [in]  ���ڿ��� ����� ������
 retn : data ���� ũ�� ���, ������ ����, �����ϸ� 0
*/
INT32 CMyEdit::Compare(TCHAR *data)
{
	INT32 i32Ret	= 0;

	// �ӽ÷� �ؽ�Ʈ �����͸� ������ ���� �Ҵ�
	TCHAR *pData	= new TCHAR[10241];
	ASSERT(pData);
	wmemset(pData, 0x00, 10241);
	// ��Ʈ�ѿ��� ������ �б�
	GetWindowText(pData, 10240);
	i32Ret	= wcscmp(data, pData);
	// ���� �޸� ����
	delete [] pData;

	return i32Ret;
}

/*
 desc : �Էµ� ���ڿ��� ��µ� ���ڿ� ���� ��
 parm : str	- [in]  �Էµ� ���ڿ��� ����� ����
 retn : TRUE - �Ȱ���. FALSE - Ʋ����
*/
BOOL CMyEdit::IsCompareText(TCHAR *str)
{
	CString strText;

	GetWindowText(strText);

	return strText.Compare(str);
}

/*
 desc : ���� ����� �����Ϳ� �̾ ���� ���ο� �߰��ϱ�
 parm : text	- [in]  �߰��ϰ��� �ϴ� �ؽ�Ʈ ������
 retn : None
*/
VOID CMyEdit::SetTextEndLine(TCHAR *text)
{
	INT32 i32Len = GetWindowTextLength();
	/* ���ڿ��� ������ ��ġ�� Ŀ���� ���� */
	SetSel(i32Len, i32Len);
	/* Focus ���� */
	SetFocus();
	/* ���� ���� ó�� ���� */
	if (i32Len > 0)			ReplaceSel(L"\r\n");
	/* ���� ���� ��� ó�� */
	if (_tcslen(text) > 0)	ReplaceSel(text);
}

/*
 desc : ���� ����Ǿ� �ִ� ��� �ؽ�Ʈ �����
 parm : None
 retn : None
*/
VOID CMyEdit::ResetAllData()
{
	SetFocus();
	/* ��� ���� ���� */
	SetSel(0, -1);
	/* ���õ� ���� ����� */
	Clear();
}

/*
 desc : ���� �Էµ� ���� ���� ����
 parm : None
 retn : None
*/
VOID CMyEdit::SelectedAll()
{
	SetFocus();
	/* ��� ���� ���� */
	SetSel(0, -1);
}

/*
 desc : ���� �Էµ� �ؽ�Ʈ�� �ִ��� ����
 parm : None
 retn : TRUE - ����. FALSE - �ִ�.
*/
BOOL CMyEdit::IsEmpty()
{
	TCHAR tzText[32]	= {NULL};
	GetWindowText(tzText, 32);
	return _tcslen(tzText) > 0 ? FALSE : TRUE;
}