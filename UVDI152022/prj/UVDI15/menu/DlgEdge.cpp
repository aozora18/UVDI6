
/*
 desc : Edge Detection Results ���
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgEdge.h"
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
CDlgEdge::CDlgEdge(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgEdge::IDD, parent)
{
	m_bDrawBG		= TRUE;
	m_bTooltip		= TRUE;
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgEdge::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* groupbox - normal */
	u32StartID	= IDC_EDGE_GRP_MODEL_SIZE;
	for (i=0; i< eEDGE_GRP_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* static - normal */
	u32StartID	= IDC_EDGE_TXT_SIZE_1;
	for (i=0; i< eEDGE_TXT_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Button - Normal */
	u32StartID	= IDC_EDGE_BTN_CANCEL;
	for (i=0; i< eEDGE_BTN_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Static - normal */
	u32StartID	= IDC_EDGE_PIC_MARK;
	for (i=0; i< eEDGE_PIC_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_pic_ctl[i]);
	/* edit - normal */
	u32StartID	= IDC_EDGE_EDT_SIZE_1;
	for (i=0; i< eEDGE_EDT_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
	/* Box - normal */
	u32StartID	= IDC_EDGE_BOX_RESULTS;
	for (i=0; i< eEDGE_BOX_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_box_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgEdge, CMyDialog)
	ON_LBN_SELCHANGE(IDC_EDGE_BOX_RESULTS, OnBoxSelChangeMark)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_EDGE_BTN_CANCEL, IDC_EDGE_BTN_CANCEL, OnBtnClicked)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgEdge::PreTranslateMessage(MSG* msg)
{

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgEdge::OnInitDlg()
{
	/* ��Ʈ�� �ʱ�ȭ */
	if (!InitCtrl())	return FALSE;
	/* �˻��� ������ ���� ���, Apply ��ư ��Ȱ��ȭ */
	if (uvEng_Camera_GetEdgeDetectCount(m_u8ACamID))
	{
		/* Load Result Data */
		LoadData();
	}

	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgEdge::OnExitDlg()
{
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgEdge::OnPaintDlg(CDC *dc)
{
	DrawPattern(dc);
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgEdge::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgEdge::InitCtrl()
{
	// by sysandj : Resize UI
	CResizeUI clsResizeUI;
	// by sysandj : Resize UI

	INT32 i = 0;
	PTCHAR pText	= NULL;

	/* group box */
	for (i = 0; i < eEDGE_GRP_MAX; i++)
	{
		m_grp_ctl[i].SetLogFont(&g_lf[eFONT_LEVEL2_BOLD]);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_grp_ctl[i]);
		// by sysandj : Resize UI
	}

	/* box - edge detection results */
	for (i = 0; i < eEDGE_BOX_MAX; i++)
	{
		m_box_ctl[i].SetLogFont(&g_lf[eFONT_LEVEL2_BOLD]);;

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_box_ctl[i]);
		// by sysandj : Resize UI
	}
	/* button - normal */
	for (i=0; i< eEDGE_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		//m_btn_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}
	/* static - Normal */
	for (i=0; i< eEDGE_TXT_MAX; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_ctl[i]);
		// by sysandj : Resize UI
	}
	/* edit - text */
	for (i=0; i< eEDGE_EDT_MAX; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_ctl[i].SetReadOnly(TRUE);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_edt_ctl[i]);
		// by sysandj : Resize UI
	}

	return TRUE;
}

/*
 desc : ���� �߻��� ���� ���� ���
 parm : None
 retn : None
*/
VOID CDlgEdge::OnBtnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_EDGE_BTN_CANCEL:
		CMyDialog::OnCancel();
		break;
	}
}

/*
 desc : DoModal Override
 parm : cam_id	- [in]  Align Camera Index (1 or 2)
 retn : ?
*/
INT_PTR CDlgEdge::MyDoModal(UINT8 cam_id)
{
	m_u8ACamID	= cam_id;
	return DoModal();
}


/*
 desc : Edge Find Result Data ����
 parm : None
 retn : None
*/
VOID CDlgEdge::LoadData()
{
	TCHAR tzData[256]	= {NULL};
	INT32 i, i32Find	= uvEng_Camera_GetEdgeDetectCount(m_u8ACamID);
	LPG_EDFR pstData	= uvEng_Camera_GetEdgeDetectResults(m_u8ACamID);

	for (i=0; pstData && i<i32Find; i++)
	{
		swprintf_s(tzData, 256, L"DIA (%.1f um) / AREA (%4u, %4u, %4u, %4u) / SIZE (%.1f, %.1f um)",
					pstData[i].dia_meter_um,
					pstData[i].circle_area.left,
					pstData[i].circle_area.top,
					pstData[i].circle_area.right,
					pstData[i].circle_area.bottom,
					pstData[i].width_um,
					pstData[i].height_um);
		m_box_ctl[eEDGE_BOX_MAX].AddString(tzData);
	}
}

/*
 desc : Box ��Ʈ���� Ŭ���� ���
 parm : None
 retn : None
*/
VOID CDlgEdge::OnBoxSelChangeMark()
{
	TCHAR tzFile[MAX_PATH_LEN] = {NULL};
	INT32 i32CurSel	= m_box_ctl[eEDGE_BOX_MAX].GetCurSel();
	STG_EDFR stEdge	= {NULL};
	CRect rPic;
	CDlgMesg dlgMesg;

	/* ���� ���õ� ��ġ �� ���� */
	if (i32CurSel < 0)	return;

	/* �ӽ� ���� ���� ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\save_img\\edge\\uvdi15_temp_mark.bmp", g_tzWorkDir);
	/* �ӽ� ���� ���Ϸ� ���� ���õ� ��ũ ���� */
	if (!uvEng_Camera_SaveGrabbedMarkToFile(m_u8ACamID,
											&uvEng_Camera_GetEdgeDetectResults(m_u8ACamID)[i32CurSel].box_area,
											0x00,
											tzFile))
	{
		dlgMesg.MyDoModal(L"Failed to save temporary file", 0x01);
		return;
	}
	/* ���� ���õ� �̹����� �ִٸ� ���� */
	if (!m_csImgMark.IsNull())	m_csImgMark.Destroy();
	/* Bitmap Image ���� */
	if (S_OK != m_csImgMark.Load(tzFile))
	{
		dlgMesg.MyDoModal(L"", 0x01);
		return;
	}

	/* ���� ���õ� Model Type�� ���� �� �Է� �ٸ��� */
	m_edt_ctl[eEDGE_EDT_SIZE_1].SetTextToNum(uvEng_Camera_GetEdgeDetectResults(m_u8ACamID)[i32CurSel].dia_meter_um / 2.0f, 1);
	m_edt_ctl[eEDGE_EDT_SIZE_2].SetTextToNum(uvEng_Camera_GetEdgeDetectResults(m_u8ACamID)[i32CurSel].width_um, 1);
	m_edt_ctl[eEDGE_EDT_SIZE_3].SetTextToNum(uvEng_Camera_GetEdgeDetectResults(m_u8ACamID)[i32CurSel].height_um, 1);
	/* ���� ���� ���� */
	InvalidateMark();
}

/*
 desc : ��ũ ���� ����
 parm : None
 retn : None
*/
VOID CDlgEdge::InvalidateMark()
{
	CRect rPic;

	/* ���� Mark ���� ��� */
	m_pic_ctl[eEDGE_PIC_MARK].GetWindowRect(rPic);
	ScreenToClient(rPic);
	/* Mark ������ ���� */
	InvalidateRect(rPic);
}

/*
 desc : ������ ������ ���� ���
 parm : dc	- [in]  Device Context for dialog windows
 retn : None
*/
VOID CDlgEdge::DrawPattern(CDC *dc)
{
	INT32 u32MoveX, u32MoveY;
	UINT32 u32DstW, u32DstH;
	DOUBLE dbRate	= 0.0f;
	CRect rDraw;

	/* ���õ� Mark�� ���ų� ����� Mark �̹����� ������ Ȯ�� */
	if (m_box_ctl[eEDGE_BOX_MAX].GetCurSel() < 0 || m_csImgMark.IsNull())
	{
		InvalidateMark();	/* ���� Mark ���� ���� */
		return;
	}

	/* Drawing Area ���ϱ� */
	m_pic_ctl[eEDGE_PIC_MARK].GetWindowRect(rDraw);
	ScreenToClient(rDraw);

	/* �̹��� ũ��� ��µ� ���� ũ�� �� */
	rDraw.DeflateRect(1,1);

	/* ���� ũ�� ������ ���� ũ�� �������� ū ��� */
	if ((DOUBLE(rDraw.Width()) / DOUBLE(m_csImgMark.GetWidth())) >
		(DOUBLE(rDraw.Height()) / DOUBLE(m_csImgMark.GetHeight())))
	{
		/* <����> ���� �������� ���� Ȯ�� or ��� ���� */
		dbRate	= DOUBLE(rDraw.Height()) / DOUBLE(m_csImgMark.GetHeight());
	}
	/* ���� ũ�� ������ ���� ũ�� �������� ū ��� */
	else
	{
		/* <����> ���� �������� ���� Ȯ�� or ��� ���� */
		dbRate	= DOUBLE(rDraw.Width()) / DOUBLE(m_csImgMark.GetWidth());
	}
	/* ���� �̹��� ������ �°� ���� �̹��� ũ�� ���� */
	u32DstW	= (UINT32)ROUNDED(dbRate * m_csImgMark.GetWidth(), 0);
	u32DstH	= (UINT32)ROUNDED(dbRate * m_csImgMark.GetHeight(),0);

	/* �̹��� ��� ���� ��ġ ��� */
	u32MoveX	= (UINT32)ROUNDED(DOUBLE((rDraw.Width()) - u32DstW) / 2.0f + 1.0f, 0);
	u32MoveY	= (UINT32)ROUNDED(DOUBLE((rDraw.Height()) - u32DstH) / 2.0f + 1.0f, 0);

	/* �̹��� ��� */
	m_csImgMark.StretchBlt(dc->m_hDC,
						   rDraw.left + u32MoveX, rDraw.top + u32MoveY, u32DstW-2, u32DstH-2,
						   0, 0, m_csImgMark.GetWidth(), m_csImgMark.GetHeight());
}