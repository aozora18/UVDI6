
/*
 desc : Monitoring - Trigger Value
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgPlcQ.h"

#include "DlgTrig.h"


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
CDlgTrig::CDlgTrig(UINT32 id, CWnd *parent)
	: CDlgChild(id, parent)
{
	m_pGrid[0]	= NULL;
	m_pGrid[1]	= NULL;
	m_pGrid[2]	= NULL;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgTrig::~CDlgTrig()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgTrig::DoDataExchange(CDataExchange* dx)
{
	CDlgChild::DoDataExchange(dx);

	UINT32 i, u32StartID;

	/* Group box */
	u32StartID	= IDC_TRIG_GRP_QUAD;
	for (i=0; i<3; i++)		DDX_Control(dx, u32StartID+i,	m_grp_ctl[i]);
	/* Static */
	u32StartID	= IDC_TRIG_TXT_CHNO;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_txt_ctl[i]);
	/* Combo box */
	u32StartID	= IDC_TRIG_CMB_CHNO;
	for (i=0; i<1; i++)		DDX_Control(dx, u32StartID+i,	m_cmb_ctl[i]);
}

BEGIN_MESSAGE_MAP(CDlgTrig, CDlgChild)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgTrig::PreTranslateMessage(MSG* msg)
{

	return CDlgChild::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgTrig::OnInitDlg()
{
	if (!CDlgChild::OnInitDlg())	return FALSE;

	/* 컨트롤 초기화 */
	if (!InitCtrl())	return FALSE;
	/* GridControl 초기화 */
	if (!InitGrid())	return FALSE;

	/* 상위 객체 호출 */
	return TRUE;
}

/*
 desc : 종료시에 한 번 호출됨
 parm : None
 retn : None
*/
VOID CDlgTrig::OnExitDlg()
{
	UINT8 i	= 0x00;
	/* 그리드 컨트롤 메모리 해제 */
	for (; i<3; i++)
	{
		if (m_pGrid[i])
		{
			if (m_pGrid[i]->GetSafeHwnd())	m_pGrid[i]->DestroyWindow();
			delete m_pGrid[i];
		}
	}
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgTrig::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgTrig::OnResizeDlg()
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgTrig::InitCtrl()
{
	INT32 i;

	/* Groupbox */
	for (i=0; i<3; i++)
	{
		m_grp_ctl[i].SetFont(&g_lf);
	}
	/* Combobox */
	for (i=0; i<1; i++)
	{
		m_cmb_ctl[i].SetLogFont(&g_lf);
	}
	/* text - normal */
	for (i=0; i<1; i++)
	{
		m_txt_ctl[i].SetTextFont(&g_lf);
		m_txt_ctl[i].SetDrawBg(1);
		m_txt_ctl[i].SetBaseProp(0, 1, 0, 0, RGB(254, 254, 254), 0, RGB(0, 0, 0));
	}
	m_cmb_ctl[0].SetCurSel(0);

	return TRUE;
}

/*
 desc : 그리드 컨트롤 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgTrig::InitGrid()
{
	TCHAR tzTitle[6][16]= { L"Item", L"Value", L"Item", L"Value (ns)", L"Item", L"Position" };
	INT32 i32Width[3][2]= { {110, 86 }, {110, 86 }, {105, 100 } }, i, j;
	INT32 i32ColCnt		= 2, i32RowCnt[3] = { 4, 5, 16 }, i32Scroll[3] = { 0, 0, 1 };
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	UINT32 u32GridID[3]	= { IDC_GRID_TRIG_QUAD, IDC_GRID_TRIG_TRIG, IDC_GRID_TRIG_CHNO };
	GV_ITEM stGV		= { NULL };
	CRect rGrid[3];

	/* Quardrature Register / Trigger Setup / Trigger Position Information */
	for (i=0; i<3; i++)
	{
		::GetWindowRect(m_grp_ctl[i].GetSafeHwnd(), rGrid[i]);
		ScreenToClient(rGrid[i]);
		rGrid[i].left	+= 15;
		rGrid[i].right	-= 15;
		rGrid[i].top	+= 22;
		rGrid[i].bottom	-= 15;
	}

	/* 그리드 컨트롤 기본 공통 속성 */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* 객체 생성 */
	for (i=0; i<3; i++)
	{
		m_pGrid[i] = new CGridCtrl;
		ASSERT(m_pGrid[i]);
		m_pGrid[i]->SetDrawScrollBar(i32Scroll[i]);
		m_pGrid[i]->SetDrawScrollBarHorz(0);
		m_pGrid[i]->SetDrawScrollBarVert(i32Scroll[i]);
		if (!m_pGrid[i]->Create(rGrid[i], this, u32GridID[i], dwStyle))
		{
			delete m_pGrid[i];
			m_pGrid[i] = NULL;
			return FALSE;
		}
		/* 객체 기본 설정 */
		m_pGrid[i]->SetLogFont(g_lf);
		m_pGrid[i]->SetRowCount(i32RowCnt[i]+1);
		m_pGrid[i]->SetColumnCount(2);
		m_pGrid[i]->SetFixedRowCount(1);
		m_pGrid[i]->SetRowHeight(0, 24);
		m_pGrid[i]->SetFixedColumnCount(0);
		m_pGrid[i]->SetBkColor(RGB(255, 255, 255));
		m_pGrid[i]->SetFixedColumnSelection(0);
	}

	/* 타이틀 (첫 번째 행) */
	stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr = RGB(214, 214, 214);
	stGV.crFgClr = RGB(0, 0, 0);
	for (i=0; i<3; i++)
	{
		for (j=0; j<i32ColCnt; j++)
		{
			stGV.nFormat= DT_VCENTER | DT_SINGLELINE | DT_CENTER;
			stGV.row	= 0;
			stGV.col	= j;
			stGV.strText= tzTitle[j+i*i32ColCnt];
			m_pGrid[i]->SetItem(&stGV);
			m_pGrid[i]->SetColumnWidth(j, i32Width[i][j]);
//			m_pGrid[i]->SetItemFont(0, j, &g_lf);
		}
	}
	/* 본문 (1 ~ Rows) 높이 / 주소 설정 설정 (루프 동작 속도 때문에 불가피하게 이중 for 문 사용 안함) */
	stGV.crBkClr = RGB(255, 255, 255);
	for (i=0; i<3; i++)
	{
		for (j=1; j<=i32RowCnt[i]; j++)
		{
			m_pGrid[i]->SetRowHeight(j, 24);

			stGV.nFormat= DT_LEFT | DT_VCENTER | DT_SINGLELINE;
			stGV.strText= L"";
			stGV.row	= j;
			stGV.col	= 0;
			m_pGrid[i]->SetItem(&stGV);
//			m_pGrid[i]->SetItemFont(j, 0, &g_lf);

			stGV.nFormat= DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			stGV.col	= 1;
			m_pGrid[i]->SetItem(&stGV);
//			m_pGrid[i]->SetItemFont(j, 1, &g_lf);
		}

		m_pGrid[i]->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
		m_pGrid[i]->UpdateGrid();
	}

	/* Quardrature Register */
	m_pGrid[0]->SetItemText(1, 0, L"Command");
	m_pGrid[0]->SetItemText(2, 0, L"Enc. Counter");
	m_pGrid[0]->SetItemText(3, 0, L"Output On/Off");
	m_pGrid[0]->SetItemText(4, 0, L"Enc. Out Value");

	/* Trigger Setup Information */
	m_pGrid[1]->SetItemText(1, 0, L"Trig. OnTime");
	m_pGrid[1]->SetItemText(2, 0, L"Strobe OnTime");
	m_pGrid[1]->SetItemText(3, 0, L"Delay Time");
	m_pGrid[1]->SetItemText(4, 0, L"Trig. Pos Pluse");
	m_pGrid[1]->SetItemText(5, 0, L"Trig. Pos Minus");

	/* Trigger Position */
	TCHAR tzPos[64]	= {NULL};
	for (i=1; i<=16; i++)
	{
		swprintf_s(tzPos, 64, L"Trigger No %02d", i);
		m_pGrid[2]->SetItemText(i, 0, tzPos);
	}

	return TRUE;
}

/*
 desc : 현재 윈도 컨트롤 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgTrig::UpdateCtrl()
{
	/* Trigger Memory Data 갱신 */
	UpdateTrigValue();
}

