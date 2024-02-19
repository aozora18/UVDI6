#pragma once


// CDlgData ��ȭ �����Դϴ�.

class CDlgData : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgData)

public:
	CDlgData(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgData();
	enum { IDD = IDD_DATA };


/* ���� �Լ�  */
protected:

	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual BOOL		OnInitDialog();


/* ���� ���� */
protected:

	LONG				m_lHostID;
	BOOL				m_bExitProc;
	E30_GPVT			m_enValType;	/* Variable Value Type */
	UINT32				m_u32VID;		/* Variable Index */
	UINT32				m_u32Size;		/* Value Size */
	TCHAR				m_tzName[CIM_CMD_NAME_SIZE];	/* Variable Name */
	TCHAR				m_tzValue[CIM_VAL_ASCII_SIZE];

/* ���� �Լ� */
protected:

	VOID				ExitProc();
	VOID				UpdateData();

/* ���� �Լ� */
public:

	INT_PTR				MyDoModal(LONG host_id,
								  E30_GPVT type, UINT32 id, PTCHAR name, PUINT8 value, UINT32 size);

/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnClose();
	afx_msg VOID		OnDestroy();
public:
	afx_msg void OnBnClickedOk();
};
