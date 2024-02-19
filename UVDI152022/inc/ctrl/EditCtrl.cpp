
/*
 desc : Input Values
*/

#include "pch.h"
#include "EditCtrl.h"

#if USE_KEYBOARD_TYPE_TOUCH
	#include "../kybd/DlgKBDN.h"
	#include "../kybd/DlgKBDT.h"
#endif


#ifdef	_DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]	= __FILE__;
#endif

/*
 desc : 생성자
 parm : None
 retn : None
*/
CEditCtrl::CEditCtrl()
{
}

/*
 desc : 파괴자
 parm : None
 retn : None
*/
CEditCtrl::~CEditCtrl()
{
}

BEGIN_MESSAGE_MAP(CEditCtrl, CMyEdit)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/*
 desc : 컨트롤이 수행될 때, 초기 호출됨
 parm : None
 retn : None
*/
VOID CEditCtrl::PreSubclassWindow()
{
	CMyEdit::PreSubclassWindow();
}

/*
 desc : 마우스 왼쪽 버튼 클릭 Down
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CEditCtrl::OnLButtonDown(UINT32 flags, CPoint point)
{
	/* 왼쪽 마우스 버튼 클릭 다운 가상 함수 호출 */
	if (m_bIsReadOnly)	LButtonDn();
	else CMyEdit::OnLButtonDown(flags, point);
}

/*
 desc : 윈도 EnableWindow 함수 호출한 것과 비슷하게 처리
		숫자 등 키보드 다이얼로그 출력 못하게 처리
 parm : enable	- [in]  Enable or Disable
 retn : None
*/
VOID CEditCtrl::SetReadOnly(BOOL enable)
{
	CMyEdit::SetReadOnly(enable);

	if (!enable)	SetTextColor(RGB(0, 0, 0));
	else			SetTextColor(RGB(128, 128, 128));
	Invalidate(TRUE);
}

/* --------------------------------------------------------------------------------------------- */
/*                                   Number Input Dialog Box                                     */
/* --------------------------------------------------------------------------------------------- */

#if USE_KEYBOARD_TYPE_TOUCH
/*
 desc : 숫자 입력 화면 출력
 parm : output	- [in]  Edit Box에 값 출력 여부
 retn : TRUE or FALSE
*/
BOOL CEditNum::PopupKBDN(BOOL output)
{
	TCHAR tzTitle[1024]	= {NULL}, tzPoint[32] = {NULL}, tzMinMax[2][32]	= {NULL};
	CDlgKBDN dlg;

	wcscpy_s(tzMinMax[0], 32, L"Min");
	wcscpy_s(tzMinMax[1], 32, L"Max");

	switch (m_enType)
	{
	case ENM_DITM::en_int8	:
	case ENM_DITM::en_int16	:
	case ENM_DITM::en_int32	:
	case ENM_DITM::en_int64	:
	case ENM_DITM::en_uint8	:
	case ENM_DITM::en_uint16	:
	case ENM_DITM::en_uint32	:
	case ENM_DITM::en_uint64	:
		swprintf_s(tzTitle, 1024, L"%s:%+I64d,%s:%+I64d", tzMinMax[0], m_i64Min, tzMinMax[1], m_i64Max);
		if (IDOK != dlg.MyDoModal(tzTitle, FALSE, BOOL(m_enSign), DOUBLE(m_i64Min), DOUBLE(m_i64Max)))	return FALSE;	break;
	case ENM_DITM::en_float	:
	case ENM_DITM::en_double	:
		switch (m_u8Point)
		{
		case 0x01	:	swprintf_s(tzTitle, 1024, L"%s:%+.1f, %s:%+.1f", tzMinMax[0], m_dbMin, tzMinMax[1], m_dbMax);	break;
		case 0x02	:	swprintf_s(tzTitle, 1024, L"%s:%+.2f, %s:%+.2f", tzMinMax[0], m_dbMin, tzMinMax[1], m_dbMax);	break;
		case 0x03	:	swprintf_s(tzTitle, 1024, L"%s:%+.3f, %s:%+.3f", tzMinMax[0], m_dbMin, tzMinMax[1], m_dbMax);	break;
		case 0x04	:	swprintf_s(tzTitle, 1024, L"%s:%+.4f, %s:%+.4f", tzMinMax[0], m_dbMin, tzMinMax[1], m_dbMax);	break;
		case 0x05	:	swprintf_s(tzTitle, 1024, L"%s:%+.5f, %s:%+.5f", tzMinMax[0], m_dbMin, tzMinMax[1], m_dbMax);	break;
		default		:	swprintf_s(tzTitle, 1024, L"%s:%+.6f, %s:%+.6f", tzMinMax[0], m_dbMin, tzMinMax[1], m_dbMax);	break;
		}		
		if (IDOK != dlg.MyDoModal(tzTitle, TRUE, BOOL(m_enSign), m_dbMin, m_dbMax))	return FALSE;	break;
	}
	{
		m_i32Value	= dlg.GetValueInt32();
		m_u32Value	= dlg.GetValueUint32();
		m_i64Value	= dlg.GetValueInt64();
		m_u64Value	= dlg.GetValueUint64();
		m_dbValue	= dlg.GetValueDouble();

		/* Data Type에 따른 출력 */
		if (output)
		{
			switch (m_enType)
			{
			case ENM_DITM::en_int8	:
			case ENM_DITM::en_int16	:
			case ENM_DITM::en_int32	:
			case ENM_DITM::en_int64	:	SetTextToNum(m_i64Value);	break;
			case ENM_DITM::en_uint8	:
			case ENM_DITM::en_uint16:
			case ENM_DITM::en_uint32:
			case ENM_DITM::en_uint64:	SetTextToNum(m_u64Value);	break;
			case ENM_DITM::en_float	:
			case ENM_DITM::en_double:	SetTextToNum(m_dbValue, m_u8Point);	break;
			default					:	return FALSE;
			}
		}
	}

	return TRUE;
}
#endif

