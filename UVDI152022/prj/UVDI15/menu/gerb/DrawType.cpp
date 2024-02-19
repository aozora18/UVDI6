
/*
 desc : Align Mark Array Image ���
 */

#include "pch.h"
#include "../../MainApp.h"
#include "DrawType.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif


/*
 desc : ������
 parm : h_draw	- [in]  �̹����� ��µǴ� ���� �ڵ�
 retn : None
*/
CDrawType::CDrawType(HWND h_draw)
{
	m_enType	= ENG_ATGL::en_global_0_local_0x0_n_point;
	m_hDraw		= h_draw;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDrawType::~CDrawType()
{
}

/*
 desc : ���� �� ����� ��� ���� ���
 parm : index	- [in]  ��� ���� ID (0x00 ~ 0x06)
		comm	- [in]  ��� ��� / ���� ���� (0x00:fail, 0x01:warn, 0x02:succ)
 retn : None
*/
VOID CDrawType::Draw()
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	TCHAR tzName[64]= { NULL };
	HDC hDC			= NULL;
	RECT rDraw		= {NULL};
	CPicture picImg;

	/* ���� ���õ� ����� ��ũ�� ���� ���� */
	switch (m_enType)
	{
	case ENG_ATGL::en_global_0_local_0x0_n_point : wcscpy_s(tzName, 64, L"global_0_local_0x0_n");	break;
	case ENG_ATGL::en_global_4_local_0x0_n_point : wcscpy_s(tzName, 64, L"global_4_local_0x0_n");	break;
	case ENG_ATGL::en_global_4_local_2x2_n_point : wcscpy_s(tzName, 64, L"global_4_local_2x2_n");	break;
	default	:	return;
	}
	/* ��� ���� ������ ���� ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\ui_img\\type\\%s.jpg", g_tzWorkDir, tzName);
	/* �̹��� ���� */
	if (!picImg.Load(tzFile))	return;

	/* �׸����� �ϴ� ���� ��� */
	::GetClientRect(m_hDraw, &rDraw);

	rDraw.left	+= 16;
	rDraw.top	+= 25;

	// by sys&j : Group Size�� ���缭 Image Draw�� ����
	//rDraw.right	= rDraw.left+ picImg.GetWidth();
	rDraw.right	 = rDraw.right - 16;
	rDraw.bottom = rDraw.top + picImg.GetHeight();

	/* �׸����� �ϴ� ���� �ڵ� ������ DC ���, �̹��� ��� */
	hDC	= ::GetDC(m_hDraw);
	if (hDC)
	{
		picImg.Show(hDC, rDraw);	/* �̹��� ��� */
		::ReleaseDC(m_hDraw, hDC);	/* DC �ڵ� ���� */
	}
}

/*
 desc : ���� �� ����� ��� ���� ���
 parm : None
 retn : None
*/
VOID CDrawType::DrawType(ENG_ATGL type)
{
	/* ȭ�� ������ ���� Mark Align Array Type �ӽ� ���� */
	m_enType	= type;
	Draw();
}

/*
 desc : ������ ǥ���� Align Mark Array Type �� ��ȯ
 parm : None
 retn : Array Type �� (ENG_ATGL)
*/
ENG_ATGL CDrawType::GetNextType()
{
	/* ���� ���õ� ����� ��ũ�� ���� ���� */
	switch (m_enType)
	{
	case ENG_ATGL::en_global_0_local_0x0_n_point: m_enType = ENG_ATGL::en_global_4_local_0x0_n_point;	break;
	case ENG_ATGL::en_global_4_local_0x0_n_point: m_enType = ENG_ATGL::en_global_4_local_2x2_n_point;	break;
	case ENG_ATGL::en_global_4_local_2x2_n_point: m_enType = ENG_ATGL::en_global_0_local_0x0_n_point;	break;
	}

	return m_enType;
}