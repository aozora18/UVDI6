
/*
 desc : Monitoring - Luria Service - Panel Data
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgLuPD.h"


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
CDlgLuPD::CDlgLuPD(UINT32 id, CWnd *parent)
	: CDlgLuria(id, parent)
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgLuPD::~CDlgLuPD()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgLuPD::DoDataExchange(CDataExchange* dx)
{
	CDlgLuria::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Groupbox */
	u32StartID	= IDC_LUPD_GRP_DCODE;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Button */
	u32StartID	= IDC_LUPD_BTN_REFRESH;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Static */
	u32StartID	= IDC_LUPD_TXT_TYPE;
	for (i=0; i<6; i++)		DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Edit box */
	u32StartID	= IDC_LUPD_EDT_TYPE;
	for (i=0; i<6; i++)		DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
	/* List box */
	u32StartID	= IDC_LUPD_BOX_DCODE;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_box_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgLuPD, CDlgLuria)
	ON_LBN_SELCHANGE(IDC_LUPD_BOX_DCODE, OnBoxSelchangeDcode)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgLuPD::PreTranslateMessage(MSG* msg)
{

	return CDlgLuria::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLuPD::OnInitDlg()
{
	if (!CDlgLuria::OnInitDlg())	return FALSE;

	/* 컨트롤 초기화 */
	InitCtrl();
	LoadValue();

	/* 상위 객체 호출 */
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgLuPD::OnExitDlg()
{
	CDlgLuria::OnExitDlg();

}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgLuPD::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgLuPD::OnResizeDlg()
{
}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgLuPD::InitCtrl()
{
	UINT8 i	= 0x00;

	/* Groupbox */
	for (i=0; i<3; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* text - normal */
	for (i=0; i<6; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* edit box */
	for (i=0; i<6; i++)
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
	/* Button - normal */
	for (i=0; i<1; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
}

/*
 desc : 현재 값 적재
 parm : None
 retn : None
*/
VOID CDlgLuPD::LoadValue()
{
	UINT16 i			= 0;
	UINT32 u32DCode		= 0xffffffff;
	TCHAR tzNum[2][16]	= {NULL}, tzVal[2][16] = {NULL}, tzStr[64] = {NULL};
	LPG_JMPL pstPL		= &uvEng_ShMem_GetLuria()->jobmgt.selected_job_dcode_list;
	LPG_JMPD pstPD		= &uvEng_ShMem_GetLuria()->jobmgt.selected_job_panel_data;

	m_box_ctl[0].SetRedraw(FALSE);
	m_box_ctl[0].ResetContent();

	/* D-Code 등록 */
	for (i=0; i<pstPL->count; i++)
	{
		m_box_ctl[0].AddNumber(UINT64(pstPL->d_code[i]));
	}
	/* 현재 선택된 D-Code 값 활성화 */
	if (uvEng_ShMem_GetLuria()->jobmgt.selected_d_code)
	{
		for (i=0; i<(UINT16)m_box_ctl[0].GetCount(); i++)
		{
			if ((UINT32)m_box_ctl[0].GetTextToNum(i) == uvEng_ShMem_GetLuria()->jobmgt.selected_d_code)
			{
				m_box_ctl[0].SetCurSel(i);
				break;
			}
		}
	}

	m_box_ctl[0].SetRedraw(TRUE);
	m_box_ctl[0].Invalidate(FALSE);
}

/*
 desc : 새로운 값으로 갱신
 parm : None
 retn : None
*/
VOID CDlgLuPD::UpdateCtrl()
{
	UINT16 i			= 0;
	TCHAR tzNum[2][16]	= {NULL}, tzVal[2][16] = {NULL}, tzStr[64] = {NULL};
	LPG_JMPD pstPD		= &uvEng_ShMem_GetLuria()->jobmgt.selected_job_panel_data;

	/* 현재 선택된 D-Code가 있는지 여부 */
	if (uvEng_ShMem_GetLuria()->jobmgt.selected_d_code < 1)	return;

	/* Type */
	switch (pstPD->type)
	{
	case ENG_PDTC::en_text		:	swprintf_s(tzStr, 64, L"0 - Text");		break;
	case ENG_PDTC::en_qr_code	:	swprintf_s(tzStr, 64, L"1 - QR Code");	break;
	case ENG_PDTC::en_ecc_200	:	swprintf_s(tzStr, 64, L"2 - ECC 200");	break;
	}
	m_edt_ctl[0].SetTextToStr(tzStr, TRUE);

	/* Content */
	switch (pstPD->content)
	{
	case ENG_PDCC::en_serial	:	swprintf_s(tzStr, 64, L"0 - Serial Number");	break;
	case ENG_PDCC::en_scale		:	swprintf_s(tzStr, 64, L"1 - Scale Value");		break;
	case ENG_PDCC::en_text		:	swprintf_s(tzStr, 64, L"2 - General Text");		break;
	}
	m_edt_ctl[1].SetTextToStr(tzStr, TRUE);
	
	/* Inverse */
	if (pstPD->inverse)	m_edt_ctl[2].SetTextToStr(L"True", TRUE);
	else				m_edt_ctl[2].SetTextToStr(L"False", TRUE);

	/* Rotation */
	swprintf_s(tzStr, 64, L"%u °", pstPD->rotation);
	m_edt_ctl[3].SetTextToStr(tzStr, TRUE);

	/* Field Size (Horz / Vert) */
	swprintf_s(tzStr, 64, L"%.4f mm", pstPD->size[0]/1000000.0f);
	m_edt_ctl[4].SetTextToStr(tzStr, TRUE);
	swprintf_s(tzStr, 64, L"%.4f mm", pstPD->size[1]/1000000.0f);
	m_edt_ctl[5].SetTextToStr(tzStr, TRUE);

	/* Location */
	m_box_ctl[1].SetRedraw(FALSE);
	m_box_ctl[1].ResetContent();

	for (i=0; i<pstPD->location_count; i++)
	{
		/* mm 단위로 변환 후 문자열로 저장 */
		swprintf_s(tzNum[0], 16, L"X : %9.6f", pstPD->coord_xy[i].x / 1000000.0f);
		swprintf_s(tzNum[1], 16, L"Y : %9.6f", pstPD->coord_xy[i].y / 1000000.0f);
		/* 동일한 크기로 변환 */
		wmemcpy_s(tzVal[0], 16, tzNum[0], 12);
		wmemcpy_s(tzVal[1], 16, tzNum[1], 12);
		/* 하나의 결과로 병합 */
		swprintf_s(tzStr, 64, L"%s   |   %s", tzVal[0], tzVal[1]);
		m_box_ctl[1].AddString(tzStr);
	}

	m_box_ctl[1].SetRedraw(TRUE);
	m_box_ctl[1].Invalidate(FALSE);
}

/*
 desc : D-Code 리스트 박스의 Item 변경이 발생한 경우
 parm : None
 retn : None
*/
VOID CDlgLuPD::OnBoxSelchangeDcode()
{
	UINT32 u32DCode	= 0xffffffff;	/* 무조건 최대 값 임시 설정 */
	LPG_JMPD pstPD	= &uvEng_ShMem_GetLuria()->jobmgt.selected_job_panel_data;
	LPG_LDJM pstJM	= &uvEng_ShMem_GetLuria()->jobmgt;
	CUniToChar csCnv;

	/* 현재 선택된 위치의 값 (D-Code) 얻기 */
	if (m_box_ctl[0].GetSelNum() < 0)
	{
		AfxMessageBox(L"The selected item does not exist", MB_ICONSTOP|MB_TOPMOST);
		return;
	}
	u32DCode	= (UINT32)m_box_ctl[0].GetSelNum();

	/* 만약 기존 선택된 값과 동일하면 요청하지 않음 */
	if (u32DCode == uvEng_ShMem_GetLuria()->jobmgt.selected_d_code)	return;
	if (!pstJM->IsSelectedJobName())	return;
	/* Luria Service에 새로운 Panel Data 요청 */
	uvEng_Luria_ReqGetPanelDataInfo(u32DCode);
}
