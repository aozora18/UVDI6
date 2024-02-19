
/*
 desc : PLC Value Testing (Register D Address의 값 테스트)
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgPSet.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgPSet::CDlgPSet(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgPSet::IDD, parent)
{
	m_bDrawBG		= TRUE;
	m_bTooltip		= TRUE;
	m_u16SetAddr	= 0x0000;
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgPSet::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Groupbox */
	u32StartID	= IDC_PSET_GRP_SETVAL;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Combobox */
	u32StartID	= IDC_PSET_CMB_DADDR;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_cmb_ctl[i]);
	/* Static - Normal */
	u32StartID	= IDC_PSET_TXT_70;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Button - Normal */
	u32StartID	= IDC_PSET_BTN_SAVE;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Check - Normal */
	u32StartID	= IDC_PSET_CHK_16BIT;
	for (i=0; i<20; i++)	DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
	/* Edit - number */
	u32StartID	= IDC_PSET_EDT_VALUE;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgPSet, CMyDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_PSET_BTN_SAVE, IDC_PSET_BTN_SAVE,	OnBtnClicked)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_PSET_CHK_16BIT, IDC_PSET_CHK_BF,	OnChkClicked)
	ON_CBN_SELCHANGE(IDC_PSET_CMB_DADDR, OnCmbSelChangeDAddr)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgPSet::PreTranslateMessage(MSG* msg)
{
	 if(msg->message == WM_KEYUP)
	 {
		 if (msg->hwnd == m_edt_ctl[0].m_hWnd)
		 {
			 /* 만약 현재 16 bit의 경우, */
			 if (m_chk_ctl[0].GetCheck())
			 {
				 /* 최대 값을 넘지 않도록 유도 */
				 if (m_edt_ctl[0].GetTextToNum() > UINT16_MAX)
				 {
					 m_edt_ctl[0].SetTextToNum(UINT16(UINT16_MAX));
				 }
			 }
			 else
			 {
				 if (m_edt_ctl[0].GetTextToNum() > UINT32_MAX)
				 {
					 m_edt_ctl[0].SetTextToNum(UINT16(UINT32_MAX));
				 }
			 }
			 ChangeEdtValue();
		 }
	 }

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgPSet::OnInitDlg()
{
	/* 컨트롤 초기화 */
	if (!InitCtrl())	return FALSE;

	/* TOP_MOST & Center */
	//CenterParentTopMost();

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgPSet::OnExitDlg()
{
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgPSet::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgPSet::OnResizeDlg()
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgPSet::InitCtrl()
{
	INT32 i;

	/* Groupbox */
	for (i=0; i<1; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* Combobox */
	for (i=0; i<1; i++)
	{
		m_cmb_ctl[i].SetLogFont(&g_lf);
	}
	/* text - normal */
	for (i=0; i<2; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* button - normal */
	for (i=0; i<1; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	/* checkbox - normal */
	for (i=0; i<20; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf);
		m_chk_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	/* edit - angle / size */
	for (i=0; i<1; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_ctl[i].SetInputType(EN_DITM::en_hex10);
	}

	/* 체크 박스 초기화 */
	m_chk_ctl[0].SetCheck(1);
	m_chk_ctl[1].SetCheck(0);
	m_chk_ctl[2].SetCheck(1);
	m_chk_ctl[3].SetCheck(0);
	/* 주소 적재 */
	LoadAddr();
	/* 카메라 사양 출력 */
	UpdateValue();
	/* 체크 박스 바꿈 */
	ChangeEdtValue();

	return TRUE;
}

/*
 desc : 버튼 이벤트
 parm : id	- [in]  버튼 ID
 retn : None
*/
VOID CDlgPSet::OnBtnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_PSET_BTN_SAVE		:	SetValue();		break;
	}
}

/*
 desc : 버튼 이벤트
 parm : id	- [in]  버튼 ID
 retn : None
*/
VOID CDlgPSet::OnChkClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_PSET_CHK_16BIT	:	m_chk_ctl[1].SetCheck(0);	break;
	case IDC_PSET_CHK_32BIT	:	m_chk_ctl[0].SetCheck(0);	break;
	case IDC_PSET_CHK_DEC	:
	case IDC_PSET_CHK_HEX	:
		switch (id)
		{
		case IDC_PSET_CHK_DEC	:	m_chk_ctl[3].SetCheck(0);
									m_edt_ctl[0].SetInputType(EN_DITM::en_hex10);	break;
		case IDC_PSET_CHK_HEX	:	m_chk_ctl[2].SetCheck(0);
									m_edt_ctl[0].SetInputType(EN_DITM::en_hex16);	break;
		}
		ChangeHexValue();	break;
	default					:	ChangeChkValue();	break;
	}
}

/*
 desc : DoModal Override Function
 parm : addr	- [in]  PLC Memory Address (D Register. Word Address Index. Zero-based)
 retn : 0 or Negative
*/
INT_PTR CDlgPSet::MyDoModal(UINT16 addr)
{
	m_u16SetAddr	= addr;
	return DoModal();
}

/*
 desc : 주소 적재
 parm : None
 retn : None
*/
VOID CDlgPSet::LoadAddr()
{
	TCHAR tzAddr[8]	= {NULL};
	UINT16 u16Start	= 0;

	m_cmb_ctl[0].SetRedraw(FALSE);
	m_cmb_ctl[0].ResetContent();

	for (u16Start=0; u16Start<uvEng_Conf_GetPLC()->addr_d_count; u16Start++)
	{
		swprintf_s(tzAddr, 8, L"%05d", u16Start+uvEng_Conf_GetPLC()->start_d_addr);
		m_cmb_ctl[0].AddString(tzAddr);
	}

	m_cmb_ctl[0].SetRedraw(TRUE);
	m_cmb_ctl[0].Invalidate(FALSE);
}

/*
 desc : Deciaml or Hex 변경 모드일 경우, Edit Control의 값 바꿈
 parm : None
 retn : None
*/
VOID CDlgPSet::ChangeHexValue()
{
	UINT32 u32Val	= 0;
	TCHAR tzVal[32]	= {NULL};

	/* Decimal or Hex 확인 */
	if (m_chk_ctl[2].GetCheck())
	{
		u32Val	= (UINT32)m_edt_ctl[0].GetTextToHex();
		m_edt_ctl[0].SetTextToNum(u32Val);
	}
	else
	{
		u32Val	= (UINT32)m_edt_ctl[0].GetTextToNum();
		swprintf_s(tzVal, 32, L"%X", u32Val);
		m_edt_ctl[0].SetWindowText(tzVal);
	}
}

/*
 desc : 체크 박스 이벤트가 발생된 경우, Edit Control의 값 바꿈
 parm : None
 retn : None
*/
VOID CDlgPSet::ChangeChkValue()
{
	UINT32 i, u32Val	= 0;
	TCHAR tzVal[16]	= {NULL};

	for (i=16; i>0; i--)
	{
		u32Val	|= (UINT8)m_chk_ctl[4+i-1].GetCheck();
		if (i != 1)	u32Val	= u32Val << 1;
	}

	/* Decimal or Hex 확인 */
	if (m_chk_ctl[2].GetCheck())	swprintf_s(tzVal, 16, L"%u", u32Val);	/* Decimal */
	else							swprintf_s(tzVal, 16, L"%X", u32Val);	/* Hex */

	/* 에디트 박스에 출력 */
	m_edt_ctl[0].SetWindowText(tzVal);
}

/*
 desc : 에디트 박스 이벤트가 발생된 경우, Checkbox의 상태 바꿈
 parm : None
 retn : None
*/
VOID CDlgPSet::ChangeEdtValue()
{
	UINT32 i, u32Val	= 0;

	/* Decimal or Hex 확인 */
	if (m_chk_ctl[2].GetCheck())	u32Val	= (UINT32)m_edt_ctl[0].GetTextToNum();	/* Decimal */
	else							u32Val	= (UINT32)m_edt_ctl[0].GetTextToHex();	/* Hex */

	/* 32 bit 입력 모드이면서, UINT16_MAX 값을 넘은 경우는 리턴 */
	if (m_chk_ctl[1].GetCheck() && u32Val > UINT16_MAX)	return;
	/* 체크 버튼 속성 설정 */
	for (i=0; i<16; i++)
	{
		m_chk_ctl[4+i].SetCheck(u32Val & 0x00000001);
		u32Val	= u32Val >> 1;
	}
}

/*
 desc : 현재 PLC Memory (D Register) 주소의 값 갱신
 parm : None
 retn : None
*/
VOID CDlgPSet::UpdateValue()
{
	TCHAR tzVal[32]	= {NULL};
	UINT16 u16Index;
	UINT32 u32Val;
	PUINT16 pPLCMem	= (PUINT16)uvEng_ShMem_GetPLC()->data;

	/* 갱신 대상의 주소 인덱스 값 */
	u16Index	= uvEng_Conf_GetPLC()->GetAddrToIndex(m_u16SetAddr);
	if (0xffff == u16Index)	return;

	/* Comboxbox 주소 값 갱신 */
	m_cmb_ctl[0].SetCurSel(u16Index);

	/* 만약 마지막 주소인 경우, 무조건 16 bit 값 출력 */
	if (m_u16SetAddr == uvEng_Conf_GetPLC()->GetLastDAddr())
	{
		m_chk_ctl[0].SetCheck(1);
		m_chk_ctl[1].SetCheck(0);
		/* 값 반환 */
		if (m_chk_ctl[2].GetCheck())	m_edt_ctl[0].SetTextToNum(pPLCMem[u16Index]);
		else
		{
			swprintf_s(tzVal, 32, L"%X", pPLCMem[u16Index]);
			m_edt_ctl[0].SetWindowText(tzVal);
		}
	}
	else
	{
		/* 메모리 인덱스 위치에 해당되는 값 읽어오기 */
		if (m_chk_ctl[0].GetCheck())
		{
			if (m_chk_ctl[2].GetCheck())	m_edt_ctl[0].SetTextToNum(pPLCMem[u16Index]);
			else
			{
				swprintf_s(tzVal, 32, L"%X", pPLCMem[u16Index]);
				m_edt_ctl[0].SetWindowText(tzVal);
			}
		}
		else
		{
			memcpy(&u32Val, &pPLCMem[u16Index], 4);
			if (m_chk_ctl[2].GetCheck())	m_edt_ctl[0].SetTextToNum(u32Val);
			else
			{
				swprintf_s(tzVal, 32, L"%X", u32Val);
				m_edt_ctl[0].SetWindowText(tzVal);
			}
		}
	}
}

/*
 desc : Combobox에서 주소 위치를 변경 했을 경우, 주소 값에 해당되는 메모리 값 출력
 parm : None
 retn : None
*/
VOID CDlgPSet::OnCmbSelChangeDAddr()
{
	TCHAR tzAddr[8]	= {NULL};
	INT32 i32Sel	= 0;
	
	/* 현재 선택된 위치 얻기 */
	i32Sel	= m_cmb_ctl[0].GetCurSel();
	if (i32Sel < 0)	return;

	/* 문자열 -> 주소 (숫자) 변환 */
	m_cmb_ctl[0].GetLBText(i32Sel, tzAddr);

	/* 현재 주소 인덱 값 얻기 */
	m_u16SetAddr	= uvEng_Conf_GetPLC()->GetAddrToIndex((UINT16)_wtoi(tzAddr));

	/* 값 갱신 */
	UpdateValue();
	ChangeEdtValue();
}

/*
 desc : PLC에 값 설정
 parm : None
 retn : None
*/
VOID CDlgPSet::SetValue()
{
	UINT32 u32Addr	= (UINT32)m_cmb_ctl[0].GetLBTextToNum();

	/* for 16 bit */
	if (m_chk_ctl[0].GetCheck())
	{
		UINT16 u16Value	= 0;
		/* for decimal */
		if (m_chk_ctl[2].GetCheck())	u16Value	= (UINT16)m_edt_ctl[0].GetTextToNum();
		/* for hexa */
		else							u16Value	= (UINT16)m_edt_ctl[0].GetTextToHex();
		uvCmn_MCQ_SetDataPLC(1, u32Addr, &u16Value);
	}
	/* for 32 bit */
	else
	{
		UINT16 u16Value[2]	= {NULL};
		UINT32 u32Value	= 0;
		/* for decimal */
		if (m_chk_ctl[2].GetCheck())	u32Value	= (UINT32)m_edt_ctl[0].GetTextToNum();
		/* for hexa */
		else							u32Value	= (UINT32)m_edt_ctl[0].GetTextToHex();
		memcpy(u16Value, &u32Value, sizeof(UINT32));
		uvCmn_MCQ_SetDataPLC(2, u32Addr, u16Value);
	}
}