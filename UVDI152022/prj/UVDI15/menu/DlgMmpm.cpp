
/*
 desc : Algin Mark Calibration
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgMmpm.h"
#include "../mesg/DlgMesg.h"
#include "DlgMMPMAutoCenter.h"

//#include "DlgEdge.h"
//#include "DlgSetMark.h"
//
//#include "./mark/DrawModel.h"		/* Mark Model Object */
//#include "./mark/GridMark.h"		/* Grid Control */
//#include "./mark/GridModel.h"		/* Grid Control */
//#include "./mark/GridModelInMark.h"	/* Grid Control */
//#include "./mark/GridMiniMotor.h"




#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : id		- [in]  �ڽ��� ���� ID
		parent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
 retn : None
*/
CDlgMmpm::CDlgMmpm(bool showAll,CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgMmpm::IDD, parent)
{
	m_bDrawBG = TRUE;
	m_bTooltip = TRUE;
	m_hDCDraw = NULL;
	m_u8Index = 0x00;
	m_u8LeftClick = 0x00;
	m_dbStep = 1.0f;
	m_pstGrab = NULL;
	m_hPrvCursor = NULL;
	this->showAll = showAll;
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgMmpm::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Group - Normal */
	u32StartID = IDC_MMPM_GRP_STEP;
	for (i = 0; i < eMMPM_GRP_MAX; i++)		DDX_Control(dx, u32StartID + i, m_grp_ctl[i]);
	/* Static - Normal */
	u32StartID = IDC_MMPM_PIC_VIEW;
	for (i = 0; i < eMMPM_PIC_MAX; i++)		DDX_Control(dx, u32StartID + i, m_pic_ctl[i]);
	/* Button - Normal */
	u32StartID = IDC_MMPM_BTN_HORZ_LEFT;
	for (i = 0; i < eMMPM_BTN_MAX; i++)	DDX_Control(dx, u32StartID + i, m_btn_ctl[i]);
	/* Step Size */
	u32StartID = IDC_MMPM_CHK_STEP_01;
	for (i = 0; i < eMMPM_CHK_MAX; i++)		DDX_Control(dx, u32StartID + i, m_chk_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgMmpm, CMyDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_MMPM_BTN_HORZ_LEFT, IDC_MMPM_BTN_CANCEL, OnBtnClicked)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_MMPM_CHK_STEP_01, IDC_MMPM_CHK_STEP_10, OnChkClicked)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgMmpm::PreTranslateMessage(MSG* msg)
{
	switch (msg->wParam)
	{
	case VK_LEFT:
	case VK_RIGHT:
	case VK_UP:
	case VK_DOWN:	KeyUpDown(UINT16(msg->wParam));	break;
	}

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMmpm::OnInitDlg()
{
	UINT8 i = 0x00;
	menuPart = 99;

	for (int i = 0; i < 3; i++) {
		m_pt3Point_Img[i].x = 9999;
		m_pt3Point_Img[i].y = 9999;
	}


	CResizeUI clsResizeUI;
	clsResizeUI.ResizeControl(this, this);

	/* ��Ʈ�� �ʱ�ȭ */
	if (!InitCtrl())	return FALSE;
	/* Align Mark ��� ���� DC �ڵ� ��� */
	m_hDCDraw = ::GetDC(m_pic_ctl[eMMPM_PIC_VIEW].GetSafeHwnd());
	/* Align Mark ��� ���� ���� ��� */
	m_pic_ctl[eMMPM_PIC_VIEW].GetClientRect(&m_rAreaDraw);
	/* ���� ���� �缳�� */
	m_rAreaDraw.left++;
	m_rAreaDraw.top++;
	m_rAreaDraw.right--;
	m_rAreaDraw.bottom--;

	/* TOP_MOST & Center */
	CenterParentTopMost();

	InitDispMMPM();
	/* Grab.Failed ���� �� �޸� �����ϱ� */
#if 0
	0x00 : ������ ��ũ�� ��������, 0x01 : ��� ��ũ ���� ��������

	if (!GetMarkData(0x00))	return FALSE;
#else
	if (!GetMarkData(showAll ? 0x01 : 0x00))	return FALSE;
#endif

	//InitDispMark();
	//DispResize(GetDlgItem(IDC_MMPM_PIC_VIEW));

	/* ���� ���� ù��° ������ ���� ��� */
	FindData(0x00);

	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgMmpm::OnExitDlg()
{
	POSITION pPos = NULL;
	LPG_ACGR pstGrab = NULL;

	/* CDC �ڵ� �޸� ���� */
	if (m_hDCDraw)	::ReleaseDC(m_pic_ctl[eMMPM_PIC_VIEW].GetSafeHwnd(), m_hDCDraw);

	/* �޸� ���� */
	pPos = m_lstGrab.GetHeadPosition();
	while (pPos)
	{
		pstGrab = m_lstGrab.GetNext(pPos);
		if (pstGrab)	delete pstGrab;
	}
	m_lstGrab.RemoveAll();
	pPos = m_lstFind.GetHeadPosition();
	while (pPos)
	{
		pstGrab = m_lstFind.GetNext(pPos);
		if (pstGrab)	delete pstGrab;
	}
	m_lstFind.RemoveAll();
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgMmpm::OnPaintDlg(CDC* dc)
{
	/* Mark Pattern ��� */
	if (m_hDCDraw && m_pstGrab)
	{
		// lk91 �̹��� �׽�Ʈ �����, u8FindMark	= IsValidGrabMarkNum(cam_id, img_id) ? 0x01 : 0x00; �κ� ������ؼ� �Ѿ����
		uvEng_Camera_DrawMarkBitmap(m_hDCDraw, m_rAreaDraw, m_pstGrab->cam_id, m_pstGrab->img_id);
	}
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgMmpm::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMmpm::InitCtrl()
{
	// by sysandj : Resize UI
	CResizeUI clsResizeUI;
	// by sysandj : Resize UI

	INT32 i = 0;
	UINT32 u32IconMove[eMMPM_BTN_CHECK][2] = { { IDI_ARR_LEFT_EN, IDI_ARR_LEFT_DIS },
								{ IDI_ARR_RIGHT_EN, IDI_ARR_RIGHT_DIS },
								{ IDI_ARR_UP_EN, IDI_ARR_UP_DIS },
								{ IDI_ARR_DOWN_EN, IDI_ARR_DOWN_DIS },
								{ IDI_UNDO_EN, IDI_UNDO_DIS },
								{ IDI_ARR_LEFT_EN, IDI_ARR_LEFT_DIS },
								{ IDI_ARR_RIGHT_EN, IDI_ARR_RIGHT_DIS } };
	PTCHAR pText = NULL;

	/* group box */
	for (i = 0; i < eMMPM_GRP_MAX; i++)
	{
		m_grp_ctl[i].SetLogFont(&g_lf[eFONT_LEVEL2_BOLD]);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_grp_ctl[i]);
		// by sysandj : Resize UI
	}
	/* button - normal */
	for (i = 0; i < eMMPM_BTN_CHECK; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);
		m_btn_ctl[i].SetIconID(u32IconMove[i][0], u32IconMove[i][1]);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}
	for (i = eMMPM_BTN_CHECK; i < eMMPM_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}
	/* check box - normal */
	for (i = 0; i < eMMPM_CHK_MAX; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_chk_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_chk_ctl[i]);
		// by sysandj : Resize UI
	}
	m_chk_ctl[eMMPM_CHK_STEP_01].SetCheck(1);

	for (i = 0; i < eMMPM_PIC_MAX; i++)
	{
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_pic_ctl[i]);
		// by sysandj : Resize UI
	}


	return TRUE;
}