/*
 desc : 마우스 왼쪽 버튼 클릭 Down
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CEditNum::LButtonDn()
{
#if USE_KEYBOARD_TYPE_TOUCH
	if (m_bPopKBDN && !PopupKBDN(TRUE))	return;

	/* 상위 부모에게 버튼 이벤트로 알림 */
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)GetSafeHwnd());
#endif
}

/* --------------------------------------------------------------------------------------------- */
/*                                   String Input Dialog Box                                     */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 마우스 왼쪽 버튼 클릭 Down
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CEditTxt::LButtonDn()
{
#if USE_KEYBOARD_TYPE_TOUCH
	TCHAR tzText[1024]	= {NULL};

	CDlgKBDT dlg;
	if (IDOK == dlg.MyDoModal(m_i32MaxStr))
	{
		dlg.GetText(tzText, 1024);
		SetWindowTextW(tzText);
	}
#endif
}

/* --------------------------------------------------------------------------------------------- */
/*                                      Grabbed Mark Method                                      */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 컨트롤이 수행될 때, 초기 호출됨
 parm : None
 retn : None
*/
VOID CEditGrab::PreSubclassWindow()
{
	SetWindowTextW(L"Match Templ.");
	CEditNum::PreSubclassWindow();
}

/*
 desc : 마우스 왼쪽 버튼 클릭 Down
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CEditGrab::LButtonDn()
{
	if (m_u8GrabMode)
	{
		m_u8GrabMode	= 0x00;
		SetWindowTextW(L"Match Templ.");
	}
	else
	{
		m_u8GrabMode	= 0x01;
		SetWindowTextW(L"Edge Detect");
	}
	Invalidate(TRUE);
}

/* --------------------------------------------------------------------------------------------- */
/*                                         Unit Display                                          */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 마우스 왼쪽 버튼 클릭 Down
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CEditUnit::LButtonDn()
{
#if USE_KEYBOARD_TYPE_TOUCH
	TCHAR tzValue[64]	= {NULL};
	if (PopupKBDN(FALSE))
	{
		swprintf_s(tzValue, 64, L"%.4f %s", m_dbValue, m_tzUnit);
		SetWindowTextW(tzValue);
	}
#endif
}

/* --------------------------------------------------------------------------------------------- */
/*                                   Expose Step Size (Luria)                                    */
/* --------------------------------------------------------------------------------------------- */
/*
 desc : 컨트롤이 수행될 때, 초기 호출됨
 parm : None
 retn : None
*/
VOID CEditStepSize::PreSubclassWindow()
{
	SetWindowTextW(L"Step 1");

	CEditCtrl::PreSubclassWindow();
}

