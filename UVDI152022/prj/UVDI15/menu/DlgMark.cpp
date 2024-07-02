
/*
 desc : Mark Model
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgMark.h"
#include "DlgEdge.h"
#include "DlgSetMark.h"

#include "./mark/DrawModel.h"		/* Mark Model Object */
#include "./mark/GridMark.h"		/* Grid Control */
#include "./mark/GridModel.h"		/* Grid Control */
#include "./mark/GridModelInMark.h"	/* Grid Control */
#include "./mark/GridMiniMotor.h"

#include "../mesg/DlgMesg.h"
#include "../GlobalVariables.h"


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
CDlgMark::CDlgMark(UINT32 id, CWnd* parent)
	: CDlgMenu(id, parent)
{
	m_pGridMotor	= NULL;
	m_u8ViewMode	= 0x00;
	m_u64ReqCmdTime	= GetTickCount64();
	/* �ڽ��� ���̾�α� ID ���� */
	m_enDlgID		= ENG_CMDI::en_menu_mark;
	um_bMoveFlag	= false;

	m_u8lamp_type = 0x00;
	mark_find_mode = 0;
	for(int i = 0; i<2; i++)
		uvCmn_Camera_SetMarkFindMode(i+1, mark_find_mode, 2);

	bmmfFile = false;
	bpatFile = false;

	calib_row = 5;
	calib_col = 5;
}

CDlgMark::~CDlgMark()
{
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgMark::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;
	/* groupbox - normal */
	u32StartID	= IDC_MARK_GRP_MODEL_INFO;
	for (i=0; i< eMARK_GRP_MAX; i++)	DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	u32StartID = IDC_MARK_GRP_STROBE_VIEW;
	for (i = 0; i < eMARK_GRD_MAX; i++)	DDX_Control(dx, u32StartID + i, m_grd_ctl[i]);
//	DDX_Control(dx, IDC_MARK_GRP_STROBE_VIEW, m_grd_ctl[0]);
	/* static - picture */
	//u32StartID	= IDC_MARK_PIC_MODEL_1;
	//for (i=0; i< eMARK_PIC_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_pic_img[i]);	
	//DDX_Control(dx, IDC_MARK_IDC_MARK_GRP_SEARCH_RESULT_2, m_pic_img[eMARK_PIC_MAX - 1]); 
	/* static - normal */
	u32StartID	= IDC_MARK_TXT_SIZE_1;
	for (i=0; i< eMARK_TXT_MAX; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* edit - recipe */
	u32StartID	= IDC_MARK_EDT_MODEL_NAME;
	for (i=0; i< eMARK_EDT_TXT_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_edt_txt[i]);
	/* edit - recipe */
	u32StartID	= IDC_MARK_EDT_MMF;
	for (i=0; i< eMARK_EDT_CTL_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
	/* edit - float */
	u32StartID	= IDC_MARK_EDT_SIZE_1;
	for (i=0; i< eMARK_EDT_FLOAT_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_edt_flt[i]);
	/* edit - float for input */
	u32StartID	= IDC_MARK_EDT_CENT_ERR_X;
	for (i=0; i< eMARK_EDT_OUT_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_edt_out[i]);
	/* button - normal */
	u32StartID	= IDC_MARK_BTN_MODEL_APPEND;
	for (i=0; i< eMARK_BTN_CTL_MAX; i++)	DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]); 
	/* button - mark set */
	u32StartID	= IDC_MARK_BTN_MARK_SET_11;
	for (i=0; i< eMARK_BTN_SET_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_btn_set[i]);
	/* button - mark reset */
	u32StartID	= IDC_MARK_BTN_MARK_RESET_1;
	for (i=0; i< eMARK_BTN_RST_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_btn_rst[i]);
	/* check - model type */
	u32StartID	= IDC_MARK_CHK_MODEL_CIRCLE;
	for (i=0; i< eMARK_CHK_TYP_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_chk_typ[i]);
	/* check - model color */
	u32StartID = IDC_MARK_CHK_COLOR_BLACK;
	for (i = 0; i < eMARK_CHK_CLR_MAX; i++)		DDX_Control(dx, u32StartID + i, m_chk_clr[i]);
	/* check - camera */
	u32StartID	= IDC_MARK_CHK_ACAM_1;
	for (i=0; i< eMARK_CHK_CAM_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_chk_cam[i]);

	u32StartID = IDC_MARK_CHK_IMG_PROC;
	for (i = 0; i < eMARK_CHK_IMG_MAX; i++)		DDX_Control(dx, u32StartID + i, m_chk_img[i]);
}

BEGIN_MESSAGE_MAP(CDlgMark, CDlgMenu)
	ON_NOTIFY(NM_CLICK, IDC_GRID_MARK_MARK_LIST,										OnGridMarkList)
	ON_NOTIFY(NM_CLICK, IDC_GRID_MARK_MODEL_LIST,										OnGridModelList)
	ON_NOTIFY(NM_CLICK, IDC_GRID_MARK_MARK_MODEL,										OnGridMarkModel)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MARK_EDT_SIZE_1, IDC_MARK_EDT_SIZE_4,				OnTypeClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_MARK_BTN_MODEL_APPEND, IDC_MARK_BTN_STROBE_GET,	OnBtnClick)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_MARK_CHK_MODEL_CIRCLE, IDC_MARK_CHK_LIVE,			OnChkClick)
	ON_NOTIFY(NM_CLICK, IDC_MARK_GRP_STROBE_VIEW, &CDlgMark::OnClickGridInput)
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_MARK_CHK_IMG_PROC, &CDlgMark::ProcImage)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgMark::PreTranslateMessage(MSG* msg)
{
	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMark::OnInitDlg()
{
	/* ��Ʈ�� �ʱ�ȭ */
	InitCtrl();
	if (!InitGrid())	return FALSE;
	/* ��ü �ʱ�ȭ */
	if (!InitObject())	return FALSE;
	/* ��Ʈ�� �ʱ�ȭ */
	EnableModelSize();
	/* �ʱ� �� ���� */
	InitValue();
	/* �⺻ �� ���� */
	SetBaseData();
	//LoadTriggerCh();
	/* MIL DISP ���� */
	InitDispMark();
	/*Strobe ���� Ȯ��*/
	InitGridStrobeView();

	m_edt_out[eMARK_EDT_GAIN_LEVEL1].SetTextToNum(uvEng_GetConfig()->set_basler.cam_gain_level[0], 1);
	m_edt_out[eMARK_EDT_GAIN_LEVEL2].SetTextToNum(uvEng_GetConfig()->set_basler.cam_gain_level[1], 1);
	m_edt_out[eMARK_EDT_GAIN_LEVEL3].SetTextToNum(uvEng_GetConfig()->set_basler.cam_gain_level[2], 1);



	menuPart = 99;

	SetDlgItemInt(IDC_MARK_EDT_CALIB_R, calib_row);
	SetDlgItemInt(IDC_MARK_EDT_CALIB_C, calib_col);

	CalibROI_left = (int*)malloc(sizeof(int) * (calib_row * calib_col));
	CalibROI_right = (int*)malloc(sizeof(int) * (calib_row * calib_col));
	CalibROI_top = (int*)malloc(sizeof(int) * (calib_row * calib_col));
	CalibROI_bottom = (int*)malloc(sizeof(int) * (calib_row * calib_col));

	memset(CalibROI_left, 0, sizeof(int) * (calib_row * calib_col));
	memset(CalibROI_right, 0, sizeof(int) * (calib_row * calib_col));
	memset(CalibROI_top, 0, sizeof(int) * (calib_row * calib_col));
	memset(CalibROI_bottom, 0, sizeof(int) * (calib_row * calib_col));


	ZoomFlag = new BOOL[uvEng_GetConfig()->set_cams.acam_count];
	searchROI_CAM = new BOOL[uvEng_GetConfig()->set_cams.acam_count];
	

	for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++) {
		searchROI_CAM[i] = false;
	}

	searchROI_ALL = false;


#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	GetDlgItem(IDC_MARK_CHK_ACAM_3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_MARK_TXT_CAM3_GAIN_LEVEL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_MARK_EDIT_CAM3_GAIN_LEVEL)->ShowWindow(SW_HIDE);
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)

#endif


	return TRUE;
}



/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgMark::OnExitDlg()
{
	m_vUILevel.clear();

	/* Ʈ���ź��� Disabled */
	//uvEng_Trig_ReqTriggerStrobe(FALSE);
	uvEng_Mvenc_ReqTriggerStrobe(FALSE);

	CloseObject();
	CloseGrid();

	delete[] ZoomFlag;
	delete[] searchROI_CAM;

	free(CalibROI_left);
	free(CalibROI_right);
	free(CalibROI_top);
	free(CalibROI_bottom);


	

	//searchROI_CAM = nullptr;
	//ZoomFlag = nullptr;

	for (int i = 0; i < _countof(m_grd_ctl); i++)
	{
		m_grd_ctl[i].DeleteAllItems();
	}
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgMark::OnPaintDlg(CDC *dc) 
{
	//UINT8 i = 0x00;
	ENG_VCCM enMode	= uvEng_Camera_GetCamMode();
	RECT rDraw;

	/* ���� Ȥ�� ���� �ֱٿ� ���õ� ��ũ ���� ��� (����) */
	for (int i = 0; i < 1 && m_pDrawModel; i++) {
		//if (m_pDrawModel[i]->GetFindType() == 0) {
		if (m_pDrawModel[i]->GetModelType() != ENG_MMDT::en_image) {
			GetDlgItem(IDC_MARK_PIC_MODEL_1 + i)->ShowWindow(SW_HIDE);
		}
	}
	for (int i = 0; i < 1 && m_pDrawModel; i++) {
		//if (m_pDrawModel[i]->GetFindType() == 0) {
		if (m_pDrawModel[i]->GetModelType() != ENG_MMDT::en_image) {
			//GetDlgItem(IDC_MARK_PIC_MODEL_1 + i)->ShowWindow(SW_HIDE);
			m_pDrawModel[i]->DrawModel(dc->m_hDC); 
		}
		else { // image
			if (bmmfFile || bpatFile) {
				GetDlgItem(IDC_MARK_PIC_MODEL_1 + i)->ShowWindow(SW_SHOW);
				/* �̹����� ��µ� ���� ���� */
				GetDlgItem(IDC_MARK_PIC_MODEL_1 + i)->GetWindowRect(&rDraw);
				ScreenToClient(&rDraw);

				TCHAR* tcharFileName = _T("");
				CString		strDir;
				int findMode;

				if (bmmfFile) {
					findMode = 0; // MOD
					strDir.Format(_T("%s\\%s\\mmf\\%s.mmf"), g_tzWorkDir, CUSTOM_DATA_CONFIG, m_pDrawModel[i]->GetMMFFile());
				}
				else {
					findMode = 1; // PAT
					strDir.Format(_T("%s\\%s\\pat\\%s.pat") ,g_tzWorkDir, CUSTOM_DATA_CONFIG,  m_pDrawModel[i]->GetPATFile());
				}
				tcharFileName = (TCHAR*)(LPCTSTR)strDir;
				uvCmn_Camera_PutMarkDisp(GetDlgItem(IDC_MARK_PIC_MODEL_1 + i)->GetSafeHwnd(), TMP_MARK, rDraw, (int)(i / 2) + 1, tcharFileName, findMode);
			}

		}
	}

	/* �̹����� ��µ� ���� ���� */
	GetDlgItem(IDC_MARK_PIC_FIND_GRAB)->GetWindowRect(&rDraw);
	ScreenToClient(&rDraw);
	//if (enMode == ENG_VCCM::en_cali_mode || enMode == ENG_VCCM::en_edge_mode || enMode == ENG_VCCM::en_live_mode)
		GetDlgItem(IDC_MARK_PIC_FIND_GRAB)->ShowWindow(SW_SHOW);
	/* ���� ��ũ ���� �˻� ����� ��� */
	switch (enMode)
	{
	case ENG_VCCM::en_cali_mode:	uvEng_Camera_DrawCaliBitmap(dc->m_hDC, rDraw);						break;
	case ENG_VCCM::en_edge_mode:	uvEng_Camera_DrawMBufID(GetDlgItem(IDC_MARK_PIC_FIND_GRAB)->GetSafeHwnd(), rDraw, 0x01);	break;
	}

	/* �ڽ� ���� ȣ�� */
	CDlgMenu::DrawOutLine();
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : is_busy	- [in]  TRUE: ���� �ó����� ���� �� ..., FALSE: ���� Idle ����
 retn : None
*/
VOID CDlgMark::OnResizeDlg()
{
}

void CDlgMark::RegisterUILevel()
{
	m_vUILevel.push_back(ST_UI_LEVEL(EN_LOGIN_LEVEL::eLOGIN_LEVEL_OPERATOR, &m_btn_ctl[eMARK_BTN_CTL_MODEL_APPEND]));
	m_vUILevel.push_back(ST_UI_LEVEL(EN_LOGIN_LEVEL::eLOGIN_LEVEL_ENGINEER, &m_btn_ctl[eMARK_BTN_CTL_MODEL_DELETE]));
	m_vUILevel.push_back(ST_UI_LEVEL(EN_LOGIN_LEVEL::eLOGIN_LEVEL_ADMIN   , &m_btn_ctl[eMARK_BTN_CTL_MODEL_RESET]));
}

/*
 desc : �θ� �����忡 ���� �ֱ������� ȣ���
 parm : tick	- [in]  ���� CPU �ð�
		is_busy	- [in]  TRUE: ���� �ó����� ���� �� ..., FALSE: ���� Idle ����
 retn : None
*/
VOID CDlgMark::UpdatePeriod(UINT64 tick, BOOL is_busy)
{
	/* Control Enable or Disable */
	//UpdateControl(tick, is_busy); 
	UpdateLiveView();
}

/*
 desc : ��Ʈ��
 parm : tick	- [in]  ���� CPU �ð�
		is_busy	- [in]  TRUE: ���� �ó����� ���� �� ..., FALSE: ���� Idle ����
 retn : None
*/
VOID CDlgMark::UpdateControl(UINT64 tick, BOOL is_busy)
{
	BOOL bSelected	= m_pGridMark->GetSelected() ? TRUE : FALSE;

	if (m_pGridMotor)	m_pGridMotor->UpdateValue();
	if (!is_busy)
	{
		UpdateLiveView();	/* Update Live ���� */
		/* Update PH Z �� ��ġ ���� ��û */
		if (m_u64ReqCmdTime + 1000 > tick)
		{
			m_u64ReqCmdTime = tick;
			uvEng_Luria_ReqGetMotorAbsPositionAll();
		}
	}
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgMark::InitCtrl()
{
	for(int i = 0; i< 1; i++)
		GetDlgItem(IDC_MARK_PIC_MODEL_1 + i)->ShowWindow(SW_HIDE);

	// by sysandj : Resize UI
	CResizeUI clsResizeUI;
	// by sysandj : Resize UI

	INT32 i = 0;
	DOUBLE dbMin[eMARK_EDT_FLOAT_MAX]	= {   30.0f,   30.0f,   30.0f,   30.0f};
	DOUBLE dbMax[eMARK_EDT_FLOAT_MAX]	= { 3000.0f, 3000.0f, 3000.0f, 3000.0f};
	/* group box */
	for (i = 0; i < eMARK_GRP_MAX; i++)
	{
		m_grp_ctl[i].SetLogFont(&g_lf[eFONT_LEVEL2_BOLD]);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_grp_ctl[i]);
		// by sysandj : Resize UI
	}
	/* static - Normal */
	for (i=0; i< eMARK_TXT_MAX; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_ctl[i]);
		// by sysandj : Resize UI
	}
	/* edit - text */
	for (i=0; i< eMARK_EDT_TXT_MAX; i++)
	{
		m_edt_txt[i].SetEditFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_edt_txt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_txt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_txt[i].SetReadOnly(TRUE);
		m_edt_txt[i].SetInputMaxStr(MARK_MODEL_NAME_LENGTH);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_edt_txt[i]);
		// by sysandj : Resize UI
	}
	/* edit - ctrl */
	for (i=0; i< eMARK_EDT_CTL_MAX; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_ctl[i].SetReadOnly(TRUE);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_edt_ctl[i]);
		// by sysandj : Resize UI
	}
	/* edit - keyboard - float */
	for (i=0; i< eMARK_EDT_FLOAT_MAX; i++)
	{
		m_edt_flt[i].SetEditFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_edt_flt[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_flt[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_flt[i].SetReadOnly(TRUE);
		m_edt_flt[i].SetInputType(ENM_DITM::en_float);
		m_edt_flt[i].SetInputSign(ENM_MNUS::en_unsign);
		m_edt_flt[i].SetMinMaxNum(dbMin[i], dbMax[i]);
		m_edt_flt[i].SetInputPoint(1);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_edt_flt[i]);
		// by sysandj : Resize UI
	}

	for (i = 0; i < eMARK_EDT_OUT_MAX; i++)
	{
		m_edt_out[i].SetEditFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_edt_out[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_out[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_out[i].SetReadOnly(TRUE);
		m_edt_out[i].SetInputType(ENM_DITM::en_float);
		m_edt_out[i].SetInputSign(ENM_MNUS::en_unsign);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_edt_out[i]);
		// by sysandj : Resize UI
	}

	/* static - Picture - Model & Mark (Grabbed) */
	//for (i=0; i< eMARK_PIC_MAX; i++)
	//{
	//	m_pic_img[i].SetTextFont(&g_lf);
	//	m_pic_img[i].SetDrawBg(1);
	//	m_pic_img[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

	//	// by sysandj : Resize UI
	//	clsResizeUI.ResizeControl(this, &m_pic_img[i]);
	//	// by sysandj : Resize UI
	//}
	/* button - normal */
	for (i=0; i< eMARK_BTN_CTL_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}
	/* button - set / reset */
	for (i=0; i< eMARK_BTN_SET_MAX; i++)
	{
		m_btn_set[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_set[i].SetBgColor(g_clrBtnColor);
		m_btn_set[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_set[i]);
		// by sysandj : Resize UI
	}
	for (i=0; i< eMARK_BTN_RST_MAX; i++)
	{
		m_btn_rst[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_rst[i].SetBgColor(g_clrBtnColor);
		m_btn_rst[i].SetTextColor(g_clrBtnTextColor);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_rst[i]);
		// by sysandj : Resize UI
	}
	/* button - motion move */
// 	for (i=0; i< eMARK_BTN_MOV_MAX; i++)
// 	{
// 		m_btn_mov[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
// 		m_btn_mov[i].SetBgColor(g_clrBtnColor);
// 		m_btn_mov[i].SetTextColor(g_clrBtnTextColor);
// 
// 		// by sysandj : Resize UI
// 		clsResizeUI.ResizeControl(this, &m_btn_mov[i]);
// 		// by sysandj : Resize UI
// 	}
	/* check - Model Type */
	for (i=0; i< eMARK_CHK_TYP_MAX; i++)
	{
		m_chk_typ[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_chk_typ[i].SetBgColor(RGB(255, 255, 255));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_chk_typ[i]);
		// by sysandj : Resize UI
	}

// 	for (i = 0; i < eMARK_CHK_AXS_MAX; i++)
// 	{
// 		m_chk_axs[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
// 		m_chk_axs[i].SetBgColor(RGB(255, 255, 255));
// 
// 		// by sysandj : Resize UI
// 		clsResizeUI.ResizeControl(this, &m_chk_axs[i]);
// 		// by sysandj : Resize UI
// 	}

	/* check - Model Color */
	for (i=0; i< eMARK_CHK_CLR_MAX; i++)
	{
		m_chk_clr[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_chk_clr[i].SetBgColor(RGB(255, 255, 255));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_chk_clr[i]);
		// by sysandj : Resize UI
	}
	/* Move Type (Absolute, Relative) */
// 	for (i=0; i< eMARK_CHK_MVT_MAX; i++)
// 	{
// 		m_chk_mvt[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
// 		m_chk_mvt[i].SetBgColor(RGB(255, 255, 255));
// 
// 		// by sysandj : Resize UI
// 		clsResizeUI.ResizeControl(this, &m_chk_mvt[i]);
// 		// by sysandj : Resize UI
// 	}
	/* Align Camera / Live View */
	for (i=0; i< eMARK_CHK_CAM_MAX; i++)
	{
		m_chk_cam[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_chk_cam[i].SetBgColor(RGB(255, 255, 255));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_chk_cam[i]);
		// by sysandj : Resize UI
	}
	m_chk_cam[eMARK_CHK_CAM_ACAM_1].SetCheck(1);

	clsResizeUI.ResizeControl(this, GetDlgItem(IDC_MARK_PIC_FIND_GRAB));
	for (i = 0; i < 1; i++)
	{
		clsResizeUI.ResizeControl(this, GetDlgItem(IDC_MARK_PIC_MODEL_1 + i));
	}

	for (i = 0; i < eMARK_CHK_IMG_MAX; i++)
	{
		m_chk_img[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_chk_img[i].SetBgColor(RGB(255, 255, 255));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_chk_img[i]);
		// by sysandj : Resize UI
	}
}

/*
 desc : GridControl �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMark::InitGrid()
{
	/* Mark Grid */
// 	m_pGridMark = new CGridMark(m_hWnd, m_grp_ctl[eMARK_GRP_RECIPE_LIST].GetSafeHwnd(), IDC_GRID_MARK_MARK_LIST);
// 	ASSERT(m_pGridMark);
// 	if (!m_pGridMark->Init(FALSE, TRUE))	return FALSE;
	/* Model Grid */
	m_pGridModel = new CGridModel(m_hWnd, m_grp_ctl[eMARK_GRP_MODEL_LIST].GetSafeHwnd(), IDC_GRID_MARK_MODEL_LIST);
	ASSERT(m_pGridModel);
	if (!m_pGridModel->Init(FALSE, TRUE))	return FALSE;
	/* ModelInMark Grid */
	m_pGridModelInMark = new CGridModelInMark(m_hWnd, m_grp_ctl[eMARK_GRP_RECIPE_MODELS].GetSafeHwnd(), IDC_GRID_MARK_MARK_MODEL);
	ASSERT(m_pGridModelInMark);
	if (!m_pGridModelInMark->Init(FALSE, FALSE, TRUE))	return FALSE;
	GetDlgItem(IDC_GRID_MARK_MARK_MODEL)->ShowWindow(FALSE);
	/* Motor Grid */
// 	m_pGridMotor= new CGridMiniMotor(m_hWnd, m_grp_ctl[eMARK_GRP_MOTION_CTRL].GetSafeHwnd());
// 	ASSERT(m_pGridMotor);
// 	m_pGridMotor->InitGrid();

	return TRUE;
}

/*
 desc : GridControl ����
 parm : None
 retn : None
*/
VOID CDlgMark::CloseGrid()
{
	//if (m_pGridMark)		delete m_pGridMark;
	if (m_pGridModel)		delete m_pGridModel;
	//if (m_pGridModelInMark)	delete m_pGridModelInMark;
	//if (m_pGridMotor)		delete m_pGridMotor;
}

/*
 desc : ��ü �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMark::InitObject()
{
	UINT32 i	= 0;
	RECT rDraw;

	/* ��ũ �˻� ��� ��� ��ü */
	m_pDrawModel	= new CDrawModel * [1]; 
	ASSERT(m_pDrawModel);
	for (i=0; i< 1; i++)
	{
		GetDlgItem(IDC_MARK_PIC_MODEL_1 + i)->GetWindowRect(&rDraw);		
		ScreenToClient(&rDraw);
		m_pDrawModel[i]	= new CDrawModel(rDraw);
		ASSERT(m_pDrawModel[i]);
	}

	return TRUE;
}

