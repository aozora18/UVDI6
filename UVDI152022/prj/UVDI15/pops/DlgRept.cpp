
/*
 desc : �뱤 �ݺ� Ƚ�� ���� ��ȭ ����
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgRept.h"

#include "../mesg/DlgMesg.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : ������
 parm : id		- [in]  �ڽ��� ���� ID
		parent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
 retn : None
*/
CDlgRept::CDlgRept(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgRept::IDD, parent)
{
	m_bDrawBG		= TRUE;
	m_bTooltip		= TRUE;
	/* Align Camera�� Photohead Up/Down �������� ���� */
	m_bMovingACam	= FALSE;
	m_bMovingPH		= FALSE;

	m_u32ExpoCount	= 1;
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgRept::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Groupbox - Normal */
	u32StartID	= IDC_REPT_GRP_COUNT;
	for (i=0; i< eREPT_GRP_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Edit - Normal */
	u32StartID	= IDC_REPT_EDT_COUNT;
	for (i=0; i< eREPT_EDT_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_edt_int[i]);
	/* Button - Normal */
	u32StartID	= IDC_REPT_BTN_APPLY;
	for (i=0; i< eREPT_BTN_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Button - Normal */
	u32StartID	= IDC_REPT_CHK_PREHEAT;
	for (i=0; i< eREPT_CHK_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgRept, CMyDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_REPT_BTN_APPLY, IDC_REPT_BTN_CANCEL, OnBtnClicked)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_REPT_CHK_PREHEAT, IDC_REPT_CHK_LED_ON, OnChkClicked)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgRept::PreTranslateMessage(MSG* msg)
{

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgRept::OnInitDlg()
{
	CResizeUI clsResizeUI;
	clsResizeUI.ResizeControl(this, this);

	/* ��Ʈ�� �ʱ�ȭ */
	if (!InitCtrl())	return FALSE;

	/* TOP_MOST & Center */
	CenterParentTopMost();
	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgRept::OnExitDlg()
{
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgRept::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgRept::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgRept::InitCtrl()
{
	// by sysandj : Resize UI
	CResizeUI clsResizeUI;
	// by sysandj : Resize UI

	INT32 i = 0;
	PTCHAR pText	= NULL;

	/* group box */
	for (i = 0; i < eREPT_GRP_MAX; i++)
	{
		m_grp_ctl[i].SetLogFont(&g_lf[eFONT_LEVEL2_BOLD]);
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_grp_ctl[i]);
		// by sysandj : Resize UI
	}
	/* edit - text */
	for (i=0; i< eREPT_EDT_MAX; i++)
	{
		m_edt_int[i].SetEditFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_edt_int[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_int[i].SetTextColor(RGB(0x00, 0x00, 0x00));
//		m_edt_int[i].SetReadOnly(TRUE);
		m_edt_int[i].SetInputType(ENM_DITM::en_uint32);
		m_edt_int[i].SetInputSign(ENM_MNUS::en_unsign);
		m_edt_int[i].SetMinMaxNum(INT64(1), INT64(100));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_edt_int[i]);
		// by sysandj : Resize UI
	}
	m_edt_int[eREPT_EDT_COUNT].SetTextToNum(UINT32(1));
	m_edt_int[eREPT_EDT_COUNT].SetDisableKBDN();

	/* button - normal */
	for (i=0; i< eREPT_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}
	/* checkbox - normal */
	for (i=0; i< eREPT_CHK_MAX; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_chk_ctl[i].SetBgColor(RGB(255, 255, 255));
		if (i != 0)
		{
			if (i < eREPT_CHK_ACAM_SIDE)	m_chk_ctl[i].SetCheck(1);
			else
			{
				m_chk_ctl[i].SetCheck(0);
				m_chk_ctl[i].EnableWindow(FALSE);
			}

			m_chk_ctl[eREPT_CHK_LED_ON].SetCheck(1);
			m_chk_ctl[eREPT_CHK_LED_ON].EnableWindow(TRUE);
		}

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_chk_ctl[i]);
		// by sysandj : Resize UI
	}

	return TRUE;
}

/*
 desc : ���� ��ư �̺�Ʈ
 parm : None
 retn : None
*/
VOID CDlgRept::OnBtnClicked(UINT32 id)
{
	CDlgMesg dlgMesg;

	ST_IO IO;
	CIOManager::GetInstance()->UpdateIO();
	if (id == IDC_REPT_BTN_APPLY)
	{
		m_bMovingACam	= FALSE;
		m_bMovingPH		= FALSE;
		m_u32ExpoCount	= (UINT32)m_edt_int[0].GetTextToNum();
		if (m_u32ExpoCount < 1)
		{
			dlgMesg.MyDoModal(L"The number of exposures has not been entered", 0x01);
			return;
		}
		if (m_chk_ctl[0].GetCheck() == 1 && m_u32ExpoCount < 2)
		{
			dlgMesg.MyDoModal(L"The number of repeated exposures should be at leat 2", 0x01);
			return;
		}
		if (m_chk_ctl[1].GetCheck() == 1)
		{
			// by sysandj : MCQ��ü �߰� �ʿ�
// 			if (!uvCmn_MCQ_IsMaterialDetected())
// 			{
// 				dlgMesg.MyDoModal(L"The material is not placed on the work-table", 0x01);
// 				return;
// 			}
	
			/*���������� ���� ���� Ȯ��*/
			CIOManager::GetInstance()->GetIO(eIO_INPUT, _T("CHUCK_VACUUM_ATM_STATUS"), IO);
			if (IO.bOn == FALSE)
			{
				dlgMesg.MyDoModal(L"The material is not placed on the work-table", 0x01);
 				return;
			}
		}
		if (m_chk_ctl[2].GetCheck() == 1)
		{
			// by sysandj : MCQ��ü �߰� �ʿ�
// 			if (!uvEng_ShMem_GetPLCExt()->r_vacuum_controller_running ||
// 				!uvEng_ShMem_GetPLCExt()->r_vacuum_status)
// 			{
// 				dlgMesg.MyDoModal(L"The vacuum is not running (or vaccum mode)", 0x01);
// 				return;
// 			}
			/*Vaccum on ���� Ȯ��*/
			CIOManager::GetInstance()->GetIO(eIO_INPUT, _T("CHUCK_VACUUM_VAC_STATUS"), IO);
			if (IO.bOn == FALSE)
			{
				dlgMesg.MyDoModal(L"The vacuum is not running (or vaccum mode)", 0x01);
				return;
			}
		}
		if (m_chk_ctl[eREPT_CHK_LED_ON].GetCheck() == 0)
		{
			UINT16 u16LedPower[MAX_PH][4] = { NULL };
			if (!uvEng_Luria_ReqSetLedAmplitude(uvEng_GetConfig()->luria_svc.ph_count, u16LedPower))
			{
				dlgMesg.MyDoModal(L"Failed to send the cmd (ReqSetLedAmplitude)", 0x01);
				return;
			}
		}
		else
		{
			CUniToChar csCnv;
			LPG_PLPI pstLedPower = NULL;
			UINT16 u16LedPower[MAX_PH][4] = { NULL };

			LPG_RJAF pstJobRecipe = uvEng_JobRecipe_GetSelectRecipe();
			//LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetSelectRecipe();
			LPG_REAF pstExpoRecipe = uvEng_ExpoRecipe_GetRecipeOnlyName(csCnv.Ansi2Uni(pstJobRecipe->expo_recipe));
			/* �����ǿ� ������ Led Power Name�� �̿��� Led Power Index ��ȸ */
			pstLedPower = uvEng_LedPower_GetLedPowerName(csCnv.Ansi2Uni(pstExpoRecipe->power_name));
			/* ���� ������ Recipe �Ŀ� �� ���� */
			for (int i = 0x00; i < uvEng_GetConfig()->luria_svc.ph_count; i++)
			{
				memcpy(&u16LedPower[i], pstLedPower->led_index[i], sizeof(UINT16) * 4/* Not MAX_LED */);
			}
	
			if (!uvEng_Luria_ReqSetLedAmplitude(uvEng_GetConfig()->luria_svc.ph_count, u16LedPower))
			{
				dlgMesg.MyDoModal(L"Failed to send the cmd (ReqSetLedAmplitude)", 0x01);
				return;
			}
		}
		/* 2 ȸ �̻� �뱤�� ��, �÷��� Ȯ�� */
		if (m_u32ExpoCount > 1)
		{
			m_bMovingACam	= m_chk_ctl[3].GetCheck() == 1 ? TRUE : FALSE;
			m_bMovingPH		= m_chk_ctl[4].GetCheck() == 1 ? TRUE : FALSE;
		}

		CMyDialog::EndDialog(IDOK);
	}
	else
	{
		CMyDialog::EndDialog(IDCANCEL);
	}
}

/*
 desc : ���� üũ�ڽ� �̺�Ʈ
 parm : None
 retn : None
*/
VOID CDlgRept::OnChkClicked(UINT32 id)
{
	UINT8 i	= 0x00;

	/* ���� �뱤 ���ο� ���� */
	BOOL bPreHeated = m_chk_ctl[0].GetCheck() == 1 ? TRUE : FALSE;

	for (i=3; i<5; i++)
	{
		m_chk_ctl[i].EnableWindow(bPreHeated);
		if (!bPreHeated)	m_chk_ctl[i].SetCheck(0);
	}
	
//	m_edt_int[0].SetReadOnly(!bPreHeated);
	if (id == IDC_REPT_CHK_PREHEAT)
	{
		if (bPreHeated)
		{
			m_edt_int[0].SetEnableKBDN();
			m_edt_int[0].SetTextToNum(UINT32(5));
		}
		else
		{
			m_edt_int[0].SetDisableKBDN();
			m_edt_int[0].SetTextToNum(UINT32(1));
		}
	}
	m_edt_int[0].SetRedraw(TRUE);
	m_edt_int[0].Invalidate(TRUE);
}

/*
 desc : DoModal Override �Լ�
 parm : None
 retn : 0L
*/
INT_PTR CDlgRept::MyDoModal()
{
	return DoModal();
}