/*
 desc : 마우스 왼쪽 버튼 클릭 Down
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CEditStepSize::LButtonDn()
{
	TCHAR tzStep[4][8]	= { L"Step 1", L"Step 2", L"Step 3", L"Step 4" };

	if (m_u8Step == 0x04)	m_u8Step = 0x01;
	else					m_u8Step++;
	SetWindowTextW(tzStep[m_u8Step-1]);
}

/*
 desc : Step Size 설정
 parm : step	- [in]  1 or 2
 retn : None
*/
VOID CEditStepSize::SetStepSize(UINT8 step)
{
	TCHAR tzStep[4][8]	= { L"Step 1", L"Step 2", L"Step 3", L"Step 4" };
	SetWindowTextW(tzStep[step-1]);
	m_u8Step	= step;
}

/* --------------------------------------------------------------------------------------------- */
/*                                   Vision Mark Reverse Setup                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 컨트롤이 수행될 때, 초기 호출됨
 parm : None
 retn : None
*/
VOID CEditMarkReverse::PreSubclassWindow()
{
	SetWindowTextW(L"Normal");

	CEditCtrl::PreSubclassWindow();
}

/*
 desc : 마우스 왼쪽 버튼 클릭 Down
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CEditMarkReverse::LButtonDn()
{
	switch (m_u8Type)
	{
	case 0x00	:	SetWindowTextW(L"Reverse");		break;
	case 0x01	:	SetWindowTextW(L"Normal");		break;
	}
	if (++m_u8Type == 2)	m_u8Type = 0x00;
}

/* --------------------------------------------------------------------------------------------- */
/*                                   Match Option - Speed Type                                   */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 컨트롤이 수행될 때, 초기 호출됨
 parm : None
 retn : None
*/
VOID CEditSpeedType::PreSubclassWindow()
{
	SetWindowTextW(L"Very Low");

	CEditCtrl::PreSubclassWindow();
}

/*
 desc : 마우스 왼쪽 버튼 클릭 Down
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CEditSpeedType::LButtonDn()
{
	switch (m_u8Type)
	{
	case 0x00	:	SetWindowTextW(L"Low");			break;
	case 0x01	:	SetWindowTextW(L"Medium");		break;
	case 0x02	:	SetWindowTextW(L"High");		break;
	case 0x03	:	SetWindowTextW(L"Very High");	break;
	case 0x04	:	SetWindowTextW(L"Very Low");	break;
	}
	if (++m_u8Type == 5)	m_u8Type = 0x00;
}

/*
 desc : Type 설정
 parm : type - [in]  0x00 : Very Low, 0x01 : Low, 0x02 : Medium, 0x03 : High, 0x04 : Very High
 retn : None
*/
VOID CEditSpeedType::SetType(UINT8 type)
{
	switch (type)
	{
	case 0x00	:	SetWindowTextW(L"Very Low");	break;
	case 0x01	:	SetWindowTextW(L"Low");			break;
	case 0x02	:	SetWindowTextW(L"Medium");		break;
	case 0x03	:	SetWindowTextW(L"High");		break;
	case 0x04	:	SetWindowTextW(L"Very High");	break;
	default		:	SetWindowTextW(L"None");		return;
	}
	m_u8Type	= type;
}

/* --------------------------------------------------------------------------------------------- */
/*                                 Match Option - Accuracy Type                                  */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 컨트롤이 수행될 때, 초기 호출됨
 parm : None
 retn : None
*/
VOID CEditAccuracyType::PreSubclassWindow()
{
	SetWindowTextW(L"Low");

	CEditCtrl::PreSubclassWindow();
}

/*
 desc : 마우스 왼쪽 버튼 클릭 Down
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CEditAccuracyType::LButtonDn()
{
	switch (m_u8Type)
	{
	case 0x01	:	SetWindowTextW(L"Medium");	break;
	case 0x02	:	SetWindowTextW(L"High");	break;
	case 0x03	:	SetWindowTextW(L"Low");		break;
	}
	if (++m_u8Type == 4)	m_u8Type = 0x01;
}

/*
 desc : Type 설정
 parm : type - [in]  0x01 : Low, 0x02 : Medium, 0x03 : High
 retn : None
*/
VOID CEditAccuracyType::SetType(UINT8 type)
{
	switch (type)
	{
	case 0x01	:	SetWindowTextW(L"Low");		break;
	case 0x02	:	SetWindowTextW(L"Medium");	break;
	case 0x03	:	SetWindowTextW(L"High");	break;
	default		:	SetWindowTextW(L"None");	return;
	}
	m_u8Type	= type;
}

