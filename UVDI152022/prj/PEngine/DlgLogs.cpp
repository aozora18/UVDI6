
/*
 desc : Monitoring - Log Message
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgLogs.h"


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
CDlgLogs::CDlgLogs(UINT32 id, CWnd *parent)
	: CDlgChild(id, parent)
{
	m_bUpdateLogs	= TRUE;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgLogs::~CDlgLogs()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgLogs::DoDataExchange(CDataExchange* dx)
{
	CDlgChild::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Button - normal */
	u32StartID	= IDC_LOGS_BTN_CLEAR;
	for (i=0; i<2; i++)		DDX_Control(dx, u32StartID+i,	m_btn_ctl[i]);
	/* Checkbox */
	u32StartID	= IDC_LOGS_CHK_PAUSE;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_chk_ctl[i]);
	/* Edit - number */
	u32StartID	= IDC_LOGS_RCH_MESG;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_rch_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgLogs, CDlgChild)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_LOGS_BTN_CLEAR, IDC_LOGS_BTN_COPY, OnBtnClicked)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_LOGS_CHK_PAUSE, IDC_LOGS_CHK_PAUSE, OnChkClicked)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgLogs::PreTranslateMessage(MSG* msg)
{

	return CDlgChild::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLogs::OnInitDlg()
{
	if (!CDlgChild::OnInitDlg())	return FALSE;

	/* 컨트롤 초기화 */
	if (!InitCtrl())	return FALSE;

	/* 상위 객체 호출 */
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgLogs::OnExitDlg()
{
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgLogs::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgLogs::OnResizeDlg()
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgLogs::InitCtrl()
{
	INT32 i;

	/* button - normal */
	for (i=0; i<2; i++)
	{
		m_btn_ctl[i].SetLogFont(g_lf);
		m_btn_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
	/* checkbox - normal */
	for (i=0; i<1; i++)
	{
		m_chk_ctl[i].SetLogFont(g_lf);
		m_chk_ctl[i].SetBgColor(RGB(0xff, 0xff, 0xff));
	}
 	/* rich edit */
 	for (i=0; i<1; i++)
 	{
		m_rch_ctl[i].SetRichFont(g_lf.lfFaceName, g_lf.lfWeight);
		m_rch_ctl[i].SetKeyInputEnable(FALSE);
 	}

	/* 등록 가능한 라인 수 설정 */
#ifdef _DEBUG
	m_rch_ctl[0].SetMaxLines(1024);
#else
	m_rch_ctl[0].SetMaxLines(4096);
#endif

	return TRUE;
}

/*
 desc : 현재 윈도 컨트롤 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgLogs::UpdateCtrl()
{
	/* Update Log Data */
	UpdateLogs();
}

/*
 desc : PLC Memory Data (Value) 갱신
 parm : None
 retn : None
*/
VOID CDlgLogs::UpdateLogs()
{
	TCHAR tzMesg[LOG_MESG_SIZE]	= {NULL};
	UINT8 u8LogLevel	= 0x00;
	PTCHAR ptzMesg		= NULL;
	COLORREF clrText[4]	= { RGB(0, 0, 0), RGB(0, 153, 255), RGB(255, 0, 0),  RGB(255, 0, 0) };
	UINT32 u32TickCnt	= (UINT32)GetTickCount();

	/* 로그 갱신 여부 확인 */
	if (!m_bUpdateLogs)	return;

	/* 화면 갱신 해제 */
	m_rch_ctl[0].SetRedraw(FALSE);

	/* 로그 메시지가 없을 때까지 읽어서 출력 */
	do {
	
		/* 현재 등록되어 있는 즉, 읽지 않은 로그 메시지가 없을 때까지 읽기 */
		ptzMesg	= uvCmn_Logs_GetLastMesg(u8LogLevel);
		if (!ptzMesg)	break;
		/* 반드시 내부 버퍼에 저장하고 난 이후에 리스트 박스에 전달 (뭔가 공유 메모리의 문자열 끝 부분 인식하는데 오류) */
		wcscpy_s(tzMesg, LOG_MESG_SIZE, ptzMesg);
		m_rch_ctl[0].Append(tzMesg, clrText[u8LogLevel]);

		/* 최소한 읽기 수행한 시간이 100 msec 이상 넘으면 안됨 */
		if (u32TickCnt + 100 < (UINT32)GetTickCount())	break;

	} while (ptzMesg);

	/* 화면 갱신 활성화 */
	m_rch_ctl[0].SetRedraw(TRUE);
	m_rch_ctl[0].Invalidate(FALSE);
}

/*
 desc : 버튼 이벤트
 parm : id	- [in]  컨트롤 ID
 retn : None
*/
VOID CDlgLogs::OnBtnClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_LOGS_BTN_CLEAR	:	m_rch_ctl[0].SetClearAll(TRUE);	break;
	case IDC_LOGS_BTN_COPY	:	m_rch_ctl[0].SetCopyAll(TRUE);	break;
	}
}

/*
 desc : 버튼 이벤트
 parm : id	- [in]  컨트롤 ID
 retn : None
*/
VOID CDlgLogs::OnChkClicked(UINT32 id)
{
	switch (id)
	{
	case IDC_LOGS_CHK_PAUSE	:	m_bUpdateLogs	= m_chk_ctl[0].GetCheck() > 0 ? FALSE : TRUE;	break;
	}
}