/*
 desc : Trigger Memory Data (Value) 갱신
 parm : None
 retn : None
*/
VOID CDlgTrig::UpdateTrigValue()
{
	UINT8 u8ChNo		= (UINT8)m_cmb_ctl[0].GetCurSel();
	UINT32 i;
	TCHAR tzData[128]	= {NULL};
	LPG_TPQR pstTrig	= uvEng_ShMem_GetTrig();

	if (!(uvEng_GetConfig()->use_libs.use_acam_kind == (UINT8)ENG_VCPK::en_camera_ids_ph))	return;
	/* Trigger Channel Number 변경 */
	swprintf_s(tzData, 128, L"Channel [%d] Trigger Position", u8ChNo+1);
	m_grp_ctl[2].SetText(tzData, FALSE);

	/* ------------------------------------------------------------------------ */
	/* 그리드 전체 갱신하지 말구. 각 컬럼 단위로 갱신 진행 (화면 깜박임 최소화) */
	/* ------------------------------------------------------------------------ */

	/* Quardrature Register */
	m_pGrid[0]->SetItemHex32(1, 1, INT32(pstTrig->command));			m_pGrid[0]->InvalidateRowCol(1, 1);
	m_pGrid[0]->SetItemText(2, 1, UINT32(pstTrig->enc_counter));		m_pGrid[0]->InvalidateRowCol(2, 1);
	if (0x01 == pstTrig->trig_strob_ctrl)	m_pGrid[0]->SetItemText(3, 1, L"Enabled");
	else									m_pGrid[0]->SetItemText(3, 1, L"Disabled");
																		m_pGrid[0]->InvalidateRowCol(3, 1);
	m_pGrid[0]->SetItemText(4, 1, UINT32(pstTrig->enc_out_val));		m_pGrid[0]->InvalidateRowCol(4, 1);

	/* Trigger Setup */
	m_pGrid[1]->SetItemText(1, 1, UINT32(pstTrig->trig_set[u8ChNo].area_trig_ontime), TRUE);
	m_pGrid[1]->InvalidateRowCol(1, 1);
	m_pGrid[1]->SetItemText(2, 1, UINT32(pstTrig->trig_set[u8ChNo].area_strob_ontime), TRUE);
	m_pGrid[1]->InvalidateRowCol(2, 1);
	m_pGrid[1]->SetItemText(3, 1, UINT32(pstTrig->trig_set[u8ChNo].area_strob_trig_delay), TRUE);
	m_pGrid[1]->InvalidateRowCol(3, 1);
	m_pGrid[1]->SetItemText(4, 1, UINT32(pstTrig->trig_set[u8ChNo].area_trig_pos_plus), TRUE);
	m_pGrid[1]->InvalidateRowCol(4, 1);
	m_pGrid[1]->SetItemText(5, 1, UINT32(pstTrig->trig_set[u8ChNo].area_trig_pos_minus), TRUE);
	m_pGrid[1]->InvalidateRowCol(5, 1);

	/* Trigger Position (Counter) */
	for (i=0; i<MAX_REGIST_TRIG_POS_COUNT; i++)
	{
		m_pGrid[2]->SetItemText(i+1, 1, UINT32(pstTrig->trig_set[u8ChNo].area_trig_pos[i]), TRUE);
		m_pGrid[1]->InvalidateRowCol(i+1, 1);
	}
}