/*
 desc : ��ü ����
 parm : None
 retn : None
*/
VOID CDlgMark::CloseObject()
{
	UINT8 i	= 0x00;

	if (m_pDrawModel)
	{
		for (i=0; i< 1; i++)	delete m_pDrawModel[i];
		delete [] m_pDrawModel;
	}
}

/*
 desc : Mark Model ũ�� ���� �Է��� ���
 parm : id	- [in]  Edit Contro ID
 retn : None
*/
VOID CDlgMark::OnTypeClick(UINT32 id)
{
	switch (id)
	{
	case IDC_MARK_EDT_SIZE_1	:	;	break;
	case IDC_MARK_EDT_SIZE_2	:	;	break;
	case IDC_MARK_EDT_SIZE_3	:	;	break;
	case IDC_MARK_EDT_SIZE_4	:	;	break;
	}
}

/*
 desc : �Ϲ� ��ư Ŭ���� ���
 parm : id	- [in]  �Ϲ� ��ư ID
 retn : None
*/
VOID CDlgMark::OnBtnClick(UINT32 id) 
{
	UINT8 i	= 0x00;
	switch (id)
	{
	case IDC_MARK_BTN_MODEL_APPEND	:	ModelAppend(0x00);	break;
	case IDC_MARK_BTN_MODEL_DELETE	:	ModelAppend(0x01);	break;
	case IDC_MARK_BTN_MODEL_RESET	:	RecipeReset(0x00);	break;
	case IDC_MARK_BTN_MMF			:	OpenSelectMARK();	break;
	case IDC_MARK_BTN_MARK_IMG		:	RegistMarkImage();	break;

	case IDC_MARK_BTN_TRIG_CH		:	SetTriggerCh();	break;
	/* Mark Search (Match) */
	case IDC_MARK_BTN_MARK_FIND		:	SetMatchModel();							break;
	case IDC_MARK_BTN_EDGE_FIND		:	SetEdgeDetect();							break;
	case IDC_MARK_BTN_CONTROL_PANEL	:	ShowCtrlPanel();							break;
	case IDC_MARK_BTN_GAIN_SET		:
		
		//������ config���̽���
		for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++)
		{
			uvEng_GetConfig()->set_basler.cam_gain_level[i] = (UINT8)m_edt_out[eMARK_EDT_GAIN_LEVEL1+i].GetTextToNum();
			uvEng_Camera_SetGainLevel(i+1, uvEng_GetConfig()->set_basler.cam_gain_level[i]);
		}
		
		/*uvEng_GetConfig()->set_basler.cam_gain_level[0] = (UINT8)m_edt_out[eMARK_EDT_GAIN_LEVEL1].GetTextToNum();
		uvEng_GetConfig()->set_basler.cam_gain_level[1] = (UINT8)m_edt_out[eMARK_EDT_GAIN_LEVEL2].GetTextToNum();
		uvEng_GetConfig()->set_basler.cam_gain_level[2] = (UINT8)m_edt_out[eMARK_EDT_GAIN_LEVEL3].GetTextToNum();
		

		uvEng_Camera_SetGainLevel(0x01, uvEng_GetConfig()->set_basler.cam_gain_level[0]);
		uvEng_Camera_SetGainLevel(0x02, uvEng_GetConfig()->set_basler.cam_gain_level[1]);
		uvEng_Camera_SetGainLevel(0x03, uvEng_GetConfig()->set_basler.cam_gain_level[2]);*/
		
		uvEng_SaveConfig();
		break;
	case IDC_MARK_BTN_MARK_MODE		:	SetMarkFindMode(true);	break;
	case IDC_MARK_BTN_MARK_MERGE: MarkMerge(); 
		break;
	case IDC_MARK_BTN_CALIB_SET: setVisionCalib();
		break;
	case IDC_MARK_BTN_CALIB: VisionCalib();
		break;
	case IDC_MARK_BTN_STROBE_SET: 
		setStrobeValue();
		break;
	case IDC_MARK_BTN_STROBE_GET: 
		/*��Ʈ�κ꿡 ���� ���ð� Ȯ�� ��û*/
		
		int strobeRecved = GlobalVariables::GetInstance()->ResetCounter("strobeRecved");
		uvEng_StrobeLamp_Send_PageDataReadRequest(0);

		GlobalVariables::GetInstance()->Waiter("strobe", 
		[] 
		{
			return  GlobalVariables::GetInstance()->GetCount("strobeRecved") != 0;
		}, 
		[this]
		{
			UpdataStrobeView();
			MessageBoxEx(nullptr, _T("Read succeed.\t"), _T("ok"), MB_OK,LANG_ENGLISH);
		},
		[]
		{
			MessageBoxEx(nullptr, _T("Read failed. timeout\t"), _T("ok"), MB_OK, LANG_ENGLISH);
		},5000);

		//UpdataStrobeView();
	break;

	}
}

/*
 desc : �Ϲ� üũ Ŭ���� ���
 parm : id	- [in]  �Ϲ� ��ư ID
 retn : None
*/
VOID CDlgMark::OnChkClick(UINT32 id) 
{
	UINT8 i			= 0x00;
	BOOL bEnable	= FALSE/*, bEnableZ = m_chk_mvt[1].GetCheck() ? TRUE : FALSE*/;

	switch (id)
	{
	/* Select model type */
	case IDC_MARK_CHK_MODEL_CIRCLE	:
	case IDC_MARK_CHK_MODEL_RING	:
	case IDC_MARK_CHK_MODEL_CROSS	:
	case IDC_MARK_CHK_MODEL_RECTANGLE:
	case IDC_MARK_CHK_MODEL_IMG		:
	//case IDC_MARK_CHK_MODEL_MMF		:
	//case IDC_MARK_CHK_MODEL_PAT		:
		for (; i< eMARK_CHK_TYP_MAX; i++)	m_chk_typ[i].SetCheck(0);
		m_chk_typ[id - IDC_MARK_CHK_MODEL_CIRCLE].SetCheck(1);
		EnableModelSize();
		UpdateModelText();
		break;
	/* Select align camera */
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	case IDC_MARK_CHK_ACAM_1:
	case IDC_MARK_CHK_ACAM_2:
		for (i = 0x00; i < eMARK_CHK_CAM_ACAM_2 + 1; i++)	m_chk_cam[i].SetCheck(0);
		m_chk_cam[id - IDC_MARK_CHK_ACAM_1].SetCheck(1);
		break;
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	case IDC_MARK_CHK_ACAM_1:
	case IDC_MARK_CHK_ACAM_2:
	case IDC_MARK_CHK_ACAM_3:
		for (i = 0x00; i < eMARK_CHK_CAM_ACAM_3 + 1; i++)m_chk_cam[i].SetCheck(0);
		m_chk_cam[id - IDC_MARK_CHK_ACAM_1].SetCheck(1);
		break;

#endif
	case IDC_MARK_CHK_COLOR_BLACK	:
		m_chk_clr[eMARK_CHK_CLR_BLACK].SetCheck(1);
		m_chk_clr[eMARK_CHK_CLR_WHITE].SetCheck(0);
		m_chk_clr[eMARK_CHK_CLR_ANY].SetCheck(0);
		break;
	case IDC_MARK_CHK_COLOR_WHITE	:
		m_chk_clr[eMARK_CHK_CLR_BLACK].SetCheck(0);
		m_chk_clr[eMARK_CHK_CLR_WHITE].SetCheck(1);
		m_chk_clr[eMARK_CHK_CLR_ANY].SetCheck(0);
		break;
	case IDC_MARK_CHK_COLOR_ANY		:
		m_chk_clr[eMARK_CHK_CLR_BLACK].SetCheck(0);
		m_chk_clr[eMARK_CHK_CLR_WHITE].SetCheck(0);
		m_chk_clr[eMARK_CHK_CLR_ANY].SetCheck(1);
		break;

	case IDC_MARK_CHK_LIVE	:	SetLiveView();	break;

	case IDC_MARK_CHK_IMG_PROC: ProcImage(); break;
	}

}

/*
 desc : Mark�� Model Type ���
 parm : None
 retn : Model Type �� ��ȯ
*/
ENG_MMDT CDlgMark::GetModelType()
{
	ENG_MMDT enType	= ENG_MMDT::en_none;

	if (m_chk_typ[eMARK_CHK_TYP_MODEL_CIRCLE	].GetCheck())	enType	= ENG_MMDT::en_circle;	/* Circle */
	if (m_chk_typ[eMARK_CHK_TYP_MODEL_RING		].GetCheck())	enType	= ENG_MMDT::en_ring;	/* Ring */
	if (m_chk_typ[eMARK_CHK_TYP_MODEL_CROSS		].GetCheck())	enType	= ENG_MMDT::en_cross;	/* Cross */
	if (m_chk_typ[eMARK_CHK_TYP_MODEL_RECTANGLE ].GetCheck())	enType  = ENG_MMDT::en_rectangle;	/* Rectangle */
	if (m_chk_typ[eMARK_CHK_TYP_MODEL_IMAGE		].GetCheck())	enType	= ENG_MMDT::en_image;	/* MMF/PAT File */

	return enType;
}

/*
 desc : Recipe �׸��� ������ ���
 parm : nm_hdr	- [in]  Grid Control ������ ����� ����ü ������
		result	- [in]  fixed value (0)
 retn : None
*/
VOID CDlgMark::OnGridMarkList(NMHDR *nm_hdr, LRESULT *result) // lk91 �̻��.
{
	NM_GRIDVIEW *pstGrid= (NM_GRIDVIEW*)nm_hdr;
	LPG_RAAF pstRecipe	= m_pGridMark->GetRecipe(pstGrid->iRow);
	if (pstRecipe) {
		UpdateMark(pstRecipe);

		CFileFind	finder;
		CString		strDir;
		CUniToChar csCnv1, csCnv2;
		BOOL IsFind;
		strDir.Format(_T("%s\\%s\\pat\\%s.pat"), g_tzWorkDir, CUSTOM_DATA_CONFIG, csCnv1.Ansi2Uni(pstRecipe->m_name[0]));
		IsFind = finder.FindFile(strDir);

		uvCmn_Camera_SetMarkOffset(pstRecipe->acam_num[0], m_pDrawModel[0]->GetMarkOffsetP(), 3, TMP_MARK);
		uvCmn_Camera_SetMarkSize(pstRecipe->acam_num[0], m_pDrawModel[0]->GetMarkSizeP(), 1, TMP_MARK);
		uvCmn_Camera_SetMarkOffset(pstRecipe->acam_num[1], m_pDrawModel[2]->GetMarkOffsetP(), 3, TMP_MARK);
		uvCmn_Camera_SetMarkSize(pstRecipe->acam_num[1], m_pDrawModel[2]->GetMarkSizeP(), 1, TMP_MARK);

		Invalidate(TRUE);
	}

}

/*
 desc : Mark �׸��� ������ ���
 parm : nm_hdr	- [in]  Grid Control ������ ����� ����ü ������
		result	- [in]  fixed value (0)
 retn : None
*/
VOID CDlgMark::OnGridModelList(NMHDR* nm_hdr, LRESULT* result) 
{
	NM_GRIDVIEW* pstGrid = (NM_GRIDVIEW*)nm_hdr;
	LPG_CMPV pstRecipe = m_pGridModel->GetRecipe(pstGrid->iRow);
	CUniToChar csCnv;

	//UINT8 u8ACamID = m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
	UINT8 u8ACamID = CheckSelectCam();

	uvEng_Camera_SetMarkMethod(ENG_MMSM::en_single, 0);

	if (pstRecipe)
	{
		UpdateModel(pstRecipe);
		UpdateModelText();
		EnableModelSize();
		m_pGridModel->Select(csCnv.Ansi2Uni(pstRecipe->name));

		for (int i = 0; i < 1; i++)
		{
			m_pDrawModel[i]->ResetModel();
		}
		for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++) {
			SetMark(i);
		}

		UINT8 u8Speed = (UINT8)uvEng_GetConfig()->mark_find.model_speed;
		UINT8 u8Level = (UINT8)uvEng_GetConfig()->mark_find.detail_level;
		DOUBLE dbSmooth = (DOUBLE)uvEng_GetConfig()->mark_find.model_smooth;
		DOUBLE dbScaleMin = 0.0f, dbScaleMax = 0.0f, dbScoreRate;
		CUniToChar csCnv1, csCnv2;
		CDlgMesg dlgMesg;

		dbScoreRate = uvEng_GetConfig()->mark_find.score_rate;

		STG_CMPV stModel = { NULL }; // lk91 ���� ����, pstRecipe�� ����, file �̸������� ��� ��������� ���α� 

		BOOL IsFind = false;
		CFileFind	finder;
		CUniToChar csCnv3, csCnv4, csCnv5;

		if (ENG_MMDT(pstRecipe->type) != ENG_MMDT::en_image)
		{
			bmmfFile = true;
			bpatFile = false;
			stModel.type = (UINT32)m_pDrawModel[0]->GetModelType();		// lk91 m_pDrawModel[0] �����ϴ� ������ mark �ǿ����� �Ѱ��� ���(������ ��)
			for (int j = 0; j < 5; j++)	stModel.param[j] = m_pDrawModel[0]->GetParam(j);
			for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++) {
				uvEng_Camera_SetModelDefine_tot(i + 1, u8Speed, u8Level, uvEng_GetConfig()->mark_find.max_mark_find, dbSmooth,
					&stModel, TMP_MARK, csCnv2.Ansi2Uni(stModel.file),	
					dbScaleMin, dbScaleMax, dbScoreRate);
			}
		}
		else if (ENG_MMDT(pstRecipe->type) == ENG_MMDT::en_image) {
			if (wcslen(m_pDrawModel[0]->GetMMFFile()) > 0) {
				//sprintf_s(stModel.file, MARK_MODEL_NAME_LENGTH, _T("%s\\%s\\mmf\\%s.mmf"),
				//	csCnv1.Uni2Ansi(g_tzWorkDir), CUSTOM_DATA_CONFIG, csCnv2.Uni2Ansi(m_pDrawModel[0]->GetMMFFile()));
				sprintf_s(stModel.file, MARK_MODEL_NAME_LENGTH, "%s\\%s\\mmf\\%s.mmf",
					csCnv1.Uni2Ansi(g_tzWorkDir), CUSTOM_DATA_CONFIG2,csCnv2.Uni2Ansi(m_pDrawModel[0]->GetMMFFile()));
				IsFind = finder.FindFile(csCnv3.Ansi2Uni(stModel.file));
				if (IsFind)
					bmmfFile = true;
				else
					bmmfFile = false;
				if (bmmfFile) {
					stModel.type = (UINT32)m_pDrawModel[0]->GetModelType();
					stModel.iSizeP = m_pDrawModel[0]->GetMarkSizeP();
					stModel.iOffsetP = m_pDrawModel[0]->GetMarkOffsetP();
					for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++) {
						uvEng_Camera_SetModelDefine_tot(i + 1, u8Speed, u8Level, uvEng_GetConfig()->mark_find.max_mark_find, dbSmooth,
							&stModel, TMP_MARK, csCnv2.Ansi2Uni(stModel.file),	
							dbScaleMin, dbScaleMax, dbScoreRate);
					}
				}
			}
			if (wcslen(m_pDrawModel[0]->GetPATFile()) > 0) {
				//sprintf_s(stModel.file, MARK_MODEL_NAME_LENGTH, _T("%s\\%s\\pat\\%s.pat"),
				//	csCnv1.Uni2Ansi(g_tzWorkDir), CUSTOM_DATA_CONFIG, csCnv4.Uni2Ansi(m_pDrawModel[0]->GetPATFile()));
				sprintf_s(stModel.file, MARK_MODEL_NAME_LENGTH, "%s\\%s\\pat\\%s.pat",
					csCnv1.Uni2Ansi(g_tzWorkDir), CUSTOM_DATA_CONFIG2, csCnv4.Uni2Ansi(m_pDrawModel[0]->GetPATFile()));
				IsFind = finder.FindFile(csCnv5.Ansi2Uni(stModel.file));
				if (IsFind)
					bpatFile = true;
				else
					bpatFile = false;
				if (bpatFile) {
					stModel.type = (UINT32)m_pDrawModel[0]->GetModelType();
					stModel.iSizeP = m_pDrawModel[0]->GetMarkSizeP();
					stModel.iOffsetP = m_pDrawModel[0]->GetMarkOffsetP();
					for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++) {
						uvEng_Camera_SetModelDefine_tot(i + 1, u8Speed, u8Level, uvEng_GetConfig()->mark_find.max_mark_find, dbSmooth,
							&stModel, TMP_MARK, csCnv2.Ansi2Uni(stModel.file),	
							dbScaleMin, dbScaleMax, dbScoreRate);
					}
				}
			}
			if (!bpatFile && !bmmfFile) {
				dlgMesg.MyDoModal(L"Failed to load mark file(MMF & PAT)", 0x01);
				return;
			}
		}

		SetMarkFindMode(false);
	}
	Invalidate(TRUE);
}

/*
 desc : Mark Model �׸��� ������ ���
 parm : nm_hdr	- [in]  Grid Control ������ ����� ����ü ������
		result	- [in]  fixed value (0)
 retn : None
*/
VOID CDlgMark::OnGridMarkModel(NMHDR *nm_hdr, LRESULT *result)
{
	NM_GRIDVIEW *pstGrid= (NM_GRIDVIEW*)nm_hdr;
	LPG_CMPV pstRecipe	= m_pGridModelInMark->GetRecipe(pstGrid->iRow);
	CUniToChar csCnv;
	if (pstRecipe)
	{
		UpdateModel(pstRecipe);
		UpdateModelText();
		m_pGridModel->Select(csCnv.Ansi2Uni(pstRecipe->name));
	}
	Invalidate(TRUE);
}

/*
 desc : Recipe ������ ȭ�鿡 ����
 parm : recipe	- [in]  Gerber Recipe ������ ����� ����ü ������
 retn : None
*/
VOID CDlgMark::UpdateMark(LPG_RAAF recipe) // lk91 �̻��
{
	UINT8 i	= 0x00, u8ACam[2] = {NULL}, u8Index = 0x00;
	LPG_CMPV pstModel	= NULL;
	CUniToChar csCnv1, csCnv2;

	/* ���� ��Ȱ��ȭ */
	SetRedraw(FALSE);

	/* ���� ��ũ ���� ���� */
	for (i=0x00; i< 1; i++)	m_pDrawModel[i]->ResetModel();
	/* ���ο� ��ũ ������ ��Ʈ�ѿ� ���� */
	//for (i=0x00; i<recipe->save_count; i++)
	//{
	//	u8ACam[recipe->acam_num[i]-1]++;
	//	switch (recipe->acam_num[i])
	//	{
	//	case 0x01	:	u8Index	= u8ACam[recipe->acam_num[i]-1];		break;
	//	case 0x02	:	u8Index	= u8ACam[recipe->acam_num[i]-1] + 0x02;	break;
	//	}
	//	/* Recipe Name - Model Name �˻� */
	//	pstModel	= uvEng_Mark_GetModelName(csCnv1.Ansi2Uni(recipe->m_name[i]));
	//	if (pstModel)
	//	{
	//		if (ENG_MMDT::en_image != ENG_MMDT(pstModel->type))
	//		{
	//			m_pDrawModel[u8Index-1]->SetModel(csCnv1.Ansi2Uni(recipe->m_name[i]),
	//											  ENG_MMDT(pstModel->type),
	//											  recipe->acam_num[i], pstModel->param);
	//		}
	//		else
	//		{
	//			m_pDrawModel[u8Index-1]->SetModel(csCnv1.Ansi2Uni(recipe->m_name[i]),											  
	//											  recipe->acam_num[i], pstModel->find_type,
	//											  csCnv2.Ansi2Uni(pstModel->file),
	//												pstModel->iSizeP, pstModel->iOffsetP);
	//		}
	//	}
	//}
	/* Mark Recipe Name ��� */
	/* ���� Ȱ��ȭ */
	SetRedraw(TRUE);
	Invalidate(TRUE);

	/* Mark �����ǿ� ���Ե� Model �̸� ��� */
	m_pGridModelInMark->UpdateModels(recipe);
}

