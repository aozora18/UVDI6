/*
 desc : Luria Status
 */

#include "pch.h"
#include "../../MainApp.h"
#include "GridLuria.h"


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

/* --------------------------------------------------------------------------------------------- */
/*                                       Base Grid Control                                       */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : ������
 parm : parent	- [in]  �ڽ��� ȣ���� �θ� �ڵ�
		grid	- [in]  Grid Control�� ��µ� ���� �ڵ�
		g_id	- [in]  Grid Control ID
		count	- [in]  �� (Item)�� ����
 retn : None
*/
CGridLuria::CGridLuria(HWND parent, HWND grid, UINT32 g_id, UINT32 count)
{
	m_pGrid			= NULL;
	m_pParent		= CWnd::FromHandle(parent);
	m_pCtrl			= CWnd::FromHandle(grid);
	m_u32GridID		= g_id;

	m_bScrollHorz	= FALSE;
	m_bScrollVert	= FALSE;

	/* �׸��� ��Ʈ�� �⺻ ���� �Ӽ� */
	m_u32Style		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	m_stGV.mask		= GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	m_stGV.nFormat	= DT_LEFT | DT_VCENTER | DT_SINGLELINE;
	m_stGV.crFgClr	= RGB(32, 32, 32);
	m_stGV.crBkClr	= RGB(240, 240, 240);
	m_stGV.lfFont = g_lf[eFONT_LEVEL2_BOLD];

	/* Grid Control�� ���ʿ� ��µ� �׸��� ����� ���� ���� */
	m_pszItem		= NULL;
	m_u32Items		= 128;
	m_u32RowCnt		= count;
	if (count)
	{
		m_pszItem	= (PTCHAR *)::Alloc(sizeof(PTCHAR) * count);
		m_pszItem[0]= (PTCHAR)::Alloc(sizeof(TCHAR) * m_u32Items * count);
		for (UINT32 i=1; i<count; i++)	m_pszItem[i] = m_pszItem[i-1] + m_u32Items;
		wmemset(m_pszItem[0], 0x00, sizeof(TCHAR) * m_u32Items);
	}
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CGridLuria::~CGridLuria()
{
	if (m_pGrid)
	{
		if (m_pGrid->GetSafeHwnd())	m_pGrid->DestroyWindow();
		delete m_pGrid;
	}
	if (m_pszItem)
	{
		::Free(m_pszItem[0]);
		::Free(m_pszItem);
		m_pszItem	= NULL;
	}
}

/*
 desc : �ʱ�ȭ (����)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGridLuria::Init()
{
	/* ���� ���� Client ũ�� */
	m_pCtrl->GetWindowRect(m_rGrid);
	m_pParent->ScreenToClient(m_rGrid);

	CResizeUI clsResizeUI;
	/* Grid ũ�� �缳�� */
	m_rGrid.left	+= (long)(15 * clsResizeUI.GetRateX());
	m_rGrid.top		+= (long)(23 * clsResizeUI.GetRateY());
	m_rGrid.right	-= (long)(15 * clsResizeUI.GetRateX());
	m_rGrid.bottom	-= (long)(15 * clsResizeUI.GetRateY());

	/* ��ü ���� */
	m_pGrid = new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetDrawScrollBarVert(m_bScrollVert);
	m_pGrid->SetDrawScrollBarHorz(m_bScrollVert);
	if (!m_pGrid->Create(m_rGrid, m_pParent, m_u32GridID, m_u32Style))
	{
		delete m_pGrid;
		return FALSE;
	}

	/* ��ü �⺻ ���� */
	m_pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	m_pGrid->SetColumnCount(1);
	m_pGrid->SetBkColor(RGB(255, 255, 255));
	m_pGrid->SetFixedRowCount(0);
	m_pGrid->SetFixedColumnCount(0);
	/* ���� */
	m_pGrid->SetColumnWidth(0, m_rGrid.Width()-20);

	/* �⺻ �Ӽ� �� ���� */
	m_pGrid->SetBaseGridProp(0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->Refresh();

	/* �Ź� ������ ���� */
	Load();
	Update(0);

	return TRUE;
}

/* --------------------------------------------------------------------------------------------- */
/*                                         MC2 ���� ����                                         */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : MC2 ����
 parm : None
 retn : None
*/
VOID CGridLuriaMC2::Load()
{
	UINT32 i = 0;

	if (m_u32RowCnt)
	{
		// by sysandj : Grid Size ����
		INT32 i32Width[2] = { 95, 70 };
		int nWidth = 0;
		int nResizeWidth = 0;
		for (int i = 0; i < _countof(i32Width); i++)
		{
			nWidth += i32Width[i];
		}

		for (int i = 0; i < _countof(i32Width); i++)
		{
			i32Width[i] += (m_rGrid.Width() - nWidth) / _countof(i32Width);
			nResizeWidth += i32Width[i];
		}
		i32Width[0] += m_rGrid.Width() - nResizeWidth - 2;

		m_rGrid.bottom += (m_u32RowCnt * 25) - m_rGrid.Height();
		m_pGrid->MoveWindow(m_rGrid);
		// by sysandj : Grid Size ����

		/* Items ���� */
		swprintf_s(m_pszItem[0], m_u32Items, L"MC2_Comm");
		swprintf_s(m_pszItem[1], m_u32Items, L"Reply Status");
		swprintf_s(m_pszItem[2], m_u32Items, L"Report Error");
		/* ȭ�� ���� ��Ȱ��ȭ */
		m_pGrid->SetRedraw(FALSE);
		/* �׸� ������ŭ Row ���� */
		m_pGrid->SetRowCount(m_u32RowCnt);
		m_pGrid->SetColumnCount(2);

		// by sysandj : Grid Size ����
		//m_pGrid->SetColumnWidth(0, 95);
		//m_pGrid->SetColumnWidth(1, 70);
		m_pGrid->SetColumnWidth(0, i32Width[0]);
		m_pGrid->SetColumnWidth(1, i32Width[1]);
		// by sysandj : Grid Size ����

		/* ��ϵ� �׸� �ٽ� ���� */
		for (; i<m_u32RowCnt; i++)
		{
			m_pGrid->SetRowHeight(i, 25);
			m_pGrid->SetItemText(i, 0, m_pszItem[i]);
			m_pGrid->SetItemFgColour(i, 0, m_stGV.crFgClr);
			m_pGrid->SetItemBkColour(i, 0, m_stGV.crBkClr);

			// by sysandj
			m_pGrid->SetItemBkColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL);
			m_pGrid->SetItemFgColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
			// by sysandj
		}
		/* ȭ�� ���� Ȱ��ȭ */
		m_pGrid->SetRedraw(TRUE);
	}

	/* �׸��� ��Ʈ�� ���� */
	m_pGrid->Invalidate();
}

