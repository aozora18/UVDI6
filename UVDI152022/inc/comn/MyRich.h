
#pragma once

#define WM_USER_SYNCSCROLL			(WM_USER + 1)

class CMyRich : public CRichEditCtrl
{
/* ������ & �ı��� */
public:

	CMyRich();
	virtual ~CMyRich();

/* ���� �Լ� */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	
public:


/* ��� ���� */
protected:

	BOOL				m_bKeyInputEnable;	/* �Ϲ� Ű���� (Ư�� Ű ����) �Է� ���� ���� */
	BOOL				m_bAutoScroll;		/* �ڵ� ��ũ�� ���� ���� */
	BOOL				m_bMouseFoucs;		/* ���콺 ��Ŀ�� ��� ���� */

	INT32				m_i32MaxLines;		/* ��ϵǴ� �α��� �ִ� ���� */

	CHARFORMAT			m_cfFont;			/* Rich Edit Font */


/* ���� �Լ� */
protected:

	/* ��Ʈ ���� �ʱ�ȭ */
	VOID				InitFont();

	/* ���� ���̴� �������� �Էµ� �α� ���ڿ��� ���� ���� ��ȯ */
	INT32				GetViewLines();

	VOID				GetCursorPos(INT32 &row, INT32 &col);

/* ���� �Լ� */
public:

	/* �Ϲ� Ű���� �Է� ���� */
	VOID				SetKeyInputEnable(BOOL enable)	{	m_bKeyInputEnable = enable;	};
	/* �α� ��� */
	VOID				Append(TCHAR *logs, COLORREF clr_text, BOOL cr_input=TRUE);

	/* �ڵ� ��ũ�� ��� ���� */
	VOID				SetAutoScroll(BOOL scroll)	{	m_bAutoScroll	= scroll;	};
	/* �ִ� ��� ������ ���� ���� �� ���� */
	VOID				SetMaxLines(INT32 lines)	{	m_i32MaxLines	= lines;	};
	/* ��ϵ� ���ڿ��� ��Ʈ ���� ���� */
	VOID				SetRichFont(TCHAR *name, UINT32 size);
	/* ��ü or ���� ���ڿ� ����� */
	VOID				SetClearAll(BOOL enable);
	/* ��ü or ���� ���ڿ� Ŭ�����忡 ���� */
	VOID				SetCopyAll(BOOL enable);
	/* ��ü or ���� ���ڿ� ���Ͽ� ���� */
	VOID				SaveToFile(BOOL enable);
	/* ���콺 ��Ŀ�� ��� ���� ���� */
	VOID				SetMouseFocus(BOOL focus)	{	m_bMouseFoucs	= focus;	};

/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void		OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};