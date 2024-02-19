
/*
 desc : The fiducial information for global and local dcode
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgLuJF.h"


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
CDlgLuJF::CDlgLuJF(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgLuJF::IDD, parent)
{
	m_bDrawBG		= TRUE;
	m_bTooltip		= TRUE;
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgLuJF::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Groupbox */
	u32StartID	= IDC_LUJF_GRP_GLOBAL;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Static */
	u32StartID	= IDC_LUJF_TXT_GLOBAL_DCODE;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Edit box */
	u32StartID	= IDC_LUJF_EDT_GLOBAL_DCODE;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
	/* List box */
	u32StartID	= IDC_LUJF_BOX_GLOBAL;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_box_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgLuJF, CMyDialog)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgLuJF::PreTranslateMessage(MSG* msg)
{

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLuJF::OnInitDlg()
{
	/* ��Ʈ�� �ʱ�ȭ */
	InitCtrl();
	LoadValue();

	/* TOP_MOST & Center */
	//CenterParentTopMost();

	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgLuJF::OnExitDlg()
{
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgLuJF::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgLuJF::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgLuJF::InitCtrl()
{
	INT32 i;

	/* Groupbox */
	for (i=0; i<2; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* text - normal */
	for (i=0; i<2; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* edit box */
	for (i=0; i<2; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_ctl[i].SetInputType(EN_DITM::en_hex10);
		m_edt_ctl[i].SetReadOnly(TRUE);
		m_edt_ctl[i].SetMouseClick(FALSE);
	}
	/* List box - normal */
	for (i=0; i<2; i++)
	{
		m_box_ctl[i].SetLogFont(&g_lf);
	}
}

/*
 desc : Fiducial ���� ����
 parm : None
 retn : None
*/
VOID CDlgLuJF::LoadValue()
{
	UINT16 i			= 0;
	TCHAR tzNum[2][16]	= {NULL}, tzVal[2][16] = {NULL}, tzStr[64] = {NULL};
	LPG_JMFD pstFD		= &uvEng_ShMem_GetLuria()->jobmgt.selected_job_fiducial;

	/* Global DCode */
	m_edt_ctl[0].SetTextToNum(pstFD->g_d_code, TRUE);
	for (i=0; i<pstFD->g_coord_count; i++)
	{
		/* mm ������ ��ȯ �� ���ڿ��� ���� */
		swprintf_s(tzNum[0], 16, L"X : %9.6f", pstFD->g_coord_xy[i].x / 1000000.0f);
		swprintf_s(tzNum[1], 16, L"Y : %9.6f", pstFD->g_coord_xy[i].y / 1000000.0f);
		/* ������ ũ��� ��ȯ */
		wmemcpy_s(tzVal[0], 16, tzNum[0], 12);
		wmemcpy_s(tzVal[1], 16, tzNum[1], 12);
		/* �ϳ��� ����� ���� */
		swprintf_s(tzStr, 64, L"%s   |   %s", tzVal[0], tzVal[1]);
		m_box_ctl[0].AddString(tzStr);
	}
	/* Local DCode */
	m_edt_ctl[1].SetTextToNum(pstFD->l_d_code, TRUE);
	for (i=0; i<pstFD->l_coord_count; i++)
	{
		/* mm ������ ��ȯ �� ���ڿ��� ���� */
		swprintf_s(tzNum[0], 16, L"X : %9.6f", pstFD->l_coord_xy[i].x / 1000000.0f);
		swprintf_s(tzNum[0], 16, L"Y : %9.6f", pstFD->l_coord_xy[i].y / 1000000.0f);
		/* ������ ũ��� ��ȯ */
		wmemcpy_s(tzVal[0], 16, tzNum[0], 12);
		wmemcpy_s(tzVal[1], 16, tzNum[1], 12);
		/* �ϳ��� ����� ���� */
		swprintf_s(tzStr, 64, L"%s  |  %s", tzVal[0], tzVal[1]);
		m_box_ctl[1].AddString(tzStr);
	}
}
