
/*
 desc : Align Camera - Histogram
*/

#include "pch.h"
#include "../MainApp.h"
#include "DlgHist.h"

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
CDlgHist::CDlgHist(UINT32 id,CWnd* parent)
	: CDlgMenu(id, parent)
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgHist::~CDlgHist()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgHist::DoDataExchange(CDataExchange* dx)
{
	CDlgMenu::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Picture */
	u32StartID	= IDC_HIST_PIC_VIEW;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_pic_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgHist, CDlgMenu)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgHist::PreTranslateMessage(MSG* msg)
{

	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgHist::OnInitDlg()
{
	if (!CDlgMenu::OnInitDlg())	return FALSE;

	/* 컨트롤 초기화 */
	InitCtrl();
	if (!InitMemDC())
	{
		AfxMessageBox(L"Failed to create the image buffer for Histogram", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* 상위 객체 호출 */
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgHist::OnExitDlg()
{
	CDlgMenu::OnExitDlg();

	/* 이미지 버퍼 메모리 해제 */
	CloseMemDC();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgHist::OnPaintDlg(CDC *dc)
{
	DrawHist();
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgHist::OnResizeDlg()
{
}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : None
*/
VOID CDlgHist::InitCtrl()
{
}

/*
 desc : 현재 윈도 컨트롤 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgHist::UpdateCtrl()
{
}

/*
 desc : 기존 출력된 데이터 모두 제거
 parm : None
 retn : None
*/
VOID CDlgHist::ResetData()
{
}

/*
 desc : Memory DC & Bitmap 생성 및 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgHist::InitMemDC()
{
	/* Histogram이 출력될 영역 구하기 */
	m_pic_ctl[0].GetWindowRect(m_rMemHist);
	ScreenToClient(m_rMemHist);
	m_rMemHist.top		+= 5;
	m_rMemHist.bottom	-= 5;
	m_rMemHist.left		+= 5;
	m_rMemHist.right	-= 5;

	/* 현재 화면 DC 얻기 */
	CDC *pDC	= GetDC();
	if (!pDC)	return FALSE;

	/* Memory DC & Bitmap 생성 */
	if (!m_dcMemHist.CreateCompatibleDC(pDC))
	{
		ReleaseDC(pDC);
		return FALSE;
	}
	if (!m_bmMemHist.CreateCompatibleBitmap(pDC, m_rMemHist.Width(), m_rMemHist.Height()))
	{
		m_dcMemHist.DeleteDC();
		ReleaseDC(pDC);
		return FALSE;
	}
	/* Memory DC와 Bitmap 연결 */
	m_bmHistOld	= (CBitmap *)m_dcMemHist.SelectObject(&m_bmMemHist);
	if (!m_bmHistOld)
	{
		if (m_bmMemHist.GetSafeHandle())
		{
			m_bmMemHist.DeleteObject();
			m_dcMemHist.DeleteDC();
		}
		ReleaseDC(pDC);
		return FALSE;
	}

	/* DC 반환 */
	ReleaseDC(pDC);

	return TRUE;
}

/*
 desc : Memory DC 영역 해제
 parm : None
 retn : None
*/
VOID CDlgHist::CloseMemDC()
{
	if (m_dcMemHist.GetSafeHdc())		m_dcMemHist.DeleteDC();
	if (m_bmMemHist.GetSafeHandle())	m_bmMemHist.DeleteObject();
}

/*
 desc : 현재 선택된 Edge Image에 대한 Level (Gray 색상 강도) 값을 그래프로 출력
 parm : None
 retn : None
*/
VOID CDlgHist::DrawHist()
{
	INT32 i, j, i32Pitch;
	UINT32 *pLevel	= NULL, u32Max = 0, u32Min = UINT32_MAX, u32NextX, u32RateY;
	DOUBLE dbPixelRateW, dbPixelRateH;
	PUINT8 pImgGray	= NULL, pImgRow = NULL;
	CPen csPen, *pOldPen;
	CDC *pDC	= NULL;
	CImage *pImgEdge	= m_pDlgMain->GetMeasure()->GetEdgeImage();

	/* Image가 적재되지 않았는지 유효성 확인 */
	if (pImgEdge->IsNull())	return;

	/* 이미지 버퍼 포인터 연결 */
	pImgGray= (PUINT8)pImgEdge->GetBits();
	if (!pImgGray)	return;

	/* 256 gray color의 Level 들이 저장될 버퍼 할당 */
	pLevel	= (PUINT32)Alloc(sizeof(UINT32) * 256);
	memset(pLevel, 0x00, sizeof(UINT32) * 256);

	/* 전체 이미지 조사 후, 각 Gray Color 별로 개수 구하기 */
	i32Pitch	= pImgEdge->GetPitch();
	for (j=0; j<pImgEdge->GetHeight(); j++)
	{
		pImgRow	= pImgGray + j * i32Pitch;
		for (i=0; i<pImgEdge->GetWidth(); i++)
		{
			pLevel[pImgRow[i]]++;
			if (u32Max < pLevel[pImgRow[i]])	u32Max = pLevel[pImgRow[i]];
			if (u32Min > pLevel[pImgRow[i]])	u32Min = pLevel[pImgRow[i]];
		}
	}

	/* 가로 (넓이) 영역을 256 등분 (256 gray color)으로 나누기 위해, 1 값당 픽셀 크기 비율 값 */
	dbPixelRateW	= (m_rMemHist.Width() - 2) / 256.0f;
	/* 세로 (높이) 영역을 Max - Min 등분으로 나누기 위해, 1 값당 픽셀 크기 비율 값 */
	dbPixelRateH	= (m_rMemHist.Height() - 2) / DOUBLE(u32Max - u32Min);

	/* Memory DC 영역에 그래프 그리기 */
	if (!csPen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0)))
	{
		if (csPen.GetSafeHandle())	csPen.DeleteObject();
		Free(pLevel);
		return;
	}

	/* Memory DC에 CBrush & CPen 연결 */
	pOldPen		= m_dcMemHist.SelectObject(&csPen);
	if (!pOldPen)
	{
		if (csPen.GetSafeHandle())	csPen.DeleteObject();
		Free(pLevel);
		return;
	}
	/* Memory DC 영역에 검정색으로 칠하기 */
	m_dcMemHist.FillSolidRect(0, 0, m_rMemHist.Width(), m_rMemHist.Height(), RGB(0, 0, 0));

	/* 256 Gray Color Level를 그래프 (곡선)로 표현 */
	u32RateY	= (m_rMemHist.Height() - 1) - (UINT32)ROUNDED(dbPixelRateH * pLevel[0], 0);
	m_dcMemHist.MoveTo(0, u32RateY);	/* 원점 (0, 0)이 Left / Top 이므로 */
	for (i=1; i<256; i++)
	{
		u32NextX	= (UINT32)ROUNDED(dbPixelRateW * i, 0) + 1;
		u32RateY	= (m_rMemHist.Height() - 1) - (UINT32)ROUNDED(dbPixelRateH * pLevel[i], 0);
		m_dcMemHist.LineTo(u32NextX, u32RateY);
	}

	/* 화면 DC 얻기 */
	pDC	= GetDC();
	if (pDC)
	{
		/* 이미지 출력 */
		pDC->StretchBlt(m_rMemHist.left, m_rMemHist.top, m_rMemHist.Width(), m_rMemHist.Height(),
						&m_dcMemHist, 0, 0, m_rMemHist.Width(), m_rMemHist.Height(), SRCCOPY);
		/* 화면 DC 반환 */
		ReleaseDC(pDC);
	}
	/* 메모리 해제 */
	Free(pLevel);
}

