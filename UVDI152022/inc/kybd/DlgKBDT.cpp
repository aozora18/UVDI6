
/*
 desc : �̴� ���� Ű���� ���
*/

#include "pch.h"
#include "DlgKBDT.h"


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
CDlgKBDT::CDlgKBDT(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgKBDT::IDD, parent)
{
	m_bShift			= 0x00;	// �⺻������ �빮��
	m_bDrawBG			= 0x01;
	m_bKeyPushEnter		= 0x01;
	m_bKeyPushESC		= 0x01;
	m_bKeyEnterApply	= 0x01;
	m_bKeyESCCancel		= 0x01;
	m_pParentWnd		= NULL;
	m_bPassword			= FALSE;
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgKBDT::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32ID;

	DDX_Control(dx, IDC_KBDT_EDT_TEXT,	m_edt_ctl[0]);
#if 0
	DDX_Control(dx, IDC_KBDT_BTN_NUM0,	m_btn_ctl[0]);
	DDX_Control(dx, IDC_KBDT_BTN_NUM1,	m_btn_ctl[1]);
	DDX_Control(dx, IDC_KBDT_BTN_NUM2,	m_btn_ctl[2]);
	DDX_Control(dx, IDC_KBDT_BTN_NUM3,	m_btn_ctl[3]);
	DDX_Control(dx, IDC_KBDT_BTN_NUM4,	m_btn_ctl[4]);
	DDX_Control(dx, IDC_KBDT_BTN_NUM5,	m_btn_ctl[5]);
	DDX_Control(dx, IDC_KBDT_BTN_NUM6,	m_btn_ctl[6]);
	DDX_Control(dx, IDC_KBDT_BTN_NUM7,	m_btn_ctl[7]);
	DDX_Control(dx, IDC_KBDT_BTN_NUM8,	m_btn_ctl[8]);
	DDX_Control(dx, IDC_KBDT_BTN_NUM9,	m_btn_ctl[9]);
#else
	u32ID	= IDC_KBDT_BTN_NUM0;
	for (i=0; i<10; i++)
	{
		DDX_Control(dx, u32ID+i,	m_btn_ctl[0+i]);
	}
	u32ID	= IDC_KBDT_BTN_CHA;
	for (i=0; i<26; i++)
	{
		DDX_Control(dx, u32ID+i,	m_btn_ctl[10+i]);
	}
#endif
#if (CUSTOM_CODE_UTRONIX  != DELIVERY_PRODUCT_ID)
	DDX_Control(dx, IDC_KBDT_BTN_SLASH,	m_btn_ctl[36]);
	DDX_Control(dx, IDC_KBDT_BTN_SHIFT,	m_btn_ctl[37]);
#endif
	DDX_Control(dx, IDC_KBDT_BTN_CLOSE,	m_btn_ctl[38]);
	DDX_Control(dx, IDC_KBDT_BTN_BACK,	m_btn_ctl[39]);
	DDX_Control(dx, IDC_KBDT_BTN_CLEAR,	m_btn_ctl[40]);
	DDX_Control(dx, IDC_KBDT_BTN_ENTER,	m_btn_ctl[41]);
	DDX_Control(dx, IDC_KBDT_BTN_UBAR,	m_btn_ctl[42]);
	DDX_Control(dx, IDC_KBDT_BTN_SEL,	m_btn_ctl[43]);
	DDX_Control(dx, IDC_KBDT_BTN_MINUS,	m_btn_ctl[44]);
#if (CUSTOM_CODE_UTRONIX  != DELIVERY_PRODUCT_ID)
	DDX_Control(dx, IDC_KBDT_BTN_SPACE,	m_btn_ctl[45]);
#endif
}

