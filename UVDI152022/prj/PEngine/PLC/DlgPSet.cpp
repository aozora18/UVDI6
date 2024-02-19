
/*
 desc : PLC Value Testing (Register D Address�� �� �׽�Ʈ)
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgPSet.h"


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
CDlgPSet::CDlgPSet(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgPSet::IDD, parent)
{
	m_bDrawBG		= TRUE;
	m_bTooltip		= TRUE;
	m_u16SetAddr	= 0x0000;
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgPSet::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Groupbox */
	u32StartID	= IDC_PSET_GRP_SETVAL;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Combobox */
	u32StartID	= IDC_PSET_CMB_DADDR;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_cmb_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_PSET_TXT_70;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Button - Normal */
	u32StartID	= IDC_PSET_BTN_SAVE;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Check - Normal */
	u32StartID	= IDC_PSET_CHK_16BIT;
	for (i=0; i<20; i++)	DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
	/* Edit - number */
	u32StartID	= IDC_PSET_EDT_VALUE;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgPSet, CMyDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_PSET_BTN_SAVE, IDC_PSET_BTN_SAVE,	OnBtnClicked)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_PSET_CHK_16BIT, IDC_PSET_CHK_BF,	OnChkClicked)
	ON_CBN_SELCHANGE(IDC_PSET_CMB_DADDR, OnCmbSelChangeDAddr)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgPSet::PreTranslateMessage(MSG* msg)
{
	 if(msg->message == WM_KEYUP)
	 {
		 if (msg->hwnd == m_edt_ctl[0].m_hWnd)
		 {
			 /* ���� ���� 16 bit�� ���, */
			 if (m_chk_ctl[0].GetCheck())
			 {
				 /* �ִ� ���� ���� �ʵ��� ���� */
				 if (m_edt_ctl[0].GetTextToNum() > UINT16_MAX)
				 {
					 m_edt_ctl[0].SetTextToNum(UINT16(UINT16_MAX));
				 }
			 }
			 else
			 {
				 if (m_edt_ctl[0].GetTextToNum() > UINT32_MAX)
				 {
					 m_edt_ctl[0].SetTextToNum(UINT16(UINT32_MAX));
				 }
			 }
			 ChangeEdtValue();
		 }
	 }

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgPSet::OnInitDlg()
{
	/* ��Ʈ�� �ʱ�ȭ */
	if (!InitCtrl())	return FALSE;

	/* TOP_MOST & Center */
	//CenterParentTopMost();

	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgPSet::OnExitDlg()
{
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgPSet::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgPSet::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgPSet::InitCtrl()
{
	INT32 i;

	/* Groupbox */
	for (i=0; i<1; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* Combobox */
	for (i=0; i<1; i++)
	{
		m_cmb_ctl[i].SetLogFont(&g_lf);
	}
	/* text - normal */
	for (i=0; i<2; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* button - normal */
	for (i=0; i<1; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	/* checkbox - normal */
	for (i=0; i<20; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf);
		m_chk_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	/* edit - angle / size */
	for (i=0; i<1; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_ctl[i].SetInputType(EN_DITM::en_hex10);
	}

	/* üũ �ڽ� �ʱ�ȭ */
	m_chk_ctl[0].SetCheck(1);
	m_chk_ctl[1].SetCheck(0);
	m_chk_ctl[2].SetCheck(1);
	m_chk_ctl[3].SetCheck(0);
	/* �ּ� ���� */
	LoadAddr();
	/* ī�޶� ��� ��� */
	UpdateValue();
	/* üũ �ڽ� �ٲ� */
	ChangeEdtValue();

	return TRUE;
}

/*
 desc : ��ư �̺�Ʈ
 parm : id	- [in]  ��ư ID
 retn : None
*/
VOID CDlgPSet::OnBtnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_PSET_BTN_SAVE		:	SetValue();		break;
	}
}

/*
 desc : ��ư �̺�Ʈ
 parm : id	- [in]  ��ư ID
 retn : None
*/
VOID CDlgPSet::OnChkClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_PSET_CHK_16BIT	:	m_chk_ctl[1].SetCheck(0);	break;
	case IDC_PSET_CHK_32BIT	:	m_chk_ctl[0].SetCheck(0);	break;
	case IDC_PSET_CHK_DEC	:
	case IDC_PSET_CHK_HEX	:
		switch (id)
		{
		case IDC_PSET_CHK_DEC	:	m_chk_ctl[3].SetCheck(0);
									m_edt_ctl[0].SetInputType(EN_DITM::en_hex10);	break;
		case IDC_PSET_CHK_HEX	:	m_chk_ctl[2].SetCheck(0);
									m_edt_ctl[0].SetInputType(EN_DITM::en_hex16);	break;
		}
		ChangeHexValue();	break;
	default					:	ChangeChkValue();	break;
	}
}

/*
 desc : DoModal Override Function
 parm : addr	- [in]  PLC Memory Address (D Register. Word Address Index. Zero-based)
 retn : 0 or Negative
*/
INT_PTR CDlgPSet::MyDoModal(UINT16 addr)
{
	m_u16SetAddr	= addr;
	return DoModal();
}

/*
 desc : �ּ� ����
 parm : None
 retn : None
*/
VOID CDlgPSet::LoadAddr()
{
	TCHAR tzAddr[8]	= {NULL};
	UINT16 u16Start	= 0;

	m_cmb_ctl[0].SetRedraw(FALSE);
	m_cmb_ctl[0].ResetContent();

	for (u16Start=0; u16Start<uvEng_Conf_GetPLC()->addr_d_count; u16Start++)
	{
		swprintf_s(tzAddr, 8, L"%05d", u16Start+uvEng_Conf_GetPLC()->start_d_addr);
		m_cmb_ctl[0].AddString(tzAddr);
	}

	m_cmb_ctl[0].SetRedraw(TRUE);
	m_cmb_ctl[0].Invalidate(FALSE);
}

/*
 desc : Deciaml or Hex ���� ����� ���, Edit Control�� �� �ٲ�
 parm : None
 retn : None
*/
VOID CDlgPSet::ChangeHexValue()
{
	UINT32 u32Val	= 0;
	TCHAR tzVal[32]	= {NULL};

	/* Decimal or Hex Ȯ�� */
	if (m_chk_ctl[2].GetCheck())
	{
		u32Val	= (UINT32)m_edt_ctl[0].GetTextToHex();
		m_edt_ctl[0].SetTextToNum(u32Val);
	}
	else
	{
		u32Val	= (UINT32)m_edt_ctl[0].GetTextToNum();
		swprintf_s(tzVal, 32, L"%X", u32Val);
		m_edt_ctl[0].SetWindowText(tzVal);
	}
}

/*
 desc : üũ �ڽ� �̺�Ʈ�� �߻��� ���, Edit Control�� �� �ٲ�
 parm : None
 retn : None
*/
VOID CDlgPSet::ChangeChkValue()
{
	UINT32 i, u32Val	= 0;
	TCHAR tzVal[16]	= {NULL};

	for (i=16; i>0; i--)
	{
		u32Val	|= (UINT8)m_chk_ctl[4+i-1].GetCheck();
		if (i != 1)	u32Val	= u32Val << 1;
	}

	/* Decimal or Hex Ȯ�� */
	if (m_chk_ctl[2].GetCheck())	swprintf_s(tzVal, 16, L"%u", u32Val);	/* Decimal */
	else							swprintf_s(tzVal, 16, L"%X", u32Val);	/* Hex */

	/* ����Ʈ �ڽ��� ��� */
	m_edt_ctl[0].SetWindowText(tzVal);
}

/*
 desc : ����Ʈ �ڽ� �̺�Ʈ�� �߻��� ���, Checkbox�� ���� �ٲ�
 parm : None
 retn : None
*/
VOID CDlgPSet::ChangeEdtValue()
{
	UINT32 i, u32Val	= 0;

	/* Decimal or Hex Ȯ�� */
	if (m_chk_ctl[2].GetCheck())	u32Val	= (UINT32)m_edt_ctl[0].GetTextToNum();	/* Decimal */
	else							u32Val	= (UINT32)m_edt_ctl[0].GetTextToHex();	/* Hex */

	/* 32 bit �Է� ����̸鼭, UINT16_MAX ���� ���� ���� ���� */
	if (m_chk_ctl[1].GetCheck() && u32Val > UINT16_MAX)	return;
	/* üũ ��ư �Ӽ� ���� */
	for (i=0; i<16; i++)
	{
		m_chk_ctl[4+i].SetCheck(u32Val & 0x00000001);
		u32Val	= u32Val >> 1;
	}
}

/*
 desc : ���� PLC Memory (D Register) �ּ��� �� ����
 parm : None
 retn : None
*/
VOID CDlgPSet::UpdateValue()
{
	TCHAR tzVal[32]	= {NULL};
	UINT16 u16Index;
	UINT32 u32Val;
	PUINT16 pPLCMem	= (PUINT16)uvEng_ShMem_GetPLC()->data;

	/* ���� ����� �ּ� �ε��� �� */
	u16Index	= uvEng_Conf_GetPLC()->GetAddrToIndex(m_u16SetAddr);
	if (0xffff == u16Index)	return;

	/* Comboxbox �ּ� �� ���� */
	m_cmb_ctl[0].SetCurSel(u16Index);

	/* ���� ������ �ּ��� ���, ������ 16 bit �� ��� */
	if (m_u16SetAddr == uvEng_Conf_GetPLC()->GetLastDAddr())
	{
		m_chk_ctl[0].SetCheck(1);
		m_chk_ctl[1].SetCheck(0);
		/* �� ��ȯ */
		if (m_chk_ctl[2].GetCheck())	m_edt_ctl[0].SetTextToNum(pPLCMem[u16Index]);
		else
		{
			swprintf_s(tzVal, 32, L"%X", pPLCMem[u16Index]);
			m_edt_ctl[0].SetWindowText(tzVal);
		}
	}
	else
	{
		/* �޸� �ε��� ��ġ�� �ش�Ǵ� �� �о���� */
		if (m_chk_ctl[0].GetCheck())
		{
			if (m_chk_ctl[2].GetCheck())	m_edt_ctl[0].SetTextToNum(pPLCMem[u16Index]);
			else
			{
				swprintf_s(tzVal, 32, L"%X", pPLCMem[u16Index]);
				m_edt_ctl[0].SetWindowText(tzVal);
			}
		}
		else
		{
			memcpy(&u32Val, &pPLCMem[u16Index], 4);
			if (m_chk_ctl[2].GetCheck())	m_edt_ctl[0].SetTextToNum(u32Val);
			else
			{
				swprintf_s(tzVal, 32, L"%X", u32Val);
				m_edt_ctl[0].SetWindowText(tzVal);
			}
		}
	}
}

/*
 desc : Combobox���� �ּ� ��ġ�� ���� ���� ���, �ּ� ���� �ش�Ǵ� �޸� �� ���
 parm : None
 retn : None
*/
VOID CDlgPSet::OnCmbSelChangeDAddr()
{
	TCHAR tzAddr[8]	= {NULL};
	INT32 i32Sel	= 0;
	
	/* ���� ���õ� ��ġ ��� */
	i32Sel	= m_cmb_ctl[0].GetCurSel();
	if (i32Sel < 0)	return;

	/* ���ڿ� -> �ּ� (����) ��ȯ */
	m_cmb_ctl[0].GetLBText(i32Sel, tzAddr);

	/* ���� �ּ� �ε� �� ��� */
	m_u16SetAddr	= uvEng_Conf_GetPLC()->GetAddrToIndex((UINT16)_wtoi(tzAddr));

	/* �� ���� */
	UpdateValue();
	ChangeEdtValue();
}

/*
 desc : PLC�� �� ����
 parm : None
 retn : None
*/
VOID CDlgPSet::SetValue()
{
	UINT32 u32Addr	= (UINT32)m_cmb_ctl[0].GetLBTextToNum();

	/* for 16 bit */
	if (m_chk_ctl[0].GetCheck())
	{
		UINT16 u16Value	= 0;
		/* for decimal */
		if (m_chk_ctl[2].GetCheck())	u16Value	= (UINT16)m_edt_ctl[0].GetTextToNum();
		/* for hexa */
		else							u16Value	= (UINT16)m_edt_ctl[0].GetTextToHex();
		uvCmn_MCQ_SetDataPLC(1, u32Addr, &u16Value);
	}
	/* for 32 bit */
	else
	{
		UINT16 u16Value[2]	= {NULL};
		UINT32 u32Value	= 0;
		/* for decimal */
		if (m_chk_ctl[2].GetCheck())	u32Value	= (UINT32)m_edt_ctl[0].GetTextToNum();
		/* for hexa */
		else							u32Value	= (UINT32)m_edt_ctl[0].GetTextToHex();
		memcpy(u16Value, &u32Value, sizeof(UINT32));
		uvCmn_MCQ_SetDataPLC(2, u32Addr, u16Value);
	}
}