/*
 desc : ��ư �̺�Ʈ ó��
 parm : None
 retn : None
*/
const int RIGHT = 0x12, LEFT = 0x11, UP = 0x21, DOWN = 0x22;
VOID CDlgMmpm::OnBtnClicked(UINT32 id)
{
	switch (id)
	{
		case IDC_MMPM_BTN_UNDO:	UndoCenter();		break;
		case IDC_MMPM_BTN_HORZ_LEFT:	MoveCenter(LEFT); break;
		case IDC_MMPM_BTN_HORZ_RIGHT:	MoveCenter(RIGHT); break;
		case IDC_MMPM_BTN_VERT_UP:	MoveCenter(UP);	break;
		case IDC_MMPM_BTN_VERT_DOWN:	MoveCenter(DOWN);	break;
		case IDC_MMPM_BTN_CHECK:	MoveCenter(0x00);	break;
		case IDC_MMPM_BTN_PREV:	FindData(0x01);		break;
		case IDC_MMPM_BTN_NEXT:	FindData(0x02);		break;
		case IDC_MMPM_BTN_APPLY:	WorkApply();		break;
		case IDC_MMPM_BTN_CANCEL:	WorkCancel();		break;
		//case IDC_MMPM_BTN_AUTO_CENTER:	AutoCenter();		break;
	}
}

/*
 desc : üũ �ڽ� �̺�Ʈ ó��
 parm : None
 retn : None
*/
VOID CDlgMmpm::OnChkClicked(UINT32 id)
{
	UINT8 i = 0x00;

	for (i = 0; i < eMMPM_CHK_MAX; i++)	m_chk_ctl[i].SetCheck(0);
	switch (id)
	{
	case IDC_MMPM_CHK_STEP_01:	m_chk_ctl[eMMPM_CHK_STEP_01].SetCheck(1);	m_dbStep = 0.1f;	break;
	case IDC_MMPM_CHK_STEP_02:	m_chk_ctl[eMMPM_CHK_STEP_02].SetCheck(1);	m_dbStep = 0.5f;	break;
	case IDC_MMPM_CHK_STEP_05:	m_chk_ctl[eMMPM_CHK_STEP_05].SetCheck(1);	m_dbStep = 5.0f;	break;
	case IDC_MMPM_CHK_STEP_10:	m_chk_ctl[eMMPM_CHK_STEP_10].SetCheck(1);	m_dbStep = 10.0f;	break;
	}
}

/*
 desc : ���� / ���� Guide (Mark) �ν� ������ �� ã��
 parm : direct	- [in]  0x00 : first, 0x01 : Prev, 0x02 : Next
 retn : None
*/
VOID CDlgMmpm::FindData(UINT8 direct)
{
	CRect rPic;
	LPG_ACGR pstGrab = NULL;

	switch (direct)
	{
	case 0x00:	m_u8Index = 0x00;										break;
	case 0x01: if (m_u8Index != 0)	m_u8Index--;						break;
	case 0x02: if (m_u8Index + 1 != m_lstGrab.GetCount())	m_u8Index++;	break;
	}
	if (m_u8Index == 0x00)
	{
		if (m_lstGrab.GetCount() == 1)
		{
			m_btn_ctl[eMMPM_BTN_PREV].EnableWindow(FALSE);
			m_btn_ctl[eMMPM_BTN_NEXT].EnableWindow(FALSE);
		}
		else
		{
			m_btn_ctl[eMMPM_BTN_PREV].EnableWindow(FALSE);
			m_btn_ctl[eMMPM_BTN_NEXT].EnableWindow(TRUE);
		}
	}
	else if (m_u8Index + 1 == m_lstGrab.GetCount())
	{
		m_btn_ctl[eMMPM_BTN_PREV].EnableWindow(TRUE);
		m_btn_ctl[eMMPM_BTN_NEXT].EnableWindow(FALSE);
	}
	else
	{
		m_btn_ctl[eMMPM_BTN_PREV].EnableWindow(TRUE);
		m_btn_ctl[eMMPM_BTN_NEXT].EnableWindow(TRUE);
	}

	/* ���� ������ �߻��� ù ��° �׸� ����  */
	if (m_lstGrab.GetCount())
	{
		pstGrab = m_lstGrab.GetAt(m_lstGrab.FindIndex(m_u8Index));
		//m_pstGrab = pstGrab; // lk91 image test �� �� �Ʒ� ���� �ּ� ��, �ּ� Ǯ��
		if (!pstGrab)	m_pstGrab = NULL;
		else
			m_pstGrab = pstGrab;//uvEng_Camera_GetGrabbedMark(pstGrab->cam_id, pstGrab->img_id);
	}

	/* �⺻ �� ���� */
	if (m_pstGrab && m_pstGrab->mark_cent_px_x < 1.0f)
	{
		m_pstGrab->mark_cent_px_x = m_pstGrab->grab_width / 2.0f;
		m_pstGrab->mark_cent_px_y = m_pstGrab->grab_height / 2.0f;
		m_pstGrab->mark_width_px = (UINT32)ROUNDUP(uvEng_Camera_GetMarkModelSize(m_pstGrab->cam_id, 0x00 /* fixed */, 0x00, 0x01), 0);
		m_pstGrab->mark_height_px = (UINT32)ROUNDUP(uvEng_Camera_GetMarkModelSize(m_pstGrab->cam_id, 0x00 /* fixed */, 0x01, 0x01), 0);
		m_pstGrab->mark_width_mm = m_pstGrab->mark_width_px * (uvEng_GetConfig()->acam_spec.spec_pixel_um[0 /* fixed */] / 1000.0f);
		m_pstGrab->mark_height_mm = m_pstGrab->mark_height_px * (uvEng_GetConfig()->acam_spec.spec_pixel_um[0 /* fixed */] / 1000.0f);
	}
	
	m_ptCenter.x = m_pstGrab->mark_cent_px_x;
	m_ptCenter.y = m_pstGrab->mark_cent_px_y;

	if (m_pstGrab->marked == 0)
	{
		m_pstGrab->move_px_x = m_ptCenter.x;
		m_pstGrab->move_px_y = m_ptCenter.y;
	}

	/* �̹��� ��� ���� ��� */
	m_pic_ctl[eMMPM_PIC_VIEW].GetWindowRect(rPic);
	ScreenToClient(rPic);
	InvalidateRect(rPic);
}

