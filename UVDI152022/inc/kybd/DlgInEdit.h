#pragma once

#include "../comn/MyDialog.h"
#include "../comn/MyEdit.h"
#include "../comn/MacButton.h"

class CDlgInEdit : public CMyDialog
{
public:
	CDlgInEdit(CWnd* parent = NULL);
	enum { IDD = IDD_IN_EDIT };

// �����Լ� ������
protected:

	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC *dc);
	virtual VOID		OnResizeDlg();

// ������, ����ü, ����ü
protected:


// ���� ����
protected:

	DOUBLE				m_dbValue;
	UINT64				m_u64Value;
	INT64				m_i64Value;

	CMacButton			m_btn_ctl[2];
	CMyEdit				m_edt_ctl[1];


// ���� �Լ�
protected:

	BOOL				InitControl();


// ���� �Լ�
public:

	INT32				GetValueInt32()		{	return (INT32)m_i64Value;	}
	INT64				GetValueInt64()		{	return m_i64Value;			}
	UINT32				GetValueUint32()	{	return (UINT32)m_u64Value;	}
	UINT64				GetValueUint64()	{	return m_u64Value;			}
	DOUBLE				GetValueDouble()	{	return m_dbValue;			}


// �޽��� ��
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL		PreTranslateMessage(MSG* msg);
	afx_msg VOID		OnSysCommand(UINT32 id, LPARAM lparam);
	afx_msg VOID		OnBtnApply();
	afx_msg VOID		OnBtnCancel();
};
