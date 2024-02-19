#pragma once
#include "../DlgSubMenu.h"
#include "../DlgSubTab.h"
#include "..\..\param\SettingManager.h"


#define DEF_UI_GRD_ROW_OFFSET	40
#define DEF_TEXT_MAX_LENGTH		40

enum EnConfSettingOptionPic
{
	eCALB_ACCURACY_PIC_MENU,
	eCALB_ACCURACY_PIC_MAX,
};

enum EnConfSettingOptionGrid
{
	eCONF_SETTING_OPTION_GRID,
	eCONF_SETTING_OPTION_GRID2,
	eCONF_SETTING_OPTION_GRID_MAX,
};

enum EnConfSettingOptionBtn
{
	eCONF_SETTING_OPTION_BTN_PREV,
	eCONF_SETTING_OPTION_BTN_NEXT,
	eCONF_SETTING_OPTION_BTN_SAVE,
	eCONF_SETTING_OPTION_BTN_MAX,
};

typedef struct ST_GRD_PARAM
{
	ST_GRD_PARAM()
	{
		bIsGroup = FALSE;
		strValue = _T("");
	}
	BOOL bIsGroup;
	int nIndexParam;
	CString strValue; 
}ST_GRD_PARAM;

typedef CArray <ST_GRD_PARAM*, ST_GRD_PARAM*&> CArrGrdParam;
typedef CArray <CArrGrdParam*, CArrGrdParam*&> CArrGrdPage;


// CDlgConfSetting 대화 상자

class CDlgConfSetting : public CDlgSubMenu
{
public:

	/* 생성자 / 파괴자 */
	CDlgConfSetting(UINT32 id, CWnd* parent = NULL);
	virtual ~CDlgConfSetting();


	/* 가상함수 선언 */
protected:

	virtual BOOL		PreTranslateMessage(MSG* msg);
	virtual VOID		DoDataExchange(CDataExchange* dx);
	virtual VOID		UpdateControl(UINT64 tick, BOOL is_busy);


public:

	virtual BOOL		OnInitDlg();
	virtual VOID		OnExitDlg();
	virtual VOID		OnPaintDlg(CDC* dc);
	virtual VOID		OnResizeDlg();
	virtual void		RegisterUILevel() {}
	virtual VOID		UpdatePeriod(UINT64 tick, BOOL is_busy);

	int					m_nGrdSizeY;


	/* 로컬 변수 */
protected:

	CMacButton			m_btn_ctl[eCONF_SETTING_OPTION_BTN_MAX];
	CDlgSubTab*			m_pDlgMenu;

	/* 로컬 함수 */
protected:

	VOID				InitCtrl();

	BOOL				InitObject();
	VOID				CloseObject();

	BOOL				CreateMenu(UINT32 id);
	VOID				DeleteMenu();

	BOOL				PopupKBDN(ENM_DITM enType, CString strInput, CString& strOutput, double dMin, double dMax, UINT8 u8DecPts = 0);

	VOID				LoadConfig();
	VOID				SaveConfig();

	BOOL				SelectPath(CString &strPath);

	VOID				LoadCommon(LPG_CIEA conf);
	VOID				SaveCommon(LPG_CIEA conf);
	VOID				LoadAlignCamera(LPG_CIEA conf);
	VOID				SaveAlignCamera(LPG_CIEA conf);
	VOID				LoadTempRange(LPG_CIEA conf);
	VOID				SaveTempRange(LPG_CIEA conf);
	VOID				LoadEtc(LPG_CIEA conf);
	VOID				SaveEtc(LPG_CIEA conf);
	VOID				LoadLuria(LPG_CIEA conf);
	VOID				SaveLuria(LPG_CIEA conf);
	//VOID				SetExpoStartXY();
	VOID				LoadTrigGrab(LPG_CIEA conf);
	VOID				SaveTrigGrab(LPG_CIEA conf);
	VOID				LoadTransformation(LPG_CIEA conf);
	VOID				SaveTransformation(LPG_CIEA conf);
	VOID				SetTempHotAir();
	VOID				SetAnalogGain();
	VOID				LoadSetupAlign(LPG_CIEA conf);
	VOID				SaveSetupAlign(LPG_CIEA conf);
	VOID				LoadMarkFind(LPG_CIEA conf);
	VOID				SeveMarkFind(LPG_CIEA conf);
	VOID				LoadStrobeLamp(LPG_CIEA conf);
	VOID				SaveStrobeLamp(LPG_CIEA conf);

	/* 공용 함수 */
public:
	void				UpdateGridParam();
	void				SaveSettingParam();
	void				SaveSettingParamMatch();

private:
	CGridCtrl	m_pGrd[eCONF_SETTING_OPTION_GRID_MAX];
	int			m_nMaxPage;
	int			m_nSelectPage;
	CArrGrdPage	m_arrGrdPage;


	/* 메시지 맵 */
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID		OnBtnClick(UINT32 id);
	afx_msg VOID OnGrdSettingBasicParamClick(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg VOID OnGrdSettingBasicParamClick2(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg VOID OnGrdSettingBasicParamChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg VOID OnGrdSettingBasicParamChanged2(NMHDR* pNotifyStruct, LRESULT* pResult);
};