/*
 desc : ���� �� ����
 parm : index	- [in]  Fixed 0
 retn : ���õ� ������ ����� ����ü ������
*/
VOID CGridLuriaMC2::Update(UINT8 index)
{
	UINT32 i, u32Bit	= 0;
	TCHAR tzComm[2][16]	= { L"Connected", L"Failed" };
	TCHAR tzStat[2][16]	= { L"Normal", L"Abnormal" };
	TCHAR tzError[2][16]= { L"No error", L"Reported" };
	COLORREF clrVal[2]	= { RGB (0, 0, 0), RGB (255, 0, 0) };
	PTCHAR ptzValue		= NULL;
	LPG_LDSM pstVal		= uvEng_ShMem_GetLuria();

	/* ȭ�� ���� ��Ȱ��ȭ */
	m_pGrid->SetRedraw(FALSE);
#if 1
	for (i=0; i<3; i++)
	{
		u32Bit	= (pstVal->system.get_system_status[0] & (0x00000001 << i)) > i;
		switch (i)
		{
		case 0 : ptzValue = tzComm[u32Bit];	break;
		case 1 : ptzValue = tzStat[u32Bit];	break;
		case 2 : ptzValue = tzComm[u32Bit];	break;
		}
		m_pGrid->SetItemText(i, 1, ptzValue);
		m_pGrid->SetItemFgColour(i, 1, clrVal[u32Bit]);
	}
#else	
	/* MotorControllerCommsFailed */
	u32Bit	= pstVal->system.get_system_status[0] & 0x00000001;
	m_pGrid->SetItemText(0, 1, tzComn[u32Bit]);
	m_pGrid->SetItemFgColour(0, 1, clrVal[u32Bit]);
	/* MotorControllerUnexpectedReply */
	u32Bit	= pstVal->system.get_system_status[0] & 0x00000002;
	m_pGrid->SetItemText(1, 1, tzStat[u32Bit]);
	m_pGrid->SetItemFgColour(1, 1, clrVal[u32Bit]);
	/* MotorControllerReportsError */
	u32Bit	= pstVal->system.get_system_status[0] & 0x00000004;
	m_pGrid->SetItemText(2, 1, tzError[u32Bit]);
	m_pGrid->SetItemFgColour(2, 1, clrVal[u32Bit]);
#endif
	/* ȭ�� ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate();
}

/* --------------------------------------------------------------------------------------------- */
/*                                         PC ���� ����                                          */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Luria PC ����
 parm : None
 retn : None
*/
VOID CGridLuriaPC::Load()
{
	UINT32 i = 0;

	if (m_u32RowCnt)
	{
		// by sysandj : Grid Size ����
		INT32 i32Width[2] = { 110, 55 };
		int nWidth = 0;
		int nResizeWidth = 0;
		for (int i = 0; i < _countof(i32Width); i++)
		{
			nWidth += i32Width[i];
		}

		for (int i = 0; i < _countof(i32Width); i++)
		{
			i32Width[i] += (m_rGrid.Width() - nWidth) / _countof(i32Width);
			nResizeWidth += i32Width[i];
		}
		i32Width[0] += m_rGrid.Width() - nResizeWidth - 2;

		m_rGrid.bottom += (m_u32RowCnt * 25) - m_rGrid.Height();
		m_pGrid->MoveWindow(m_rGrid);
		// by sysandj : Grid Size ����

		/* Items ���� */
		swprintf_s(m_pszItem[0], m_u32Items, L"License Dongle");
		swprintf_s(m_pszItem[1], m_u32Items, L"Disk Failure");
		swprintf_s(m_pszItem[2], m_u32Items, L"Disk Full");

		/* ȭ�� ���� ��Ȱ��ȭ */
		m_pGrid->SetRedraw(FALSE);
		/* �׸� ������ŭ Row ���� */
		m_pGrid->SetRowCount(m_u32RowCnt);
		m_pGrid->SetColumnCount(2);

		// by sysandj : Grid Size ����
		//m_pGrid->SetColumnWidth(0, 110);
		//m_pGrid->SetColumnWidth(1, 55);
		m_pGrid->SetColumnWidth(0, i32Width[0]);
		m_pGrid->SetColumnWidth(1, i32Width[1]);
		// by sysandj : Grid Size ����
				
		/* ��ϵ� �׸� �ٽ� ���� */
		for (; i<m_u32RowCnt; i++)
		{
			m_pGrid->SetRowHeight(i, 25);
			m_pGrid->SetItemText(i, 0, m_pszItem[i]);
			m_pGrid->SetItemFgColour(i, 0, m_stGV.crFgClr);
			m_pGrid->SetItemBkColour(i, 0, m_stGV.crBkClr);

			// by sysandj
			m_pGrid->SetItemBkColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL);
			m_pGrid->SetItemFgColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
			// by sysandj
		}
		/* ȭ�� ���� Ȱ��ȭ */
		m_pGrid->SetRedraw(TRUE);
	}

	/* �׸��� ��Ʈ�� ���� */
	m_pGrid->Invalidate();
}