BEGIN_MESSAGE_MAP(CDlgKBDT, CMyDialog)
#if (CUSTOM_CODE_UTRONIX  != DELIVERY_PRODUCT_ID)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_KBDT_BTN_ENTER, IDC_KBDT_BTN_SPACE, OnBtnClicked)
#endif
//	ON_WM_KEYUP()	/* Dialog ��ݿ����� KeyDown & KeyUp Event �޽��� �߻� �ȵ� */
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgKBDT::PreTranslateMessage(MSG* msg)
{
	switch (msg->message)
	{
	case WM_KEYUP		:
		OnKeyUp(UINT32(msg->wParam),
				UINT32(LOWORD(msg->lParam)),
				UINT32(HIWORD(msg->lParam)));
		break;
	case  WM_KEYDOWN	:
		if (msg->wParam==VK_RETURN)
		{
#if (CUSTOM_CODE_UTRONIX  != DELIVERY_PRODUCT_ID)
			OnBtnClicked(IDC_KBDT_BTN_ENTER);
#endif
		}
		break;
	}

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : Create �Լ� �������̵� ó��
 parm : parent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
		value	- [in]  ���� ���õ� �� ��, Edit â�� ��µ� ��
 retn : Create �Լ� ��ȯ ��
*/
BOOL CDlgKBDT::Create(CWnd *parent, TCHAR *value)
{
	m_pParentWnd	= parent;
	wmemset(m_tzText, 0x00, 1024);
	wcscpy_s(m_tzText, 1024, value);

//	return CMyDialog::Create(IDD_LOGS, GetDesktopWindow());
	return CMyDialog::Create();
}

/*
 desc : DoModal Overriding
 parm : max_str	- [in]  �ִ� �Է��� �� �ִ� ����
 retn : INT_PTR
*/
INT_PTR CDlgKBDT::MyDoModal(UINT32 max_str, BOOL bPassword/* = FALSE*/)
{
	m_pParentWnd	= NULL;
	m_u32MaxStr		= max_str < 1023 ? max_str : 1023;
	m_bPassword		= bPassword;

	return CMyDialog::DoModal();
}

/*
 desc : DoModal Override
 ���� :	value	- [in]  ���� ���õ� �� ��, Edit â�� ��µ� ��
		max_str	- [in]  �ִ� �Է��� �� �ִ� ����
 retn : ?
*/
INT_PTR CDlgKBDT::MyDoModal(TCHAR *value, UINT32 max_str)
{
	m_u32MaxStr	= max_str < 1023 ? max_str : 1023;

	if (value)
	{
		wmemset(m_tzText, 0x00, 1024);
		wcscpy_s(m_tzText, 1024, value);
	}

	return DoModal();
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgKBDT::OnInitDlg()
{
	// �ڽ��� �θ� ���� �߾ӿ� ��ġ ��Ŵ
	CenterWindow(m_pParentWnd);
	/* ���� ��Ʈ�� �ʱ�ȭ */
	if (!InitCtrl())	return FALSE;
	/* TOP_MOST & Center */
	CenterParentTopMost();

	/* ���� Ÿ��Ʋ �Է� */
	TCHAR tzTitle[1024]	= {NULL}, tzMain[1024]	= {NULL};
	/* ���� ���� */
	GetWindowText(tzMain, 1023);
	/* �ִ� �� ���±� */
	swprintf_s(tzTitle, 1024, L"%s (Max : %u)", tzMain, m_u32MaxStr);
	SetWindowText(tzTitle);

	m_edt_ctl[0].SetWindowText(m_tzText);

	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgKBDT::OnExitDlg()
{
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgKBDT::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgKBDT::OnResizeDlg()
{
	CRect rCtrl;
	// ���� Client ����
	GetClientRect(rCtrl);
}

/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgKBDT::InitCtrl()
{
	UINT32 i;
	LOGFONT lf	= g_lf[eFONT_LEVEL2_BOLD];

  	// �Ϲ� ��ư
	for (i=0; i<46; i++)
 	{
 		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
 		//m_btn_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);
 	}
	// ����Ʈ
//  	lf.lfHeight	+= 16;
//  	lf.lfWeight	+= 16;
	lf.lfHeight += 8;
	lf.lfWeight += 8;
	for (i=0; i<1; i++)
	{
		m_edt_ctl[i].SetEditFont(&lf);
		m_edt_ctl[i].SetReadOnly(TRUE);
	}
	// �� ���
	m_edt_ctl[0].SetWindowText(L"");
	m_edt_ctl[0].SetMouseClick(FALSE);


	if (m_bPassword)
	{
		m_edt_ctl[0].SetPasswordChar(_T('*'));
	}


	return TRUE;
}

/*
 desc : ���� Ű���尡 ���ȴٰ� ������ ��� ����
 parm : key_num	- [in]  �־��� Ű�� ���� ���� �ڵ� ���� ����
		rep_cnt	- [in]  ����ڰ� Ű�� ������ ���� ��, �ݺ� Ƚ��
		flags	- [in]  �˻� �ڵ�, Ű ��ȯ �ڵ�, ���� Ű ���� �� ���ؽ�Ʈ �ڵ带 ����
 retn : None
*/
VOID CDlgKBDT::OnKeyUp(UINT32 key_num, UINT32 rep_cnt, UINT32 flags)
{
	BOOL bSucc[7]= {FALSE};
	/*UINT8 u8Index	= 0;*/
#if 0
	TCHAR tzNum[10]	= {L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9'};
#else
	TCHAR tzNum[2]	= {0x30, 0x00};
#endif

	/* ---------------------------------------------- */
	/* ���� Ű���� ���� �������� ���� ���, ���� ó�� */
	/* ---------------------------------------------- */
	bSucc[0] = (46 < key_num && key_num < 58);
	bSucc[1] = (95 < key_num && key_num < 106);
	bSucc[2] = (190 == key_num || key_num == 110);
	bSucc[3] = (key_num == 8 || key_num == 46);
	bSucc[4] = (key_num == 189 || key_num == 109);	// '-'
	bSucc[5] = (64 < key_num && key_num < 91);
	bSucc[6] = (191 == key_num);
	// Backspace or Delete Key�� ������ ���
	if (bSucc[3])	DelCh();
	// ���� Ű���忡�� �߻��� ���ڰ� �Է�
	else
	{
		if (bSucc[4])
		{
			if (GetKeyState(VK_SHIFT) & 0x8000)	AddCh(L'_');
			else								AddCh(L'-');
		}
		else if (bSucc[2])	AddCh(L'.');
		// ���� ��
		else if (bSucc[0])	AddCh(tzNum[0]+TCHAR(key_num-48));
		else if (bSucc[1])	AddCh(tzNum[0]+TCHAR(key_num-96));
		// ���� ��
		else if (bSucc[5])
		{
			// Caps lock�� ON ������ ���
			if (GetKeyState(VK_CAPITAL) & 0x8000)
			{
				// �빮��
				if (GetKeyState(VK_SHIFT) & 0x8000)	AddCh(0x41+TCHAR(key_num-65));
				// �ҹ���
				else								AddCh(0x61+TCHAR(key_num-65));
			}
			else
			// Caps lock�� OFF ������ ���
			{
				// �ҹ���
				if (GetKeyState(VK_SHIFT) & 0x8000)	AddCh(0x41+TCHAR(key_num-65));
				// �빮��
				else								AddCh(0x61+TCHAR(key_num-65));
			}
		}
		else if (bSucc[6])	AddCh(L'/');
	}
}
#if (CUSTOM_CODE_UTRONIX  != DELIVERY_PRODUCT_ID)
/*
 desc : ��ư Ŭ�� �̺�Ʈ ó��
 parm : id	- [in]  Ŭ���� ��ư ID
 retn : None
*/
VOID CDlgKBDT::OnBtnClicked(UINT32 id)
{
	if (id >= IDC_KBDT_BTN_NUM0 && id <= IDC_KBDT_BTN_NUM9)
	{
		AddCh(0x30+TCHAR(id-IDC_KBDT_BTN_NUM0));
	}
	else if (id >= IDC_KBDT_BTN_CHA && id <= IDC_KBDT_BTN_CHZ)
	{
		// �ҹ���
		if (m_bShift)	AddCh(0x61+TCHAR(id-IDC_KBDT_BTN_CHA));
		// �빮��
		else			AddCh(0x41+TCHAR(id-IDC_KBDT_BTN_CHA));
	}
	else
	{
		switch (id)
		{
		case IDC_KBDT_BTN_CLOSE		:	Cancel();			break;
		case IDC_KBDT_BTN_ENTER		:	Apply();			break;
		case IDC_KBDT_BTN_BACK		:	DelCh();			break;
		case IDC_KBDT_BTN_CLEAR		:	Clear();			break;
		case IDC_KBDT_BTN_UBAR		:	AddCh(L'_');		break;
		case IDC_KBDT_BTN_MINUS		:	AddCh(L'-');		break;
		case IDC_KBDT_BTN_SEL		:	SelectAll();		break;
#if (CUSTOM_CODE_UTRONIX  != DELIVERY_PRODUCT_ID)
		case IDC_KBDT_BTN_SHIFT		:	UpdateUpDnChar();	break;
		case IDC_KBDT_BTN_SLASH		:	AddCh(L'/');		break;
		case IDC_KBDT_BTN_SPACE		:	AddCh(L' ');		break;
#endif
		}
	}
}
#endif
/*
 desc : Keyboard���� Enter Ű ���� ������ ���
 parm : None
 retn : None
*/
VOID CDlgKBDT::Apply()
{
	/* ���� �Էµ� ������ �������� */
	wmemset(m_tzText, 0x00, 1024);
	m_edt_ctl[0].GetWindowText(m_tzText, 1024);
	/* �Էµ� ���ڿ� ���̰� ������ Ȯ�� */
	if (wcslen(m_tzText) < 1)	return;

	if (m_pParentWnd)
	{
		m_pParentWnd->PostMessage(WM_IDD_KBDT, 1, (LPARAM)m_pParentWnd);
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
VOID CDlgKBDT::Cancel()
{
	if (m_pParentWnd)
	{
		m_pParentWnd->PostMessage(WM_IDD_KBDT, 0, (LPARAM)m_pParentWnd);
	}
	else
	{
		CMyDialog::OnCancel();
	}
}

/*
 desc : Ű������ ���� ��ư (��ȣ �� Dot ����) Ȱ��ȭ ����
 parm : None
 retn : None
*/
VOID CDlgKBDT::EnableEnterKey()
{
#if 0
	BOOL bEnable	= FALSE;
	TCHAR tzVal[512]= {NULL};
	DOUBLE dbVal	= 0;
	/* ������� �Էµ� �� �߿��� Min ������ �۰ų� Max ������ ū ���, �� �̻� �Է� ���ϵ��� ó�� */
	m_edt_ctl[0].GetWindowTextW(tzVal, 512);
	dbVal	= _wtof(tzVal);
	if (dbVal >= m_dbMin && dbVal <= m_dbMax)	bEnable	= TRUE;
	m_btn_ctl[16].EnableWindow(bEnable);
	if (!bEnable)	m_btn_ctl[0].SetFocus();
	else			m_btn_ctl[16].SetFocus();
#endif
}

/*
 desc : ����Ʈ ��Ʈ�� �ڽ��� �� �Է�
 parm : ch	- [in]  �Է� (�߰�)�� ���� ��
 retn : None
*/
VOID CDlgKBDT::AddCh(TCHAR ch)
{
	TCHAR tzText[2]	= {ch, NULL}, tzData[1024] = {NULL};
	INT32 i32Len	= m_edt_ctl[0].GetWindowTextLength();

	// ����� �� �ؽ�Ʈ�� �Էµ� �������� Ȯ��
	if (i32Len > (INT32)m_u32MaxStr || i32Len > 1023)	return;
	// ������� �Էµ� ���ڿ� ���
	m_edt_ctl[0].GetWindowText(tzData, 1023);
#if 0
	/* ���ӵ� Dot (.) �Է� �����ϱ� ���� */
	if (ch == L'.' && tzData[i32Len-1] == ch)	return;
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
#else
	tzData[i32Len]	= ch;
	m_edt_ctl[0].SetWindowTextW(tzData);
#endif
	/* Enable Enter Key */
	EnableEnterKey();
}

/*
 desc : ����Ʈ ��Ʈ�� �ڽ��� �� �Է�
 parm : None
 retn : None
*/
VOID CDlgKBDT::DelCh()
{
	TCHAR tzText[1024]	= {NULL};
	INT32 i32Len		= m_edt_ctl[0].GetWindowTextLength();

	// ���� �Էµ� ���� ����
	m_edt_ctl[0].GetWindowText(tzText, 1023);
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
VOID CDlgKBDT::Clear()
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
VOID CDlgKBDT::SetSign(UINT8 ch)
{
	TCHAR tzText[1024]	= {NULL};
	INT32 i, i32Len		= m_edt_ctl[0].GetWindowTextLength();

	// ���� �Էµ� ���� ����
	m_edt_ctl[0].GetWindowText(tzText, 1023);
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
 desc : ���ڿ� �� ��ȯ
 parm : value	- [out] ��ȯ�Ǿ� ����� ���� ����
		size	- [in]  'value' ���� ũ��
 retn : None
*/
VOID CDlgKBDT::GetText(TCHAR *text, UINT32 size)
{
	if (m_pParentWnd)
	{
		m_edt_ctl[0].GetWindowText(text, size);
	}
	else
	{
		wmemcpy_s(text, size, m_tzText, wcslen(m_tzText));
	}
}

/*
 desc : Ű���� �� �� Dot '.' ��ư�� Enable or Disabl ó��
 parm : flag	- [in]  Enable or Disable
 retn : None
*/
VOID CDlgKBDT::DotEnable(BOOL enable)
{
	m_btn_ctl[15].EnableWindow(enable);
}

/*
 desc : Ű���� �� �� '+/-' ��ư�� Enable or Disabl ó��
 parm : flag	- [in]  Enable or Disable
 retn : None
*/
VOID CDlgKBDT::SignEnable(BOOL enable)
{
	m_btn_ctl[14].EnableWindow(enable);
}

/*
 desc : Edit â�� ������ ��� ���� �Ѵ�
 parm : None
 retn : None
*/
VOID CDlgKBDT::SelectAll()
{
	// ��Ŀ�� ����
	m_edt_ctl[0].SetFocus();
	// �Էµ� ���ڿ��� ��ü ����
	m_edt_ctl[0].SetSel(0, -1, TRUE);
}

/*
 desc : �빮�� or �ҹ��� ó��
 parm : None
 retn : None
*/
VOID CDlgKBDT::UpdateUpDnChar()
{
	TCHAR tzNum	= 0x00, tzAlph[2]	= {0x00, 0x00};
	UINT32 i, u32ID	= IDC_KBDT_BTN_CHA;

	// ��ҹ���
	m_bShift	= !m_bShift;
	// �ҹ���ȭ
	if (m_bShift)	tzNum = 0x61;
	// �빮��ȭ
	else			tzNum = 0x41;
	// ��ư �ؽ�Ʈ ����
	for (i=0; i<26; i++)
	{
		tzAlph[0]	= tzNum + TCHAR(i);
		m_btn_ctl[10+i].SetWindowText(tzAlph);
	}
}