/*
 desc : Mark ������ ȭ�鿡 ����
 parm : recipe	- [in]  Gerber Recipe ������ ����� ����ü ������
 retn : None
*/
VOID CDlgMark::UpdateModel(LPG_CMPV recipe) 
{
	UINT8 i, u8Count;
	CUniToChar csCnv;

	m_edt_txt[eMARK_EDT_TXT_MODEL_NAME].SetTextToStr(csCnv.Ansi2Uni(recipe->name), TRUE);

	/* ��� üũ ��ư ���� */
	for (i=0x00; i< eMARK_CHK_TYP_MAX; i++)	m_chk_typ[i].SetCheck(0);
	/* Model ũ�� ��� */
	switch (ENG_MMDT(recipe->type))
	{
	case ENG_MMDT::en_circle	:	u8Count	= 1;	m_chk_typ[eMARK_CHK_TYP_MODEL_CIRCLE].SetCheck(1);	break;
	case ENG_MMDT::en_ring		:	u8Count	= 2;	m_chk_typ[eMARK_CHK_TYP_MODEL_RING	].SetCheck(1);	break;
	case ENG_MMDT::en_cross		:	u8Count	= 4;	m_chk_typ[eMARK_CHK_TYP_MODEL_CROSS	].SetCheck(1);	break;
	case ENG_MMDT::en_rectangle	:	u8Count = 2;	m_chk_typ[eMARK_CHK_TYP_MODEL_RECTANGLE].SetCheck(1);	break;
	case ENG_MMDT::en_image		:	u8Count	= 0;	m_chk_typ[eMARK_CHK_TYP_MODEL_IMAGE].SetCheck(1);	break;
	default						:	return;
	}

	m_btn_ctl[eMARK_BTN_CTL_MARK_IMG].EnableWindow(TRUE); 
	
	for (i=0x00; i< eMARK_CHK_CLR_MAX; i++)	m_chk_clr[i].SetCheck(0);
	if (u8Count)
	{
		m_chk_clr[eMARK_CHK_CLR_BLACK].EnableWindow(TRUE);
		m_chk_clr[eMARK_CHK_CLR_WHITE].EnableWindow(TRUE);
		m_chk_clr[eMARK_CHK_CLR_ANY].EnableWindow(TRUE);
		m_btn_ctl[eMARK_BTN_CTL_FILE_LOAD].EnableWindow(FALSE); 
		m_btn_ctl[eMARK_BTN_CTL_MARK_IMG].EnableWindow(FALSE); 
		
		if (recipe->param[0] == 1)		m_chk_clr[eMARK_CHK_CLR_BLACK	].SetCheck(1);
		else if(recipe->param[0] == 2)	m_chk_clr[eMARK_CHK_CLR_WHITE	].SetCheck(1);
		else							m_chk_clr[eMARK_CHK_CLR_ANY		].SetCheck(1);

		/* Model Size */
		for (i= eMARK_EDT_FLOAT_SIZE_1; i < u8Count; i++)
		{
			m_edt_flt[i].SetTextToNum(recipe->param[i+1], 1);
		}
		/* ������ Model Size ���� */
		for (; i< 4; i++)	m_edt_flt[i].SetTextToStr(L"", TRUE);
	}
	else
	{
		m_chk_clr[eMARK_CHK_CLR_BLACK	].EnableWindow(FALSE);
		m_chk_clr[eMARK_CHK_CLR_WHITE	].EnableWindow(FALSE);
		m_chk_clr[eMARK_CHK_CLR_ANY		].EnableWindow(FALSE);
		m_btn_ctl[eMARK_BTN_CTL_FILE_LOAD].EnableWindow(TRUE);
		m_btn_ctl[eMARK_BTN_CTL_MARK_IMG].EnableWindow(TRUE);

		for (i=0x00; i<4; i++)	m_edt_flt[i].SetTextToStr(L"", TRUE);
		OutputFileMARK(csCnv.Ansi2Uni(recipe->file));
	}
}

/*
 desc : ���� ���� ���� ��� �ʱ�ȭ
 parm : mode	- [in]  0x00: Only Model, 0x01: All
 retn : None
*/
VOID CDlgMark::RecipeReset(UINT8 mode)
{
	UINT8 i = 0x00;

	/* Recipe & Model Name �ʱ�ȭ */
	m_edt_txt[eMARK_EDT_TXT_MODEL_NAME].SetTextToStr(L"", TRUE);
	/* Model Size �ʱ�ȭ */
	for (i=0x00; i<4; i++)	m_edt_flt[i].SetTextToStr(L"", TRUE);
	/* ���� ��Ȱ��ȭ */
	SetRedraw(FALSE);
	for (i=0x00; mode && i<4; i++)	m_pDrawModel[i]->ResetModel();
	/* ���� Ȱ��ȭ */
	SetRedraw(TRUE);
	Invalidate(TRUE);

	/* ModelInMark �׸��� �ʱ�ȭ */
	m_pGridModelInMark->UpdateModels(NULL);
}

/*
 desc : Recipe ���/����
 parm : mode	- [in]  0x00:Append, 0x01:Delete
 retn : None
*/
VOID CDlgMark::MarkAppend(UINT8 mode) // lk91 �̻��
{
	UINT8 i = 0;
	UINT8 j = 0;
	TCHAR tzName[MARK_MODEL_NAME_LENGTH] = {NULL};
	STG_RAAF stRecipe	= {NULL};
	CUniToChar csCnv;
	CDlgMesg dlgMesg;


	/* Mark Recipe ��� */
	if (0x00 == mode)
	{
		if (!IsSetValidMark())	return;

		/* �⺻������ �ִ� 4���� �����ϵ��� �� */
		for (i=0x00,j=0x00; i<4; i++)
		{
			if (m_pDrawModel[i]->IsSetModel())	j++;
		}
		stRecipe.Init(j);
		/* Mark Type (Fixed 0x00) */
		stRecipe.mark_type	= 0x00;	/* 0x00:Geometric, 0x01:Pattern Image */
		strcpy_s(stRecipe.align_name, MARK_MODEL_NAME_LENGTH, csCnv.Uni2Ansi(tzName));

		/* Model Name, Align Camera Number */
		for (i=0x00,j=0x00; i<4; i++)
		{
			if (m_pDrawModel[i]->IsSetModel())
			{
				/* Mark Model Name ���� �Ҵ� �� ���� */
				strcpy_s(stRecipe.m_name[j], MARK_MODEL_NAME_LENGTH, csCnv.Uni2Ansi(m_pDrawModel[i]->GetModelName()));	
				stRecipe.acam_num[j]= m_pDrawModel[i]->GetACamID();
				j++;
			}
		}
		/* Mark Recipe ��� (���Ͽ� ����) */
		uvEng_Mark_AlignRecipeAppend(&stRecipe, 0x00);

		/* �޸� ���� */
		stRecipe.Close();
	}
	/* Mark Recipe ���� */
	else
	{
		/* Mark Recipe ���� (���Ͽ��� ����) */
		if (!uvEng_Mark_AlignRecipeDelete(tzName))
		{
			dlgMesg.MyDoModal(L"Failed to delete the mark recipe", 0x01);
		}
		else
		{
			/* ���� ���õ� �����ǰ� �����Ǿ��ٸ� ... */
			if (uvEng_Mark_GetSelectAlignRecipe())
			{
				if (0 == wcscmp(csCnv.Ansi2Uni(uvEng_Mark_GetSelectAlignRecipe()->align_name), tzName))
				{
					uvEng_MarkSelAlignRecipeReset();
				}
			}
		}
	}

	/* �ٽ� ��ũ ���� �ҷ� ���� */
	m_pGridMark->Load();
	/* �ʱ�ȭ */
	RecipeReset(0x01);
}

/*
 desc : Mark ���/����
 parm : mode	- [in]  0x00:Append, 0x01:Delete
 retn : None
*/
VOID CDlgMark::ModelAppend(UINT8 mode) 
{
	TCHAR tzName[MARK_MODEL_NAME_LENGTH] = {NULL};
	TCHAR tzFile[MARK_MODEL_NAME_LENGTH] = {NULL};
	STG_CMPV stRecipe	= {NULL};
	CUniToChar csCnv;

	/* Recipe Name */
	//m_edt_txt[1].GetWindowTextW(tzName, MARK_MODEL_NAME_LENGTH);
	m_edt_txt[eMARK_EDT_TXT_MODEL_NAME].GetWindowTextW(tzName, MARK_MODEL_NAME_LENGTH);
	strcpy_s(stRecipe.name, MARK_MODEL_NAME_LENGTH, csCnv.Uni2Ansi(tzName));
	/* Color */
	//stRecipe.param[0]	= 128.0f;	/* White */ 
	//if (m_chk_clr[eMARK_CHK_CLR_BLACK	].GetCheck())	stRecipe.param[0]	= 256.0f;	/* Black */
	if		(m_chk_clr[eMARK_CHK_CLR_BLACK	].GetCheck())		stRecipe.param[0] = 1;	
	else if (m_chk_clr[eMARK_CHK_CLR_WHITE	].GetCheck())		stRecipe.param[0] = 2;
	else if (m_chk_clr[eMARK_CHK_CLR_ANY	].GetCheck())		stRecipe.param[0] = 3;

 
	/* Model Type */
	stRecipe.type	= (UINT32)GetModelType();
	if (ENG_MMDT(stRecipe.type) != ENG_MMDT::en_image)
	{
		/* Model Size */
		stRecipe.param[1]	= m_edt_flt[eMARK_EDT_FLOAT_SIZE_1].GetTextToDouble();
		stRecipe.param[2]	= m_edt_flt[eMARK_EDT_FLOAT_SIZE_2].GetTextToDouble();
		stRecipe.param[3]	= m_edt_flt[eMARK_EDT_FLOAT_SIZE_3].GetTextToDouble();
		stRecipe.param[4]	= m_edt_flt[eMARK_EDT_FLOAT_SIZE_4].GetTextToDouble();
	}
	else
	{
		m_edt_ctl[eMARK_EDT_CTL_MMF].GetWindowTextW(tzFile, MARK_MODEL_NAME_LENGTH);
		strcpy_s(stRecipe.file, MARK_MODEL_NAME_LENGTH, csCnv.Uni2Ansi(tzFile));

		TCHAR* tcharFileName = _T("");
		CUniToChar csCnv;

		CFileFind	finder;
		CString		strDir;
		//CUniToChar csCnv1, csCnv2;
		BOOL IsFind;
		int tmpfindType;
		strDir.Format(_T("%s\\%s\\mmf\\%s.mmf"), g_tzWorkDir, CUSTOM_DATA_CONFIG, tzFile);
		IsFind = finder.FindFile(strDir);
		if (IsFind) {
			// CString -> TCHAR * 
			tcharFileName = (TCHAR*)(LPCTSTR)strDir;
			tmpfindType = 0;
		}
		else {
			strDir.Format(_T("%s\\%s\\pat\\%s.pat"), g_tzWorkDir, CUSTOM_DATA_CONFIG, tzFile);
			IsFind = finder.FindFile(strDir);
			if (IsFind) {
				// CString -> TCHAR * 
				tcharFileName = (TCHAR*)(LPCTSTR)strDir;
				tmpfindType = 1;
			}
		}
		if (IsFind) {
			//UINT8 u8ACamID = m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
			UINT8 u8ACamID = CheckSelectCam();
			CDPoint tSizeP;

			uvCmn_Camera_InitSetMarkSizeOffset(u8ACamID, tcharFileName, tmpfindType, TMP_MARK);
			stRecipe.iSizeP = uvCmn_Camera_GetMarkSize(u8ACamID, TMP_MARK);
			stRecipe.iOffsetP = uvCmn_Camera_GetMarkOffset(u8ACamID, false, TMP_MARK);

		}
	}

	/* �� ��� */
	if (0x00 == mode) {
		uvEng_Mark_ModelAppend(&stRecipe, 0x00);

		// lk91 mmf ���� ���� �߰�
		UINT8 u8ACamID = CheckSelectCam();
		TCHAR SaveFileName[1000];

		BOOL IsFind = false;
		CFileFind	finder;
		CUniToChar csCnv1, csCnv2;

		UINT8 u8Speed = (UINT8)uvEng_GetConfig()->mark_find.model_speed;
		UINT8 u8Level = (UINT8)uvEng_GetConfig()->mark_find.detail_level;
		DOUBLE dbSmooth = (DOUBLE)uvEng_GetConfig()->mark_find.model_smooth;
		DOUBLE dbScaleMin = 0.0f, dbScaleMax = 0.0f, dbScoreRate = 70.0f;

		if (ENG_MMDT(stRecipe.type) != ENG_MMDT::en_image)
		{
			bmmfFile = true;
			bpatFile = false;
			//for (int j = 0; j < 5; j++)	stRecipe.param[j] = m_pDrawModel[0]->GetParam(j);
			for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++) {
				uvEng_Camera_SetModelDefine_tot(i + 1, u8Speed, u8Level, uvEng_GetConfig()->mark_find.max_mark_find, dbSmooth,
					&stRecipe, TMP_MARK, csCnv1.Ansi2Uni(stRecipe.file),
					dbScaleMin, dbScaleMax, dbScoreRate);
			}
		}
		
		//CString strFileName = _T("");

		//m_edt_txt[eMARK_EDT_TXT_MODEL_NAME].GetWindowTextW(strFileName);

		//// �ؽ�Ʈ �����Ͱ� �������.
		//strFileName = strFileName.Trim();
		//if (strFileName.IsEmpty())
		//{
		//	return;
		//}

		_stprintf_s(SaveFileName, 1000, _T("%s\\%s\\mmf\\%s.mmf"), g_tzWorkDir, CUSTOM_DATA_CONFIG, csCnv2.Ansi2Uni(stRecipe.name));
		uvCmn_Camera_ModMarkSave(u8ACamID, SaveFileName, TMP_MARK);
	}
	else {
		if (uvEng_Mark_ModelDelete(tzName))
		{
			for (int i = 0; i < 1; i++)
			{
				m_pDrawModel[i]->ResetModel();
			}

			m_edt_ctl[eMARK_EDT_CTL_MMF].GetWindowTextW(tzFile, MARK_MODEL_NAME_LENGTH);

			CFileFind	finder;
			CString		strDir;
			//CUniToChar csCnv1, csCnv2;
			BOOL IsFind;
			int tmpfindType;
			strDir.Format(_T("%s\\%s\\mmf\\%s.mmf"), g_tzWorkDir, CUSTOM_DATA_CONFIG, tzFile);
			IsFind = finder.FindFile(strDir);
			if (IsFind) {
				if (!DeleteFile(strDir)) {
					CDlgMesg dlgMesg;
					dlgMesg.MyDoModal(L"Failed to delete the mmf mark file", 0x01);
				}

			}
			strDir.Format(_T("%s\\%s\\pat\\%s.pat"), g_tzWorkDir, CUSTOM_DATA_CONFIG, tzFile);
			IsFind = finder.FindFile(strDir);
			if (IsFind) {
				if (!DeleteFile(strDir)) {
					CDlgMesg dlgMesg;
					dlgMesg.MyDoModal(L"Failed to delete the pat mark file", 0x01);
				}

			}
		}
		
	}
	/* �ٽ� �� ���� �ҷ� ���� */
	m_pGridModel->Load();
	/* �ʱ�ȭ */
	RecipeReset(0x00);
}

/*
 desc : �� ũ�� ��Ʈ�� (Edit Box) ��� ���� ����
 parm : None
 retn : None
*/
VOID CDlgMark::EnableModelSize() 
{
	UINT8 i	= 0x00, u8Mask = 0x00;

	/* �ϴ�, ������ 4�� ��ũ ũ�� ��Ʈ�� ��Ȱ��ȭ */
	for (i=0; i<4; i++)	m_edt_flt[i].SetDisableKBDN();
	if (m_chk_typ[eMARK_CHK_TYP_MODEL_CIRCLE].GetCheck())	u8Mask	|= 0x01;	/* Circle */
	if (m_chk_typ[eMARK_CHK_TYP_MODEL_RING].GetCheck())	u8Mask	|= 0x03;	/* Ring */
	if (m_chk_typ[eMARK_CHK_TYP_MODEL_CROSS].GetCheck())	u8Mask	|= 0x07;	/* Cross */
	if (m_chk_typ[eMARK_CHK_TYP_MODEL_RECTANGLE].GetCheck())	u8Mask |= 0x03;	/* Rectangle */

	if (m_chk_typ[eMARK_CHK_TYP_MODEL_IMAGE].GetCheck())
	{
		m_chk_clr[eMARK_CHK_CLR_BLACK	].EnableWindow(FALSE);
		m_chk_clr[eMARK_CHK_CLR_WHITE	].EnableWindow(FALSE);
		m_chk_clr[eMARK_CHK_CLR_ANY	].EnableWindow(FALSE);
		m_btn_ctl[eMARK_BTN_CTL_FILE_LOAD].EnableWindow(TRUE);
		m_btn_ctl[eMARK_BTN_CTL_MARK_IMG].EnableWindow(TRUE);
	}
	else
	{
		m_chk_clr[eMARK_CHK_CLR_BLACK	].EnableWindow(TRUE);
		m_chk_clr[eMARK_CHK_CLR_WHITE	].EnableWindow(TRUE);
		m_chk_clr[eMARK_CHK_CLR_ANY	].EnableWindow(TRUE);
		m_btn_ctl[eMARK_BTN_CTL_FILE_LOAD].EnableWindow(FALSE);
		m_btn_ctl[eMARK_BTN_CTL_MARK_IMG].EnableWindow(FALSE);
	}

	if (0x01 == (u8Mask & 0x01))	m_edt_flt[eMARK_EDT_FLOAT_SIZE_1].SetEnableKBDN();
	if (0x02 == (u8Mask & 0x02))	m_edt_flt[eMARK_EDT_FLOAT_SIZE_2].SetEnableKBDN();
	if (0x04 == (u8Mask & 0x04))
	{
		m_edt_flt[eMARK_EDT_FLOAT_SIZE_3].SetEnableKBDN();
		m_edt_flt[eMARK_EDT_FLOAT_SIZE_4].SetEnableKBDN();
	}
}

/*
 desc : �� ũ�� ��Ʈ���� �ؽ�Ʈ ����
 parm : None
 retn : None
*/
VOID CDlgMark::UpdateModelText()
{
	UINT8 i	= 0;

	/* ��Ʈ�� ���� ��Ȱ��ȭ */
	for (i=0; i<= eMARK_TXT_SIZE_4; i++)	m_txt_ctl[i].SetRedraw(FALSE);
	
	if (m_chk_typ[eMARK_CHK_TYP_MODEL_CIRCLE].GetCheck())
	{
		m_txt_ctl[eMARK_TXT_SIZE_1].SetWindowTextW(L"Circle Size(um)");
		for (i= eMARK_TXT_SIZE_2; i<= eMARK_TXT_SIZE_4; i++)
		{
			m_txt_ctl[i].SetWindowTextW(L"Not used(um)");
		}
		for (i = eMARK_EDT_FLOAT_SIZE_2; i <= eMARK_EDT_FLOAT_SIZE_4; i++)
		{
			m_edt_flt[i].SetWindowTextW(L"");
		}
		m_edt_ctl[eMARK_EDT_CTL_MMF].SetWindowTextW(L"");
	}
	else if (m_chk_typ[eMARK_CHK_TYP_MODEL_RING].GetCheck())
	{
		m_txt_ctl[eMARK_TXT_SIZE_1].SetWindowTextW(L"Inner Size(um)");
		m_txt_ctl[eMARK_TXT_SIZE_2].SetWindowTextW(L"Outer Size(um)");
		for (i=2; i<4; i++)	m_txt_ctl[i].SetWindowTextW(L"Not used(um)");
		m_edt_ctl[eMARK_EDT_CTL_MMF].SetWindowTextW(L"");
	}
	else if (m_chk_typ[eMARK_CHK_TYP_MODEL_CROSS].GetCheck())
	{
		m_txt_ctl[eMARK_TXT_SIZE_1].SetWindowTextW(L"Width Size(um)");
		m_txt_ctl[eMARK_TXT_SIZE_2].SetWindowTextW(L"Height Size(um)");
		m_txt_ctl[eMARK_TXT_SIZE_3].SetWindowTextW(L"Horz. Size(um)");
		m_txt_ctl[eMARK_TXT_SIZE_4].SetWindowTextW(L"Vert. Size(um)");
		m_edt_ctl[eMARK_EDT_CTL_MMF].SetWindowTextW(L"");
	}
	else if (m_chk_typ[eMARK_CHK_TYP_MODEL_RECTANGLE].GetCheck())
	{
		m_txt_ctl[eMARK_TXT_SIZE_1].SetWindowTextW(L"Width Size(um)");
		m_txt_ctl[eMARK_TXT_SIZE_2].SetWindowTextW(L"Height Size(um)");
		for (i = 2; i < 4; i++)	m_txt_ctl[i].SetWindowTextW(L"Not used(um)");
		m_edt_ctl[eMARK_EDT_CTL_MMF].SetWindowTextW(L"");
	}
	else
	{
		for (i = eMARK_TXT_SIZE_1; i <= eMARK_TXT_SIZE_4; i++)
		{
			m_txt_ctl[i].SetWindowTextW(L"Not used(um)");
		}
		for (i = eMARK_EDT_FLOAT_SIZE_1; i <= eMARK_EDT_FLOAT_SIZE_4; i++)
		{
			m_edt_flt[i].SetWindowTextW(L"");
		}
	}

	/* ��Ʈ�� ���� ��Ȱ��ȭ */
	for (i = eMARK_TXT_SIZE_1; i <= eMARK_TXT_SIZE_4; i++)
	{
		m_txt_ctl[i].SetRedraw(TRUE);
		m_txt_ctl[i].Invalidate(FALSE);
	}
}

/*
 desc : �ʱ� �� ����
 parm : None
 retn : None
*/
VOID CDlgMark::InitValue()
{

}