/*
 desc : �˻� ������ Guide (Mark) ������ ���
 parm : type	- [in]  0x00 : ������ ��ũ�� ��������, 0x01 : ��� ��ũ ���� ��������
 retn : TRUE or FALSE
*/
BOOL CDlgMmpm::GetMarkData(UINT8 type)
{
	UINT8 i = 0x00;
	LPG_ACGR pstGrab = NULL;
	LPG_ACGR pstTemp = NULL;

	/* ������ ���� (�˻�)�� Mark ��� ���� �ӽ÷� �����ϱ� ���� */
	for (; i < uvEng_Camera_GetGrabbedCount(); i++)
	{
		/* �˻��� ������ ��� */
		pstTemp = uvEng_Camera_GetGrabbedMarkIndex(i);
		/* �˻��� �����Ͱ� Guide (Mark) �ν� ������ ��츸 �޸𸮿� �ӽ� ���� */
		if (!pstTemp)	continue;
		/* �� ������ ��ũ ������ �����´ٸ� ..., ������ ��ũ�� Skip */
		if (type == 0x00 && 0x01 == pstTemp->marked)	continue;
		/* �˻��� ���ο� ��ũ ���� ���� */
		pstGrab = new STG_ACGR;
		ASSERT(pstGrab);
		memcpy(pstGrab, pstTemp, sizeof(STG_ACGR) - sizeof(PUINT8));
		pstGrab->grab_data = pstTemp->grab_data;
		m_lstGrab.AddTail(pstGrab);
	}

	// lk91 �̹��� test �� (�� �ּ��ϰ� �Ʒ� �ڵ� ����ϱ�)
	//for (; i < 1; i++)
	//{
	//	/* �˻��� ������ ��� */
	//	//pstTemp = uvEng_Camera_GetGrabbedMarkIndex(i);
	//	///* �˻��� �����Ͱ� Guide (Mark) �ν� ������ ��츸 �޸𸮿� �ӽ� ���� */
	//	//if (!pstTemp)	continue;
	//	///* �� ������ ��ũ ������ �����´ٸ� ..., ������ ��ũ�� Skip */
	//	//if (type == 0x00 && 0x01 == pstTemp->marked)	continue;
	//	///* �˻��� ���ο� ��ũ ���� ���� */
	//	pstGrab = new STG_ACGR;
	//	//ASSERT(pstGrab);
	//	//pstTemp = uvEng_Camera_RunModelCali(0x01, 0xff, (UINT8)0x05, 2, TRUE, uvEng_GetConfig()->mark_find.image_process);
	//	int tmpCamNo = 1;
	//	pstTemp = uvEng_Camera_RunModelCali(tmpCamNo, 0xff, DISP_TYPE_MMPM, 2, TRUE, uvEng_GetConfig()->mark_find.image_process);
	//	// Mark �� ��� Mark Find�� �Ѿ�� �ش� �̹����� �Ѱ��ִ� ����
	//	//uvEng_Camera_RunModelCali(UINT8 cam_id, UINT8 mode, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp, UINT8 img_proc) // default : UINT8 mode=0xff
	//	memcpy(pstGrab, pstTemp, sizeof(STG_ACGR) - sizeof(PUINT8));
	//	pstGrab->grab_data = pstTemp->grab_data;
	//	m_lstGrab.AddTail(pstGrab);
	//}

	return TRUE;
}

/*
 desc : �ʱ� ������ ����
 parm : None
 retn : None
*/
VOID CDlgMmpm::Restore()
{
	UINT8 i = 0x00;
	POSITION pPos = NULL;
	LPG_ACGR pstTemp = NULL;
	LPG_ACGR pstGrab = NULL;

	m_pstGrab = NULL;
	for (; i < m_lstGrab.GetCount(); i++)
	{
		pPos = m_lstGrab.FindIndex(i);
		if (!pPos)	continue;

		pstTemp = m_lstGrab.GetAt(pPos);
		pstGrab = uvEng_Camera_GetGrabbedMark(pstTemp->cam_id, pstTemp->img_id);
		if (pstGrab)
		{
			memcpy(pstGrab, pstTemp, sizeof(STG_ACGR));
		}
	}
}

/*
 desc : ������ ���, ���� ������ ������ �����ϱ�
 parm : None
 retn : None
*/
VOID CDlgMmpm::WorkApply()
{
	
	try
	{
		for (int i = 0; i < m_lstGrab.GetCount(); i++)
		{
			auto pPos = m_lstGrab.FindIndex(i);
			if (!pPos)	throw exception();
			LPG_ACGR pstModifyed = m_lstGrab.GetAt(pPos);

			if (pstModifyed == nullptr || pstModifyed->marked == 0)
				throw exception();

			LPG_ACGR pstGrab = uvEng_Camera_GetGrabbedMark(pstModifyed->cam_id, pstModifyed->img_id);
			if (pstGrab == nullptr)
				throw exception();
			
			pstGrab->move_mm_x = pstModifyed->move_mm_x;
			pstGrab->move_mm_y = pstModifyed->move_mm_y;
			pstGrab->move_px_x = pstModifyed->move_px_x;
			pstGrab->move_px_y = pstModifyed->move_px_y;
			pstGrab->score_rate = 100;
			pstGrab->scale_rate = 100;
			pstGrab->marked = pstModifyed->marked;
			
		}
	}
	catch(...)
	{
			CDlgMesg dlgMesg;
			dlgMesg.MyDoModal(L"Unset (adjusted) value exists by operator", 0x01);
			return;
	}


	///* ������ ���� (�˻�)�� Mark ��� ���� �ӽ÷� �����ϱ� ���� */
	//for (; i < uvEng_Camera_GetGrabbedCount(); i++)
	//{

	//	uvEng_Camera_GetGrabbedMark()
	//	/* �˻��� ������ ��� */
	//	pstGrab = uvEng_Camera_GetGrabbedMarkIndex(i);

	//	/* �˻��� �����Ͱ� Guide (Mark) �ν� ������ ��츸 �޸𸮿� �ӽ� ���� */
	//	if (0x00 == pstGrab->marked || !pstGrab->IsMarkValid())
	//	{
	//		CDlgMesg dlgMesg;
	//		dlgMesg.MyDoModal(L"Unset (adjusted) value exists by operator", 0x01);
	//		return;
	//	}
	//}

	CMyDialog::OnOK();
}

/*
 desc : ����� ���, ���� ������ �����
 parm : None
 retn : None
*/
VOID CDlgMmpm::WorkCancel()
{
	CMyDialog::OnCancel();
}

/*
 desc : �ʱ� ������ ���� ��Ŵ
 parm : None
 retn : None
*/
VOID CDlgMmpm::UndoCenter()
{
	CRect rPic;

	Restore();
	FindData(0x00);
	/* �̹��� ��� ���� ��� */
	m_pic_ctl[eMMPM_PIC_VIEW].GetWindowRect(rPic);
	ScreenToClient(rPic);
	InvalidateRect(rPic);
}

