// DlgData.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "MainApp.h"
#include "DlgData.h"


// CDlgData 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgData, CDialogEx)

CDlgData::CDlgData(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgData::IDD, pParent)
{
	m_bExitProc	= FALSE;
}

CDlgData::~CDlgData()
{
}

void CDlgData::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgData, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CDlgData::OnBnClickedOk)
END_MESSAGE_MAP()

/*
 desc : 다이얼로그 생성 후 초기화
*/
BOOL CDlgData::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	UpdateData();

	return TRUE;
}

/*
 desc : 다이얼로그가 종료될 때 마지막 한번 호출
 parm : None
 retn : None
*/
VOID CDlgData::OnClose()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnClose();
}
VOID CDlgData::OnDestroy()
{
	if (!m_bExitProc)	ExitProc();
	CDialogEx::OnDestroy();
}
VOID CDlgData::ExitProc()
{
	/* 이미 이전에 호출된적이 있음 */
	m_bExitProc	= TRUE;
}


// CDlgData 메시지 처리기입니다.

INT_PTR CDlgData::MyDoModal(LONG host_id,
							E30_GPVT type, UINT32 id, PTCHAR name, PUINT8 value, UINT32 size)
{
	m_lHostID	= host_id;
	m_enValType	= type;
	m_u32VID	= id;
	wmemset(m_tzName, 0x00, CIM_CMD_NAME_SIZE);
	wcscpy_s(m_tzName, CIM_CMD_NAME_SIZE, name);
	if (value && size)
	{
		wmemset(m_tzValue, 0x00, CIM_VAL_ASCII_SIZE);
		memcpy_s(m_tzValue, CIM_VAL_ASCII_SIZE, value, size);
	}
	return DoModal();
}

VOID CDlgData::UpdateData()
{
	TCHAR tzVID[32]	= {NULL};
	swprintf_s(tzVID, 32, L"%I32u", m_u32VID);
	((CEdit*)GetDlgItem(IDC_DATA_EDIT_VID))->SetWindowText(tzVID);
	((CEdit*)GetDlgItem(IDC_DATA_EDIT_NAME))->SetWindowText(m_tzName);
	((CEdit*)GetDlgItem(IDC_DATA_EDIT_VALUE))->SetWindowText(m_tzValue);
}

void CDlgData::OnBnClickedOk()
{
	TCHAR tzValue[3][256]	= {NULL};
	UNG_CVVT enValType;

	((CEdit*)GetDlgItem(IDC_DATA_EDIT_VID))->GetWindowText(tzValue[0], 256);
	((CEdit*)GetDlgItem(IDC_DATA_EDIT_NAME))->GetWindowText(tzValue[1], 256);
	((CEdit*)GetDlgItem(IDC_DATA_EDIT_VALUE))->GetWindowText(tzValue[2], 256);

	enValType.SetText(m_enValType, tzValue[2]);

	uvCIMLib_SetCacheValueName(m_lHostID, m_enValType, tzValue[1], enValType);

	CDialogEx::OnOK();
}