/*
 desc : ���� �� ����
 parm : index	- [in]  Fixed 0
 retn : ���õ� ������ ����� ����ü ������
*/
VOID CGridLuriaPC::Update(UINT8 index)
{
	UINT32 i, u32Bit	= 0;
	TCHAR tzYesNo[2][8]	= { L"No", L"Yes" };
	COLORREF clrVal[2]	= { RGB (0, 0, 0), RGB (255, 0, 0) };
	LPG_LDSM pstVal		= uvEng_ShMem_GetLuria();

	/* ȭ�� ���� ��Ȱ��ȭ */
	m_pGrid->SetRedraw(FALSE);
#if 1
	for (i=0; i<3; i++)
	{
		u32Bit	= pstVal->system.get_system_status[2] & (0x00000001 << i);
		if (u32Bit > 1)	u32Bit	= 0;	/* There is a high probability that it is trash due to communication problems. */
		if (0 == i)
		{
			m_pGrid->SetItemText(i, 1, tzYesNo[!u32Bit]);
			m_pGrid->SetItemFgColour(i, 1, clrVal[u32Bit]);
		}
		else
		{
			m_pGrid->SetItemText(i, 1, tzYesNo[u32Bit]);
			m_pGrid->SetItemFgColour(i, 1, clrVal[u32Bit]);
		}
	}
#else
	/* LicenseDongleMissing */
	u32Bit	= pstVal->system.get_system_status[0] & 0x00000001;
	m_pGrid->SetItemText(0, 1, tzYesNo[u32Bit]);
	m_pGrid->SetItemFgColour(0, 1, clrVal[u32Bit]);
	/* DiskFailure */
	u32Bit	= pstVal->system.get_system_status[0] & 0x00000002;
	m_pGrid->SetItemText(1, 1, tzYesNo[u32Bit]);
	m_pGrid->SetItemFgColour(1, 1, clrVal[u32Bit]);
	/* DiskFull */
	u32Bit	= pstVal->system.get_system_status[0] & 0x00000004;
	m_pGrid->SetItemText(2, 1, tzYesNo[u32Bit]);
	m_pGrid->SetItemFgColour(2, 1, clrVal[u32Bit]);
#endif
	/* ȭ�� ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate();
}

/* --------------------------------------------------------------------------------------------- */
/*                                    OverallErrorStatusMulti                                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : MC2 ����
 parm : None
 retn : None
*/
VOID CGridLuriaOverall::Load()
{
	UINT32 i = 0;

	if (m_u32RowCnt)
	{
		/* Items ���� */
		swprintf_s(m_pszItem[0], m_u32Items, L"Temp regulator 1 out of range");
		swprintf_s(m_pszItem[1], m_u32Items, L"Temp regulator 2 out of range");
		swprintf_s(m_pszItem[2], m_u32Items, L"Low ethernet link quality");
		swprintf_s(m_pszItem[3], m_u32Items, L"Board (FPGA) Temp. too high");
		swprintf_s(m_pszItem[4], m_u32Items, L"Board fan stopped");
		swprintf_s(m_pszItem[5], m_u32Items, L"AF sensor position error");
		swprintf_s(m_pszItem[6], m_u32Items, L"Laser light-source speckle wheel error");
		swprintf_s(m_pszItem[7], m_u32Items, L"DMD temperature too high");
		swprintf_s(m_pszItem[8], m_u32Items, L"Humidity sensor reports too high humidity");

		// by sysandj : Grid Size ����
		INT32 i32Width[2] = { 317, 60 };
		int nWidth = 0;
		int nResizeWidth = 0;
		for (int i = 0; i < _countof(i32Width); i++)
		{
			nWidth += i32Width[i];
		}

		for (int i = 0; i < _countof(i32Width); i++)
		{
			i32Width[i] += (m_rGrid.Width() - nWidth) / _countof(i32Width);
			nResizeWidth += i32Width[i];
		}
		i32Width[0] += m_rGrid.Width() - nResizeWidth - 2;

		m_rGrid.bottom += (m_u32RowCnt * 25) - m_rGrid.Height();
		m_pGrid->MoveWindow(m_rGrid);
		// by sysandj : Grid Size ����

		/* ȭ�� ���� ��Ȱ��ȭ */
		m_pGrid->SetRedraw(FALSE);
		/* �׸� ������ŭ Row ���� */
		m_pGrid->SetRowCount(m_u32RowCnt);
		m_pGrid->SetColumnCount(2);
		m_pGrid->SetColumnWidth(0, i32Width[0]);
		m_pGrid->SetColumnWidth(1, i32Width[1]);
		/* ��ϵ� �׸� �ٽ� ���� */
		for (; i<m_u32RowCnt; i++)
		{
			m_pGrid->SetRowHeight(i, 25);
			m_pGrid->SetItemText(i, 0, m_pszItem[i]);
			m_pGrid->SetItemFgColour(i, 0, m_stGV.crFgClr);
			m_pGrid->SetItemBkColour(i, 0, m_stGV.crBkClr);

			// by sysandj
			m_pGrid->SetItemBkColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL);
			m_pGrid->SetItemFgColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
			// by sysandj
		}
		/* ȭ�� ���� Ȱ��ȭ */
		m_pGrid->SetRedraw(TRUE);
	}

	/* �׸��� ��Ʈ�� ���� */
	m_pGrid->Invalidate();
}