/*
 desc : �⺻ ������ ���� �� ��Ʈ�ѿ� ����
 parm : None
 retn : None
*/
VOID CDlgMark::SetBaseData()
{
	//LPG_RAAF pstRecipe	= uvEng_Mark_GetSelectAlignRecipe();
	//CUniToChar csCnv;
	//if (pstRecipe)
	//{
	//	UpdateMark(pstRecipe);
	//	m_pGridMark->Select(csCnv.Ansi2Uni(pstRecipe->align_name));
	//}
	//else
	//{
		m_chk_typ[eMARK_CHK_TYP_MODEL_CIRCLE].SetCheck(1);
		m_chk_clr[eMARK_CHK_CLR_BLACK	].SetCheck(1);
		UpdateModelText();
		EnableModelSize();
	//}
}

/*
 desc : ���� ������ ���� ������ ȭ�� ���� �� �����ͷ� ���� (���� ����)
 parm : index	- [in]  0 : Camera 1 / Mark 1, 1 : Camera 1 / Mark 2
						2 : Camera 2 / Mark 1, 3 : Camera 2 / Mark 2
 retn : None
*/
VOID CDlgMark::SetMark(UINT8 index) 
{
	//UINT8 u8ACamID		= index < 2 ? 0x01 : 0x02;
	UINT8 u8ACamID = index + 1;
	LPG_CMPV pstRecipe	= m_pGridModel->GetSelected();
	CUniToChar csCnv, csCnv2;

	if (!pstRecipe)	return;

	/* ���� ��Ȱ��ȭ */
	SetRedraw(FALSE);

	/* �� ���� �� ȭ�� ���� */
	if (ENG_MMDT::en_image != ENG_MMDT(pstRecipe->type))
	{
		m_pDrawModel[0]->SetModel(csCnv.Ansi2Uni(pstRecipe->name),
									  ENG_MMDT(pstRecipe->type), u8ACamID,
									  pstRecipe->param);
	}
	else {
		m_pDrawModel[0]->SetModel(csCnv.Ansi2Uni(pstRecipe->name),
			u8ACamID, csCnv2.Ansi2Uni(pstRecipe->file), pstRecipe->iSizeP, pstRecipe->iOffsetP);

		uvCmn_Camera_SetMarkOffset(u8ACamID, pstRecipe->iOffsetP, 3, TMP_MARK);
		uvCmn_Camera_SetMarkSize(u8ACamID, pstRecipe->iSizeP, 1, TMP_MARK);

	}
	/* ���� Ȱ��ȭ */
	SetRedraw(TRUE);
	Invalidate(TRUE);
}

/*
 desc : Motion �̵�
 parm : direct	- [in]  ENG_MDMD ��
 retn : None
*/
VOID CDlgMark::SetMotionMoving(ENG_MDMD direct)
{
// 	UINT8 u8MoveMethod	= 0x00, u8ACamID = 0x00, u8UpDown = 0x00;
// 	BOOL bMotionCtrl	= m_chk_mvt[1].GetCheck() == 0 ? TRUE : FALSE;
// 	DOUBLE dbMotorSpeed	= 0.0f;
// 	DOUBLE dbMotorDist	= 0.0f;
// 	DOUBLE dbMotorPos	= 0.0f;	/* StageX, StageY, ACam X1, ACamX2 */
// 	ENG_MMDI enDrvID	= ENG_MMDI::en_axis_none;
// 
// 	/* �̵� �Ÿ��� �������� ���������� ���� */
// 	u8MoveMethod	= (UINT8)m_chk_mvt[0].GetCheck();	/* 0:Absolute, 1:Relative */
// 	/* �Էµ� �ӵ� �� �̵� �Ÿ� ��� */
// 	dbMotorSpeed	= m_edt_flt[4].GetTextToDouble();	/* Speed */
// 	dbMotorDist		= m_edt_flt[5].GetTextToDouble();	/* Distance */
// 
// 	if (bMotionCtrl)
// 	{
// 		/* ���� ���õ� ����̺� ���� ��� */
// 		if (enDrvID == ENG_MMDI::en_axis_none)	return;
// 		/* ���� Motion ��ġ ��� */
// 		dbMotorPos	= uvCmn_MC2_GetDrvAbsPos(enDrvID);
// 		/* ��� �̵� �Ÿ��� ���, ���� ��ġ�� �̵� �Ÿ� ����ŭ �ջ��� �� */
// 		if (u8MoveMethod)
// 		{
// 			switch (enDrvID)
// 			{
// 			case ENG_MMDI::en_stage_x		:
// 				if (direct == ENG_MDMD::en_move_left)	dbMotorPos	+= dbMotorDist;			/* left */
// 				else									dbMotorPos	-= dbMotorDist;	break;	/* right */
// 			case ENG_MMDI::en_stage_y		:
// 				if (direct == ENG_MDMD::en_move_up)		dbMotorPos	+= dbMotorDist;			/* up */
// 				else									dbMotorPos	-= dbMotorDist;	break;	/* down */
// 			case ENG_MMDI::en_align_cam1	:
// 			case ENG_MMDI::en_align_cam2	:
// 				if (direct == ENG_MDMD::en_move_left)	dbMotorPos	-= dbMotorDist;			/* left */
// 				else									dbMotorPos	+= dbMotorDist;	break;	/* right */
// 			}
// 		}
// 		else
// 		{
// 			dbMotorPos	= dbMotorDist;
// 		}
// 		/* �뱤 �̵� ��ġ ���� Min / Max ����� �ȵǹǷ�, Min / Max ������ �ڵ� ���� */
// 		if (dbMotorPos < uvEng_GetConfig()->mc2_svc.min_dist[UINT8(enDrvID)])
// 		{
// 			dbMotorPos	= uvEng_GetConfig()->mc2_svc.min_dist[UINT8(enDrvID)];
// 		}
// 		if (dbMotorPos > uvEng_GetConfig()->mc2_svc.max_dist[UINT8(enDrvID)])
// 		{
// 			dbMotorPos	= uvEng_GetConfig()->mc2_svc.max_dist[UINT8(enDrvID)];
// 		}
// 		/* ���� ��ġ�� �̵� */
// 		uvEng_MC2_SendDevAbsMove(enDrvID, dbMotorPos, dbMotorSpeed);
// 	}
// 	/* ī�޶� Z �� �̵� */
// 	else
// 	{
// 		/* ��� Ȥ�� ���� �̵� ���� ����, �ϴ� �̵� �Ÿ� �� ���� */
// 		dbMotorPos	= dbMotorDist;
// 		/* ��� �̵� �Ÿ��� ���, ���� ��ġ�� �̵� �Ÿ� ����ŭ �ջ��� �� */
// 		if (u8MoveMethod)
// 		{
// 			if (direct == ENG_MDMD::en_move_down)	u8UpDown	= 0x02;	/* DOWN */
// 			else									u8UpDown	= 0x01;	/* UP */
// 		}
// 		/* ���� ��ġ�� �̵� */
// 		uvEng_MCQ_SetACamMovePosZ(u8ACamID, u8MoveMethod, dbMotorPos, u8UpDown);
// 	}
}

/*
 desc : Recipe ����
 parm : None
 retn : None
*/
VOID CDlgMark::SelectMarkRecipe() // lk91 �̻��.
{
//	UINT8 i = 0x00, j	= 0, u8ACamID, u8SetOK = 0x01;
//	UINT8 u8Speed		= (UINT8)uvEng_GetConfig()->mark_find.model_speed;
//	UINT8 u8Level		= (UINT8)uvEng_GetConfig()->mark_find.detail_level;
//	DOUBLE dbSmooth		= (DOUBLE)uvEng_GetConfig()->mark_find.model_smooth; 
//	DOUBLE dbScaleMin	= 0.0f, dbScaleMax = 0.0f, dbScoreRate;
//	CAtlList <STG_CMPV> lstMark[2]; // mark ���� 
//	STG_CMPV stModel		= {NULL}, stMark[2] = {NULL};
//	LPG_RAAF pstRecipeMark	= m_pGridMark->GetSelected();
//	LPG_RJAF pstRecipeJob	= uvEng_JobRecipe_GetSelectRecipe();
//	LPG_REAF pstRecipeExpo	= uvEng_ExpoRecipe_GetSelectRecipe();
//	LPG_RAAF pstRecipeAlign = uvEng_Mark_GetSelectAlignRecipe();
//	CUniToChar csCnv1, csCnv2;
//	CDlgMesg dlgMesg;
//
//	if (!pstRecipeMark)
//	{
//		dlgMesg.MyDoModal(L"There is no recipe [mark] selected", 0x01);
//		return;
//	}
//	if (!IsSetValidMark())	return;
//	/* ���� ��ϵ� �Ź� (���õ� �Ź�)�ִٸ�, ��ũ �˻� ��� ���� ���� */
//	if (pstRecipeAlign && pstRecipeExpo)
//	{
//		// by sysandj : ��������(����)
//		uvEng_Camera_SetMarkMethod(ENG_MMSM(pstRecipeAlign->search_type), pstRecipeAlign->search_count);
//		/* ��ϵ� ��ũ ���� ũ�⸦ 1.0���� ����, scale_range��ŭ �����ͺ��� ū �ͱ��� �˻��϶�� ���� �� */
//		dbScaleMin	= 1.0f - (100.0f - pstRecipeExpo->mark_scale_range)/100.0f;
//		dbScaleMax	= 1.0f + (100.0f - pstRecipeExpo->mark_scale_range)/100.0f;
//	}
//	else
//	{
//		uvEng_Camera_SetMarkMethod(ENG_MMSM::en_single, 0x01);
//	}
//
//	/* Align Mark Basler Camera ���� MIL ���̺귯���� ��� */
//	for (i=0x00; i<4; i++)
//	{
//		if (m_pDrawModel[i]->IsSetModel())
//		{
//			/* ��� ��� �� �ʱ�ȭ */
//			stModel.Reset();
//			/* Mark Model & File (mmf) ���� ���� */
//			strcpy_s(stModel.name, MARK_MODEL_NAME_LENGTH, csCnv1.Uni2Ansi(m_pDrawModel[i]->GetModelName()));
//// 			if (((CButton*)GetDlgItem(IDC_MARK_RAD_M))->GetCheck())
//// 			{
//// 				if (wcslen(m_pDrawModel[i]->GetMMFFile()) > 0)
//// 				{
//// 					sprintf_s(stModel.file, MARK_MODEL_NAME_LENGTH, "%s\\data\\mmf\\%s.mmf",
//// 							   csCnv1.Uni2Ansi(g_tzWorkDir), csCnv2.Uni2Ansi(m_pDrawModel[i]->GetMMFFile()));
//// 
//// 					//m_pDrawModel[i]->SetFindType();
//// 					stModel.find_type = 0;
//// 				}
//// 			}
//// 			else if (((CButton*)GetDlgItem(IDC_MARK_RAD_P))->GetCheck())
//// 			{
//// 				if (wcslen(m_pDrawModel[i]->GetPATFile()) > 0)	
//// 				{
//// 					sprintf_s(stModel.file, MARK_MODEL_NAME_LENGTH, "%s\\data\\pat\\%s.pat",
//// 						csCnv1.Uni2Ansi(g_tzWorkDir), csCnv2.Uni2Ansi(m_pDrawModel[i]->GetPATFile()));
//// 					stModel.find_type = 1;
//// 				}
//// 			}
//			stModel.type	= (UINT32)m_pDrawModel[i]->GetModelType();
//			//if (stModel.find_type == 1 || ENG_MMDT(stModel.type) == ENG_MMDT::en_image) {
//			if (ENG_MMDT(stModel.type) == ENG_MMDT::en_image) {
//				stModel.iSizeP = m_pDrawModel[i]->GetMarkSizeP();
//				stModel.iOffsetP = m_pDrawModel[i]->GetMarkOffsetP();
//			}
//			else {
//				for (j=0; j<5; j++)	stModel.param[j] = m_pDrawModel[i]->GetParam(j);
//			}
//			/* ī�޶� ���� ��ũ �ӽ� ��� */
//			u8ACamID	= m_pDrawModel[i]->GetACamID();
//			if (u8ACamID > 0 && u8ACamID < 3)	lstMark[u8ACamID-1].AddTail(stModel);	
//		}
//	}
//	/* �ּ��� Align Camera ���� �� 1������ ��ϵǾ� �־�� �� */
//	if (lstMark[0].GetCount() < 1 || lstMark[1].GetCount() < 1)
//	{
//		dlgMesg.MyDoModal(L"The model is not registered for two alignment cameras.", 0x01);
//		return;
//	}
//	/* Mark Model ��� (for Mil Library) */
//	for (i=0x00; i<0x02; i++)	/* ī�޶� 2���� ���ؼ� ó�� */
//	{
//		/* �Ʒ����� ����� ��, �� Ÿ�� ��, �������� �� ������ �� ȥ�� �Ǿ� ������ ���� ó�� (���� ����) */
//		for (j=0x00; j<lstMark[i].GetCount(); j++)	
//		{
//			stMark[j].Reset();
//			stModel	= lstMark[i].GetAt(lstMark[i].FindIndex(j));
//			memcpy(&stMark[j], &stModel, sizeof(STG_CMPV)); 
//		}
//#if 0
//		if (!pstRecipeJob)	dbScoreRate	= uvEng_GetConfig()->mark_find.score_rate;
//		else				dbScoreRate	= pstRecipeJob->mark_score_accept;
//#else
//		dbScoreRate	= uvEng_GetConfig()->mark_find.score_rate;
//		if (pstRecipeExpo)
//		{
//			if (dbScoreRate > pstRecipeExpo->mark_score_accept)
//				dbScoreRate	= pstRecipeExpo->mark_score_accept;
//		}
//#endif
//		if (u8SetOK)
//		{
//			/* Model Name�� ��� */
//			//if (ENG_MMDT(stMark[0].type) != ENG_MMDT::en_image && stMark[0].find_type == 0) 
//			if (ENG_MMDT(stMark[0].type) != ENG_MMDT::en_image)
//			{
//				if (!uvEng_Camera_SetModelDefine(i+1, u8Speed, u8Level, dbSmooth,		// Recipe Select �Ҷ�, Model Define
//												 stMark, (UINT8)lstMark[i].GetCount(),
//												 dbScaleMin, dbScaleMax, dbScoreRate))
//				{
//					dlgMesg.MyDoModal(L"Failed to regist the mark model for align carmer", 0x01);
//					u8SetOK	= 0x00;
//				}
//			}
//			/* PAT ������ ��� */
//			else if (stMark[0].find_type == 1) {
//				uvEng_Camera_SetModelDefinePAT(i + 1,		// Recipe Select �Ҷ�, Model Define
//					csCnv1.Ansi2Uni(stMark[0].name),
//					csCnv2.Ansi2Uni(stMark[0].file),
//					stMark[0].iSizeP,
//					stMark[0].iOffsetP, 2);
//			}
//			/* MMF ������ ��� */
//			else
//			{
//				uvEng_Camera_SetModelDefineMMF(i+1,		// Recipe Select �Ҷ�, Model Define
//											   csCnv1.Ansi2Uni(stMark[0].name),
//											   csCnv2.Ansi2Uni(stMark[0].file),
//												stMark[0].iSizeP,
//												stMark[0].iOffsetP, 2);
//			}
//		}
//		lstMark[i].RemoveAll();
//	}
//	if (!u8SetOK)	return;
//
//	/* ���������� ���� �޸𸮿� ���õ� ��ũ ������ pstRecipeMark ���� */
//	if (!uvEng_Mark_SelAlignRecipeName(csCnv1.Ansi2Uni(pstRecipeMark->align_name)))
//	{
//		dlgMesg.MyDoModal(L"Failed to select the recipe for mark", 0x01);
//		return;
//	}
//
//#if 0
//	/* ���� ���õ� �����ǰ� ������ Ȯ�� */
//	if (!uvEng_JobRecipe_GetSelectRecipe())
//	{
//		m_pDlgMain->PostMessageW(WM_MAIN_CHILD, WPARAM(ENG_CMDI::en_menu_gerb), 0L);
//	}
//	/* �θ𿡰� ���� �ƴٰ� �˸���, �뱤 ȭ������ ��ȯ ���� */
//	else
//	{
//		m_pDlgMain->PostMessageW(WM_MAIN_CHILD, WPARAM(ENG_CMDI::en_menu_expo), 0L);
//	}
//#else
//	if (uvEng_JobRecipe_GetSelectRecipe())
//	{
//		m_pDlgMain->PostMessageW(WM_MAIN_CHILD, WPARAM(ENG_CMDI::en_menu_expo), 0L);
//	}
//#endif
}

/*
 desc : ī�޶� ������ ��ũ ������ ��ȿ���� ���� Ȯ��
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMark::IsSetValidMark()
{
	UINT8 i	= 0x00, j1 = 0x00, j2 = 0x00;
	BOOL bEqual	= FALSE;
	CDlgMesg dlgMesg;

	/* ī�޶� ���� �ּ� 1 �� �̻��� ��ũ ������ �����Ǿ� �ִ��� ���� */
	//for (i=0x00; i<0x02; i++)
	//{
	//	if (m_pDrawModel[i]->IsSetModel())		j1++;
	//	if (m_pDrawModel[i+2]->IsSetModel())	j2++;
	//}
	//if (j1 < 0x01 || j2 < 0x01)	return FALSE;

	if(!m_pDrawModel[0]->IsSetModel())
		return FALSE;
	/* ī�޶� 1�� : ������ ��ũ ������ ����Ǿ� �ִ��� ���� */
 	if (!bEqual)	bEqual = m_pDrawModel[0]->IsModelName(m_pDrawModel[1]->GetModelName());
//	if (!bEqual)	bEqual = m_pDrawModel[0]->IsModelType(m_pDrawModel[1]->GetModelType());
//	if (!bEqual)	bEqual = m_pDrawModel[0]->IsModelColor(m_pDrawModel[1]->GetModelColor());
	if (!bEqual)	bEqual = m_pDrawModel[0]->IsModelSize(m_pDrawModel[1]->GetModelSize());
	/* ī�޶� 2�� : ������ ��ũ ������ ����Ǿ� �ִ��� ���� */
	//if (!bEqual)	bEqual = m_pDrawModel[2]->IsModelName(m_pDrawModel[3]->GetModelName());
//	if (!bEqual)	bEqual = m_pDrawModel[2]->IsModelType(m_pDrawModel[3]->GetModelType());
//	if (!bEqual)	bEqual = m_pDrawModel[2]->IsModelColor(m_pDrawModel[3]->GetModelColor());
	//if (!bEqual)	bEqual = m_pDrawModel[2]->IsModelSize(m_pDrawModel[3]->GetModelSize());

	if (bEqual)	dlgMesg.MyDoModal(L"The registered mark data is invalid", 0x01);

	return !bEqual;
}

/*
 desc : Ʈ���� ä�� ��ȣ ����
 parm : None
 retn : None
*/
VOID CDlgMark::LoadTriggerCh()
{
	UINT8 u8ChNo		= 0x01;
	TCHAR tzChNo[32]	= {NULL};
	//LPG_CTSP pstTrig	= &uvEng_GetConfig()->trig_grab;

	//u8ChNo	= uvEng_GetConfig()->set_comn.strobe_lamp_type == 0x00 ? 0x01 : 0x02;	/* 0x00: Coaxial : 0x01 : Ring */
	u8ChNo = uvEng_GetConfig()->set_comn.strobe_lamp_type;

	//swprintf_s(tzChNo, 32, L"CH\n[ %u ]", u8ChNo);
	//m_btn_ctl[eMARK_BTN_CTL_TRIG_CH].SetWindowTextW(tzChNo);

	/*IO ��ȣ �ҷ�����*/
	ST_IO stIO;
	CIOManager::GetInstance()->UpdateIO();
	/*�ش� ��ȣ ����*/
	CIOManager::GetInstance()->GetIO(eIO_OUTPUT, _T("CAMERA 1 IR RING LIGHT CHANGE"), stIO);
	stIO.bOn = (BOOL)u8ChNo;
	CIOManager::GetInstance()->SetIO(eIO_OUTPUT, stIO);
	CIOManager::GetInstance()->GetIO(eIO_OUTPUT, _T("CAMERA 2 IR RING LIGHT CHANGE"), stIO);
	stIO.bOn = (BOOL)u8ChNo;
	CIOManager::GetInstance()->SetIO(eIO_OUTPUT, stIO);
	CIOManager::GetInstance()->GetIO(eIO_OUTPUT, _T("CAMERA 3 IR RING LIGHT CHANGE"), stIO);
	stIO.bOn = (BOOL)u8ChNo;
	CIOManager::GetInstance()->SetIO(eIO_OUTPUT, stIO);

	if(u8ChNo)
	{
		swprintf_s(tzChNo, 32, L"Lamp\n[Ring]");
	}
	else
	{
		swprintf_s(tzChNo, 32, L"Lamp\n[IR]");
	}

	/*UI ��ư ǥ��*/
	m_btn_ctl[eMARK_BTN_CTL_TRIG_CH].SetWindowTextW(tzChNo);
}

/*
 desc : Ʈ���� ä�� ��ȣ ����
 parm : None
 retn : None
*/
VOID CDlgMark::SetTriggerCh()
{
	CDlgMesg dlgMesg;
	///* Live ���� ���� */
	//if (m_chk_cam[eMARK_CHK_CAM_LIVE].GetCheck())
	//{
	//	dlgMesg.MyDoModal(L"Live Mode is currently on", 0x01);
	//	return;
	//}

	UINT8 u8ChNo	= uvEng_GetConfig()->set_comn.strobe_lamp_type;
	if (!u8ChNo)	uvEng_GetConfig()->set_comn.strobe_lamp_type = 0x01;	/* IR */
	else			uvEng_GetConfig()->set_comn.strobe_lamp_type = 0x00;	/* Ring, Amber */

	LoadTriggerCh();

}

