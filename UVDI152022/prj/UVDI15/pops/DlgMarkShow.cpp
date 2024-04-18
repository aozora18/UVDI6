
/*
 desc : 프로그램 종료될 때, 대기 알림
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgMarkShow.h"
#include "../mesg/DlgMesg.h"
#include <afxtaskdialog.h>
#include "..\GlobalVariables.h"

#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

IMPLEMENT_DYNAMIC(CDlgMarkShow, CDialogEx)

/*
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		pParent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
 retn : None
*/
CDlgMarkShow::CDlgMarkShow(CWnd* pParent /*=NULL*/)
	: CMyDialog(CDlgMarkShow::IDD, pParent)
{
	m_hTxtMark = NULL;
	m_hPicMark = NULL;
	m_nMarkIndex = 0;
	m_bIsLocal = FALSE;
}

/*
 desc : 소멸자
 parm : None
 retn : None
*/
CDlgMarkShow::~CDlgMarkShow()
{
	
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgMarkShow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PIC_MARK_IMAGE, m_picMark);
	DDX_Control(pDX, IDC_TXT_MARK_INFO, m_txtMark);
	DDX_Control(pDX, IDC_TXT_MARK_TITLE, m_txtTitle);
}

BEGIN_MESSAGE_MAP(CDlgMarkShow, CMyDialog)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDOK, &CDlgMarkShow::OnBnClickedOk)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgMarkShow::PreTranslateMessage(MSG* msg)
{
	if (msg->message == WM_KEYDOWN)
	{
		if (msg->wParam == VK_RETURN || msg->wParam == VK_ESCAPE)
		{
			return TRUE;                // Do not process further
		}
	}

	return CDialogEx::PreTranslateMessage(msg);
}

