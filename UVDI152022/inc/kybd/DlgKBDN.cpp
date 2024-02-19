
/*
 desc : �̴� ���� Ű���� ���
*/

#include "pch.h"
#include "DlgKBDN.h"


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
CDlgKBDN::CDlgKBDN(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgKBDN::IDD, parent)
{
	m_bDrawBG			= 0x01;
	m_bKeyPushEnter		= 0x01;
	m_bKeyPushESC		= 0x01;
	m_bKeyEnterApply	= 0x01;
	m_bKeyESCCancel		= 0x01;
	m_bDotUse			= TRUE;
	m_bSignUse			= TRUE;
	m_pParentWnd		= NULL;
	m_bEnableEnterKey	= FALSE;

	wmemset(m_tzTitle, 0x00, 1024);
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgKBDN::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	DDX_Control(dx, IDC_KBDN_EDT_VALUE,	m_edt_ctl[0]);
	/* button control */
	u32StartID	= IDC_KBDN_BTN_CLOSE;
	for (i=0; i<17; i++)	DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgKBDN, CMyDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_KBDN_BTN_CLOSE, IDC_KBDN_BTN_ENTER, OnBtnClicked)
//	ON_WM_KEYUP()	/* Dialog ��ݿ����� KeyDown & KeyUp Event �޽��� �߻� �ȵ� */
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgKBDN::PreTranslateMessage(MSG* msg)
{
	if (msg->message == WM_KEYUP)
	{
		if (msg->wParam != VK_RETURN && msg->wParam != VK_ESCAPE)
		{
			OnKeyUp(UINT32(msg->wParam),
					UINT32(LOWORD(msg->lParam)),
					UINT32(HIWORD(msg->lParam)));
		}
	}
	else if (msg->message == WM_KEYDOWN)
	{
		if (msg->wParam==VK_RETURN)
		{
			if (m_bKeyPushEnter && m_bEnableEnterKey)
			{
				Apply();
			}
		}
	}

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : Create �Լ� �������̵� ó��
 parm : parent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
		value	- [in]  ���� �����Ǿ� �ִ� ��. Edit â�� ��µǴ� ��
		dot_use	- [in]  TRUE - �Ҽ��� ����, FALSE - �Ҽ��� ������
		sign_use- [in]  TRUE - ���� ���, FALSE - ���� ��� ����
		min_val	- [in]  �Է� ������ �ּ� ��
		max_val	- [in]  �Է� ������ �ִ� ��
 retn : Create �Լ� ��ȯ ��
*/
BOOL CDlgKBDN::Create(CWnd *parent,
					  BOOL dot_use,
					  BOOL sign_use,
					  DOUBLE min_val,
					  DOUBLE max_val)
{
	m_pParentWnd	= parent;
	m_bDotUse		= dot_use;
	m_bSignUse		= sign_use;
	m_dbMin			= min_val;
	m_dbMax			= max_val;

//	return CMyDialog::Create(IDD_LOGS, GetDesktopWindow());
	return CMyDialog::Create();
}

/*
 desc : DoModal Overriding
 parm : dot_use	- [in]  TRUE - �Ҽ��� ����, FALSE - �Ҽ��� ������
		sign_use- [in]  TRUE - ���� ���, FALSE - ���� ��� ����
		min_val	- [in]  �Է� ������ �ּ� ��
		max_val	- [in]  �Է� ������ �ִ� ��
 retn : INT_PTR
*/
INT_PTR CDlgKBDN::MyDoModal(TCHAR *title, BOOL dot_use, BOOL sign_use, DOUBLE min_val, DOUBLE max_val)
{
	wmemset(m_tzTitle, 0x00, 1024);
	if (title)	wcscpy_s(m_tzTitle, 1024, title);

	m_pParentWnd= NULL;
	m_bDotUse	= dot_use;
	m_bSignUse	= sign_use;

	m_dbMin		= min_val;
	m_dbMax		= max_val;

	return CMyDialog::DoModal();
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgKBDN::OnInitDlg()
{
	// �ڽ��� �θ� ���� �߾ӿ� ��ġ ��Ŵ
	CenterWindow(m_pParentWnd);
	/* ���� ���� ���� */
	if (wcslen(m_tzTitle))	SetWindowText(m_tzTitle);

	/* ���� ��Ʈ�� �ʱ�ȭ */
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
VOID CDlgKBDN::OnExitDlg()
{
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgKBDN::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgKBDN::OnResizeDlg()
{
	CRect rCtrl;
	// ���� Client ����
	GetClientRect(rCtrl);
}

void CDlgKBDN::RegisterUILevel()
{

}

/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgKBDN::InitCtrl()
{
	UINT32 i;
	LOGFONT lf	= g_lf[eFONT_LEVEL2_BOLD];

  	// �Ϲ� ��ư
	for (i=0; i<17; i++)
 	{
 		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
 		//m_btn_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);
 	}
	m_btn_ctl[16].EnableWindow(FALSE);
	m_btn_ctl[0].SetFocus();
	// ����Ʈ
 	lf.lfHeight	+= 16;
 	lf.lfWeight	+= 16;
	for (i=0; i<1; i++)
	{
		m_edt_ctl[i].SetEditFont(&lf);
		m_edt_ctl[i].SetReadOnly(TRUE);
	}
	// �� ���
	m_edt_ctl[0].SetWindowText(L"");
	m_edt_ctl[0].SetMouseClick(FALSE);

	/* ��ȣ ���� ���� */
	m_btn_ctl[11].EnableWindow((BOOL)m_bSignUse);
	/* �Ҽ��� ���� ���� */
	m_btn_ctl[12].EnableWindow((BOOL)m_bDotUse);

	return TRUE;
}

/*
 desc : ���� Ű���尡 ���ȴٰ� ������ ��� ����
 parm : key_num	- [in]  �־��� Ű�� ���� ���� �ڵ� ���� ����
		rep_cnt	- [in]  ����ڰ� Ű�� ������ ���� ��, �ݺ� Ƚ��
		flags	- [in]  �˻� �ڵ�, Ű ��ȯ �ڵ�, ���� Ű ���� �� ���ؽ�Ʈ �ڵ带 ����
 retn : None
*/
VOID CDlgKBDN::OnKeyUp(UINT32 key_num, UINT32 rep_cnt, UINT32 flags)
{
	BOOL bSucc[6]	= {FALSE};
	TCHAR tzNum[10]	= {L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9'};

	/* ---------------------------------------------- */
	/* ���� Ű���� ���� �������� ���� ���, ���� ó�� */
	/* ---------------------------------------------- */
	bSucc[0] = (47 < key_num && key_num < 58);
	bSucc[1] = (95 < key_num && key_num < 106);
	bSucc[2] = (190 == key_num || key_num == 110);
	bSucc[3] = (key_num == 8 || key_num == 46);
	bSucc[4] = (key_num == 189 || key_num == 109);	// '-'
	bSucc[5] = (key_num == 187 || key_num == 107);	// '+'
	// Backspace or Delete Key�� ������ ���
	if (bSucc[3])	DelCh();
	// ���� Ű���忡�� �߻��� ���ڰ� �Է�
	else
	{
		if (bSucc[4])		SetSign(0x02);	// '-'
		else if (bSucc[5])	SetSign(0x01);	// '+'
		else if (bSucc[2])	AddCh(L'.');
		else if (bSucc[0])	AddCh(tzNum[key_num-48]);
		else if (bSucc[1])	AddCh(tzNum[key_num-96]);
	}
}

/*
 desc : ��ư Ŭ�� �̺�Ʈ ó��
 parm : id	- [in]  Ŭ���� ��ư ID
 retn : None
*/
VOID CDlgKBDN::OnBtnClicked(UINT32 id)
{
	TCHAR tzNum[11]	= {L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'0', L'.'};
	switch (id)
	{
	case IDC_KBDN_BTN_CLOSE		:	Cancel();	break;
	case IDC_KBDN_BTN_ENTER		:	Apply();	break;
	case IDC_KBDN_BTN_NUM1		:
	case IDC_KBDN_BTN_NUM2		:
	case IDC_KBDN_BTN_NUM3		:
	case IDC_KBDN_BTN_NUM4		:
	case IDC_KBDN_BTN_NUM5		:
	case IDC_KBDN_BTN_NUM6		:
	case IDC_KBDN_BTN_NUM7		:
	case IDC_KBDN_BTN_NUM8		:
	case IDC_KBDN_BTN_NUM9		:
	case IDC_KBDN_BTN_NUM0		:	AddCh(tzNum[id-IDC_KBDN_BTN_NUM1]);	break;
	case IDC_KBDN_BTN_BACK		:	DelCh();			break;
	case IDC_KBDN_BTN_CLEAR		:	Clear();			break;
	case IDC_KBDN_BTN_SIGN		:	SetSign(0x00);		break;
	case IDC_KBDN_BTN_DOT		:	AddCh(tzNum[10]);	break;
	case IDC_KBDN_BTN_SEL		:	SelectAll();		break;
	}
}

/*
 desc : Ű������ ���� ��ư (��ȣ �� Dot ����) Ȱ��ȭ ����
 parm : None
 retn : None
*/
VOID CDlgKBDN::EnableEnterKey()
{
	BOOL bEnable	= FALSE;
	TCHAR tzVal[512]= {NULL};
	DOUBLE dbVal	= 0;
	/* ������� �Էµ� �� �߿��� Min ������ �۰ų� Max ������ ū ���, �� �̻� �Է� ���ϵ��� ó�� */
	m_edt_ctl[0].GetWindowTextW(tzVal, 512);
	dbVal	= _wtof(tzVal);
	m_bEnableEnterKey	= FALSE;
	if (dbVal >= m_dbMin && dbVal <= m_dbMax)	m_bEnableEnterKey	= TRUE;
	m_btn_ctl[16].EnableWindow(m_bEnableEnterKey);
	if (!bEnable)	m_btn_ctl[0].SetFocus();
	else			m_btn_ctl[16].SetFocus();
}

/*
 desc : Keyboard���� Enter Ű ���� ������ ���
 parm : None
 retn : None
*/
VOID CDlgKBDN::Apply()
{
	/* ���ڿ� ���� �ʱ�ȭ */
	wmemset(m_tzValue, 0x00, 1024);

	/* ���� ������ �� ��� */
	m_edt_ctl[0].GetWindowText(m_tzValue, 1024);	/* ���ڿ� ��� */
	m_i16Value	= (INT16)_wtoi(m_tzValue);			/* I64 ��� */
	m_u16Value	= (UINT16)_wtoi(m_tzValue);			/* U64 ��� */
	m_i32Value	= (INT32)_wtoi(m_tzValue);			/* I64 ��� */
	m_u32Value	= (UINT32)_wtoi(m_tzValue);			/* U64 ��� */
	m_i64Value	= (INT64)_wtoi(m_tzValue);			/* I64 ��� */
	m_u64Value	= (UINT64)_wtoi(m_tzValue);			/* U64 ��� */
	m_dbValue	= (DOUBLE)_wtof(m_tzValue);			/* DOUBLE ��� */

	/* �Էµ� ���ڿ� ���̰� ������ Ȯ�� */
	if (wcslen(m_tzValue) < 1)	return;

	if (m_pParentWnd)
	{
		m_pParentWnd->PostMessage(WM_IDD_KBDM, 1, (LPARAM)m_pParentWnd);
	}
	else
	{
		CMyDialog::OnOK();
	}
}

/*
 desc : Keyboard���� Cancel Ű ���� ������ ���
 parm : None
 retn : None
*/
VOID CDlgKBDN::Cancel()
{
	if (m_pParentWnd)
	{
		m_pParentWnd->PostMessage(WM_IDD_KBDM, 0, (LPARAM)m_pParentWnd);
	}
	else
	{
		CMyDialog::OnCancel();
	}
}


/*
 desc : ����Ʈ ��Ʈ�� �ڽ��� �� �Է�
 parm : ch	- [in]  �Է� (�߰�)�� ���� ��
 retn : None
*/
VOID CDlgKBDN::AddCh(TCHAR ch)
{
	TCHAR tzText[2]	= {ch, NULL}, tzData[256] = {NULL};
	INT32 i, i32Len	= m_edt_ctl[0].GetWindowTextLength();

	// ����� �� �ؽ�Ʈ�� �Էµ� �������� Ȯ��
	if (i32Len > 24)	return;
	// ���� �Էµ� ���ڰ� Dot (L'.')�̰�, ������ �̹� Dot�� �Էµ� �������� Ȯ��
	for (i=0; (ch == L'.' && i<i32Len); i++)
	{
		// ������� �Էµ� ���ڿ� ���
		m_edt_ctl[0].GetWindowText(tzData, 256);
		if (tzData[i] == L'.')	return;
	}
	// ���� �Էµ� ù ���ڰ� Dot (L'.')�� �����, Ȥ�� '+', '-' �̶��
	if ((i32Len == 0 && ch == L'.') ||
		(i32Len == 1 && ch == L'.' && (tzData[0] == L'+' || tzData[0] == L'-')))
	{
		tzText[0]	= L'0';
		tzText[1]	= ch;
	}
	m_edt_ctl[0].SetFocus();
	// �Էµ� ���ڿ��� ���������� �̵�
	m_edt_ctl[0].SetSel(i32Len, i32Len);
	// �������� ���� ���ڿ� �߰�
	m_edt_ctl[0].ReplaceSel(tzText);

	/* Enable Enter Key */
	EnableEnterKey();
}

/*
 desc : ����Ʈ ��Ʈ�� �ڽ��� �� �Է�
 parm : None
 retn : None
*/
VOID CDlgKBDN::DelCh()
{
	TCHAR tzText[256]	= {NULL};
	INT32 i32Len		= m_edt_ctl[0].GetWindowTextLength();

	// ���� �Էµ� ���� ����
	m_edt_ctl[0].GetWindowText(tzText, 256);
	// �� �̻� ���ŵ� ���ڰ� ������ Ȯ��
	if (i32Len < 1)	return;
	// ������ ���� ����
	tzText[i32Len-1]	= NULL;
	// ������ ���ڿ� �ٿ��ֱ�
	m_edt_ctl[0].SetWindowText(tzText);

	/* Enable Enter Key */
	EnableEnterKey();
}

/*
 desc : ���ڿ� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgKBDN::Clear()
{
	// ��Ŀ�� ����
	m_edt_ctl[0].SetFocus();
	// �Էµ� ���ڿ��� ��ü ����
	m_edt_ctl[0].SetSel(0, -1, TRUE);
	m_edt_ctl[0].Clear();

	/* Enable Enter Key */
	EnableEnterKey();
}

/*
 desc : �ε�ȣ �Է�
 parm : ch	- [in]  0x00 - ��ư '+/-', 0x01 - '+', 0x02 - '-'
 retn : None
*/
VOID CDlgKBDN::SetSign(UINT8 ch)
{
	TCHAR tzText[256]	= {NULL};
	INT32 i, i32Len		= m_edt_ctl[0].GetWindowTextLength();

	// ���� �Էµ� ���� ����
	m_edt_ctl[0].GetWindowText(tzText, 256);
	// �Էµ� ���ڿ��� �� ���� ���ڰ� '+' or '-'���� Ȯ��
	if (tzText[0] == L'+' || tzText[0] == L'-')
	{
		if (tzText[0] == L'+')	tzText[0] = (0x01 == ch) ? L'+' : L'-';
		else					tzText[0] = (0x02 == ch) ? L'-' : L'+';
	}
	else
	{
		// �ϴ� ���� �Էµ� ���ڿ��� ��� �� ���� �ڷ� �̵� ��Ŵ
		for (i=i32Len-1; i>=0; i--)	tzText[i+1]	= tzText[i];
		// �� �տ� ������ L'+' �� �Է�
		tzText[0] = (0x01 == ch) ? L'+' : L'-';
	}

	// ���ڿ� �Է�
	m_edt_ctl[0].SetWindowText(tzText);

	/* Enable Enter Key */
	EnableEnterKey();
}

/*
 desc : Ű���� �� �� Dot '.' ��ư�� Enable or Disabl ó��
 parm : flag	- [in]  Enable or Disable
 retn : None
*/
VOID CDlgKBDN::DotEnable(BOOL enable)
{
	m_btn_ctl[12].EnableWindow(enable);
}

/*
 desc : Ű���� �� �� '+/-' ��ư�� Enable or Disabl ó��
 parm : flag	- [in]  Enable or Disable
 retn : None
*/
VOID CDlgKBDN::SignEnable(BOOL enable)
{
	m_btn_ctl[11].EnableWindow(enable);
}

/*
 desc : Edit â�� ������ ��� ���� �Ѵ�
 parm : None
 retn : None
*/
VOID CDlgKBDN::SelectAll()
{
	// ��Ŀ�� ����
	m_edt_ctl[0].SetFocus();
	// �Էµ� ���ڿ��� ��ü ����
	m_edt_ctl[0].SetSel(0, -1, TRUE);
}