/*
 desc : Live Mode ����
 parm : None
 retn : None
*/
VOID CDlgMark::SetLiveView() 
{
	//UINT8 u8ACamID	= m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
	UINT8 u8ACamID = CheckSelectCam();
	BOOL bSucc		= FALSE;
	UINT32 u32OnOff	= 0x00000000;
	CDlgMesg dlgMesg;

	
			if (!u8ACamID)
	{
		dlgMesg.MyDoModal(L"There is no camera selected!", 0x01);
		return;
	}
		uvEng_Camera_ClearShapes(DISP_TYPE_MARK_LIVE);
		
	/* ���� äũ�� �����̸� */
	if (m_chk_cam[eMARK_CHK_CAM_LIVE].GetCheck())	/* Live View */
	{
		/* Align Camera is Live Mode */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_live_mode);
		/* Trigger & Strobe : Enabled */
		//if (uvEng_Mvenc_ReqTriggerStrobe(TRUE))
		//{
		//	bSucc = uvEng_Mvenc_ReqEncoderLive(u8ACamID, m_u8lamp_type);
		//}
		uvEng_Mvenc_ReqWirteTrigLive(u8ACamID, TRUE);
	}
	else
	{
		uvEng_Mvenc_ReqWirteTrigLive(u8ACamID, FALSE);
		/* Trigger Disabled */
		//bSucc = uvEng_Mvenc_ReqEncoderOutReset();
		bSucc = 1;
		/* Trigger & Strobe : Disabled */
		if (bSucc)
		{
			bSucc = uvEng_Mvenc_ReqTriggerStrobe(FALSE);
			UINT64 u64Tick = GetTickCount64();

			do	{

				/* Trigger Board�κ��� ������ ���� ���� ���� �Դ��� Ȯ�� */
				if (0 == uvEng_ShMem_GetTrig()->enc_out_val)
				{
					/* �ִ� 1 �� ���� ��� �� ���� �������� */
					if (GetTickCount64() > (u64Tick + 1000))	break;
				}
				else
				{
					/* �ִ� 2 �� ���� ��� �� ���� ������, ���� �������� */
					if (GetTickCount64() > (u64Tick + 2000))	break;
				}
				Sleep(10);

			}	while (1);
			/* Align Camera is Calibration Mode */
			uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
		}
	}
	Invalidate(TRUE);
}

/*
 desc : Live Grabbed Image ���
 parm : None
 retn : None
*/
VOID CDlgMark::UpdateLiveView()	
{
	/* Grabbed Image Display */
	if (m_chk_cam[eMARK_CHK_CAM_LIVE].GetCheck())	/* Live Mode */
	{
		//UINT8 u8ACamID	= m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
		UINT8 u8ACamID = CheckSelectCam();

		//RECT rDraw;
		//GetDlgItem(IDC_MARK_PIC_FIND_GRAB)->GetClientRect(&rDraw);
		//HDC hDC = ::GetDC(GetDlgItem(IDC_MARK_PIC_FIND_GRAB)->m_hWnd);

		//m_pic_img[4].GetClientRect(&rDraw);
		//HDC hDC = ::GetDC(m_pic_img[4].m_hWnd);
		//if (hDC)
		//{
		//	uvEng_Camera_DrawLiveBitmap(hDC, rDraw, u8ACamID, false);
		//	::ReleaseDC(GetDlgItem(IDC_MARK_PIC_FIND_GRAB)->m_hWnd, hDC);
		//}

		if (m_u8ViewMode == 0x00)	m_u8ViewMode	= 0x01;	/* Live Mode ���� */

		uvEng_Camera_DrawImageBitmap(DISP_TYPE_MARK_LIVE, u8ACamID-1, u8ACamID,0,0);
		uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MARK_LIVE, 0);
	}
	else	/* Grabbed Mode */
	{
		/* ��� ������ Live Mode���ٰ� Grab Mode�� �ٲ� ��� */
		if (0x01 == m_u8ViewMode)	m_u8ViewMode	= 0x00;	/* Grab Mode ���� */
	}
}

/*
 desc : Mark �˻� (Geomatric) ����
 parm : None
 retn : None
*/
VOID CDlgMark::SetMatchModel()	
{
	//UINT8 u8ACamID	= m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
	UINT8 u8ACamID = CheckSelectCam();
	TCHAR tzVal[64]	= {NULL};
	UINT64 u64Tick	= GetTickCount64();
	LPG_ACGR pstGrab= NULL;
	CDlgMesg dlgMesg;

	/* ���� ��� �� �ʱ�ȭ */
	m_edt_out[eMARK_EDT_OUT_CENT_ERR_X].SetWindowTextW(L"0.0000 ");
	m_edt_out[eMARK_EDT_OUT_CENT_ERR_Y].SetWindowTextW(L"0.0000 ");
	m_edt_out[eMARK_EDT_OUT_SCALE_RATE].SetWindowTextW(L"0.0000 ");	/* Scale */
	m_edt_out[eMARK_EDT_OUT_SCORE_RATE].SetWindowTextW(L"0.0000 ");	/* Score */

	if (uvEng_Camera_GetCamMode() != ENG_VCCM::en_image_mode) {
		/* Live ���� ���� */
		if (m_chk_cam[eMARK_CHK_CAM_LIVE].GetCheck()) 
		{
			dlgMesg.MyDoModal(L"Live Mode is currently on", 0x01);
			return;
		}
		/* Mark Model�� ��ϵǾ� �ִ��� ���� */
		if (!uvEng_Camera_IsSetMarkModelACam(u8ACamID, 2))
		{
			dlgMesg.MyDoModal(L"No Mark Model registered", 0x01);
			return;
		}
		/* ī�޶� Grabbed Mode�� Calibration Mode�� ���� */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);
		
		
		//uvEng_Camera_SetCamMode(ENG_VCCM::en_edge_mode);
		
		/* Camera �ʿ� Trigger Event ������ 1�� �߻� */

		uvEng_Camera_TriggerMode((int)u8ACamID,ENG_TRGM::en_line_mode);

		auto triggerMode = uvEng_Camera_GetTriggerMode(u8ACamID);

		vector<function<bool()>> trigAction =
		{
			[&]() {return uvEng_Camera_SWGrab(u8ACamID); },
			[&]() 
			{
				if (!uvEng_Mvenc_ReqTrigOutOne(u8ACamID))
				{
					dlgMesg.MyDoModal(L"Failed to send the Trigger Event", 0x01);
					return false;
				}
				return true;
			},
		};
		
		if (trigAction[triggerMode == ENG_TRGM::en_Sw_mode ? 0 : 1]() == false) return;
	}
	//uvEng_Camera_DrawImageBitmap(DISP_TYPE_MARK_LIVE, u8ACamID - 1, u8ACamID);

	/* Grabbed Image�� ��Ī ����� �����ϴ��� ���� Ȯ�� */
	do {
		UINT8 mode = 0xff;
		/* Grabbed Image�� �����ϴ��� Ȯ�� */
		//uvEng_Camera_RunEdgeDetect(u8ACamID);
		
		
		pstGrab = uvEng_Camera_RunModelCali(u8ACamID, mode, (UINT8)DISP_TYPE_MARK_LIVE, TMP_MARK, TRUE, m_chk_img[0].GetCheck(),0);
		if (pstGrab && 0x00 != pstGrab->marked)	break;	/* ��� �� ��� */
		/* ���� �ð� ���� ������ ������ ���� ���� ���� */
#ifdef _DEBUG
		//if (u64Tick+5000 < GetTickCount64())	break;
		if (u64Tick + 100 < GetTickCount64())	break;
#else
		if (u64Tick+1000 < GetTickCount64())	break;
#endif
		Sleep(100);

	} while (1);

	if (pstGrab && 0x00 != pstGrab->marked)
	{
		swprintf_s(tzVal, 64, L"%.4f ", pstGrab->move_mm_x);
		m_edt_out[eMARK_EDT_OUT_CENT_ERR_X].SetWindowTextW(tzVal);
		swprintf_s(tzVal, 64, L"%.4f ", pstGrab->move_mm_y);
		m_edt_out[eMARK_EDT_OUT_CENT_ERR_Y].SetWindowTextW(tzVal);
		swprintf_s(tzVal, 64, L"%.4f ", pstGrab->scale_rate);
		m_edt_out[eMARK_EDT_OUT_SCALE_RATE].SetWindowTextW(tzVal);
		swprintf_s(tzVal, 64, L"%.4f ", pstGrab->score_rate);
		m_edt_out[eMARK_EDT_OUT_SCORE_RATE].SetWindowTextW(tzVal);
		InvalidateView();	/*uvEng_Camera_DrawCali(m_pic_img[eMARK_PIC_FIND_GRAB].GetSafeHwnd());*/

#ifdef _DEBUG
		double dStageX, dStageY, dCam1X;
		dStageX = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_x);
		dStageY = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_stage_y);
		dCam1X = uvCmn_MC2_GetDrvAbsPos(ENG_MMDI::en_axis_acam1);

		TCHAR szMesg[LOG_MESG_SIZE] = { NULL };
		//swprintf_s(szMesg, LOG_MESG_SIZE, L"Mark Foucs :  %.4f,%.4f,%.4f,%.4f,%.4f\n"
		//	, dCam1X, dStageY, dStageX, pstGrab->scale_rate, pstGrab->score_rate);
// 		swprintf_s(szMesg, LOG_MESG_SIZE, L"Mark Foucs :  %.4f,%.4f,%.4f,%.4f,%.4f\n"
// 			, dCam1X, pstGrab->move_mm_x, pstGrab->move_mm_y, pstGrab->scale_rate, pstGrab->score_rate);
		swprintf_s(szMesg, LOG_MESG_SIZE, L"Mark Foucs :  CAM1X[%.4f],move_mm_x[%.4f],move_mm_y[%.4f],scale_rate[%.4f],score_rate[%.4f],coverage_rate[%.4f],fit_error[%.4f],circle_radius[%.4f]\n"
			, dCam1X, pstGrab->move_mm_x, pstGrab->move_mm_y, pstGrab->scale_rate, pstGrab->score_rate, pstGrab->coverage_rate, pstGrab->fit_error, pstGrab->circle_radius);

		/*Log ���*/
		m_strLog.Format(szMesg);
		txtWrite(m_strLog);
#else

#endif

	}
}

VOID CDlgMark::txtWrite(CString msg)
{
	CStdioFile	imsifile;

	TCHAR tzFile[_MAX_PATH] = { NULL };
	SYSTEMTIME stTm = { NULL };

	GetLocalTime(&stTm);

	TCHAR tzPath[_MAX_PATH];
	swprintf_s(tzPath, _MAX_PATH, L"%s\\logs\\expo\\%04d-%02d-%02d FocusDatat.dat", g_tzWorkDir, stTm.wYear, stTm.wMonth, stTm.wDay);


	imsifile.Open(tzPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite);

	imsifile.SeekToEnd();

	swprintf_s(tzFile, L"[%04d-%02d-%02d %02d:%02d:%02d.%03d]  ",
		(UINT16)stTm.wYear, (UINT16)stTm.wMonth, (UINT16)stTm.wDay,
		(UINT16)stTm.wHour, (UINT16)stTm.wMinute, (UINT16)stTm.wSecond, (UINT16)stTm.wMilliseconds);

	imsifile.WriteString(tzFile);
	imsifile.WriteString(msg);

	imsifile.Close();

}

/*
 desc : Edge Detection ����
 parm : None
 retn : None
*/
VOID CDlgMark::SetEdgeDetect()
{
	//UINT8 u8ACamID	= m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
	UINT8 u8ACamID = CheckSelectCam();
	BOOL bSucc				= FALSE;
	TCHAR tzDiaMeter[32]	= {NULL};
	UINT32 u32SizeHorz		= 0, u32SizeVert = 0, u32ErrRange = 0;
	UINT64 u64Tick			= GetTickCount64();
	CDlgMesg dlgMesg;

	/* Align Camera is Edge Detection Mode */
	uvEng_Camera_SetCamMode(ENG_VCCM::en_edge_mode);
	/* Camera �ʿ� Trigger Event ������ 1�� �߻� */
	if (!uvEng_Mvenc_ReqTrigOutOne(u8ACamID))
	{
		dlgMesg.MyDoModal(L"Failed to send the Trigger Event", 0x01);
		return;
	}
	/* Grabbed Image�� ��Ī ����� �����ϴ��� ���� Ȯ�� */
	do {

		/* Edge Detection �� ��� */
		if (uvEng_Camera_RunEdgeDetect(u8ACamID))
		{
			InvalidateView();
			bSucc	= TRUE;
			break;
		}

		/* ���� �ð� ���� ������ ������ ���� ���� ���� */
#ifdef _DEBUG
		if (u64Tick+5000 < GetTickCount64())	break;
#else
		if (u64Tick+3000 < GetTickCount64())	break;
#endif
		Sleep(100);

	} while (1);

	if (bSucc)
	{
		CDlgEdge dlgEdge;
		dlgEdge.MyDoModal(u8ACamID);
		InvalidateView();
	}
}

/*
 desc : �̹��� �� ���� ����
 parm : None
 retn : None
*/
VOID CDlgMark::InvalidateView()
{
}

/*
 desc : MMF ���� ���� ���̾�α� ���� �� ���ϸ� ���� �� ����Ʈ ��Ʈ�ѿ� ����
 parm : None
 retn : None
*/
VOID CDlgMark::OpenSelectMARK()
{
	TCHAR tzInit[MAX_PATH_LEN]	= {NULL};
	PTCHAR ptzFileName	= NULL;
	PTCHAR tzFileName = (PTCHAR)::Alloc(sizeof(TCHAR) * MAX_PATH_LEN);
	PTCHAR ptzPathFile	= (PTCHAR)::Alloc(sizeof(TCHAR)*MAX_PATH_LEN);
	
	/* Reset the file buffer */
	wmemset(ptzPathFile, 0x00, MAX_PATH_LEN);
	wmemset(tzFileName, 0x00, MAX_PATH_LEN);

	/* Set the path of mmf directory */
	swprintf_s(tzInit, MAX_PATH_LEN, L"%s\\%s", g_tzWorkDir, CUSTOM_DATA_CONFIG);
	if (!uvCmn_GetSelectFile(L"MARK File (*.mmf, *.pat)|*.mmf;*.pat",
							 MAX_PATH_LEN, ptzPathFile,
							 L"Please select the MMF&PAT file", tzInit)) 
	{
		Free(ptzPathFile);
		return;
	}
	if (wcslen(ptzPathFile) < 1) 
	{
		Free(ptzPathFile);
		return;
	}

	/* Ȯ���� �� ��ü ��� ������ ������ ���� �̸��� ���� */
#if 1
	ptzFileName = uvCmn_GetFileName(ptzPathFile, (UINT32)wcslen(ptzPathFile)); 
	UINT32 tmp = UINT32(wcslen(ptzFileName) - 4);
	wmemcpy(tzFileName, ptzFileName, UINT32(wcslen(ptzFileName) - 4));
	/* ���õ� ���ϸ� �Է� */
	OutputFileMARK(tzFileName);
#else
	/* ���õ� ���ϸ� �Է� */
	ptzFileName	= uvCmn_GetFileName(ptzPathFile, (UINT32)wcslen(ptzPathFile));
	OutputFileMMF(ptzFileName);
#endif

	/* Release the memory of file buffer */
	Free(ptzPathFile);
	Free(tzFileName);
}

/*
 desc : MMF ���� �̸� ��� (���� �̸��� �� ���, �����ٿ� ǥ���� ���ؼ� T.T)
 parm : file	- [in]  ��� ��� ���� �̸� (Ȯ���� ����)
 retn : None
*/
VOID CDlgMark::OutputFileMARK(PTCHAR file) 
{
	m_edt_ctl[eMARK_EDT_CTL_MMF].SetWindowTextW(file); 
	m_edt_ctl[eMARK_EDT_CTL_MMF].Invalidate(TRUE);
}

/* ----------------------------------------------------------------------------------------- */
/*                                 lk91 VISION �߰� �Լ�                                     */
/* ----------------------------------------------------------------------------------------- */

/* desc: Frame Control�� MIL DISP�� ���� */ 
VOID CDlgMark::InitDispMark()
{
	CWnd* pWnd_Mark;
	pWnd_Mark = GetDlgItem(IDC_MARK_PIC_MODEL_1);
	uvEng_Camera_SetDispMark(pWnd_Mark);

	CWnd* pWnd[2];
	pWnd[0] = GetDlgItem(IDC_MARK_PIC_FIND_GRAB);
	pWnd[1] = NULL;
	uvEng_Camera_SetDisp(pWnd, 0x00);

	DispResize(GetDlgItem(IDC_MARK_PIC_FIND_GRAB));

	uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MARK_LIVE, 0);
}

/* desc: Frame Control�� �̹��� ������ ������ ������ */
void CDlgMark::DispResize(CWnd* pWnd)
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

	uvEng_Camera_SetMarkLiveDispSize(tgtSize);
}

/* desc: Image Mark ��� */
VOID CDlgMark::RegistMarkImage() 
{
	//UINT8 u8ACamID = m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
	UINT8 u8ACamID = CheckSelectCam();
	BOOL bSucc = FALSE;

	TCHAR tzName[MARK_MODEL_NAME_LENGTH] = { NULL };
	m_edt_txt[eMARK_EDT_TXT_MODEL_NAME].GetWindowTextW(tzName, MARK_MODEL_NAME_LENGTH);

	CString strFileName = _T("");

	m_edt_txt[eMARK_EDT_TXT_MODEL_NAME].GetWindowTextW(strFileName);

	// �ؽ�Ʈ �����Ͱ� �������.
	strFileName = strFileName.Trim();
	if (strFileName.IsEmpty())
	{
		AfxMessageBox(_T("Model ���� �Է��ؾ� �մϴ�."));
		return;
	}
	//����ȯ �� �ؽ�Ʈ�� ���� üũ
	if (strFileName.GetLength() > 100)
	{
		CString strMsg = _T("");
		strMsg.Format(_T("�Է� �ؽ�Ʈ�� ���̴� %d �� �ʰ��� �� �����ϴ�.", 100));
		AfxMessageBox(strMsg);
		return;
	}
	if (abs(um_rectArea.left - um_rectArea.right) == 0 || abs(um_rectArea.top - um_rectArea.bottom) == 0) {
		AfxMessageBox(_T("ȭ�鿡 �����ϰ��� �ϴ� MARK ROI�� �������ּ���."));
		return;
	}

	// lk91 mmf&pat Folder Check & Create
	CFileFind	tmpfinder;
	BOOL tmpIsFind;
	CString tmpstrDir;
	tmpstrDir.Format(_T("%s\\%s\\mmf"), g_tzWorkDir, CUSTOM_DATA_CONFIG);
	tmpIsFind = tmpfinder.FindFile(tmpstrDir);
	if (!tmpIsFind) {
		CreateDirectory(tmpstrDir, NULL);
	}
	tmpstrDir.Format(_T("%s\\%s\\pat"), g_tzWorkDir, CUSTOM_DATA_CONFIG);
	tmpIsFind = tmpfinder.FindFile(tmpstrDir);
	if (!tmpIsFind) {
		CreateDirectory(tmpstrDir, NULL);
	}
	tmpfinder.Close();


	STG_CMPV stRecipe = { NULL };
	CUniToChar csCnv;
	CString strFile;
	strcpy_s(stRecipe.name, MARK_MODEL_NAME_LENGTH, csCnv.Uni2Ansi(tzName));

	// lk91 Grab �̺�Ʈ �߰�
	CDlgMesg dlgMesg;
	if (uvEng_Camera_GetCamMode() != ENG_VCCM::en_image_mode) {
		/* Live ���� ���� */
		if (m_chk_cam[eMARK_CHK_CAM_LIVE].GetCheck())
		{
			dlgMesg.MyDoModal(L"Live Mode is currently on", 0x01);
			return;
		}
		/* ī�޶� Grabbed Mode�� Calibration Mode�� ���� */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);
		/* Camera �ʿ� Trigger Event ������ 1�� �߻� */
		if (!uvEng_Mvenc_ReqTrigOutOne(u8ACamID))
		{
			dlgMesg.MyDoModal(L"Failed to send the Trigger Event", 0x01);
			return;
		}
	}

	//uvEng_Camera_SetCamMode(ENG_VCCM::en_image_mode);// lk91!! 240202 tmp

	//uvEng_Camera_SetCamMode(ENG_VCCM::en_live_mode);
	strFile.Format(_T("%s\\%s\\pat\\%s.pat"), g_tzWorkDir, CUSTOM_DATA_CONFIG, strFileName);
	if (!uvCmn_Camera_RegistPat(u8ACamID, um_rectArea, strFile, TMP_MARK)) {
		AfxMessageBox(_T("Grab Error!"));
		return;
	}
	strFile.Format(_T("%s\\%s\\mmf\\%s.mmf"), g_tzWorkDir, CUSTOM_DATA_CONFIG, strFileName);
	if (!uvCmn_Camera_RegistMod(u8ACamID, um_rectArea, strFile, TMP_MARK)) {
		AfxMessageBox(_T("Grab Error!"));
		return;
	}
	//uvEng_Camera_SetCamMode(ENG_VCCM::en_none);
	//CString -> TCHAR ����ȯ
	//memset(tcharFileName, '\0', sizeof(tcharFileName));
	//wcscpy(tcharFileName, (LPCTSTR)strFile);
	//wcscpy_s(tcharFileName, (LPCTSTR)strFile);
	//SaveFileName = _T("C:\\DI_TEST\\data\\pat\\000.pat");

	// CString -> TCHAR * 
	TCHAR* tcharFileName = _T("");
	tcharFileName = (TCHAR*)(LPCTSTR)strFile;
	CDlgSetMark dlgSetMark;
	if (dlgSetMark.MyDoModal(u8ACamID, strFileName) == IDOK)
	{
		stRecipe.type = (UINT32)GetModelType();
		stRecipe.iSizeP = uvCmn_Camera_GetMarkSize(u8ACamID, TMP_MARK);
		stRecipe.iOffsetP = uvCmn_Camera_GetMarkOffset(u8ACamID, false, TMP_MARK);

		// Cstring -> char *
		wchar_t* wchar_str;
		int char_str_len;
		// 1. CString to wchar * conversion
		wchar_str = strFileName.GetBuffer(strFileName.GetLength());
		char_str_len = WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, NULL, 0, NULL, NULL);
		// 2. wchar_t* to char* conversion
		WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, stRecipe.file, char_str_len, 0, 0);

		uvEng_Mark_ModelAppend(&stRecipe, 0x00);

		/* �ٽ� �� ���� �ҷ� ���� */
		m_pGridModel->Load();
		/* �ʱ�ȭ */
		RecipeReset(0x00);

	}
	//uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);// lk91!! 240202 tmp
}