/*
 desc : Ư�� ��������, ������ ����ŭ �̵�
 parm : type	- [in]  0x00 : None, 0x11 : Left, 0x12 : Right, 0x21 : Top, 0x22 : Bottom
 retn : None
*/
VOID CDlgMmpm::MoveCenter(UINT8 type)
{
	DOUBLE dbPixelSize = 0.0f;
	STG_GMFR stGMFR = { NULL };
	STG_GMSR stGMSR = { NULL };
	CRect rPic;

	if (!m_pstGrab)	return;

	/* �ȼ� ũ�� �� �ӽ� ���� */
	dbPixelSize = uvEng_GetConfig()->acam_spec.spec_pixel_um[0 /* fixed */] / 1000.0f;
	/* None Event ��, Ȯ�� ��ư ���� ��� */
	if (0x00 == type)
	{
		/* �ƹ��� �ϵ� ���� �ʴ´�. Vision Library�� ������ ���� ��ȿ ���� �� */
		m_pstGrab->SetMarkValid(0x01);
		m_pstGrab->marked = 1;
	}
	/* Keyboard Event */
	else
	{
		double step = 0;
		/* ��/��� �̵� */
		if (LEFT == type || RIGHT == type)
		{
			step = (0x11 == type) ? +m_dbStep : -m_dbStep;
			
			m_pstGrab->mark_cent_px_x -= step;
			m_ptCenter.x -= step;
		}
		/* ��/�Ϸ� �̵� */
		else
		{
			step = (0x21 == type) ? +m_dbStep : -m_dbStep;
			
			m_pstGrab->mark_cent_px_y -= step;
			m_ptCenter.y -= step;
		}
	}


	/* �� �缳�� */
	m_pstGrab->move_px_x = -(m_pstGrab->grab_width / 2.0f - m_pstGrab->mark_cent_px_x);
	m_pstGrab->move_px_y = -(m_pstGrab->grab_height / 2.0f - m_pstGrab->mark_cent_px_y);
	/* Convert pixel to mm */
	m_pstGrab->move_mm_x = m_pstGrab->move_px_x * dbPixelSize;
	m_pstGrab->move_mm_y = m_pstGrab->move_px_y * dbPixelSize;
	m_pstGrab->mark_cent_mm_x = m_pstGrab->mark_cent_px_x * dbPixelSize;
	m_pstGrab->mark_cent_mm_y = m_pstGrab->mark_cent_px_y * dbPixelSize;
	/* GMFR Data */
	stGMFR.score_rate = 100.0f;
	stGMFR.scale_rate = 100.0f;
	stGMFR.cent_x = m_pstGrab->mark_cent_px_x;
	stGMFR.cent_y = m_pstGrab->mark_cent_px_y;
	/* GMSR Data */
	stGMSR.cent_x = m_pstGrab->mark_cent_px_x;
	stGMSR.cent_y = m_pstGrab->mark_cent_px_y;
	stGMSR.mark_width = (UINT32)ROUNDUP(m_pstGrab->mark_width_px, 0);
	stGMSR.mark_height = (UINT32)ROUNDUP(m_pstGrab->mark_height_px, 0);
	stGMSR.valid_multi = 0x01;
	stGMSR.manual_set = 0x01;	/* �������� �ν� �ߴٰ� ���� (�̹����� ȸ������ �ʱ� ����) */
	
	/* --------------------------- */
	/* ������ ���� ��ȿ�ϴٰ� ���� */
	/* --------------------------- */

	//m_pstGrab->SetMarkValid(0x01);
	/* �������� �� ���� */
	uvEng_Camera_SetGrabbedMarkEx(m_pstGrab, &stGMFR, &stGMSR);

	/* �̹��� ��� ���� ��� */
	m_pic_ctl[eMMPM_PIC_VIEW].GetWindowRect(rPic);
	ScreenToClient(rPic);
	/* Align Mark Area ���� */
	InvalidateRect(rPic);

	// Overlay 
	CString sTmp;
	uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MMPM, 1);
	uvEng_Camera_OverlayAddCrossList(DISP_TYPE_MMPM, 1, m_ptCenter.x, m_ptCenter.y, 20, 20, eM_COLOR_RED);
	uvEng_Camera_DrawOverlayDC(true, DISP_TYPE_MMPM, 1);
}

/*
 desc : ���� Ű���� �̺�Ʈ ó��
 parm : key	- [in]  ���� Ű���� ��
 retn : None
*/
VOID CDlgMmpm::KeyUpDown(UINT16 key)
{
	switch (key)
	{
	case VK_LEFT:	MoveCenter(0x11);	break;	/* ����Ű ���� */
	case VK_RIGHT:	MoveCenter(0x12);	break;	/* ����Ű ������ */
	case VK_UP:	MoveCenter(0x21);	break;	/* ����Ű ���� */
	case VK_DOWN:	MoveCenter(0x22);	break;	/* ����Ű �Ʒ��� */
	}
}


/*
 desc : ���콺 �̺�Ʈ (LButtonUp) ����
 parm : flags	- [in]  Indicates whether various virtual keys are down.
						This parameter can be any combination of the following values:
		point	- [in]  Specifies the x- and y-coordinate of the cursor.
						These coordinates are always relative to the upper-left corner of the window.
 retn : None
*/
//VOID CDlgMmpm::OnLButtonDown(UINT flags, CPoint point)
//{
//	//CDPoint	dRateP;
//	//CRect rt1, rt2;
//	//GetDlgItem(IDC_MMPM_PIC_VIEW)->GetWindowRect(&rt2);
//	//rt1 = rt2;
//	//ScreenToClient(&rt2);
//	//dRateP.x = (double)ACA1920_SIZE_X / rt1.Width();
//	//dRateP.y = (double)ACA1920_SIZE_Y / rt1.Height();
//
//	CRect rt1;
//	GetDlgItem(IDC_MMPM_PIC_VIEW)->GetWindowRect(&rt1);
//	ScreenToClient(&rt1);
//
//	double dRate = 1.0 / tgt_rate;
//
//	//UINT8 u8ACamID = m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
//
//	if (rt1.PtInRect(point) && (menuPart == 5 || menuPart == 6))
//	{
//		OldZoomFlag = ZoomFlag;
//		ZoomFlag = false;
//
//		UpdateData(TRUE);
//
//		point.x = abs(point.x - rt1.left);
//		point.y = abs(point.y - rt1.top);
//
//		um_bMoveFlag = true;
//		CDPoint	iTmpP;
//		iTmpP = uvCmn_Camera_trZoomPoint(DISP_TYPE_MMPM, 1, point);
//
//		iTmpP.x = (int)(iTmpP.x * dRate + 0.5);
//		iTmpP.y = (int)(iTmpP.y * dRate + 0.5);
//
//		m_ptClickDown.x = point.x;
//		m_ptClickDown.y = point.y;
//
//		um_rectArea.left = iTmpP.x;
//		um_rectArea.top = iTmpP.y;
//		um_rectArea.right = iTmpP.x;
//		um_rectArea.bottom = iTmpP.y;
//
//		UpdateData(FALSE);
//	}
//	else if (menuPart == 2 || menuPart == 3 || menuPart == 4) {
//		OldZoomFlag = ZoomFlag;
//		ZoomFlag = false;
//	}
//	else if (ZoomFlag && menuPart == 99)
//	{
//		um_bMoveFlag = true;
//		uvCmn_Camera_SetZoomDownP(DISP_TYPE_MMPM, 1, point);
//	}
//
//
//	//CRect rPic;
//
//	///* �̹��� ��� ���� ��� */
//	//m_pic_ctl[eMMPM_PIC_VIEW].GetWindowRect(rPic);
//	//ScreenToClient(rPic);
//	///* �ش� ������ Ŭ������ ���� ��� ���� */
//	//if (!rPic.PtInRect(point))
//	//{
//	//	/* ���콺 Ŭ������ �ʾҴٰ� ���� */
//	//	m_u8LeftClick	= 0x00;
//	//	ReleaseCapture();
//	//	return;
//	//}
//	///* ���콺 Ŀ�� ���� */
//	//m_hNowCursor	= AfxGetApp()->LoadStandardCursor(IDC_CROSS);
//	//m_hPrvCursor	= ::SetCursor(m_hNowCursor);
//	///* ��ȿ ���� ���콺 Ŭ�� ���� */
//	//m_u8LeftClick	= 0x01;
//	///* ���콺�� Ŭ���� ��ġ �ӽ� ���� */
//	//m_ptClickDown.x	= point.x;
//	//m_ptClickDown.y	= point.y;
//	///* ���콺 Ŀ���� ������ ������� ��� �޽��� �޵��� ���� */
//	//SetCapture();
//
//	CMyDialog::OnLButtonDown(flags, point);
//}

