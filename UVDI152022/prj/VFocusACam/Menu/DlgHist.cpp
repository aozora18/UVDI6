
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
 desc : ������
 parm : id		- [in]  �ڽ��� ���� ID
		parent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
 retn : None
*/
CDlgHist::CDlgHist(UINT32 id,CWnd* parent)
	: CDlgMenu(id, parent)
{
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgHist::~CDlgHist()
{
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
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
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgHist::PreTranslateMessage(MSG* msg)
{

	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgHist::OnInitDlg()
{
	if (!CDlgMenu::OnInitDlg())	return FALSE;

	/* ��Ʈ�� �ʱ�ȭ */
	InitCtrl();
	if (!InitMemDC())
	{
		AfxMessageBox(L"Failed to create the image buffer for Histogram", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

	/* ���� ��ü ȣ�� */
	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgHist::OnExitDlg()
{
	CDlgMenu::OnExitDlg();

	/* �̹��� ���� �޸� ���� */
	CloseMemDC();
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgHist::OnPaintDlg(CDC *dc)
{
	DrawHist();
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgHist::OnResizeDlg()
{
}

/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgHist::InitCtrl()
{
}

/*
 desc : ���� ���� ��Ʈ�� ���� ����
 parm : None
 retn : None
*/
VOID CDlgHist::UpdateCtrl()
{
}

/*
 desc : ���� ��µ� ������ ��� ����
 parm : None
 retn : None
*/
VOID CDlgHist::ResetData()
{
}

/*
 desc : Memory DC & Bitmap ���� �� �ʱ�ȭ
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgHist::InitMemDC()
{
	/* Histogram�� ��µ� ���� ���ϱ� */
	m_pic_ctl[0].GetWindowRect(m_rMemHist);
	ScreenToClient(m_rMemHist);
	m_rMemHist.top		+= 5;
	m_rMemHist.bottom	-= 5;
	m_rMemHist.left		+= 5;
	m_rMemHist.right	-= 5;

	/* ���� ȭ�� DC ��� */
	CDC *pDC	= GetDC();
	if (!pDC)	return FALSE;

	/* Memory DC & Bitmap ���� */
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
	/* Memory DC�� Bitmap ���� */
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

	/* DC ��ȯ */
	ReleaseDC(pDC);

	return TRUE;
}

/*
 desc : Memory DC ���� ����
 parm : None
 retn : None
*/
VOID CDlgHist::CloseMemDC()
{
	if (m_dcMemHist.GetSafeHdc())		m_dcMemHist.DeleteDC();
	if (m_bmMemHist.GetSafeHandle())	m_bmMemHist.DeleteObject();
}

/*
 desc : ���� ���õ� Edge Image�� ���� Level (Gray ���� ����) ���� �׷����� ���
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

	/* Image�� ������� �ʾҴ��� ��ȿ�� Ȯ�� */
	if (pImgEdge->IsNull())	return;

	/* �̹��� ���� ������ ���� */
	pImgGray= (PUINT8)pImgEdge->GetBits();
	if (!pImgGray)	return;

	/* 256 gray color�� Level ���� ����� ���� �Ҵ� */
	pLevel	= (PUINT32)Alloc(sizeof(UINT32) * 256);
	memset(pLevel, 0x00, sizeof(UINT32) * 256);

	/* ��ü �̹��� ���� ��, �� Gray Color ���� ���� ���ϱ� */
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

	/* ���� (����) ������ 256 ��� (256 gray color)���� ������ ����, 1 ���� �ȼ� ũ�� ���� �� */
	dbPixelRateW	= (m_rMemHist.Width() - 2) / 256.0f;
	/* ���� (����) ������ Max - Min ������� ������ ����, 1 ���� �ȼ� ũ�� ���� �� */
	dbPixelRateH	= (m_rMemHist.Height() - 2) / DOUBLE(u32Max - u32Min);

	/* Memory DC ������ �׷��� �׸��� */
	if (!csPen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0)))
	{
		if (csPen.GetSafeHandle())	csPen.DeleteObject();
		Free(pLevel);
		return;
	}

	/* Memory DC�� CBrush & CPen ���� */
	pOldPen		= m_dcMemHist.SelectObject(&csPen);
	if (!pOldPen)
	{
		if (csPen.GetSafeHandle())	csPen.DeleteObject();
		Free(pLevel);
		return;
	}
	/* Memory DC ������ ���������� ĥ�ϱ� */
	m_dcMemHist.FillSolidRect(0, 0, m_rMemHist.Width(), m_rMemHist.Height(), RGB(0, 0, 0));

	/* 256 Gray Color Level�� �׷��� (�)�� ǥ�� */
	u32RateY	= (m_rMemHist.Height() - 1) - (UINT32)ROUNDED(dbPixelRateH * pLevel[0], 0);
	m_dcMemHist.MoveTo(0, u32RateY);	/* ���� (0, 0)�� Left / Top �̹Ƿ� */
	for (i=1; i<256; i++)
	{
		u32NextX	= (UINT32)ROUNDED(dbPixelRateW * i, 0) + 1;
		u32RateY	= (m_rMemHist.Height() - 1) - (UINT32)ROUNDED(dbPixelRateH * pLevel[i], 0);
		m_dcMemHist.LineTo(u32NextX, u32RateY);
	}

	/* ȭ�� DC ��� */
	pDC	= GetDC();
	if (pDC)
	{
		/* �̹��� ��� */
		pDC->StretchBlt(m_rMemHist.left, m_rMemHist.top, m_rMemHist.Width(), m_rMemHist.Height(),
						&m_dcMemHist, 0, 0, m_rMemHist.Width(), m_rMemHist.Height(), SRCCOPY);
		/* ȭ�� DC ��ȯ */
		ReleaseDC(pDC);
	}
	/* �޸� ���� */
	Free(pLevel);
}