/* desc: Image �ҷ����� */
void CDlgMark::LoadImageFile()	
{
	//UINT8 u8ACamID = m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
	UINT8 u8ACamID = CheckSelectCam();
	BOOL bSucc = FALSE;

	uvEng_Camera_SetCamMode(ENG_VCCM::en_image_mode);

	GetDlgItem(IDC_MARK_PIC_FIND_GRAB)->ShowWindow(SW_SHOW);
	//DispResize(GetDlgItem(IDC_MARK_PIC_FIND_GRAB));

	CWnd* pImageWnd = GetDlgItem(IDC_MARK_PIC_FIND_GRAB);
	
	CRect rDraw;
	pImageWnd->GetClientRect(rDraw);
	HDC hDC = ::GetDC(pImageWnd->m_hWnd);

	if (hDC)
	{
		TCHAR tzInit[MAX_PATH_LEN] = { NULL };
		PTCHAR ptzFileName = NULL;
		PTCHAR ptzPathFile = (PTCHAR)::Alloc(sizeof(TCHAR) * MAX_PATH_LEN);

		/* Reset the file buffer */
		wmemset(ptzPathFile, 0x00, MAX_PATH_LEN);

		if (!uvCmn_GetSelectFile(L"BMP File (*.bmp) | *.bmp",
			MAX_PATH_LEN, ptzPathFile,
			L"Please select the MMF file", tzInit)) 
		{
			::Free(ptzPathFile);
			return;
		}
		if (uvEng_Camera_LoadImageFromFile(u8ACamID, ptzPathFile)) {
			uvEng_Camera_DrawImageBitmap(DISP_TYPE_MARK_LIVE, u8ACamID-1, u8ACamID); 
		}
		else {
			CDlgMesg dlgMesg;
			dlgMesg.MyDoModal(L"Failed to load the Image", 0x01);
		}

		::ReleaseDC(pImageWnd->m_hWnd, hDC);
		::Free(ptzPathFile); 
	}
}

/* desc: Mark ROI, Search ROI, Measure, Zoom Menu ���� */
void CDlgMark::OnContextMenu(CWnd* /*pWnd*/, CPoint fi_pointP/*point*/)
{
	UpdateData(TRUE);

	WINDOWPLACEMENT wndpl;
	int idNo = IDC_MARK_PIC_FIND_GRAB;
	GetDlgItem(idNo)->GetWindowPlacement(&wndpl);
	//UINT8 u8ACamID = m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
	UINT8 u8ACamID = CheckSelectCam();
	CRect rectTmp;
	rectTmp = wndpl.rcNormalPosition;
	if (fi_pointP.x < rectTmp.left || fi_pointP.x > rectTmp.right
		|| fi_pointP.y < rectTmp.top || fi_pointP.y > rectTmp.bottom)
		return;

	CMenu* pMenu, * pSubMenu;

	pMenu = new CMenu();
	pMenu->LoadMenuW(IDR_MENU1);
	pSubMenu = pMenu->GetSubMenu(0);
	if (menuPart == 1) { // MARK ROI
		pSubMenu->CheckMenuItem(ID_MENU_MARK_ROI, MF_BYCOMMAND | MF_CHECKED);
		pSubMenu->CheckMenuItem(ID_MENU_MEASURE, MF_BYCOMMAND | MF_UNCHECKED);
		pSubMenu->CheckMenuItem(ID_MENU_CALIB, MF_BYCOMMAND | MF_UNCHECKED);
	}
	else if (menuPart == 2 || menuPart == 4) { // SEARCH ROI || ZOOM
		pSubMenu->CheckMenuItem(ID_MENU_MARK_ROI, MF_BYCOMMAND | MF_UNCHECKED);
		pSubMenu->CheckMenuItem(ID_MENU_MEASURE, MF_BYCOMMAND | MF_UNCHECKED);
		pSubMenu->CheckMenuItem(ID_MENU_CALIB, MF_BYCOMMAND | MF_UNCHECKED);
	}
	else if (menuPart == 3) { // MEASURE
		pSubMenu->CheckMenuItem(ID_MENU_MARK_ROI, MF_BYCOMMAND | MF_UNCHECKED);
		pSubMenu->CheckMenuItem(ID_MENU_MEASURE, MF_BYCOMMAND | MF_CHECKED);
		pSubMenu->CheckMenuItem(ID_MENU_CALIB, MF_BYCOMMAND | MF_UNCHECKED);
	}
	else if (menuPart == 6) { // CALIB ROI
		pSubMenu->CheckMenuItem(ID_MENU_MARK_ROI, MF_BYCOMMAND | MF_UNCHECKED);
		pSubMenu->CheckMenuItem(ID_MENU_MEASURE, MF_BYCOMMAND | MF_UNCHECKED);
		pSubMenu->CheckMenuItem(ID_MENU_CALIB, MF_BYCOMMAND | MF_CHECKED);
	}
	else { // 99
		pSubMenu->CheckMenuItem(ID_MENU_MARK_ROI, MF_BYCOMMAND | MF_UNCHECKED);
		pSubMenu->CheckMenuItem(ID_MENU_SEARCH_ROI_1, MF_BYCOMMAND | MF_UNCHECKED);
		pSubMenu->CheckMenuItem(ID_MENU_SEARCH_ROI_2, MF_BYCOMMAND | MF_UNCHECKED);
		pSubMenu->CheckMenuItem(ID_MENU_SEARCH_ROI_3, MF_BYCOMMAND | MF_UNCHECKED);
		pSubMenu->CheckMenuItem(ID_MENU_SEARCH_ALL, MF_BYCOMMAND | MF_UNCHECKED);
		pSubMenu->CheckMenuItem(ID_MENU_MEASURE, MF_BYCOMMAND | MF_UNCHECKED);
		pSubMenu->CheckMenuItem(ID_MENU_CALIB, MF_BYCOMMAND | MF_UNCHECKED);
	}

	if (menuPart == 2)
	{
		pSubMenu->CheckMenuItem(ID_MENU_SEARCH_ALL, MF_BYCOMMAND | MF_UNCHECKED);

		if(searchROI_CAM[0])
			pSubMenu->CheckMenuItem(ID_MENU_SEARCH_ROI_1, MF_BYCOMMAND | MF_CHECKED);
		if (searchROI_CAM[1])
			pSubMenu->CheckMenuItem(ID_MENU_SEARCH_ROI_2, MF_BYCOMMAND | MF_CHECKED);
		if (searchROI_CAM[2])
			pSubMenu->CheckMenuItem(ID_MENU_SEARCH_ROI_3, MF_BYCOMMAND | MF_CHECKED);
	}

	int cmd;
	cmd = pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD, fi_pointP.x, fi_pointP.y, this); 
	if (pMenu)
	{
		delete pMenu;
		pMenu = NULL;
	}
	if (cmd == ID_MENU_IMAGE_LOAD) { 
		menuPart = 0;
		LoadImageFile();
		menuPart = 99;
	}
	else if (cmd == ID_MENU_MARK_ROI) {
		if (menuPart == 1) 
			menuPart = 99;
		else
			menuPart = 1;
	}
	else if (cmd == ID_MENU_SEARCH_ROI_1 || cmd == ID_MENU_SEARCH_ROI_2 || cmd == ID_MENU_SEARCH_ROI_3) {
		if (cmd == ID_MENU_SEARCH_ROI_1) {
			if (searchROI_CAM[0])
				searchROI_CAM[0] = false;
			else
				searchROI_CAM[0] = true;
		}
		else if (cmd == ID_MENU_SEARCH_ROI_2) {
			if (searchROI_CAM[1])
				searchROI_CAM[1] = false;
			else
				searchROI_CAM[1] = true;
		}
		else if (cmd == ID_MENU_SEARCH_ROI_3) {
			if (searchROI_CAM[2])
				searchROI_CAM[2] = false;
			else
				searchROI_CAM[2] = true;
		}

		if (!searchROI_CAM[0] && !searchROI_CAM[1] && !searchROI_CAM[2])
			menuPart = 99;
		else
			menuPart = 2;
	}
	else if (cmd == ID_MENU_SEARCH_ALL) {
		CDlgMesg dlgMesg;

		if (menuPart != 2)
		{
			if (IDOK != dlgMesg.MyDoModal(L"Do you want to set the selected CAM to All SEARCH ?", 0x02)) {
				return;
			}
			searchROI_ALL = TRUE;
			menuPart = 99;
		}

		STG_CRD stRoi = { NULL };
		CRect tmpRoi;
		if (searchROI_ALL)
		{
			for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++) {
				if (!searchROI_CAM[i])
					continue;
				tmpRoi.left		= stRoi.roi_Left[i] = 5;
				tmpRoi.right	= stRoi.roi_Right[i] = ACA1920_SIZE_X - 5;
				tmpRoi.top		= stRoi.roi_Top[i] = 5;
				tmpRoi.bottom	= stRoi.roi_Bottom[i] = ACA1920_SIZE_Y - 5;
				
				uvEng_Mark_SetMarkROI(&stRoi, i);
				uvCmn_Camera_MilSetMarkROI(i, tmpRoi);
			}
			uvEng_Mark_MarkROISave();
			searchROI_ALL = FALSE;
		}
		uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MARK_LIVE, u8ACamID-1);
		uvEng_Camera_DrawOverlayDC(true, DISP_TYPE_MARK_LIVE, u8ACamID-1);
	}
	else if (cmd == ID_MENU_MEASURE) {
		if (menuPart == 3)
			menuPart = 99;
		else
			menuPart = 3;
	}
	else if (cmd == ID_MENU_CALIB) {
	if (menuPart == 6)
		menuPart = 99;
	else
		menuPart = 6;
	}
	else if (cmd == 0) {
		return;
	}
	else { // zoom
		menuPart = 4;
		if (cmd == ID_MENU_ZOOM_IN)
		{
			MenuZoomIn();
		}
		else if (cmd == ID_MENU_ZOOM_OUT) 
		{
			MenuZoomOut();
		}
		else if (cmd == ID_MENU_ZOOM_FIT)
		{
			MenuZoomFit();
		}
		menuPart = 99;
	}
}

/* desc: �̻�� */
BOOL CDlgMark::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return CDlgMenu::OnCommand(wParam, lParam);
}

/* desc: ���콺 ��ư Ŭ�� �̺�Ʈ, Mark ROI, Search ROI, Measure, Zoom ��� */
void CDlgMark::OnLButtonDown(UINT nFlags, CPoint point)
{
	//CDPoint	dRateP;
	//CRect rt1, rt2;
	//GetDlgItem(IDC_MARK_PIC_FIND_GRAB)->GetWindowRect(&rt2);
	//rt1 = rt2;
	//ScreenToClient(&rt2);
	//dRateP.x = (double)ACA1920_SIZE_X / rt1.Width();
	//dRateP.y = (double)ACA1920_SIZE_Y / rt1.Height();
	//UINT8 u8ACamID = m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
	UINT8 u8ACamID = CheckSelectCam();

	CRect rt;
	GetDlgItem(IDC_MARK_PIC_FIND_GRAB)->GetWindowRect(&rt);
	ScreenToClient(&rt);

	double dRate = 1.0 / tgt_rate;

	if (rt.PtInRect(point) && (menuPart == 1 || menuPart == 2 || menuPart == 3 || menuPart == 6))
	{
		OldZoomFlag = ZoomFlag[u8ACamID];
		ZoomFlag[u8ACamID] = false;

		UpdateData(TRUE);

		point.x = abs(point.x - rt.left);
		point.y = abs(point.y - rt.top);

		um_bMoveFlag = true;
		CDPoint	iTmpP;
		iTmpP = uvCmn_Camera_trZoomPoint(DISP_TYPE_MARK_LIVE, u8ACamID, point);

		iTmpP.x = (int)(iTmpP.x * dRate + 0.5);
		iTmpP.y = (int)(iTmpP.y * dRate + 0.5);

		ptMenu.x = point.x;
		ptMenu.y = point.y;

		um_rectArea.left = (LONG)iTmpP.x;
		um_rectArea.top = (LONG)iTmpP.y;
		um_rectArea.right = (LONG)iTmpP.x;
		um_rectArea.bottom = (LONG)iTmpP.y;

		UpdateData(FALSE);
	}
	//else if (rt2.PtInRect(point) && (ZoomFlag[u8ACamID]/*menuPart == 5*/))
	else if (ZoomFlag[u8ACamID])
	{
		um_bMoveFlag = true;
		uvCmn_Camera_SetZoomDownP(DISP_TYPE_MARK_LIVE, u8ACamID, point);
	}

	CDlgMenu::OnLButtonDown(nFlags, point);
}

/* desc: ���콺 ��ư Ŭ�� �̺�Ʈ, Mark ROI, Search ROI, Measure, Zoom ��� */
void CDlgMark::OnLButtonUp(UINT nFlags, CPoint point)
{
	//CDPoint	dRateP;
	int left, right, top, bottom;
	UINT8 u8ACamID = CheckSelectCam();

	CRect rt;
	GetDlgItem(IDC_MARK_PIC_FIND_GRAB)->GetWindowRect(&rt);
	//dRateP.x = (double)ACA1920_SIZE_X / rt.Width();
	//dRateP.y = (double)ACA1920_SIZE_Y / rt.Height();
	ScreenToClient(&rt);

	double dRate = 1.0 / tgt_rate;

	if (um_bMoveFlag && rt.PtInRect(point) && (menuPart == 1 || menuPart == 2 || menuPart == 3 || menuPart == 6))
	{
		UpdateData(TRUE);

		point.x = abs(point.x - rt.left);
		point.y = abs(point.y - rt.top);

		CDPoint	iTmpP;
		iTmpP = uvCmn_Camera_trZoomPoint(DISP_TYPE_MARK_LIVE, u8ACamID, point);
		iTmpP.x = (int)(iTmpP.x * dRate);
		iTmpP.y = (int)(iTmpP.y * dRate);

		if (iTmpP.x < um_rectArea.left + 5)	um_rectArea.right = um_rectArea.left + 5;
		else								um_rectArea.right = (LONG)iTmpP.x;

		if (iTmpP.y < um_rectArea.top + 5)	um_rectArea.bottom = um_rectArea.top + 5;
		else								um_rectArea.bottom = (LONG)iTmpP.y;

		um_rectArea.right =  (LONG)iTmpP.x;
		um_rectArea.bottom = (LONG)iTmpP.y;

		// Rect ���� ����ó��
		left = um_rectArea.left;
		right = um_rectArea.right;
		top = um_rectArea.top;
		bottom = um_rectArea.bottom;

		if (left > right)
		{
			left = um_rectArea.right;
			right = um_rectArea.left;
		}

		if (top > bottom)
		{
			top = um_rectArea.bottom;
			bottom = um_rectArea.top;

		}
		um_rectArea.left = left;
		um_rectArea.right = right;
		um_rectArea.top = top;
		um_rectArea.bottom = bottom;

		//0:imageload, 1:markroi, 2:searchroi1, 3:searchroi2, 4:measure, 5:zoom, 6. Calib Part
		if (menuPart == 1) {
			CString sTmp;
			sTmp = "Mark";

			uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MARK_LIVE, u8ACamID - 1);
			uvEng_Camera_OverlayAddBoxList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.left, um_rectArea.top, um_rectArea.right, um_rectArea.bottom, PS_SOLID, eM_COLOR_GREEN);
			uvEng_Camera_OverlayAddTextList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.right - 50, um_rectArea.bottom, sTmp, eM_COLOR_GREEN, 6, 12, VISION_FONT_TEXT, true);
			uvEng_Camera_DrawOverlayDC(true, DISP_TYPE_MARK_LIVE, u8ACamID - 1);
		}
		else if (menuPart == 2) {
			CString sTmp;
			sTmp.Format(_T("Search ROI"));

			STG_CRD stRoi = { NULL };
			for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++) {
				if (!searchROI_CAM[i])
					continue;
				stRoi.roi_Left[i] = um_rectArea.left;
				stRoi.roi_Right[i] = um_rectArea.right;
				stRoi.roi_Top[i] = um_rectArea.top;
				stRoi.roi_Bottom[i] = um_rectArea.bottom;
				uvEng_Mark_SetMarkROI(&stRoi, i);
				uvCmn_Camera_MilSetMarkROI(i, um_rectArea);

				sTmp.Format(_T("%s #%d"), sTmp, i + 1);
			}
			uvEng_Mark_MarkROISave();


			uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MARK_LIVE, u8ACamID - 1);
			uvEng_Camera_OverlayAddBoxList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.left, um_rectArea.top, um_rectArea.right, um_rectArea.bottom, PS_SOLID, eM_COLOR_BLUE);
			uvEng_Camera_OverlayAddTextList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.right - 50, um_rectArea.bottom, sTmp, eM_COLOR_BLUE, 6, 12, VISION_FONT_TEXT, true);
			uvEng_Camera_DrawOverlayDC(true, DISP_TYPE_MARK_LIVE, u8ACamID - 1);

		}
		else if (menuPart == 3) {
			CString sTmp;
			//sTmp = "Measure";
			//UINT8 u8ACamID = m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
			UINT8 u8ACamID = CheckSelectCam();
			double measX, measY, measLength;
			measX = uvEng_GetConfig()->acam_spec.GetPixelToMM(u8ACamID - 1) * (double)(um_rectArea.right - um_rectArea.left);
			measY = uvEng_GetConfig()->acam_spec.GetPixelToMM(u8ACamID - 1) * (double)(um_rectArea.bottom - um_rectArea.top);
			measLength = sqrt(pow(measX, 2) + pow(measY, 2));
			sTmp.Format(_T("x : %.2lf, y : %.2lf, length : %.2lf"), measX, measY, measLength);

			uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MARK_LIVE, u8ACamID - 1);
			uvEng_Camera_OverlayAddLineList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.left, um_rectArea.top, um_rectArea.right, um_rectArea.bottom, PS_SOLID, eM_COLOR_RED);
			uvEng_Camera_OverlayAddBoxList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.left, um_rectArea.top, um_rectArea.right, um_rectArea.bottom, PS_SOLID, eM_COLOR_MAGENTA);
			uvEng_Camera_OverlayAddTextList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.left + 50, um_rectArea.bottom, sTmp, eM_COLOR_MAGENTA, 6, 12, VISION_FONT_TEXT, true);
			uvEng_Camera_DrawOverlayDC(true, DISP_TYPE_MARK_LIVE, u8ACamID - 1);
		}
		else if (menuPart == 6) {
			int div_x = 0;
			int div_y = 0;
			div_x = (um_rectArea.right - um_rectArea.left) / calib_col;
			div_y = (um_rectArea.bottom - um_rectArea.top) / calib_row;
			CString sTmp;
			uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MARK_LIVE, u8ACamID - 1);
			for (int i = 0; i < calib_col; i++) {
				for (int j = 0; j < calib_row; j++) {
					uvEng_Camera_OverlayAddBoxList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.left + (div_x * i), um_rectArea.top + (div_y * j),
						um_rectArea.left + (div_x * (i + 1)), um_rectArea.top + (div_y * (j + 1)), PS_SOLID, eM_COLOR_MAGENTA);
					sTmp.Format(_T("%d"), i + (j * calib_col) + 1);
					uvEng_Camera_OverlayAddTextList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.left + (div_x * (i + 1)) - 50, um_rectArea.top + (div_y * (j + 1)) - 50, sTmp, eM_COLOR_MAGENTA, 6, 12, VISION_FONT_TEXT, true);

					CalibROI_left[i + (j * calib_col)] = um_rectArea.left + (div_x * i);
					CalibROI_right[i + (j * calib_col)] = um_rectArea.left + (div_x * (i + 1));
					CalibROI_top[i + (j * calib_col)] = um_rectArea.top + (div_y * j);
					CalibROI_bottom[i + (j * calib_col)] = um_rectArea.top + (div_y * (j + 1));
				}
			}
			uvEng_Camera_DrawOverlayDC(true, DISP_TYPE_MARK_LIVE, u8ACamID - 1);
		}

		UpdateData(FALSE);
		ZoomFlag[u8ACamID] = OldZoomFlag;
	}

	um_bMoveFlag = false;

	CDlgMenu::OnLButtonUp(nFlags, point);
}

