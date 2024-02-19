
/*
 desc : �� ����� ��� �� ���� ���� ���
 */

#include "pch.h"
#include "../../MainApp.h"
#include "DrawDev.h"


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
CDrawDev::CDrawDev(HWND *h_draw)
{
	/* ���� �ڵ� ���� */
	memcpy(m_hDraw, h_draw, sizeof(HWND) * 9);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDrawDev::~CDrawDev()
{
}

/*
 desc : ���� �� ����� ��� ���� ���
 parm : index	- [in]  ��� ���� ID (0x00 ~ 0x06)
		comm	- [in]  ��� ��� / ���� ���� (0x00:fail, 0x01:warn, 0x02:succ)
 retn : None
*/
VOID CDrawDev::DrawDev(UINT8 index, UINT8 comm)
{
	TCHAR tzFile[MAX_PATH_LEN]	= {NULL};
	TCHAR tzName[3][32]	= { L"dev_fail", L"dev_warn", L"dev_succ" };
	HDC hDC		= NULL;
	RECT rDraw	= {NULL};
	CPicture picImg;

	/* �׸����� �ϴ� ���� ��� */
	::GetClientRect(m_hDraw[index], &rDraw);

	/* ��� ���� ������ ���� ���� */
	swprintf_s(tzFile, MAX_PATH_LEN, L"%s\\ui_img\\status\\%s.jpg", g_tzWorkDir, tzName[comm]);
	/* �̹��� ���� */
	if (!picImg.Load(tzFile))	return;
	/* �׸����� �ϴ� ���� �ڵ� ������ DC ���, �̹��� ��� */
	hDC	= ::GetDC(m_hDraw[index]);
	if (hDC)
	{
		picImg.Show(hDC, rDraw);		/* �̹��� ��� */
		::ReleaseDC(m_hDraw[index], hDC);	/* DC �ڵ� ���� */
	}
}

/*
 desc : ���� �� ����� ��� ���� ���
 parm : None
 retn : None
*/
VOID CDrawDev::DrawDev()
{
	UINT8 u8Comm= 0x00, i = 0x00;

	for (; i<9; i++)
	{
		switch (i)
		{
		/* 01. Luria Service ���� ���� */
		case 0x00	:	u8Comm	= uvCmn_Luria_IsConnected() ? 0x01 : 0x00;
						u8Comm	+= uvEng_Luria_IsServiceInited() ? 0x01 : 0x00;		break;

		/* 02. MC2 ���� ���� */
		case 0x01	:	u8Comm	= uvCmn_MC2_IsConnected() ? 0x02 : 0x00;			break;
		/* 04. Aling Camera 1 ���� ���� */
		case 0x03	:	u8Comm	= uvCmn_Camera_IsConnectedACam(0x01) ? 0x02 : 0x00;	break;
		/* 05. Aling Camera 2 ���� ���� */
		case 0x04	:	u8Comm	= uvCmn_Camera_IsConnectedACam(0x02) ? 0x02 : 0x00;	break;
		/* 09. Vacuum ���� ���� */
		case 0x08	:	u8Comm	= uvCmn_Mvenc_IsConnected() ? 0x02 : 0x00;	break;
		}

		/* ���� �˻� ��ġ�� Hot Air, Vacuum Ȥ�� Chiller �� ���, PLC�� ���� �ȵǾ� �ִٸ�... */
		if ((0x05 == i || 0x06 == i || 0x07 == i) && !m_u8DevStat[2])	u8Comm	= 0x01;	/* Warning */
		/* ���� ��� ���¿� ���� ��� ���°� �ٸ���, ��� ���� ������ ���� */
		if (m_u8DevStat[i] != u8Comm)
		{
			/* �ֽ� ��� ���·� ���� */
			m_u8DevStat[i]	= u8Comm;
			/* ��� ���� �̹��� ���� */
			DrawDev(i, u8Comm);
		}
	}
}

/*
 desc : ��ü ����� ��� ���� ������ ����
 parm : None
 retn : None
*/
VOID CDrawDev::DrawDevAll()
{
	/* ���� �ֱ��� ��� ���� �� �ʱ�ȭ */
	memset(m_u8DevStat, 0xff, sizeof(UINT8) * 9);
	/* ��� ���� ���� */
	DrawDev();
}