
/*
 desc : Edge Detection Results 출력
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
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgEdge::CDlgEdge(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgEdge::IDD, parent)
{
	m_bDrawBG		= TRUE;
	m_bTooltip		= TRUE;
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
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
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgEdge::PreTranslateMessage(MSG* msg)
{

	return CMyDialog::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgEdge::OnInitDlg()
{
	/* 컨트롤 초기화 */
	if (!InitCtrl())	return FALSE;
	/* 검색된 개수가 없는 경우, Apply 버튼 비활성화 */
	if (uvEng_Camera_GetEdgeDetectCount(m_u8ACamID))
	{
		/* Load Result Data */
		LoadData();
	}

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgEdge::OnExitDlg()
{
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgEdge::OnPaintDlg(CDC *dc)
{
	DrawPattern(dc);
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgEdge::OnResizeDlg()
{
}

/*
 desc : 공통 컨트롤 초기화
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
 desc : 현재 발생된 에러 정보 출력
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
 desc : Edge Find Result Data 적재
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
 desc : Box 컨트롤을 클릭한 경우
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

	/* 현재 선택된 위치 값 저장 */
	if (i32CurSel < 0)	return;

	/* 임시 저장 파일 설정 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\save_img\\edge\\uvdi15_temp_mark.bmp", g_tzWorkDir);
	/* 임시 저장 파일로 현재 선택된 마크 저장 */
	if (!uvEng_Camera_SaveGrabbedMarkToFile(m_u8ACamID,
											&uvEng_Camera_GetEdgeDetectResults(m_u8ACamID)[i32CurSel].box_area,
											0x00,
											tzFile))
	{
		dlgMesg.MyDoModal(L"Failed to save temporary file", 0x01);
		return;
	}
	/* 기존 선택된 이미지가 있다면 제거 */
	if (!m_csImgMark.IsNull())	m_csImgMark.Destroy();
	/* Bitmap Image 적재 */
	if (S_OK != m_csImgMark.Load(tzFile))
	{
		dlgMesg.MyDoModal(L"", 0x01);
		return;
	}

	/* 현재 선택된 Model Type에 따라 값 입력 다르게 */
	m_edt_ctl[eEDGE_EDT_SIZE_1].SetTextToNum(uvEng_Camera_GetEdgeDetectResults(m_u8ACamID)[i32CurSel].dia_meter_um / 2.0f, 1);
	m_edt_ctl[eEDGE_EDT_SIZE_2].SetTextToNum(uvEng_Camera_GetEdgeDetectResults(m_u8ACamID)[i32CurSel].width_um, 1);
	m_edt_ctl[eEDGE_EDT_SIZE_3].SetTextToNum(uvEng_Camera_GetEdgeDetectResults(m_u8ACamID)[i32CurSel].height_um, 1);
	/* 현재 영역 갱신 */
	InvalidateMark();
}

/*
 desc : 마크 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgEdge::InvalidateMark()
{
	CRect rPic;

	/* 현재 Mark 영역 얻기 */
	m_pic_ctl[eEDGE_PIC_MARK].GetWindowRect(rPic);
	ScreenToClient(rPic);
	/* Mark 영역만 갱신 */
	InvalidateRect(rPic);
}

/*
 desc : 설정된 값으로 패턴 출력
 parm : dc	- [in]  Device Context for dialog windows
 retn : None
*/
VOID CDlgEdge::DrawPattern(CDC *dc)
{
	INT32 u32MoveX, u32MoveY;
	UINT32 u32DstW, u32DstH;
	DOUBLE dbRate	= 0.0f;
	CRect rDraw;

	/* 선택된 Mark가 없거나 적재된 Mark 이미지가 없는지 확인 */
	if (m_box_ctl[eEDGE_BOX_MAX].GetCurSel() < 0 || m_csImgMark.IsNull())
	{
		InvalidateMark();	/* 기존 Mark 영역 갱신 */
		return;
	}

	/* Drawing Area 구하기 */
	m_pic_ctl[eEDGE_PIC_MARK].GetWindowRect(rDraw);
	ScreenToClient(rDraw);

	/* 이미지 크기와 출력될 영역 크기 비교 */
	rDraw.DeflateRect(1,1);

	/* 가로 크기 비율이 세로 크기 비율보다 큰 경우 */
	if ((DOUBLE(rDraw.Width()) / DOUBLE(m_csImgMark.GetWidth())) >
		(DOUBLE(rDraw.Height()) / DOUBLE(m_csImgMark.GetHeight())))
	{
		/* <세로> 비율 기준으로 영상 확대 or 축소 진행 */
		dbRate	= DOUBLE(rDraw.Height()) / DOUBLE(m_csImgMark.GetHeight());
	}
	/* 세로 크기 비율이 가로 크기 비율보다 큰 경우 */
	else
	{
		/* <가로> 비율 기준으로 영상 확대 or 축소 진행 */
		dbRate	= DOUBLE(rDraw.Width()) / DOUBLE(m_csImgMark.GetWidth());
	}
	/* 윈도 이미지 영역에 맞게 영상 이미지 크기 조정 */
	u32DstW	= (UINT32)ROUNDED(dbRate * m_csImgMark.GetWidth(), 0);
	u32DstH	= (UINT32)ROUNDED(dbRate * m_csImgMark.GetHeight(),0);

	/* 이미지 출력 시작 위치 계산 */
	u32MoveX	= (UINT32)ROUNDED(DOUBLE((rDraw.Width()) - u32DstW) / 2.0f + 1.0f, 0);
	u32MoveY	= (UINT32)ROUNDED(DOUBLE((rDraw.Height()) - u32DstH) / 2.0f + 1.0f, 0);

	/* 이미지 출력 */
	m_csImgMark.StretchBlt(dc->m_hDC,
						   rDraw.left + u32MoveX, rDraw.top + u32MoveY, u32DstW-2, u32DstH-2,
						   0, 0, m_csImgMark.GetWidth(), m_csImgMark.GetHeight());
}