/* desc: ���콺 �̵� �̺�Ʈ, Mark ROI, Search ROI, Measure, Zoom ��� */
void CDlgMark::OnMouseMove(UINT nFlags, CPoint point)
{
	//CDPoint	dRateP;
	CRect rt;
	GetDlgItem(IDC_MARK_PIC_FIND_GRAB)->GetWindowRect(&rt);
	ScreenToClient(&rt);
	//dRateP.x = (double)ACA1920_SIZE_X / rt1.Width();
	//dRateP.y = (double)ACA1920_SIZE_Y / rt1.Height();
	//UINT8 u8ACamID = m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
	UINT8 u8ACamID = CheckSelectCam();

	double dRate = 1.0 / tgt_rate;

	if (um_bMoveFlag && rt.PtInRect(point) && !ZoomFlag[u8ACamID]) 
	{
		UpdateData(TRUE);

		point.x = abs(point.x - rt.left);
		point.y = abs(point.y - rt.top);

		CDPoint	iTmpP;
		iTmpP = uvCmn_Camera_trZoomPoint(DISP_TYPE_MARK_LIVE, u8ACamID, point);
		iTmpP.x = (int)(iTmpP.x * dRate);
		iTmpP.y = (int)(iTmpP.y * dRate);

		if (iTmpP.x < um_rectArea.left + 5)	um_rectArea.right = um_rectArea.left + 5;
		else								um_rectArea.right = (LONG)iTmpP.x;

		if (iTmpP.y < um_rectArea.top + 5)	um_rectArea.bottom = um_rectArea.top + 5;
		else								um_rectArea.bottom = (LONG)iTmpP.y;

		um_rectArea.right =  (LONG)iTmpP.x;
		um_rectArea.bottom = (LONG)iTmpP.y;

		int		iBrushStyle;
		iBrushStyle = PS_DOT;

		if (menuPart == 1) {
			CString sTmp;
			sTmp = "Mark";
			uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MARK_LIVE, u8ACamID-1);
			uvEng_Camera_OverlayAddBoxList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.left, um_rectArea.top, um_rectArea.right, um_rectArea.bottom, iBrushStyle, eM_COLOR_GREEN);
			uvEng_Camera_OverlayAddTextList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.right - 50, um_rectArea.bottom, sTmp, eM_COLOR_GREEN, 6, 12, VISION_FONT_TEXT, true);
			uvEng_Camera_DrawOverlayDC(true, DISP_TYPE_MARK_LIVE, u8ACamID - 1);
		}
		else if (menuPart == 2) {
			CString sTmp;
			sTmp.Format(_T("Search ROI"));

			for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++) {
				if (!searchROI_CAM[i])
					continue;
				sTmp.Format(_T("%s #%d"), sTmp, i + 1);
			}

			uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MARK_LIVE, u8ACamID - 1);
			uvEng_Camera_OverlayAddBoxList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.left, um_rectArea.top, um_rectArea.right, um_rectArea.bottom, iBrushStyle, eM_COLOR_BLUE);
			uvEng_Camera_OverlayAddTextList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.right - 50, um_rectArea.bottom, sTmp, eM_COLOR_BLUE, 6, 12, VISION_FONT_TEXT, true);
			uvEng_Camera_DrawOverlayDC(true, DISP_TYPE_MARK_LIVE, u8ACamID - 1);
		}
		else if (menuPart == 3) {
			CString sTmp;
			sTmp = "Measure";
			uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MARK_LIVE, u8ACamID - 1);
			uvEng_Camera_OverlayAddBoxList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.left, um_rectArea.top, um_rectArea.right, um_rectArea.bottom, iBrushStyle, eM_COLOR_MAGENTA);
			uvEng_Camera_OverlayAddTextList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.right - 50, um_rectArea.bottom, sTmp, eM_COLOR_MAGENTA, 6, 12, VISION_FONT_TEXT, true);
			uvEng_Camera_DrawOverlayDC(true, DISP_TYPE_MARK_LIVE, u8ACamID - 1);
		}
		else if (menuPart == 6) {
			CString sTmp;
			//sTmp = "Calib ROI";
			uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MARK_LIVE, u8ACamID - 1);
			int div_x = 0;
			int div_y = 0;
			div_x = (um_rectArea.right - um_rectArea.left) / calib_col;
			div_y = (um_rectArea.bottom - um_rectArea.top) / calib_row;
			for (int i = 0; i < calib_col; i++) {
				for (int j = 0; j < calib_row; j++) {
					uvEng_Camera_OverlayAddBoxList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.left + (div_x * i), um_rectArea.top + (div_y * j),
						um_rectArea.left + (div_x * (i+1)), um_rectArea.top + (div_y * (j + 1)), iBrushStyle, eM_COLOR_MAGENTA);
					sTmp.Format(_T("%d"), i + (j * calib_col) + 1);
					uvEng_Camera_OverlayAddTextList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.left + (div_x * (i + 1)) - 50, um_rectArea.top + (div_y * (j + 1)) - 50, sTmp, eM_COLOR_MAGENTA, 6, 12, VISION_FONT_TEXT, true);
				}
			}
			sTmp = "Calib ROI";
			uvEng_Camera_OverlayAddBoxList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.left, um_rectArea.top, um_rectArea.right, um_rectArea.bottom, iBrushStyle, eM_COLOR_MAGENTA);
			uvEng_Camera_OverlayAddTextList(DISP_TYPE_MARK_LIVE, u8ACamID - 1, um_rectArea.right - 50, um_rectArea.bottom, sTmp, eM_COLOR_MAGENTA, 6, 12, VISION_FONT_TEXT, true);
			uvEng_Camera_DrawOverlayDC(true, DISP_TYPE_MARK_LIVE, u8ACamID - 1);
		}
		UpdateData(FALSE);
	}
	else if (ZoomFlag[u8ACamID] && um_bMoveFlag)
	{
		CWnd* pImageWnd = GetDlgItem(IDC_MARK_PIC_FIND_GRAB);
		CRect rDraw;
		pImageWnd->GetClientRect(rDraw);

		uvCmn_Camera_MoveZoomDisp(DISP_TYPE_MARK_LIVE, u8ACamID, point, rDraw);
	}
	CDlgMenu::OnMouseMove(nFlags, point);
}

/* desc: Zoom In */
void CDlgMark::MenuZoomIn()
{
	//UINT8 u8ACamID = m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
	UINT8 u8ACamID = CheckSelectCam();
	//RECT rDraw;
	CWnd* pImageWnd = GetDlgItem(IDC_MARK_PIC_FIND_GRAB);
	CRect rDraw;
	pImageWnd->GetClientRect(rDraw);
	uvCmn_Camera_MilZoomIn(DISP_TYPE_MARK_LIVE, u8ACamID, rDraw);
	ZoomFlag[u8ACamID] = true;
}

/* desc: Zoom Out */
void CDlgMark::MenuZoomOut()
{
	//UINT8 u8ACamID = m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
	UINT8 u8ACamID = CheckSelectCam();
	CWnd* pImageWnd = GetDlgItem(IDC_MARK_PIC_FIND_GRAB);
	CRect rDraw;
	pImageWnd->GetClientRect(rDraw);
	if (!uvCmn_Camera_MilZoomOut(DISP_TYPE_MARK_LIVE, u8ACamID, rDraw)) 
	{
		ZoomFlag[u8ACamID] = false;
	}
}

/* desc: Zoom Fit */
void CDlgMark::MenuZoomFit()
{
	//UINT8 u8ACamID = m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
	UINT8 u8ACamID = CheckSelectCam();
	uvCmn_Camera_MilAutoScale(DISP_TYPE_MARK_LIVE, u8ACamID);
	ZoomFlag[u8ACamID] = false;
}

/* desc: ���콺 �� �̺�Ʈ, Zoom In, Out ��� */
BOOL CDlgMark::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	//UINT8 u8ACamID = m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
	UINT8 u8ACamID = CheckSelectCam();
	CWnd* pImageWnd = GetDlgItem(IDC_MARK_PIC_FIND_GRAB);
	CRect rDraw;
	pImageWnd->GetClientRect(rDraw);

	WINDOWPLACEMENT wndpl;
	int idNo = IDC_MARK_PIC_FIND_GRAB;
	GetDlgItem(idNo)->GetWindowPlacement(&wndpl);
	CRect rectTmp;
	rectTmp = wndpl.rcNormalPosition;
	if (pt.x < rectTmp.left || pt.x > rectTmp.right
		|| pt.y < rectTmp.top || pt.y > rectTmp.bottom)
		return false;


	if (zDelta > 0) {
		uvCmn_Camera_MilZoomIn(DISP_TYPE_MARK_LIVE, u8ACamID, rDraw);
		ZoomFlag[u8ACamID] = true;
	}
	else {
		if(!uvCmn_Camera_MilZoomOut(DISP_TYPE_MARK_LIVE, u8ACamID, rDraw))
			ZoomFlag[u8ACamID] = false;
	}
	
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}

void CDlgMark::ShowCtrlPanel()
{
	if (AfxGetMainWnd()->GetSafeHwnd())
	{
		::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), eMSG_MAIN_OPEN_CONSOLE, NULL, NULL);
	}
}

VOID CDlgMark::SetMarkFindMode(BOOL bChg)
{
	if (!bpatFile && bmmfFile) {
		m_btn_ctl[eMARK_BTN_CTL_MARK_FIND_MODE].SetWindowTextW(_T("MOD"));
		mark_find_mode = 0;
		for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++)
			uvCmn_Camera_SetMarkFindMode(i + 1, mark_find_mode, 2);
	}
	else if (bChg) {
		if (mark_find_mode == 0 && bpatFile) {
			m_btn_ctl[eMARK_BTN_CTL_MARK_FIND_MODE].SetWindowTextW(_T("PAT"));
			mark_find_mode = 1;
			for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++)
				uvCmn_Camera_SetMarkFindMode(i + 1, mark_find_mode, 2);
		}
		else if (mark_find_mode == 1 && bmmfFile) {
			m_btn_ctl[eMARK_BTN_CTL_MARK_FIND_MODE].SetWindowTextW(_T("MOD"));
			mark_find_mode = 0;
			for (int i = 0; i < uvEng_GetConfig()->set_cams.acam_count; i++)
				uvCmn_Camera_SetMarkFindMode(i + 1, mark_find_mode, 2);
		}
	}
}


void CDlgMark::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// lk91 !! ���� �����ϱ� 231017
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	WINDOWPLACEMENT wndpl;
	GetDlgItem(IDC_MARK_PIC_MODEL_1)->GetWindowPlacement(&wndpl);
	if (point.x > wndpl.rcNormalPosition.left && point.x<wndpl.rcNormalPosition.right
		&& point.y>wndpl.rcNormalPosition.top && point.y < wndpl.rcNormalPosition.bottom)
	{
		//UINT8 u8ACamID = m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
		UINT8 u8ACamID = CheckSelectCam();
		//BOOL bSucc = FALSE;

		TCHAR tzName[MARK_MODEL_NAME_LENGTH] = { NULL };
		m_edt_txt[eMARK_EDT_TXT_MODEL_NAME].GetWindowTextW(tzName, MARK_MODEL_NAME_LENGTH);

		CString strFileName = _T("");

		m_edt_txt[eMARK_EDT_TXT_MODEL_NAME].GetWindowTextW(strFileName);

		// �ؽ�Ʈ �����Ͱ� �������.
		strFileName = strFileName.Trim();
		if (strFileName.IsEmpty())
		{
			return;
		}

		STG_CMPV stRecipe = { NULL };
		CUniToChar csCnv;

		strcpy_s(stRecipe.name, MARK_MODEL_NAME_LENGTH, csCnv.Uni2Ansi(tzName));

		CString strFile;

		//uvEng_Camera_SetCamMode(ENG_VCCM::en_live_mode);
		//strFile.Format(_T("%s\\data\\pat\\%s.pat"), g_tzWorkDir, strFileName);
		//if (!uvCmn_Camera_RegistPat(u8ACamID, um_rectArea, strFile, TMP_MARK)) {
		//	AfxMessageBox(_T("Grab Error!"));
		//	return;
		//}
		strFile.Format(_T("%s\\%s\\mmf\\%s.mmf"), g_tzWorkDir, CUSTOM_DATA_CONFIG, strFileName);
		//if (!uvCmn_Camera_RegistMod(u8ACamID, um_rectArea, strFile, TMP_MARK)) {
		//	AfxMessageBox(_T("Grab Error!"));
		//	return;
		//}

		// CString -> TCHAR * 
		TCHAR* tcharFileName = _T("");
		tcharFileName = (TCHAR*)(LPCTSTR)strFile;
		CDlgSetMark dlgSetMark;
		if (dlgSetMark.MyDoModal(u8ACamID, strFileName) == IDOK)
		{
			stRecipe.type = (UINT32)GetModelType();
			stRecipe.iSizeP = uvCmn_Camera_GetMarkSize(u8ACamID, TMP_MARK);
			stRecipe.iOffsetP = uvCmn_Camera_GetMarkOffset(u8ACamID, false, TMP_MARK);

			// Cstring -> char *
			wchar_t* wchar_str;
			int char_str_len;
			// 1. CString to wchar * conversion
			wchar_str = strFileName.GetBuffer(strFileName.GetLength());
			char_str_len = WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, NULL, 0, NULL, NULL);
			// 2. wchar_t* to char* conversion
			WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, stRecipe.file, char_str_len, 0, 0);

			uvEng_Mark_ModelAppend(&stRecipe, 0x01);

			/* �ٽ� �� ���� �ҷ� ���� */
			m_pGridModel->Load();
			/* �ʱ�ȭ */
			RecipeReset(0x00);
		}
	}
	CDlgMenu::OnLButtonDblClk(nFlags, point);
}

VOID CDlgMark::ProcImage()
{
	//UINT8 u8ACamID = m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
	UINT8 u8ACamID = CheckSelectCam();

	if (uvCmn_Camera_ProcImage(u8ACamID, m_chk_img[0].GetCheck())) {
		uvEng_Camera_DrawImageBitmap(DISP_TYPE_MARK_LIVE, u8ACamID - 1, u8ACamID);
	}
	else {
		CDlgMesg dlgMesg;
		dlgMesg.MyDoModal(L"Fail", 0x01);
	}
}


/*
 desc : Mark Merge �۾�, �ΰ��� mmf ���ϸ� ���
 parm : None
 retn : None
*/
VOID CDlgMark::MarkMerge()
{
	// 1. 2���� mmf ���� load
	TCHAR tzInit[MAX_PATH_LEN] = { NULL };
	PTCHAR ptzFileName = NULL;
	PTCHAR tzFileName = (PTCHAR)::Alloc(sizeof(TCHAR) * MAX_PATH_LEN);
	PTCHAR ptzPathFile = (PTCHAR)::Alloc(sizeof(TCHAR) * MAX_PATH_LEN);

	swprintf_s(ptzPathFile, MAX_PATH_LEN, L"%s\\%s\\mmf", g_tzWorkDir, CUSTOM_DATA_CONFIG);
	///* Reset the file buffer */
	//wmemset(ptzPathFile, 0x00, MAX_PATH_LEN);
	//wmemset(tzFileName, 0x00, MAX_PATH_LEN);

	///* Set the path of mmf directory */
	//swprintf_s(tzInit, MAX_PATH_LEN, L"%s\\data\\mmf", g_tzWorkDir);
	//if (!uvCmn_GetSelectFile(L"MARK File (*.mmf)|*.mmf",
	//	MAX_PATH_LEN, ptzPathFile,
	//	L"Please select the MMF file", tzInit))
	//{
	//	Free(ptzPathFile);
	//	return;
	//}
	//if (wcslen(ptzPathFile) < 1)
	//{
	//	Free(ptzPathFile);
	//	return;
	//}

	///* Ȯ���� �� ��ü ��� ������ ������ ���� �̸��� ���� */
	//ptzFileName = uvCmn_GetFileName(ptzPathFile, (UINT32)wcslen(ptzPathFile));
	//UINT32 tmp = UINT32(wcslen(ptzFileName) - 4);
	//wmemcpy(tzFileName, ptzFileName, UINT32(wcslen(ptzFileName) - 4));

	UINT8 u8Speed = (UINT8)uvEng_GetConfig()->mark_find.model_speed;
	UINT8 u8Level = (UINT8)uvEng_GetConfig()->mark_find.detail_level;
	DOUBLE dbSmooth = (DOUBLE)uvEng_GetConfig()->mark_find.model_smooth;
	DOUBLE dbScaleMin = 0.0f, dbScaleMax = 0.0f, dbScoreRate = 80.0f;
	//CUniToChar csCnv1, csCnv2;
	STG_CMPV stModel = { NULL };
	//bmmfFile = true;
	//bpatFile = false;
	//CString		strPathFile;

	//strPathFile.Format(_T("%s"), ptzFileName);
	////strPathFile.Format(_T("%s\\data\\mmf\\%s.mmf"), g_tzWorkDir, m_pDrawModel[i]->GetMMFFile());
	//stModel.type = (UINT32)m_pDrawModel[0]->GetModelType();		// lk91 m_pDrawModel[0] �����ϴ� ������ mark �ǿ����� �Ѱ��� ���(������ ��)
	for (int j = 0; j < 5; j++)	stModel.param[j] = m_pDrawModel[0]->GetParam(j);

	//TCHAR tzInit2[MAX_PATH_LEN] = { NULL };
	//PTCHAR ptzFileName2 = NULL;
	//PTCHAR tzFileName2 = (PTCHAR)::Alloc(sizeof(TCHAR) * MAX_PATH_LEN);
	//PTCHAR ptzPathFile2 = (PTCHAR)::Alloc(sizeof(TCHAR) * MAX_PATH_LEN);

	///* Reset the file buffer */
	//wmemset(ptzPathFile2, 0x00, MAX_PATH_LEN);
	//wmemset(tzFileName2, 0x00, MAX_PATH_LEN);

	///* Set the path of mmf directory */
	//swprintf_s(tzInit2, MAX_PATH_LEN, L"%s\\data\\mmf", g_tzWorkDir);
	//if (!uvCmn_GetSelectFile(L"MARK File (*.mmf)|*.mmf",
	//	MAX_PATH_LEN, ptzPathFile2,
	//	L"Please select the MMF file", tzInit2))
	//{
	//	Free(ptzPathFile2);
	//	return;
	//}
	//if (wcslen(ptzPathFile2) < 1)
	//{
	//	Free(ptzPathFile2);
	//	return;
	//}

	///* Ȯ���� �� ��ü ��� ������ ������ ���� �̸��� ���� */
	//ptzFileName2 = uvCmn_GetFileName(ptzPathFile2, (UINT32)wcslen(ptzPathFile2));
	//tmp = UINT32(wcslen(ptzFileName2) - 4);
	//wmemcpy(tzFileName2, ptzFileName2, UINT32(wcslen(ptzFileName2) - 4));


	TCHAR tzName[MARK_MODEL_NAME_LENGTH] = { NULL };
	m_edt_txt[eMARK_EDT_TXT_MODEL_NAME].GetWindowTextW(tzName, MARK_MODEL_NAME_LENGTH);

	uvEng_Camera_MergeMark(1, &stModel, u8Speed, u8Level, uvEng_GetConfig()->mark_find.max_mark_find, dbSmooth,
		TMP_MARK, ptzPathFile, ptzPathFile, tzName,
		dbScaleMin, dbScaleMax, dbScoreRate);


	/* Release the memory of file buffer */
	//Free(ptzPathFile2);
	//Free(tzFileName2);
	Free(ptzPathFile);
	Free(tzFileName);






	//AfxMessageBox(_T("������ ������ Mark 2���� ����Ͽ� Merge �ϴ� �۾��Դϴ�.\n�� ���� 3���̻��� Mark�� Merge �� ��� ù��° merge �۾��� ���� ū �������� Mark�� ���Խ��Ѿ� �մϴ�.\n\n[Merge ����]\n1.����Ž���� â�� �ι� �����Ǹ� merge �ϰ��� �ϴ� mmf ������ �������ּ���.\n2.merge �� ������ ~\mmf\merge\merge_result.mmf ���Ϸ� ����˴ϴ�.\n3.�� Merge �ϰ� ������� �ش� ��ο� �ִ� merge_result.mmf ���� ���� �����ϰ� ������ Mark�� �����ϸ� �˴ϴ�.\n"), MB_ICONINFORMATION);
	//// 1. 2���� mmf ���� load
	//TCHAR tzInit[MAX_PATH_LEN] = { NULL };
	//PTCHAR ptzFileName = NULL;
	//PTCHAR tzFileName = (PTCHAR)::Alloc(sizeof(TCHAR) * MAX_PATH_LEN);
	//PTCHAR ptzPathFile = (PTCHAR)::Alloc(sizeof(TCHAR) * MAX_PATH_LEN);

	///* Reset the file buffer */
	//wmemset(ptzPathFile, 0x00, MAX_PATH_LEN);
	//wmemset(tzFileName, 0x00, MAX_PATH_LEN);

	///* Set the path of mmf directory */
	//swprintf_s(tzInit, MAX_PATH_LEN, L"%s\\data\\mmf", g_tzWorkDir);
	//if (!uvCmn_GetSelectFile(L"MARK File (*.mmf)|*.mmf",
	//	MAX_PATH_LEN, ptzPathFile,
	//	L"Please select the MMF file", tzInit))
	//{
	//	Free(ptzPathFile);
	//	return;
	//}
	//if (wcslen(ptzPathFile) < 1)
	//{
	//	Free(ptzPathFile);
	//	return;
	//}

	///* Ȯ���� �� ��ü ��� ������ ������ ���� �̸��� ���� */
	//ptzFileName = uvCmn_GetFileName(ptzPathFile, (UINT32)wcslen(ptzPathFile));
	//UINT32 tmp = UINT32(wcslen(ptzFileName) - 4);
	//wmemcpy(tzFileName, ptzFileName, UINT32(wcslen(ptzFileName) - 4));

	//UINT8 u8Speed = (UINT8)uvEng_GetConfig()->mark_find.model_speed;
	//UINT8 u8Level = (UINT8)uvEng_GetConfig()->mark_find.detail_level;
	//DOUBLE dbSmooth = (DOUBLE)uvEng_GetConfig()->mark_find.model_smooth;
	//DOUBLE dbScaleMin = 0.0f, dbScaleMax = 0.0f, dbScoreRate = 80.0f;
	//CUniToChar csCnv1, csCnv2;
	//STG_CMPV stModel = { NULL };
	//bmmfFile = true;
	//bpatFile = false;
	//CString		strPathFile;
	//
	//strPathFile.Format(_T("%s"), ptzFileName);
	////strPathFile.Format(_T("%s\\data\\mmf\\%s.mmf"), g_tzWorkDir, m_pDrawModel[i]->GetMMFFile());
	//stModel.type = (UINT32)m_pDrawModel[0]->GetModelType();		// lk91 m_pDrawModel[0] �����ϴ� ������ mark �ǿ����� �Ѱ��� ���(������ ��)
	//for (int j = 0; j < 5; j++)	stModel.param[j] = m_pDrawModel[0]->GetParam(j);

	//TCHAR tzInit2[MAX_PATH_LEN] = { NULL };
	//PTCHAR ptzFileName2 = NULL;
	//PTCHAR tzFileName2 = (PTCHAR)::Alloc(sizeof(TCHAR) * MAX_PATH_LEN);
	//PTCHAR ptzPathFile2 = (PTCHAR)::Alloc(sizeof(TCHAR) * MAX_PATH_LEN);

	///* Reset the file buffer */
	//wmemset(ptzPathFile2, 0x00, MAX_PATH_LEN);
	//wmemset(tzFileName2, 0x00, MAX_PATH_LEN);

	///* Set the path of mmf directory */
	//swprintf_s(tzInit2, MAX_PATH_LEN, L"%s\\data\\mmf", g_tzWorkDir);
	//if (!uvCmn_GetSelectFile(L"MARK File (*.mmf)|*.mmf",
	//	MAX_PATH_LEN, ptzPathFile2,
	//	L"Please select the MMF file", tzInit2))
	//{
	//	Free(ptzPathFile2);
	//	return;
	//}
	//if (wcslen(ptzPathFile2) < 1)
	//{
	//	Free(ptzPathFile2);
	//	return;
	//}

	///* Ȯ���� �� ��ü ��� ������ ������ ���� �̸��� ���� */
	//ptzFileName2 = uvCmn_GetFileName(ptzPathFile2, (UINT32)wcslen(ptzPathFile2));
	//tmp = UINT32(wcslen(ptzFileName2) - 4);
	//wmemcpy(tzFileName2, ptzFileName2, UINT32(wcslen(ptzFileName2) - 4));


	//TCHAR tzName[MARK_MODEL_NAME_LENGTH] = { NULL };
	//m_edt_txt[eMARK_EDT_TXT_MODEL_NAME].GetWindowTextW(tzName, MARK_MODEL_NAME_LENGTH);

	//uvEng_Camera_MergeMark(1, &stModel, u8Speed, u8Level, uvEng_GetConfig()->mark_find.max_mark_find, dbSmooth,
	//	TMP_MARK, ptzPathFile, ptzPathFile2, tzName,
	//	dbScaleMin, dbScaleMax, dbScoreRate);

	//	
	///* Release the memory of file buffer */
	//Free(ptzPathFile2);
	//Free(tzFileName2);
	//Free(ptzPathFile);
	//Free(tzFileName);
}

