
/*
 desc : �޽��� ���
*/

#include "stdafx.h"
#include "EditMsg.h"

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

/*
 desc : ������
 parm : None
 retn : None
*/
CEditMsg::CEditMsg()
{
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CEditMsg::~CEditMsg()
{
}

BEGIN_MESSAGE_MAP(CEditMsg, CMyEdit)
	ON_WM_MOUSEMOVE()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

/*
 desc : ��Ʈ���� ����� ��, �ʱ� ȣ���
 parm : None
 retn : None
*/
VOID CEditMsg::PreSubclassWindow()
{

	CMyEdit::PreSubclassWindow();
}

/*
 desc : ���콺�� ���� (����Ʈ) ������ ���� ���
 parm : flags	- [in]  �پ��� ��� Ű�� ���ȴ��� ����
						MK_CONTROL CTRL	Ű�� ���� ���� �� �����մϴ�.
						MK_LBUTTON		���콺 ���� ���߰� ���� ���� �� �����մϴ�.
						MK_MBUTTON		���콺 ��� ���߰� ���� ���� �� �����մϴ�.
						MK_RBUTTON		���콺 ������ ���߰� ���� ���� �� �����մϴ�.
						MK_SHIFT SHIFT	Ű�� ���� ���� �� �����մϴ�.
		point	- [in]  Ŀ���� x�� y ��ǥ�� �����մϴ�.
						�̷��� ��ǥ�� �׻� â�� ���� �� �𼭸��� ������Դϴ�.
 retn : None
*/
VOID CEditMsg::OnMouseMove(UINT32 flags, CPoint point)
{
	/* �����Լ� ȣ�� */
	MouseMove(flags, point);

	CMyEdit::OnMouseMove(flags, point);
}
/*
 desc : ���콺�� ������ ������ ���� ĸó
 parm : point	- [in]  ���콺 �̵� ��ġ
 retn : 0L
*/
LRESULT CEditMsg::OnNcHitTest(CPoint point)
{
	LRESULT rHitTest = CMyEdit::OnNcHitTest(point);

	/* ���� �Լ� ȣ�� */
	NcHitTest(point);

	return rHitTest;
}

/* --------------------------------------------------------------------------------------------- */
/*                                        MC2 Error Message                                      */
/* --------------------------------------------------------------------------------------------- */
#if 0
/*
 desc : ���콺�� ���� (����Ʈ) ������ ���� ���
 parm : flags	- [in]  �θ� �Լ� ����
		point	- [in]  �θ� �Լ� ����
 retn : None
*/
VOID CEditSD2SErr::MouseMove(UINT32 flags, CPoint point)
{
	UINT16 u16Code	= 0x0000;
	TCHAR tzMesg[64]= {NULL};

	if (m_pEdtMsg)
	{
		/* ���� ���� �ڵ� ��� */
		u16Code	= (UINT16)GetTextToNum();
		if (0x0000 == u16Code)	m_pEdtMsg->SetWindowTextW(L"None Error!");
		else
		{
			if (xMC2_GetDatMC2Error(u16Code, tzMesg, 64))
			{
				m_pEdtMsg->SetWindowTextW(tzMesg);
			}
		}
	}
}
#else
/*
 desc : ���콺�� ������ ������ ���� ĸó
 parm : point	- [in]  ���콺 �̵� ��ġ
 retn : 0L
*/
LRESULT CEditSD2SErr::NcHitTest(CPoint point)
{
	CRect rClient;

	if (!m_pEdtMsg)	return 0L;

	/* ���� ���� ��� */
	GetClientRect(rClient);
	/* ���� ���� ��� */
	ClientToScreen(rClient);

	/* ���� ���콺 �����Ͱ� ���� ���� �ȿ� �ִ��� ���ο� ���� �޽��� ��� ���� */
	if (rClient.PtInRect(point))
	{
		PTCHAR ptzMesg	= NULL;
		UINT16 u16Code	= (UINT16)GetTextToHex();
		CUniToChar csCnv;
		/* ����� ù ���ڿ� ������ �־� �ش�.*/
		if (0x0000 == u16Code)	m_pEdtMsg->SetWindowText(L"None Error");
		else
		{
			ptzMesg	= uvData_MC2_GetError(u16Code);
			if (ptzMesg)	m_pEdtMsg->SetWindowTextW(ptzMesg);
			else			m_pEdtMsg->SetWindowTextW(L"Undefined error");
		}
	}
	else
	{
		m_pEdtMsg->SetWindowText(L"");
	}

	return 0L;
}
#endif