/*
 desc : ���� �� ����
 parm : index	- [in]  Photohead Index (Zero-based)
 retn : ���õ� ������ ����� ����ü ������
*/
VOID CGridLuriaOverall::Update(UINT8 index)
{
	UINT32 i, u32Bit	= 0;
	TCHAR tzAlarm[2][8]	= { L"OK", L"Alarm" };
	COLORREF clrVal[2]	= { RGB (0, 0, 0), RGB (255, 0, 0) };
	LPG_LDSM pstVal		= uvEng_ShMem_GetLuria();

	/* ȭ�� ���� ��Ȱ��ȭ */
	m_pGrid->SetRedraw(FALSE);
#if 1
	for (i=0; i<9; i++)
	{
		u32Bit	= pstVal->system.get_overall_error_status_multi[index] & (0x00000001 << i);
		m_pGrid->SetItemText(i, 1, tzAlarm[u32Bit]);
		m_pGrid->SetItemFgColour(i, 1, clrVal[u32Bit]);
	}
#else
	/* Temp regulator 1 out of range */
	u32Bit	= pstVal->system.get_overall_error_status_multi[index] & 0x00000001;
	m_pGrid->SetItemText(0, 1, tzAlarm[u32Bit]);
	m_pGrid->SetItemFgColour(0, 1, clrVal[u32Bit]);
	/* Temp regulator 2 out of range */
	u32Bit	= pstVal->system.get_overall_error_status_multi[index] & 0x00000002;
	m_pGrid->SetItemText(1, 1, tzAlarm[u32Bit]);
	m_pGrid->SetItemFgColour(1, 1, clrVal[u32Bit]);
	/* Low ethernet link quality */
	u32Bit	= pstVal->system.get_overall_error_status_multi[index] & 0x00000004;
	m_pGrid->SetItemText(2, 1, tzAlarm[u32Bit]);
	m_pGrid->SetItemFgColour(2, 1, clrVal[u32Bit]);
	/* Board temp (FPGA temperature) too high */
	u32Bit	= pstVal->system.get_overall_error_status_multi[index] & 0x00000008;
	m_pGrid->SetItemText(3, 1, tzAlarm[u32Bit]);
	m_pGrid->SetItemFgColour(3, 1, clrVal[u32Bit]);
	/* Board fan stopped */
	u32Bit	= pstVal->system.get_overall_error_status_multi[index] & 0x00000010;
	m_pGrid->SetItemText(4, 1, tzAlarm[u32Bit]);
	m_pGrid->SetItemFgColour(4, 1, clrVal[u32Bit]);
	/* AF sensor position error */
	u32Bit	= pstVal->system.get_overall_error_status_multi[index] & 0x00000020;
	m_pGrid->SetItemText(5, 1, tzAlarm[u32Bit]);
	m_pGrid->SetItemFgColour(5, 1, clrVal[u32Bit]);
	/* Laser light-source speckle wheel error */
	u32Bit	= pstVal->system.get_overall_error_status_multi[index] & 0x00000040;
	m_pGrid->SetItemText(6, 1, tzAlarm[u32Bit]);
	m_pGrid->SetItemFgColour(6, 1, clrVal[u32Bit]);
	/* DMD temperature too high */
	u32Bit	= pstVal->system.get_overall_error_status_multi[index] & 0x00000080;
	m_pGrid->SetItemText(7, 1, tzAlarm[u32Bit]);
	m_pGrid->SetItemFgColour(7, 1, clrVal[u32Bit]);
	/* Humidity sensor reports too high humidity */
	u32Bit	= pstVal->system.get_overall_error_status_multi[index] & 0x00000100;
	m_pGrid->SetItemText(8, 1, tzAlarm[u32Bit]);
	m_pGrid->SetItemFgColour(8, 1, clrVal[u32Bit]);
#endif
	/* ȭ�� ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate();
}

/* --------------------------------------------------------------------------------------------- */
/*                                   Light Source Status Multi                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : MC2 ����
 parm : None
 retn : None
*/
VOID CGridLuriaLedSource::Load()
{
	UINT32 i = 0;

	if (m_u32RowCnt)
	{
		/* Items ���� */
		swprintf_s(m_pszItem[0], m_u32Items, L"Light source is connected");
		swprintf_s(m_pszItem[1], m_u32Items, L"Door open");
		swprintf_s(m_pszItem[2], m_u32Items, L"Light source driver board temperature out of range");
		swprintf_s(m_pszItem[3], m_u32Items, L"Light source temperature out of range");
		swprintf_s(m_pszItem[4], m_u32Items, L"Duty-cycle limitation");
		swprintf_s(m_pszItem[5], m_u32Items, L"Over-current protection (OCP)");
		swprintf_s(m_pszItem[6], m_u32Items, L"Over-power protection (OPP)");

		// by sysandj : Grid Size ����
		INT32 i32Width[2] = { 317, 60 };
		int nWidth = 0;
		int nResizeWidth = 0;
		for (int i = 0; i < _countof(i32Width); i++)
		{
			nWidth += i32Width[i];
		}

		for (int i = 0; i < _countof(i32Width); i++)
		{
			i32Width[i] += (m_rGrid.Width() - nWidth) / _countof(i32Width);
			nResizeWidth += i32Width[i];
		}
		i32Width[0] += m_rGrid.Width() - nResizeWidth - 2;

		m_rGrid.bottom += (m_u32RowCnt * 25) - m_rGrid.Height();
		m_pGrid->MoveWindow(m_rGrid);
		// by sysandj : Grid Size ����

		/* ȭ�� ���� ��Ȱ��ȭ */
		m_pGrid->SetRedraw(FALSE);
		/* �׸� ������ŭ Row ���� */
		m_pGrid->SetRowCount(m_u32RowCnt);
		m_pGrid->SetColumnCount(2);
		m_pGrid->SetColumnWidth(0, i32Width[0]);
		m_pGrid->SetColumnWidth(1, i32Width[1]);
		/* ��ϵ� �׸� �ٽ� ���� */
		for (; i<m_u32RowCnt; i++)
		{
			m_pGrid->SetRowHeight(i, 25);
			m_pGrid->SetItemText(i, 0, m_pszItem[i]);
			m_pGrid->SetItemFgColour(i, 0, m_stGV.crFgClr);
			m_pGrid->SetItemBkColour(i, 0, m_stGV.crBkClr);

			// by sysandj
			m_pGrid->SetItemBkColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL);
			m_pGrid->SetItemFgColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
			// by sysandj
		}
		/* ȭ�� ���� Ȱ��ȭ */
		m_pGrid->SetRedraw(TRUE);
	}

	/* �׸��� ��Ʈ�� ���� */
	m_pGrid->Invalidate();
}

