
/*
 desc : Callback Event
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgCbks.h"


IMPLEMENT_DYNAMIC(CDlgCbks, CDialogEx)

/*
 desc : ������
 parm : None
 retn : None
*/
CDlgCbks::CDlgCbks(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCbks::IDD, pParent)
{
	m_bExitProc	= FALSE;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgCbks::~CDlgCbks()
{
}

/* ��Ʈ�� ���� */
void CDlgCbks::DoDataExchange(CDataExchange* dx)
{
	CDialogEx::DoDataExchange(dx);

}

/* �޽��� �� */
BEGIN_MESSAGE_MAP(CDlgCbks, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/*
 desc : ���̾�α� ���� �� �ʱ�ȭ
*/
BOOL CDlgCbks::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	/* ��Ʈ�� �ʱ�ȭ */
	InitCtrl();

	return TRUE;
}

/*
 desc : ���̾�αװ� ����� �� ������ �ѹ� ȣ��
 parm : None
 retn : None
*/
VOID CDlgCbks::OnClose()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnClose();
}
VOID CDlgCbks::OnDestroy()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnDestroy();
}
VOID CDlgCbks::ExitProc()
{
	/* �̹� ������ ȣ������� ���� */
	m_bExitProc	= TRUE;

}

/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgCbks::InitCtrl()
{
}

/*
 desc : �ֱ������� ȣ��Ǵ� �Լ�
 parm : None
 retn : None
*/
VOID CDlgCbks::UpdateCtrl()
{
}

/*
 desc : �̺�Ʈ ������ ���ŵ� ��� (�θ� ���ؼ� ȣ���)
 parm : event	- [in]  �̺�Ʈ ������ ����� ����ü ������
 retn : None
*/
VOID CDlgCbks::UpdateCtrl(STG_CCED *event)
{
}