/*
 desc : ���콺 �̺�Ʈ (LButtonUp) ����
 parm : flags	- [in]  Indicates whether various virtual keys are down.
						This parameter can be any combination of the following values:
		point	- [in]  Specifies the x- and y-coordinate of the cursor.
						These coordinates are always relative to the upper-left corner of the window.
 retn : None
*/
//VOID CDlgMmpm::OnLButtonUp(UINT flags, CPoint point)
//{
//	//CDPoint	dRateP;
//	int left, right, top, bottom;
//
//	//CRect rt;
//	//GetDlgItem(IDC_MMPM_PIC_VIEW)->GetWindowRect(&rt);
//	//dRateP.x = (double)ACA1920_SIZE_X / rt.Width();
//	//dRateP.y = (double)ACA1920_SIZE_Y / rt.Height();
//	//ScreenToClient(&rt);
//
//	CRect rt;
//	GetDlgItem(IDC_MMPM_PIC_VIEW)->GetWindowRect(&rt);
//	ScreenToClient(&rt);
//
//	double dRate = 1.0 / tgt_rate;
//
//	//if (um_bMoveFlag && rt.PtInRect(point) && (menuPart == 2 || menuPart == 5))
//	if (rt.PtInRect(point) && (menuPart == 2 || menuPart == 3 || menuPart == 4 || (um_bMoveFlag && (menuPart == 5 || menuPart == 6))))
//	{
//		UpdateData(TRUE);
//
//		point.x = abs(point.x - rt.left);
//		point.y = abs(point.y - rt.top);
//
//		CDPoint	iTmpP;
//		iTmpP = uvCmn_Camera_trZoomPoint(DISP_TYPE_MMPM, 1, point);
//		iTmpP.x = (int)(iTmpP.x * dRate);
//		iTmpP.y = (int)(iTmpP.y * dRate);
//
//		if (iTmpP.x < um_rectArea.left + 5)	um_rectArea.right = um_rectArea.left + 5;
//		else								um_rectArea.right = iTmpP.x;
//
//		if (iTmpP.y < um_rectArea.top + 5)	um_rectArea.bottom = um_rectArea.top + 5;
//		else								um_rectArea.bottom = iTmpP.y;
//
//		um_rectArea.right = iTmpP.x;
//		um_rectArea.bottom = iTmpP.y;
//
//		// Rect ���� ����ó��
//		left = um_rectArea.left;
//		right = um_rectArea.right;
//		top = um_rectArea.top;
//		bottom = um_rectArea.bottom;
//
//		if (left > right)
//		{
//			left = um_rectArea.right;
//			right = um_rectArea.left;
//		}
//
//		if (top > bottom)
//		{
//			top = um_rectArea.bottom;
//			bottom = um_rectArea.top;
//
//		}
//		um_rectArea.left = left;
//		um_rectArea.right = right;
//		um_rectArea.top = top;
//		um_rectArea.bottom = bottom;
//
//		if (m_pstGrab && (menuPart == 2 || menuPart == 3 || menuPart == 4)) {
//			CString sTmp;
//			//sTmp = "#1";
//			m_pt3Point_Img[menuPart - 2] = iTmpP;
//
//			int chk = 0;
//			uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MMPM, 1);
//			for (int i = 0; i < 3; i++) {
//				if (m_pt3Point_Img[i].x != 9999 && m_pt3Point_Img[i].y != 9999) {
//					chk++;
//					sTmp.Format(_T("#%d"), i + 1);
//					uvEng_Camera_OverlayAddCrossList(DISP_TYPE_MMPM, 1, m_pt3Point_Img[i].x, m_pt3Point_Img[i].y, 20, 20, eM_COLOR_GREEN);
//					uvEng_Camera_OverlayAddTextList(DISP_TYPE_MMPM, 1, m_pt3Point_Img[i].x + 10, m_pt3Point_Img[i].y + 10, sTmp, eM_COLOR_GREEN, 6, 12, VISION_FONT_TEXT, true);
//				}
//			}
//			uvEng_Camera_DrawOverlayDC(true, DISP_TYPE_MMPM, 1);
//
//
//			//////////////////////////////////
//			if (chk == 3) {
//				// �������� �߽� ���ϱ�
//				//double d, d2, yi;
//				CDPoint CenPoint[2];
//
//				// �� ���� �߽��� ���ϱ�
//				CenPoint[0].x = 0.5 * (m_pt3Point_Img[0].x + m_pt3Point_Img[1].x);
//				CenPoint[0].y = 0.5 * (m_pt3Point_Img[0].y + m_pt3Point_Img[1].y);
//				CenPoint[1].x = 0.5 * (m_pt3Point_Img[0].x + m_pt3Point_Img[2].x);
//				CenPoint[1].y = 0.5 * (m_pt3Point_Img[0].y + m_pt3Point_Img[2].y);
//
//				double a1 = -1.0 * (m_pt3Point_Img[0].x - m_pt3Point_Img[1].x) / (m_pt3Point_Img[0].y - m_pt3Point_Img[1].y);
//				double b1 = CenPoint[0].y - a1 * CenPoint[0].x;
//				double a2 = -1.0 * (m_pt3Point_Img[0].x - m_pt3Point_Img[2].x) / (m_pt3Point_Img[0].y - m_pt3Point_Img[2].y);
//				double b2 = CenPoint[1].y - a2 * CenPoint[1].x;
//
//				//CDPoint c;
//				// ������ ���� ���� �� ����.
//				if (a1 != a2) {
//					m_ptCenter.x = (b2 - b1) / (a1 - a2);
//					m_ptCenter.y = a1 * m_ptCenter.x + b1;
//
//					//uvEng_Camera_OverlayAddCrossList(DISP_TYPE_MMPM, 1, c.x, c.y, 20, 20, eM_COLOR_RED);
//				}
//
//				//if (m_pstGrab) {
//				CRect rPic;
//				/* �̹��� ��� ���� ��� */
//				m_pic_ctl[eMMPM_PIC_VIEW].GetWindowRect(rPic);
//				ScreenToClient(rPic);
//
//				/* �� �ʱ�ȭ (�ݵ�� �ʿ�) */
//				m_pstGrab->mark_cent_px_x = m_pstGrab->grab_width / 2.0f;
//				m_pstGrab->mark_cent_px_y = m_pstGrab->grab_height / 2.0f;
//				/* Point X/Y ��ġ�� rPic�� �߽ɿ��� �󸶸�ŭ ���������� ��� */
//				m_pstGrab->mark_cent_px_x += ((rPic.left + rPic.right) / 2.0f - m_ptCenter.x) / tgt_rate;
//				m_pstGrab->mark_cent_px_y += ((rPic.top + rPic.bottom) / 2.0f - m_ptCenter.y) / tgt_rate;
//				//m_pstGrab->mark_cent_px_x += ((rPic.left + rPic.right) / 2.0f - point.x) / tgt_rate;
//				//m_pstGrab->mark_cent_px_y += ((rPic.top + rPic.bottom) / 2.0f - point.y) / tgt_rate;
//
//				/* �̵��� ��ġ �� ���� */
//				MoveCenter(0x00);
//				//}
//			}
//			//uvEng_Camera_DrawOverlayDC(true, DISP_TYPE_MMPM, 1);
//			///////////////////////////////////
//		}
//		else if (menuPart == 5) {
//			CString sTmp;
//			//sTmp = "Measure";
//			double measX, measY, measLength;
//			measX = uvEng_GetConfig()->acam_spec.GetPixelToMM(0) * (double)(um_rectArea.right - um_rectArea.left);
//			measY = uvEng_GetConfig()->acam_spec.GetPixelToMM(0) * (double)(um_rectArea.bottom - um_rectArea.top);
//			measLength = sqrt(pow(measX, 2) + pow(measY, 2));
//			sTmp.Format(_T("x : %.2lf, y : %.2lf, length : %.2lf"), measX, measY, measLength);
//
//			uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MMPM, 1);
//			uvEng_Camera_OverlayAddLineList(DISP_TYPE_MMPM, 1, um_rectArea.left, um_rectArea.top, um_rectArea.right, um_rectArea.bottom, PS_SOLID, eM_COLOR_RED);
//			uvEng_Camera_OverlayAddBoxList(DISP_TYPE_MMPM, 1, um_rectArea.left, um_rectArea.top, um_rectArea.right, um_rectArea.bottom, PS_SOLID, eM_COLOR_MAGENTA);
//			uvEng_Camera_OverlayAddTextList(DISP_TYPE_MMPM, 1, um_rectArea.left + 50, um_rectArea.bottom, sTmp, eM_COLOR_MAGENTA, 6, 12, VISION_FONT_TEXT, true);
//			uvEng_Camera_DrawOverlayDC(true, DISP_TYPE_MMPM, 1);
//		}
//		else if (menuPart == 6) {
//			CString sTmp;
//			sTmp = "Auto Center ROI";
//
//			uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MMPM, 1);
//			uvEng_Camera_OverlayAddBoxList(DISP_TYPE_MMPM, 1, um_rectArea.left, um_rectArea.top, um_rectArea.right, um_rectArea.bottom, PS_SOLID, eM_COLOR_BLUE);
//			uvEng_Camera_OverlayAddTextList(DISP_TYPE_MMPM, 1, um_rectArea.left + 50, um_rectArea.bottom, sTmp, eM_COLOR_BLUE, 6, 12, VISION_FONT_TEXT, true);
//			uvEng_Camera_DrawOverlayDC(true, DISP_TYPE_MMPM, 1);
//		}
//		UpdateData(FALSE);
//		ZoomFlag = OldZoomFlag;
//	}
//
//	um_bMoveFlag = false;
//
//	///* ���콺 ���� ��ư Ŭ�� ���� ó�� */
//	//if (m_u8LeftClick)
//	//{
//	//	/* ���� Mouse Click Down ��ġ�� Up ��ġ�� �����ϸ�, ���� Ŭ���� ��ġ ������� �׸��� */
//	//	if (point.x == m_ptClickDown.x && point.y == m_ptClickDown.y)
//	//	{
//	//		OnMouseMove(flags, m_ptClickDown);
//	//	}
//	//	m_u8LeftClick	= 0x00;
//	//	if (m_hPrvCursor)	::SetCursor(m_hPrvCursor);	/* Ŀ�� ���� */
//	//	ReleaseCapture();	/* SetCapture ��� ����  */
//	//}
//
//	CMyDialog::OnLButtonUp(flags, point);
//}