/*
 desc : ���� �� ����
 parm : index	- [in]  Photohead Index (Zero-based) for LED
 retn : ���õ� ������ ����� ����ü ������
*/
VOID CGridLuriaLedSource::Update(UINT8 index)
{
	UINT8 i, u8Bit		= 0x00;
	TCHAR tzConn[2][8]	= { L"No", L"Yes" };
	TCHAR tzDoor[2][8]	= { L"Close", L"Open" };
	TCHAR tzOver[2][8]	= { L"Normal", L"Alarm" };
	TCHAR tzDuty[2][8]	= { L"Normal", L"Limited" };
	TCHAR tzOCP[2][8]	= { L"No OCP", L"OCP" };
	TCHAR tzOPP[2][8]	= { L"No OPP", L"OPP" };
	PTCHAR ptzValue		= NULL;
	COLORREF clrVal[2]	= { RGB (0, 0, 0), RGB (255, 0, 0) };
	LPG_LDSM pstVal		= uvEng_ShMem_GetLuria();

	/* ȭ�� ���� ��Ȱ��ȭ */
	m_pGrid->SetRedraw(FALSE);
#if 0	/* ���� �������� ���� �ȵ� */
	for (i=0; i<7; i++)
	{
		u8Bit	= 0x01;	/* Fixed */
		switch (i)
		{
		case 0 : ptzValue = tzConn[u8Bit];	u8Bit	= !u8Bit;	break;;
		case 1 : ptzValue = tzDoor[u8Bit];						break;
		case 2 : 		 
		case 3 : ptzValue = tzOver[u8Bit];						break;
		case 4 : ptzValue = tzDuty[u8Bit];						break;
		case 5 : ptzValue = tzOCP[u8Bit];						break;
		case 6 : ptzValue = tzOPP[u8Bit];						break;
		}
		m_pGrid->SetItemText(i, 1, ptzValue);
		m_pGrid->SetItemFgColour(i, 1, clrVal[u8Bit]);
	}
#else	/* ��� ȸ�� �� ���� �������� ó�� */
	for (i=0; i<7; i++)
	{
		switch (i)
		{
		case 0 : ptzValue = tzConn[1];	break;;
		case 1 : ptzValue = tzDoor[0];	break;
		case 2 : 		 
		case 3 : ptzValue = tzOver[0];	break;
		case 4 : ptzValue = tzDuty[0];	break;
		case 5 : ptzValue = tzOCP[0];	break;
		case 6 : ptzValue = tzOPP[0];	break;
		}
		m_pGrid->SetItemText(i, 1, ptzValue);
		m_pGrid->SetItemFgColour(i, 1, clrVal[0]);
	}
#endif
	/* ȭ�� ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate();
}

/* --------------------------------------------------------------------------------------------- */
/*                                 Photohead Init / Comm. Status                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : PH �ʱ�ȭ ���� / ��� ����
 parm : None
 retn : None
*/
VOID CGridLuriaPHStat::Load()
{
	UINT32 i = 0;

	if (m_u32RowCnt)
	{
		/* Items ���� */
		swprintf_s(m_pszItem[0], m_u32Items, L"PH Inited");
		swprintf_s(m_pszItem[1], m_u32Items, L"PH Comm.");
		swprintf_s(m_pszItem[2], m_u32Items, L"PH Motor");

		// by sysandj : Grid Size ����
		INT32 i32Width[2] = { 74, 45 };
		int nWidth = 0;
		int nResizeWidth = 0;
		for (int i = 0; i < _countof(i32Width); i++)
		{
			nWidth += i32Width[i];
		}

		for (int i = 0; i < _countof(i32Width); i++)
		{
			i32Width[i] += (m_rGrid.Width() - nWidth) / _countof(i32Width);
			nResizeWidth += i32Width[i];
		}
		i32Width[0] += m_rGrid.Width() - nResizeWidth - 2;

		m_rGrid.bottom += (m_u32RowCnt * 25) - m_rGrid.Height();
		m_pGrid->MoveWindow(m_rGrid);
		// by sysandj : Grid Size ����

		/* ȭ�� ���� ��Ȱ��ȭ */
		m_pGrid->SetRedraw(FALSE);
		/* �׸� ������ŭ Row ���� */
		m_pGrid->SetRowCount(m_u32RowCnt);
		m_pGrid->SetColumnCount(2);
		m_pGrid->SetColumnWidth(0, i32Width[0]);
		m_pGrid->SetColumnWidth(1, i32Width[1]);
		/* ��ϵ� �׸� �ٽ� ���� */
		for (i=0; i<m_u32RowCnt; i++)
		{
			m_pGrid->SetRowHeight(i, 25);
			m_pGrid->SetItemText(i, 0, m_pszItem[i]);
			m_pGrid->SetItemFgColour(i, 0, m_stGV.crFgClr);
			m_pGrid->SetItemBkColour(i, 0, m_stGV.crBkClr);

			// by sysandj
			m_pGrid->SetItemBkColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL);
			m_pGrid->SetItemFgColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
			// by sysandj
		}
		/* ȭ�� ���� Ȱ��ȭ */
		m_pGrid->SetRedraw(TRUE);
	}

	/* �׸��� ��Ʈ�� ���� */
	m_pGrid->Invalidate();
}

