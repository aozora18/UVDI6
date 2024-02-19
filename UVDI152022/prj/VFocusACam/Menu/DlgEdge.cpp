
/*
 desc : Align Camera - Edge Find
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgEdge.h"

#include "../Meas/Measure.h"

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
CDlgEdge::CDlgEdge(UINT32 id,CWnd* parent)
	: CDlgMenu(id, parent)
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgEdge::~CDlgEdge()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgEdge::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Picture */
	u32StartID	= IDC_EDGE_PIC_VIEW;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_pic_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgEdge, CDlgMenu)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgEdge::PreTranslateMessage(MSG* msg)
{

	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgEdge::OnInitDlg()
{
	if (!CDlgMenu::OnInitDlg())	return FALSE;

	/* 컨트롤 초기화 */
	InitCtrl();

	/* 상위 객체 호출 */
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgEdge::OnExitDlg()
{
	CDlgMenu::OnExitDlg();

}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgEdge::OnPaintDlg(CDC *dc)
{
	/* Drawing for Edge Object */
	DrawEdge();
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
 desc : 윈도가 보이지 않는지 / 보여지는지 이벤트 감지
 parm : show	- [in]  숨김/보임 여부 플래그
		status	- [in]  Specifies the status of the window being shown
						It is 0 if the message is sent because of a ShowWindow member function call; otherwise nStatus is one of the following:
						SW_PARENTCLOSING : Parent window is closing (being made iconic) or a pop-up window is being hidden.
						SW_PARENTOPENING : Parent window is opening (being displayed) or a pop-up window is being shown.
 retn : None
*/
VOID CDlgEdge::OnShowWindow(BOOL show, UINT32 status)
{
#if 0	/* OnPaint 에서 자동 호출 */
	/* 무조건 숨겨지는 경우에만 값 설정 */
	if (show)	DrawEdge();
#endif
}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgEdge::InitCtrl()
{

}

/*
 desc : 현재 윈도 컨트롤 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgEdge::UpdateCtrl()
{
}

/*
 desc : 기존 출력된 데이터 모두 제거
 parm : None
 retn : None
*/
VOID CDlgEdge::ResetData()
{
}

/*
 desc : 현재 저장된 Edge 이미지 출력
 parm : None
 retn : None
*/
VOID CDlgEdge::DrawEdge()
{
	TCHAR tzDiaSize[128]= {NULL};
	INT32 u32MoveX, u32MoveY, i32BkMode;
	UINT32 u32DstW, u32DstH;
	DOUBLE dbRate		= 0.0f;
	COLORREF clrText	= RGB(255, 255, 0), clrOld;
	LPG_ZAAL pstGrabSel	= m_pDlgMain->GetGrabSel();
	CFont ftText, *ftOld;
	CRect rDraw;
	CImage *pImgEdge	= m_pDlgMain->GetMeasure()->GetEdgeImage();

	/* 현재 적재된 이미지가 있는지 여부에 따라 */
	if (pImgEdge->IsNull())	return;

	/* DC 얻기 */
	CDC *pDC	= GetDC();
	if (!pDC)	return;

	/* Drawing Area 구하기 */
	m_pic_ctl[0].GetWindowRect(rDraw);
	ScreenToClient(rDraw);

	/* 이미지 크기와 출력될 영역 크기 비교 */
	rDraw.left	+= 1;
	rDraw.right	-= 1;
	rDraw.top	+= 1;
	rDraw.bottom-= 1;

	/* 가로 크기 비율이 세로 크기 비율보다 큰 경우 */
	if ((DOUBLE(rDraw.Width()) / DOUBLE(pImgEdge->GetWidth())) >
		(DOUBLE(rDraw.Height()) / DOUBLE(pImgEdge->GetHeight())))
	{
		/* <세로> 비율 기준으로 영상 확대 or 축소 진행 */
		dbRate	= DOUBLE(rDraw.Height()) / DOUBLE(pImgEdge->GetHeight());
	}
	/* 세로 크기 비율이 가로 크기 비율보다 큰 경우 */
	else
	{
		/* <가로> 비율 기준으로 영상 확대 or 축소 진행 */
		dbRate	= DOUBLE(rDraw.Width()) / DOUBLE(pImgEdge->GetWidth());
	}
	/* 윈도 이미지 영역에 맞게 영상 이미지 크기 조정 */
	u32DstW	= (UINT32)ROUNDED(dbRate * pImgEdge->GetWidth(), 0);
	u32DstH	= (UINT32)ROUNDED(dbRate * pImgEdge->GetHeight(),0);

	/* 이미지 출력 시작 위치 계산 */
	u32MoveX= (UINT32)ROUNDED(DOUBLE((rDraw.Width()) - u32DstW) / 2.0f + 1.0f, 0);
	u32MoveY= (UINT32)ROUNDED(DOUBLE((rDraw.Height()) - u32DstH) / 2.0f + 1.0f, 0);

	/* 이미지 출력 */
	pImgEdge->StretchBlt(pDC->m_hDC,
						 rDraw.left + u32MoveX, rDraw.top + u32MoveY, u32DstW-2, u32DstH-2,
						 0, 0, pImgEdge->GetWidth(), pImgEdge->GetHeight());

	/* 텍스트 출력 */
	clrOld		= pDC->SetTextColor(clrText);
	i32BkMode	= pDC->SetBkMode(TRANSPARENT);
	/* Font 설정 */
	if (ftText.CreateFontIndirectW(&g_lf))
	{
		ftOld	= pDC->SelectObject(&ftText);
		if (ftOld)
		{
			/* Text Ouput */
			swprintf_s(tzDiaSize, 128, L"Circle Size = %.4f (px)", pstGrabSel->avg_value[2]);
			pDC->TextOutW(rDraw.left+20, rDraw.top+20, tzDiaSize, (INT32)wcslen(tzDiaSize));
		}
		pDC->SelectObject(ftOld);
		ftText.DeleteObject();
	}

	/* Text Color 복구 */
	pDC->SetTextColor(clrOld);
	pDC->SetBkMode(i32BkMode);
	/* DC 해제 */
	ReleaseDC(pDC);
}