/*
 desc : ���콺 �̺�Ʈ (LButtonUp) ����
 parm : flags	- [in]  Indicates whether various virtual keys are down.
						This parameter can be any combination of the following values:
		point	- [in]  Specifies the x- and y-coordinate of the cursor.
						These coordinates are always relative to the upper-left corner of the window.
 retn : None
*/
//VOID CDlgMmpm::OnMouseMove(UINT flags, CPoint point)
//{
//	//CDPoint	dRateP;
//	//CRect rt1, rt2;
//	//GetDlgItem(IDC_MMPM_PIC_VIEW)->GetWindowRect(&rt2);
//	//rt1 = rt2;
//	//ScreenToClient(&rt2);
//	//dRateP.x = (double)ACA1920_SIZE_X / rt1.Width();
//	//dRateP.y = (double)ACA1920_SIZE_Y / rt1.Height();
//
//	CRect rt;
//	GetDlgItem(IDC_MMPM_PIC_VIEW)->GetWindowRect(&rt);
//	ScreenToClient(&rt);
//
//	double dRate = 1.0 / tgt_rate;
//
//	if (um_bMoveFlag && rt.PtInRect(point) && !ZoomFlag && (menuPart == 5 || menuPart == 6))
//	{
//		UpdateData(TRUE);
//
//		point.x = abs(point.x - rt.left);
//		point.y = abs(point.y - rt.top);
//
//		CDPoint	iTmpP;
//		iTmpP = uvCmn_Camera_trZoomPoint(DISP_TYPE_MMPM, 1, point);
//		iTmpP.x = (int)(iTmpP.x * dRate);
//		iTmpP.y = (int)(iTmpP.y * dRate);
//
//		if (iTmpP.x < um_rectArea.left + 5)	um_rectArea.right = um_rectArea.left + 5;
//		else								um_rectArea.right = iTmpP.x;
//
//		if (iTmpP.y < um_rectArea.top + 5)	um_rectArea.bottom = um_rectArea.top + 5;
//		else								um_rectArea.bottom = iTmpP.y;
//
//		um_rectArea.right = iTmpP.x;
//		um_rectArea.bottom = iTmpP.y;
//
//		int		iBrushStyle;
//		iBrushStyle = PS_DOT;
//
//		if (menuPart == 5) {
//			CString sTmp;
//			sTmp = "Measure";
//			uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MMPM, 1);
//			uvEng_Camera_OverlayAddBoxList(DISP_TYPE_MMPM, 1, um_rectArea.left, um_rectArea.top, um_rectArea.right, um_rectArea.bottom, iBrushStyle, eM_COLOR_MAGENTA);
//			uvEng_Camera_OverlayAddTextList(DISP_TYPE_MMPM, 1, um_rectArea.right - 50, um_rectArea.bottom, sTmp, eM_COLOR_MAGENTA, 6, 12, VISION_FONT_TEXT, true);
//			uvEng_Camera_DrawOverlayDC(true, DISP_TYPE_MMPM, 1);
//		}
//		else if (menuPart == 6)
//		{
//			CString sTmp;
//			sTmp = "Auto Center ROI";
//			uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MMPM, 1);
//			uvEng_Camera_OverlayAddBoxList(DISP_TYPE_MMPM, 1, um_rectArea.left, um_rectArea.top, um_rectArea.right, um_rectArea.bottom, iBrushStyle, eM_COLOR_BLUE);
//			uvEng_Camera_OverlayAddTextList(DISP_TYPE_MMPM, 1, um_rectArea.right - 50, um_rectArea.bottom, sTmp, eM_COLOR_BLUE, 6, 12, VISION_FONT_TEXT, true);
//			uvEng_Camera_DrawOverlayDC(true, DISP_TYPE_MMPM, 1);
//		}
//		UpdateData(FALSE);
//	}
//	else if (ZoomFlag && um_bMoveFlag && menuPart == 99)
//	{
//		CWnd* pImageWnd = GetDlgItem(IDC_MMPM_PIC_VIEW);
//		CRect rDraw;
//		pImageWnd->GetClientRect(rDraw);
//
//		uvCmn_Camera_MoveZoomDisp(DISP_TYPE_MMPM, 1, point, rDraw);
//	}
//
//	CMyDialog::OnMouseMove(flags, point);
//}

