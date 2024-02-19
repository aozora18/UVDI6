
/*
 desc : Monitoring - Monitoring
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMoni.h"


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
CDlgMoni::CDlgMoni(UINT32 id, CWnd *parent)
	: CDlgChild(id, parent)
{
	m_u64TickHDD	= 0;
	m_u64TickMEM	= 0;
	m_u64TickCPU	= 0;
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CDlgMoni::~CDlgMoni()
{
}

/*
 desc : ���� ID ����
 parm : dx	- ���� ��ü ID
 retn : None
*/
VOID CDlgMoni::DoDataExchange(CDataExchange* dx)
{
	CDlgChild::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Group box */
	u32StartID	= IDC_MONI_GRP_SYSTEM;
	for (i=0; i<5; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Button */
	u32StartID	= IDC_MONI_BTN_DEVICE_RESET;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Static */
	u32StartID	= IDC_MONI_TXT_SYS_DRV;
	for (i=0; i<13; i++)	DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Edit box */
	u32StartID	= IDC_MONI_EDT_SYS_DRV;
	for (i=0; i<13; i++)	DDX_Control(dx, u32StartID+i,	m_edt_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgMoni, CDlgChild)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_MONI_BTN_DEVICE_RESET, IDC_MONI_BTN_DEVICE_RESET, OnBtnClicked)
END_MESSAGE_MAP()

/*
 desc : ��� �޽��� ����ä�� ������
 parm : msg	- �޽��� ������ ����� ����ü ������
 retn : ���� �θ� �޽��� �Լ� ȣ��... Ȥ�� 1 or 0
*/
BOOL CDlgMoni::PreTranslateMessage(MSG* msg)
{

	return CDlgChild::PreTranslateMessage(msg);
}

/*
 desc : �ʱ� ����ÿ� �� �� ȣ���
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMoni::OnInitDlg()
{
	if (!CDlgChild::OnInitDlg())	return FALSE;

	/* CPU ���� ��� : �ʱ�ȭ */
	if (!m_csCPU.Init())
	{
		AfxMessageBox(L"Failed to initialize the CPU Info.", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

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
VOID CDlgMoni::OnExitDlg()
{
}

/*
 desc : ���� ���ŵ� ������ ȣ���
 parm : dc	- ���� DC
 retn : None
*/
VOID CDlgMoni::OnPaintDlg(CDC *dc)
{
}

/*
 desc : ���̾�α� ũ�Ⱑ ����� ���... ���� ��Ʈ�� ũ�� ����
 parm : None
 retn : None
*/
VOID CDlgMoni::OnResizeDlg()
{
}

/*
 desc : ���� ��Ʈ�� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgMoni::InitCtrl()
{
	INT32 i;

	/* Groupbox */
	for (i=0; i<5; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* text - normal */
	for (i=0; i<13; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	/* Edit box */
	for (i=0; i<13; i++)
	{
		m_edt_ctl[i].SetEditFont(&g_lf);
		m_edt_ctl[i].SetBackColor(RGB(0xff, 0xff, 0xff));
		m_edt_ctl[i].SetTextColor(RGB(0x00, 0x00, 0x00));
		m_edt_ctl[i].SetReadOnly(TRUE);
		m_edt_ctl[i].SetMouseClick(FALSE);
	}
	/* Button - normal */
	for (i=0; i<1; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
}

/*
 desc : ���� ���� ��Ʈ�� ���� ����
 parm : None
 retn : None
*/
VOID CDlgMoni::UpdateCtrl()
{
	/* Update Log Data */
	UpdateMoni();
}

/*
 desc : PLC Memory Data (Value) ����
 parm : None
 retn : None
*/
VOID CDlgMoni::UpdateMoni()
{
	TCHAR tzVal[64]		= {NULL};
	UINT16 u16ErrCode	= 0x0000;
	UINT32 u32Rand		= 0;
	UINT64 u64Tick		= GetTickCount64();
	DOUBLE dbRateCPU	= 100.0f;	/* ������ 100.0f�� ���� */
	LPG_CSDM pstCfgMoni	= &uvEng_GetConfig()->monitor;
	LPG_CMSI pstCfgMC2	= &uvEng_GetConfig()->mc2_svc;
	LPG_MDSM pstShMemMC2= uvEng_ShMem_GetMC2();

	/* Monitoring ���� �ð� ��� */
	m_edt_ctl[6].SetTextToStr(uvEng_GetConfig()->monitor.mon_time, TRUE);
	/* ���� & ��� ���� */
	m_edt_ctl[7].SetTextToNum(uvCmn_Logs_GetErrorWarnCount(0x00), TRUE);
	m_edt_ctl[8].SetTextToNum(uvCmn_Logs_GetErrorWarnCount(0x01), TRUE);

	/* CPU (1 ~ 2 �� ���� ���� ����) */
	u32Rand	= (UINT32)uvCmn_GetRandNumer(0x00, 2);
	if (m_u64TickCPU + (1000 + u32Rand) < u64Tick)
	{
		m_u64TickCPU	= u64Tick;

		/* CPU & Process ������ ��� */
		if (m_csCPU.GetUsageRate(TRUE))
		{
			/* �Ʒ� �ӽ� ������ ������ ������ ���� HDD Drive �����ϱ� ���� */
			dbRateCPU	= m_csCPU.GetRateCPU();
 			swprintf_s(tzVal, 64, L"%.2f %%", dbRateCPU);
 			m_edt_ctl[2].SetTextToStr(tzVal, TRUE);
 			swprintf_s(tzVal, 64, L"%.2f %%", m_csCPU.GetRateProc());
 			m_edt_ctl[3].SetTextToStr(tzVal, TRUE);
		}
	}

	/* Memory (10 ~ 20 �� ���� ����) */
	u32Rand	= (UINT32)uvCmn_GetRandNumer(0x00, 3);
	if (m_u64TickMEM + (10000 + u32Rand) < u64Tick)
	{
		m_u64TickMEM	= u64Tick;

		UINT64 u64Mem	= uvCmn_GetProcessMemoryUsage();
		MEMORYSTATUSEX stMem	= {NULL};
		if (uvCmn_GetMemStatus(stMem))
		{
			swprintf_s(tzVal, 64, L"%u %%", stMem.dwMemoryLoad);
			m_edt_ctl[4].SetTextToStr(tzVal, TRUE);
			swprintf_s(tzVal, 64, L"%.2f %%", DOUBLE(u64Mem) / stMem.ullTotalPhys * 100.0f);
			m_edt_ctl[5].SetTextToStr(tzVal, TRUE);
		}			
	}

	/* CPU �������� 15 % ���� �� ��� ���� ���� ���� */
	/* HDD Drive : �ּ� 1 �� ���� �� ���� ���� ���� */
#ifdef _DEBUG
	if (dbRateCPU < 15.0f && m_u64TickHDD + (1 * 5000) < u64Tick)
#else
	if (dbRateCPU < 15.0f && m_u64TickHDD + (1 * 60000) < u64Tick)
#endif
	{
		m_u64TickHDD	= u64Tick;

		TCHAR tzDrv[8]	= {L"C:\\"};
		UINT64 u64Total, u64Free;
		DOUBLE dbGBytes	= 1024 * 1024 * 1024;

		/* ����̺� ��Ī �� �뷮 ��� */
		tzDrv[0]	= L'C' + pstCfgMoni->hdd_drive;
		if (uvCmn_GetHDDInfo(u64Total, u64Free, tzDrv))
		{
			/* ����̺� �� �뷮 ��� */
			swprintf_s(tzVal, 64, L"%.1f GB", u64Free / dbGBytes);
			m_edt_ctl[0].SetTextToStr(tzDrv, TRUE);
			m_edt_ctl[1].SetTextToStr(tzVal, TRUE);
		}
	}

	/* Device Error - MC2 */
	u16ErrCode	= pstShMemMC2->GetErrorCode(pstCfgMC2->axis_id, pstCfgMC2->drive_count);
	swprintf_s(tzVal, 64, L"0x%04x", u16ErrCode);
	m_edt_ctl[9].SetTextToStr(tzVal, TRUE);
	/* Device Error - PLC */
	u16ErrCode	= uvCmn_MCQ_GetLastErrorCode();
	swprintf_s(tzVal, 64, L"0x%04x", u16ErrCode);
	m_edt_ctl[10].SetTextToStr(tzVal, TRUE);
	/* Device Error - Luria */
	u16ErrCode	= uvCmn_Luria_GetLastErrorStatus();
	swprintf_s(tzVal, 64, L"0x%04x", u16ErrCode);
	m_edt_ctl[11].SetTextToStr(tzVal, TRUE);
	/* Device Error - Robot */
	u16ErrCode	= uvCmn_EFEM_GetLastErrorCode();
	swprintf_s(tzVal, 64, L"0x%04x", u16ErrCode);
	m_edt_ctl[12].SetTextToStr(tzVal, TRUE);
}

/*
 desc : ��ư �̺�Ʈ
 parm : id	- [in]  ��ư ID
 retn : None
*/
VOID CDlgMoni::OnBtnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_MONI_BTN_DEVICE_RESET	:	ResetDeviceInfo();	break;
	}
}

/*
 desc : ����̽� ���� ���� �ʱ�ȭ
 parm : None
 retn : None
*/
VOID CDlgMoni::ResetDeviceInfo()
{
	SYSTEMTIME stTm	= {NULL};

	GetLocalTime(&stTm);
	swprintf_s(uvEng_GetConfig()->monitor.mon_time, 24, L"%04d-%02d-%02d %02d:%02d:%02d",
			   stTm.wYear, stTm.wMonth, stTm.wDay, stTm.wHour, stTm.wMinute, stTm.wSecond);
	uvCmn_Luria_ResetLastErrorStatus();
	uvCmn_MCQ_ResetLastErrorCode();
	uvEng_ShMem_GetMC2()->ResetDriveAll();
	uvCmn_Logs_ResetErrorMesg();
}