﻿// DlgCalb.cpp: 구현 파일
//

#include "pch.h"

#include "DlgCalbUVPower.h"

#include "./uvpower/DlgCalbUVPowerMeasure.h"
#include "./uvpower/DlgCalbUVPowerCheck.h"
#include "afxdialogex.h"
#include "../../../../inc/conf/luria.h"
#include "../../../../inc/conf/conf_uvdi15.h"

#include <string>
#include <map>


/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgCalbUVPower::CDlgCalbUVPower(UINT32 id, CWnd* parent)
	: CDlgSubMenu(id, parent)
{
	m_enDlgID		= ENG_CMDI_SUB::en_menu_sub_calb_uvpower;
	m_enTabDlgID	= ENG_CMDI_TAB::en_menu_tab_none;
	m_pDlgMenu		= NULL;


}

CDlgCalbUVPower::~CDlgCalbUVPower()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgCalbUVPower::DoDataExchange(CDataExchange* dx)
{
	CDlgSubMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* button - normal */
	u32StartID = IDC_CALB_UVPOWER_BTN_SUBMENU_POWER_MEASURE;
	for (i = 0; i < eCALB_UVPOWER_BTN_MAX; i++)		DDX_Control(dx, u32StartID + i, m_btn_ctl[i]);

	

	/* static - picture */
	u32StartID = IDC_CALB_UVPOWER_PIC_MENU;
	for (i = 0; i < eCALB_UVPOWER_PIC_MAX; i++)		DDX_Control(dx, u32StartID + i, m_pic_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgCalbUVPower, CDlgSubMenu)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CALB_UVPOWER_BTN_SUBMENU_POWER_MEASURE, IDC_CALB_UVPOWER_BTN_SUBMENU_POWER_CHECK, OnBtnClick)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CALB_UVPOWER_BTN_SUBMENU_POWER_SHOW, &CDlgCalbUVPower::OnBnClickedCalbUvpowerBtnSubmenuPowerShow)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgCalbUVPower::PreTranslateMessage(MSG* msg)
{
	return CDlgSubMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgCalbUVPower::OnInitDlg()
{
	/* 컨트롤 초기화 */
	InitCtrl();
	if (!CreateMenu(IDC_CALB_UVPOWER_BTN_SUBMENU_POWER_MEASURE))	return FALSE;

	SetTimer(eCALB_UVPOWER_TIMER_START, 200, NULL);



	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgCalbUVPower::OnExitDlg()
{
	KillTimer(eCALB_UVPOWER_TIMER_START);
	DeleteMenu();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgCalbUVPower::OnPaintDlg(CDC* dc)
{

	/* 자식 윈도 호출 */
	CDlgSubMenu::DrawOutLine();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgCalbUVPower::OnResizeDlg()
{
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgCalbUVPower::UpdatePeriod(UINT64 tick, BOOL is_busy)
{

	/* Control Enable or Disable */
	UpdateControl(tick, is_busy);
}

/*
 desc : 컨트롤
 parm : tick	- [in]  현재 CPU 시간
		is_busy	- [in]  TRUE: 현재 시나리오 동작 중 ..., FALSE: 현재 Idle 상태
 retn : None
*/
VOID CDlgCalbUVPower::UpdateControl(UINT64 tick, BOOL is_busy)
{
	UINT8 i = 0x00;
	/* 현재 상태 정보 메뉴 다이얼로그 ID가 아니면, 상태 버튼에는 알람 여부에 따른 색상 표현 */
	if (m_enTabDlgID != m_pDlgMenu->GetDlgID())
	{
		for (i = 0; i < eCALB_UVPOWER_BTN_MAX; i++)
		{
			m_btn_ctl[i].SetBgColor(DEF_COLOR_BTN_MENU_NORMAL);
			m_btn_ctl[i].SetTextColor(DEF_COLOR_BTN_MENU_NORMAL_TEXT);
			m_btn_ctl[i].Invalidate(TRUE);
		}

		/* 현재 선택된 메뉴 (자식) 다이얼로그 ID 저장 */
		m_enTabDlgID = m_pDlgMenu->GetDlgID();
		/* 현재 선택된 버튼에 따라 배경색 다르게 출력 */
		i = 0xff;
		switch (m_enTabDlgID)
		{
		case ENG_CMDI_TAB::en_menu_tab_calb_uvpower_measure:		i = eCALB_UVPOWER_BTN_MEASURE;		break;
		case ENG_CMDI_TAB::en_menu_tab_calb_uvpower_check:			i = eCALB_UVPOWER_BTN_CHECK;		break;
		}
		if (i != 0xff)
		{
			//m_btn_ctl[i].SetBgColor(RGB(0x80, 0xff, 0x00));
			m_btn_ctl[i].SetBgColor(DEF_COLOR_BTN_MENU_SELECT);
			m_btn_ctl[i].Invalidate(TRUE);
		}
	}
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgCalbUVPower::InitCtrl()
{
	CResizeUI clsResizeUI;

	/* button - normal */
	for (int i = 0; i < eCALB_UVPOWER_BTN_MAX; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf[eFONT_LEVEL2_BOLD]);
		m_btn_ctl[i].SetBgColor(g_clrBtnColor);
		m_btn_ctl[i].SetTextColor(g_clrBtnTextColor);
		m_btn_ctl[i].SetMultiLines(2);
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_btn_ctl[i]);
		// by sysandj : Resize UI
	}

	for (int i = 0; i < eCALB_UVPOWER_PIC_MAX; i++)
	{
		// by sysandj : Resize UI
		clsResizeUI.ResizeControl(this, &m_pic_ctl[i]);
		// by sysandj : Resize UI
	}
}

/*
 desc : 객체 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgCalbUVPower::InitObject()
{
	
	
	return TRUE;
}

/*
 desc : 객체 해제
 parm : None
 retn : None
*/
VOID CDlgCalbUVPower::CloseObject()
{
}

/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgCalbUVPower::OnBtnClick(UINT32 id)
{
	CreateMenu(id);
}

/*
 desc : 현재 실행된 자식 윈도 메모리 해제
 parm : None
 retn : None
*/
VOID CDlgCalbUVPower::DeleteMenu()
{
	if (!m_pDlgMenu)	return;

	if (m_pDlgMenu->GetSafeHwnd())	m_pDlgMenu->DestroyWindow();
	delete m_pDlgMenu;
	m_pDlgMenu = NULL;
}

/*
 desc : 자식 메뉴 화면 생성
 parm : id	- [in]  버튼 컨트롤 ID
 retn : TRUE or FALSE
*/
BOOL CDlgCalbUVPower::CreateMenu(UINT32 id)
{
	UINT32 u32DlgID = 0;

	/* 기존 실행된 자식 윈도가 있다면 메모리 해제 */
	DeleteMenu();

	/* 생성하고자 하는 윈도 (메뉴 다이얼로그) ID 설정 */
	u32DlgID = IDD_CALB_UVPOWER_MEASURE + (id - IDC_CALB_UVPOWER_BTN_SUBMENU_POWER_MEASURE);
	/* 해당 자식 (메뉴) 윈도 메모리 할당 */
	switch (id)
	{
	case IDC_CALB_UVPOWER_BTN_SUBMENU_POWER_MEASURE: m_pDlgMenu = new CDlgCalbUVPowerMeasure(u32DlgID, this);	break;
	case IDC_CALB_UVPOWER_BTN_SUBMENU_POWER_CHECK:	m_pDlgMenu = new CDlgCalbUVPowerCheck(u32DlgID, this);	break;
	}

	/* 자식 (메뉴) 윈도 생성 */
	if (!m_pDlgMenu->Create(m_pic_ctl[eCALB_UVPOWER_PIC_MENU].GetSafeHwnd()))
	{
		delete m_pDlgMenu;
		m_pDlgMenu = NULL;
		return FALSE;
	}

	return TRUE;
}


void CDlgCalbUVPower::OnTimer(UINT_PTR nIDEvent)
{
	if (eCALB_UVPOWER_TIMER_START == nIDEvent)
	{
		UpdateControl(GetTickCount64(), FALSE);
	}

	CDlgSubMenu::OnTimer(nIDEvent);
}



void CDlgCalbUVPower::OnBnClickedCalbUvpowerBtnSubmenuPowerShow()
{
	auto uiWorks = [&]()
	{
		CDlgParam dlg;
		DLG_PARAM stParam;
		VCT_DLG_PARAM stVctParam;
		LPG_CIEA cfg = uvEng_GetConfig();
		LPG_LDSM shmem = uvCmn_Luria_GetShMem();

		for (int ph = 0; ph < cfg->luria_svc.ph_count; ph++)
			for (int led = 0; led < cfg->luria_svc.led_count; led++)
				stepMap[ph][led] = shmem->directph.light_source_driver_amplitude[ph][led];

		for (int led = 0; led < cfg->luria_svc.led_count; led++)
		{
			int sum = 0;
			for (int ph = 0; ph < cfg->luria_svc.ph_count; ph++)
			{
				sum += stepMap[ph][led];
			}
			if(sum == 0 && GetTickCount() > ampvalueCallTick + 3000)
				uvEng_Luria_ReqGetLedPowerLedAll((ENG_LLPI)((int)ENG_LLPI::en_365nm + led));
		}

		ampvalueCallTick = GetTickCount();

		if (shmem == nullptr) return;
		for (int ph = 0; ph < cfg->luria_svc.ph_count; ph++)
		{
			CString temp;
			temp.Format(_T("ph%d , led 1-4"), ph + 1);
			stParam.Init();
			stParam.strName = temp;
			temp.Format(_T("%d, %d, %d, %d"), stepMap[ph][0], stepMap[ph][1], stepMap[ph][2], stepMap[ph][3]);
			stParam.strValue = temp;
			stParam.strUnit = _T("step");
			stParam.enFormat = ENM_DITM::en_none;
			stVctParam.push_back(stParam);
		}

		dlg.MyDoModal(stVctParam);
	};

	uiWorks();
}
