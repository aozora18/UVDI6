
#pragma once

#define WM_USER_SYNCSCROLL			(WM_USER + 1)

class CMyRich : public CRichEditCtrl
{
/* 생성자 & 파괴자 */
public:

	CMyRich();
	virtual ~CMyRich();

/* 가상 함수 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	
public:


/* 멤버 변수 */
protected:

	BOOL				m_bKeyInputEnable;	/* 일반 키보드 (특수 키 제외) 입력 제외 여부 */
	BOOL				m_bAutoScroll;		/* 자동 스크롤 동작 여부 */
	BOOL				m_bMouseFoucs;		/* 마우스 포커스 사용 여부 */

	INT32				m_i32MaxLines;		/* 등록되는 로그의 최대 개수 */

	CHARFORMAT			m_cfFont;			/* Rich Edit Font */


/* 로컬 함수 */
protected:

	/* 폰트 정보 초기화 */
	VOID				InitFont();

	/* 현재 보이는 영역에서 입력된 로그 문자열의 라인 개수 반환 */
	INT32				GetViewLines();

	VOID				GetCursorPos(INT32 &row, INT32 &col);

/* 공용 함수 */
public:

	/* 일반 키보드 입력 제외 */
	VOID				SetKeyInputEnable(BOOL enable)	{	m_bKeyInputEnable = enable;	};
	/* 로그 등록 */
	VOID				Append(TCHAR *logs, COLORREF clr_text, BOOL cr_input=TRUE);

	/* 자동 스크롤 사용 여부 */
	VOID				SetAutoScroll(BOOL scroll)	{	m_bAutoScroll	= scroll;	};
	/* 최대 등록 가능한 문자 라인 수 설정 */
	VOID				SetMaxLines(INT32 lines)	{	m_i32MaxLines	= lines;	};
	/* 등록될 문자열의 폰트 정보 설정 */
	VOID				SetRichFont(TCHAR *name, UINT32 size);
	/* 전체 or 선택 문자열 지우기 */
	VOID				SetClearAll(BOOL enable);
	/* 전체 or 선택 문자열 클립보드에 복사 */
	VOID				SetCopyAll(BOOL enable);
	/* 전체 or 선택 문자열 파일에 저장 */
	VOID				SaveToFile(BOOL enable);
	/* 마우스 포커스 사용 여부 설정 */
	VOID				SetMouseFocus(BOOL focus)	{	m_bMouseFoucs	= focus;	};

/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void		OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};