#pragma region Initalize
/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMarkShow::OnInitDlg()
{
	/* TOP_MOST & Center */
	// CenterParentTopMost();
	CDialogEx::OnInitDialog();

	CResizeUI clsResizeUI;
	clsResizeUI.ResizeControl(this, this);

	/* 컨트롤 생성 */
	CreateControl();


	return TRUE;
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgMarkShow::OnPaintDlg(CDC * pDc)
{
	if (!uvEng_Camera_GetGrabbedCount())
	{
		DrawInit();
	}
	else
	{
		DrawMark(m_nMarkIndex);
	}
}

/*
 desc : 각 컨트롤들을 생성한다.
 parm : None
 retn : None
*/
VOID CDlgMarkShow::CreateControl()
{
	CResizeUI clsResizeUI;

	m_hTxtMark = m_txtMark.GetSafeHwnd();
	m_hPicMark = m_picMark.GetSafeHwnd();

	// by sysandj : Resize UI
	clsResizeUI.ResizeControl(this, &m_txtTitle);
	clsResizeUI.ResizeControl(this, &m_txtMark);
	clsResizeUI.ResizeControl(this, &m_picMark);
	clsResizeUI.ResizeControl(this, GetDlgItem(IDOK));
	// by sysandj : Resize UI

	m_picMark.GetClientRect(&m_rPicMark);

	DrawInit();
}

VOID CDlgMarkShow::DrawInit()
{
	UINT8 i = 0x00;
	TCHAR tzFile[MAX_PATH_LEN] = { NULL };
	TCHAR tzMark[128] = { NULL };
	CPicture picImg;
	CMyStatic* pText = NULL;

	UINT8 u8ACamNo = GetMarkACamNoFromIndex(m_nMarkIndex, m_bIsLocal);
	UINT8 u8ImgNo = GetMarkImgIDFromIndex(m_nMarkIndex, u8ACamNo,m_bIsLocal);

	/* 이미지 적재 */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\ui_img\\status\\mark_init.jpg", g_tzWorkDir);
	if (!picImg.Load(tzFile))	return;

	m_txtTitle.SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
	m_txtTitle.SetDrawBg(1);
	m_txtTitle.SetBaseProp(0, 1, 0, 0, FORD_GRAPHITE, 0, WHITE_);

	swprintf_s(tzMark, 128, L"[%d.%02d] [SCORE 000.000] [SCALE 000.000 0] [%%] [X +000.0] [Y +000.0] [um]",
		u8ACamNo, u8ImgNo);
	m_txtMark.SetWindowText(tzMark);
	m_txtMark.SetTextFont(&g_lf[eFONT_LEVEL2_BOLD]);
	m_txtMark.SetDrawBg(1);
	m_txtMark.SetTextColor(RGB(128, 128, 128));
	m_txtMark.Invalidate(FALSE);

	/* 이미지 출력 */
	if (m_hPicMark)
	{
		HDC hDC = ::GetDC(m_hPicMark);
		if (hDC)
		{
			picImg.Show(hDC, m_rPicMark);
			::ReleaseDC(m_hPicMark, hDC);
		}
	}
}

LPG_ACGR CDlgMarkShow::GetMarkInfo(int index, BOOL bIsLocal)
{
	UINT8 u8ACamNo = GetMarkACamNoFromIndex(index, bIsLocal);
	UINT8 u8ImgNo = GetMarkImgIDFromIndex(index, u8ACamNo, bIsLocal);

	return uvEng_Camera_GetGrabbedMark(u8ACamNo, u8ImgNo);
}

int CDlgMarkShow::GetMarkACamNoFromIndex(int index, BOOL bIsLocal)
{
	// 현재 global 4개 기준으로만 되어 있음
	//int nACamNo = m_nMarkIndex / 2 + 1;
	
	AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();
	auto status = motions.status;
	auto pool = status.markPoolForCam; // bIsLocal ? status.markPoolForCamLocal : status.markPoolForCamGlobal;
	
	for (int i = 1; i <= status.acamCount; i++)
	{
		auto find = std::find_if(pool[i].begin(),pool[i].end(),
								[&](const STG_XMXY val)
								{
									return val.tgt_id == index; 
								});

		if (find != pool[i].end())
		{
			// 해당하는 요소를 찾았을 때 처리
			return i;
		}
	}
	return 1;
}

int CDlgMarkShow::GetMarkImgIDFromIndex(int fiducialIndex,int camIndex,  BOOL bIsLocal)
{
	// 현재 global 4개 기준으로만 되어 있음
	//int nImgID = m_nMarkIndex % 2;
	int nImgID =-1;

	AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();
	auto status = motions.status;

	auto pool = status.markPoolForCam[camIndex];  //bIsLocal ? status.markPoolForCamLocal[camIndex] : status.markPoolForCamGlobal[camIndex];

	if (pool.size() == 0)
		return nImgID;

	//fiducialIndex *= bIsLocal ? 1 : -1;

	auto find = ::find_if(pool.begin(), pool.end(),[&](const STG_XMXY val)
	{
			return val.tgt_id == fiducialIndex;
	});

	vector<int> fidVec;
	auto grabMark = uvEng_Camera_GetGrabbedMarkAll();
	for (int i = 0; i < grabMark->GetCount(); i++)
	{
		auto item = grabMark->GetAt(grabMark->FindIndex(i));
		fidVec.push_back(item->fiducialMarkIndex);
	}
	std::sort(fidVec.begin(), fidVec.end());
	if (find != pool.end())
	{	
		int min = bIsLocal ? status.globalMarkCnt : 0;
		for (int i = 0; i < grabMark->GetCount(); i++)
		{
			auto item = grabMark->GetAt(grabMark->FindIndex(i));
			if (item->fiducialMarkIndex == fiducialIndex && 
				(item->reserve & (bIsLocal ? STG_XMXY_RESERVE_FLAG::LOCAL : STG_XMXY_RESERVE_FLAG::GLOBAL)) != 0)
			{
				nImgID = item->img_id;
				break;
			}
		}
	}

	return nImgID;
}

/*
 desc : 검색된 마크 이미지 출력
 parm : dc		- [in]  이미지를 출력할 Context (부모)
		page_no	- [in]  현재 출력하고자 하는 마크의 페이지 번호 (1:Global, 2~5:Local)
 retn : None
*/
BOOL CDlgMarkShow::DrawMark(int index)
{
	
	LPG_RJAF pstRecipe = uvEng_JobRecipe_GetSelectRecipe();
	if (pstRecipe == nullptr)
		return FALSE;

	LPG_REAF expoRecipe = uvEng_ExpoRecipe_GetSelectRecipe();

	AlignMotion& motions = GlobalVariables::GetInstance()->GetAlignMotion();

	auto alignMotion = motions.markParams.alignMotion;
	auto aligntype = motions.markParams.alignType;
	int u8ACamNo = 0;
	int u8ImgNo = 0;
	switch (alignMotion)
	{
		case ENG_AMOS::en_static_3cam:
		{
			u8ACamNo = 3;
			u8ImgNo = GetMarkImgIDFromIndex(index, u8ACamNo, m_bIsLocal);
		}
		break;
		
		default:
		{
			u8ACamNo = GetMarkACamNoFromIndex(index, m_bIsLocal);
			u8ImgNo = GetMarkImgIDFromIndex(index, u8ACamNo, m_bIsLocal);
		}
		break;

	}

	
	if (u8ImgNo == -1)return FALSE;
	
	TCHAR tzMark[128] = { NULL };
	/*BOOL bRedraw		= FALSE;*/
	LPG_ACGR pstMark = NULL;
	
	COLORREF clrText[2] = { RGB(255, 0, 0), RGB(34, 177, 76) };
	CMyStatic* pText = NULL;

	CString strTitle;
	strTitle.Format(_T("%s MARK %02d (ACAM ID : %d, IMAGE ID : %d)"), m_bIsLocal ? _T("LOCAL") : _T("GLOBLA"), m_nMarkIndex, u8ACamNo, u8ImgNo);
	m_txtTitle.SetWindowText(strTitle);

	pstMark = uvEng_Camera_GetGrabbedMark(u8ACamNo, u8ImgNo);
	if (!pstMark)	return FALSE;
	/* Output the grabbed images on screen */
	//uvEng_Camera_DrawMarkMBufID(m_hPicMark, m_rPicMark, pstMark->cam_id, pstMark->img_id,pstMark->img_id);
	HDC hcd = ::GetDC(m_hPicMark);
	uvEng_Camera_DrawMarkDataBitmap(hcd, m_rPicMark, pstMark, pstMark->marked,true,0);
	::ReleaseDC(m_hPicMark,hcd);
	/* Update the grabbed results to the text buffer */
	swprintf_s(tzMark, 128, L"[%d.%02d] [SCORE %6.3f] [SCALE %6.3f %u] [%%] [X %+4.2f] [Y %+4.2f] [um]",
		pstMark->cam_id, pstMark->img_id + 1, pstMark->score_rate, pstMark->scale_rate, pstMark->scale_size,
		pstMark->move_mm_x * 1000.0f, pstMark->move_mm_y * 1000.0f);

	/* Check if it is higher than the mark valid score of the registered recipe */
	UINT8 u8Mark = pstMark->IsMarkValid() ? 0x01 : 0x00;
	if (u8Mark && expoRecipe)
	{
		u8Mark = expoRecipe->mark_score_accept <= pstMark->score_rate ? 0x01 : 0x00;
	}

	/* Output the grabbed results to the text control */
	m_txtMark.SetWindowText(tzMark);
	m_txtMark.SetTextColor(clrText[u8Mark]);
	m_txtMark.Invalidate(FALSE);

	return TRUE;
}
#pragma endregion

/*
 desc : 일정 시간마다 동작을 수행한다.
 parm : 타이머 ID
 retn : None
*/
void CDlgMarkShow::OnTimer(UINT_PTR nIDEvent)
{
	if (FALSE == IsWindowVisible())
	{
		return;
	}

	CMyDialog::OnTimer(nIDEvent);
}

/*
 desc : 화면이 띄워져 있는지 확인한다.
 parm : 지금 화면이 보이는지의 대한 여부와 상태
 retn : None
*/
void CDlgMarkShow::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CMyDialog::OnShowWindow(bShow, nStatus);

	if (TRUE == bShow)
	{
		if (FALSE == DrawMark(m_nMarkIndex))
		{
			DrawInit();
		}
	}
	else
	{
		// 화면에 보이지 않는 상황에서 타이머를 돌릴 필요는 없다.
		
	}
}

/*
 desc : 상단바에 대한 이벤트를 받는다.
 parm : 상단바에서 발생된 이벤트 ID
 retn : None
*/
void CDlgMarkShow::OnSysCommand(UINT nID, LPARAM lParam)
{
	//종료버튼 눌릴 시
	if (nID == SC_CLOSE)
	{

	}

	CMyDialog::OnSysCommand(nID, lParam);
}

void CDlgMarkShow::OnBnClickedOk()
{
	CMyDialog::OnOK();
}
