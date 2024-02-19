
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
 desc : ������
 parm : id		- [in]  �ڽ��� ���� ID
		parent	- [in]  �ڽ��� ȣ���� �θ� ���� Ŭ���� ������
 retn : None
*/
CDlgEdge::CDlgEdge(UINT32 id,CWnd* parent)
	: CDlgMenu(id, parent)
{
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgEdge::~CDlgEdge()
{
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
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
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgEdge::PreTranslateMessage(MSG* msg)
{

	return CDlgMenu::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgEdge::OnInitDlg()
{
	if (!CDlgMenu::OnInitDlg())	return FALSE;

	/* ��Ʈ�� �ʱ�ȭ */
	InitCtrl();

	/* ���� ��ü ȣ�� */
	return TRUE;
}

/*
 desc : ����ÿ� �� �� ȣ���
 parm : None
 retn : None
*/
VOID CDlgEdge::OnExitDlg()
{
	CDlgMenu::OnExitDlg();

}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgEdge::OnPaintDlg(CDC *dc)
{
	/* Drawing for Edge Object */
	DrawEdge();
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgEdge::OnResizeDlg()
{
}

/*
 desc : ������ ������ �ʴ��� / ���������� �̺�Ʈ ����
 parm : show	- [in]  ����/���� ���� �÷���
		status	- [in]  Specifies the status of the window being shown
						It is 0 if the message is sent because of a ShowWindow member function call; otherwise nStatus is one of the following:
						SW_PARENTCLOSING : Parent window is closing (being made iconic) or a pop-up window is being hidden.
						SW_PARENTOPENING : Parent window is opening (being displayed) or a pop-up window is being shown.
 retn : None
*/
VOID CDlgEdge::OnShowWindow(BOOL show, UINT32 status)
{
#if 0	/* OnPaint ���� �ڵ� ȣ�� */
	/* ������ �������� ��쿡�� �� ���� */
	if (show)	DrawEdge();
#endif
}

/*
 desc : ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgEdge::InitCtrl()
{

}

/*
 desc : ���� ���� ��Ʈ�� ���� ����
 parm : None
 retn : None
*/
VOID CDlgEdge::UpdateCtrl()
{
}

/*
 desc : ���� ��µ� ������ ��� ����
 parm : None
 retn : None
*/
VOID CDlgEdge::ResetData()
{
}

/*
 desc : ���� ����� Edge �̹��� ���
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

	/* ���� ����� �̹����� �ִ��� ���ο� ���� */
	if (pImgEdge->IsNull())	return;

	/* DC ��� */
	CDC *pDC	= GetDC();
	if (!pDC)	return;

	/* Drawing Area ���ϱ� */
	m_pic_ctl[0].GetWindowRect(rDraw);
	ScreenToClient(rDraw);

	/* �̹��� ũ��� ��µ� ���� ũ�� �� */
	rDraw.left	+= 1;
	rDraw.right	-= 1;
	rDraw.top	+= 1;
	rDraw.bottom-= 1;

	/* ���� ũ�� ������ ���� ũ�� �������� ū ��� */
	if ((DOUBLE(rDraw.Width()) / DOUBLE(pImgEdge->GetWidth())) >
		(DOUBLE(rDraw.Height()) / DOUBLE(pImgEdge->GetHeight())))
	{
		/* <����> ���� �������� ���� Ȯ�� or ��� ���� */
		dbRate	= DOUBLE(rDraw.Height()) / DOUBLE(pImgEdge->GetHeight());
	}
	/* ���� ũ�� ������ ���� ũ�� �������� ū ��� */
	else
	{
		/* <����> ���� �������� ���� Ȯ�� or ��� ���� */
		dbRate	= DOUBLE(rDraw.Width()) / DOUBLE(pImgEdge->GetWidth());
	}
	/* ���� �̹��� ������ �°� ���� �̹��� ũ�� ���� */
	u32DstW	= (UINT32)ROUNDED(dbRate * pImgEdge->GetWidth(), 0);
	u32DstH	= (UINT32)ROUNDED(dbRate * pImgEdge->GetHeight(),0);

	/* �̹��� ��� ���� ��ġ ��� */
	u32MoveX= (UINT32)ROUNDED(DOUBLE((rDraw.Width()) - u32DstW) / 2.0f + 1.0f, 0);
	u32MoveY= (UINT32)ROUNDED(DOUBLE((rDraw.Height()) - u32DstH) / 2.0f + 1.0f, 0);

	/* �̹��� ��� */
	pImgEdge->StretchBlt(pDC->m_hDC,
						 rDraw.left + u32MoveX, rDraw.top + u32MoveY, u32DstW-2, u32DstH-2,
						 0, 0, pImgEdge->GetWidth(), pImgEdge->GetHeight());

	/* �ؽ�Ʈ ��� */
	clrOld		= pDC->SetTextColor(clrText);
	i32BkMode	= pDC->SetBkMode(TRANSPARENT);
	/* Font ���� */
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

	/* Text Color ���� */
	pDC->SetTextColor(clrOld);
	pDC->SetBkMode(i32BkMode);
	/* DC ���� */
	ReleaseDC(pDC);
}

