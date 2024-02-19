
/*
 desc : Main Dialog
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgMain.h"
#include "MainThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 desc : 생성자
 parm : 자신의 윈도 ID, 부모 윈도 객체
 retn : None
*/
CDlgMain::CDlgMain(CWnd* parent /*=NULL*/)
	: CMyDialog(CDlgMain::IDD, parent)
{
	m_bDrawBG				= 0x01;
	m_hIcon					= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgMain::DoDataExchange(CDataExchange* dx)
{
	CMyDialog::DoDataExchange(dx);

}

BEGIN_MESSAGE_MAP(CDlgMain, CMyDialog)
	ON_MESSAGE(WM_MAIN_THREAD,	OnMsgMainThread)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_SET_HOMED, &CDlgMain::OnBnClickedMainBtnCmdSetHomed)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_GET_HOMED, &CDlgMain::OnBnClickedMainBtnCmdGetHomed)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_SET_CURRENT, &CDlgMain::OnBnClickedMainBtnCmdSetCurrent)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_GET_CURRENT, &CDlgMain::OnBnClickedMainBtnCmdGetCurrent)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_SET_DIRECT, &CDlgMain::OnBnClickedMainBtnCmdSetDirect)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_GET_DIRECT, &CDlgMain::OnBnClickedMainBtnCmdGetDirect)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_GET_AXIS_POS, &CDlgMain::OnBnClickedMainBtnCmdGetAxisPos)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_GET_DIAG_C, &CDlgMain::OnBnClickedMainBtnCmdGetDiagC)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_GET_EST, &CDlgMain::OnBnClickedMainBtnCmdGetEst)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_GET_PER, &CDlgMain::OnBnClickedMainBtnCmdGetPer)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_GET_RHS, &CDlgMain::OnBnClickedMainBtnCmdGetRhs)
	ON_BN_CLICKED(IDC_MAIN_BTN_APP_EXIT, &CDlgMain::OnBnClickedMainBtnAppExit)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_SEND, &CDlgMain::OnBnClickedMainBtnCmdSend)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_RESET, &CDlgMain::OnBnClickedMainBtnCmdReset)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_SET_HOME_OFFSET, &CDlgMain::OnBnClickedMainBtnCmdSetHomeOffset)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_GET_HOME_OFFSET, &CDlgMain::OnBnClickedMainBtnCmdGetHomeOffset)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_SET_SPEED, &CDlgMain::OnBnClickedMainBtnCmdSetSpeed)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_GET_SPEED, &CDlgMain::OnBnClickedMainBtnCmdGetSpeed)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_SET_JERK, &CDlgMain::OnBnClickedMainBtnCmdSetJerk)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_GET_JERK, &CDlgMain::OnBnClickedMainBtnCmdGetJerk)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_SET_AXIS_ALL_HOMING, &CDlgMain::OnBnClickedMainBtnCmdSetAxisAllHoming)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_SET_AXIS_MOVE, &CDlgMain::OnBnClickedMainBtnCmdSetAxisMove)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_SET_SERVO_OFF, &CDlgMain::OnBnClickedMainBtnCmdSetServoOff)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_SET_SERVO_ON, &CDlgMain::OnBnClickedMainBtnCmdSetServoOn)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_SET_AXIS_STOP, &CDlgMain::OnBnClickedMainBtnCmdSetAxisStop)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_SET_VAC_ON, &CDlgMain::OnBnClickedMainBtnCmdSetVacOn)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_SET_VAC_OFF, &CDlgMain::OnBnClickedMainBtnCmdSetVacOff)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_GET_NOTCH_SUCC, &CDlgMain::OnBnClickedMainBtnCmdGetNotchSucc)
	ON_BN_CLICKED(IDC_MAIN_BTN_CMD_GET_NOTCH_FAIL, &CDlgMain::OnBnClickedMainBtnCmdGetNotchFail)
END_MESSAGE_MAP()

