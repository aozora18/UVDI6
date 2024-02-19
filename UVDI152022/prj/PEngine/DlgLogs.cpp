
/*
 desc : Monitoring - Log Message
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgLogs.h"


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
CDlgLogs::CDlgLogs(UINT32 id, CWnd *parent)
	: CDlgChild(id, parent)
{
	m_bUpdateLogs	= TRUE;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgLogs::~CDlgLogs()
{
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgLogs::DoDataExchange(CDataExchange* dx)
{
	CDlgChild::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Button - normal */
	u32StartID	= IDC_LOGS_BTN_CLEAR;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Checkbox */
	u32StartID	= IDC_LOGS_CHK_PAUSE;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
	/* Edit - number */
	u32StartID	= IDC_LOGS_RCH_MESG;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_rch_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgLogs, CDlgChild)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_LOGS_BTN_CLEAR, IDC_LOGS_BTN_COPY, OnBtnClicked)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_LOGS_CHK_PAUSE, IDC_LOGS_CHK_PAUSE, OnChkClicked)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgLogs::PreTranslateMessage(MSG* msg)
{

	return CDlgChild::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLogs::OnInitDlg()
{
	if (!CDlgChild::OnInitDlg())	return FALSE;

	/* ��Ʈ�� �ʱ�ȭ */
	if (!InitCtrl())	return FALSE;

	/* ���� ��ü ȣ�� */
	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgLogs::OnExitDlg()
{
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgLogs::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgLogs::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLogs::InitCtrl()
{
	INT32 i;

	/* button - normal */
	for (i=0; i<2; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	/* checkbox - normal */
	for (i=0; i<1; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf);
		m_chk_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
 	/* rich edit */
 	for (i=0; i<1; i++)
 	{
		m_rch_ctl[i].SetRichFont(g_lf.lfFaceName, g_lf.lfWeight);
		m_rch_ctl[i].SetKeyInputEnable(FALSE);
 	}

	/* ��� ������ ���� �� ���� */
#ifdef _DEBUG
	m_rch_ctl[0].SetMaxLines(1024);
#else
	m_rch_ctl[0].SetMaxLines(4096);
#endif

	return TRUE;
}

/*
 desc : ���� ���� ��Ʈ�� ���� ����
 parm : None
 retn : None
*/
VOID CDlgLogs::UpdateCtrl()
{
	/* Update Log Data */
	UpdateLogs();
}

/*
 desc : PLC Memory Data (Value) ����
 parm : None
 retn : None
*/
VOID CDlgLogs::UpdateLogs()
{
	TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};
	UINT8 u8LogLevel	= 0x00;
	PTCHAR ptzMesg		= NULL;
	COLORREF clrText[4]	= { RGB(0, 0, 0), RGB(0, 153, 255), RGB(255, 0, 0),  RGB(255, 0, 0) };
	UINT32 u32TickCnt	= (UINT32)GetTickCount();

	/* �α� ���� ���� Ȯ�� */
	if (!m_bUpdateLogs)	return;

	/* ȭ�� ���� ���� */
	m_rch_ctl[0].SetRedraw(FALSE);

	/* �α� �޽����� ���� ������ �о ��� */
	do {
	
		/* ���� ��ϵǾ� �ִ� ��, ���� ���� �α� �޽����� ���� ������ �б� */
		ptzMesg	= uvCmn_Logs_GetLastMesg(u8LogLevel);
		if (!ptzMesg)	break;
		/* �ݵ�� ���� ���ۿ� �����ϰ� �� ���Ŀ� ����Ʈ �ڽ��� ���� (���� ���� �޸��� ���ڿ� �� �κ� �ν��ϴµ� ����) */
		wcscpy_s(tzMesg, LOG_MESG_SIZE, ptzMesg);
		m_rch_ctl[0].Append(tzMesg, clrText[u8LogLevel]);

		/* �ּ��� �б� ������ �ð��� 100 msec �̻� ������ �ȵ� */
		if (u32TickCnt + 100 < (UINT32)GetTickCount())	break;

	} while (ptzMesg);

	/* ȭ�� ���� Ȱ��ȭ */
	m_rch_ctl[0].SetRedraw(TRUE);
	m_rch_ctl[0].Invalidate(FALSE);
}

/*
 desc : ��ư �̺�Ʈ
 parm : id	- [in]  ��Ʈ�� ID
 retn : None
*/
VOID CDlgLogs::OnBtnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_LOGS_BTN_CLEAR	:	m_rch_ctl[0].SetClearAll(TRUE);	break;
	case IDC_LOGS_BTN_COPY	:	m_rch_ctl[0].SetCopyAll(TRUE);	break;
	}
}

/*
 desc : ��ư �̺�Ʈ
 parm : id	- [in]  ��Ʈ�� ID
 retn : None
*/
VOID CDlgLogs::OnChkClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_LOGS_CHK_PAUSE	:	m_bUpdateLogs	= m_chk_ctl[0].GetCheck() > 0 ? FALSE : TRUE;	break;
	}
}