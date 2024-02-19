
#pragma once

#if (CUSTOM_CODE_UVDI15_LLS10 == DELIVERY_PRODUCT_ID || \
 CUSTOM_CODE_UVDI15_LLS06 == DELIVERY_PRODUCT_ID|| \
 CUSTOM_CODE_UVDI15 == DELIVERY_PRODUCT_ID|| \
CUSTOM_CODE_HDDI6== DELIVERY_PRODUCT_ID)
#include "../../prj/uvdi15/resource.h"
#elif (DELIVERY_PRODUCT_ID == CUSTOM_CODE_GEN2I)
#include "../../../prj/utronix/resource.h"
#endif


#include "../comn/MyDialog.h"
#include "../comn/MacButton.h"
#include "../comn/MyEditVert.h"


class CDlgKBDN : public CMyDialog
{
public:
	CDlgKBDN(CWnd* parent = NULL);
	enum { IDD = IDD_KBDM };
	BOOL				Create(CWnd *parent, BOOL dot_use=TRUE, BOOL sign_use=TRUE,
							   DOUBLE min_val=INT64_MIN, DOUBLE max_val=INT64_MAX);
	INT_PTR				MyDoModal(TCHAR *title, BOOL dot_use, BOOL sign,
								  DOUBLE min_val=FLT_MIN, DOUBLE max_val=FLT_MAX);


// �����Լ� ������
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel();

	virtual VOID		Apply();
	virtual VOID		Cancel();

public:

	virtual VOID		UpdatePeriod()	{};

// ���� ����
protected:

	BOOL				m_bDotUse;
	BOOL				m_bSignUse;
	BOOL				m_bEnableEnterKey;

	TCHAR				m_tzTitle[1024];
	TCHAR				m_tzValue[1024];
	INT16				m_i16Value;
	UINT16				m_u16Value;
	INT32				m_i32Value;
	UINT32				m_u32Value;
	INT64				m_i64Value;
	UINT64				m_u64Value;
	DOUBLE				m_dbValue;

	/* �ִ� ���� �ּ� �� */
	DOUBLE				m_dbMin;
	DOUBLE				m_dbMax;

	CMacButton			m_btn_ctl[17];
	CMyEditVert			m_edt_ctl[1];

// ���� �Լ�
protected:

	BOOL				InitCtrl();

	VOID				AddCh(TCHAR ch);
	VOID				DelCh();
	VOID				Clear();
	VOID				SetSign(UINT8 ch=0x00);
	VOID				SelectAll();

	VOID				OnKeyUp(UINT32 key_num, UINT32 rep_cnt, UINT32 flags);

	VOID				EnableEnterKey();


// ���� �Լ�
public:

	INT16				GetValueInt16()		{	return m_i16Value;	}
	UINT16				GetValueUint16()	{	return m_u16Value;	}
	INT32				GetValueInt32()		{	return m_i32Value;	}
	UINT32				GetValueUint32()	{	return m_u32Value;	}
	INT64				GetValueInt64()		{	return m_i64Value;	}
	UINT64				GetValueUint64()	{	return m_u64Value;	}
	DOUBLE				GetValueDouble()	{	return m_dbValue;	}
	PTCHAR				GetValueText()		{	return m_tzValue;	}

	// Button Enable or Disable
	VOID				DotEnable(BOOL enable);
	VOID				SignEnable(BOOL enable);


// �޽��� ��
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClicked(UINT32 id);
};
