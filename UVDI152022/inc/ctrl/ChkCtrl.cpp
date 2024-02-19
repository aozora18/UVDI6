
/*
 desc : Check Control
*/

#include "pch.h"
#include "ChkCtrl.h"

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
CChkCtrl::CChkCtrl()
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CChkCtrl::~CChkCtrl()
{
}

BEGIN_MESSAGE_MAP(CChkCtrl, CMacCheckBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

/*
 desc : 마우스 왼쪽 버튼 클릭 Down
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CChkCtrl::OnLButtonDown(UINT32 flags, CPoint point)
{
	/* 윈도 혹은 다이얼로그 어디 영역에서나 현재 Click Event 먹히기 위함 */
	SetCapture();
	/* 왼쪽 마우스 버튼 클릭 다운 가상 함수 호출 */
	LButtonDn();

	CMacCheckBox::OnLButtonDown(flags, point);
}

/*
 desc : 마우스 왼쪽 버튼 클릭 UP
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CChkCtrl::OnLButtonUp(UINT32 flags, CPoint point)
{
	/* 왼쪽 마우스 버튼 클릭 업 가상 함수 호출 */
	LButtonUp();
	/* Capture Release */
	ReleaseCapture();
	CMacCheckBox::OnLButtonUp(flags, point);
}

/* --------------------------------------------------------------------------------------------- */
/*                                         MC2 Motor Control                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 마우스 왼쪽 버튼 클릭 UP
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CChkServoLock::LButtonUp()
{
	BOOL bServoLock	= FALSE;
#if 0
	/* 현재 응답 올 때까지 대기 중인면, 아무리 명령어 날려도 동작되지 않음 */
	if (GetCheck() == BST_INDETERMINATE)	return;
	/* 일단 처리 중이라고 설정 */
	SetCheck(BST_INDETERMINATE);
#endif
	if (uvData_MC2_GetData()->IsServoOff(m_enDrvID))	bServoLock	= TRUE;
#if (USE_LIB_MC2)
	uvMC2_SendDevLocked(m_enDrvID, bServoLock);
#endif
}