/*
 desc : ���� �� ����
 parm : index	- [in]  Photohead Index (Zero-based)
 retn : ���õ� ������ ����� ����ü ������
*/
VOID CGridLuriaPHStat::Update(UINT8 index)
{
	BOOL bSucc			= TRUE;
	UINT32 /*i, */u32Bit= 0;
	TCHAR tzYesNo[2][8]	= { L"Yes", L"No" };
	COLORREF clrVal[2]	= { RGB (0, 0, 0), RGB (255, 0, 0) };
	LPG_CIEA pstCfg		= uvEng_GetConfig();
	LPG_LDSM pstVal		= uvEng_ShMem_GetLuria();
	LPG_LDSM pstPH		= uvEng_ShMem_GetLuria();
	/* ȭ�� ���� ��Ȱ��ȭ */
	m_pGrid->SetRedraw(FALSE);
#if 1
	/* PH Inited ? */
#if 0
	for (i=0,u32Bit=0; i<pstCfg->luria_svc.ph_count; i++)
	{
		u32Bit	+= pstVal->system.get_system_status[1] & (0x00000001 << i) ? 0x01 : 0x00;
	}
	u32Bit	= u32Bit > 0 ? 1 : 0;
#else
	u32Bit	= (pstVal->system.get_system_status[1] & 0x000000ff) > 0 ? 0x01 : 0x00;
#endif
	m_pGrid->SetItemText(0, 1, tzYesNo[u32Bit]);
	m_pGrid->SetItemFgColour(0, 1, clrVal[u32Bit]);
	/* PH Comm ? */
#if 0
	for (i=0,u32Bit=0; i<pstCfg->luria_svc.ph_count; i++)
	{
		u32Bit	= (pstVal->system.get_system_status[1] & (0x00000001 << (i+8))) > 0 ? 0x01 : 0x00;
		if (u32Bit)	break;
	}
	u32Bit	= u32Bit > 0 ? 1 : 0;
#else
	u32Bit	= (pstVal->system.get_system_status[1] & 0x0000ff00) > 0 ? 0x01 : 0x00;
#endif
	m_pGrid->SetItemText(1, 1, tzYesNo[u32Bit]);
	m_pGrid->SetItemFgColour(1, 1, clrVal[u32Bit]);
	/* PH Motor */
#if 0
	u32Bit	= pstPH->directph.IsAllMotorMiddleInited(pstCfg->luria_svc.ph_count) ? 0x00 : 0x01;
#else
	u32Bit	= pstVal->system.get_system_status[0] > 0 ? 0x01 : 0x00;
#endif
	m_pGrid->SetItemText(2, 1, tzYesNo[u32Bit]);
	m_pGrid->SetItemFgColour(2, 1, clrVal[u32Bit]);
#endif
	/* ȭ�� ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate();
}

/* --------------------------------------------------------------------------------------------- */
/*                                Job Load / Mark Regist / Expose                                */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : Gerber Loaded / Mark Registered / Expose ����
 parm : None
 retn : None
*/
VOID CGridLuriaJob::Load()
{
	UINT32 i = 0;

	if (m_u32RowCnt)
	{
		/* Items ���� */
		swprintf_s(m_pszItem[0], m_u32Items, L"Load");
		swprintf_s(m_pszItem[1], m_u32Items, L"Regist");
		swprintf_s(m_pszItem[2], m_u32Items, L"Expose");

		// by sysandj : Grid Size ����
		INT32 i32Width[2] = { 49, 160 };
		int nWidth = 0;
		int nResizeWidth = 0;
		for (int i = 0; i < _countof(i32Width); i++)
		{
			nWidth += i32Width[i];
		}

		for (int i = 0; i < _countof(i32Width); i++)
		{
			i32Width[i] += (m_rGrid.Width() - nWidth) / _countof(i32Width);
			nResizeWidth += i32Width[i];
		}
		i32Width[0] += m_rGrid.Width() - nResizeWidth - 2;

		m_rGrid.bottom += (m_u32RowCnt * 25) - m_rGrid.Height();
		m_pGrid->MoveWindow(m_rGrid);
		// by sysandj : Grid Size ����

		/* ȭ�� ���� ��Ȱ��ȭ */
		m_pGrid->SetRedraw(FALSE);
		/* �׸� ������ŭ Row ���� */
		m_pGrid->SetRowCount(m_u32RowCnt);
		m_pGrid->SetColumnCount(2);
		m_pGrid->SetColumnWidth(0, i32Width[0]);
		m_pGrid->SetColumnWidth(1, i32Width[1]);
		/* ��ϵ� �׸� �ٽ� ���� */
		for (i=0; i<m_u32RowCnt; i++)
		{
			m_pGrid->SetRowHeight(i, 25);
			m_pGrid->SetItemText(i, 0, m_pszItem[i]);
			m_pGrid->SetItemFgColour(i, 0, m_stGV.crFgClr);
			m_pGrid->SetItemBkColour(i, 0, m_stGV.crBkClr);

			// by sysandj
			m_pGrid->SetItemBkColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL);
			m_pGrid->SetItemFgColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
			// by sysandj
		}
		/* ȭ�� ���� Ȱ��ȭ */
		m_pGrid->SetRedraw(TRUE);
	}

	/* �׸��� ��Ʈ�� ���� */
	m_pGrid->Invalidate();
}