/*
 desc : 시스템 명령어 제어
 parm : id		- 선택된 항목 ID 값
		lparam	- ???
 retn : 1 - 성공 / 0 - 실패
*/
VOID CDlgMain::OnSysCommand(UINT32 id, LPARAM lparam)
{
//	switch (id)
//	{
//	}

	CMyDialog::OnSysCommand(id, lparam);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::OnInitDlg()
{
	UINT32 u32Size	= 0, i = 0;

	/* 라이브러리 초기화 */
	if (!InitLib())		return FALSE;
	if (!InitCtrl())	return FALSE;

	/* Main Thread 생성 (!!! 중요. InitLib 가 성공하면, 반드시 생성해야 함 !!!) */
	m_pMainThread	= new CMainThread(m_hWnd);
	ASSERT(m_pMainThread);
	if (!m_pMainThread->CreateThread(0, NULL, NORMAL_THREAD_SPEED))
	{
		delete m_pMainThread;
		m_pMainThread	= NULL;
	}

	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgMain::OnExitDlg()
{
	UINT32 i	= 0;
	/* 기본 감시 스레드 메모리 해제 */
	if (m_pMainThread)
	{
		m_pMainThread->Stop();
		while (m_pMainThread->IsBusy())	m_pMainThread->Sleep();
		delete m_pMainThread;
		m_pMainThread	= NULL;
	}
	/* 라이브러리 해제 */
	CloseLib();
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgMain::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgMain::OnResizeDlg()
{
}

/*
 desc : Library Init
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::InitLib()
{
	return uvEng_Init(ENG_ERVM::en_cmps_sw);
}

/*
 desc : Library Close
 parm : None
 retn : None
*/
VOID CDlgMain::CloseLib()
{
	uvEng_Close();
}

/*
 desc : 컨트롤 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgMain::InitCtrl()
{
	((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_NAME))->SetCurSel(0);
	((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_DIRECT))->SetCurSel(0);
	((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_SPEED))->SetCurSel(0);

	return TRUE;
}

/*
 desc : Main Thread에서 발생된 이벤트 처리
 parm : wparam	- [in]  Event ID
		lparam	- [in]  Event Data
 retn : 0L
*/
LRESULT CDlgMain::OnMsgMainThread(WPARAM wparam, LPARAM lparam)
{

	return 0L;
}

/*
 desc : 부모 스레드에 의해 주기적으로 호출됨
 parm : None
 retn : None
*/
VOID CDlgMain::UpdatePeriod()
{
}

void CDlgMain::OnBnClickedMainBtnCmdSetHomed()
{
	TCHAR tzValue[32] = {NULL};
	INT32 i32Sel = ((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_NAME))->GetCurSel();
	if (i32Sel < 0)	return;
	ENG_PANC enAxis = ENG_PANC(i32Sel);
	((CEdit *)GetDlgItem(IDC_MAIN_EDT_CMD_SET_HOMED))->GetWindowTextW(tzValue, 32);
	if (wcslen(tzValue) < 1)	return;
	DOUBLE dbValue	= _wtof(tzValue);

	uvEng_MPA_SetAxisHomePosition(enAxis, dbValue);
}

void CDlgMain::OnBnClickedMainBtnCmdGetHomed()
{
	TCHAR tzValue[128]	= {NULL};
	LPG_MPDV pstVal	= uvEng_ShMem_GetLSPA();

	if (!uvEng_MPA_GetAxisHomePosition())	return;

	swprintf_s(tzValue, 128, L"T:%.1f, R:%.4f, Z:%.4f",
			   pstVal->homed_pos[0], pstVal->homed_pos[1], pstVal->homed_pos[2]);
	((CEdit*)GetDlgItem(IDC_MAIN_EDT_RECV))->SetWindowTextW(tzValue);
}


void CDlgMain::OnBnClickedMainBtnCmdSetCurrent()
{
	TCHAR tzValue[32] = {NULL};
	INT32 i32Sel = ((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_NAME))->GetCurSel();
	if (i32Sel < 0)	return;
	ENG_PANC enAxis = ENG_PANC(i32Sel);
	((CEdit *)GetDlgItem(IDC_MAIN_EDT_CMD_SET_CURRENT))->GetWindowTextW(tzValue, 32);
	if (wcslen(tzValue) < 1)	return;
	UINT8 u8Value	= (UINT8)_wtoi(tzValue);

	uvEng_MPA_SetAxisCurrentLimit(enAxis, u8Value);
}

void CDlgMain::OnBnClickedMainBtnCmdGetCurrent()
{
	TCHAR tzValue[128]	= {NULL};
	LPG_MPDV pstVal	= uvEng_ShMem_GetLSPA();

	if (!uvEng_MPA_GetAxisCurrentLimit())	return;

	swprintf_s(tzValue, 128, L"T:%d, R:%d, Z:%d",
			   pstVal->current_limit[0], pstVal->current_limit[1], pstVal->current_limit[2]);
	((CEdit*)GetDlgItem(IDC_MAIN_EDT_RECV))->SetWindowTextW(tzValue);
}


void CDlgMain::OnBnClickedMainBtnCmdSetDirect()
{
	TCHAR tzValue[32] = {NULL};
	INT32 i32Axis	= ((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_NAME))->GetCurSel();
	INT32 i32Direct	= ((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_DIRECT))->GetCurSel();
	if (i32Axis < 0 || i32Direct < 0)	return;
	ENG_PANC enAxis		= ENG_PANC(i32Axis);
	ENG_MAMD enDirect	= ENG_MAMD(i32Direct);
	((CEdit *)GetDlgItem(IDC_MAIN_EDT_CMD_SET_DIRECT))->GetWindowTextW(tzValue, 32);
	if (wcslen(tzValue) < 1)	return;
	DOUBLE dbValue	= _wtof(tzValue);

	uvEng_MPA_SetAxisDirectLimit(enAxis, enDirect, dbValue);
}

void CDlgMain::OnBnClickedMainBtnCmdGetDirect()
{
	TCHAR tzValue[128]	= {NULL};
	LPG_MPDV pstVal	= uvEng_ShMem_GetLSPA();
	INT32 i32Direct	= ((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_DIRECT))->GetCurSel();
	if (i32Direct < 0)	return;
	ENG_MAMD enDirect	= ENG_MAMD(i32Direct);

	if (!uvEng_MPA_GetAxisDirectLimit(enDirect))	return;

	if (i32Direct)
	{
		swprintf_s(tzValue, 128, L"T:%.3f, R:%.4f, Z:%.4f",
				   pstVal->direct_r_limit[0],
				   pstVal->direct_r_limit[1],
				   pstVal->direct_r_limit[2]);
	}
	else
	{
		swprintf_s(tzValue, 128, L"T:%.3f, R:%.4f, Z:%.4f",
				   pstVal->direct_f_limit[0],
				   pstVal->direct_f_limit[1],
				   pstVal->direct_f_limit[2]);
	}
	((CEdit*)GetDlgItem(IDC_MAIN_EDT_RECV))->SetWindowTextW(tzValue);
}

void CDlgMain::OnBnClickedMainBtnCmdGetAxisPos()
{
	TCHAR tzMesg[1024]	= {NULL};
	LPG_MPDV pstVal	= uvEng_ShMem_GetLSPA();

	if (!uvEng_MPA_GetAxisPos())	return;
	swprintf_s(tzMesg, 1024, L"Angle:%.3f, Horz.:%.3f um, Vert.:%.3f um",
			   pstVal->axis_pos[0], pstVal->axis_pos[1], pstVal->axis_pos[2]);
	AfxMessageBox(tzMesg);
}

void CDlgMain::OnBnClickedMainBtnCmdGetDiagC()
{
	TCHAR tzMesg[1024]	= {NULL};
	LPG_MPDV pstVal	= uvEng_ShMem_GetLSPA();
	CUniToChar csCnv;

	if (!uvEng_MPA_GetDiagState())	return;
	swprintf_s(tzMesg, 1024, L"%s", csCnv.Ansi2Uni(pstVal->last_string_data));
	AfxMessageBox(tzMesg);
}

void CDlgMain::OnBnClickedMainBtnCmdGetEst()
{
	TCHAR tzMesg[1024]	= {NULL};
	LPG_MPDV pstVal	= uvEng_ShMem_GetLSPA();
	CUniToChar csCnv;

	if (!uvEng_MPA_GetMotionState())	return;
	swprintf_s(tzMesg, 1024, L"%s", csCnv.Ansi2Uni(pstVal->last_string_data));
	AfxMessageBox(tzMesg);
}

void CDlgMain::OnBnClickedMainBtnCmdGetPer()
{
	TCHAR tzMesg[1024]	= {NULL};
	LPG_MPDV pstVal	= uvEng_ShMem_GetLSPA();
	CUniToChar csCnv;

	if (!uvEng_MPA_GetErrorReason())	return;
	swprintf_s(tzMesg, 1024, L"%s", csCnv.Ansi2Uni(pstVal->last_string_data));
	AfxMessageBox(tzMesg);
}

void CDlgMain::OnBnClickedMainBtnCmdGetRhs()
{
	TCHAR tzMesg[1024]	= {NULL};
	LPG_MPDV pstVal	= uvEng_ShMem_GetLSPA();
	CUniToChar csCnv;

	if (!uvEng_MPA_GetAllSwitchState())	return;
	swprintf_s(tzMesg, 1024, L"Z(Vert) : H(%d) Forward (%d) Reverse (%d)\n"
							 L"R(Horz) : H(%d) Forward (%d) Reverse (%d)\n"
							 L"T(Angle): H(%d) Forward (%d) Reverse (%d)",
			   (UINT8)pstVal->IsMotionAxisChecked(ENG_PANC::pac_z, ENG_PASP::asp_h),
			   (UINT8)pstVal->IsMotionAxisChecked(ENG_PANC::pac_z, ENG_PASP::asp_f),
			   (UINT8)pstVal->IsMotionAxisChecked(ENG_PANC::pac_z, ENG_PASP::asp_r),

			   (UINT8)pstVal->IsMotionAxisChecked(ENG_PANC::pac_r, ENG_PASP::asp_h),
			   (UINT8)pstVal->IsMotionAxisChecked(ENG_PANC::pac_r, ENG_PASP::asp_f),
			   (UINT8)pstVal->IsMotionAxisChecked(ENG_PANC::pac_r, ENG_PASP::asp_r),

			   (UINT8)pstVal->IsMotionAxisChecked(ENG_PANC::pac_t, ENG_PASP::asp_h),
			   (UINT8)pstVal->IsMotionAxisChecked(ENG_PANC::pac_t, ENG_PASP::asp_f),
			   (UINT8)pstVal->IsMotionAxisChecked(ENG_PANC::pac_t, ENG_PASP::asp_r));
	AfxMessageBox(tzMesg);
}

void CDlgMain::OnBnClickedMainBtnAppExit()
{
PostMessage(WM_CLOSE, 0, 0L);
}

void CDlgMain::OnBnClickedMainBtnCmdSend()
{
	TCHAR tzCmd[MAX_CMD_MILARA_SEND]	= {NULL};
	GetDlgItem(IDC_MAIN_EDT_CMD_SEND)->GetWindowTextW(tzCmd, MAX_CMD_MILARA_SEND);
//	uvEng_MPA_SendCmd(tzCmd, (UINT32)wcslen(tzCmd));
}

void CDlgMain::OnBnClickedMainBtnCmdReset()
{
	uvEng_MPA_ResetCommData();
}

void CDlgMain::OnBnClickedMainBtnCmdSetHomeOffset()
{
	TCHAR tzValue[32] = {NULL};
	INT32 i32Axis	= ((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_NAME))->GetCurSel();
	if (i32Axis < 0)	return;
	ENG_PANC enAxis	= ENG_PANC(i32Axis);
	((CEdit *)GetDlgItem(IDC_MAIN_EDT_CMD_SET_HOME_OFFSET))->GetWindowTextW(tzValue, 32);
	if (wcslen(tzValue) < 1)	return;
	DOUBLE dbValue	= _wtof(tzValue);

	uvEng_MPA_SetAxisHomeOffset(enAxis, dbValue);
}

void CDlgMain::OnBnClickedMainBtnCmdGetHomeOffset()
{
	TCHAR tzValue[32] = {NULL};
	LPG_MPDV pstVal	= uvEng_ShMem_GetLSPA();
	if (!uvEng_MPA_GetAxisHomeOffset())	return;
	swprintf_s(tzValue, 128, L"T:%.3f, R:%.4f, Z:%.4f",
			   pstVal->home_offset[0],
			   pstVal->home_offset[1],
			   pstVal->home_offset[2]);
	((CEdit*)GetDlgItem(IDC_MAIN_EDT_RECV))->SetWindowTextW(tzValue);
}

void CDlgMain::OnBnClickedMainBtnCmdSetSpeed()
{
	TCHAR tzValue[32] = {NULL};
	INT32 i32Axis	= ((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_NAME))->GetCurSel();
	INT32 i32Speed	= ((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_SPEED))->GetCurSel();
	if (i32Axis < 0 || i32Speed < 0)	return;
	ENG_PANC enAxis		= ENG_PANC(i32Axis);
	ENG_MAST enSpeed	= ENG_MAST(i32Speed);
	((CEdit *)GetDlgItem(IDC_MAIN_EDT_CMD_SET_SPEED))->GetWindowTextW(tzValue, 32);
	if (wcslen(tzValue) < 1)	return;
	DOUBLE dbValue	= _wtof(tzValue);

	uvEng_MPA_SetAxisInstSpeed(enAxis, enSpeed, dbValue);
}

void CDlgMain::OnBnClickedMainBtnCmdGetSpeed()
{
	TCHAR tzValue[128]	= {NULL};
	LPG_MPDV pstVal	= uvEng_ShMem_GetLSPA();
	INT32 i32Speed	= ((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_SPEED))->GetCurSel();
	if (i32Speed < 0)	return;
	ENG_MAST enSpeed= ENG_MAST(i32Speed);

	if (!uvEng_MPA_GetAxisInstSpeed(enSpeed))	return;
	if (!i32Speed)
	{
		swprintf_s(tzValue, 128, L"T:%.3f, R:%.4f, Z:%.4f",
				   pstVal->axis_acl[0],
				   pstVal->axis_acl[1],
				   pstVal->axis_acl[2]);
	}
	else
	{
		swprintf_s(tzValue, 128, L"T:%.3f, R:%.4f, Z:%.4f",
				   pstVal->axis_dcl[0],
				   pstVal->axis_dcl[1],
				   pstVal->axis_dcl[2]);
	}
	((CEdit*)GetDlgItem(IDC_MAIN_EDT_RECV))->SetWindowTextW(tzValue);
}

void CDlgMain::OnBnClickedMainBtnCmdSetJerk()
{
	TCHAR tzValue[32] = {NULL};
	INT32 i32Axis	= ((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_NAME))->GetCurSel();
	INT32 i32Speed	= ((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_SPEED))->GetCurSel();
	if (i32Axis < 0 || i32Speed < 0)	return;
	ENG_PANC enAxis		= ENG_PANC(i32Axis);
	ENG_MAST enSpeed	= ENG_MAST(i32Speed);
	((CEdit *)GetDlgItem(IDC_MAIN_EDT_CMD_SET_JERK))->GetWindowTextW(tzValue, 32);
	if (wcslen(tzValue) < 1)	return;
	DOUBLE dbValue	= _wtof(tzValue);

	uvEng_MPA_SetAxisJerkSpeed(enAxis, enSpeed, dbValue);
}

void CDlgMain::OnBnClickedMainBtnCmdGetJerk()
{
	TCHAR tzValue[128]	= {NULL};
	LPG_MPDV pstVal	= uvEng_ShMem_GetLSPA();
	INT32 i32Speed	= ((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_SPEED))->GetCurSel();
	if (i32Speed < 0)	return;
	ENG_MAST enSpeed= ENG_MAST(i32Speed);

	if (!uvEng_MPA_GetAxisJerkSpeed(enSpeed))	return;

	if (!i32Speed)
	{
		swprintf_s(tzValue, 128, L"T:%.3f, R:%.4f, Z:%.4f",
				   pstVal->axis_acl[0],
				   pstVal->axis_acl[1],
				   pstVal->axis_acl[2]);
	}
	else
	{
		swprintf_s(tzValue, 128, L"T:%.3f, R:%.4f, Z:%.4f",
				   pstVal->axis_dcl[0],
				   pstVal->axis_dcl[1],
				   pstVal->axis_dcl[2]);
	}
	((CEdit*)GetDlgItem(IDC_MAIN_EDT_RECV))->SetWindowTextW(tzValue);
}

void CDlgMain::OnBnClickedMainBtnCmdSetAxisAllHoming()
{
	uvEng_MPA_SetAxisAllHoming();
}

void CDlgMain::OnBnClickedMainBtnCmdSetAxisMove()
{
	INT32 i32Sel = ((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_NAME))->GetCurSel();
	ENG_PANC enAxis = ENG_PANC(i32Sel);
	uvEng_MPA_MoveHomePosition(enAxis);
}

void CDlgMain::OnBnClickedMainBtnCmdSetServoOff()
{
	INT32 i32Sel = ((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_NAME))->GetCurSel();
	ENG_PANC enAxis = ENG_PANC(i32Sel);
	uvEng_MPA_ServoControl(enAxis, ENG_PSOO::soo_off);
}

void CDlgMain::OnBnClickedMainBtnCmdSetServoOn()
{
	INT32 i32Sel = ((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_NAME))->GetCurSel();
	ENG_PANC enAxis = ENG_PANC(i32Sel);
	uvEng_MPA_ServoControl(enAxis, ENG_PSOO::soo_on);
}

void CDlgMain::OnBnClickedMainBtnCmdSetAxisStop()
{
	INT32 i32Sel = ((CComboBox *)GetDlgItem(IDC_MAIN_CMB_AXIS_NAME))->GetCurSel();
	ENG_PANC enAxis = ENG_PANC(i32Sel);
	uvEng_MPA_StopToAxis(enAxis);
}

void CDlgMain::OnBnClickedMainBtnCmdSetVacOn()
{
	uvEng_MPA_ChuckVacuum(ENG_PSOO::soo_on);
}

void CDlgMain::OnBnClickedMainBtnCmdSetVacOff()
{
	uvEng_MPA_ChuckVacuum(ENG_PSOO::soo_off);
}


void CDlgMain::OnBnClickedMainBtnCmdGetNotchSucc()
{
	uvEng_MPA_GetWaferNotchCount(ENG_PASR::asr_succ);
}


void CDlgMain::OnBnClickedMainBtnCmdGetNotchFail()
{
	uvEng_MPA_GetWaferNotchCount(ENG_PASR::asr_fail);
}
