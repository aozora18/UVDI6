
/*
 desc : Exposure
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgExpo.h"


#include "./expo/DrawMark.h"	/* Mark Search Results Object */
#include "./expo/DrawDev.h"		/* TCP/IP Communication Status */
#include "./expo/MarkDist.h"	/* Distance between marks */
#include "./expo/ExpoVal.h"		/* Update to realtime value */
#include "./expo/TempAlarm.h"	/* Temperature Alarm */

#include "DlgMmpm.h"
#include "../pops/DlgStep.h"
#include "../pops/DlgRept.h"
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
CDlgExpo::CDlgExpo(UINT32 id, CWnd* parent)
	: CDlgMenu(id, parent)
{
	m_pDrawMark	= NULL;
	m_pDrawDev	= NULL;
	m_pMarkDist	= NULL;
	m_pExpoVal	= NULL;
	m_pTempAlarm= NULL;

	/* �ڽ��� ���̾�α� ID ���� */
	m_enDlgID	= ENG_CMDI::en_menu_expo;

	m_u8MarkNo	= 0x00;	/* 0: All, 1 ~ 4 (only global mark) */
	m_u8MarkPage= 0x01;	/* ���� ��� ����� Mark Page Number */

	m_u64ReqTime= GetTickCount64();
}

CDlgExpo::~CDlgExpo()
{
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgExpo::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;
	 
	DDX_Control(dx, IDC_ADDALIGNOFFSET, includeAlignOffsetBtn);

	/* groupbox - normal */
	u32StartID	= IDC_EXPO_GRP_TACT_TIME;
	for (i=0; i<eEXPO_GRP_MAX; i++)				DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* button - normal */
	u32StartID	= IDC_EXPO_BTN_RESET;
	for (i=0; i< eEXPO_BTN_MAX; i++)			DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* progress - normal */
	u32StartID	= IDC_EXPO_PGR_EXPO_RATE;
	for (i=0; i< eEXPO_PGR_MAX; i++)			DDX_Control(dx, u32StartID+i,	m_pgr_ctl[i]);

	/* static - normal */
	u32StartID	= IDC_EXPO_TXT_EXPO_WORKS;
	for (i=0; i<eEXPO_TXT_CTL_MAX; i++)			DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* static - device */
	//u32StartID	= IDC_EXPO_TXT_SERVICE;
	//for (i=0; i< eEXPO_TXT_DEV_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_txt_dev[i]);
	/* static - mark search result */
	u32StartID	= IDC_EXPO_TXT_MARK1;
	for (i=0; i< eEXPO_TXT_MAK_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_txt_mak[i]);
	/* static - data value */
	u32StartID	= IDC_EXPO_TXT_TACT_LAST;
	for (i=0; i< eEXPO_TXT_VAL_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_txt_val[i]);
	/* static - subject title */
	u32StartID	= IDC_EXPO_TXT_MARK_TOP_HORZ;
	for (i=0; i< eEXPO_TXT_DST_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_txt_dst[i]);
	/* static - temperature alarm */
	//u32StartID	= IDC_EXPO_TXT_ALARM_DI;
	//for (i=0; i< eEXPO_TXT_TMP_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_txt_tmp[i]);
	/* static - mark search result */
	u32StartID	= IDC_EXPO_PIC_MARK_1;
	for (i=0; i< eEXPO_PIC_MAK_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_pic_mak[i]);
	/* static - picture - device */
	//u32StartID	= IDC_EXPO_PIC_SERVICE;
	//for (i=0; i< eEXPO_PIC_DEV_MAX; i++)		DDX_Control(dx, u32StartID+i,	m_pic_dev[i]);

	u32StartID = IDC_EXPO_BTN_RESET;
	for (i = 0; i < eEXPO_BTN_MAX; i++)			DDX_Control(dx, u32StartID + i, m_btn_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgExpo, CDlgMenu)
 	ON_CONTROL_RANGE(BN_CLICKED, IDC_EXPO_BTN_RESET, IDC_EXPO_BTN_MARK_NEXT,	OnBtnClick)
	ON_BN_CLICKED(IDC_ADDALIGNOFFSET, &CDlgExpo::OnBnClickedAddalignoffset)
	ON_CONTROL_RANGE(STN_DBLCLK,IDC_EXPO_PIC_MARK_1, IDC_EXPO_PIC_MARK_4 ,&CDlgExpo::OnStnDblclickExpoPicMarks)

END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgExpo::PreTranslateMessage(MSG* msg)
{
	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgExpo::OnInitDlg()
{
	/* ��Ʈ�� �ʱ�ȭ */
	InitCtrl();
	/* ��ü �ʱ�ȭ */
	if (!InitObject())	return FALSE;

	InitDispMark();
	
	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgExpo::OnExitDlg()
{
	CloseObject();
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgExpo::OnPaintDlg(CDC *dc)
{
	/* ���� Ȥ�� ���� �ֱٿ� �˻��� ��ũ ��� ���� ��� (����) */
	if (m_pDrawMark)	m_pDrawMark->DrawPage(m_u8MarkPage);
	if (m_pDrawDev)		m_pDrawDev->DrawDevAll();
	if (m_pMarkDist)	m_pMarkDist->DrawDist();
	if (m_pTempAlarm)	m_pTempAlarm->DrawTempAlarm();

	/* �ڽ� ���� ȣ�� */
	CDlgMenu::DrawOutLine();
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgExpo::OnResizeDlg()
{
}

/*
 desc : �θ� �����忡 ���� �ֱ������� ȣ���
 parm : tick	- [in]  ���� CPU �ð�
		is_busy	- [in]  TRUE: ���� �ó����� ���� �� ..., FALSE: ���� Idle ����
 retn : None
*/
VOID CDlgExpo::UpdatePeriod(UINT64 tick, BOOL is_busy)
{
	/* ��� ���� ����Ǿ�����, ��� ���� ������ ���� */
	if (m_pDrawDev)		m_pDrawDev->DrawDev();
	if (m_pTempAlarm)	m_pTempAlarm->DrawTempAlarm();
#if 0
 	if (m_pDrawMark)	m_pDrawMark->DrawPage(dc->m_hDC, m_u8MarkPage);
	if (m_pMarkDist)	m_pMarkDist->DrawDist();
#endif
	if (m_pExpoVal)		m_pExpoVal->DrawExpo();
	/* Control Enable or Disable */
	UpdateControl(tick, is_busy);

	//�ڵ� ���� �Ʒ�ó�� �ٲټ���.
//	if (!is_busy)  �����ڵ�
//	{
//		/* Luria�� �ʱ�ȭ �Ǿ����� ���� Ȯ�� */
//		if (uvEng_Luria_IsServiceInited())
//		{
//			/* ���� �ֱ⸶�� �µ� �� ��û */
//			if (m_u64ReqTime+2000 < GetTickCount64())
//			{
//				/* ���� �ֱٿ� ��û�� �ð� ���� */
//				m_u64ReqTime	= GetTickCount64();
//				/* ��� ���а� ���� LED�� Board�� �µ� �� ��û */
//#if 0
//				uvEng_Luria_ReqGetPhLedTempFreqAll(0);
//#else
//				uvEng_Luria_ReqGetPhLedTempAll();
//#endif
//			}
//		}
//	}
	
	//if �˻縦 �����˻� (������ false�϶� return) �� �����Ұ�. 
	//�ڵ����� �ٰ�, ���� �پ��� �������� �������� ���� ��Ȯ����.
	if (is_busy) return;
	if (uvEng_Luria_IsServiceInited() == false) return;
	if (m_u64ReqTime + 2000 > GetTickCount64()) return;
	
	m_u64ReqTime	= GetTickCount64();
	uvEng_Luria_ReqGetPhLedTempAll();
}

/*
 desc : ��Ʈ��
 parm : tick	- [in]  ���� CPU �ð�
		is_busy	- [in]  TRUE: ���� �ó����� ���� �� ..., FALSE: ���� Idle ����
 retn : None
*/
VOID CDlgExpo::UpdateControl(UINT64 tick, BOOL is_busy)
{
	UINT16 u16Marks	= uvEng_Camera_GetGrabbedCount();
	//BOOL bError		= uvEng_IsEngineAlarm();
	LPG_RJAF recipe = uvEng_JobRecipe_GetSelectRecipe();
	BOOL bSelect	= recipe ? TRUE : FALSE;
	BOOL bLoaded	= uvCmn_Luria_IsJobNameLoaded();
	BOOL bMarked	= uvEng_Luria_IsMarkGlobal();
	
	ENG_AMOS motion = GlobalVariables::GetInstance()->GetAlignMotion().markParams.alignMotion;

	/* ��� ���ο� ������ �߻��� ���, ������ Disabled */
#if (CUSTOM_CODE_WAIT_JOB == 0)
	if (bError)	is_busy	= TRUE;
#endif
	if (m_btn_ctl[eEXPO_BTN_RESET].IsWindowEnabled() != (!is_busy))									m_btn_ctl[eEXPO_BTN_RESET].EnableWindow(!is_busy);								/* Reset */
	if (m_btn_ctl[eEXPO_BTN_RECIPE_LOAD].IsWindowEnabled() != (!bLoaded && bSelect && !is_busy))	m_btn_ctl[eEXPO_BTN_RECIPE_LOAD].EnableWindow(!bLoaded && bSelect && !is_busy);	/* Recipe Loaded */
	if (m_btn_ctl[eEXPO_BTN_VACUMM_ONOFF].IsWindowEnabled() != (!is_busy))							m_btn_ctl[eEXPO_BTN_VACUMM_ONOFF].EnableWindow(bLoaded && !is_busy);			/* Vacuum on off */
	if (m_btn_ctl[eEXPO_BTN_EXPO_ALIGN].IsWindowEnabled() != (bLoaded && !is_busy && bMarked))		m_btn_ctl[eEXPO_BTN_EXPO_ALIGN].EnableWindow(bLoaded && !is_busy && bMarked);	/* Expose Align */
	if (m_btn_ctl[eEXPO_BTN_EXPO_DIRECT].IsWindowEnabled() != (bLoaded && !is_busy))				m_btn_ctl[eEXPO_BTN_EXPO_DIRECT].EnableWindow(bLoaded && !is_busy);				/* Expose Direct */
	if (m_btn_ctl[eEXPO_BTN_CHECK_ALIGN].IsWindowEnabled() != (bLoaded && !is_busy && bMarked))		m_btn_ctl[eEXPO_BTN_CHECK_ALIGN].EnableWindow(bLoaded && !is_busy && bMarked);	/* Check Align */
	if (m_btn_ctl[eEXPO_BTN_MARK_NO].IsWindowEnabled() != (bLoaded && !is_busy && bMarked))			m_btn_ctl[eEXPO_BTN_MARK_NO].EnableWindow(bLoaded && !is_busy && bMarked);		/* Mark Number */
	if (m_btn_ctl[eEXPO_BTN_MARK_PREV].IsWindowEnabled() != (bSelect && u16Marks > 0))				m_btn_ctl[eEXPO_BTN_MARK_PREV].EnableWindow(bSelect && u16Marks > 0);			/* Mark Prev */
	if (m_btn_ctl[eEXPO_BTN_MARK_NEXT].IsWindowEnabled() != (bSelect && u16Marks > 0))				m_btn_ctl[eEXPO_BTN_MARK_NEXT].EnableWindow(bSelect && u16Marks > 0);			/* Mark Next */
	if (includeAlignOffsetBtn.IsWindowEnabled() != (bLoaded && motion == ENG_AMOS::en_static_3cam && !is_busy && bMarked))		includeAlignOffsetBtn.EnableWindow(bLoaded && !is_busy && bMarked);
	
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgExpo::InitCtrl()
{
	// by sysandj : Resize UI
	CResizeUI clsResizeUI;
	// by sysandj : Resize UI

	INT32 i = 0;

	clsResizeUI.ResizeControl(this, &m_grp_ctl[i]);

	/* group box */
	for (i = 0; i < eEXPO_GRP_MAX; i++)
	{
		m_grp_ctl[i].SetLogFont(&g_lf[eFONT_LEVEL2_BOLD]);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_grp_ctl[i]);
		// by sysandj : Resize UI
	}
	/* button - normal */

	for (i=0; i< eEXPO_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}

	includeAlignOffsetBtn.SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
	includeAlignOffsetBtn.SetBgColor(g_clrBtnColor);
	includeAlignOffsetBtn.SetTextColor(g_clrBtnTextColor);
	clsResizeUI.ResizeControl(this, &includeAlignOffsetBtn);

	

	/* static - Normal */
	for (i=0; i< eEXPO_TXT_CTL_MAX; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_ctl[i]);
		// by sysandj : Resize UI
	}
	/* static - Device Status (TCP/IP or Power) */
	//for (i=0; i< eEXPO_TXT_DEV_MAX; i++)
	//{
	//	m_txt_dev[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
	//	m_txt_dev[i].SetDrawBg(1);
	//	m_txt_dev[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

	//	// by sysandj : Resize UI
	//	clsResizeUI.ResizeControl(this, &m_txt_dev[i]);
	//	// by sysandj : Resize UI
	//}
	/* static - mark search result */
	

	for (i=0; i< eEXPO_TXT_MAK_MAX; i++)
	{
		m_txt_mak[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_mak[i].SetDrawBg(1);
		m_txt_mak[i].SetBaseProp(1, 1, 0, RGB(128, 128, 128), RGB(254, 254, 254), 0, RGB(0, 0, 0));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_mak[i]);
		// by sysandj : Resize UI
	}
	/* static - Subject (Top Title) / Data Value */
	for (i=0; i< eEXPO_TXT_VAL_MAX; i++)
	{
		m_txt_val[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_val[i].SetDrawBg(1);
		m_txt_val[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_val[i]);
		// by sysandj : Resize UI
	}
	/* static - Distance between marks */
	for (i=0; i< eEXPO_TXT_DST_MAX; i++)
	{
		m_txt_dst[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
		m_txt_dst[i].SetDrawBg(1);
		m_txt_dst[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_txt_dst[i]);
		// by sysandj : Resize UI
	}
	/* static - temperature alarm */
	//for (i=0; i< eEXPO_TXT_TMP_MAX; i++)
	//{
	//	m_txt_tmp[i].SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
	//	m_txt_tmp[i].SetDrawBg(1);
	//	m_txt_tmp[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));

	//	// by sysandj : Resize UI
	//	clsResizeUI.ResizeControl(this, &m_txt_tmp[i]);
	//	// by sysandj : Resize UI
	//}
	/* progress - normal */
	for (i=0; i< eEXPO_PGR_MAX; i++)
	{
		m_pgr_ctl[i].SetColor(RGB(192, 192, 192));
//		m_pgr_ctl[i].SetIndeterminate(TRUE);

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_pgr_ctl[i]);
		// by sysandj : Resize UI
	}
}

/*
 desc : ��ü �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgExpo::InitObject()
{
	// by sysandj : Resize UI
	CResizeUI clsResizeUI;
	// by sysandj : Resize UI

	UINT32 i	= 0;
	HWND hTxtMark[eEXPO_PIC_MAK_MAX]	= { NULL, };
	HWND hPicMark[eEXPO_PIC_MAK_MAX]	= { NULL, };
	HWND hPicDev[eEXPO_PIC_DEV_MAX]		= { NULL, };
	HWND hPgrCtrl[eEXPO_PGR_MAX]		= { NULL, };
	HWND hTxtDist[eEXPO_TXT_DST_MAX]	= { NULL, };
	HWND hTxtExpo[eEXPO_TXT_VAL_MAX]	= { NULL, };
	HWND hTxtTemp[eEXPO_TXT_TMP_MAX]	= { NULL, };
	
	RECT rPicMark[eEXPO_PIC_MAK_MAX];

	/* ��ũ �˻� ��� ��� ��ü */
	for (i=0; i<eEXPO_PIC_MAK_MAX; i++)
	{
		hTxtMark[i]	= m_txt_mak[i].GetSafeHwnd();
		hPicMark[i]	= m_pic_mak[i].GetSafeHwnd();

		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_pic_mak[i]);
		// by sysandj : Resize UI

		m_pic_mak[i].GetClientRect(&rPicMark[i]);
	}
	m_pDrawMark	= new CDrawMark(hPicMark, hTxtMark, rPicMark); 
	ASSERT(m_pDrawMark);
	/* ����� ��� Ȥ�� ���� ���� ��� ��ü */
	//for (i=0; i< eEXPO_PIC_DEV_MAX; i++)
	//{
	//	hPicDev[i]	= m_pic_dev[i].GetSafeHwnd();

	//	// by sysandj : Resize UI
	//	clsResizeUI.ResizeControl(this, &m_pic_dev[i]);
	//	// by sysandj : Resize UI
	//}
	//m_pDrawDev	= new CDrawDev(hPicDev);
	//ASSERT(m_pDrawDev);
	/* �˻��� ��ũ ���� �Ÿ� ������ ��ȿ���� ���� */
	for (i=0; i< eEXPO_TXT_DST_MAX; i++)
	{
		hTxtDist[i]	= m_txt_dst[i].GetSafeHwnd();
	}
	m_pMarkDist	= new CMarkDist(hTxtDist);
	ASSERT(m_pMarkDist);
	/* Temperature Alarm */
	//for (i=0; i< eEXPO_TXT_TMP_MAX; i++)
	//{
	//	hTxtTemp[i]	= m_txt_tmp[i].GetSafeHwnd();
	//}
	//m_pTempAlarm	= new CTempAlarm(hTxtTemp);
	//ASSERT(m_pTempAlarm);
	/* Exposure ȭ�鿡�� �ǽð����� ���ŵǴ� �� */
	for (i=0; i< eEXPO_TXT_VAL_MAX; i++)
	{
		hTxtExpo[i]	= m_txt_val[i].GetSafeHwnd();
	}

	for (i = 0; i < eEXPO_PGR_MAX; i++)
	{
		hPgrCtrl[i] = m_pgr_ctl[i].GetSafeHwnd();
	}

	m_pExpoVal	= new CExpoVal(hTxtExpo, hPgrCtrl);
	ASSERT(m_pExpoVal);
	/* ���� ���õ� ������ ��� */
	m_pExpoVal->DrawRecipe();

	return TRUE;
}

/*
 desc : ��ü ����
 parm : None
 retn : None
*/
VOID CDlgExpo::CloseObject()
{
	/* ��ũ �˻� ��� ��� ��ü */
	if (m_pDrawMark)	delete m_pDrawMark;
	if (m_pDrawDev)		delete m_pDrawDev;
	if (m_pMarkDist)	delete m_pMarkDist;
	if (m_pExpoVal)		delete m_pExpoVal;
	if (m_pTempAlarm)	delete m_pTempAlarm;
}

/*
 desc : �Ϲ� ��ư Ŭ���� ���
 parm : id	- [in]  �Ϲ� ��ư ID
 retn : None
*/
VOID CDlgExpo::OnBtnClick(UINT32 id)
{
	LPG_RJAF pstJob = uvEng_JobRecipe_GetSelectRecipe();
	LPG_REAF pstExpo = uvEng_ExpoRecipe_GetSelectRecipe();
	LPG_RAAF pstAlign = uvEng_Mark_GetSelectAlignRecipe();
	/* �˻� ����� ��ũ ���� ���� */
	switch (id)
	{
	case IDC_EXPO_BTN_CHECK_ALIGN	:
	case IDC_EXPO_BTN_EXPO_ALIGN	:
		if (!pstJob || !pstAlign)
		{
			CDlgMesg dlgMesg;
			dlgMesg.MyDoModal(L"The recipe (mark or gerber) does not exist", 0x01);
			return;
		}
		/* �ݵ�� 2�� �˻��ؾ� �ϹǷ�... �˻� ���� ���� */
		uvEng_Camera_SetMarkMethod(ENG_MMSM(pstAlign->search_type), pstAlign->search_count);
		break;
	}

	switch (id)
	{
	case IDC_EXPO_BTN_RESET			:	ResetData();			break;
	case IDC_EXPO_BTN_RECIPE_LOAD	:	RecipeLoad();			break;
	case IDC_EXPO_BTN_RECIPE_UNLOAD	:	VacuumOnOff();			break;
	case IDC_EXPO_BTN_EXPO_DIRECT	:	ExposeDirect();			break;
	case IDC_EXPO_BTN_MARK_PREV		:	RedrawMarkPage(0x00);	break;
	case IDC_EXPO_BTN_MARK_NEXT		:	RedrawMarkPage(0x01);	break;
	case IDC_EXPO_BTN_MARK_NO		:	NextCheckMarkNo();		break;
	case IDC_EXPO_BTN_CHECK_ALIGN	:	MoveCheckMarkNo();		break;
	case IDC_EXPO_BTN_EXPO_ALIGN	:	ExposeAlign();			break;
	}
}

/*
 desc : ��ũ ��µ� ������ ��ȣ
 parm : mode	- [in]  0x00 : ���� ������ ����
						0x01 : ���� ������ ����
 retn : None
*/
VOID CDlgExpo::RedrawMarkPage(UINT8 mode)
{
	/* ���� ������ ���� */
	if (0x00 == mode)
	{
		if (m_u8MarkPage > 0x01)	m_u8MarkPage--;
	}
	/* ���� ������ ���� */
	else
	{
		const int MAX_PAGE = 30;
		if (m_u8MarkPage < MAX_PAGE)	m_u8MarkPage++;
	}

	/* ���� �������� ��ũ 4�� ���� ��� */
#if 0
	/* ��ư Ÿ��Ʋ ���� */
	TCHAR tzTitle[32]	= {NULL};
	swprintf_s(tzTitle, 32, L"Mark Page\n[%d / 5]", m_u8MarkPage);
	m_btn_ctl[eEXPO_BTN_MARK_NO].SetWindowTextW(tzTitle);
#endif
	/* ��ũ �˻� ���� ��� */
	DrawMarkData();
}

/*
 desc : ���� ���õ� Recipe ���� ����
 parm : None
 retn : None
*/
VOID CDlgExpo::RecipeLoad()
{
	UINT8 u8Offset	= 0xff;

	if (uvEng_GetConfig()->set_comn.run_develop_mode)
	{
		CDlgStep dlgStep;
		if (IDOK != dlgStep.MyDoModal())	return;
		u8Offset	= dlgStep.GetOffset();
	}
	m_pDlgMain->RunWorkJob(ENG_BWOK::en_gerb_load, PUINT64(&u8Offset));
}

/*
 desc : Stage Vacuum On/off
 parm : None
 retn : None
*/
VOID CDlgExpo::VacuumOnOff()
{
	//m_pDlgMain->RunWorkJob(ENG_BWOK::en_gerb_unload);
	///* ������ ���� �ȵƴٰ� Ÿ��Ʋ �缳�� */
	//m_pExpoVal->DrawRecipe();
	TCHAR tzMesg[256] = { NULL }, tzYesNo[2][8] = { L"OFF", L"ON" };

	ST_IO stIO;
	CIOManager::GetInstance()->UpdateIO();

#if (DELIVERY_PRODUCT_ID == CUSTOM_CODE_UVDI15 || DELIVERY_PRODUCT_ID == CUSTOM_CODE_HDDI6)
	CIOManager::GetInstance()->GetIO(eIO_OUTPUT, _T("VACUUM PUMP UNIT STAGE VACUUM ON"), stIO);
#endif
	stIO.bOn = !stIO.bOn;

	CDlgMesg dlgMesg;
	swprintf_s(tzMesg, 256, L"Vacuum %s?", tzYesNo[stIO.bOn]);
	if (IDOK == dlgMesg.MyDoModal(tzMesg, 0x02))
	{
		CIOManager::GetInstance()->SetIO(eIO_OUTPUT, stIO);
	}
}

/*
 ���� : Transformation Information �ɼ� ���� Ȯ��
 ���� : None
 ��ȯ : TRUE or FALSE
*/
BOOL CDlgExpo::IsCheckTransRecipe()
{
	TCHAR tzMesg[256]	= {NULL}, tzYesNo[2][8]	= { L"No", L"Yes" };
	LPG_CGTI pstTrans	= NULL;
	CDlgMesg dlgMesg;

	/* Check the Global Transformation Information */
	pstTrans	= &uvEng_GetConfig()->global_trans;
	swprintf_s(tzMesg, 256, L"Apply global transformation recipe values ?\n"
							L"Rect.Lock(%s), Rotation(%s), Scaling(%s), Offset(%s)",
			   tzYesNo[pstTrans->use_rectangle], tzYesNo[pstTrans->use_rotation_mode],
			   tzYesNo[pstTrans->use_scaling_mode], tzYesNo[pstTrans->use_offset_mode]);
	if (dlgMesg.MyDoModal(tzMesg, 0x00) != IDOK)	return FALSE;

	return TRUE;
}

/*
 desc : ���� �뱤 ���� (Align Mark �˻� ����)
 parm : None
 retn : None
*/
VOID CDlgExpo::ExposeDirect()
{
	BOOL bPreHeated	= FALSE;
	//STG_CPHE stExpo	= {NULL};
	STG_CELA stExpo = { NULL };
	stExpo.Init();
	CDlgMesg dlgMesg;

	if (IDOK != dlgMesg.MyDoModal(L"Is Direct Exposure Working right ?"))	return;
#if 1
	if (uvEng_GetConfig()->set_comn.run_develop_mode)
#endif
	{
		CDlgRept dlgRept;
		if (IDOK != dlgRept.MyDoModal())	return;
		stExpo.expo_count	= dlgRept.GetExpoCount();
		stExpo.move_acam	= dlgRept.IsMovingACam() ? 0x01 : 0x00;
		stExpo.move_ph		= dlgRept.IsMovingPH() ? 0x01 : 0x00;
	}

	/* Transformation Recipe Information ���� Ȯ�� */
	if (uvEng_GetConfig()->global_trans.use_trans && !IsCheckTransRecipe())	return;
	/* Vacuum�� Shutter�� ���� ���� ���� */
	// by sysandj : MCQ��ü �߰� �ʿ�

	m_pDlgMain->RunWorkJob(ENG_BWOK::en_expo_only, PUINT64(&stExpo));
}

/*
 desc : ����� �뱤 ���� (Align Mark �˻� ����)
 parm : None
 retn : None
*/
VOID CDlgExpo::ExposeAlign()
{
	CDlgMesg dlgMesg;
	STG_CELA stExpo = { NULL };
	stExpo.Init();

	if (IDOK != dlgMesg.MyDoModal(L"Is Align Exposure Working right ?"))	return;

	if (!uvEng_Luria_IsMarkGlobal())
	{
		dlgMesg.MyDoModal(L"There is no globalmark information", 0x01);
		return;
	}
	/* Transformation Recipe Information ���� Ȯ�� */
	if (uvEng_GetConfig()->global_trans.use_trans && !IsCheckTransRecipe())	return;

	//m_pDlgMain->RunWorkJob(ENG_BWOK::en_expo_align, PUINT64(&stExpo));
	m_pDlgMain->RunWorkJob(ENG_BWOK::en_expo_align);
}

/*
 desc : ���� ���õ� Mark Number ��ġ�� Camera X �� �� Stage X / Y �� �̵��ϱ�
 parm : None
 retn : None
*/
VOID CDlgExpo::MoveCheckMarkNo()
{
	CDlgMesg dlgMesg;
	STG_CELA stExpo = { NULL };
	stExpo.Init();
	/* Vacuum�� Shutter�� ���� ���� ���� */
	// by sysandj : MCQ��ü �߰� �ʿ�

	
	UINT64 flag = (includeAlignOffset << 4) | m_u8MarkNo;

	if (m_u8MarkNo)	m_pDlgMain->RunWorkJob(ENG_BWOK::en_mark_move, &flag);
	//else			m_pDlgMain->RunWorkJob(ENG_BWOK::en_mark_test);
	else
	{
		if (IDOK != dlgMesg.MyDoModal(L"Is Test Align Mark Working right ?"))	return;
#if 1
		if (uvEng_GetConfig()->set_comn.run_develop_mode)
#endif
		{
			CDlgRept dlgRept;
			if (IDOK != dlgRept.MyDoModal())	return;
			stExpo.expo_count = dlgRept.GetExpoCount();
			stExpo.move_acam = dlgRept.IsMovingACam() ? 0x01 : 0x00;
			stExpo.move_ph = dlgRept.IsMovingPH() ? 0x01 : 0x00;
			
	}
		stExpo.includeAddAlignOffset = includeAlignOffset;
		m_pDlgMain->RunWorkJob(ENG_BWOK::en_mark_test, PUINT64(&stExpo));
	}

}

/*
 desc : Mark No ��ư�� ���� ������, �˻� ����� Mark Number ���� ��Ű��
 parm : None
 retn : None
 note : �켱, Global Mark�� �˻��ϱ�� �� (T.T)
*/
VOID CDlgExpo::NextCheckMarkNo()
{
	TCHAR tzMark[32]	= {NULL}, tzAlign[32] = { L"MARK\nMOVE" };

	if (++m_u8MarkNo == (uvEng_Luria_GetMarkCount(ENG_AMTF::en_global))+1)
	{
		m_u8MarkNo	= 0x00;
	}
	/* ��ư�� �� ��� */
	if (m_u8MarkNo)
	{
		swprintf_s(tzMark, 32, L"MARK\n[ %u ]", m_u8MarkNo);
	}
	else
	{
		wcscpy_s(tzAlign, 32, L"ALIGN\nTEST");
		wcscpy_s(tzMark, 32, L"MARK\n[ ALL ]");
	}
	m_btn_ctl[eEXPO_BTN_CHECK_ALIGN].SetWindowTextW(tzAlign);
	m_btn_ctl[eEXPO_BTN_CHECK_ALIGN].Invalidate(TRUE);
	m_btn_ctl[eEXPO_BTN_MARK_NO].SetWindowTextW(tzMark);
	m_btn_ctl[eEXPO_BTN_MARK_NO].Invalidate(TRUE);
}

/*
 desc : �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgExpo::ResetData()
{
	/* ���� ���� �ʱ�ȭ */
	uvEng_ShMem_GetLuria()->link.ResetData();
	uvEng_ResetJobWorkInfo();
	/* Expose ���� ��ġ �ʱ�ȭ */
	m_pExpoVal->ResetData();
#if 1
	/* Luria HW Inited (���� ����ȭ (����) ���������� �Ʒ� �ڵ� ����) */
	uvEng_Luria_ReqSetHWInit();
#endif

	Invalidate(TRUE);
}

/*
 desc : ��ũ �˻� ����� �̹����� ���� ��� (����) �� ��� (����)
 parm : None
 retn : None
*/
VOID CDlgExpo::DrawMarkData()
{
#if 0
	UINT8 i	= 0x00;
#if 1
	RECT rMarkArea	= {NULL};
	/* ����� ��ũ ���� ���� */
	m_grp_ctl[eEXPO_BTN_EXPO_ALIGN].GetWindowRect(&rMarkArea);
	ScreenToClient(&rMarkArea);
	InvalidateRect(&rMarkArea, TRUE);
#else
	/* ����� ��ũ ��� ������ ���� ���� */
	m_pDrawMark->DrawPage(m_u8MarkPage);
#endif
	/* Update to Group Control */
	for (i=0x00; i<eEXPO_GRP_MAX; i++)
	{
#if 0
		if (i != eEXPO_GRP_ALIGN_MARK)	m_grp_ctl[i].InvalidateArea();
#else
		if (i != eEXPO_GRP_ALIGN_MARK)	m_grp_ctl[i].Invalidate(TRUE);
#endif
	}
#else
	Invalidate(TRUE);
#endif
}

/* desc: Frame Control�� MIL DISP�� ���� */
VOID CDlgExpo::InitDispMark()
{
	CWnd* pWnd[4];
	for (int i = 0; i < eEXPO_PIC_MAK_MAX; i++)
	{
		pWnd[i] = GetDlgItem(IDC_EXPO_PIC_MARK_1 + i);
		//DispResize(GetDlgItem(IDC_MMPM_PIC_VIEW));
	}
	uvEng_Camera_SetDispExpo(pWnd);
	DispResize(pWnd);
}

void CDlgExpo::DispResize(CWnd* pWnd[4])
{
	double tgt_rate;
	for (int i = 0; i < eEXPO_PIC_MAK_MAX; i++)
	{
		CRect lpRect;
		pWnd[i]->GetWindowRect(lpRect);
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

		pWnd[i]->MoveWindow(lpRect, TRUE);

		//uvEng_Camera_SetMarkLiveDispSize(tgtSize);

	}
}


void CDlgExpo::OnBnClickedAddalignoffset()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	enum
	{
		noOffset,
		addAlign,
		addAlign_Expo,
	};

	includeAlignOffset = includeAlignOffset == addAlign_Expo ? noOffset : includeAlignOffset + 1;
	auto txtMap = map<int, WCHAR*>{{noOffset, L"[No Offset]"}, { addAlign,L"[add Align Offset]" }, { addAlign_Expo,L"[add Align,Expo Offsets]" } };
	includeAlignOffsetBtn.SetWindowTextW(txtMap[includeAlignOffset]);
		
}


void CDlgExpo::OnStnDblclickExpoPicMarks(UINT32 id)
{
	switch (id)
	{
		case IDC_EXPO_PIC_MARK_1:
		case IDC_EXPO_PIC_MARK_2:
		case IDC_EXPO_PIC_MARK_3:
		case IDC_EXPO_PIC_MARK_4:
		{
			AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();
			int CENTER_CAM = motions.markParams.centerCamIdx;
			auto result =  motions.IsNeedManualFixOffset(((UINT16)motions.markParams.alignMotion & 0b10) == 0b10 ? &CENTER_CAM : nullptr);
			auto* config = uvEng_GetConfig();

			switch (result)
			{
				case ENG_MFOR::canFix:
				{
					CDlgMmpm dlgMmpm(false,nullptr);
					if((IDOK == dlgMmpm.DoModal()) == false) // cancle������
						AfxMessageBox(L"[error]user cancled.", MB_ICONSTOP | MB_TOPMOST);

					Invalidate(1);
				}
				break;

				case ENG_MFOR::noRecipeLoaded:
				{
					AfxMessageBox(L"[error]no recipe Loaded.", MB_ICONSTOP | MB_TOPMOST);
				}
				break;

				case ENG_MFOR::grabcountMiss:
				{
					AfxMessageBox(L"[error]The offset cannot be set manually. All marks are ungrabbed.", MB_ICONSTOP | MB_TOPMOST);
				}
				break;

				case ENG_MFOR::noNeedToFix:
				{
					AfxMessageBox(L"all marks are properly grabbed. no need to set manually", MB_ICONSTOP | MB_TOPMOST);
				}
				break;
			}
		}
		break;
			
	}


	int debug=0;
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}