/* --------------------------------------------------------------------------------------------- */
/*                               Edge Detection - Threshold Type                                 */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 컨트롤이 수행될 때, 초기 호출됨
 parm : None
 retn : None
*/
VOID CEditThreshold::PreSubclassWindow()
{
	SetWindowTextW(L"Very High");

	CEditCtrl::PreSubclassWindow();
}

/*
 desc : 마우스 왼쪽 버튼 클릭 Down
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CEditThreshold::LButtonDn()
{
	switch (m_u8Type)
	{
	case 0x01	:	SetWindowTextW(L"High");		break;
	case 0x02	:	SetWindowTextW(L"Medium");		break;
	case 0x03	:	SetWindowTextW(L"Low");			break;
	case 0x04	:	SetWindowTextW(L"Very High");	break;
	}
	if (++m_u8Type == 5)	m_u8Type = 0x01;
}

/*
 desc : Type 설정
 parm : type - [in]  0x01 : Low, 0x02 : Medium, 0x03 : High
 retn : None
*/
VOID CEditThreshold::SetType(UINT8 type)
{
	switch (type)
	{
	case 0x01	:	SetWindowTextW(L"Very High");	break;
	case 0x02	:	SetWindowTextW(L"High");		break;
	case 0x03	:	SetWindowTextW(L"Medium");		break;
	case 0x04	:	SetWindowTextW(L"Low");			break;
	default		:	SetWindowTextW(L"None");		return;
	}
	m_u8Type	= type;
}

/* --------------------------------------------------------------------------------------------- */
/*                                    Mark Model Definition                                      */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 컨트롤이 수행될 때, 초기 호출됨
 parm : None
 retn : None
*/
VOID CEditMarkType::PreSubclassWindow()
{
	SetWindowTextW(L"Circle");

	CEditCtrl::PreSubclassWindow();
}

/*
 desc : 마우스 왼쪽 버튼 클릭 Down
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CEditMarkType::LButtonDn()
{
	switch (m_enType)
	{
	case ENG_MMDT::en_circle	:	SetWindowTextW(L"Ellipse");		m_enType = ENG_MMDT::en_ellipse;	break;
	case ENG_MMDT::en_ellipse	:	SetWindowTextW(L"Square");		m_enType = ENG_MMDT::en_square;		break;
	case ENG_MMDT::en_square	:	SetWindowTextW(L"Rectangle");	m_enType = ENG_MMDT::en_rectangle;	break;
	case ENG_MMDT::en_rectangle	:	SetWindowTextW(L"Ring");		m_enType = ENG_MMDT::en_ring;		break;
	case ENG_MMDT::en_ring		:	SetWindowTextW(L"Cross");		m_enType = ENG_MMDT::en_cross;		break;
	case ENG_MMDT::en_cross		:	SetWindowTextW(L"Diamond");		m_enType = ENG_MMDT::en_diamond;	break;
	case ENG_MMDT::en_diamond	:	SetWindowTextW(L"Triangle");	m_enType = ENG_MMDT::en_triangle;	break;
	case ENG_MMDT::en_triangle	:	SetWindowTextW(L"Circle");		m_enType = ENG_MMDT::en_circle;		break;
	}

	/* 상위 부모에게 버튼 이벤트로 알림 */
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)GetSafeHwnd());
}

/*
 desc : Type 설정
 parm : type - [in]  Model Type
 retn : None
*/
VOID CEditMarkType::SetType(ENG_MMDT type)
{
	switch (type)
	{
	case ENG_MMDT::en_circle	:	SetWindowTextW(L"Circle");	   break;
	case ENG_MMDT::en_ellipse	:	SetWindowTextW(L"Ellipse");	   break;
	case ENG_MMDT::en_square	:	SetWindowTextW(L"Square");	   break;
	case ENG_MMDT::en_rectangle	:	SetWindowTextW(L"Rectangle");  break;
	case ENG_MMDT::en_ring		:	SetWindowTextW(L"Ring");	   break;
	case ENG_MMDT::en_cross		:	SetWindowTextW(L"Cross");	   break;
	case ENG_MMDT::en_diamond	:	SetWindowTextW(L"Diamond");	   break;
	case ENG_MMDT::en_triangle	:	SetWindowTextW(L"Triangle");   break;
	}
	m_enType	= type;
}