/*
 desc : DoModal �Կ����ε� �Լ�
 parm : None
 retn : DoModal()
*/
INT_PTR CDlgMmpm::MyDoModal()
{
	return DoModal();
}

/* ----------------------------------------------------------------------------------------- */
/*                                 lk91 VISION �߰� �Լ�                                     */
/* ----------------------------------------------------------------------------------------- */

/* desc: Frame Control�� MIL DISP�� ���� */
VOID CDlgMmpm::InitDispMMPM()
{
	CWnd* pWnd;
	pWnd = GetDlgItem(IDC_MMPM_PIC_VIEW);
	uvEng_Camera_SetDispMMPM(pWnd);

	DispResize(GetDlgItem(IDC_MMPM_PIC_VIEW));
}

/* desc: Frame Control�� �̹��� ������ ������ ������ */
void CDlgMmpm::DispResize(CWnd* pWnd)
{
	CRect lpRect;
	pWnd->GetWindowRect(lpRect);
	ScreenToClient(lpRect);

	CSize szDraw(lpRect.right - lpRect.left, lpRect.bottom - lpRect.top);
	CSize tgtSize;

	/* �ݵ�� 1.0f ������ �ʱ�ȭ */
	tgt_rate = 1.0f;

	/* ���� ũ�� ������ ���� ũ�� �������� ū ��� */
	if ((DOUBLE(szDraw.cx) / DOUBLE(ACA1920_SIZE_X)) > (DOUBLE(szDraw.cy) / DOUBLE(ACA1920_SIZE_Y)))
	{
		/* <����> ���� �������� ���� Ȯ�� or ��� ���� */
		tgt_rate = DOUBLE(szDraw.cy) / DOUBLE(ACA1920_SIZE_Y);
	}
	/* ���� ũ�� ������ ���� ũ�� �������� ū ��� */
	else
	{
		/* <����> ���� �������� ���� Ȯ�� or ��� ���� */
		tgt_rate = DOUBLE(szDraw.cx) / DOUBLE(ACA1920_SIZE_X);
	}
	/* ���� �̹��� ������ �°� ���� �̹��� ũ�� ���� */
	tgtSize.cx = (int)(tgt_rate * ACA1920_SIZE_X);
	tgtSize.cy = (int)(tgt_rate * ACA1920_SIZE_Y);

	lpRect.left = ((lpRect.right - lpRect.left) / 2 + lpRect.left) - (tgtSize.cx / 2);
	lpRect.top = ((lpRect.bottom - lpRect.top) / 2 + lpRect.top) - (tgtSize.cy / 2);
	lpRect.right = lpRect.left + tgtSize.cx;
	lpRect.bottom = lpRect.top + tgtSize.cy;

	pWnd->MoveWindow(lpRect, TRUE);

	uvEng_Camera_SetMMPMDispSize(tgtSize);
}

BOOL CDlgMmpm::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CWnd* pImageWnd = GetDlgItem(IDC_MMPM_PIC_VIEW);
	CRect rDraw;
	pImageWnd->GetClientRect(rDraw);

	WINDOWPLACEMENT wndpl;
	int idNo = IDC_MMPM_PIC_VIEW;
	GetDlgItem(idNo)->GetWindowPlacement(&wndpl);
	CRect rectTmp;
	rectTmp = wndpl.rcNormalPosition;
	if (pt.x < rectTmp.left || pt.x > rectTmp.right
		|| pt.y < rectTmp.top || pt.y > rectTmp.bottom)
		return false;


	if (zDelta > 0) {
		uvCmn_Camera_MilZoomIn(DISP_TYPE_MMPM, 1, rDraw);
		ZoomFlag = true;
	}
	else {
		if (!uvCmn_Camera_MilZoomOut(DISP_TYPE_MMPM, 1, rDraw)) {
			ZoomFlag = false;
		}
		else {
			ZoomFlag = true;
		}
	}

	return CMyDialog::OnMouseWheel(nFlags, zDelta, pt);
}


