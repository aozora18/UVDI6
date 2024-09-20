
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
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
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
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
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
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
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
 desc : 초기 실행시에 한 번 호출됨
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

	/* 컨트롤 초기화 */
	if (!InitCtrl())	return FALSE;
	/* Align Mark 출력 위한 DC 핸들 얻기 */
	m_hDCDraw = ::GetDC(m_pic_ctl[eMMPM_PIC_VIEW].GetSafeHwnd());
	/* Align Mark 출력 위한 영역 얻기 */
	m_pic_ctl[eMMPM_PIC_VIEW].GetClientRect(&m_rAreaDraw);
	/* 윈도 영역 재설정 */
	m_rAreaDraw.left++;
	m_rAreaDraw.top++;
	m_rAreaDraw.right--;
	m_rAreaDraw.bottom--;

	/* TOP_MOST & Center */
	CenterParentTopMost();

	InitDispMMPM();
	/* Grab.Failed 정보 얻어서 메모리 저장하기 */
#if 0
	0x00 : 실패한 마크만 가져오기, 0x01 : 모든 마크 정보 가져오기

	if (!GetMarkData(0x00))	return FALSE;
#else
	if (!GetMarkData(showAll ? 0x01 : 0x00))	return FALSE;
#endif

	//InitDispMark();
	//DispResize(GetDlgItem(IDC_MMPM_PIC_VIEW));

	/* 현재 가장 첫번째 실패한 정보 얻기 */
	FindData(0x00);

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgMmpm::OnExitDlg()
{
	POSITION pPos = NULL;
	LPG_ACGR pstGrab = NULL;

	/* CDC 핸들 메모리 제거 */
	if (m_hDCDraw)	::ReleaseDC(m_pic_ctl[eMMPM_PIC_VIEW].GetSafeHwnd(), m_hDCDraw);

	/* 메모리 해제 */
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
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgMmpm::OnPaintDlg(CDC* dc)
{
	/* Mark Pattern 출력 */
	if (m_hDCDraw && m_pstGrab)
	{
		// lk91 이미지 테스트 진행시, u8FindMark	= IsValidGrabMarkNum(cam_id, img_id) ? 0x01 : 0x00; 부분 디버깅해서 넘어가야해
		uvEng_Camera_DrawMarkBitmap(m_hDCDraw, m_rAreaDraw, m_pstGrab->cam_id, m_pstGrab->img_id);
	}
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgMmpm::OnResizeDlg()
{
}

/*
 desc : 공통 컨트롤 초기화
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
 desc : 버튼 이벤트 처리
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
 desc : 체크 박스 이벤트 처리
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
 desc : 이전 / 다음 Guide (Mark) 인식 실패한 곳 찾기
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

	/* 에러 정보가 발생된 첫 번째 항목 설정  */
	if (m_lstGrab.GetCount())
	{
		pstGrab = m_lstGrab.GetAt(m_lstGrab.FindIndex(m_u8Index));
		//m_pstGrab = pstGrab; // lk91 image test 할 때 아래 두줄 주석 후, 주석 풀기
		if (!pstGrab)	m_pstGrab = NULL;
		else
			m_pstGrab = pstGrab;//uvEng_Camera_GetGrabbedMark(pstGrab->cam_id, pstGrab->img_id);
	}

	/* 기본 값 설정 */
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

	/* 이미지 출력 영역 얻기 */
	m_pic_ctl[eMMPM_PIC_VIEW].GetWindowRect(rPic);
	ScreenToClient(rPic);
	InvalidateRect(rPic);
}

/*
 desc : 검색 실패한 Guide (Mark) 정보만 얻기
 parm : type	- [in]  0x00 : 실패한 마크만 가져오기, 0x01 : 모든 마크 정보 가져오기
 retn : TRUE or FALSE
*/
BOOL CDlgMmpm::GetMarkData(UINT8 type)
{
	UINT8 i = 0x00;
	LPG_ACGR pstGrab = NULL;
	LPG_ACGR pstTemp = NULL;

	/* 실제로 측정 (검색)된 Mark 결과 값을 임시로 저장하기 위함 */
	for (; i < uvEng_Camera_GetGrabbedCount(); i++)
	{
		/* 검색된 데이터 얻기 */
		pstTemp = uvEng_Camera_GetGrabbedMarkIndex(i);
		/* 검색된 데이터가 Guide (Mark) 인식 실패인 경우만 메모리에 임시 저장 */
		if (!pstTemp)	continue;
		/* 꼭 실패한 마크 정보만 가져온다면 ..., 성공한 마크는 Skip */
		if (type == 0x00 && 0x01 == pstTemp->marked)	continue;
		/* 검색된 새로운 마크 정보 저장 */
		pstGrab = new STG_ACGR;
		ASSERT(pstGrab);
		memcpy(pstGrab, pstTemp, sizeof(STG_ACGR) - sizeof(PUINT8));
		pstGrab->grab_data = pstTemp->grab_data;
		m_lstGrab.AddTail(pstGrab);
	}

	// lk91 이미지 test 용 (위 주석하고 아래 코드 사용하기)
	//for (; i < 1; i++)
	//{
	//	/* 검색된 데이터 얻기 */
	//	//pstTemp = uvEng_Camera_GetGrabbedMarkIndex(i);
	//	///* 검색된 데이터가 Guide (Mark) 인식 실패인 경우만 메모리에 임시 저장 */
	//	//if (!pstTemp)	continue;
	//	///* 꼭 실패한 마크 정보만 가져온다면 ..., 성공한 마크는 Skip */
	//	//if (type == 0x00 && 0x01 == pstTemp->marked)	continue;
	//	///* 검색된 새로운 마크 정보 저장 */
	//	pstGrab = new STG_ACGR;
	//	//ASSERT(pstGrab);
	//	//pstTemp = uvEng_Camera_RunModelCali(0x01, 0xff, (UINT8)0x05, 2, TRUE, uvEng_GetConfig()->mark_find.image_process);
	//	int tmpCamNo = 1;
	//	pstTemp = uvEng_Camera_RunModelCali(tmpCamNo, 0xff, DISP_TYPE_MMPM, 2, TRUE, uvEng_GetConfig()->mark_find.image_process);
	//	// Mark 가 없어서 Mark Find는 넘어가고 해당 이미지만 넘겨주는 형식
	//	//uvEng_Camera_RunModelCali(UINT8 cam_id, UINT8 mode, UINT8 dlg_id, UINT8 mark_no, BOOL useMilDisp, UINT8 img_proc) // default : UINT8 mode=0xff
	//	memcpy(pstGrab, pstTemp, sizeof(STG_ACGR) - sizeof(PUINT8));
	//	pstGrab->grab_data = pstTemp->grab_data;
	//	m_lstGrab.AddTail(pstGrab);
	//}

	return TRUE;
}

/*
 desc : 초기 값으로 복구
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
 desc : 적용한 경우, 새로 설정한 값으로 진행하기
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


	///* 실제로 측정 (검색)된 Mark 결과 값을 임시로 저장하기 위함 */
	//for (; i < uvEng_Camera_GetGrabbedCount(); i++)
	//{

	//	uvEng_Camera_GetGrabbedMark()
	//	/* 검색된 데이터 얻기 */
	//	pstGrab = uvEng_Camera_GetGrabbedMarkIndex(i);

	//	/* 검색된 데이터가 Guide (Mark) 인식 실패인 경우만 메모리에 임시 저장 */
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
 desc : 취소한 경우, 이전 값으로 덮어쓰기
 parm : None
 retn : None
*/
VOID CDlgMmpm::WorkCancel()
{
	CMyDialog::OnCancel();
}

/*
 desc : 초기 값으로 복구 시킴
 parm : None
 retn : None
*/
VOID CDlgMmpm::UndoCenter()
{
	CRect rPic;

	Restore();
	FindData(0x00);
	/* 이미지 출력 영역 얻기 */
	m_pic_ctl[eMMPM_PIC_VIEW].GetWindowRect(rPic);
	ScreenToClient(rPic);
	InvalidateRect(rPic);
}

/*
 desc : 특정 방향으로, 설정된 값만큼 이동
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

	/* 픽셀 크기 값 임시 저장 */
	dbPixelSize = uvEng_GetConfig()->acam_spec.spec_pixel_um[0 /* fixed */] / 1000.0f;
	/* None Event 즉, 확인 버튼 누른 경우 */
	if (0x00 == type)
	{
		/* 아무런 일도 하지 않는다. Vision Library가 측정한 값이 유효 값이 됨 */
		m_pstGrab->SetMarkValid(0x01);
		m_pstGrab->marked = 1;
	}
	/* Keyboard Event */
	else
	{
		double step = 0;
		/* 좌/우로 이동 */
		if (LEFT == type || RIGHT == type)
		{
			step = (0x11 == type) ? +m_dbStep : -m_dbStep;
			
			m_pstGrab->mark_cent_px_x -= step;
			m_ptCenter.x -= step;
		}
		/* 상/하로 이동 */
		else
		{
			step = (0x21 == type) ? +m_dbStep : -m_dbStep;
			
			m_pstGrab->mark_cent_px_y -= step;
			m_ptCenter.y -= step;
		}
	}


	/* 값 재설정 */
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
	stGMSR.manual_set = 0x01;	/* 수동으로 인식 했다고 설정 (이미지를 회전하지 않기 위함) */
	
	/* --------------------------- */
	/* 강제로 값이 유효하다고 설정 */
	/* --------------------------- */

	//m_pstGrab->SetMarkValid(0x01);
	/* 수동으로 값 설정 */
	uvEng_Camera_SetGrabbedMarkEx(m_pstGrab, &stGMFR, &stGMSR);

	/* 이미지 출력 영역 얻기 */
	m_pic_ctl[eMMPM_PIC_VIEW].GetWindowRect(rPic);
	ScreenToClient(rPic);
	/* Align Mark Area 갱신 */
	InvalidateRect(rPic);

	// Overlay 
	CString sTmp;
	uvEng_Camera_DrawOverlayDC(false, DISP_TYPE_MMPM, 1);
	uvEng_Camera_OverlayAddCrossList(DISP_TYPE_MMPM, 1, m_ptCenter.x, m_ptCenter.y, 20, 20, eM_COLOR_RED);
	uvEng_Camera_DrawOverlayDC(true, DISP_TYPE_MMPM, 1);
}

