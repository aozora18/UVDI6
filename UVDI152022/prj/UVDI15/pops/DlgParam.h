#pragma once
#include "../../../inc/comn/MyDialog.h"
#include "../../../inc/grid/GridCtrl.h"
#include "../../../inc/kybd/DlgKBDN.h"

using namespace std;

typedef struct stInputData
{
	CString strName;
	CString strValue;
	CString strUnit;
	ENM_DITM enFormat;
	double dMin;
	double dMax;
	UINT8 u8DecPts;

	stInputData()
	{
		Init();
	}

	void Init()
	{
		strName = _T("");
		strValue = _T("");
		strUnit = _T("");
		enFormat = ENM_DITM::en_none;
		dMin = -9999.;
		dMax = 9999.;
		u8DecPts = 0;
	}

} DLG_PARAM;

typedef vector<DLG_PARAM> VCT_DLG_PARAM;

class CDlgParam : public CMyDialog
{
	DECLARE_DYNAMIC(CDlgParam)

public:
	CDlgParam(CWnd* pParent = NULL);
	virtual ~CDlgParam();

	enum { IDD = IDD_PARAM };

	INT_PTR				MyDoModal(VCT_DLG_PARAM &stVctParam);

/* ���� �Լ� */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual BOOL		OnInitDlg();
	virtual VOID		DoDataExchange(CDataExchange* pDX);
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC* dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel() {}

private:

	enum enGrid
	{
		eGRD_COL_NAME,
		eGRD_COL_VALUE,
		eGRD_COL_UNIT,
		eGRD_COL_MAX
	};

	enum enDefines
	{
		DEF_DEFAULT_GRID_ROW_SIZE = 35,
		DEF_STR_MAX_LENGTH = 256,
	};

/* ���� ���� */
protected:
	VCT_DLG_PARAM*		m_pstVctParam;
	VCT_DLG_PARAM		m_stVctBackUp;

	CMacButton			m_btnConfirm;
	CGridCtrl			m_grdParam;

/* ���� �Լ� */
protected:

	VOID				InitBtn();
	VOID				InitGrid();
	VOID				AttachButton();
	VOID				ResizeUI();

	BOOL				PopupKBDN(ENM_DITM enType, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts = 0);

/* �޽��� �� */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedParamBtnConfirm();
	afx_msg void OnClickGridParam(NMHDR* pNotifyStruct, LRESULT* pResult);
};