//void CDlgMmpm::OnContextMenu(CWnd* /*pWnd*/, CPoint fi_pointP/*point*/)
//{
//	UpdateData(TRUE);
//
//	WINDOWPLACEMENT wndpl;
//	int idNo = IDC_MMPM_PIC_VIEW;
//	GetDlgItem(idNo)->GetWindowPlacement(&wndpl);
//	CRect rectTmp;
//	rectTmp = wndpl.rcNormalPosition;
//	GetDlgItem(idNo)->GetWindowRect(rectTmp);
//	if (fi_pointP.x < rectTmp.left || fi_pointP.x > rectTmp.right
//		|| fi_pointP.y < rectTmp.top || fi_pointP.y > rectTmp.bottom)
//		return;
//
//	CMenu* pMenu, * pSubMenu;
//
//	pMenu = new CMenu();
//	pMenu->LoadMenuW(IDR_MENU1);
//	pSubMenu = pMenu->GetSubMenu(1);
//	//if (menuPart == 1) { // MARK ROI
//	//	pSubMenu->CheckMenuItem(ID_MENU_MARK_ROI, MF_BYCOMMAND | MF_CHECKED);
//	//	pSubMenu->CheckMenuItem(ID_MENU_MEASURE, MF_BYCOMMAND | MF_UNCHECKED);
//	//}
//	//else if (menuPart == 2 || menuPart == 4) { // SEARCH ROI || ZOOM
//	//	pSubMenu->CheckMenuItem(ID_MENU_MARK_ROI, MF_BYCOMMAND | MF_UNCHECKED);
//	//	pSubMenu->CheckMenuItem(ID_MENU_MEASURE, MF_BYCOMMAND | MF_UNCHECKED);
//	//}
//	//else if (menuPart == 3) { // MEASURE
//	//	pSubMenu->CheckMenuItem(ID_MENU_MARK_ROI, MF_BYCOMMAND | MF_UNCHECKED);
//	//	pSubMenu->CheckMenuItem(ID_MENU_MEASURE, MF_BYCOMMAND | MF_CHECKED);
//	//}
//	//else { // 99
//	//	pSubMenu->CheckMenuItem(ID_MENU_MARK_ROI, MF_BYCOMMAND | MF_UNCHECKED);
//	//	pSubMenu->CheckMenuItem(ID_MENU_SEARCH_ROI_1, MF_BYCOMMAND | MF_UNCHECKED);
//	//	pSubMenu->CheckMenuItem(ID_MENU_SEARCH_ROI_2, MF_BYCOMMAND | MF_UNCHECKED);
//	//	pSubMenu->CheckMenuItem(ID_MENU_SEARCH_ALL, MF_BYCOMMAND | MF_UNCHECKED);
//	//	pSubMenu->CheckMenuItem(ID_MENU_MEASURE, MF_BYCOMMAND | MF_UNCHECKED);
//	//}
//
//	if (menuPart == 99) {
//		pSubMenu->CheckMenuItem(ID_3POINTSET_1, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_3POINTSET_2, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_3POINTSET_3, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_MMPM_MEASURE, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_MMPM_AUTOCENTERROI, MF_BYCOMMAND | MF_UNCHECKED);
//	}
//	else if (menuPart == 2) {
//		pSubMenu->CheckMenuItem(ID_3POINTSET_1, MF_BYCOMMAND | MF_CHECKED);
//		pSubMenu->CheckMenuItem(ID_3POINTSET_2, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_3POINTSET_3, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_MMPM_MEASURE, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_MMPM_AUTOCENTERROI, MF_BYCOMMAND | MF_UNCHECKED);
//	}
//	else if (menuPart == 3) {
//		pSubMenu->CheckMenuItem(ID_3POINTSET_1, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_3POINTSET_2, MF_BYCOMMAND | MF_CHECKED);
//		pSubMenu->CheckMenuItem(ID_3POINTSET_3, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_MMPM_MEASURE, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_MMPM_AUTOCENTERROI, MF_BYCOMMAND | MF_UNCHECKED);
//	}
//	else if (menuPart == 4) {
//		pSubMenu->CheckMenuItem(ID_3POINTSET_1, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_3POINTSET_2, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_3POINTSET_3, MF_BYCOMMAND | MF_CHECKED);
//		pSubMenu->CheckMenuItem(ID_MMPM_MEASURE, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_MMPM_AUTOCENTERROI, MF_BYCOMMAND | MF_UNCHECKED);
//	}
//	else if (menuPart == 5) {
//		pSubMenu->CheckMenuItem(ID_3POINTSET_1, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_3POINTSET_2, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_3POINTSET_3, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_MMPM_MEASURE, MF_BYCOMMAND | MF_CHECKED);
//		pSubMenu->CheckMenuItem(ID_MMPM_AUTOCENTERROI, MF_BYCOMMAND | MF_UNCHECKED);
//	}
//	else if (menuPart == 6) {
//		pSubMenu->CheckMenuItem(ID_3POINTSET_1, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_3POINTSET_2, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_3POINTSET_3, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_MMPM_MEASURE, MF_BYCOMMAND | MF_UNCHECKED);
//		pSubMenu->CheckMenuItem(ID_MMPM_AUTOCENTERROI, MF_BYCOMMAND | MF_CHECKED);
//	}
//	int cmd;
//	cmd = pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD, fi_pointP.x, fi_pointP.y, this);
//	if (pMenu)
//	{
//		delete pMenu;
//		pMenu = NULL;
//	}
//	if (cmd == ID_ZOOM_IN_MMPM)
//	{
//		MenuZoomIn();
//		menuPart = 99;
//
//	}
//	else if (cmd == ID_ZOOM_OUT_MMPM)
//	{
//		MenuZoomOut();
//		menuPart = 99;
//	}
//	else if (cmd == ID_ZOOM_FIT_MMPM)
//	{
//		MenuZoomFit();
//		menuPart = 99;
//	}
//	else if (cmd == ID_3POINTSET_1)
//	{
//		if (menuPart == 2) {
//			menuPart = 99;
//		}
//		else {
//			menuPart = 2;
//		}
//	}
//	else if (cmd == ID_3POINTSET_2)
//	{
//		if (menuPart == 3) {
//			menuPart = 99;
//		}
//		else {
//			menuPart = 3;
//
//		}
//	}
//	else if (cmd == ID_3POINTSET_3)
//	{
//		if (menuPart == 4) {
//			menuPart = 99;
//		}
//		else {
//			menuPart = 4;
//
//		}
//	}
//	else if (cmd == ID_MMPM_MEASURE)
//	{
//		if (menuPart == 5) {
//			menuPart = 99;
//		}
//		else {
//
//			menuPart = 5;
//		}
//	}
//	else if (cmd == ID_MMPM_AUTOCENTERROI)
//	{
//		if (menuPart == 6) {
//			menuPart = 99;
//		}
//		else {
//
//			menuPart = 6;
//		}
//	}
//	else {
//		menuPart = 99;
//	}
//}

/* desc: Zoom In */
void CDlgMmpm::MenuZoomIn()
{
	//RECT rDraw;
	CWnd* pImageWnd = GetDlgItem(IDC_MMPM_PIC_VIEW);
	CRect rDraw;
	pImageWnd->GetClientRect(rDraw);
	uvCmn_Camera_MilZoomIn(DISP_TYPE_MMPM, 1, rDraw);
	ZoomFlag = true;
}

/* desc: Zoom Out */
void CDlgMmpm::MenuZoomOut()
{
	CWnd* pImageWnd = GetDlgItem(IDC_MMPM_PIC_VIEW);
	CRect rDraw;
	pImageWnd->GetClientRect(rDraw);
	if (!uvCmn_Camera_MilZoomOut(DISP_TYPE_MMPM, 1, rDraw))
	{
		ZoomFlag = false;
	}
	else {
		ZoomFlag = true;
	}
}

/* desc: Zoom Fit */
void CDlgMmpm::MenuZoomFit()
{
	uvCmn_Camera_MilAutoScale(DISP_TYPE_MMPM, 1);
	ZoomFlag = false;
}

/* desc: Auto Center */
//void CDlgMmpm::AutoCenter()
//{
//	// 1. ROI ������ AUTO CENTER �̹��� ����ϱ�
//	LPG_ACGR pstGrab = NULL;
//
//	if (abs(um_rectArea.left - um_rectArea.right) == 0 || abs(um_rectArea.top - um_rectArea.bottom) == 0) {
//		AfxMessageBox(_T("ȭ�鿡 �����ϰ��� �ϴ� ROI�� �������ּ���."));
//		return;
//	}
//
//	/* ���� ������ �߻��� ù ��° �׸� ����  */
//	if (m_lstGrab.GetCount())
//	{
//		pstGrab = m_lstGrab.GetAt(m_lstGrab.FindIndex(m_u8Index));
//
//		m_pstGrab = pstGrab; // lk91 image test �� �� �Ʒ� ���� �ּ� ��, �ּ� Ǯ��
//		//if (!pstGrab)	m_pstGrab	= NULL;
//		//else			m_pstGrab	= uvEng_Camera_GetGrabbedMark(pstGrab->cam_id, pstGrab->img_id);
//	}
//	else {
//		AfxMessageBox(_T("Grab Count Zero."));
//		return;
//	}
//	uvCmn_Camera_RegistMMPM_AutoCenter(um_rectArea, m_pstGrab->cam_id, m_pstGrab->img_id);
//
//	// 2. Dlg ����
//	CDlgMMPMAutoCenter dlgMMPMAutoCenter;
//	if (dlgMMPMAutoCenter.DoModal() == IDOK) {
//
//	}
//
//
//}