/* --------------------------------------------------------------------------------------------- */
/*                                        Mark Model Color                                       */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 컨트롤이 수행될 때, 초기 호출됨
 parm : None
 retn : None
*/
VOID CEditMarkColor::PreSubclassWindow()
{
	SetWindowTextW(L"Black");

	CEditCtrl::PreSubclassWindow();
}

/*
 desc : 마우스 왼쪽 버튼 클릭 Down
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CEditMarkColor::LButtonDn()
{
	switch (m_u32Color)
	{
	case 256	:	SetWindowTextW(L"White");	m_u32Color	= 0x00000080;	break;
	case 128	:	SetWindowTextW(L"Any");		m_u32Color	= 0x11000000;	break;
	default		:	SetWindowTextW(L"Black");	m_u32Color	= 0x00000100;	break;
	}
}

/*
 desc : 색상 설정
 parm : color	- [in]  Color Value
 retn : None
*/
VOID CEditMarkColor::SetColor(UINT32 color)
{
	switch (color)
	{
	case 256	:	SetWindowTextW(L"Black");	break;
	case 128	:	SetWindowTextW(L"White");	break;
	default		:	SetWindowTextW(L"Any");		break;
	}
	m_u32Color	= color;
}

/* --------------------------------------------------------------------------------------------- */
/*                                Auto increment of numbers                                      */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 마우스 왼쪽 버튼 클릭 Down
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CEditAutoNum::LButtonDn()
{
	/* 현재 값 읽어오고 증가 시키기 */
	switch (m_u8Type)
	{
	case 0x00	:	m_i32Value	= (INT32)GetTextToNum()	+ m_i32Period;
					if (INT32_MIN < m_i32Value)	m_i32Value	= INT32_MIN;	break;
	case 0x01	:	m_u32Value	= (UINT32)GetTextToNum() + m_u32Period;
					if (INT32_MAX < m_u32Value)	m_u32Value	= INT32_MAX;	break;
	case 0x02	:	m_dbValue	= (DOUBLE)GetTextToDouble()	+ m_dbPeriod;
					if (m_dbMax < m_dbValue)	m_dbValue	= m_dbMin;		break;
	}
	/* 증감 값 출력 */
	switch (m_u8Type)
	{
	case 0x00	:	SetTextToNum(m_i32Value);			break;
	case 0x01	:	SetTextToNum(m_u32Value);			break;
	case 0x02	:	SetTextToNum(m_dbValue, m_u8Point);	break;
	}

	/* 상위 부모에게 버튼 이벤트로 알림 */
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)GetSafeHwnd());
}

/* --------------------------------------------------------------------------------------------- */
/*                                  Output String : Yes or No                                    */
/* --------------------------------------------------------------------------------------------- */

/*
 desc : 컨트롤이 수행될 때, 초기 호출됨
 parm : None
 retn : None
*/
VOID CEditYesNo::PreSubclassWindow()
{
	SetWindowTextW(L"No");

	CEditCtrl::PreSubclassWindow();
}

/*
 desc : 마우스 왼쪽 버튼 클릭 Down
 parm : flags	- [in]  
		point	- [in]  
 retn : None
*/
VOID CEditYesNo::LButtonDn()
{
	m_u8YesNo = !m_u8YesNo;
	switch (m_u8YesNo)
	{
	case 0x00	:	SetWindowTextW(L"No");		break;
	case 0x01	:	SetWindowTextW(L"Yes");		break;
	}
}

/*
 desc : 값 설정
 parm : yesno	- [in]  0x00 : No, 0x01 : Yes
 retn : None
*/
VOID CEditYesNo::PutYesNo(UINT8 yesno)
{
	m_u8YesNo	= yesno;
	switch (m_u8YesNo)
	{
	case 0x00	:	SetWindowTextW(L"No");		break;
	case 0x01	:	SetWindowTextW(L"Yes");		break;
	}
}