
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
 desc : 생성자
 parm : None
 retn : None
*/
CMyRich::CMyRich()
{
	m_bKeyInputEnable	= TRUE;
	m_bAutoScroll		= FALSE;
	m_i32MaxLines		= 0;	/* 기본적으로 무제한으로 등록 가능 (메모리 한계까지) */
	m_bMouseFoucs		= TRUE;

	/* 폰트 정보 초기화 */
	InitFont();
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMyRich::~CMyRich()
{
}

/* 메시지 맵 */
BEGIN_MESSAGE_MAP(CMyRich, CRichEditCtrl)
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

BOOL CMyRich::PreTranslateMessage(MSG* msg)
{
#if 0
	/* 마우스 포커스 기능 막기 위함 */
	if (!m_bMouseFoucs && (msg->message == WM_LBUTTONDOWN || msg->message == WM_RBUTTONDOWN))
	{
		SendMessage(WM_KILLFOCUS, 0, 0L);
		return TRUE;
	}
#endif
#if 0
	/* 마우스 휠 기능 */
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
		/* Rich Edit Control의 ScrollBar 얻기 */
		SCROLLINFO stInfo	= {NULL};
		GetScrollInfo(SB_VERT, &stInfo);
		/* 현재 스크롤 위치에 가감 값 적용 */
		stInfo.nPos	=+ i32Val;
		SetScrollPos(SB_VERT, stInfo.nPos);
	}
#endif
	if (msg->message == WM_KEYDOWN)
	{
		/* Edit에서 MultiLine 속성일 때, Enter Key 먹고자 하기 위함 */
		if (!m_bKeyInputEnable)
		{
			/* 가상키 값 Disable 처리 */
			if (VK_RETURN == msg->wParam || VK_BACK == msg->wParam ||
				VK_DELETE == msg->wParam || VK_TAB == msg->wParam)	return TRUE;
#if 0
			/* Ctrl & Alt 키 조합을 제외한 모든 키 값 Disable 처리 */
			if (0x20 <= msg->wParam && msg->wParam <= 0x7e)	return TRUE;
#endif
			/* Ctrl + C 키 감지 */
			if (!((L'c' == msg->wParam || L'C' == msg->wParam) && GetKeyState(VK_CONTROL)))
				return TRUE;
		}
	}

	return CRichEditCtrl::PreTranslateMessage(msg);
}

/*
 desc : 폰트 정보 초기화
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
 desc : 폰트 정보 설정
 parm : name	- [in]  폰트 이름
		size	- [in]  폰트 크기 (Character height, in twips (1/1440 of an inch or 1/20 of a printer's point))
 retn : None
*/
VOID CMyRich::SetRichFont(TCHAR *name, UINT32 size)
{
	if (size < 1)	return;

	memset(m_cfFont.szFaceName, 0x00, LF_FACESIZE);
	wcscpy_s(m_cfFont.szFaceName, LF_FACESIZE, name);
	m_cfFont.yHeight	= size * 20 /* 계산된 값 1 / 20 */;
}

/*
 desc : 현재 RichEdit Control 영역에서 보여지고 있는 라인의 개수 얻기
 parm : None
 retn : Line Counts 반환
*/
INT32 CMyRich::GetViewLines()
{
	INT32 i32FirstChar, i32LastChar, i32FirstLine, i32LastLine;
	CRect r;

	/* 현재 윈도 영역 크기 얻기 */
	GetClientRect(r);

	/* 왼쪽 상단의 문자 색인 가져오기 */
	i32FirstChar	= CharFromPos(CPoint(0, 0));
	/* 오른쪽 상단의 문자 색인 가져오기 */
	i32LastChar		= CharFromPos(CPoint(r.right, r.bottom));
	if (i32LastChar < 0)	i32LastChar	= GetTextLength();

	/* 라인 개수로 변환하기 */
	i32FirstLine	= LineFromChar(i32FirstChar);
	i32LastLine		= LineFromChar(i32LastChar);

	/* 현재 영역에서 보여지는 라인 개수 반환 */
	return i32LastLine - i32FirstLine;
}

