
/*
 desc : RichEditCtrl Override
*/

#include "pch.h"
#include "MyRich.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : None
 retn : None
*/
CMyRich::CMyRich()
{
	m_bKeyInputEnable	= TRUE;
	m_bAutoScroll		= FALSE;
	m_i32MaxLines		= 0;	/* �⺻������ ���������� ��� ���� (�޸� �Ѱ����) */
	m_bMouseFoucs		= TRUE;

	/* ��Ʈ ���� �ʱ�ȭ */
	InitFont();
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CMyRich::~CMyRich()
{
}

/* �޽��� �� */
BEGIN_MESSAGE_MAP(CMyRich, CRichEditCtrl)
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

BOOL CMyRich::PreTranslateMessage(MSG* msg)
{
#if 0
	/* ���콺 ��Ŀ�� ��� ���� ���� */
	if (!m_bMouseFoucs && (msg->message == WM_LBUTTONDOWN || msg->message == WM_RBUTTONDOWN))
	{
		SendMessage(WM_KILLFOCUS, 0, 0L);
		return TRUE;
	}
#endif
#if 0
	/* ���콺 �� ��� */
	if (WM_MOUSEWHEEL == msg->message)
	{
		INT32 i32Val	= 1;
		/* Down Scroll Event */
		if (INT16(HIWORD(msg->wParam)) < 0)
		{
		}
		/* Up Scroll Event */
		else
		{
			i32Val	= -1;
		}
		/* Rich Edit Control�� ScrollBar ��� */
		SCROLLINFO stInfo	= {NULL};
		GetScrollInfo(SB_VERT, &stInfo);
		/* ���� ��ũ�� ��ġ�� ���� �� ���� */
		stInfo.nPos	=+ i32Val;
		SetScrollPos(SB_VERT, stInfo.nPos);
	}
#endif
	if (msg->message == WM_KEYDOWN)
	{
		/* Edit���� MultiLine �Ӽ��� ��, Enter Key �԰��� �ϱ� ���� */
		if (!m_bKeyInputEnable)
		{
			/* ����Ű �� Disable ó�� */
			if (VK_RETURN == msg->wParam || VK_BACK == msg->wParam ||
				VK_DELETE == msg->wParam || VK_TAB == msg->wParam)	return TRUE;
#if 0
			/* Ctrl & Alt Ű ������ ������ ��� Ű �� Disable ó�� */
			if (0x20 <= msg->wParam && msg->wParam <= 0x7e)	return TRUE;
#endif
			/* Ctrl + C Ű ���� */
			if (!((L'c' == msg->wParam || L'C' == msg->wParam) && GetKeyState(VK_CONTROL)))
				return TRUE;
		}
	}

	return CRichEditCtrl::PreTranslateMessage(msg);
}

/*
 desc : ��Ʈ ���� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CMyRich::InitFont()
{
	memset(&m_cfFont, 0x00, sizeof(CHARFORMAT));

	m_cfFont.cbSize			= sizeof(CHARFORMAT);
	m_cfFont.dwMask			= CFM_COLOR|CFM_FACE;
	m_cfFont.dwEffects		= ~(CFE_AUTOCOLOR | CFE_UNDERLINE | CFE_BOLD);
	m_cfFont.bPitchAndFamily= FIXED_PITCH|FF_DONTCARE;
	m_cfFont.crTextColor	= RGB(0, 0, 0);
}

/*
 desc : ��Ʈ ���� ����
 parm : name	- [in]  ��Ʈ �̸�
		size	- [in]  ��Ʈ ũ�� (Character height, in twips (1/1440 of an inch or 1/20 of a printer's point))
 retn : None
*/
VOID CMyRich::SetRichFont(TCHAR *name, UINT32 size)
{
	if (size < 1)	return;

	memset(m_cfFont.szFaceName, 0x00, LF_FACESIZE);
	wcscpy_s(m_cfFont.szFaceName, LF_FACESIZE, name);
	m_cfFont.yHeight	= size * 20 /* ���� �� 1 / 20 */;
}

/*
 desc : ���� RichEdit Control �������� �������� �ִ� ������ ���� ���
 parm : None
 retn : Line Counts ��ȯ
*/
INT32 CMyRich::GetViewLines()
{
	INT32 i32FirstChar, i32LastChar, i32FirstLine, i32LastLine;
	CRect r;

	/* ���� ���� ���� ũ�� ��� */
	GetClientRect(r);

	/* ���� ����� ���� ���� �������� */
	i32FirstChar	= CharFromPos(CPoint(0, 0));
	/* ������ ����� ���� ���� �������� */
	i32LastChar		= CharFromPos(CPoint(r.right, r.bottom));
	if (i32LastChar < 0)	i32LastChar	= GetTextLength();

	/* ���� ������ ��ȯ�ϱ� */
	i32FirstLine	= LineFromChar(i32FirstChar);
	i32LastLine		= LineFromChar(i32LastChar);

	/* ���� �������� �������� ���� ���� ��ȯ */
	return i32LastLine - i32FirstLine;
}

/*
 desc : �α� ���
 parm : logs		- [in]  �α� �����Ͱ� ����Ǿ� �ִ� ���ڿ� ���� ������
		clr_text	- [in]  ���ڿ�
		cr_input	- [in]  Carrage Return �� �Է� ���� (TRUE or FALSE)
 retn : None
*/
VOID CMyRich::Append(TCHAR *logs, COLORREF clr_text, BOOL cr_input)
{
	LONG l32Row		= 0, l32Col = 0;
	INT32 i32TextLen	= 0, i32InsPoint = 0;
	INT32 i32OldLines	= 0, i32NewLines = 0;

	/* ���� Ŀ���� ��ġ ��� */
	GetSel(l32Row, l32Col);

	/* ���ο� ���ڿ��� �߰��ϱ� ���� ���� ���� �� ���� */
	i32OldLines	= GetLineCount();

	/* ���� ���� ������ �Է� ������ �ɷ� ������, �� ó�� �� ���� */
	if (m_i32MaxLines && m_i32MaxLines < i32OldLines)
	{
		TCHAR tzBuff[1024]	= {NULL};
		
		/* ù ���� ���ڿ� ���� ��� */
		i32TextLen	= LineLength(0);
		/* ù ���� ���ڿ� ��� */
		i32TextLen	= GetLine(0, tzBuff, i32TextLen);
		/* ù ���� ���ڿ� ���� (Carrage Return ���� 1 bytes ����) */
		SetSel(0, i32TextLen+1);
		/* ���õ� ���ڿ� NULL �����ϱ� */
		ReplaceSel(L"");
	}

	/* ��Ʈ ���� ���� �� ���� */
	m_cfFont.crTextColor	= clr_text;
	/* �߰��� ���ڿ��� ��ġ (��, ���� ��Ʈ�ѿ� ��µ� ������ ���ڿ� ��ġ)�� ��Ʈ ���� */
	i32InsPoint	= GetWindowTextLength();	/* ���ڿ� �� ��ġ ��� */
	SetSel(i32InsPoint, -1);				/* ���ڿ� �� ��ġ ���� */
	SetSelectionCharFormat(m_cfFont);		/* ���ڿ� �� ��ġ�� ���õ� ���� ��Ʈ ���� */
	/* ���ڿ� �߰� */
	ReplaceSel(logs);

	/* ���ڿ� �������� �ٹٲ� ���� �߰� ���� */
	if (cr_input)
	{
		/* �׻� ������ ���ڿ��� �ٹٲ� ���� �߰��ϱ� */
		i32TextLen	= GetWindowTextLength();
		/* ���ڿ� ������ ���� */
		SetSel(i32TextLen, i32TextLen);
		/* ���ڿ� �������� Carrage Return �� �߰� */
		ReplaceSel(L"\r\n");
	}

	/* �߰��� ���ڿ� (����)�� �°� ��ũ�� ���� ��Ű�� (��ũ�� ũ�� ���� ��ŭ�� ����) */
	if (m_bAutoScroll)
	{
		/* ���� �߰��� ������ ��ġ (���� ��) ��� */
		i32NewLines	= GetLineCount();
#if 1
		LineScroll(i32NewLines-i32OldLines);
#else
		LineScroll(INT_MAX);
		LineScroll(1-GetViewLines());
#endif
	}

	/* Ŀ�� ��ġ ���� */
	SetSel(l32Row, l32Col);
}

/*
 desc : ��ü or ���� ���ڿ� �����
 parm : enable	- [in]  TRUE : ��ü ���ڿ� ����, FALSE : ���� ���ڿ� ����
 retn : None
*/
VOID CMyRich::SetClearAll(BOOL enable)
{
	/* ��ü ���� */
	if (enable)
	{		
		SetSel(0, -1);
	}
	/* ���õ� ���� ����� */
#if 0
	Clear();
#else
	ReplaceSel(L"");
#endif
}

/*
 desc : ��ü or ���� ���ڿ� ����
 parm : enable	- [in]  TRUE : ��ü ���ڿ� ����, FALSE : ���� ���ڿ� ����
 retn : None
*/
VOID CMyRich::SetCopyAll(BOOL enable)
{
	/* ��ü ���� */
	if (enable)
	{
		SetSel(0, -1);
	}
	/* ���õ� ���� ���� */
	Copy();

	/* ���� ��� */
	SetSel(-1, -1);
}

/*
 desc : ��ü or ���� ���ڿ� ����
 parm : enable	- [in]  TRUE : ��ü ���ڿ� ����, FALSE : ���� ���ڿ� ����
 retn : None
*/
VOID CMyRich::SaveToFile(BOOL enable)
{
	TCHAR tzPath[MAX_PATH]	= {NULL}, tzFile[MAX_PATH] = {NULL};
	long i32FirstChar, i32LastChar;
	PTCHAR ptzText	= NULL;
	CString strText;
	CUniToChar csCnv;

	/* ��ü ���� */
	if (enable)	SetSel(0, -1);

	/* ���� ���õ� ���� �ε��� �������� */
	GetSel(i32FirstChar, i32LastChar);
	if (i32FirstChar == i32LastChar)	return;

	/* �ε��� ������ ���� �������� */
	if (GetTextRange(i32FirstChar, i32LastChar, strText) > 0)
	{
		/* �ӽ� ���Ϸ� ���� ��, ������ ���α׷� (�⺻ ���α׷����� ������ ���α׷�)���� �ڵ� ���� */
		if (GetTempPathW(MAX_PATH, tzPath) > 3)
		{
			/* �ӽ� ���� ��ġ�� �ӽ� ���� ������ ���� ���� */
			swprintf_s(tzFile, MAX_PATH, L"%s\\xditel.log", tzPath);
			/* �ӽ� ���Ͽ� ������ ������ ���ڿ� ���� (�����) */
			ptzText	= strText.GetBuffer();
			uvCmn_SaveTxtFileW(ptzText, strText.GetLength(), tzFile);
			/* Notepad ���� ���� */
			ShellExecuteW(m_hWnd, L"open", tzFile, NULL, NULL, SW_SHOW);
		}
	}

	/* ���� ��� */
	SetSel(-1, -1);
}

void CMyRich::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (GetParent())
	{
		int nOldFirstVisibleLine = GetFirstVisibleLine();

		CRichEditCtrl::OnVScroll(nSBCode, nPos, pScrollBar);

		int nNewFirstVisibleLine = GetFirstVisibleLine();
		int nLinesScrolled = nNewFirstVisibleLine - nOldFirstVisibleLine;

		// ��ũ���� �Ϸ�� �� ������ �������� �̵� ( �ؽ�Ʈ�� �߰��� ��ġ�� ���� ���� )
		LineScroll(-GetFirstVisibleLine() + nNewFirstVisibleLine);

		GetParent()->SendMessage(WM_USER_SYNCSCROLL, (WPARAM)this, (LPARAM)nLinesScrolled);
	}

	CRichEditCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

/*
 desc : ���� Ŀ���� ��� �÷� ��ġ ���
 parm : row	- [out]  ��ȯ�� Ŀ���� ��ġ (��)
		col	- [out]  ��ȯ�� Ŀ���� ��ġ (��)
 retn : None
*/
VOID CMyRich::GetCursorPos(INT32 &row, INT32 &col)
{
	LONG lSelStart, lSelEnd;

	GetSel(lSelStart, lSelEnd);
	/* �� ��� */
	row	= LineFromChar(lSelStart);
	/* �� ��� */
	col	= lSelStart - LineIndex(row);
}