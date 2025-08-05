// DlgCalbFlatness.cpp: 구현 파일
//
// DlgCalb.cpp: 구현 파일
//

#include "pch.h"
#include "DlgCalbAutofocus.h"
#include "afxdialogex.h"

#define DEF_DEFAULT_GRID_ROW_SIZE 35

/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgCalbAutofocus::CDlgCalbAutofocus(UINT32 id, CWnd* parent)
	: CDlgSubMenu(id, parent)
{
	m_enDlgID = ENG_CMDI_SUB::en_menu_sub_conf_autofocus;

}

CDlgCalbAutofocus::~CDlgCalbAutofocus()
{
	 
}

VOID CDlgCalbAutofocus::UpdateControl(UINT64 tick, BOOL is_busy)
{

}

VOID CDlgCalbAutofocus::OnResizeDlg()
{

}

VOID CDlgCalbAutofocus::UpdatePeriod(UINT64 tick, BOOL is_busy)
{

}


VOID CDlgCalbAutofocus::InitGridOption()
{
	
}


/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgCalbAutofocus::DoDataExchange(CDataExchange* dx)
{
	CDlgSubMenu::DoDataExchange(dx);
}

BEGIN_MESSAGE_MAP(CDlgCalbAutofocus, CDlgSubMenu)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CALB_FLATNESS_BTN_SAVE_DATA, IDC_CALB_FLATNESS_BTN_OPEN_CTRL_PANEL, OnBtnClick)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgCalbAutofocus::PreTranslateMessage(MSG* msg)
{
	if (msg->message == WM_KEYDOWN)
	{
		if (msg->wParam == VK_RETURN || msg->wParam == VK_ESCAPE)
		{
			return TRUE;                // Do not process further
		}
	}

	return CDlgSubMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgCalbAutofocus::OnInitDlg()
{
	InitCtrl();
	InitGridOption();
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgCalbAutofocus::OnExitDlg()
{
	
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgCalbAutofocus::OnPaintDlg(CDC* dc)
{

}
   
/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgCalbAutofocus::InitCtrl()
{
	 
}
   

VOID CDlgCalbAutofocus::LoadDataConfig()
{
	
}

VOID CDlgCalbAutofocus::SaveDataConfig()
{
	 
	uvEng_SaveConfig();
}

 
  

/*
 desc : 일반 버튼 클릭한 경우
 parm : id	- [in]  일반 버튼 ID
 retn : None
*/
VOID CDlgCalbAutofocus::OnBtnClick(UINT32 id)
{
 

	switch (id)
	{
	
	default:
		break;
	}
}
             
