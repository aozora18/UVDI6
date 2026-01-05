#pragma once

//#include "../../DlgSubTab.h"
//#include "../../../inc/comn/MyDialog.h"
#include "../../../../../inc/comn/MyDialog.h"
#include "../DlgCalbExposure.h"



// DlgStepOffsetCalc 대화 상자

//CALB_EXPOSURE_STEPOFFSET_BTN_INIT
//CALB_EXPOSURE_STEPOFFSET_BTN_LOAD
//CALB_EXPOSURE_STEPOFFSET_BTN_SAVE

enum EnCalbExposureStepOffsetCaolBtn
{
    eCALB_EXPOSURE_STEPOFFSET_BTN_LOAD,
    eCALB_EXPOSURE_STEPOFFSET_BTN_SAVE,
    eCALB_EXPOSURE_STEPOFFSET_BTN_MAX,
};

enum EnCalbExposureStepOffsetCaolGrd
{
    eCALB_EXPOSURE_STEPOFFSET_GRD_PH_STEP,
    eCALB_EXPOSURE_STEPOFFSET_GRD_MAX,
};


enum EN_GRID_PHSTEP_ROW
{
    ePHSTEP_COL_TITLE = 0,
    ePHSTEP_ROW_NO = 0,
    ePHSTEP_ROW_PH2_X,
    ePHSTEP_ROW_PH2_Y,
    ePHSTEP_ROW_PH3_X,
    ePHSTEP_ROW_PH3_Y,
    ePHSTEP_ROW_PH4_X,
    ePHSTEP_ROW_PH4_Y,
    ePHSTEP_ROW_PH5_X,
    ePHSTEP_ROW_PH5_Y,
    ePHSTEP_ROW_PH6_X,
    ePHSTEP_ROW_PH6_Y,
    ePHSTEP_ROW_MAX,
};

class CDlgStepOffsetCalc : public CMyDialog
{
public:

    /* 생성자 / 파괴자 */
    CDlgStepOffsetCalc(CWnd* parent = NULL);
    enum { IDD = IDD_CALB_EXPOSURE_FLUSH_STEP };

   /* 가상 함수 */
protected:

    virtual BOOL		PreTranslateMessage(MSG* msg);
    virtual BOOL		OnInitDlg();
    virtual VOID		DoDataExchange(CDataExchange* dx);
    virtual VOID		OnExitDlg();
    virtual VOID		OnPaintDlg(CDC* dc);
    virtual VOID		OnResizeDlg();
    virtual void		RegisterUILevel() {}

    /* 로컬 함수 */
protected:

    BOOL				InitCtrl();
    void                InitCalcGrid();

    // 엑셀에서 읽어온 PH별 Step X, Y 데이터 (1~6번 PH)
    double              m_dStepX[7];
    double              m_dStepY[7];

    // 계산 실행 함수
    void CalculateOffsets();
    // 엑셀 파일 파싱 함수 (간이 구현 혹은 라이브러리 연동용)
    BOOL LoadExcelData(CString strPath);
public:
    INT_PTR				MyDoModal();
    virtual VOID		UpdatePeriod() {};

    CMacButton			m_btn_ctl[eCALB_EXPOSURE_STEPOFFSET_BTN_MAX];
    CGridCtrl			m_grd_ctl[eCALB_EXPOSURE_STEPOFFSET_GRD_MAX];

    BOOL				PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts = 0);

	DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedCalbExposureStepoffsetBtnLoad();
    afx_msg void OnBnClickedCalbExposureStepoffsetBtnSave();
    afx_msg	void		OnClickGridOption(NMHDR* pNotifyStruct, LRESULT* pResult);
};