/*
 desc : 방향 키보드 이벤트 처리
 parm : key	- [in]  눌린 키보드 값
 retn : None
*/
VOID CDlgMmpm::KeyUpDown(UINT16 key)
{
	switch (key)
	{
	case VK_LEFT:	MoveCenter(0x11);	break;	/* 방향키 왼쪽 */
	case VK_RIGHT:	MoveCenter(0x12);	break;	/* 방향키 오른쪽 */
	case VK_UP:	MoveCenter(0x21);	break;	/* 방향키 위쪽 */
	case VK_DOWN:	MoveCenter(0x22);	break;	/* 방향키 아래쪽 */
	}
}


/*
 desc : 마우스 이벤트 (LButtonUp) 인지
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
//	///* 이미지 출력 영역 얻기 */
//	//m_pic_ctl[eMMPM_PIC_VIEW].GetWindowRect(rPic);
//	//ScreenToClient(rPic);
//	///* 해당 영역을 클릭하지 않은 경우 리턴 */
//	//if (!rPic.PtInRect(point))
//	//{
//	//	/* 마우스 클릭하지 않았다고 설정 */
//	//	m_u8LeftClick	= 0x00;
//	//	ReleaseCapture();
//	//	return;
//	//}
//	///* 마우스 커서 변경 */
//	//m_hNowCursor	= AfxGetApp()->LoadStandardCursor(IDC_CROSS);
//	//m_hPrvCursor	= ::SetCursor(m_hNowCursor);
//	///* 유효 영역 마우스 클릭 했음 */
//	//m_u8LeftClick	= 0x01;
//	///* 마우스가 클릭한 위치 임시 저장 */
//	//m_ptClickDown.x	= point.x;
//	//m_ptClickDown.y	= point.y;
//	///* 마우스 커서가 윈도를 벗어나더라도 계속 메시지 받도록 설정 */
//	//SetCapture();
//
//	CMyDialog::OnLButtonDown(flags, point);
//}

