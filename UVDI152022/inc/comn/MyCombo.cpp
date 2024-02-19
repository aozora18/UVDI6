
#include "pch.h"
#include "MyCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
 desc : ������
 parm : None
 retn : None
*/
CMyCombo::CMyCombo()
{
}

/*
 desc : �ı���
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
 desc : �޺� ��Ʈ�� �⺻ ����
 parm : lf		- LOGFONT ������ ��ü
 retn : None
*/
VOID CMyCombo::SetLogFont(LOGFONT *lf)
{
	if (m_ftCombo.GetSafeHandle())			m_ftCombo.DeleteObject();
	if (m_ftCombo.CreateFontIndirect(lf))	SetFont(&m_ftCombo);

	Invalidate(TRUE);
}

/*
 desc : �޺� ���� �� ���� ��ġ�� �ؽ�Ʈ�� ���� ������ ��ȯ
 parm : None
 retn : ���� �� ��ȯ, ������ ��� INT64 �ִ� ��
*/
INT64 CMyCombo::GetLBTextToNum()
{
	TCHAR tzText[1024]	= {NULL};
	if (GetCurSel() == LB_ERR)	return INT32_MAX;
	GetLBText(GetCurSel(), tzText);
	return _wtoi(tzText);
}

/*
 desc : ���� ���� ���
 parm : num	- [in]  ��� ����� ����
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