/*
 desc : ���� �� ����
 parm : index	- [in]  Fixed 0.
 retn : ���õ� ������ ����� ����ü ������
*/
VOID CGridLuriaJob::Update(UINT8 index)
{
	UINT8 u8Index		= 0;
	TCHAR tzLoad[4][24]	= { L"Load not started", L"Loading", L"Load complete", L"Load failed" };
	COLORREF clrVal[2]	= { RGB (255, 0, 0), RGB (0, 0, 0) };
	LPG_LDSM pstVal		= uvEng_ShMem_GetLuria();

	/* ȭ�� ���� ��Ȱ��ȭ */
	m_pGrid->SetRedraw(FALSE);
	/* Job Loaded */
	u8Index	= pstVal->jobmgt.selected_job_load_state;
	if (u8Index == 0x0f)		u8Index = 0x04;
	else if (0x00 == u8Index)	u8Index = 0x01;
	m_pGrid->SetItemText(0, 1, tzLoad[u8Index-1]);
	u8Index	= (u8Index == 0x04) ? 0x00 : 0x01;
	m_pGrid->SetItemFgColour(0, 1, clrVal[u8Index]);
#if 0
	/* Mark Registered */
	switch (pstVal->panel.get_registration_status)
	{
	case 1	: wcscpy_s(m_pszItem[0], m_u32Items, L"Ok");											break;
	case 2	: wcscpy_s(m_pszItem[0], m_u32Items, L"MissingFiducials");								break;
	case 3	: wcscpy_s(m_pszItem[0], m_u32Items, L"MissingRegistration");							break;
	case 4	: wcscpy_s(m_pszItem[0], m_u32Items, L"GenerateNotRun");								break;
	case 5	: wcscpy_s(m_pszItem[0], m_u32Items, L"StripHeightNotSet");								break;
	case 6	: wcscpy_s(m_pszItem[0], m_u32Items, L"StripWidthNotSet");								break;
	case 7	: wcscpy_s(m_pszItem[0], m_u32Items, L"NumberOfStripesNotSet");							break;
	case 8	: wcscpy_s(m_pszItem[0], m_u32Items, L"OverlapNotSet");									break;
	case 9	: wcscpy_s(m_pszItem[0], m_u32Items, L"ScalingPointDistanceNotSet");					break;
	case 10	: wcscpy_s(m_pszItem[0], m_u32Items, L"CornerShiftsOutsideLimits");						break;
	case 11	: wcscpy_s(m_pszItem[0], m_u32Items, L"NumberOfRegAndFidNotEqual");						break;
	case 12	: wcscpy_s(m_pszItem[0], m_u32Items, L"UnableToInterpretFiducials");					break;
	case 13	: wcscpy_s(m_pszItem[0], m_u32Items, L"UnableToInterpretRegistrations");				break;
	case 14	: wcscpy_s(m_pszItem[0], m_u32Items, L"InvalidNumberOfScalingPoints");					break;
	case 15	: wcscpy_s(m_pszItem[0], m_u32Items, L"NumberOfStripesMatrixSizeMismatch");				break;
	case 16	: wcscpy_s(m_pszItem[0], m_u32Items, L"MissingOffsets");								break;
	case 17	: wcscpy_s(m_pszItem[0], m_u32Items, L"MissingStripFlipList");							break;
	case 18	: wcscpy_s(m_pszItem[0], m_u32Items, L"MissingXBaseStepSize");							break;
	case 19	: wcscpy_s(m_pszItem[0], m_u32Items, L"RotationOutsideLimits");							break;
	case 20	: wcscpy_s(m_pszItem[0], m_u32Items, L"ScalingOutsideLimits");							break;
	case 21	: wcscpy_s(m_pszItem[0], m_u32Items, L"OffsetOutsideLimits");							break;
	case 22	: wcscpy_s(m_pszItem[0], m_u32Items, L"NumberOfFixedParamIncorrect");					break;
	case 23	: wcscpy_s(m_pszItem[0], m_u32Items, L"CalcRotationOutOfLimits");						break;
	case 24	: wcscpy_s(m_pszItem[0], m_u32Items, L"CalcScaleOutOfLimits");							break;
	case 25	: wcscpy_s(m_pszItem[0], m_u32Items, L"CalcOffsetOutOfLimits");							break;
	case 26	: wcscpy_s(m_pszItem[0], m_u32Items, L"FixedRotationParamOutOfLimits");					break;
	case 27	: wcscpy_s(m_pszItem[0], m_u32Items, L"FixedScaleParamOutOfLimits");					break;
	case 28	: wcscpy_s(m_pszItem[0], m_u32Items, L"FixedOffsetParamOutOfLimits");					break;
	case 29	: wcscpy_s(m_pszItem[0], m_u32Items, L"WarpOutsideLimitsInZone");						break;
	case 30	: wcscpy_s(m_pszItem[0], m_u32Items, L"WarpOutsideLimitsOutsideZone");					break;
	case 31	: wcscpy_s(m_pszItem[0], m_u32Items, L"WarpUnableToCalculateWarpZone");					break;
	case 32	: wcscpy_s(m_pszItem[0], m_u32Items, L"WarpUndefinedLocalZonesFixedParamNotAllowed");	break;
	case 33	: wcscpy_s(m_pszItem[0], m_u32Items, L"WarpFixedParamNotAllowedWithSharedZones");		break;
	case 34	: wcscpy_s(m_pszItem[0], m_u32Items, L"WarpOutisdeFpgaLimits");							break;
	case 99	: wcscpy_s(m_pszItem[0], m_u32Items, L"InternalError");									break;
	default	: wcscpy_s(m_pszItem[0], m_u32Items, L"None");											break;
	}
#else
	uvCmn_Luria_GetRegistrationStatusExt(pstVal->panel.get_registration_status, m_pszItem[0], m_u32Items);
#endif
	m_pGrid->SetItemText(1, 1, m_pszItem[0]);
	u8Index	= (pstVal->panel.get_registration_status != 0x01 ||
			   pstVal->panel.get_registration_status != 0x00) ? 0x00 : 0x01;
	m_pGrid->SetItemFgColour(1, 1, clrVal[u8Index]);
	/* Expose */
	switch (pstVal->exposure.get_exposure_state[0])
	{
	case 0x00 :
	case 0x01 : wcscpy_s(m_pszItem[1], m_u32Items, L"Idle");					break;
	case 0x11 : wcscpy_s(m_pszItem[1], m_u32Items, L"Preprint running");		break;
	case 0x12 : wcscpy_s(m_pszItem[1], m_u32Items, L"Preprint success");		break;
	case 0x1F : wcscpy_s(m_pszItem[1], m_u32Items, L"Preprint failed");			break;
	case 0x21 : wcscpy_s(m_pszItem[1], m_u32Items, L"Print running");			break;
	case 0x22 : wcscpy_s(m_pszItem[1], m_u32Items, L"Print success");			break;
	case 0x2F : wcscpy_s(m_pszItem[1], m_u32Items, L"Print failed");			break;
	case 0x31 : wcscpy_s(m_pszItem[1], m_u32Items, L"PrintSimulation running");	break;
	case 0x32 : wcscpy_s(m_pszItem[1], m_u32Items, L"PrintSimulation success");	break;
	case 0x3F : wcscpy_s(m_pszItem[1], m_u32Items, L"PrintSimulation failed");	break;
	case 0xA1 : wcscpy_s(m_pszItem[1], m_u32Items, L"Abort in progress");		break;
	case 0xA2 : wcscpy_s(m_pszItem[1], m_u32Items, L"Abort success");			break;
	case 0xAF : wcscpy_s(m_pszItem[1], m_u32Items, L"Abort failed");			break;
	}
	m_pGrid->SetItemText(2, 1, m_pszItem[1]);
	u8Index	= ((pstVal->exposure.get_exposure_state[0] & 0x0f) == 0x0f) ? 0x00 : 0x01;
	m_pGrid->SetItemFgColour(2, 1, clrVal[u8Index]);

	/* ȭ�� ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate();
}

/* --------------------------------------------------------------------------------------------- */
/*                  PH Information (LED & Board Temp, LED Life Time, LED ON/OFF                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : �ʱ�ȭ (����)
 parm : None
 retn : TRUE or FALSE
*/
BOOL CGridLuriaPHInfo::Init()
{
	TCHAR tzCol[5][16]= { L"Item", L"365 (W)", L"380 (W)", L"395 (W)", L"405 (W)" };
	TCHAR tzRow[4][16]= { L"LED", L"BOARD", L"LIFE", L"LIGHT" };
	INT32 i, j, iColCnt = 5, iRowCnt = 5;
	INT32 i32Width[5]	= { 55, 56, 56, 56, 56 };
	UINT32 dwFormat[4]	= { DT_VCENTER | DT_SINGLELINE | DT_RIGHT,
							DT_VCENTER | DT_SINGLELINE | DT_RIGHT,
							DT_VCENTER | DT_SINGLELINE | DT_CENTER,
							DT_VCENTER | DT_SINGLELINE | DT_CENTER };

	// ���� ���� Client ũ��
	/* ���� ���� Client ũ�� */
	m_pCtrl->GetWindowRect(m_rGrid);
	m_pParent->ScreenToClient(m_rGrid);

	CResizeUI clsResizeUI;
	/* Grid ũ�� �缳�� */
	m_rGrid.left	+= (long)(15 * clsResizeUI.GetRateX());
	m_rGrid.top		+= (long)(23 * clsResizeUI.GetRateY());
	m_rGrid.right	-= (long)(15 * clsResizeUI.GetRateX());
	m_rGrid.bottom	-= (long)(15 * clsResizeUI.GetRateY());

	// �׸��� ��Ʈ�� �⺻ ���� �Ӽ�
	m_stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	m_stGV.crFgClr = RGB(32, 32, 32);
	/* ��ü ���� */
	m_pGrid = new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetDrawScrollBarVert(m_bScrollHorz);
	m_pGrid->SetDrawScrollBarHorz(m_bScrollHorz);
	if (!m_pGrid->Create(m_rGrid, m_pParent, m_u32GridID, m_u32Style))
	{
		delete m_pGrid;
		m_pGrid = NULL;
		return FALSE;
	}

	// by sysandj : Grid Size ����
	int nWidth = 0;
	int nResizeWidth = 0;
	for (int i = 0; i < _countof(i32Width); i++)
	{
		nWidth += i32Width[i];
	}

	for (int i = 0; i < _countof(i32Width); i++)
	{
		i32Width[i] += (m_rGrid.Width() - nWidth) / _countof(i32Width);
		nResizeWidth += i32Width[i];
	}
	i32Width[0] += m_rGrid.Width() - nResizeWidth - 2;

	m_rGrid.bottom += (m_u32RowCnt * 25) - m_rGrid.Height();
	m_pGrid->MoveWindow(m_rGrid);
	// by sysandj : Grid Size ����

	/* ��ü �⺻ ���� */
	m_pGrid->SetFont(&g_font[eFONT_LEVEL3_BOLD]);
	m_pGrid->SetRowCount(iRowCnt);
	m_pGrid->SetColumnCount(iColCnt);
	//m_pGrid->SetFixedRowCount(1);
	//m_pGrid->SetFixedColumnCount(1);
	m_pGrid->SetRowHeight(0, 25);
	m_pGrid->SetFixedColumnCount(0);
	m_pGrid->SetBkColor(RGB(255, 255, 255));
	m_pGrid->SetFixedColumnSelection(0);

	/* Ÿ��Ʋ (���� ��) */
	m_stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	m_stGV.crBkClr = RGB(214, 214, 214);
	m_stGV.crFgClr = RGB(0, 0, 0);
	for (i=0; i<iColCnt; i++)
	{
		m_stGV.row = 0;
		m_stGV.col = i;
		m_stGV.strText = tzCol[i];
		m_pGrid->SetItem(&m_stGV);
		m_pGrid->SetColumnWidth(i, i32Width[i]);

		// by sysandj
		m_pGrid->SetItemBkColour(0, i, DEF_COLOR_BTN_MENU_NORMAL);
		m_pGrid->SetItemFgColour(0, i, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		// by sysandj
		
//		m_pGrid->SetItemFont(0, i, &g_lf);
	}
	/* Ÿ��Ʋ (���� ��) */
	m_stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	for (i=1; i<iRowCnt; i++)
	{
		m_pGrid->SetRowHeight(i, 25);
		m_stGV.row	= i;
		m_stGV.col	= 0;
		m_stGV.strText.Format(L"%s", tzRow[i-1]);
		m_pGrid->SetItem(&m_stGV);

		// by sysandj
		m_pGrid->SetItemBkColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL);
		m_pGrid->SetItemFgColour(i, 0, DEF_COLOR_BTN_MENU_NORMAL_TEXT);
		// by sysandj
		
//		m_pGrid->SetItemFont(i, 0, &g_lf);
	}
	m_stGV.crBkClr = RGB(255, 255, 255);
	for (i=1; i<iRowCnt; i++)
	{
		for (j=1; j<iColCnt; j++)
		{
			m_stGV.nFormat = dwFormat[i-1];
			m_stGV.row	= i;
			m_stGV.col	= j;
			m_pGrid->SetItem(&m_stGV);
//			m_pGrid->SetItemFont(i, j, &g_lf);
		}
	}

	m_pGrid->SetBaseGridProp(0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->Refresh();

	/* �ٷ� �� ���� */
	Update();

	return TRUE;
}

/*
 desc : ���� �� ����
 parm : index	- [in]  Fixed 0.
 retn : ���õ� ������ ����� ����ü ������
*/
VOID CGridLuriaPHInfo::Update(UINT8 index)
{
	INT32 i = 0, j		= 0;
	UINT32 u32Time		= 0;
	TCHAR tzValue[32]	= { NULL }, tzOnOff[2][8] = { L"OFF", L"ON" };
	COLORREF clrVal[2]	= { RGB (255, 0, 0), RGB (0, 0, 0) };
	LPG_LDSM pstVal		= uvEng_ShMem_GetLuria();

	/* ȭ�� ���� ��Ȱ��ȭ */
	m_pGrid->SetRedraw(FALSE);

	/* column for led (265, 385, 390, 405) */
	for (i=0; i<4; i++)
	{
		/* LED Temperature */
		swprintf_s(tzValue, 32, L"%.1f ��c ", pstVal->directph.light_source_driver_temp_led[index][i] / 10.0f);
		m_pGrid->SetItemText(1, i+1, tzValue);
		/* Board Temperature */
		swprintf_s(tzValue, 32, L"%.1f ��c ", pstVal->directph.light_source_driver_temp_board[index][i] / 2.0f);
		m_pGrid->SetItemText(2, i+1, tzValue);
		/* LED Life Time */
		u32Time	= pstVal->directph.light_source_on_time_counter[index][i];
		swprintf_s(tzValue, 32, L"%4u:%02u", uvCmn_GetTimeToType(u32Time, 0x00),
											 uvCmn_GetTimeToType(u32Time, 0x01));
		m_pGrid->SetItemText(3, i+1, tzValue);
		/* LED Power (On or Off) */
		m_pGrid->SetItemText(4, i+1, tzOnOff[pstVal->directph.light_source_driver_light_onoff[index][i]]);
	}

	/* ȭ�� ���� Ȱ��ȭ */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate();
}