/*
 desc : 마우스 이벤트 (LButtonUp) 인지
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
//		// Rect 방향 예외처리
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
//				// 외접원의 중심 구하기
//				//double d, d2, yi;
//				CDPoint CenPoint[2];
//
//				// 각 변의 중심점 구하기
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
//				// 평행한 선은 만날 수 없다.
//				if (a1 != a2) {
//					m_ptCenter.x = (b2 - b1) / (a1 - a2);
//					m_ptCenter.y = a1 * m_ptCenter.x + b1;
//
//					//uvEng_Camera_OverlayAddCrossList(DISP_TYPE_MMPM, 1, c.x, c.y, 20, 20, eM_COLOR_RED);
//				}
//
//				//if (m_pstGrab) {
//				CRect rPic;
//				/* 이미지 출력 영역 얻기 */
//				m_pic_ctl[eMMPM_PIC_VIEW].GetWindowRect(rPic);
//				ScreenToClient(rPic);
//
//				/* 값 초기화 (반드시 필요) */
//				m_pstGrab->mark_cent_px_x = m_pstGrab->grab_width / 2.0f;
//				m_pstGrab->mark_cent_px_y = m_pstGrab->grab_height / 2.0f;
//				/* Point X/Y 위치가 rPic의 중심에서 얼마만큼 떨어졌는지 계산 */
//				m_pstGrab->mark_cent_px_x += ((rPic.left + rPic.right) / 2.0f - m_ptCenter.x) / tgt_rate;
//				m_pstGrab->mark_cent_px_y += ((rPic.top + rPic.bottom) / 2.0f - m_ptCenter.y) / tgt_rate;
//				//m_pstGrab->mark_cent_px_x += ((rPic.left + rPic.right) / 2.0f - point.x) / tgt_rate;
//				//m_pstGrab->mark_cent_px_y += ((rPic.top + rPic.bottom) / 2.0f - point.y) / tgt_rate;
//
//				/* 이동한 위치 값 적용 */
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
//	///* 마우스 좌측 버튼 클릭 해제 처리 */
//	//if (m_u8LeftClick)
//	//{
//	//	/* 만약 Mouse Click Down 위치와 Up 위치가 동일하면, 현재 클릭한 위치 기반으로 그리기 */
//	//	if (point.x == m_ptClickDown.x && point.y == m_ptClickDown.y)
//	//	{
//	//		OnMouseMove(flags, m_ptClickDown);
//	//	}
//	//	m_u8LeftClick	= 0x00;
//	//	if (m_hPrvCursor)	::SetCursor(m_hPrvCursor);	/* 커서 복구 */
//	//	ReleaseCapture();	/* SetCapture 기능 해제  */
//	//}
//
//	CMyDialog::OnLButtonUp(flags, point);
//}