VOID CDlgMark::setVisionCalib()
{
	calib_row = GetDlgItemInt(IDC_MARK_EDT_CALIB_R);
	calib_col = GetDlgItemInt(IDC_MARK_EDT_CALIB_C);

	SetDlgItemInt(IDC_MARK_EDT_CALIB_R, calib_row);
	SetDlgItemInt(IDC_MARK_EDT_CALIB_C, calib_col);

	free(CalibROI_left);
	free(CalibROI_right);
	free(CalibROI_top);
	free(CalibROI_bottom);

	CalibROI_left = (int*)malloc(sizeof(int) * (calib_row * calib_col));
	CalibROI_right = (int*)malloc(sizeof(int) * (calib_row * calib_col));
	CalibROI_top = (int*)malloc(sizeof(int) * (calib_row * calib_col));
	CalibROI_bottom = (int*)malloc(sizeof(int) * (calib_row * calib_col));

	memset(CalibROI_left, 0, sizeof(int) * (calib_row * calib_col));
	memset(CalibROI_right, 0, sizeof(int) * (calib_row * calib_col));
	memset(CalibROI_top, 0, sizeof(int) * (calib_row * calib_col));
	memset(CalibROI_bottom, 0, sizeof(int) * (calib_row * calib_col));
}

VOID CDlgMark::VisionCalib()
{
	//UINT8 u8ACamID = m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck() ? 0x01 : 0x02;
	UINT8 u8ACamID = CheckSelectCam();
	TCHAR tzVal[64] = { NULL };
	UINT64 u64Tick = GetTickCount64();
	CDlgMesg dlgMesg;

	/* ���� ��� �� �ʱ�ȭ */
	m_edt_out[eMARK_EDT_OUT_CENT_ERR_X].SetWindowTextW(L"0.0000 ");
	m_edt_out[eMARK_EDT_OUT_CENT_ERR_Y].SetWindowTextW(L"0.0000 ");
	m_edt_out[eMARK_EDT_OUT_SCALE_RATE].SetWindowTextW(L"0.0000 ");	/* Scale */
	m_edt_out[eMARK_EDT_OUT_SCORE_RATE].SetWindowTextW(L"0.0000 ");	/* Score */

	if (uvEng_Camera_GetCamMode() != ENG_VCCM::en_image_mode) {
		/* Live ���� ���� */
		if (m_chk_cam[eMARK_CHK_CAM_LIVE].GetCheck())
		{
			dlgMesg.MyDoModal(L"Live Mode is currently on", 0x01);
			return;
		}
		/* Mark Model�� ��ϵǾ� �ִ��� ���� */
		if (!uvEng_Camera_IsSetMarkModelACam(u8ACamID, 2))
		{
			dlgMesg.MyDoModal(L"No Mark Model registered", 0x01);
			return;
		}
		/* ī�޶� Grabbed Mode�� Calibration Mode�� ���� */
		uvEng_Camera_SetCamMode(ENG_VCCM::en_cali_mode);
		/* Camera �ʿ� Trigger Event ������ 1�� �߻� */
		if (!uvEng_Mvenc_ReqTrigOutOne(u8ACamID))
		{
			dlgMesg.MyDoModal(L"Failed to send the Trigger Event", 0x01);
			return;
		}
	}
	uvEng_Camera_DrawImageBitmap(DISP_TYPE_MARK_LIVE, u8ACamID - 1, u8ACamID);

	/* Grabbed Image�� ��Ī ����� �����ϴ��� ���� Ȯ�� */
	uvEng_Camera_RunModel_VisionCalib(u8ACamID, (UINT8)DISP_TYPE_MARK_LIVE, TMP_MARK, CalibROI_left, CalibROI_right, CalibROI_top, CalibROI_bottom, calib_row, calib_col);

}

/*
 desc : UI���� ������ Cam ��ȣ Ȯ��
 parm : None
 retn : None
*/
UINT8 CDlgMark::CheckSelectCam()
{
	UINT8 u8ACamID;

	/*�� Cam�� �ش�Ǵ� Check Box Ȯ���Ͽ� ���õ� Cam ��ȣ ����*/
	if (m_chk_cam[eMARK_CHK_CAM_ACAM_1].GetCheck())			u8ACamID = 0x01;
	else if (m_chk_cam[eMARK_CHK_CAM_ACAM_2].GetCheck())	u8ACamID = 0x02;
	else                                                    u8ACamID = 0x03;

	return u8ACamID;
}


VOID CDlgMark::InitGridStrobeView()
{
	CResizeUI	clsResizeUI;
	CRect		rGrid;
	std::vector <int>			vRowSize(4);
	std::vector <int>			vColSize(3);

	 uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[0];

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	std::vector <std::wstring>	vTitle = { _T("Amber1"), _T("IR1"), _T("Coaxial1") };
	std::vector <std::wstring>	vTitle2 = { _T("Amber2"), _T("IR2"), _T("Coaxial2") }
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	std::vector <std::wstring>	vTitle = { _T("Amber_IR1"), _T("Amber_IR2"), _T("Amber_IR3") };
	std::vector <std::wstring>	vTitle2 = { _T("Coaxial1"), _T("Coaxial2"), _T("Coaxial3") }
#endif
;
	std::vector <std::wstring>	vItem = { _T("HEAD"), _T("0.0000"), _T("0.0000"), _T("0.0000"), _T("READ"), _T("MOVE") };
	std::vector <COLORREF>		vColColor = { ALICE_BLUE, WHITE_, WHITE_, WHITE_, DEF_COLOR_BTN_PAGE_NORMAL, DEF_COLOR_BTN_PAGE_NORMAL };
	std::vector <COLORREF>		vColTextColor = { BLACK_, BLACK_, BLACK_, BLACK_, WHITE_, WHITE_ };

	CGridCtrl* pGrid = &m_grd_ctl[eMARK_GRD_STROBE_VIEW];

	clsResizeUI.ResizeControl(this, pGrid);
	pGrid->GetWindowRect(rGrid);
	this->ScreenToClient(rGrid);

	int nHeight = 30;
	int nTotalHeight = 0;
	int nTotalWidth = 0;

	for (auto& height : vRowSize)
	{
		height = nHeight;
		nTotalHeight += height;
	}

	if (rGrid.Height() < nTotalHeight)
	{
		nTotalHeight = 0;

		for (auto& height : vRowSize)
		{
			height = (rGrid.Height() - 1) / (int)vRowSize.size();
			nTotalHeight += height;
		}
	}

	for (auto& width : vColSize)
	{
		width = (rGrid.Width() ) / (int)vColSize.size();
		nTotalWidth += width;
	}

	vColSize.front() += (rGrid.Width() - 1) - nTotalWidth;

	/* ��ü �⺻ ���� */
	pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	pGrid->SetRowCount((int)vRowSize.size());
	pGrid->SetColumnCount((int)vColSize.size());
	pGrid->SetFixedColumnCount(0);
	pGrid->SetGridLineColor(BLACK_);
	pGrid->SetBkColor(WHITE_);
	pGrid->SetFixedColumnSelection(0);

	/* Ÿ��Ʋ (ù ��° ��) */
	//for (int i = 0; i < (int)vColSize.size(); i++)
	//{
	//	pGrid->SetItemFormat(0, i, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	//	pGrid->SetColumnWidth(i, vColSize[i]);
	//}
	UINT16 LampValue;
	for (int nRow = 0; nRow < (int)vRowSize.size(); nRow++)
	{
		pGrid->SetRowHeight(nRow, vRowSize[nRow]);

		for (int nCol = 0; nCol < (int)vColSize.size(); nCol++)
		{
			/*ù ��°�� ����*/
			if (0 == nRow)
			{
				pGrid->SetItemTextFmt(nRow, nCol, _T("%s"), vTitle[nCol].c_str());

				pGrid->SetItemBkColour(nRow, nCol, vColColor[0]);
				pGrid->SetItemFgColour(nRow, nCol, vColTextColor[0]);
			}
			/*�� ��°�� ����*/
			else if(2 == nRow)
			{
				pGrid->SetItemTextFmt(nRow, nCol, _T("%s"), vTitle2[nCol].c_str());

				pGrid->SetItemBkColour(nRow, nCol, vColColor[0]);
				pGrid->SetItemFgColour(nRow, nCol, vColTextColor[0]);
			}

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
			// 0 1 2
			// 0 1 2
			// 0 1 2
			// 4 5 6
			else if (1 == nRow)
			{
				LampValue = uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[nCol];
				pGrid->SetItemTextFmt(nRow, nCol, _T("%d"), uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[nCol]);

				pGrid->SetItemBkColour(nRow, nCol, vColColor[1]);
				pGrid->SetItemFgColour(nRow, nCol, vColTextColor[0]);
		}
			else if (3 == nRow)
			{
				LampValue = uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[nCol + vColSize.size()];
				pGrid->SetItemTextFmt(nRow, nCol, _T("%d"), uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[nCol + vColSize.size()]);

				pGrid->SetItemBkColour(nRow, nCol, vColColor[1]);
				pGrid->SetItemFgColour(nRow, nCol, vColTextColor[0]);
			}
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
			// 0 1 2
			// 0 1 2
			// 0 2 4 
			// 1 3 5
			else if (1 == nRow)
			{
				LampValue = uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[nCol];
				pGrid->SetItemTextFmt(nRow, nCol, _T("%d"), uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[nCol*2]);

				pGrid->SetItemBkColour(nRow, nCol, vColColor[1]);
				pGrid->SetItemFgColour(nRow, nCol, vColTextColor[0]);
			}
			else if (3 == nRow)
			{
				LampValue = uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[nCol + vColSize.size()];
				pGrid->SetItemTextFmt(nRow, nCol, _T("%d"), uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[(nCol * 2) +1]);

				pGrid->SetItemBkColour(nRow, nCol, vColColor[1]);
				pGrid->SetItemFgColour(nRow, nCol, vColTextColor[0]);
			}
#endif

			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
	}

	/* Grid Size �籸�� */
	rGrid.bottom = rGrid.top + nTotalHeight + 1;
	pGrid->MoveWindow(rGrid);

	/* �⺻ �Ӽ� �� ���� */
	pGrid->SetBaseGridProp(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	pGrid->EnableSelection(FALSE);
	pGrid->Refresh();
}

VOID CDlgMark::UpdataStrobeView()
{
	CGridCtrl* pGrid = &m_grd_ctl[eMARK_GRD_STROBE_VIEW];

	UINT16 LampValue;
	for (int nRow = 0; nRow < (int)pGrid->GetRowCount(); nRow++)
	{
		for (int nCol = 0; nCol < (int)pGrid->GetColumnCount(); nCol++)
		{

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
			// 0 1 2
			// 0 1 2
			// 0 1 2
			// 4 5 6
			
			if (1 == nRow)
			{
				LampValue = uvEng_GetConfig()->set_strobe_lamp.u16BufferValue[nCol];
				pGrid->SetItemTextFmt(nRow, nCol, _T("%d"), uvEng_GetConfig()->set_strobe_lamp.u16BufferValue[nCol]);

			}
			else if (3 == nRow)
			{
				LampValue = uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[nCol + 1 + vColSize.size()];
				pGrid->SetItemTextFmt(nRow, nCol, _T("%d"), uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[nCol + 1 + vColSize.size()]);

			}
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
			// 0 1 2
			// 0 1 2
			// 0 2 4 
			// 1 3 5
			auto temp = uvEng_GetConfig()->set_strobe_lamp;
			if (1 == nRow)
			{
				pGrid->SetItemTextFmt(nRow, nCol, _T("%d"), temp.u16StrobeValue[nCol * 2]);

			}
			else if (3 == nRow)
			{
				pGrid->SetItemTextFmt(nRow, nCol, _T("%d"), temp.u16StrobeValue[(nCol * 2) + 1]);

			}
#endif

			pGrid->SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
	}

	pGrid->Refresh();
}

VOID CDlgMark::setStrobeValue()
{
	CGridCtrl* pGrid = &m_grd_ctl[eMARK_GRD_STROBE_VIEW];

	UINT16 recvStrobeValues[8];
#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15)
	GlobalVariables::getInstance()->ResetCounter("strobeRecved");
	/*Cam1*/
	uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[0] = (UINT16)pGrid->GetItemTextToInt(1, 0);
	uvEng_StrobeLamp_Send_ChannelStrobeControl(0, 0, uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[0]);
	Sleep(100);
	uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[1] = (UINT16)pGrid->GetItemTextToInt(1, 1);
	uvEng_StrobeLamp_Send_ChannelStrobeControl(0, 1, uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[1]);
	Sleep(100);
	uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[2] = (UINT16)pGrid->GetItemTextToInt(1, 2);
	uvEng_StrobeLamp_Send_ChannelStrobeControl(0, 2, uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[2]);
	Sleep(100);

	/*Cam2*/
	uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[4] = (UINT16)pGrid->GetItemTextToInt(3, 0);
	uvEng_StrobeLamp_Send_ChannelStrobeControl(0, 4, uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[4]);
	Sleep(100);
	uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[5] = (UINT16)pGrid->GetItemTextToInt(3, 1);
	uvEng_StrobeLamp_Send_ChannelStrobeControl(0, 5, uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[5]);
	Sleep(100);
	uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[6] = (UINT16)pGrid->GetItemTextToInt(3, 2);
	uvEng_StrobeLamp_Send_ChannelStrobeControl(0, 6, uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[6]);
	Sleep(100);

	GlobalVariables::getInstance()->Waiter("strobe",
		[]
		{
			return GlobalVariables::getInstance()->GetCount("strobeRecved") == 6;
		},
		[]
		{
			MessageBoxEx(nullptr, _T("Write succeed.\t"), _T("ok"), MB_OK, LANG_ENGLISH);
		},
		[&]
		{
			std::wstring info = L"Write timeout. send = 6, receive = " + std::to_wstring(GlobalVariables::getInstance()->GetCount("strobeRecved")) + L"\t";
			MessageBoxEx(nullptr, info.c_str(), _T("failed"), MB_OK, LANG_ENGLISH);
		}
	);
#elif(DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)

	GlobalVariables::GetInstance()->ResetCounter("strobeRecved");

	/*Cam1*/
	uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[0] = (UINT16)pGrid->GetItemTextToInt(1, 0);
	recvStrobeValues[0] = uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[0];
	uvEng_StrobeLamp_Send_ChannelStrobeControl(0, 0, uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[0]);
	Sleep(100);
	uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[1] = (UINT16)pGrid->GetItemTextToInt(3, 0);
	recvStrobeValues[1] = uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[1];
	uvEng_StrobeLamp_Send_ChannelStrobeControl(0, 1, uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[1]);
	Sleep(100);
	/*Cam2*/
	uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[2] = (UINT16)pGrid->GetItemTextToInt(1, 1);
	recvStrobeValues[2] = uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[2];
	uvEng_StrobeLamp_Send_ChannelStrobeControl(0, 2, uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[2]);
	Sleep(100);
	uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[3] = (UINT16)pGrid->GetItemTextToInt(3, 1);
	recvStrobeValues[3] = uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[3];
	uvEng_StrobeLamp_Send_ChannelStrobeControl(0, 3, uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[3]);
	Sleep(100);
	/*Cam3*/
	uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[4] = (UINT16)pGrid->GetItemTextToInt(1, 2);
	recvStrobeValues[4] = uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[4];
	uvEng_StrobeLamp_Send_ChannelStrobeControl(0, 4, uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[4]);
	Sleep(100);
	uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[5] = (UINT16)pGrid->GetItemTextToInt(3, 2);
	recvStrobeValues[5] = uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[5];
	uvEng_StrobeLamp_Send_ChannelStrobeControl(0, 5, uvEng_GetConfig()->set_strobe_lamp.u16StrobeValue[5]);
	Sleep(100);

	GlobalVariables::GetInstance()->Waiter("strobe",
	[]
	{
		return GlobalVariables::GetInstance()->GetCount("strobeRecved") == 6;
	},
	[]
	{
		MessageBoxEx(nullptr, _T("Write succeed.\t"), _T("ok"), MB_OK, LANG_ENGLISH);
	},
	[&]
	{
		std::wstring info = L"Write timeout. send = 6, receive = " + std::to_wstring(GlobalVariables::GetInstance()->GetCount("strobeRecved")) + L"\t";
		MessageBoxEx(nullptr, info.c_str(), _T("failed"), MB_OK, LANG_ENGLISH);
	}
	,10000);

	#endif

	
	uvEng_SaveConfig();
}

void CDlgMark::OnClickGridInput(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	CGridCtrl* pGrid = &m_grd_ctl[eMARK_GRD_STROBE_VIEW];

	CString strOutput;
	CString strInput = pGrid->GetItemText(pItem->iRow, pItem->iColumn);
	double	dMin = 0;
	double	dMax = 1000;

	if (1 == pItem->iRow || 3 == pItem->iRow)
	{
		if (PopupKBDN(ENM_DITM::en_double, strInput, strOutput, dMin, dMax, 0))
		{
			pGrid->SetItemTextFmt(pItem->iRow, pItem->iColumn, _T("%s"), strOutput);
		}
	}

	pGrid->Refresh();
}

BOOL CDlgMark::PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts/* = 0*/)
{
	TCHAR tzTitle[1024] = { NULL }, tzPoint[512] = { NULL }, tzMinMax[2][32] = { NULL };
	CDlgKBDN dlg;

	wcscpy_s(tzMinMax[0], 32, L"Min");
	wcscpy_s(tzMinMax[1], 32, L"Max");

	switch (enType)
	{
	case ENM_DITM::en_int8:
	case ENM_DITM::en_int16:
	case ENM_DITM::en_int32:
	case ENM_DITM::en_int64:
	case ENM_DITM::en_uint8:
	case ENM_DITM::en_uint16:
		swprintf_s(tzPoint, 512, L"%s:%%.%df,%s:%%.%df", tzMinMax[0], u8DecPts, tzMinMax[1], u8DecPts);
		swprintf_s(tzTitle, 1024, tzPoint, dMin, dMax);
		if (IDOK != dlg.MyDoModal(tzTitle, FALSE, TRUE, dMin, dMax))	return FALSE;	break;
	case ENM_DITM::en_uint32:
	case ENM_DITM::en_uint64:
		swprintf_s(tzPoint, 512, L"%s:%%.%df,%s:%%.%df", tzMinMax[0], u8DecPts, tzMinMax[1], u8DecPts);
		swprintf_s(tzTitle, 1024, tzPoint, dMin, dMax);
		if (IDOK != dlg.MyDoModal(tzTitle, FALSE, TRUE, dMin, dMax))	return FALSE;	break;
	case ENM_DITM::en_float:
	case ENM_DITM::en_double:
		swprintf_s(tzPoint, 512, L"%s:%%.%df,%s:%%.%df", tzMinMax[0], u8DecPts, tzMinMax[1], u8DecPts);
		swprintf_s(tzTitle, 1024, tzPoint, dMin, dMax);
		if (IDOK != dlg.MyDoModal(tzTitle, TRUE, TRUE, dMin, dMax))	return FALSE;	break;
	}
	switch (enType)
	{
	case ENM_DITM::en_int8:
	case ENM_DITM::en_int16:
	case ENM_DITM::en_int32:
	case ENM_DITM::en_int64:
	case ENM_DITM::en_uint8:
	case ENM_DITM::en_uint16:
	case ENM_DITM::en_uint32:
	case ENM_DITM::en_uint64:
		strOutput.Format(_T("%d"), dlg.GetValueInt32());
		break;
	case ENM_DITM::en_float:
	case ENM_DITM::en_double:
		swprintf_s(tzPoint, 512, L"%%.%df", u8DecPts);
		strOutput.Format(tzPoint, dlg.GetValueDouble());
	}

	return TRUE;
}