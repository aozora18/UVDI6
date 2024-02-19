
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
 desc : 생성자
 parm : id		- [in]  자신의 윈도 ID
		parent	- [in]  자신을 호출한 부모 윈도 클래스 포인터
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
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgMoni::~CDlgMoni()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
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
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgMoni::PreTranslateMessage(MSG* msg)
{

	return CDlgChild::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMoni::OnInitDlg()
{
	if (!CDlgChild::OnInitDlg())	return FALSE;

	/* CPU 정보 얻기 : 초기화 */
	if (!m_csCPU.Init())
	{
		AfxMessageBox(L"Failed to initialize the CPU Info.", MB_ICONSTOP|MB_TOPMOST);
		return FALSE;
	}

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
VOID CDlgMoni::OnExitDlg()
{
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgMoni::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgMoni::OnResizeDlg()
{
}

/*
 desc : 공통 컨트롤 초기화
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
 desc : 현재 윈도 컨트롤 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgMoni::UpdateCtrl()
{
	/* Update Log Data */
	UpdateMoni();
}

/*
 desc : PLC Memory Data (Value) 갱신
 parm : None
 retn : None
*/
VOID CDlgMoni::UpdateMoni()
{
	TCHAR tzVal[64]		= {NULL};
	UINT16 u16ErrCode	= 0x0000;
	UINT32 u32Rand		= 0;
	UINT64 u64Tick		= GetTickCount64();
	DOUBLE dbRateCPU	= 100.0f;	/* 무조건 100.0f로 고정 */
	LPG_CSDM pstCfgMoni	= &uvEng_GetConfig()->monitor;
	LPG_CMSI pstCfgMC2	= &uvEng_GetConfig()->mc2_svc;
	LPG_MDSM pstShMemMC2= uvEng_ShMem_GetMC2();

	/* Monitoring 시작 시간 출력 */
	m_edt_ctl[6].SetTextToStr(uvEng_GetConfig()->monitor.mon_time, TRUE);
	/* 에러 & 경고 개수 */
	m_edt_ctl[7].SetTextToNum(uvCmn_Logs_GetErrorWarnCount(0x00), TRUE);
	m_edt_ctl[8].SetTextToNum(uvCmn_Logs_GetErrorWarnCount(0x01), TRUE);

	/* CPU (1 ~ 2 초 정도 마다 갱신) */
	u32Rand	= (UINT32)uvCmn_GetRandNumer(0x00, 2);
	if (m_u64TickCPU + (1000 + u32Rand) < u64Tick)
	{
		m_u64TickCPU	= u64Tick;

		/* CPU & Process 점유율 계산 */
		if (m_csCPU.GetUsageRate(TRUE))
		{
			/* 아래 임시 변수에 저장한 이유는 추후 HDD Drive 갱신하기 위함 */
			dbRateCPU	= m_csCPU.GetRateCPU();
 			swprintf_s(tzVal, 64, L"%.2f %%", dbRateCPU);
 			m_edt_ctl[2].SetTextToStr(tzVal, TRUE);
 			swprintf_s(tzVal, 64, L"%.2f %%", m_csCPU.GetRateProc());
 			m_edt_ctl[3].SetTextToStr(tzVal, TRUE);
		}
	}

	/* Memory (10 ~ 20 초 마다 갱신) */
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

	/* CPU 점유율이 15 % 이하 인 경우 갱신 조건 설정 */
	/* HDD Drive : 최소 1 분 마다 한 번씩 갱신 수행 */
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

		/* 드라이브 명칭 및 용량 얻기 */
		tzDrv[0]	= L'C' + pstCfgMoni->hdd_drive;
		if (uvCmn_GetHDDInfo(u64Total, u64Free, tzDrv))
		{
			/* 드라이브 및 용량 얻기 */
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
 desc : 버튼 이벤트
 parm : id	- [in]  버튼 ID
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
 desc : 디바이스 에러 정보 초기화
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