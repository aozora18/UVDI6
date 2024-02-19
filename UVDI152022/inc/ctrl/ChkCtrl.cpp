
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
 desc : ������
 parm : None
 retn : None
*/
CChkCtrl::CChkCtrl()
{
}

/*
 desc : �ı���
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
 desc : ���콺 ���� ��ư Ŭ�� Down
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CChkCtrl::OnLButtonDown(UINT32 flags, CPoint point)
{
	/* ���� Ȥ�� ���̾�α� ��� ���������� ���� Click Event ������ ���� */
	SetCapture();
	/* ���� ���콺 ��ư Ŭ�� �ٿ� ���� �Լ� ȣ�� */
	LButtonDn();

	CMacCheckBox::OnLButtonDown(flags, point);
}

/*
 desc : ���콺 ���� ��ư Ŭ�� UP
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CChkCtrl::OnLButtonUp(UINT32 flags, CPoint point)
{
	/* ���� ���콺 ��ư Ŭ�� �� ���� �Լ� ȣ�� */
	LButtonUp();
	/* Capture Release */
	ReleaseCapture();
	CMacCheckBox::OnLButtonUp(flags, point);
}

/* --------------------------------------------------------------------------------------------- */
/*                                         MC2 Motor Control                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ���콺 ���� ��ư Ŭ�� UP
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CChkServoLock::LButtonUp()
{
	BOOL bServoLock	= FALSE;
#if 0
	/* ���� ���� �� ������ ��� ���θ�, �ƹ��� ��ɾ� ������ ���۵��� ���� */
	if (GetCheck() == BST_INDETERMINATE)	return;
	/* �ϴ� ó�� ���̶�� ���� */
	SetCheck(BST_INDETERMINATE);
#endif
	if (uvData_MC2_GetData()->IsServoOff(m_enDrvID))	bServoLock	= TRUE;
#if (USE_LIB_MC2)
	uvMC2_SendDevLocked(m_enDrvID, bServoLock);
#endif
}

