
/*
 desc : 메시지 출력
*/

#include "stdafx.h"
#include "EditMsg.h"

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

/*
 desc : 생성자
 parm : None
 retn : None
*/
CEditMsg::CEditMsg()
{
}

/*
 desc : 파괴자
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
 desc : 컨트롤이 수행될 때, 초기 호출됨
 parm : None
 retn : None
*/
VOID CEditMsg::PreSubclassWindow()
{

	CMyEdit::PreSubclassWindow();
}

/*
 desc : 마우스가 윈도 (에디트) 영역에 들어온 경우
 parm : flags	- [in]  다양한 사상 키가 눌렸는지 여부
						MK_CONTROL CTRL	키가 눌려 있을 때 설정합니다.
						MK_LBUTTON		마우스 왼쪽 단추가 눌려 있을 때 설정합니다.
						MK_MBUTTON		마우스 가운데 단추가 눌려 있을 때 설정합니다.
						MK_RBUTTON		마우스 오른쪽 단추가 눌려 있을 때 설정합니다.
						MK_SHIFT SHIFT	키가 눌려 있을 때 설정합니다.
		point	- [in]  커서의 x와 y 좌표를 지정합니다.
						이러한 좌표는 항상 창의 왼쪽 위 모서리에 상대적입니다.
 retn : None
*/
VOID CEditMsg::OnMouseMove(UINT32 flags, CPoint point)
{
	/* 가상함수 호출 */
	MouseMove(flags, point);

	CMyEdit::OnMouseMove(flags, point);
}
/*
 desc : 마우스가 움직일 때마다 영역 캡처
 parm : point	- [in]  마우스 이동 위치
 retn : 0L
*/
LRESULT CEditMsg::OnNcHitTest(CPoint point)
{
	LRESULT rHitTest = CMyEdit::OnNcHitTest(point);

	/* 가상 함수 호출 */
	NcHitTest(point);

	return rHitTest;
}

/* --------------------------------------------------------------------------------------------- */
/*                                        MC2 Error Message                                      */
/* --------------------------------------------------------------------------------------------- */
#if 0
/*
 desc : 마우스가 윈도 (에디트) 영역에 들어온 경우
 parm : flags	- [in]  부모 함수 참조
		point	- [in]  부모 함수 참조
 retn : None
*/
VOID CEditSD2SErr::MouseMove(UINT32 flags, CPoint point)
{
	UINT16 u16Code	= 0x0000;
	TCHAR tzMesg[64]= {NULL};

	if (m_pEdtMsg)
	{
		/* 현재 에러 코드 얻기 */
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
 desc : 마우스가 움직일 때마다 영역 캡처
 parm : point	- [in]  마우스 이동 위치
 retn : 0L
*/
LRESULT CEditSD2SErr::NcHitTest(CPoint point)
{
	CRect rClient;

	if (!m_pEdtMsg)	return 0L;

	/* 내부 영역 얻기 */
	GetClientRect(rClient);
	/* 윈도 영역 얻기 */
	ClientToScreen(rClient);

	/* 현재 마우스 포인터가 윈도 영역 안에 있는지 여부에 따라 메시지 출력 여부 */
	if (rClient.PtInRect(point))
	{
		PTCHAR ptzMesg	= NULL;
		UINT16 u16Code	= (UINT16)GetTextToHex();
		CUniToChar csCnv;
		/* 참고로 첫 글자에 공백을 넣어 준다.*/
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
