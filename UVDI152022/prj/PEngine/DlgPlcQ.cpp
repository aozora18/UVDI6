
/*
 desc : Monitoring - PLC Value
*/

#include "pch.h"
#include "MainApp.h"
#include "DlgPlcQ.h"

#include "PLC/DlgPSet.h"


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
CDlgPlcQ::CDlgPlcQ(UINT32 id, CWnd *parent)
	: CDlgChild(id, parent)
{
	m_pGrid		= NULL;
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CDlgPlcQ::~CDlgPlcQ()
{
}

/*
 desc : 윈도 ID 매핑
 parm : dx	- 매핑 객체 ID
 retn : None
*/
VOID CDlgPlcQ::DoDataExchange(CDataExchange* dx)
{
	CDlgChild::DoDataExchange(dx);

}

BEGIN_MESSAGE_MAP(CDlgPlcQ, CDlgChild)
	ON_BN_CLICKED(IDC_GRID_PLCQ_DATA,	OnGridClicked)
END_MESSAGE_MAP()

/*
 desc : 모든 메시지 가로채기 ㅋㅋㅋ
 parm : msg	- 메시지 정보가 저장된 구조체 포인터
 retn : 상위 부모 메시지 함수 호출... 혹은 1 or 0
*/
BOOL CDlgPlcQ::PreTranslateMessage(MSG* msg)
{

	return CDlgChild::PreTranslateMessage(msg);
}

/*
 desc : 초기 실행시에 한 번 호출됨
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgPlcQ::OnInitDlg()
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
VOID CDlgPlcQ::OnExitDlg()
{
	/* 그리드 컨트롤 메모리 해제 */
	if (m_pGrid)
	{
		if (m_pGrid->GetSafeHwnd())	m_pGrid->DestroyWindow();
		delete m_pGrid;
	}
}

/*
 desc : 윈도 갱신될 때마다 호출됨
 parm : dc	- 윈도 DC
 retn : None
*/
VOID CDlgPlcQ::OnPaintDlg(CDC *dc)
{
}

/*
 desc : 다이얼로그 크기가 변경될 경우... 각종 컨트롤 크기 변경
 parm : None
 retn : None
*/
VOID CDlgPlcQ::OnResizeDlg()
{
}

/*
 desc : 공통 컨트롤 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgPlcQ::InitCtrl()
{

	return TRUE;
}

/*
 desc : 그리드 컨트롤 초기화
 parm : None
 retn : TRUE or FALSE
*/
BOOL CDlgPlcQ::InitGrid()
{
	TCHAR tzTitle[3][40]= { L"D Address",
							L"F E D C  B A 9 8  7 6 5 4  3 2 1 0",
							L"Value (16 : 32 bit)" };
	TCHAR tzDAddr[32]	= {NULL};
	INT32 i32Width[3]	= { 90, 240, 145 }, i;
	INT32 i32ColCnt		= 3, i32RowCnt = (INT32)uvEng_Conf_GetPLC()->addr_d_count;
	UINT32 dwStyle		= WS_CHILD | WS_TABSTOP | WS_VISIBLE;
	UINT32 dwFormat[2]	= { DT_VCENTER | DT_SINGLELINE | DT_CENTER,
							DT_VCENTER | DT_SINGLELINE | DT_RIGHT };
	GV_ITEM stGV		= { NULL };
	CRect rGrid;

	/* 현재 윈도 영역 얻고, 그리드 크기 설정 */
	GetClientRect(rGrid);
	rGrid.left	+= 10;
	rGrid.right	-= 10;
	rGrid.top	+= 13;
	rGrid.bottom-= 10;

	/* 그리드 컨트롤 기본 공통 속성 */
	stGV.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR | GVIF_FGCLR;
	stGV.crFgClr = RGB(32, 32, 32);

	/* 객체 생성 */
	m_pGrid = new CGridCtrl;
	ASSERT(m_pGrid);
	m_pGrid->SetDrawScrollBar(1);
	m_pGrid->SetDrawScrollBarVert(1);
	m_pGrid->SetDrawScrollBarHorz(1);
	if (!m_pGrid->Create(rGrid, this, IDC_GRID_PLCQ_DATA, dwStyle))
	{
		delete m_pGrid;
		m_pGrid = NULL;
		return FALSE;
	}
	/* 객체 기본 설정 */
	m_pGrid->SetLogFont(g_lf);
	m_pGrid->SetRowCount(i32RowCnt+1);
	m_pGrid->SetColumnCount(i32ColCnt);
	m_pGrid->SetFixedRowCount(1);
	m_pGrid->SetRowHeight(0, 25);
	m_pGrid->SetFixedColumnCount(0);
	m_pGrid->SetBkColor(RGB(255, 255, 255));
	m_pGrid->SetFixedColumnSelection(0);

	/* 타이틀 (첫 번째 행) */
	stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	stGV.crBkClr = RGB(214, 214, 214);
	stGV.crFgClr = RGB(0, 0, 0);
	for (i=0; i<i32ColCnt; i++)
	{
		stGV.nFormat= dwFormat[0];
		stGV.row = 0;
		stGV.col = i;
		stGV.strText = tzTitle[i];
		m_pGrid->SetItem(&stGV);
		m_pGrid->SetColumnWidth(i, i32Width[i]);
//		m_pGrid->SetItemFont(0, i, &g_lf);
	}

	/* 본문 (1 ~ Rows) 높이 / 주소 설정 (루프 동작 속도 때문에 불가피하게 이중 for 문 사용 안함) */
	stGV.crBkClr = RGB(255, 255, 255);
	for (i=1; i<=i32RowCnt; i++)
	{
		m_pGrid->SetRowHeight(i, 25);

		stGV.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		stGV.strText.Format(L"%05d", uvEng_Conf_GetPLC()->start_d_addr+(i-1));
		stGV.row	= i;
		stGV.col	= 0;
		m_pGrid->SetItem(&stGV);
//		m_pGrid->SetItemFont(i, 0, &g_lf);

		stGV.strText= L"";
		stGV.row	= i;
		stGV.col	= 1;
		m_pGrid->SetItem(&stGV);
//		m_pGrid->SetItemFont(i, 1, &g_lf);

		stGV.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		stGV.strText= L"";
		stGV.row	= i;
		stGV.col	= 2;
		m_pGrid->SetItem(&stGV);
//		m_pGrid->SetItemFont(i, 2, &g_lf);
	}

	m_pGrid->SetBaseGridProp(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0);
	m_pGrid->UpdateGrid();

	return TRUE;
}

