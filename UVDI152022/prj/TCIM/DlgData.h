#pragma once


// CDlgData 대화 상자입니다.

class CDlgData : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgData)

public:
	CDlgData(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgData();
	enum { IDD = IDD_DATA };


/* 가상 함수  */
protected:

	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual BOOL		OnInitDialog();


/* 로컬 변수 */
protected:

	LONG				m_lHostID;
	BOOL				m_bExitProc;
	E30_GPVT			m_enValType;	/* Variable Value Type */
	UINT32				m_u32VID;		/* Variable Index */
	UINT32				m_u32Size;		/* Value Size */
	TCHAR				m_tzName[CIM_CMD_NAME_SIZE];	/* Variable Name */
	TCHAR				m_tzValue[CIM_VAL_ASCII_SIZE];

/* 로컬 함수 */
protected:

	VOID				ExitProc();
	VOID				UpdateData();

/* 공용 함수 */
public:

	INT_PTR				MyDoModal(LONG host_id,
								  E30_GPVT type, UINT32 id, PTCHAR name, PUINT8 value, UINT32 size);

/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnClose();
	afx_msg VOID		OnDestroy();
public:
	afx_msg void OnBnClickedOk();
};
