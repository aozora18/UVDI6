
#pragma once

#if (CUSTOM_CODE_UVDI15_LLS10 == DELIVERY_PRODUCT_ID || \
 CUSTOM_CODE_UVDI15_LLS06 == DELIVERY_PRODUCT_ID|| \
 CUSTOM_CODE_UVDI15 == DELIVERY_PRODUCT_ID|| \
CUSTOM_CODE_HDDI6== DELIVERY_PRODUCT_ID)
#include "../../prj/uvdi15/resource.h"
#endif

#include "../comn/MyDialog.h"
#include "../comn/MacButton.h"
#include "../comn/MyEditVert.h"

class CDlgKBDT : public CMyDialog
{
public:
	CDlgKBDT(CWnd* parent = NULL);
	enum { IDD = IDD_KBDT };
	BOOL				Create(CWnd *parent, TCHAR *value);
	INT_PTR				MyDoModal(UINT32 max_str=1023, BOOL bPassword = FALSE);
	INT_PTR				MyDoModal(TCHAR *value, UINT32 max_str=1023);

// 가상함수 재정의
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel() {}

	virtual VOID		Apply();
	virtual VOID		Cancel();

public:

	virtual VOID		UpdatePeriod()	{};


// 로컬 변수
protected:

	BOOL				m_bShift;			// 대문자 : 0x00, 소문자 0x01
	TCHAR				m_tzText[1024];

	BOOL				m_bPassword;		/* 입력 창 암호화 적용 유무 */
	UINT32				m_u32MaxStr;		/* 최대 입력할 수 있는 글자 수 */

	CMacButton			m_btn_ctl[46];
	CMyEditVert			m_edt_ctl[1];

// 로컬 함수
protected:

	BOOL				InitCtrl();

	VOID				AddCh(TCHAR ch);
	VOID				DelCh();
	VOID				Clear();
	VOID				SetSign(UINT8 ch=0x00);
	VOID				SelectAll();

	VOID				OnKeyUp(UINT32 key_num, UINT32 rep_cnt, UINT32 flags);

	VOID				UpdateUpDnChar();

	VOID				EnableEnterKey();


// 공용 함수
public:

	VOID				GetText(TCHAR *text, UINT32 size);
	TCHAR				*GetText()	{	return m_tzText;	};

	// Button Enable or Disable
	VOID				DotEnable(BOOL enable);
	VOID				SignEnable(BOOL enable);

// 메시지 맵
protected:
	DECLARE_MESSAGE_MAP()
#if (CUSTOM_CODE_UTRONIX  != DELIVERY_PRODUCT_ID)
	afx_msg VOID		OnBtnClicked(UINT32 id);
#endif
};