/*
 desc : 현재 윈도 컨트롤 영역 갱신
 parm : None
 retn : None
*/
VOID CDlgPlcQ::UpdateCtrl()
{
	/* PLC Memory Data 갱신 */
	UpdatePLCValue();
}

/*
 desc : PLC Memory Data (Value) 갱신
 parm : None
 retn : None
*/
VOID CDlgPlcQ::UpdatePLCValue()
{
	TCHAR tzVal[64]	= {NULL}, tzBin[2][4] = { L"0 ", L"1 " };
	UINT16 u16Val	= {NULL};
	UINT32 u32Begin, u32End, u32Val = 0;
	PUINT16 pPlcMem	= (PUINT16)uvEng_ShMem_GetPLC()->data;	/* PLC Shared Memory */

	/* 현재 스크롤 바의 위치 얻기 */
	if (!m_pGrid->GetViewRowIndex(u32Begin, u32End))	return;

	/* 화면 갱신 중지 */
	m_pGrid->SetRedraw(FALSE);

	/* 화면에 값 갱신 */
	for (; u32Begin<=u32End; u32Begin++)
	{
		/* 반드시 초기화 */
		wmemset(tzVal, 0x00, 64);

		/* 현재 위치의 값 임시 복사 */
		u16Val	= pPlcMem[u32Begin-1];

		/* 문자열 버퍼에 추가 */
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >> 15 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >> 14 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >> 13 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >> 12 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), L" ");
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >> 11 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >> 10 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >>  9 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >>  8 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), L" ");
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >>  7 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >>  6 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >>  5 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >>  4 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), L" ");
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >>  3 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >>  2 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val >>  1 & 0x0001)]);
		wcscat_s(tzVal, _countof(tzVal), tzBin[(u16Val & 0x0001)]);
		/* 마지막 스페이지 영역을 NULL 값으로 설정 */
		tzVal[34]	= 0x00;
		/* 문자열에 값 출력 */
		m_pGrid->SetItemText(u32Begin, 1, tzVal);
		/* 16 진수 값 출력 */
		if ((u32Begin % 2) == 1)
		{
			memcpy(&u32Val, &pPlcMem[u32Begin-1], 4);
			swprintf_s(tzVal, 64, L"%5u : %10u ", u16Val, u32Val);
			m_pGrid->SetItemText(u32Begin, 2, tzVal);
		}
		else
		{
			swprintf_s(tzVal, 64, L"%5u :            ", u16Val);
			m_pGrid->SetItemText(u32Begin, 2, tzVal);
		}
	}

	/* 화면 갱신 활성화 */
	m_pGrid->SetRedraw(TRUE);
	m_pGrid->Invalidate(FALSE);
}

/*
 desc : GridCtrl 즉, PLC Memory 데이터가 출력된 영역 중 임의 행 (Row)을 더블 클릭한 경우
 parm : None
 retn : None
*/
VOID CDlgPlcQ::OnGridClicked()
{
	UINT16 u16Addr;
	CDlgPSet dlgPSet;

	/* 현재 선택된 행과 열의 번호 얻기 */
	PPOINT ptSelPos	= m_pGrid->GetSelPos();
	if (!ptSelPos)	return;

	/* 메모리 주소 값 얻기 */
	u16Addr	= (UINT16)m_pGrid->GetItemTextToInt(ptSelPos->x, 0);
	/* 값 변경 대화 상자 출력 */
	dlgPSet.MyDoModal(u16Addr);
}