/*
 desc : 로그 등록
 parm : logs		- [in]  로그 데이터가 저장되어 있는 문자열 버퍼 포인터
		clr_text	- [in]  문자열
		cr_input	- [in]  Carrage Return 값 입력 여부 (TRUE or FALSE)
 retn : None
*/
VOID CMyRich::Append(TCHAR *logs, COLORREF clr_text, BOOL cr_input)
{
	LONG l32Row		= 0, l32Col = 0;
	INT32 i32TextLen	= 0, i32InsPoint = 0;
	INT32 i32OldLines	= 0, i32NewLines = 0;

	/* 현재 커서의 위치 얻기 */
	GetSel(l32Row, l32Col);

	/* 새로운 문자열을 추가하기 전에 현재 라인 수 저장 */
	i32OldLines	= GetLineCount();

	/* 임의 라인 수까지 입력 제한이 걸려 있으면, 맨 처음 줄 제거 */
	if (m_i32MaxLines && m_i32MaxLines < i32OldLines)
	{
		TCHAR tzBuff[1024]	= {NULL};
		
		/* 첫 줄의 문자열 길이 얻기 */
		i32TextLen	= LineLength(0);
		/* 첫 줄의 문자열 얻기 */
		i32TextLen	= GetLine(0, tzBuff, i32TextLen);
		/* 첫 줄의 문자열 선택 (Carrage Return 길이 1 bytes 포함) */
		SetSel(0, i32TextLen+1);
		/* 선택된 문자열 NULL 변경하기 */
		ReplaceSel(L"");
	}

	/* 폰트 색상 변경 및 설정 */
	m_cfFont.crTextColor	= clr_text;
	/* 추가될 문자열의 위치 (즉, 현재 컨트롤에 출력된 마지막 문자열 위치)의 폰트 설정 */
	i32InsPoint	= GetWindowTextLength();	/* 문자열 끝 위치 얻기 */
	SetSel(i32InsPoint, -1);				/* 문자열 끝 위치 선택 */
	SetSelectionCharFormat(m_cfFont);		/* 문자열 끝 위치의 선택된 곳에 폰트 설정 */
	/* 문자열 추가 */
	ReplaceSel(logs);

	/* 문자열 마지막에 줄바꿈 문자 추가 여부 */
	if (cr_input)
	{
		/* 항상 마지막 문자열에 줄바꿈 문자 추가하기 */
		i32TextLen	= GetWindowTextLength();
		/* 문자열 마지막 선택 */
		SetSel(i32TextLen, i32TextLen);
		/* 문자열 마지막에 Carrage Return 값 추가 */
		ReplaceSel(L"\r\n");
	}

	/* 추가된 문자열 (라인)에 맞게 스크롤 증가 시키기 (스크롤 크기 비율 만큼씩 증가) */
	if (m_bAutoScroll)
	{
		/* 새로 추가된 라인의 위치 (라인 수) 얻기 */
		i32NewLines	= GetLineCount();
#if 1
		LineScroll(i32NewLines-i32OldLines);
#else
		LineScroll(INT_MAX);
		LineScroll(1-GetViewLines());
#endif
	}

	/* 커서 위치 복귀 */
	SetSel(l32Row, l32Col);
}

/*
 desc : 전체 or 선택 문자열 지우기
 parm : enable	- [in]  TRUE : 전체 문자열 제거, FALSE : 선택 문자열 제거
 retn : None
*/
VOID CMyRich::SetClearAll(BOOL enable)
{
	/* 전체 선택 */
	if (enable)
	{		
		SetSel(0, -1);
	}
	/* 선택된 영역 지우기 */
#if 0
	Clear();
#else
	ReplaceSel(L"");
#endif
}

/*
 desc : 전체 or 선택 문자열 복사
 parm : enable	- [in]  TRUE : 전체 문자열 복사, FALSE : 선택 문자열 복사
 retn : None
*/
VOID CMyRich::SetCopyAll(BOOL enable)
{
	/* 전체 선택 */
	if (enable)
	{
		SetSel(0, -1);
	}
	/* 선택된 영역 복사 */
	Copy();

	/* 선택 취소 */
	SetSel(-1, -1);
}

/*
 desc : 전체 or 선택 문자열 저장
 parm : enable	- [in]  TRUE : 전체 문자열 복사, FALSE : 선택 문자열 복사
 retn : None
*/
VOID CMyRich::SaveToFile(BOOL enable)
{
	TCHAR tzPath[MAX_PATH]	= {NULL}, tzFile[MAX_PATH] = {NULL};
	long i32FirstChar, i32LastChar;
	PTCHAR ptzText	= NULL;
	CString strText;
	CUniToChar csCnv;

	/* 전체 선택 */
	if (enable)	SetSel(0, -1);

	/* 현재 선택된 영역 인덱스 가져오기 */
	GetSel(i32FirstChar, i32LastChar);
	if (i32FirstChar == i32LastChar)	return;

	/* 인덱스 범위의 문자 가져오기 */
	if (GetTextRange(i32FirstChar, i32LastChar, strText) > 0)
	{
		/* 임시 파일로 저장 후, 에디터 프로그램 (기본 프로그램으로 설정된 프로그램)으로 자동 열기 */
		if (GetTempPathW(MAX_PATH, tzPath) > 3)
		{
			/* 임시 폴더 위치에 임시 파일 열도록 파일 설정 */
			swprintf_s(tzFile, MAX_PATH, L"%s\\xditel.log", tzPath);
			/* 임시 파일에 강제로 선택한 문자열 저장 (덮어쓰기) */
			ptzText	= strText.GetBuffer();
			uvCmn_SaveTxtFileW(ptzText, strText.GetLength(), tzFile);
			/* Notepad 열기 수행 */
			ShellExecuteW(m_hWnd, L"open", tzFile, NULL, NULL, SW_SHOW);
		}
	}

	/* 선택 취소 */
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

		// 스크롤이 완료된 후 라인의 시작으로 이동 ( 텍스트가 중간에 걸치는 현상 방지 )
		LineScroll(-GetFirstVisibleLine() + nNewFirstVisibleLine);

		GetParent()->SendMessage(WM_USER_SYNCSCROLL, (WPARAM)this, (LPARAM)nLinesScrolled);
	}

	CRichEditCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

/*
 desc : 현재 커서의 행과 컬럼 위치 얻기
 parm : row	- [out]  반환될 커서의 위치 (행)
		col	- [out]  반환될 커서의 위치 (열)
 retn : None
*/
VOID CMyRich::GetCursorPos(INT32 &row, INT32 &col)
{
	LONG lSelStart, lSelEnd;

	GetSel(lSelStart, lSelEnd);
	/* 행 얻기 */
	row	= LineFromChar(lSelStart);
	/* 열 얻기 */
	col	= lSelStart - LineIndex(row);
}