/*
 desc : 마우스 이벤트 (LButtonUp) 인지
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
 desc : DoModal 함오버로드 함수
 parm : None
 retn : DoModal()
*/
INT_PTR CDlgMmpm::MyDoModal()
{
	return DoModal();
}

/* ----------------------------------------------------------------------------------------- */
/*                                 lk91 VISION 추가 함수                                     */
/* ----------------------------------------------------------------------------------------- */

/* desc: Frame Control을 MIL DISP에 연결 */
VOID CDlgMmpm::InitDispMMPM()
{
	CWnd* pWnd;
	pWnd = GetDlgItem(IDC_MMPM_PIC_VIEW);
	uvEng_Camera_SetDispMMPM(pWnd);

	DispResize(GetDlgItem(IDC_MMPM_PIC_VIEW));
}

/* desc: Frame Control을 이미지 사이즈 비율로 재조정 */
void CDlgMmpm::DispResize(CWnd* pWnd)
{
	CRect lpRect;
	pWnd->GetWindowRect(lpRect);
	ScreenToClient(lpRect);

	CSize szDraw(lpRect.right - lpRect.left, lpRect.bottom - lpRect.top);
	CSize tgtSize;

	/* 반드시 1.0f 값으로 초기화 */
	tgt_rate = 1.0f;

	/* 가로 크기 비율이 세로 크기 비율보다 큰 경우 */
	if ((DOUBLE(szDraw.cx) / DOUBLE(ACA1920_SIZE_X)) > (DOUBLE(szDraw.cy) / DOUBLE(ACA1920_SIZE_Y)))
	{
		/* <세로> 비율 기준으로 영상 확대 or 축소 진행 */
		tgt_rate = DOUBLE(szDraw.cy) / DOUBLE(ACA1920_SIZE_Y);
	}
	/* 세로 크기 비율이 가로 크기 비율보다 큰 경우 */
	else
	{
		/* <가로> 비율 기준으로 영상 확대 or 축소 진행 */
		tgt_rate = DOUBLE(szDraw.cx) / DOUBLE(ACA1920_SIZE_X);
	}
	/* 윈도 이미지 영역에 맞게 영상 이미지 크기 조정 */
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
//	// 1. ROI 값으로 AUTO CENTER 이미지 등록하기
//	LPG_ACGR pstGrab = NULL;
//
//	if (abs(um_rectArea.left - um_rectArea.right) == 0 || abs(um_rectArea.top - um_rectArea.bottom) == 0) {
//		AfxMessageBox(_T("화면에 설정하고자 하는 ROI를 세팅해주세요."));
//		return;
//	}
//
//	/* 에러 정보가 발생된 첫 번째 항목 설정  */
//	if (m_lstGrab.GetCount())
//	{
//		pstGrab = m_lstGrab.GetAt(m_lstGrab.FindIndex(m_u8Index));
//
//		m_pstGrab = pstGrab; // lk91 image test 할 때 아래 두줄 주석 후, 주석 풀기
//		//if (!pstGrab)	m_pstGrab	= NULL;
//		//else			m_pstGrab	= uvEng_Camera_GetGrabbedMark(pstGrab->cam_id, pstGrab->img_id);
//	}
//	else {
//		AfxMessageBox(_T("Grab Count Zero."));
//		return;
//	}
//	uvCmn_Camera_RegistMMPM_AutoCenter(um_rectArea, m_pstGrab->cam_id, m_pstGrab->img_id);
//
//	// 2. Dlg 생성
//	CDlgMMPMAutoCenter dlgMMPMAutoCenter;
//	if (dlgMMPMAutoCenter.DoModal() == IDOK) {
//
//	}
//
//
//}

