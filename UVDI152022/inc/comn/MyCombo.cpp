
#include "pch.h"
#include "MyCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : 생성자
 parm : None
 retn : None
*/
CMyCombo::CMyCombo()
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CMyCombo::~CMyCombo()
{
	if (m_ftCombo.GetSafeHandle())	m_ftCombo.DeleteObject();
}


BEGIN_MESSAGE_MAP(CMyCombo, CComboBox)
//	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


VOID CMyCombo::PreSubclassWindow()
{

	CComboBox::PreSubclassWindow();
}

/*
 desc : 콤보 컨트롤 기본 설정
 parm : lf		- LOGFONT 포인터 객체
 retn : None
*/
VOID CMyCombo::SetLogFont(LOGFONT *lf)
{
	if (m_ftCombo.GetSafeHandle())			m_ftCombo.DeleteObject();
	if (m_ftCombo.CreateFontIndirect(lf))	SetFont(&m_ftCombo);

	Invalidate(TRUE);
}

/*
 desc : 콤보 상자 내 현재 위치의 텍스트를 숫자 값으로 반환
 parm : None
 retn : 숫자 값 반환, 실패할 경우 INT64 최대 값
*/
INT64 CMyCombo::GetLBTextToNum()
{
	TCHAR tzText[1024]	= {NULL};
	if (GetCurSel() == LB_ERR)	return INT32_MAX;
	GetLBText(GetCurSel(), tzText);
	return _wtoi(tzText);
}

/*
 desc : 숫자 값을 등록
 parm : num	- [in]  등록 대상의 숫자
 parm : None
*/
VOID CMyCombo::AddNumber(INT64 num)
{
	TCHAR tzNum[64]	= {NULL};
	swprintf_s(tzNum, 64, L"%I64d", num);
	AddString(tzNum);
}
VOID CMyCombo::AddNumber(UINT64 num)
{
	TCHAR tzNum[64]	= {NULL};
	swprintf_s(tzNum, 64, L"%I64u", num);
	AddString(tzNum);
}